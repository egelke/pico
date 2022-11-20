#include "lcd.h"
#include "device.h"
#include "interface.h"
#include "pico/stdlib.h"
#include <stdlib.h>

#define JHD1804_I2C_ADDRESS     0x3E

#define JHD1804_MEM_COLUMNS     40
#define JHD1804_MEM_ROWS        2
#define JHD1804_VIEW_COLUMNS    16
#define JHD1804_VIEW_ROWS       2

// commands
#define JHD1804_CLEARDISPLAY    0x01
#define JHD1804_RETURNHOME      0x02
#define JHD1804_ENTRYMODESET    0x04
#define JHD1804_DISPLAYCONTROL  0x08
#define JHD1804_CURSORSHIFT     0x10
#define JHD1804_FUNCTIONSET     0x20
#define JHD1804_SETCGRAMADDR    0x40
#define JHD1804_SETDDRAMADDR    0x80

// flags for display entry mode
#define JHD1804_ENTRYRIGHT      0x00
#define JHD1804_ENTRYLEFT       0x02
#define JHD1804_ENTRYSHIFTINCREMENT 0x01
#define JHD1804_ENTRYSHIFTDECREMENT 0x00

// flags for display on/off control
#define JHD1804_DISPLAYON       0x04
#define JHD1804_DISPLAYOFF      0x00
#define JHD1804_CURSORON        0x02
#define JHD1804_CURSOROFF       0x00
#define JHD1804_BLINKON         0x01
#define JHD1804_BLINKOFF        0x00

// flags for display/cursor shift
#define JHD1804_DISPLAYMOVE     0x08
#define JHD1804_CURSORMOVE      0x00
#define JHD1804_MOVERIGHT       0x04
#define JHD1804_MOVELEFT        0x00

// flags for function set
#define JHD1804_8BITMODE        0x10
#define JHD1804_4BITMODE        0x00
#define JHD1804_2LINE           0x08
#define JHD1804_1LINE           0x00
#define JHD1804_5x10DOTS        0x04
#define JHD1804_5x8DOTS         0x00


int jhd1804_init(lcd_device_priv_t *d) {
    int ret = LCD_OK;
    lcd_intf_t *intf = *(d->intf);
    lcd_intf_meta_t *meta = &(d->intf_meta);

    sleep_ms(50); //wait for the led to start

    //set fuction
    ret |= intf->send_cmd(intf, meta, JHD1804_FUNCTIONSET | JHD1804_2LINE | JHD1804_5x10DOTS);

    //display control
    ret |= intf->send_cmd(intf, meta, JHD1804_DISPLAYCONTROL | JHD1804_DISPLAYON | JHD1804_CURSOROFF | JHD1804_BLINKOFF);

    //clear screen & cursor return
    ret != intf->send_cmd(intf, meta, JHD1804_CLEARDISPLAY);
    sleep_ms(2);

    //mode
    ret |= intf->send_cmd(intf, meta, JHD1804_ENTRYMODESET | JHD1804_ENTRYLEFT | JHD1804_ENTRYSHIFTDECREMENT);

    return ret;
}

int jhd1804_set_cursor(lcd_device_priv_t *d, uint8_t column, uint8_t line) {
    lcd_intf_t *intf = *(d->intf);
    uint8_t offset = (line << 6) +  column; //b6 is line & b0-5 is column, see https://github.com/Seeed-Studio/Grove_LCD_RGB_Backlight/blob/master/rgb_lcd.cpp#L154
    return intf->send_cmd(intf, &(d->intf_meta), JHD1804_SETDDRAMADDR | offset);
}

int jhd1804_shift_view(lcd_device_priv_t *d, shift_direction_t dir) {
    lcd_intf_t *intf = *(d->intf);
    return intf->send_cmd(intf, &(d->intf_meta), JHD1804_CURSORSHIFT | JHD1804_DISPLAYMOVE | dir);
}


lcd_device_t* lcd_create_jhd1804(lcd_intf_t **intf) {
    lcd_device_priv_t *d = lcd_create_device(intf, MAX_STR_LEN);
    if (d == NULL) return NULL;

    //public
    d->emul = false;
    d->size.columns = JHD1804_MEM_COLUMNS;
    d->size.lines = JHD1804_MEM_ROWS;
    d->view.columns = JHD1804_VIEW_COLUMNS;
    d->view.lines = JHD1804_VIEW_ROWS;

    //private
    d->intf_meta.i2c_addr = JHD1804_I2C_ADDRESS;
    d->init = jhd1804_init;
    d->set_cursor = jhd1804_set_cursor;
    d->shift_view = jhd1804_shift_view;

    return (lcd_device_t*) d;
}
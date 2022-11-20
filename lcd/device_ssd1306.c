#include "lcd.h"
#include "device.h"
#include "interface.h"

#define SSD1306_I2C_ADDRESS     0x3C

#define SSD1306_HEIGHT          32
#define SSD1306_WIDTH           128
#define SSD1306_PAGE_HEIGHT     8
#define SSD1306_NUM_PAGES       SSD1306_HEIGHT / SSD1306_PAGE_HEIGHT
#define SSD1306_BUF_LEN         (SSD1306_NUM_PAGES * SSD1306_WIDTH)

// commands (see datasheet)
#define SSD1306_SET_CONTRAST        0x81
#define SSD1306_SET_DISPLAY_SOURCE  0xA4
#define SSD1306_SET_NORM_INV        0xA6
#define SSD1306_SET_DISP            0xAE
#define SSD1306_SET_MEM_ADDR        0x20
#define SSD1306_SET_COL_ADDR        0x21
#define SSD1306_SET_PAGE_ADDR       0x22
#define SSD1306_SET_COL_START_ADDR_LOW 0x00
#define SSD1306_SET_COL_START_ADDR_HIGH 0x10
#define SSD1306_SET_PAGE_START_ADDR 0xB0
#define SSD1306_SET_DISP_START_LINE 0x40
#define SSD1306_SET_SEG_REMAP       0xA0
#define SSD1306_SET_MUX_RATIO       0xA8
#define SSD1306_SET_COM_OUT_DIR     0xC0
#define SSD1306_SET_DISP_OFFSET     0xD3
#define SSD1306_SET_COM_PIN_CFG     0xDA
#define SSD1306_SET_COM_PIN_CFG_MASK 0X02
#define SSD1306_SET_DISP_CLK_DIV    0xD5
#define SSD1306_SET_PRECHARGE       0xD9
#define SSD1306_SET_VCOM_DESEL      0xDB
#define SSD1306_SET_CHARGE_PUMP     0x8D
#define SSD1306_SET_CHARGE_PUMP_MASK 0x10
#define SSD1306_SET_HORIZ_SCROLL    0x26
#define SSD1306_SET_SCROLL          0x2E

int ssd1306_init(lcd_device_priv_t *d) {
    int ret = LCD_OK;
    lcd_intf_t *intf = *(d->intf);
    lcd_intf_meta_t *meta = &(d->intf_meta);

    //set display off: default
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP | 0x00);

    /* addressing settings */

    //set address mode to horizontal (go line by line, wrap to the next line when reaching the end)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_MEM_ADDR);
    ret |= intf->send_cmd(intf, meta, 0x00);

    //drawing area on the screen: default = entire screen (we need to crop the pages)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COL_ADDR);
    ret |= intf->send_cmd(intf, meta, 0);
    ret |= intf->send_cmd(intf, meta, SSD1306_WIDTH-1);
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_PAGE_ADDR);
    ret |= intf->send_cmd(intf, meta, 0);
    ret |= intf->send_cmd(intf, meta, SSD1306_NUM_PAGES-1);


    /* hardware (panel resolution and layout) */

    //set display start line: default = 0
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_START_LINE | 0x00);

    //segment remap: column address 127 is mapped to SEG0 -> left to rigth
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_SEG_REMAP | 0x01);

    //set mutiplex reatio (horizontal lines): number of lines (default to 64)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_MUX_RATIO);
    ret |= intf->send_cmd(intf, meta, SSD1306_HEIGHT - 1);

    //set COM (common) output scan direction: invert / bottom-up (i.e. upside down)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COM_OUT_DIR | (0x01 << 3));

    //set display offset: default = no offset
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_OFFSET);
    ret |= intf->send_cmd(intf, meta, 0x00);

    // set COM (common) pins hardware configuration: Sequential COM pin configuration & Disable COM Left/Right remap
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COM_PIN_CFG); 
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_COM_PIN_CFG_MASK | (0x00 << 4) | (0x00 << 5) );


    /* timing and driving scheme */

    //set display clock divide ratio: default (Freq = 1000b & D = 0000b)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP_CLK_DIV);
    ret |= intf->send_cmd(intf, meta, (0x8 << 4) | 0x0);

    // set pre-charge period: default phase 1 = 2 DCLK, phase 2 = 2 DCLK
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_PRECHARGE); 
    ret |= intf->send_cmd(intf, meta, (0x2 << 4) | 0x2);

    // set VCOMH deselect level: 0.77xVcc
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_VCOM_DESEL); 
    ret |= intf->send_cmd(intf, meta, 0x2 << 4);


    /* fundamental (display) */

    //Set contract control: default = 0x7F
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_CONTRAST); 
    ret |= intf->send_cmd(intf, meta, 0x7F);

    //Entire Display ON: default = follow ram
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISPLAY_SOURCE | 0x00);

    //Set Normal/Inverse Display: default = normal
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_NORM_INV | 0x00);


    /* charge pump */

    //Charge Pump Setting: enable (Vcc internally generated on our board)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_CHARGE_PUMP);
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_CHARGE_PUMP_MASK | (0x01 << 2));


    /* scrolling */

    //(de)activate scroll: deactivate (no default)
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_SCROLL | 0x00);


    //clear screen
    memset(intf->dbuf, 0, intf->dbuf_len);
    int buf_len = SSD1306_BUF_LEN;
    while (buf_len > 0) {
        int tx_size = intf->dbuf_len < buf_len ? intf->dbuf_len : buf_len;
        ret |= intf->send_dbuf(intf, meta, tx_size);
        buf_len -= tx_size;
    }

    // set display on
    ret |= intf->send_cmd(intf, meta, SSD1306_SET_DISP | 0x01); 

    return ret;
}

int ssd1306_set_cursor(lcd_device_priv_t *d, uint8_t column, uint8_t line) {
    int ret = LCD_OK;
    lcd_intf_t *intf = *(d->intf);
    //TODO::SET COLUMN ADDRESS

    uint8_t pixel_column = column * EMUL_FONT_WITH;
    ret |= intf->send_cmd(intf, &(d->intf_meta), SSD1306_SET_COL_START_ADDR_LOW | (pixel_column & 0x0F));
    ret |= intf->send_cmd(intf, &(d->intf_meta), SSD1306_SET_COL_START_ADDR_HIGH | (pixel_column >> 4));
    ret |= intf->send_cmd(intf, &(d->intf_meta), SSD1306_SET_PAGE_START_ADDR | line);

    return ret;
}


lcd_device_t *lcd_create_ssd1306_emul(lcd_intf_t **intf) {
    lcd_device_priv_t *d = lcd_create_device(intf, MAX_STR_LEN * EMUL_FONT_WITH);

    d->emul = true;
    d->size.columns = 21;
    d->size.lines = 4;
    d->view.columns = 21;
    d->view.lines = 4;

    d->intf_meta.i2c_addr = SSD1306_I2C_ADDRESS;
    d->init = ssd1306_init;
    d->set_cursor = ssd1306_set_cursor;
    d->shift_view = NULL;

    return  (lcd_device_t *)d;
}
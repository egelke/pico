#include <stdlib.h>
#include "device.h"
#include "interface.h"
#include "interface_i2c.h"

#define I2C_DBUF_DTA_HEADER     0x40
#define I2C_DBUF_CMD_HEADER     0x80

//i2c specific interface, repeats common parts & add specifics parts
typedef struct lcd_intf_i2c {
    //generic
    LCD_INTF_GEN

    //specific
    i2c_inst_t* inst;
} lcd_intf_i2c_t;


int i2c_send_cmd(lcd_intf_i2c_t *intf, lcd_intf_meta_t *meta, const uint8_t cmd) {
    uint8_t cbuf[] = {I2C_DBUF_CMD_HEADER, cmd};
    return i2c_write_blocking(intf->inst, meta->i2c_addr, cbuf, 2, false) == 2 ? LCD_OK : LCD_ERROR_GENERIC;
}

int i2c_send_dbuf(lcd_intf_i2c_t *intf, lcd_intf_meta_t *meta, const uint8_t data_len) {
    return i2c_write_blocking(intf->inst, meta->i2c_addr, intf->dbuf - 1, data_len + 1, false) == (data_len + 1) ? LCD_OK : LCD_ERROR_GENERIC;
}

lcd_intf_t* lcd_create_i2c(i2c_inst_t *inst) {
    uint8_t dhead[] = {I2C_DBUF_DTA_HEADER};
    lcd_intf_i2c_t* intf = (lcd_intf_i2c_t*) lcd_create_interface(sizeof(lcd_intf_i2c_t), dhead, sizeof(dhead), MAX_STR_LEN);
    if (intf == NULL) return NULL;

//init generic
    intf->send_cmd = (fn_lcd_send_cmd) i2c_send_cmd;
    intf->send_dbuf = (fn_lcd_send_dbuf) i2c_send_dbuf;

//init specific
    intf->inst = inst;
    
    return (lcd_intf_t*) intf;
}

#ifndef EGELKE_INTERFACE_H
#define EGELKE_INTERFACE_H

#include "lcd.h"
#include <stdlib.h>
#include <string.h>

typedef struct lcd_intf_meta {
    uint8_t i2c_addr;
    //uint8_t spi_addr;
} lcd_intf_meta_t;

typedef int (*fn_lcd_send_cmd)(lcd_intf_t *intf, lcd_intf_meta_t* meta, const uint8_t cmd);
typedef int (*fn_lcd_send_dbuf)(lcd_intf_t *intf, lcd_intf_meta_t* meta, const uint8_t data_len);

#define LCD_INTF_GEN \
    uint8_t *dbuf; \
    size_t dbuf_len; \
    fn_lcd_send_cmd send_cmd; \
    fn_lcd_send_dbuf send_dbuf; \

//generic interface, defines common parts
struct lcd_intf {
    LCD_INTF_GEN
};

inline lcd_intf_t *lcd_create_interface(size_t hsize, uint8_t *dheader, size_t dheader_size, size_t dbuf_size) {
    lcd_intf_t* intf = calloc(1, hsize + dheader_size + dbuf_size); //allocate the struct (header) and data buffer.
    if (intf == NULL) return NULL;

    intf->dbuf_len = dbuf_size;
    intf->dbuf = (void*) intf + hsize; //put the dbuf pointer at the end of the struct, i.e. at the start of the dbuf header
    memcpy(intf->dbuf, dheader, dheader_size); //copy the dbuf header
    intf->dbuf += dheader_size; //move the pointer to the actual dbuf, i.e. past the dbuf header
    return intf;
}

inline lcd_intf_t *lcd_resize_dbuf(lcd_intf_t *intf, size_t new_dsize) {
    int prefix_size = (void*) intf->dbuf - (void*) intf; //size of everything before the current dbuf
    //printf("resize: prefix size %d", prefix_size);
    lcd_intf_t* new_intf = realloc(intf, prefix_size + new_dsize);
    if (new_intf == NULL) return NULL;

    new_intf->dbuf = (void*) new_intf + prefix_size; //reset the buffer in case the mem was reallocated
    new_intf->dbuf_len = new_dsize; //update the dbuf size
    memset(new_intf->dbuf, 0, new_intf->dbuf_len); //clear the buffer mem
    
    return new_intf;
}

#endif
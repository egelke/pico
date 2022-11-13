#ifndef EGELKE_DEVICE_H
#define EGELKE_DEVICE_H

#include "lcd.h"
#include "interface.h"
#include <stdlib.h>

typedef int (*fn_lcd_init)(lcd_device_t *c);
typedef int (*fn_lcd_shift_view)(lcd_device_t *c, shift_direction_t dir);

typedef struct lcd_device_priv {
//public
    LCD_DEVICE_PUB

//private
    lcd_intf_t** intf;
    lcd_intf_meta_t intf_meta;

    fn_lcd_init init;
    fn_lcd_shift_view shift_view; 
} lcd_device_priv_t;

inline lcd_device_priv_t* lcd_convert_device(lcd_device_t* d) {
    return (lcd_device_priv_t*) d;
}

inline lcd_device_priv_t* lcd_create_device(lcd_intf_t **intf, uint8_t min_dbuf_size) {
    lcd_device_priv_t *d = calloc(1, sizeof(lcd_device_priv_t));
    if (d == NULL) return NULL;

    if ((*intf)->dbuf_len < min_dbuf_size) {
        *intf = lcd_resize_dbuf(*intf, min_dbuf_size);
        if (*intf == NULL) return NULL;
    }
    d->intf = intf;

    return d;
}

#endif

#ifndef EGELKE_OLED_H
#define EGELKE_OLED_H

#include "hardware/i2c.h"

struct oled_canvas;
enum display_source;

typedef int (*fn_oled_init)(struct oled_canvas *c);
typedef int (*fn_oled_set_source)(struct oled_canvas *c, enum display_source source);
typedef int (*fn_oled_update_gddram)(struct oled_canvas *c);
typedef int (*fn_oled_send_cmd)(struct oled_canvas *c, uint8_t cmd);
typedef int (*fn_oled_send_buffer)(struct oled_canvas *c);

typedef struct oled_canvas {
    void *hw_addr;
    uint8_t *buffer;
    uint16_t buffer_len;
    fn_oled_init init;
    fn_oled_set_source set_source;
    fn_oled_update_gddram update_gddram;
    fn_oled_send_cmd send_cmd;
    fn_oled_send_buffer send_buffer;
} oled_canvas_t;

typedef enum display_source {
    source_all_on = 1,
    source_gddram = 0
} display_source_t;

oled_canvas_t oled_create_ssd1306oI2c(i2c_inst_t *hw_addr);

int oled_init(oled_canvas_t *c);
int oled_set_source(oled_canvas_t *c, display_source_t source);
int oled_update_gddram(oled_canvas_t *c);
int oled_destroy(oled_canvas_t *c);

#endif
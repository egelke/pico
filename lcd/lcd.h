#ifndef EGELKE_LCD_H
#define EGELKE_LCD_H

#include <stdint.h>
#include <stdbool.h> 

#define MAX_STR_LEN             40
#define EMUL_FONT_WITH          6

#define LCD_OK                  0
#define LCD_WARN_NOTDEF         1
#define LCD_ERROR_GENERIC       -1
#define LCD_ERROR_STRSIZE       -2

struct lcd_intf;
typedef struct lcd_intf lcd_intf_t;

//todo::improve
enum shift_direction {
    shift_left  = 0x00,
    shift_rigth = 0x04
};
typedef enum shift_direction shift_direction_t;

typedef struct lcd_pane {
    uint8_t columns;
    uint8_t lines;
} lcd_pane_t, lcd_pos_t;

#define LCD_DEVICE_PUB \
    bool emul; \
    lcd_pane_t size; \
    lcd_pane_t view; 

typedef struct lcd_device {
    LCD_DEVICE_PUB
} lcd_device_t;

int lcd_init(lcd_device_t *d);
int lcd_write_str(lcd_device_t *d, const char * str);
int lcd_set_cursor(lcd_device_t *d, uint8_t column, uint8_t line);
int lcd_shift_view(lcd_device_t *d, shift_direction_t dir);

#endif
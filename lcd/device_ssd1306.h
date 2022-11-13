#ifndef EGELKE_DEVICE_SSD1306_H
#define EGELKE_DEVICE_SSD1306_H

#include "lcd.h"

lcd_device_t* lcd_create_ssd1306_emul(lcd_intf_t **intf);

#endif
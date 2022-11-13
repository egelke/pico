#ifndef EGELKE_INTERFACE_I2C_H
#define EGELKE_INTERFACE_I2C_H

#include "lcd.h"
#include "hardware/i2c.h"

lcd_intf_t* lcd_create_i2c(i2c_inst_t *inst);

#endif
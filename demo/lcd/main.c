#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "hardware/i2c.h"
#include "hardware/gpio.h"

#include "lcd.h"
#include "interface_i2c.h"
#include "device_jhd1804.h"
#include "device_ssd1306.h"

bool rtcb_shift_view_left(repeating_timer_t *t) {
    lcd_shift_view(t->user_data, shift_left);
    return true;
}

int main() {
    int ret = PICO_OK;
    stdio_init_all();

    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);

    lcd_intf_t *intf = lcd_create_i2c(i2c0);
    if (!intf) goto error;
    lcd_device_t *jhd1804 = lcd_create_jhd1804(&intf);
    if (!jhd1804) goto error;
    lcd_device_t *ssd1306 = lcd_create_ssd1306_emul(&intf);
    if (!ssd1306) goto error;

    ret |= lcd_init(jhd1804);
    ret |= lcd_write_str(jhd1804, "Hello Alycia, Laura & Darya!");
    if (ret) goto error;

    ret |= lcd_init(ssd1306);
    ret |= lcd_write_str(ssd1306, "Hello Alycia, Laura & Darya!  ");
    ret |= lcd_write_str(ssd1306, "Groetjes van Tato");
    if (ret) goto error;

    repeating_timer_t timer;
    add_repeating_timer_ms(-1000, rtcb_shift_view_left, jhd1804, &timer);

    int i = 40;
    while(i--) {
        sleep_ms(1000);
    }

    cancel_repeating_timer(&timer);
    free(jhd1804);
    free(ssd1306);
    free(intf);
    
    return 0;

error:
    printf("Error occured");
    return -1;
}

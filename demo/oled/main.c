
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "egelke_oled.h"

void main() {
    stdio_init_all();

    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("Egelke OLED demo"));

    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    oled_canvas_t c = oled_create_ssd1306oI2c(i2c0);
    oled_init(&c);

    while(true) {
        oled_set_source(&c, source_all_on);
        sleep_ms(500);
        oled_set_source(&c, source_gddram);
        sleep_ms(500);
    }

    oled_destroy(&c);
}
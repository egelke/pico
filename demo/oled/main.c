
#include <stdio.h>
#include "pico/stdlib.h"
#include "pico/binary_info.h"
#include "hardware/i2c.h"
#include "egelke_oled.h"

void main() {
    int ret;
    stdio_init_all();

    bi_decl(bi_2pins_with_func(PICO_DEFAULT_I2C_SDA_PIN, PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C));
    bi_decl(bi_program_description("Egelke OLED demo"));

    i2c_init(i2c0, 400 * 1000);
    gpio_set_function(PICO_DEFAULT_I2C_SDA_PIN, GPIO_FUNC_I2C);
    gpio_set_function(PICO_DEFAULT_I2C_SCL_PIN, GPIO_FUNC_I2C);
    gpio_pull_up(PICO_DEFAULT_I2C_SDA_PIN);
    gpio_pull_up(PICO_DEFAULT_I2C_SCL_PIN);

    //oled_canvas_t c = oled_create_ssd1306oI2c(i2c0, mode_buffered);
    oled_canvas_t c = oled_create_ssd1306oI2c(i2c0);
    ret = oled_init(&c);
    if (ret != PICO_OK) goto error;

    //int i=3;
    //while(i--) {
        ret = oled_set_source(&c, source_all_on);
        if (ret != PICO_OK) goto error;
        sleep_ms(500);
        ret = oled_set_source(&c, source_gddram);
        if (ret != PICO_OK) goto error;
        sleep_ms(500);
    //}

    oled_draw_hline(&c, 0, 0, 127);
    oled_draw_hline(&c, 10, 20, 100);
    oled_draw_hline(&c, 20, 23, 60);
    oled_draw_hline(&c, 0, 31, 127);
    oled_update_gddram(&c);
    //oled_draw_text(&c, "Hello World!  How are you doing? Great!  How many lines can we write here? 4?");
    oled_destroy(&c);
    return;

error:
    printf("failed with error %x", ret);
}
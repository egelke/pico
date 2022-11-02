#include <stdio.h>
#include "pico/stdlib.h"

#define PICO_STDIO_USB_CONNECT_WAIT_TIMEOUT_MS = 10000

int main() {
    stdio_init_all();

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, true);
    while (true)
    {
        printf("blink\r\n");
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        sleep_ms(250);
        gpio_put(PICO_DEFAULT_LED_PIN, false);
        sleep_ms(250);
    }
    

}
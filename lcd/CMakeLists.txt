add_library(egelke_lcd
    lcd.c
    interface_i2c.c
    device_jhd1804.c
    device_ssd1306.c
)

target_link_libraries(egelke_lcd
    pico_stdlib
    hardware_i2c
)
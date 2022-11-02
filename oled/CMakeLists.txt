add_library(egelke_oled
    egelke_oled.c
)

target_link_libraries(egelke_oled
    pico_stdlib
    hardware_i2c
)
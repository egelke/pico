add_executable(demo_lcd main.c)

pico_add_extra_outputs(demo_lcd)

target_link_libraries(demo_lcd
    pico_stdlib
    hardware_i2c
    egelke_lcd
)

target_include_directories(demo_lcd PUBLIC
    "${PROJECT_SOURCE_DIR}/lcd"
)
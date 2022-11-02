add_executable(demo_oled main.c)

pico_add_extra_outputs(demo_oled)

target_link_libraries(demo_oled
    pico_stdlib
    hardware_i2c
    egelke_oled
)

target_include_directories(demo_oled PUBLIC
    "${PROJECT_SOURCE_DIR}/oled"
)
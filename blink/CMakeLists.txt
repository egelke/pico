add_executable(blink main.c)

pico_add_extra_outputs(blink)

target_link_libraries(blink pico_stdlib)

pico_enable_stdio_usb(blink 0)
pico_enable_stdio_uart(blink 1)
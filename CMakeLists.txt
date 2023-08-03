cmake_minimum_required(VERSION 3.25)

include(pico_sdk_import.cmake)

project(picoled C CXX)

pico_sdk_init()

add_subdirectory(lib/pico-ssd1306)

add_executable(picoled
    src/main.cxx
    src/main.hxx
)

target_link_libraries(picoled
    pico_stdlib
    pico_stdio
    hardware_i2c
    pico_ssd1306
)

target_include_directories(picoled PRIVATE
    lib/pico-ssd1306
)

pico_enable_stdio_usb(picoled 1)
pico_enable_stdio_uart(picoled 0)

pico_add_extra_outputs(picoled)
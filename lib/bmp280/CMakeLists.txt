add_library(bmp280 STATIC
    bmp280.c
)

target_include_directories(bmp280 PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}
)

target_link_libraries(bmp280
    pico_stdlib
    hardware_i2c
)

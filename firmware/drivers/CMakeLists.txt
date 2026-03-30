add_library(drivers STATIC)

target_sources(drivers PRIVATE
    src/oled.c
)

target_include_directories(drivers PUBLIC include/)

target_link_libraries(drivers PRIVATE bsp)

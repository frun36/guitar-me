add_library(common STATIC)

target_sources(common PRIVATE
    src/ring_buffer.c
)

target_include_directories(common PUBLIC include/)

add_library(common STATIC)

target_sources(common PRIVATE
    src/common.c
)

target_include_directories(common PUBLIC include/)

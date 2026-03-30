add_executable(app)

target_sources(app PRIVATE
    src/main.c
    src/Button.c
    src/LED.c
    src/DSP.c
    src/Encoder.c
    src/FX/EQ_Peak.c
)

target_include_directories(app PRIVATE
    include/
)

target_compile_options(app PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Wunused-parameter
    -Winit-self
    -Wcast-align
    -Wconversion
    -Wnull-dereference
    -Wduplicated-cond
    -Wsign-conversion
    -Wlogical-op
    
    -fdiagnostics-color=always
)

target_link_libraries(app PRIVATE 
    common
    bsp 
    drivers 
    CMSISDSP 
    m
)

target_link_options(app PRIVATE
    -Wl,-Map=$<TARGET_FILE_DIR:app>/app.map
    -Wl,--gc-sections
)

add_custom_target(flash
    COMMAND arm-none-eabi-objcopy -O binary $<TARGET_FILE:app> $<TARGET_FILE_DIR:app>/app.bin
    COMMAND st-flash --connect-under-reset --reset write $<TARGET_FILE_DIR:app>/app.bin 0x8000000
    COMMAND st-flash --connect-under-reset reset
    DEPENDS app
    COMMENT "Flashing $<TARGET_FILE_NAME:app> to 0x8000000..."
    VERBATIM
)

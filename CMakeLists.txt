cmake_minimum_required(VERSION 3.22)

# Project
project(guitar-me)
enable_language(C ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_C_EXTENSIONS ON)

add_executable(${CMAKE_PROJECT_NAME})

# Build types
set(CMAKE_C_FLAGS_DEBUG "-O0 -g3 -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g3 -DDEBUG")

set(CMAKE_C_FLAGS_RELEASE "-O3 -DNDEBUG")
set(CMAKE_CXX_FLAGS_RELEASE "-O3 -DNDEBUG")

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()
message("Build type: " ${CMAKE_BUILD_TYPE})

# For clangd
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

# User sources
target_sources(${CMAKE_PROJECT_NAME} PRIVATE
    src/main.c
    # ...
)

target_include_directories(${CMAKE_PROJECT_NAME} PRIVATE
    include/
    # ...
)

# STM32CubeMX generated sources
add_subdirectory(
    guitar-me-cube/cmake/stm32cubemx 
)

# DSP
add_compile_options(
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)
add_link_options(
    -mcpu=cortex-m4
    -mfpu=fpv4-sp-d16
    -mfloat-abi=hard
)

add_library(cmsis_dsp_lib STATIC IMPORTED)
set_target_properties(cmsis_dsp_lib PROPERTIES
    IMPORTED_LOCATION "${CMAKE_SOURCE_DIR}/guitar-me-cube/Drivers/CMSIS/Lib/GCC/libarm_cortexM4lf_math.a"
)
target_include_directories(cmsis_dsp_lib INTERFACE
    "${CMAKE_SOURCE_DIR}/guitar-me-cube/Drivers/CMSIS/DSP/Include"
)
target_compile_definitions(cmsis_dsp_lib INTERFACE
    ARM_MATH_CM4
)

# Libraries

target_link_libraries(${CMAKE_PROJECT_NAME}
    stm32cubemx
    cmsis_dsp_lib
    m
    # ...
)

# Defines
target_compile_definitions(${CMAKE_PROJECT_NAME} PRIVATE
    # ...
)

# Remove wrong libob.a library dependency when using cpp files
# list(REMOVE_ITEM CMAKE_C_IMPLICIT_LINK_LIBRARIES ob)

# Target for flash
add_custom_target(flash
    COMMAND arm-none-eabi-objcopy -O binary ${CMAKE_PROJECT_NAME}.elf ${CMAKE_PROJECT_NAME}.bin
    COMMAND st-flash --connect-under-reset --reset write ${CMAKE_PROJECT_NAME}.bin 0x8000000
    COMMAND st-flash --connect-under-reset reset
    DEPENDS ${CMAKE_PROJECT_NAME}
    WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
    COMMENT "Flashing ${CMAKE_PROJECT_NAME}.bin to 0x8000000..."
)

add_custom_target(openocd
    COMMAND openocd -f interface/stlink.cfg -f target/stm32f3x.cfg
    DEPENDS ${CMAKE_PROJECT_NAME}
    COMMENT "Launching OpenOCD"
    USES_TERMINAL
)

add_custom_target(gdb
    COMMAND arm-none-eabi-gdb -x ${CMAKE_SOURCE_DIR}/.gdbinit ${ELF_FILE}
    DEPENDS ${PROJECT_NAME}
    COMMENT "Starting GDB Session..."
    USES_TERMINAL
)


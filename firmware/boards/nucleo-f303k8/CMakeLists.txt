# STM32CubeMX generated symbols (macros)
set(MX_Defines_Syms 
	USE_FULL_LL_DRIVER 
	HSE_VALUE=8000000 
	HSE_STARTUP_TIMEOUT=100 
	LSE_STARTUP_TIMEOUT=5000 
	LSE_VALUE=32768 
	EXTERNAL_CLOCK_VALUE=8000000 
	HSI_VALUE=8000000 
	LSI_VALUE=40000 
	VDD_VALUE=3300 
	PREFETCH_ENABLE=1 
	STM32F303x8
    $<$<CONFIG:Debug>:DEBUG>
)

# STM32CubeMX generated include paths
set(MX_Include_Dirs
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Inc
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Device/ST/STM32F3xx/Include
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS/Include
)

# STM32CubeMX generated application sources
set(MX_Application_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/stm32f3xx_it.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/sysmem.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/syscalls.c
    ${CMAKE_CURRENT_SOURCE_DIR}/startup_stm32f303x8.s
)

# STM32 HAL/LL Drivers
set(STM32_Drivers_Src
    ${CMAKE_CURRENT_SOURCE_DIR}/Core/Src/system_stm32f3xx.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_utils.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_exti.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_gpio.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_adc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_dma.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_rcc.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_dac.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_i2c.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_tim.c
    ${CMAKE_CURRENT_SOURCE_DIR}/Drivers/STM32F3xx_HAL_Driver/Src/stm32f3xx_ll_usart.c
)

add_library(bsp_nucleo-f303k8 OBJECT
    ${STM32_Drivers_Src} 
    ${MX_Application_Src}
    ${CMAKE_CURRENT_SOURCE_DIR}/src/bsp_codec.c
)

if(CMAKE_C_COMPILER_ID MATCHES "GNU")
  # Suppress conversion warnings only for GNU compilers
  target_compile_options(bsp_nucleo-f303k8 PUBLIC -Wno-sign-conversion -Wno-conversion)
endif()


target_include_directories(bsp_nucleo-f303k8 PUBLIC ${MX_Include_Dirs})
target_compile_definitions(bsp_nucleo-f303k8 PUBLIC ${MX_Defines_Syms})

# For CMSIS DSP
set(CMSISCORE "${CMAKE_CURRENT_SOURCE_DIR}/Drivers/CMSIS" CACHE INTERNAL "Path to CMSIS Core headers provided by the BSP")

target_sources(bsp_nucleo-f303k8
    PUBLIC FILE_SET HEADERS BASE_DIRS ../include
    FILES ../include/bsp_codec.h
)

target_link_libraries(bsp_nucleo-f303k8 PRIVATE common)

target_link_options(bsp_nucleo-f303k8 INTERFACE 
    -T${CMAKE_CURRENT_SOURCE_DIR}/STM32F303XX_FLASH.ld 
)

if(NOT TARGET bsp)
  add_library(bsp ALIAS bsp_nucleo-f303k8)
endif()

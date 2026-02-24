#pragma once

#include "stm32f303x8.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"

static inline void LED_Init() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef led = {
        .Pin = LL_GPIO_PIN_3,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };

    LL_GPIO_Init(GPIOB, &led);
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

static inline void LED_On() {
    LL_GPIO_SetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

static inline void LED_Off() {
    LL_GPIO_ResetOutputPin(GPIOB, LL_GPIO_PIN_3);
}

static inline void LED_Toggle() {
    LL_GPIO_TogglePin(GPIOB, LL_GPIO_PIN_3);
}

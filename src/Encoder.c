#include "Encoder.h"

#include <stm32f3xx_ll_bus.h>
#include <stm32f3xx_ll_gpio.h>
#include <stm32f3xx_ll_tim.h>

void Encoder_Init() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM3);

    LL_GPIO_InitTypeDef ch1 = {
        .Pin = LL_GPIO_PIN_4,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_HIGH,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_2,
    };
    LL_GPIO_Init(GPIOB, &ch1);
    LL_GPIO_InitTypeDef ch2 = {
        .Pin = LL_GPIO_PIN_5,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_HIGH,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_2,
    };
    LL_GPIO_Init(GPIOB, &ch2);

    LL_TIM_InitTypeDef tim = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = 0xFFFF,
    };
    LL_TIM_Init(TIM3, &tim);

    LL_TIM_ENCODER_InitTypeDef enc = {
        .EncoderMode = LL_TIM_ENCODERMODE_X4_TI12,
        .IC1Polarity = LL_TIM_IC_POLARITY_FALLING,
        .IC1ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI,
        .IC1Prescaler = LL_TIM_ICPSC_DIV1,
        .IC1Filter = LL_TIM_IC_FILTER_FDIV16_N8,
        .IC2Polarity = LL_TIM_IC_POLARITY_FALLING,
        .IC2ActiveInput = LL_TIM_ACTIVEINPUT_DIRECTTI,
        .IC2Prescaler = LL_TIM_ICPSC_DIV1,
        .IC2Filter = LL_TIM_IC_FILTER_FDIV16_N8,
    };
    LL_TIM_ENCODER_Init(TIM3, &enc);

    LL_TIM_EnableCounter(TIM3);
}

int GetPosition() {
    int pos = LL_TIM_GetCounter(TIM3); // max 0xFFFF
    // wrap at half resolution, should be good enough
    if (pos & 0x8000)
        pos -= 0xFFFF;
    return pos >> 2;
}

int Encoder_GetDelta() {
    static int prev = 0;
    int curr = GetPosition();
    int delta = curr - prev;
    prev = curr;
    return delta;
}

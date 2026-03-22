#include "Button.h"

#include <stdbool.h>

#include "stm32f303x8.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_exti.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_system.h"

#define DEBOUNCE_MS 50

static uint32_t s_last_press_time = 0;
static Button_Event_t s_event;

static inline void HandlePress() {
    s_event = BTN_PRESSED;
}

void EXTI0_IRQHandler() {
    if (LL_EXTI_IsActiveFlag_0_31(LL_EXTI_LINE_0)) {
        LL_EXTI_ClearFlag_0_31(LL_EXTI_LINE_0);
        uint32_t curr_time = SysTick->VAL;
        // Bounce occurs during press and release
        // checks edge was falling and debounce period elapsed
        if ((LL_GPIO_ReadInputPort(GPIOB) & LL_GPIO_PIN_0)
            && s_last_press_time - curr_time > DEBOUNCE_MS) {
            HandlePress();
        }
        s_last_press_time = curr_time;
    }
}

void Button_Init() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef btn = {
        .Pin = LL_GPIO_PIN_0,
        .Mode = LL_GPIO_MODE_INPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_UP
    };

    LL_GPIO_Init(GPIOB, &btn);

    NVIC_SetPriority(EXTI0_IRQn, 1);
    NVIC_EnableIRQ(EXTI0_IRQn);

    LL_SYSCFG_SetEXTISource(LL_SYSCFG_EXTI_PORTB, LL_SYSCFG_EXTI_LINE0);
    LL_EXTI_InitTypeDef it = {
        .Line_0_31 = LL_EXTI_LINE_0,
        .Line_32_63 = LL_EXTI_LINE_NONE,
        .LineCommand = ENABLE,
        .Mode = LL_EXTI_MODE_IT,
        .Trigger = LL_EXTI_TRIGGER_RISING_FALLING, // for debouncing
    };
    LL_EXTI_Init(&it);
}

Button_Event_t Button_GetEvent() {
    Button_Event_t e = s_event;
    s_event = BTN_NONE;
    return e;
}

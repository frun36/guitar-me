#include "bsp_led.h"

#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"

#define LED1_PORT GPIOB
#define LED1_PIN LL_GPIO_PIN_3
#define LED2_PORT GPIOA
#define LED2_PIN LL_GPIO_PIN_11

static inline GPIO_TypeDef* GetLedPort(BSP_LED_t led) {
    switch (led) {
        case LED1:
            return LED1_PORT;
        case LED2:
            return LED2_PORT;
        default:
            return LED1_PORT;
    }
}

static inline uint32_t GetLedPin(BSP_LED_t led) {
    switch (led) {
        case LED1:
            return LED1_PIN;
        case LED2:
            return LED2_PIN;
        default:
            return LED1_PIN;
    }
}

void BSP_LED_Init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);

    LL_GPIO_InitTypeDef led1 = {
        .Pin = LED1_PIN,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };

    LL_GPIO_Init(LED1_PORT, &led1);
    LL_GPIO_ResetOutputPin(LED1_PORT, LED1_PIN);

    LL_GPIO_InitTypeDef led2 = {
        .Pin = LED2_PIN,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };
    LL_GPIO_Init(GPIOA, &led2);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
}

void BSP_LED_On(BSP_LED_t led) {
    LL_GPIO_SetOutputPin(GetLedPort(led), GetLedPin(led));
}

void BSP_LED_Off(BSP_LED_t led) {
    LL_GPIO_ResetOutputPin(GetLedPort(led), GetLedPin(led));
}

void BSP_LED_Toggle(BSP_LED_t led) {
    LL_GPIO_TogglePin(GetLedPort(led), GetLedPin(led));
}

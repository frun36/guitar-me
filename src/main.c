#include <stdio.h>

#include "Codec.h"
#include "DSP.h"
#include "Encoder.h"
#include "LED.h"
#include "OLED.h"
#include "stm32f303x8.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_usart.h"

extern void SystemClock_Config(void);
extern void MX_USART2_UART_Init();

// For printf
int _write(int file, char* ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
        LL_USART_TransmitData8(USART2, ptr[i]);
    }
    return len;
}

int main(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    SystemClock_Config();
    MX_USART2_UART_Init();

    printf("Helou\r\n");

    LED_Init();
    Encoder_Init();
    OLED_Init();
    for (uint32_t i = 0; i < OLED_WIDTH; i++) {
        OLED_SetPixel(i, i % OLED_HEIGHT, true);
    }
    OLED_Flush();

    DSP_Init();
    Codec_Init();

    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef led = {
        .Pin = LL_GPIO_PIN_11,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };
    LL_GPIO_Init(GPIOA, &led);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);

    while (1) {
        LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);

        Codec_Handle();
        DSP_UpdateParameters(Encoder_GetDelta());

        LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
    }
}

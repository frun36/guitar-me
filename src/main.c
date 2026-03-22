#include <stdio.h>

#include "Button.h"
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
    Button_Init();
    Encoder_Init();
    OLED_Init();

    DSP_Init();
    Codec_Init();

    while (1) {
        DSP_UpdateParameters(Encoder_GetDelta());
    }
}

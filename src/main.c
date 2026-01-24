#include <stdio.h>

#include "LED.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_utils.h"

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

    LED_Init();

    while (1) {
        printf("Helou\r\n");
        LED_On();
        LL_mDelay(500);
        LED_Off();
        LL_mDelay(500);
    }
}

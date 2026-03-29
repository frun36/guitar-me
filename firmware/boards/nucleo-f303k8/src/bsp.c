#include "bsp.h"

#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_usart.h"

extern void SystemClock_Config(void);
extern void MX_USART2_UART_Init(void);

// For printf
int _write(int file, char* ptr, int len) {
    (void)file;
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
        LL_USART_TransmitData8(USART2, ptr[i]);
    }
    return len;
}

void BSP_Init(void) {
    LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

    SystemClock_Config();
    MX_USART2_UART_Init();
}

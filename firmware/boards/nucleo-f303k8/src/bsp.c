#include "bsp.h"

#include "bsp_led.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_cortex.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_utils.h"

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

    NVIC_SetPriority(SysTick_IRQn, 0);
    LL_SYSTICK_EnableIT();
}

volatile uint32_t s_tick;
BSP_TickHandler_t s_tick_handler;

// Ideally, audio processing <1ms
void SysTick_Handler(void) {
    s_tick++;
    BSP_LED_Toggle(LED1);
    if (s_tick_handler) {
        s_tick_handler(s_tick);
    }
}

uint32_t BSP_GetTime(void) {
    return s_tick;
}

void BSP_SetTickHandler(BSP_TickHandler_t tick_handler) {
    s_tick_handler = tick_handler;
}

void BSP_Delay(uint32_t ms) {
    uint32_t now = BSP_GetTime();
    // will wait at least ms
    while (BSP_GetTime() - now <= ms) {}
}

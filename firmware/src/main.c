#include "main.h"
#include "dac.h"
#include "usart.h"
#include "gpio.h"
#include "stm32f3xx_ll_dac.h"

#include <stdio.h>

extern void SystemClock_Config(void);

// For printf
int _write(int file, char *ptr, int len) {
    for (int i = 0; i < len; i++) {
        while (!LL_USART_IsActiveFlag_TXE(USART2)) {}
        LL_USART_TransmitData8(USART2, ptr[i]);
    }
    return len;
}

int main(void)
{
  LL_APB2_GRP1_EnableClock(LL_APB2_GRP1_PERIPH_SYSCFG);
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR);

  NVIC_SetPriorityGrouping(NVIC_PRIORITYGROUP_4);

  SystemClock_Config();

  MX_GPIO_Init();
  MX_USART2_UART_Init();
  MX_DAC1_Init();

  uint32_t dacOut = 2048;
  LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);
  LL_mDelay(10);
  while (1)
  {
    LL_DAC_ConvertData12RightAligned(DAC1, LL_DAC_CHANNEL_1, dacOut);
    LL_DAC_TrigSWConversion(DAC1, LL_DAC_CHANNEL_1);
    dacOut += 128;
    if (dacOut > 4095) dacOut = 0;
    LL_GPIO_TogglePin(LED_GPIO_Port, LED_Pin);
    printf("blink, dac=%u\r\n", dacOut);
    LL_mDelay(100);
  }
}

#include <stdio.h>

#include "LED.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_dac.h"
#include "stm32f3xx_ll_dma.h"
#include "stm32f3xx_ll_system.h"
#include "stm32f3xx_ll_tim.h"
#include "stm32f3xx_ll_usart.h"
#include "stm32f3xx_ll_utils.h"

#define CLOCK_FREQ 64000000
#define OUT_SAMPLE_FREQ 32000

extern void SystemClock_Config(void);
extern void MX_USART2_UART_Init();

const uint16_t Sine_LUT[64] = {2048, 2249, 2447, 2642, 2831, 3013, 3185, 3347,
                               3495, 3630, 3750, 3853, 3939, 4007, 4056, 4085,
                               4095, 4085, 4056, 4007, 3939, 3853, 3750, 3630,
                               3495, 3347, 3185, 3013, 2831, 2642, 2447, 2249,
                               2048, 1847, 1649, 1454, 1265, 1083, 911,  749,
                               601,  466,  346,  243,  157,  89,   40,   11,
                               1,    11,   40,   89,   157,  243,  346,  466,
                               601,  749,  911,  1083, 1265, 1454, 1649, 1847};

volatile uint32_t idx = 0;

void DAC_Init() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_TIM6);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_DAC1);

    // DMA
    LL_DMA_InitTypeDef dma = {
        .PeriphOrM2MSrcAddress = LL_DAC_DMA_GetRegAddr(
            DAC1,
            LL_DAC_CHANNEL_1,
            LL_DAC_DMA_REG_DATA_12BITS_RIGHT_ALIGNED
        ),
        .MemoryOrM2MDstAddress = (uint32_t)Sine_LUT,
        .Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
        .Mode = LL_DMA_MODE_CIRCULAR,
        .PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT,
        .MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT,
        .PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_WORD,
        .MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD,
        .NbData = 64,
    };
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_3, &dma);
    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_SYSCFG_SetRemapDMA_DAC(LL_SYSCFG_DAC1_CH1_RMP_DMA1_CH3);

    // Timer
    LL_TIM_InitTypeDef timer = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = CLOCK_FREQ / OUT_SAMPLE_FREQ - 1,
    };
    LL_TIM_Init(TIM6, &timer);
    LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_UPDATE);

    // DAC
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

    DAC1->SR |= DAC_SR_DMAUDR1;
    LL_DAC_InitTypeDef dac = {
        .TriggerSource = LL_DAC_TRIG_EXT_TIM6_TRGO,
        .WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE,
        .OutputBuffer = LL_DAC_OUTPUT_BUFFER_ENABLE,
    };
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &dac);
    LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_EnableDMAReq(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);

    LL_mDelay(10);

    LL_TIM_GenerateEvent_UPDATE(TIM6);
    LL_TIM_EnableCounter(TIM6);
}

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
    DAC_Init();

    while (1) {
        LL_mDelay(100);
    }
}

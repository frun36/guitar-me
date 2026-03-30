#include "bsp_codec.h"

#include <stdbool.h>
#include <stdint.h>
#include <stm32f3xx_ll_adc.h>
#include <stm32f3xx_ll_bus.h>
#include <stm32f3xx_ll_dac.h>
#include <stm32f3xx_ll_dma.h>
#include <stm32f3xx_ll_gpio.h>
#include <stm32f3xx_ll_system.h>
#include <stm32f3xx_ll_tim.h>

#include "bsp.h"
#include "config.h"

static uint16_t s_adc_buf[BUF_SIZE];
static uint16_t s_dac_buf[BUF_SIZE];

static BSP_Codec_Callback_t s_dsp_process;

void DMA1_Channel1_IRQHandler(void) {
    bool data_ready = false;
    uint16_t* adc_ptr;
    uint16_t* dac_ptr;

    if (LL_DMA_IsActiveFlag_HT1(DMA1)) {
        LL_DMA_ClearFlag_HT1(DMA1);
        data_ready = true;
        adc_ptr = s_adc_buf;
        dac_ptr = s_dac_buf;
    }

    if (LL_DMA_IsActiveFlag_TC1(DMA1)) {
        LL_DMA_ClearFlag_TC1(DMA1);
        data_ready = true;
        adc_ptr = s_adc_buf + BUF_SIZE_HALF;
        dac_ptr = s_dac_buf + BUF_SIZE_HALF;
    }

    if (data_ready) {
        s_dsp_process(adc_ptr, dac_ptr);
    }
}

static void TIM6_Init(void) {
    LL_TIM_InitTypeDef timer = {
        .Prescaler = 0,
        .CounterMode = LL_TIM_COUNTERMODE_UP,
        .Autoreload = CLOCK_FREQ / SAMPLE_FREQ - 1,
    };
    LL_TIM_Init(TIM6, &timer);
    LL_TIM_SetTriggerOutput(TIM6, LL_TIM_TRGO_UPDATE);
}

static void ADC_Init(void) {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_ADC12);
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_DMA1);

    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_0, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_0, LL_GPIO_PULL_NO);

    // ADC
    LL_ADC_CommonInitTypeDef adc_common = {
        .CommonClock = LL_ADC_CLOCK_ASYNC_DIV1
    };
    LL_ADC_CommonInit(__LL_ADC_COMMON_INSTANCE(ADC1), &adc_common);

    LL_ADC_InitTypeDef adc = {
        .Resolution = LL_ADC_RESOLUTION_12B,
        .DataAlignment = LL_ADC_DATA_ALIGN_RIGHT,
        .LowPowerMode = LL_ADC_LP_MODE_NONE,
    };
    LL_ADC_Init(ADC1, &adc);

    LL_ADC_EnableInternalRegulator(ADC1);
    for (volatile uint32_t i = 0; i < 10000; i++)
        ;

    LL_ADC_StartCalibration(ADC1, LL_ADC_SINGLE_ENDED);
    while (LL_ADC_IsCalibrationOnGoing(ADC1))
        ;

    LL_ADC_REG_InitTypeDef adc_reg = {
        .TriggerSource = LL_ADC_REG_TRIG_EXT_TIM6_TRGO,
        .SequencerLength = LL_ADC_REG_SEQ_SCAN_DISABLE,
        .SequencerDiscont = LL_ADC_REG_SEQ_DISCONT_DISABLE,
        .ContinuousMode = LL_ADC_REG_CONV_SINGLE,
        .DMATransfer = LL_ADC_REG_DMA_TRANSFER_UNLIMITED,
        .Overrun = LL_ADC_REG_OVR_DATA_OVERWRITTEN,
    };
    LL_ADC_REG_Init(ADC1, &adc_reg);

    LL_ADC_REG_SetSequencerRanks(ADC1, LL_ADC_REG_RANK_1, LL_ADC_CHANNEL_1);
    LL_ADC_SetChannelSamplingTime(
        ADC1,
        LL_ADC_CHANNEL_1,
        LL_ADC_SAMPLINGTIME_181CYCLES_5
    );

    LL_ADC_ClearFlag_ADRDY(ADC1);
    LL_ADC_Enable(ADC1);
    while (LL_ADC_IsActiveFlag_ADRDY(ADC1) == 0)
        ;

    // DMA
    LL_DMA_InitTypeDef dma = {
        .PeriphOrM2MSrcAddress =
            LL_ADC_DMA_GetRegAddr(ADC1, LL_ADC_DMA_REG_REGULAR_DATA),
        .MemoryOrM2MDstAddress = (uint32_t)s_adc_buf,
        .Direction = LL_DMA_DIRECTION_PERIPH_TO_MEMORY,
        .Mode = LL_DMA_MODE_CIRCULAR,
        .PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT,
        .MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT,
        .PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD,
        .MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD,
        .NbData = BUF_SIZE,
    };
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_1, &dma);

    LL_DMA_EnableIT_HT(DMA1, LL_DMA_CHANNEL_1);
    LL_DMA_EnableIT_TC(DMA1, LL_DMA_CHANNEL_1);

    NVIC_SetPriority(DMA1_Channel1_IRQn, 0);
    NVIC_EnableIRQ(DMA1_Channel1_IRQn);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_1);
    LL_ADC_REG_StartConversion(ADC1);
}

static void DAC_Init(void) {
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
        .MemoryOrM2MDstAddress = (uint32_t)s_dac_buf,
        .Direction = LL_DMA_DIRECTION_MEMORY_TO_PERIPH,
        .Mode = LL_DMA_MODE_CIRCULAR,
        .PeriphOrM2MSrcIncMode = LL_DMA_PERIPH_NOINCREMENT,
        .MemoryOrM2MDstIncMode = LL_DMA_MEMORY_INCREMENT,
        .PeriphOrM2MSrcDataSize = LL_DMA_PDATAALIGN_HALFWORD,
        .MemoryOrM2MDstDataSize = LL_DMA_MDATAALIGN_HALFWORD,
        .NbData = BUF_SIZE,
    };
    LL_DMA_Init(DMA1, LL_DMA_CHANNEL_3, &dma);
    LL_SYSCFG_SetRemapDMA_DAC(LL_SYSCFG_DAC1_CH1_RMP_DMA1_CH3);

    // DAC
    LL_GPIO_SetPinMode(GPIOA, LL_GPIO_PIN_4, LL_GPIO_MODE_ANALOG);
    LL_GPIO_SetPinPull(GPIOA, LL_GPIO_PIN_4, LL_GPIO_PULL_NO);

    LL_DAC_InitTypeDef dac = {
        .TriggerSource = LL_DAC_TRIG_EXT_TIM6_TRGO,
        .WaveAutoGeneration = LL_DAC_WAVE_AUTO_GENERATION_NONE,
        .OutputBuffer = LL_DAC_OUTPUT_BUFFER_DISABLE,
    };
    LL_DAC_Init(DAC1, LL_DAC_CHANNEL_1, &dac);
    LL_DAC_EnableTrigger(DAC1, LL_DAC_CHANNEL_1);
    LL_DAC_Enable(DAC1, LL_DAC_CHANNEL_1);

    BSP_Delay(10);

    LL_DMA_EnableChannel(DMA1, LL_DMA_CHANNEL_3);
    LL_DAC_EnableDMAReq(DAC1, LL_DAC_CHANNEL_1);
}

void BSP_Codec_Init(BSP_Codec_Callback_t dsp_process) {
    s_dsp_process = dsp_process;

    DAC_Init();
    ADC_Init();
    TIM6_Init();
    LL_TIM_EnableCounter(TIM6);
}

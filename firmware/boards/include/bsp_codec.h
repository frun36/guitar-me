#pragma once

#include <stdint.h>

typedef void (*BSP_Codec_Callback_t)(uint16_t* in, uint16_t* out);

void BSP_Codec_Init(BSP_Codec_Callback_t dsp_process);

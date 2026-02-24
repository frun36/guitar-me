#pragma once

#include <stdint.h>

void DSP_Init();
void DSP_Process(uint16_t* in, uint16_t* out);
void DSP_UpdateParameters(int delta);

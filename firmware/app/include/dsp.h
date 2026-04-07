#pragma once

#include <stdbool.h>
#include <stdint.h>

void DSP_Init(void);
void DSP_Process(uint16_t* in, uint16_t* out);
void DSP_UpdateParameters(int delta, bool btn);

#pragma once

#include <stm32f303x8.h> // symbols for ARM math

#include "arm_math.h"

typedef struct {
    // User defined parameters
    float32_t f_c; // Cutoff frequency (Hz)
    float32_t q; // Filter Q factor
    float32_t g; // Gain (dB)
    // For coefficient computation
    float32_t a;
    float32_t w_0;
    float32_t alpha;
} FX_EQ_Peak_t;

void FX_EQ_Peak_Init(
    FX_EQ_Peak_t* p,
    float32_t f_c,
    float32_t q,
    float32_t g,
    q15_t* coeffs
);

void FX_EQ_Peak_UpdateParameters(
    FX_EQ_Peak_t* p,
    float32_t df_c,
    float32_t dq,
    float32_t dg,
    q15_t* coeffs
);

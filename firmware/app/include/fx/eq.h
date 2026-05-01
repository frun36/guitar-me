#pragma once

#include "arm_math_types.h"

typedef enum { FX_EQ_PEAK, FX_EQ_LOW_SHELF, FX_EQ_HIGH_SHELF } FX_EQ_Shape_t;

typedef struct FX_EQ_s {
    // User defined parameters
    float32_t f_c; // Cutoff frequency
    float32_t q; // Filter quality
    float32_t g; // Gain (dB)
    // For coefficient computation
    float32_t a;
    float32_t w_0;
    float32_t alpha;
    float32_t* coeffs;
    void (*update_coeffs)(struct FX_EQ_s*);
} FX_EQ_t;

void FX_EQ_Init(
    FX_EQ_t* f,
    FX_EQ_Shape_t shape,
    float32_t f_c,
    float32_t q,
    float32_t g,
    float32_t* coeffs
);

void FX_EQ_Update(
    FX_EQ_t* f,
    float32_t df_c,
    float32_t dq,
    float32_t dg
);

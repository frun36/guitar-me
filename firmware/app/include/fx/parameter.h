#pragma once

#include "arm_math_types.h"

typedef enum { FX_PARAMETER_LIN, FX_PARAMETER_EXP } FX_Parameter_StepType_t;

typedef struct {
    float32_t min;
    FX_Parameter_StepType_t step_type;
    float32_t step;
    float32_t max;
    float32_t* val;
} FX_Parameter_t;

void FX_Parameter_Init(
    FX_Parameter_t* p,
    float32_t min,
    FX_Parameter_StepType_t step_type,
    float32_t step,
    float32_t max,
    float32_t* val
);

void FX_Parameter_Update(FX_Parameter_t* p, int steps);

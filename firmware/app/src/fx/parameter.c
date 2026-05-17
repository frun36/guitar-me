#include "fx/parameter.h"

#include <math.h>

void FX_Parameter_Init(
    FX_Parameter_t* p,
    float32_t min,
    FX_Parameter_StepType_t step_type,
    float32_t step,
    float32_t max,
    float32_t* val
) {
    p->min = min;
    p->step_type = step_type;
    p->step = step;
    p->max = max;
    p->val = val;
}

void FX_Parameter_Update(FX_Parameter_t* p, int steps) {
    switch (p->step_type) {
        case FX_PARAMETER_LIN:
            *p->val += steps * p->step;
            break;
        case FX_PARAMETER_EXP:
            *p->val *= powf(p->step, steps);
            break;
    }

    if (*p->val < p->min)
        *p->val = p->min;

    if (*p->val > p->max)
        *p->val = p->max;
}

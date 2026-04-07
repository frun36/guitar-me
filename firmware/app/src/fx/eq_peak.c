#include "fx/eq_peak.h"
#include <math.h>
#include <stm32f303x8.h> // symbols for ARM math

#include "config.h"

#define LN10_OVER_40 0.05756463f

static void UpdateComputationParameters(FX_EQ_Peak_t* p) {
    p->a = expf(p->g * LN10_OVER_40);
    p->w_0 = 2.f * PI * p->f_c / (SAMPLE_FREQ >> DECIMATE_EXP);
    p->alpha = arm_sin_f32(p->w_0) / (2. * p->q);
}

static void UpdateCoefficients(FX_EQ_Peak_t* p, float32_t* coeffs) {
    float32_t a_0 = 1.f + p->alpha / p->a;
    float32_t cos_w_0 = arm_cos_f32(p->w_0);

    coeffs[0] = (1.f + p->alpha * p->a) / a_0; // b_0
    coeffs[1] = (-2.f * cos_w_0) / a_0; // b_1
    coeffs[2] = (1.f - p->alpha * p->a) / a_0; // b_2

    coeffs[3] = -(-2.f * cos_w_0) / a_0; // a_1
    coeffs[4] = -(1.f - p->alpha / p->a) / a_0; // a_2
}

void FX_EQ_Peak_Init(
    FX_EQ_Peak_t* p,
    float32_t f_c,
    float32_t q,
    float32_t g,
    float32_t* coeffs
) {
    p->f_c = f_c;
    p->q = q;
    p->g = g;

    UpdateComputationParameters(p);
    UpdateCoefficients(p, coeffs);
}

void FX_EQ_Peak_UpdateParameters(
    FX_EQ_Peak_t* p,
    float32_t df_c,
    float32_t dq,
    float32_t dg,
    float32_t* coeffs
) {
    p->f_c += df_c;
    p->q += dq;
    p->g += dg;
    UpdateComputationParameters(p);
    UpdateCoefficients(p, coeffs);
}

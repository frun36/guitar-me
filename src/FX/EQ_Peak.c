#include "FX/EQ_Peak.h"
#include <math.h>
#include <stm32f303x8.h> // symbols for ARM math

#include "Config.h"
#include "arm_math.h"

#define LN10_OVER_40 0.05756463f

static void UpdateComputationParameters(FX_EQ_Peak_t* p) {
    p->a = expf(p->g * LN10_OVER_40);
    p->w_0 = 2.f * PI * p->f_c / (SAMPLE_FREQ >> DECIMATE_EXP);
    p->alpha = sinf(p->w_0) / (2. * p->q);
}

static void UpdateCoefficients(FX_EQ_Peak_t* p, q15_t* coeffs) {
    float32_t newCoeffs[6]; // b_0, 0, b_1, b_2, a_1, a_2
    float32_t a_0 = 1.f + p->alpha / p->a;
    float32_t cos_w_0 = cosf(p->w_0);

    // assuming post-shift of 1
    newCoeffs[0] = 0.5 * (1.f + p->alpha * p->a) / a_0; // b_0
    newCoeffs[1] = 0;
    newCoeffs[2] = 0.5 * (-2.f * cos_w_0) / a_0; // b_1
    newCoeffs[3] = 0.5 * (1.f - p->alpha * p->a) / a_0; // b_2

    newCoeffs[4] = -0.5 * (-2.f * cos_w_0) / a_0; // a_1
    newCoeffs[5] = -0.5 * (1.f - p->alpha / p->a) / a_0; // a_2
    arm_float_to_q15(newCoeffs, coeffs, 6);
}

void FX_EQ_Peak_Init(
    FX_EQ_Peak_t* p,
    float32_t f_c,
    float32_t q,
    float32_t g,
    q15_t* coeffs
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
    q15_t* coeffs
) {
    p->f_c += df_c;
    p->q += dq;
    p->g += dg;
    UpdateComputationParameters(p);
    UpdateCoefficients(p, coeffs);
}

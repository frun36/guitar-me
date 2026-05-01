#include "fx/eq.h"

#include <math.h>
#include <stm32f303x8.h> // symbols for ARM math

#include "config.h"
#include "dsp/fast_math_functions.h"

#define LN10_OVER_40 0.05756463f

static void UpdateComputationParameters(FX_EQ_t* f) {
    f->a = expf(f->g * LN10_OVER_40);
    f->w_0 = 2.f * PI * f->f_c / (SAMPLE_FREQ >> DECIMATE_EXP);
    f->alpha = arm_sin_f32(f->w_0) / (2. * f->q);
}

static void Neutral_UpdateCoefficients(FX_EQ_t* f) {
    f->coeffs[0] = 1.f;
    f->coeffs[1] = 0.f;
    f->coeffs[2] = 0.f;

    f->coeffs[3] = 0.f;
    f->coeffs[4] = 0.f;
}

static void Peak_UpdateCoefficients(FX_EQ_t* f) {
    float32_t a_0 = 1.f + f->alpha / f->a;
    float32_t cos_w_0 = arm_cos_f32(f->w_0);

    f->coeffs[0] = (1.f + f->alpha * f->a) / a_0; // b_0
    f->coeffs[1] = (-2.f * cos_w_0) / a_0; // b_1
    f->coeffs[2] = (1.f - f->alpha * f->a) / a_0; // b_2

    f->coeffs[3] = -(-2.f * cos_w_0) / a_0; // a_1
    f->coeffs[4] = -(1.f - f->alpha / f->a) / a_0; // a_2
}

static void LowShelf_UpdateCoefficients(FX_EQ_t* f) {
    float32_t cos_w_0 = arm_cos_f32(f->w_0);
    float32_t a_p_1 = f->a + 1;
    float32_t a_m_1 = f->a - 1;
    float32_t a_p_1_cos = a_p_1 * cos_w_0;
    float32_t a_m_1_cos = a_m_1 * cos_w_0;
    float32_t _2_sqrt_a_alpha = 2 * sqrtf(f->a) * f->alpha;

    float32_t a_0 = a_p_1 + a_m_1_cos + _2_sqrt_a_alpha;

    f->coeffs[0] = (f->a * (a_p_1 - a_m_1_cos + _2_sqrt_a_alpha)) / a_0; // b_0
    f->coeffs[1] = (2 * f->a * (a_m_1 - a_p_1_cos)) / a_0; // b_1
    f->coeffs[2] = (f->a * (a_p_1 - a_m_1_cos - _2_sqrt_a_alpha)) / a_0; // b_2

    f->coeffs[3] = -(-2 * (a_m_1 + a_p_1_cos)) / a_0; // a_1
    f->coeffs[4] = -(a_p_1 + a_m_1_cos - _2_sqrt_a_alpha) / a_0; // a_2
}

static void HighShelf_UpdateCoefficients(FX_EQ_t* f) {
    float32_t cos_w_0 = arm_cos_f32(f->w_0);
    float32_t a_p_1 = f->a + 1;
    float32_t a_m_1 = f->a - 1;
    float32_t a_p_1_cos = a_p_1 * cos_w_0;
    float32_t a_m_1_cos = a_m_1 * cos_w_0;
    float32_t _2_sqrt_a_alpha = 2 * sqrtf(f->a) * f->alpha;

    float32_t a_0 = a_p_1 - a_m_1_cos + _2_sqrt_a_alpha;

    f->coeffs[0] = (f->a * (a_p_1 + a_m_1_cos + _2_sqrt_a_alpha)) / a_0; // b_0
    f->coeffs[1] = (-2 * f->a * (a_m_1 + a_p_1_cos)) / a_0; // b_1
    f->coeffs[2] = (f->a * (a_p_1 + a_m_1_cos - _2_sqrt_a_alpha)) / a_0; // b_2

    f->coeffs[3] = -(2 * (a_m_1 - a_p_1_cos)) / a_0; // a_1
    f->coeffs[4] = -(a_p_1 - a_m_1_cos - _2_sqrt_a_alpha) / a_0; // a_2
}

void FX_EQ_Init(
    FX_EQ_t* f,
    FX_EQ_Shape_t shape,
    float32_t f_c,
    float32_t q,
    float32_t g,
    float32_t* coeffs
) {
    f->f_c = f_c;
    f->q = q;
    f->g = g;
    f->coeffs = coeffs;
    switch (shape) {
        case FX_EQ_PEAK:
            f->update_coeffs = Peak_UpdateCoefficients;
            break;
        case FX_EQ_LOW_SHELF:
            f->update_coeffs = LowShelf_UpdateCoefficients;
            break;
        case FX_EQ_HIGH_SHELF:
            f->update_coeffs = HighShelf_UpdateCoefficients;
            break;
        default:
            f->update_coeffs = Neutral_UpdateCoefficients;
    }

    UpdateComputationParameters(f);
    f->update_coeffs(f);
}

void FX_EQ_Update(FX_EQ_t* f, float32_t df_c, float32_t dq, float32_t dg) {
    f->f_c += df_c;
    f->q += dq;
    f->g += dg;
    UpdateComputationParameters(f);
    f->update_coeffs(f);
}

#include "fx/eq_w_lut.h"

float32_t FX_EQ_ComputeFrequencyResponse(FX_EQ_t* f, size_t i) {
    float32_t cos_w = g_cos_w[i];
    float32_t sin_w = g_sin_w[i];

    float32_t cos_2w = g_cos_2w[i];
    float32_t sin_2w = g_sin_2w[i];

    float32_t b0 = f->coeffs[0];
    float32_t b1 = f->coeffs[1];
    float32_t b2 = f->coeffs[2];
    float32_t a1 = -f->coeffs[3];
    float32_t a2 = -f->coeffs[4];

    float32_t num_r = b0 + (b1 * cos_w) + (b2 * cos_2w);
    float32_t num_i = -((b1 * sin_w) + (b2 * sin_2w));

    float32_t den_r = 1.0 + a1 * cos_w + a2 * cos_2w;
    float32_t den_i = -((a1 * sin_w) + (a2 * sin_2w));

    // 20log10(x) = 10log10(x^2), avoids sqrt for magnitude
    return 10.f
        * log10f(
               (num_r * num_r + num_i * num_i) / (den_r * den_r + den_i * den_i)
        );
}

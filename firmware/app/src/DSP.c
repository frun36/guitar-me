#include "DSP.h"

#include <stm32f303x8.h> // symbols for ARM math

#include "config.h"
#include "FX/EQ_Peak.h"
#include "LED.h"
#include "OLED.h"
#include "arm_math.h"
#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_utils.h"

// --- Data ---

#define IN_N (BUF_SIZE_HALF)
#define N (IN_N >> DECIMATE_EXP)

static float32_t s_buffer[N];
static q15_t s_tmp_buffer[N];

// --- Decimator ---

#if (DECIMATE_EXP == 1 && DECIMATOR_TAPS == 31 && INTERPOLATOR_TAPS == 32)
static const q15_t DECIMATOR_COEFFS[DECIMATOR_TAPS] = {
    -56,  0,     96,  0,     -221,  0,     462, 0,     -878, 0,    1609,
    0,    -3176, 0,   10342, 16410, 10342, 0,   -3176, 0,    1609, 0,
    -878, 0,     462, 0,     -221,  0,     96,  0,     -56,
};

static const q15_t INTERPOLATOR_COEFFS[INTERPOLATOR_TAPS] = {
    -76,   -91,   128,   192,  -287,  -418,  592,  818,   -1110, -1490, 1996,
    2699,  -3755, -5571, 9647, 29495, 29495, 9647, -5571, -3755, 2699,  1996,
    -1490, -1110, 818,   592,  -418,  -287,  192,  128,   -91,   -76,
};

#elif (DECIMATE_EXP == 2 && DECIMATOR_TAPS == 31 && INTERPOLATOR_TAPS == 32)

static const q15_t DECIMATOR_COEFFS[DECIMATOR_TAPS] = {
    -39,  -67,  -68,  0,    156,  324,  327,  0,    -621, -1189, -1139,
    0,    2249, 5022, 7322, 8216, 7322, 5022, 2249, 0,    -1139, -1189,
    -621, 0,    327,  324,  156,  0,    -68,  -67,  -39,
};

static const q15_t INTERPOLATOR_COEFFS[INTERPOLATOR_TAPS] = {
    -83,   -238,  -336,  -208,  311,   1092,  1547,  886,   -1203, -3897, -5219,
    -2923, 4067,  14568, 25226, 31946, 31946, 25226, 14568, 4067,  -2923, -5219,
    -3897, -1203, 886,   1547,  1092,  311,   -208,  -336,  -238,  -83,
};

#endif

#if DECIMATE_EXP
static q15_t s_decimator_state[DECIMATOR_TAPS + IN_N - 1];
static arm_fir_decimate_instance_q15 s_decimator;

static q15_t
    s_interpolator_state[(INTERPOLATOR_TAPS << DECIMATE_EXP) + IN_N - 1];
static arm_fir_interpolate_instance_q15 s_interpolator;
#endif

static void PrepareInput(q15_t* in) {
#if DECIMATE_EXP
    arm_offset_q15((q15_t*)in, -2048, (q15_t*)in, IN_N);
    arm_shift_q15((q15_t*)in, 4, (q15_t*)in, IN_N);

    // second half of float buffer
    arm_fir_decimate_q15(&s_decimator, (q15_t*)in, (q15_t*)s_tmp_buffer, IN_N);
    arm_q15_to_float((q15_t*)s_tmp_buffer, s_buffer, N);
#else
    arm_offset_q15((q15_t*)in, -2048, (q15_t*)in, IN_N);
    arm_shift_q15(s_buffer, 4, (q15_t*)in, IN_N);
    arm_q15_to_float((q15_t*)in, s_buffer, N);
#endif
}

static void PrepareOutput(q15_t* out) {
#if DECIMATE_EXP
    // first half of float buffer
    arm_float_to_q15(s_buffer, (q15_t*)s_tmp_buffer, N);
    arm_fir_interpolate_q15(
        &s_interpolator,
        (q15_t*)s_tmp_buffer,
        (q15_t*)out,
        N
    );

    arm_shift_q15((q15_t*)out, -4, (q15_t*)out, IN_N);
    arm_offset_q15((q15_t*)out, 2048, (q15_t*)out, IN_N);
#else
    arm_float_to_q15(s_buffer, (q15_t*)out, N);
    arm_shift_q15((q15_t*)out, -4, (q15_t*)out, IN_N);
    arm_offset_q15((q15_t*)out, 2048, (q15_t*)out, IN_N);
#endif
}

// --- FX ---

static FX_EQ_Peak_t s_peak;
static float32_t s_filter_coeffs[5];
static float32_t s_filter_state[4];
static arm_biquad_casd_df1_inst_f32 s_filter;

static int CheckClipping() {
    size_t clipped = 0;
    for (size_t i = 0; i < N; i++) {
        if (s_buffer[i] < -0.98f || s_buffer[i] > 0.98f)
            clipped++;
        else
            clipped = 0;
        if (clipped >= 7)
            return 1;
    }
    return 0;
}

void DSP_Init() {
    // Processing LED
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOA);
    LL_GPIO_InitTypeDef led = {
        .Pin = LL_GPIO_PIN_11,
        .Mode = LL_GPIO_MODE_OUTPUT,
        .Speed = LL_GPIO_SPEED_FREQ_LOW,
        .OutputType = LL_GPIO_OUTPUT_PUSHPULL,
        .Pull = LL_GPIO_PULL_NO
    };
    LL_GPIO_Init(GPIOA, &led);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);

#if DECIMATE_EXP
    arm_fir_decimate_init_q15(
        &s_decimator,
        DECIMATOR_TAPS,
        1 << DECIMATE_EXP,
        (q15_t*)DECIMATOR_COEFFS,
        s_decimator_state,
        IN_N
    );

    arm_fir_interpolate_init_q15(
        &s_interpolator,
        1 << DECIMATE_EXP,
        INTERPOLATOR_TAPS,
        (q15_t*)INTERPOLATOR_COEFFS,
        s_interpolator_state,
        N
    );
#endif

    FX_EQ_Peak_Init(&s_peak, 800, 4, 0, s_filter_coeffs);
    arm_biquad_cascade_df1_init_f32(
        &s_filter,
        1,
        s_filter_coeffs,
        s_filter_state
    );
}

void DSP_Process(uint16_t* in, uint16_t* out) {
    LL_GPIO_SetOutputPin(GPIOA, LL_GPIO_PIN_11);
    PrepareInput((q15_t*)in);

    if (CheckClipping())
        LED_On();
    else
        LED_Off();

    arm_biquad_cascade_df1_f32(&s_filter, s_buffer, s_buffer, N);

    PrepareOutput((q15_t*)out);
    LL_GPIO_ResetOutputPin(GPIOA, LL_GPIO_PIN_11);
}

void DSP_UpdateParameters(int delta, bool btn) {
    static bool mode = false;
    if (delta == 0 && !btn)
        return;
    // clear old line
    for (uint32_t i = 0; i < OLED_WIDTH; i++) {
        OLED_SetPixel(i, (OLED_HEIGHT >> 1) + s_peak.g, false);
    }

    mode = btn ? !mode : mode;

    FX_EQ_Peak_UpdateParameters(&s_peak, (btn ? (mode ? 500 : -500) : 0), 0, delta, s_filter_coeffs);

    // draw new line
    for (uint32_t i = 0; i < OLED_WIDTH; i++) {
        OLED_SetPixel(i, (OLED_HEIGHT >> 1) + s_peak.g, true);
    }
    OLED_Flush();
}

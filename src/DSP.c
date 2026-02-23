#include "DSP.h"

#include <stm32f303x8.h> // symbols for ARM math

#include "Config.h"
#include "LED.h"
#include "arm_math.h"

#define Q15(x) ((q15_t)((x) * 32768.0f))

#define IN_N (BUF_SIZE_HALF)
#define N (IN_N >> DECIMATE_EXP)

static q15_t Buffer[N];

#if DECIMATE_EXP == 1 && DECIMATOR_TAPS == 31 && INTERPOLATOR_TAPS == 32
static const q15_t DECIMATOR_COEFFS[DECIMATOR_TAPS] = {
    -56,  0,     96,  0,     -221,  0,     462, 0,     -878, 0,    1609,
    0,    -3176, 0,   10342, 16410, 10342, 0,   -3176, 0,    1609, 0,
    -878, 0,     462, 0,     -221,  0,     96,  0,     -56,
};

static const q15_t INTERPOLATOR_COEFFS[INTERPOLATOR_TAPS]= {
    -76,   -91,   128,   192,  -287,  -418,  592,  818,   -1110, -1490, 1996,
    2699,  -3755, -5571, 9647, 29495, 29495, 9647, -5571, -3755, 2699,  1996,
    -1490, -1110, 818,   592,  -418,  -287,  192,  128,   -91,   -76,
};
#endif

#if DECIMATE_EXP
static q15_t DecimatorState[DECIMATOR_TAPS + IN_N - 1];
static arm_fir_decimate_instance_q15 Decimator;

static q15_t InterpolatorState[(INTERPOLATOR_TAPS << DECIMATE_EXP) + IN_N - 1];
static arm_fir_interpolate_instance_q15 Interpolator;
#endif

static q15_t FILTER_COEFFS[6] = {17174, 0, -32065, 15068, 32065, -15857};
static q15_t FilterState[4];
static arm_biquad_casd_df1_inst_q15 Filter;

static int CheckClipping() {
    size_t clipped = 0;
    for (size_t i = 0; i < N; i++) {
        if (Buffer[i] < Q15(-0.98f) || Buffer[i] > Q15(0.98f))
            clipped++;
        else
            clipped = 0;
        if (clipped >= 7)
            return 1;
    }
    return 0;
}

void DSP_Init() {
#if DECIMATE_EXP
    arm_fir_decimate_init_q15(
        &Decimator,
        DECIMATOR_TAPS,
        1 << DECIMATE_EXP,
        (q15_t*)DECIMATOR_COEFFS,
        DecimatorState,
        IN_N
    );

    arm_fir_interpolate_init_q15(
        &Interpolator,
        1 << DECIMATE_EXP,
        INTERPOLATOR_TAPS,
        (q15_t*)INTERPOLATOR_COEFFS,
        InterpolatorState,
        N
    );
#endif

    arm_biquad_cascade_df1_init_q15(&Filter, 1, FILTER_COEFFS, FilterState, 1);
}

void DSP_Process(uint16_t* in, uint16_t* out) {
#if DECIMATE_EXP
    arm_offset_q15((q15_t*)in, -2048, (q15_t*)in, IN_N);
    arm_shift_q15((q15_t*)in, 4, (q15_t*)in, IN_N);
    arm_fir_decimate_q15(&Decimator, (q15_t*)in, Buffer, IN_N);
#else
    arm_offset_q15((q15_t*)in, -2048, Buffer, IN_N);
    arm_shift_q15(Buffer, 4, Buffer, IN_N);
#endif

    if (CheckClipping())
        LED_On();
    else
        LED_Off();

    arm_biquad_cascade_df1_q15(&Filter, Buffer, Buffer, N);

#if DECIMATE_EXP
    arm_fir_interpolate_q15(&Interpolator, Buffer, (q15_t*)out, N);
    arm_shift_q15((q15_t*)out, -4, (q15_t*)out, IN_N);
    arm_offset_q15((q15_t*)out, 2048, (q15_t*)out, IN_N);
#else
    arm_shift_q15(Buffer, -4, Buffer, IN_N);
    arm_offset_q15(Buffer, 2048, (q15_t*)out, IN_N);
#endif
}

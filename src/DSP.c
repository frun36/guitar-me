#include "DSP.h"

#include <stm32f303x8.h> // symbols for ARM math

#include "Config.h"
#include "LED.h"
#include "arm_math.h"

#define Q15(x) ((q15_t)((x) * 32768.0f))

#define N BUF_SIZE_HALF

static q15_t DSP_Buf[N] = {};

int CheckClipping() {
    size_t clipped = 0;
    for (size_t i = 0; i < N; i++) {
        if (DSP_Buf[i] < Q15(-0.98f) || DSP_Buf[i] > Q15(0.98f))
            clipped++;
        else
            clipped = 0;
        if (clipped >= 7)
            return 1;
    }
    return 0;
}

void DSP_Process(const uint16_t* in, uint16_t* out) {
    arm_offset_q15((q15_t*)in, -2048, DSP_Buf, N);
    arm_shift_q15(DSP_Buf, 4, DSP_Buf, N);

    if (CheckClipping())
        LED_On();
    else
        LED_Off();

    arm_shift_q15(DSP_Buf, -4, DSP_Buf, N);
    arm_offset_q15(DSP_Buf, 2048, (q15_t*)out, N);
}

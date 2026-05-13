#include "display.h"

#include "dsp.h"
#include "oled.h"

void Display_UpdateCurve(void) {
    static uint8_t s_line_shape[OLED_WIDTH];
    for (uint32_t i = 0; i < OLED_WIDTH; i++) {
        OLED_SetPixel(i, s_line_shape[i], false);
    }
    DSP_GetFrequencyResponseCurve(s_line_shape);
    for (uint32_t i = 0; i < OLED_WIDTH; i++) {
        OLED_SetPixel(i, s_line_shape[i], true);
    }
    OLED_Flush();
}

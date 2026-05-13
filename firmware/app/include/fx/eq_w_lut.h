#include "arm_math_types.h"
#include "oled.h"

#if OLED_WIDTH == 128

// Each pixel on the x axis corresponds to a frequency from 20 to 20000Hz,
// spaced geometrically. The LUT contains values useful for frequency response computation.
extern const float32_t g_cos_w[OLED_WIDTH];
extern const float32_t g_sin_w[OLED_WIDTH];
extern const float32_t g_cos_2w[OLED_WIDTH];
extern const float32_t g_sin_2w[OLED_WIDTH];

#endif

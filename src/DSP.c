#include "DSP.h"

#include <string.h>

#include "Config.h"

void DSP_Process(const uint16_t* in, uint16_t* out) {
    memcpy(out, in, BUF_SIZE_HALF * sizeof(uint16_t));
}

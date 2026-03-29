#include <stdio.h>

#include "Button.h"
#include "DSP.h"
#include "Encoder.h"
#include "LED.h"
#include "OLED.h"
#include "bsp.h"
#include "bsp_codec.h"

int main(void) {
    BSP_Init();
    printf("Helou\r\n");

    LED_Init();
    Button_Init();
    Encoder_Init();
    OLED_Init();

    DSP_Init();
    BSP_Codec_Init(DSP_Process);

    while (1) {
        DSP_UpdateParameters(
            Encoder_GetDelta(),
            Button_GetEvent() == BTN_PRESSED
        );
    }
}

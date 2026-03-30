#include <stdio.h>

#include "Button.h"
#include "DSP.h"
#include "Encoder.h"
#include "bsp.h"
#include "bsp_codec.h"
#include "bsp_led.h"
#include "oled.h"

int main(void) {
    BSP_Init();
    printf("Helou\r\n");

    BSP_LED_Init();
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

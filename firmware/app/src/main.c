#include <stdio.h>

#include "bsp.h"
#include "bsp_codec.h"
#include "bsp_led.h"
#include "control.h"
#include "dsp.h"
#include "oled.h"

void TickHandler(uint32_t ms) {
    Control_Tick(ms);
}

int main(void) {
    BSP_Init();
    BSP_SetTickHandler(TickHandler);
    printf("Helou\r\n");

    OLED_Init();

    BSP_LED_Init();
    Control_Init();

    DSP_Init();
    BSP_Codec_Init(DSP_Process);

    while (1) {
        int32_t encoder_delta = 0;
        bool was_btn_pressed = false;
        Control_Event_t evt;
        do {
            evt = Control_PopEvent();
            if (evt.type == EVT_BTN1_PRESSED)
                was_btn_pressed = true;
            else if (evt.type == EVT_ENC1_MOVED)
                encoder_delta += evt.data;
        } while (evt.type != EVT_NONE);

        DSP_UpdateParameters(encoder_delta, was_btn_pressed);
    }
}

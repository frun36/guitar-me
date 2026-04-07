#include <stdio.h>

#include "bsp.h"
#include "bsp_codec.h"
#include "bsp_led.h"
#include "control.h"
#include "dsp.h"
#include "oled.h"

int main(void) {
    BSP_Init();
    printf("Helou\r\n");

    OLED_Init();

    BSP_LED_Init();
    Control_Init();

    DSP_Init();
    BSP_Codec_Init(DSP_Process);

    uint32_t last_control_check_time = 0;
    uint32_t curr_time = 0;

    while (1) {
        curr_time = BSP_GetTime();
        if (curr_time - last_control_check_time >= 10) {
            Control_Tick();
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
}

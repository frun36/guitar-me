#include "control.h"

#include "bsp_control.h"
#include "ring_buffer.h"

#define RB_SIZE 8

static RingBuffer_t s_rb;
static Control_Event_t s_evt_buff[RB_SIZE];
static int32_t s_btn1_active_count = 0;
static int32_t s_enc1_count = 0;

void Control_Init(void) {
    BSP_Control_Init();
    RingBuffer_Init(&s_rb, s_evt_buff, RB_SIZE, sizeof(Control_Event_t));
}

Control_Event_t Control_PopEvent(void) {
    Control_Event_t evt = {
        .type = EVT_NONE,
        .data = 0,
    };
    // unchanged if empty
    RingBuffer_Pop(&s_rb, &evt);
    return evt;
}

static bool ButtonFsm(bool (*read_btn)(void), int32_t* active_count) {
    bool state = read_btn();

    if (state)
        *active_count += 1;
    else
        *active_count = 0;

    if (*active_count == 3)
        return true;
    else
        return false;
}

static int32_t EncoderFsm(int32_t (*read_enc)(void), int32_t* prev_count) {
    int32_t new_count = read_enc();
    int32_t diff = new_count - *prev_count;
    *prev_count = new_count;
    return diff;
}

void Control_Tick(uint32_t ms) {
    // Every 16 ms
    if (ms % 16)
        return;

    Control_Event_t evt;

    // BTN1
    if (ButtonFsm(BSP_Control_ReadBtn1, &s_btn1_active_count)) {
        evt.type = EVT_BTN1_PRESSED;
        RingBuffer_Push(&s_rb, &evt);
    }

    // ENC1
    if ((evt.data = EncoderFsm(BSP_Control_ReadEnc1, &s_enc1_count)) != 0) {
        evt.type = EVT_ENC1_MOVED;
        RingBuffer_Push(&s_rb, &evt);
    }
}

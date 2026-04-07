#include <stdint.h>

typedef struct {
    enum {
        EVT_NONE,
        EVT_BTN1_PRESSED,
        EVT_ENC1_MOVED,
    } type;

    int32_t data;
} Control_Event_t;

void Control_Init(void);
void Control_Tick(void);

Control_Event_t Control_PopEvent(void);

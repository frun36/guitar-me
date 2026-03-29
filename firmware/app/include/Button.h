#pragma once

typedef enum { BTN_NONE = 0, BTN_PRESSED } Button_Event_t;

void Button_Init();

Button_Event_t Button_GetEvent();

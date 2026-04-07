#pragma once

typedef enum { LED1, LED2 } BSP_LED_t;

void BSP_LED_Init(void);
void BSP_LED_On(BSP_LED_t led);
void BSP_LED_Off(BSP_LED_t led);
void BSP_LED_Toggle(BSP_LED_t led);

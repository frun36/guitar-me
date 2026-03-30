#pragma once

#include <stdint.h>

typedef void (*BSP_TickHandler_t)(uint32_t);

void BSP_Init(void);

uint32_t BSP_GetTime(void);
void BSP_SetTickHandler(BSP_TickHandler_t);
void BSP_Delay(uint32_t ms);

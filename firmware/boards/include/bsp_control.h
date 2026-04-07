#pragma once

#include <stdbool.h>
#include <stdint.h>

void BSP_Control_Init(void);

bool BSP_Control_ReadBtn1(void);
int32_t BSP_Control_ReadEnc1(void);

#pragma once

#include <stdint.h>
#include <stdbool.h>

#define OLED_WIDTH 128
#define OLED_HEIGHT 64

void OLED_Init();
void OLED_Flush();
void OLED_SetPixel(uint32_t x, uint32_t y, bool on);

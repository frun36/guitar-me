#include "oled.h"

#include <stdbool.h>

#include "bsp.h"
#include "bsp_i2c.h"

#define SSD1306_I2C_ADDR 0x78

static uint8_t s_buff[128][8];

static void WriteCommand(uint8_t byte);
static void ResetWindow(void);

static void WriteCommand(uint8_t byte) {
    BSP_I2C_MemWrite(SSD1306_I2C_ADDR, 0x00, (uint8_t[]) {byte}, 1);
}

static void ResetWindow(void) {
    WriteCommand(0x22); // Page addresses:
    WriteCommand(0x00); // all
    WriteCommand(0x07); // all

    WriteCommand(0x21); // Column addresses:
    WriteCommand(0x00); // all
    WriteCommand(0x7F); // all
}

void OLED_Init(void) {
    BSP_I2C_Init();

    WriteCommand(0xAE); // Off

    WriteCommand(0x8D); // Charge pump config
    WriteCommand(0x14); // Charge pump on

    WriteCommand(0x20); // Addressing mode:
    WriteCommand(0x01); // vertical
    ResetWindow();

    WriteCommand(0xAF); // On

    WriteCommand(0xA5); // Flash white
    BSP_Delay(100);
    OLED_Flush();
    WriteCommand(0xA4); // Reset to RAM
}

void OLED_SetPixel(uint32_t x, uint32_t y, bool on) {
#if OLED_HEIGHT == 32
    y <<= 1;
#endif
    x = 127 - x;
    uint8_t mask = (1 << (y & 0x7));
    if (on) {
        s_buff[x][y >> 3] |= mask;
    } else {
        s_buff[x][y >> 3] &= ~mask;
    }
}

void OLED_Flush(void) {
    ResetWindow();
    BSP_I2C_MemWrite(SSD1306_I2C_ADDR, 0x40, (uint8_t*)s_buff, sizeof(s_buff));
}

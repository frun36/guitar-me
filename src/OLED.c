#include "OLED.h"

#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_i2c.h"
#include "stm32f3xx_ll_utils.h"

#define SSD1306_I2C_ADDR 0x78
#define PAGE_OFFSET 2

// 32px display has every other bit wired...
static uint8_t s_buff[128][8] = {};

static void WriteCommand(uint8_t byte);
static void ResetWindow();

static void WriteCommand(uint8_t byte) {
    LL_I2C_HandleTransfer(
        I2C1,
        SSD1306_I2C_ADDR,
        LL_I2C_ADDRSLAVE_7BIT,
        2,
        LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_WRITE
    );
    while (LL_I2C_IsActiveFlag_TXIS(I2C1) == 0)
        ;
    LL_I2C_TransmitData8(I2C1, 0x00); // Command
    while (LL_I2C_IsActiveFlag_TXIS(I2C1) == 0)
        ;
    LL_I2C_TransmitData8(I2C1, byte);
    while (LL_I2C_IsActiveFlag_STOP(I2C1) == 0)
        ;

    LL_I2C_ClearFlag_STOP(I2C1);
}

static void ResetWindow() {
    WriteCommand(0x22); // Page addresses:
    WriteCommand(0x00); // all
    WriteCommand(0x07); // all

    WriteCommand(0x21); // Column addresses:
    WriteCommand(0x00); // all
    WriteCommand(0x7F); // all
}

void OLED_Flush(void) {
    ResetWindow();
    for (uint32_t col = 0; col < OLED_WIDTH; col++) {
        LL_I2C_HandleTransfer(
            I2C1,
            0x78,
            LL_I2C_ADDRSLAVE_7BIT,
            9,
            LL_I2C_MODE_AUTOEND,
            LL_I2C_GENERATE_START_WRITE
        );

        while (!LL_I2C_IsActiveFlag_TXIS(I2C1))
            ;
        LL_I2C_TransmitData8(I2C1, 0x40); // Data

        for (uint32_t i = 0; i < 8; i++) {
            while (!LL_I2C_IsActiveFlag_TXIS(I2C1))
                ;
            LL_I2C_TransmitData8(I2C1, s_buff[col][i]);
        }

        while (!LL_I2C_IsActiveFlag_STOP(I2C1))
            ;
        LL_I2C_ClearFlag_STOP(I2C1);
    }
}

void OLED_Init() {
    LL_AHB1_GRP1_EnableClock(LL_AHB1_GRP1_PERIPH_GPIOB);
    LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_I2C1);

    LL_GPIO_InitTypeDef gpio = {
        .Pin = LL_GPIO_PIN_6 | LL_GPIO_PIN_7,
        .Mode = LL_GPIO_MODE_ALTERNATE,
        .Speed = LL_GPIO_SPEED_FREQ_HIGH,
        .OutputType = LL_GPIO_OUTPUT_OPENDRAIN,
        .Pull = LL_GPIO_PULL_NO,
        .Alternate = LL_GPIO_AF_4,
    };
    LL_GPIO_Init(GPIOB, &gpio);

    LL_I2C_InitTypeDef i2c = {
        .PeripheralMode = LL_I2C_MODE_I2C,
        .Timing = 0x00201D2B, // magic number from CubeMX
        .AnalogFilter = LL_I2C_ANALOGFILTER_ENABLE,
        .DigitalFilter = 0,
        .OwnAddress1 = 0x00,
        .TypeAcknowledge = LL_I2C_ACK,
        .OwnAddrSize = LL_I2C_OWNADDRESS1_7BIT,
    };
    LL_I2C_Disable(I2C1);
    LL_I2C_EnableClockStretching(I2C1);
    LL_I2C_Init(I2C1, &i2c);
    LL_I2C_Enable(I2C1);

    WriteCommand(0xAE); // Off

    WriteCommand(0x8D); // Charge pump config
    WriteCommand(0x14); // Charge pump on

    WriteCommand(0x20); // Addressing mode:
    WriteCommand(0x01); // vertical
    ResetWindow();

    WriteCommand(0xAF); // On

    WriteCommand(0xA5); // Flash white
    LL_mDelay(100);
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

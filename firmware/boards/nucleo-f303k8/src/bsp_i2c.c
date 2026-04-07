#include "bsp_i2c.h"

#include <stdbool.h>
#include <stdint.h>

#include "stm32f3xx_ll_bus.h"
#include "stm32f3xx_ll_gpio.h"
#include "stm32f3xx_ll_i2c.h"

void BSP_I2C_Init(void) {
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
        .Timing = 0x0010020A, // magic number from CubeMX, fast mode 400kHz
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
}

void BSP_I2C_MemWrite(
    uint8_t dev_address,
    uint8_t control_byte,
    uint8_t* data,
    uint32_t n
) {
    // TODO: timeout on waiting loops

    uint32_t n_total = n + 1;
    uint32_t chunk_size = (n_total > 255) ? 255 : n_total;

    while (LL_I2C_IsActiveFlag_BUSY(I2C1))
        ;

    LL_I2C_HandleTransfer(
        I2C1,
        dev_address,
        LL_I2C_ADDRSLAVE_7BIT,
        chunk_size,
        (n_total > 255) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND,
        LL_I2C_GENERATE_START_WRITE
    );

    bool control_byte_sent = false;
    uint32_t data_idx = 0;
    while (n_total > 0) {
        if (LL_I2C_IsActiveFlag_TXIS(I2C1)) {
            if (!control_byte_sent) {
                LL_I2C_TransmitData8(I2C1, control_byte);
                control_byte_sent = true;
            } else {
                LL_I2C_TransmitData8(I2C1, data[data_idx++]);
            }
            n_total--;
        } else if (LL_I2C_IsActiveFlag_TCR(I2C1)) {
            chunk_size = (n_total > 255) ? 255 : n_total;

            LL_I2C_HandleTransfer(
                I2C1,
                dev_address,
                LL_I2C_ADDRSLAVE_7BIT,
                chunk_size,
                (n_total > 255) ? LL_I2C_MODE_RELOAD : LL_I2C_MODE_AUTOEND,
                LL_I2C_GENERATE_NOSTARTSTOP
            );
        }
    }
    while (!LL_I2C_IsActiveFlag_STOP(I2C1))
        ;
    LL_I2C_ClearFlag_STOP(I2C1);
}


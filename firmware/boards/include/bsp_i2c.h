#pragma once

#include <stdint.h>

void BSP_I2C_Init(void);

void BSP_I2C_MemWrite(
    uint8_t dev_address,
    uint8_t control_byte,
    uint8_t* data,
    uint32_t n
);

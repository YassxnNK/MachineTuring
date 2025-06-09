/**
 * @file    i2c_hal.h
 * @brief   Interface HAL I2C pour PIC24
 */

#ifndef I2C_HAL_H
#define I2C_HAL_H

#include <stdint.h>
#include "error_defs.h"

void I2C1_Init(void);
i2c_error_t I2C1_WriteSequence(uint8_t addr, uint8_t mode, const uint8_t* data, uint8_t len);

#endif // I2C_HAL_H

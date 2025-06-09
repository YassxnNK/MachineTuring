/**
 * @file    i2c_hal.c
 * @brief   HAL I2C pour PIC24, avec gestion d'erreurs
 */

#include "i2c_hal.h"
#include <xc.h>

#define OSCCON_IOLOCK_BIT 6

void I2C1_Init(void) {
    __builtin_write_OSCCONL(OSCCON & ~(1 << OSCCON_IOLOCK_BIT));
    RPOR4bits.RP8R = 0b1000; // SCL1
    RPOR4bits.RP9R = 0b1000; // SDA1
    __builtin_write_OSCCONL(OSCCON | (1 << OSCCON_IOLOCK_BIT));

    I2C1BRG = 157;
    I2C1CON = 0x9200;
    I2C1STAT = 0;
}

i2c_error_t I2C1_WriteSequence(uint8_t addr, uint8_t mode, const uint8_t* data, uint8_t len) {
    I2C1CONbits.SEN = 1; while (I2C1CONbits.SEN);
    I2C1TRN = addr; while (I2C1STATbits.TRSTAT); if (I2C1STATbits.ACKSTAT) return I2C_ERR_ADDR;
    I2C1TRN = mode; while (I2C1STATbits.TRSTAT); if (I2C1STATbits.ACKSTAT) return I2C_ERR_CTRL;

    for (uint8_t i = 0; i < len; i++) {
        I2C1TRN = data[i];
        while (I2C1STATbits.TRSTAT);
        if (I2C1STATbits.ACKSTAT) return I2C_ERR_DATA;
    }

    I2C1CONbits.PEN = 1; while (I2C1CONbits.PEN);
    return I2C_OK;
}

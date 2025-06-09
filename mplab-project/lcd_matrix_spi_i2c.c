/**
 * File: spi_driver.c
 * Description: Low-level SPI communication functions for PIC24FJ64GA002
 */

#include <xc.h>
#include <stdint.h>

#define SPI_SUCCESS 0
#define SPI_ERROR_BUSY 1

uint8_t SPI1_Init(void) {
    // Configure SPI1 with Fcy/64, master mode, 8-bit, mode 0 (CKP=0, CKE=1)
    SPI1STAT = 0;
    SPI1CON1 = 0b0000000000110011;
    SPI1CON2 = 0;
    SPI1STATbits.SPIEN = 1;
    return SPI_SUCCESS;
}

uint8_t SPI1_Write(uint8_t data) {
    if (SPI1STATbits.SPITBF) return SPI_ERROR_BUSY;

    SPI1BUF = data;
    while (!SPI1STATbits.SPIRBF);
    volatile uint8_t dummy = SPI1BUF;
    return SPI_SUCCESS;
}

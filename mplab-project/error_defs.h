/**
 * @file    error_defs.h
 * @brief   Définitions des codes d'erreurs par module
 */

#ifndef ERROR_DEFS_H
#define ERROR_DEFS_H

// === I2C Errors ===
typedef enum {
    I2C_OK = 0,
    I2C_ERR_ADDR,
    I2C_ERR_CTRL,
    I2C_ERR_DATA
} i2c_error_t;

// === SPI Errors ===
typedef enum {
    SPI_OK = 0,
    SPI_ERR_INIT,
    SPI_ERR_TRANSFER
} spi_error_t;

// === LCD Driver Errors ===
typedef enum {
    LCD_OK = 0,
    LCD_ERR_I2C,
    LCD_ERR_INIT
} lcd_error_t;

// === Matrix Driver Errors ===
typedef enum {
    MATRIX_OK = 0,
    MATRIX_ERR_SPI,
    MATRIX_ERR_PARAM
} matrix_error_t;

#endif // ERROR_DEFS_H

/**
 * @file    lcd_driver.c
 * @brief   Pilotage du LCD I2C 2x20 avec ST7036i (Newhaven)
 */

#include "lcd_driver.h"
#include "i2c_hal.h"
#include <libpic30.h>

#define LCD_I2C_ADDR     0x78
#define LCD_CMD_MODE     0x00
#define LCD_DATA_MODE    0x40

lcd_error_t lcd_send_cmd(uint8_t cmd) {
    if (I2C1_WriteSequence(LCD_I2C_ADDR, LCD_CMD_MODE, &cmd, 1) != I2C_OK)
        return LCD_ERR_I2C;
    __delay_ms(1);
    return LCD_OK;
}

lcd_error_t lcd_send_data(uint8_t data) {
    if (I2C1_WriteSequence(LCD_I2C_ADDR, LCD_DATA_MODE, &data, 1) != I2C_OK)
        return LCD_ERR_I2C;
    return LCD_OK;
}

lcd_error_t lcd_init(void) {
    I2C1_Init();
    __delay_ms(40);

    if (lcd_send_cmd(0x38) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x39) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x14) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x78) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x5E) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x6D) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x0C) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x01) != LCD_OK) return LCD_ERR_INIT;
    if (lcd_send_cmd(0x06) != LCD_OK) return LCD_ERR_INIT;

    return LCD_OK;
}

lcd_error_t lcd_write_text(const char* str) {
    uint8_t buffer[20];
    uint8_t len = 0;

    while (*str && len < sizeof(buffer)) {
        buffer[len++] = *str++;
    }

    if (I2C1_WriteSequence(LCD_I2C_ADDR, LCD_DATA_MODE, buffer, len) != I2C_OK)
        return LCD_ERR_I2C;

    return LCD_OK;
}

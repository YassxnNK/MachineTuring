/**
 * @file    lcd_driver.h
 * @brief   Interface du pilote LCD I2C (Newhaven ST7036i)
 */

#ifndef LCD_DRIVER_H
#define LCD_DRIVER_H

#include <stdint.h>
#include "error_defs.h"

lcd_error_t lcd_init(void);
lcd_error_t lcd_send_cmd(uint8_t cmd);
lcd_error_t lcd_send_data(uint8_t data);
lcd_error_t lcd_write_text(const char* str);

#endif // LCD_DRIVER_H

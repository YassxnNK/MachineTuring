/**
 * @file    matrix_driver.h
 * @brief   Interface du pilote de matrice LED RGB
 */

#ifndef MATRIX_DRIVER_H
#define MATRIX_DRIVER_H

#include <stdint.h>
#include "error_defs.h"

matrix_error_t matrix_init(void);
matrix_error_t matrix_set_pixel(uint8_t* frame, uint8_t x, uint8_t y, uint8_t color);
matrix_error_t matrix_send_frames(void);
matrix_error_t matrix_clear_all(uint8_t color);

#endif // MATRIX_DRIVER_H

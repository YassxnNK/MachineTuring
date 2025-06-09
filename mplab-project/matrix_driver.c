/**
 * @file    matrix_driver.c
 * @brief   Gestion d'une matrice LED RGB via SPI
 */

#include "matrix_driver.h"
#include <libpic30.h>

#define FRAME_SIZE 64

static uint8_t frame1[FRAME_SIZE];
static uint8_t frame2[FRAME_SIZE];

matrix_error_t matrix_set_pixel(uint8_t* frame, uint8_t x, uint8_t y, uint8_t color) {
    if (!frame || x >= 8 || y >= 8) return MATRIX_ERR_PARAM;
    frame[y * 8 + x] = color;
    return MATRIX_OK;
}

/*
matrix_error_t matrix_send_frames(void) {
    if (SPI1_WriteBuffer(frame2, FRAME_SIZE) != SPI_OK) return MATRIX_ERR_SPI;
    if (SPI1_WriteBuffer(frame1, FRAME_SIZE) != SPI_OK) return MATRIX_ERR_SPI;
    return MATRIX_OK;
}
 * */

matrix_error_t matrix_clear_all(uint8_t color) {
    for (uint8_t i = 0; i < FRAME_SIZE; i++) {
        frame1[i] = color;
        frame2[i] = color;
    }
    return MATRIX_OK;
}

matrix_error_t matrix_init(void) {
    return SPI1_Init();
}

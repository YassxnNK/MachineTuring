#ifndef ALGO_MANAGEMENT_H
#define ALGO_MANAGEMENT_H

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include "led_management.h"

typedef enum {
    LEFT,
    RIGHT,
    CONTINUE
} move_t;

typedef enum {
    ALGO_ERR_OK = 0,
    ALGO_ERR_NULL_STATE,
    ALGO_ERR_NULL_POS,
    ALGO_ERR_NULL_COLOR,
    ALGO_ERR_NULL_MOVE,
    ALGO_ERR_MIN_MOVE,
    ALGO_ERR_MAX_MOVE
} algo_err_t;

typedef struct state_s {
    uint8_t id_state;

    struct state_s *next_state_red;
    struct state_s *next_state_yel;
    struct state_s *next_state_grn;

    color_t next_color_red;
    color_t next_color_yel;
    color_t next_color_grn;

    move_t next_movement_red;
    move_t next_movement_grn;
    move_t next_movement_yel;
} state_t;

typedef struct {
    state_t *current_state;
    uint8_t current_pos;
    move_t current_movement;
    bool is_done;
} algo_t;

// Prototypes
algo_err_t run_turing_step(algo_t *machine, led_t *leds, size_t num_leds);
algo_err_t move_head(algo_t *machine, size_t num_leds, move_t direction);

#endif // ALGO_MANAGEMENT_H

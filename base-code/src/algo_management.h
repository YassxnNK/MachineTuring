#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include "led_management.h"

typedef enum{
    LEFT,
    RIGHT,
    CONTINUE
} move_t;

typedef struct state_s{
    uint8_t id_state;
    struct state_s *next_state_red;
    struct state_s *next_state_blu;
    struct state_s *next_state_grn;
    color_t next_color_red;
    color_t next_color_blu;
    color_t next_color_grn;
    move_t next_movement;
} state_t;

typedef struct {
    state_t *current_state;
    uint8_t current_pos;
    move_t current_movement;
    bool is_done;
} algo_t;

void run_turing_step(algo_t *machine, led_t *leds, size_t num_leds);
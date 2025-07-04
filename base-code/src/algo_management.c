#include "algo_management.h"

algo_err_t move_head(algo_t *machine, size_t num_leds, move_t direction) {
    if (!machine) return ALGO_ERR_NULL_STATE;

    if (direction == LEFT) {
        if (machine->current_pos > 0) {
            machine->current_pos--;
        } else {
            machine->is_done = true;
            return ALGO_ERR_MIN_MOVE;
        }
    } else if (direction == RIGHT) {
        if (machine->current_pos + 1 < num_leds) {
            machine->current_pos++;
        } else {
            machine->is_done = true;
            return ALGO_ERR_MAX_MOVE;
        }
    }
    machine->current_movement = direction;
    return ALGO_ERR_OK;
}

algo_err_t run_turing_step(algo_t *machine, led_t *leds, size_t num_leds) {
    if (!machine || !leds) return ALGO_ERR_NULL_STATE;
    if (machine->is_done) return ALGO_ERR_OK;
    if (!machine->current_state) {
        machine->is_done = true;
        return ALGO_ERR_NULL_STATE;
    }

    uint8_t pos = machine->current_pos;
    if (pos >= num_leds) {
        machine->is_done = true;
        return ALGO_ERR_NULL_POS;
    }

    color_t current_color = leds[pos].color;
    move_t move;

    // Appliquer la règle selon la couleur actuelle
    switch (current_color) {
        case COLOR_RED:
            leds[pos].color = machine->current_state->next_color_red;
            machine->current_state = machine->current_state->next_state_red;
            if (!machine->current_state) return ALGO_ERR_NULL_STATE;
            move = machine->current_state->next_movement_red;
            break;

        case COLOR_YEL:
            leds[pos].color = machine->current_state->next_color_yel;
            machine->current_state = machine->current_state->next_state_yel;
            if (!machine->current_state) return ALGO_ERR_NULL_STATE;
            move = machine->current_state->next_movement_yel;
            break;

        case COLOR_GRN:
            leds[pos].color = machine->current_state->next_color_grn;
            machine->current_state = machine->current_state->next_state_grn;
            if (!machine->current_state) return ALGO_ERR_NULL_STATE;
            move = machine->current_state->next_movement_grn;
            break;

        default:
            machine->is_done = true;
            return ALGO_ERR_NULL_COLOR;
    }

    if (move == CONTINUE) {
        if (machine->current_movement == CONTINUE) {
            machine->is_done = true;
            return ALGO_ERR_NULL_MOVE;
        }
        move = machine->current_movement;
    }

    return move_head(machine, num_leds, move);
}

#include "algo_management.h"

void move_head(algo_t *machine, size_t num_leds, move_t direction) {
    if (direction == LEFT) {
        if (machine->current_pos > 0) {
            machine->current_pos--;
        } else {
            machine->is_done = true;
        }
    } else if (direction == RIGHT) {
        if (machine->current_pos + 1 < num_leds) {
            machine->current_pos++;
        } else {
            machine->is_done = true;
        }
    }
    machine->current_movement = direction;
}

void run_turing_step(algo_t *machine, led_t *leds, size_t num_leds) {
    // If machine is done no need to conitnue
    if (machine->is_done) {
        return;
    }

    // If the state is nul, then machine is done
    if (machine->current_state == NULL) {
        machine->is_done = true;
        return;
    }

    uint8_t pos = machine->current_pos;

    // Led safety
    if (pos >= num_leds) {
        machine->is_done = true;
        return;
    }

    color_t current_color = leds[pos].color;

    move_t move;
    // Handle transition based on current color
    switch (current_color) {
        case COLOR_RED:
            leds[pos].color = machine->current_state->next_color_red;
            machine->current_state = machine->current_state->next_state_red;
            move = machine->current_state->next_movement_red;
            break;

        case COLOR_YEL:
            leds[pos].color = machine->current_state->next_color_yel;
            machine->current_state = machine->current_state->next_state_yel;
            move = machine->current_state->next_movement_yel;
            break;

        case COLOR_GRN:
            leds[pos].color = machine->current_state->next_color_grn;
            machine->current_state = machine->current_state->next_state_grn;
            move = machine->current_state->next_movement_grn;
            break;

        default:
            machine->is_done = true; // Invalid color
            return;
    }

    if (move == CONTINUE) {
        if (machine->current_movement == CONTINUE) {
            machine->is_done = true;
            return;
        }
        move = machine->current_movement;
    }
    move_head(machine, num_leds, move);

    // Optional safety
    if (machine->current_state == NULL) {
        machine->is_done = true;
    }
}
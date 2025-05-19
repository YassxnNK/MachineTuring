#include "algo_management.h"

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

    // Handle transition based on current color
    switch (current_color) {
        case COLOR_RED:
            leds[pos].color = machine->current_state->next_color_red;
            machine->current_state = machine->current_state->next_state_red;
            break;

        case COLOR_BLU:
            leds[pos].color = machine->current_state->next_color_blu;
            machine->current_state = machine->current_state->next_state_blu;
            break;

        case COLOR_GRN:
            leds[pos].color = machine->current_state->next_color_grn;
            machine->current_state = machine->current_state->next_state_grn;
            break;

        default:
            machine->is_done = true; // Invalid color
            return;
    }

    // Move the head
    switch (machine->current_state->next_movement) {
        case LEFT:
            if (machine->current_pos > 0) {
                machine->current_pos--;
            } else {
                machine->is_done = true;
            }
            machine->current_movement = LEFT;
            break;

        case RIGHT:
            if (machine->current_pos + 1 < num_leds) {
                machine->current_pos++;
            } else {
                machine->is_done = true;
            }
            machine->current_movement = RIGHT;
            break;

        case CONTINUE:
            if (machine->current_movement == CONTINUE) {
                machine->is_done = true;
                return;
            }

            // TODO: Some function because we have dup code
            if (machine->current_movement == LEFT) {
                if (machine->current_pos > 0) {
                    machine->current_pos--;
                } else {
                    machine->is_done = true;
                }
                machine->current_movement = LEFT;
            }
            else {
                if (machine->current_pos + 1 < num_leds) {
                    machine->current_pos++;
                } else {
                    machine->is_done = true;
                }
                machine->current_movement = RIGHT;
            }
            break;
    }

    // Optional safety
    if (machine->current_state == NULL) {
        machine->is_done = true;
    }
}
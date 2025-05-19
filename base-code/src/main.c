#include "algo_management.h"

int main(int, char**){
    // TODO: load from json
    led_t leds[NUM_LEDS];

    // Initialize leds
    for (int i = 0; i < NUM_LEDS; i++) {
        leds[i].position = i;
        leds[i].color = COLOR_RED; 
    }

    // Setup machine
    algo_t machine = {};

    // Run loop
    while (!machine.is_done) {
        run_turing_step(&machine, leds, NUM_LEDS);
        light_leds(leds, machine.current_pos);
    }
}

#define JSMN_HEADER
#include "jsmn.h"

#include "parser_json.h"

algo_t init_pingpong_algo() {
    static state_t right;
    static state_t left;

    // État "right" : avance vers la droite
    right.id_state = 0;
    right.next_state_red  = &left;   // Change d'état si rouge
    right.next_state_grn  = &right;
    right.next_state_yel  = &right;

    right.next_color_red  = COLOR_RED;
    right.next_color_grn  = COLOR_GRN;
    right.next_color_yel  = COLOR_GRN;

    right.next_movement_red  = RIGHT;
    right.next_movement_grn  = RIGHT;
    right.next_movement_yel  = RIGHT;

    // État "left" : revient vers la gauche
    left.id_state = 1;
    left.next_state_red  = &right;
    left.next_state_grn  = &left;
    left.next_state_yel  = &left;

    left.next_color_red  = COLOR_RED;
    left.next_color_grn  = COLOR_YEL;
    left.next_color_yel  = COLOR_YEL;

    left.next_movement_red  = LEFT;
    left.next_movement_grn  = LEFT;
    left.next_movement_yel  = LEFT;

    algo_t machine = {
        .current_state = &right,
        .current_pos = 4,
        .current_movement = RIGHT,
        .is_done = false
    };

    return machine;
}

void wait_ms(int milliseconds) {
    #ifdef _WIN32
        Sleep(milliseconds);
    #else
        usleep(milliseconds * 1000);
    #endif
}

void clear_console() {
    #ifdef _WIN32
        system("cls");
    #else
        system("clear");
    #endif
}



int main(int, char**){
    // TODO: load from json
    led_t leds[NUM_LEDS];

    // Setup machine
    //algo_t machine = init_pingpong_algo();
    algo_t machine;
    parse_status_t status = parse_turing_file("./algo/algo1.json", &machine, leds);
    if (status != PARSE_OK) {
        printf("Erreur de parsing: %d\n", status);
        exit(0);
    }

    // Run loop
    while (!machine.is_done) {
        clear_console();
        run_turing_step(&machine, leds, NUM_LEDS);
        light_leds(leds, machine.current_pos);
        wait_ms(250);
    }
}

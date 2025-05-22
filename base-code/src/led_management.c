#include "led_management.h"



void change_color(led_t* led, uint8_t color) {
    
}

void light_leds(led_t* leds, int pos) {
    printf("Leds simulation:\n");
    for (size_t i = 0; i < NUM_LEDS; i++) {
        printf("[");
        if (pos == i)
            printf("(");

        switch (leds[i].color) {
            case COLOR_RED:
                printf("R");
                break;
            case COLOR_YEL:
                printf("B");
                break;
            case COLOR_GRN:
                printf("G");
                break;
        }
        if (pos == i)
            printf(")");

        printf("]");
    }
    printf("\n\n");
}
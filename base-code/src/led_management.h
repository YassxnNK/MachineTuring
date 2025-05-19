#include <stdint.h>
#include <stdio.h>

#define NUM_LEDS 18

typedef enum{
    COLOR_RED,
    COLOR_BLU,
    COLOR_GRN
} color_t;

typedef struct {
    color_t color;
    uint8_t position;
} led_t;

void light_leds(led_t* leds, int pos);
// TODO: Check if a position enum isn't needed (shouldn't be but we never know)
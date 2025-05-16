/* 
 * File:   parserJSON.h
 * Author: ya.bolard
 *
 * Created on May 15, 2025, 11:59 AM
 */

#ifndef PARSERJSON_H
#define PARSERJSON_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Déclaration des structures */
typedef enum color_s {
    RED,
    YELLOW,
    GREEN
} color_t;

typedef enum direction_s {
    RIGHT,
    LEFT,
    NONE
} direction_t;

typedef struct next_step_s {
    color_t next_color;
    direction_t direction;
    int next_state;
} next_step_t;

/* Déclaration des variables globales */
#define NB_CELLS 17
extern char init_display[NB_CELLS];  // 1 à 16
extern int starting_pos;
extern next_step_t **state;          // [state_id][RED/YELLOW/GREEN]
extern int nb_states;

/* Déclaration des fonctions */
color_t parse_color(const char* str);
direction_t parse_direction(char c);
void parse_json_with_jsmn(const char* filename);
char* read_file(const char* filename);
void free_state(void);

#endif

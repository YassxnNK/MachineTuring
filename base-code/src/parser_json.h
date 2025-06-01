#ifndef PARSER_JSON_H
#define PARSER_JSON_H

#include "algo_management.h"
#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

typedef enum {
    PARSE_OK = 0,
    PARSE_ERR_FILE_NOT_FOUND,
    PARSE_ERR_JSON,
    PARSE_ERR_NO_STATE_1,
    PARSE_ERR_INVALID_TRANSITION,
    PARSE_ERR_MEMORY
} parse_status_t;

/// Parse le fichier JSON et initialise l'algo
parse_status_t parse_turing_file(const char *filename, algo_t *machine, led_t *leds);

/// Donne accès à la bande de LED lue depuis le JSON
led_t *get_loaded_leds(void);

/// Libère la mémoire allouée dynamiquement
void free_parsed_data(void);

#endif

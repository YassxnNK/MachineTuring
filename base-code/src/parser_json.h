#ifndef PARSER_JSON_H
#define PARSER_JSON_H

#include "algo_management.h"
#include "jsmn.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdint.h>

/// Codes d'erreur pour le parsing JSON
typedef enum {
    PARSE_OK = 0,
    PARSE_ERR_FILE_NOT_FOUND,    // Impossible d'ouvrir le fichier
    PARSE_ERR_JSON,              // Erreur dans le format JSON (jsmn_parse)
    PARSE_ERR_NO_STATE_1,        // Aucun état "1" trouvé comme point de départ
    PARSE_ERR_INVALID_TRANSITION,// Mauvaise définition de transition
    PARSE_ERR_MEMORY             // Allocation mémoire échouée
} parse_status_t;

/// Parse le fichier JSON et initialise la machine (états + bande de LED)
parse_status_t parse_turing_file(const char *filename, algo_t *machine, led_t *leds);

/// Accès à la dernière bande de LED chargée (si besoin de référence externe)
led_t *get_loaded_leds(void);

/// Libère toutes les ressources allouées dynamiquement par le parseur
void free_parsed_data(void);

#endif // PARSER_JSON_H

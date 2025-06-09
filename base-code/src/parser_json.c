#include "parser_json.h"

#define MAX_TOKENS 512
#define MAX_STATES 64

typedef struct {
    int id;
    state_t *ptr;
} state_map_t;

// Mémoire interne
static state_t *state_table = NULL;
static int state_count = 0;

/**
 * Compare un jeton jsmn à une chaîne C.
 * Renvoie PARSE_OK + *result = true si égal, PARSE_OK + *result = false sinon.
 * Si erreur (tok ou paramètres null), renvoie PARSE_ERR_JSON_TOKEN.
 */
parse_err_t json_token_eq(const char *json, jsmntok_t *tok, const char *s, bool *result) {
    if (!json || !tok || !s || !result) {
        return PARSE_ERR_JSON_TOKEN;
    }
    if (tok->type != JSMN_STRING) {
        *result = false;
        return PARSE_OK;
    }
    int len = tok->end - tok->start;
    if ((int)strlen(s) != len) {
        *result = false;
        return PARSE_OK;
    }
    *result = (strncmp(json + tok->start, s, len) == 0);
    return PARSE_OK;
}

/**
 * Convertit un caractère ('r','g','y') en color_t via 'out_col'.
 * Si char invalide, renvoie PARSE_ERR_INVALID_COLOR.
 */
static parse_err_t char_to_color(char c, color_t *out_col) {
    if (!out_col) return PARSE_ERR_JSON;
    switch (c) {
        case 'r':
            *out_col = COLOR_RED; return PARSE_OK;
        case 'g':
            *out_col = COLOR_GRN; return PARSE_OK;
        case 'y':
            *out_col = COLOR_YEL; return PARSE_OK;
        default:
            return PARSE_ERR_INVALID_COLOR;
    }
}

/**
 * Convertit un caractère ('l','r','c') en move_t via 'out_move'.
 * Si char invalide, renvoie PARSE_ERR_INVALID_MOVE.
 */
static parse_err_t char_to_move(char c, move_t *out_move) {
    if (!out_move) return PARSE_ERR_JSON;
    switch (c) {
        case 'l':
            *out_move = LEFT; return PARSE_OK;
        case 'r':
            *out_move = RIGHT; return PARSE_OK;
        case 'c':
            *out_move = CONTINUE; return PARSE_OK;
        default:
            return PARSE_ERR_INVALID_MOVE;
    }
}

void free_parsed_data(void) {
    if (state_table) {
        free(state_table);
        state_table = NULL;
    }
    state_count = 0;
}

/// Lit le fichier JSON et initialise 'machine' et 'leds'.
parse_err_t parse_turing_file(const char *filename, algo_t *machine, led_t *leds) {
    free_parsed_data(); // Reset mémoire éventuelle

    if (!filename || !machine || !leds) {
        return PARSE_ERR_MEMORY;
    }

    FILE *f = fopen(filename, "r");
    if (!f) {
        return PARSE_ERR_FILE_NOT_FOUND;
    }

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *json = malloc(len + 1);
    if (!json) {
        fclose(f);
        return PARSE_ERR_MEMORY;
    }
    fread(json, 1, len, f);
    json[len] = '\0';
    fclose(f);

    jsmn_parser parser;
    jsmntok_t tokens[MAX_TOKENS];
    jsmn_init(&parser);
    int count = jsmn_parse(&parser, json, len, tokens, MAX_TOKENS);
    if (count < 0) {
        free(json);
        return PARSE_ERR_JSON;
    }

    state_table = calloc(MAX_STATES, sizeof(state_t));
    if (!state_table) {
        free(json);
        return PARSE_ERR_MEMORY;
    }

    state_map_t id_map[MAX_STATES];
    int current_pos = 0;
    move_t initial_movement = LEFT;  // Valeur par défaut si non spécifiée
    state_count = 0;

    // Parcours des tokens JSON
    for (int i = 1; i < count; i++) {
        bool match = false;
        parse_err_t st_ok;

        // --- init_display ---
        st_ok = json_token_eq(json, &tokens[i], "init_display", &match);
        if (st_ok != PARSE_OK) { free(json); return st_ok; }
        if (match) {
            jsmntok_t *obj = &tokens[i + 1];
            int j = i + 2;
            for (int k = 0; k < obj->size; k++) {
                // Clé possible : "pos"
                st_ok = json_token_eq(json, &tokens[j], "pos", &match);
                if (st_ok != PARSE_OK) { free(json); return st_ok; }
                if (match) {
                    current_pos = atoi(json + tokens[j + 1].start);
                    j += 2;
                    continue;
                }

                st_ok = json_token_eq(json, &tokens[j], "movement", &match);
                if (st_ok != PARSE_OK) { free(json); return st_ok; }
                if (match) {
                    st_ok = char_to_move(json[tokens[j + 1].start], &initial_movement);
                    j += 2;
                    continue;
                }
                // Autre clé → indice de LED (sous forme de chaîne de chiffres)
                int idx = atoi(json + tokens[j].start) - 1;
                if (idx < 0 || idx >= NUM_LEDS) {
                    free(json);
                    free_parsed_data();
                    return PARSE_ERR_JSON;
                }
                char color_char = json[tokens[j + 1].start];
                color_t cval;
                st_ok = char_to_color(color_char, &cval);
                if (st_ok != PARSE_OK) {
                    free(json);
                    free_parsed_data();
                    return st_ok;
                }
                leds[idx].position = (uint8_t)idx;
                leds[idx].color = cval;
                j += 2;
            }
            continue;
        }

        // --- state (début d'un objet d'états) ---
        st_ok = json_token_eq(json, &tokens[i], "state", &match);
        if (st_ok != PARSE_OK) { free(json); return st_ok; }
        if (match) {
            int j = i + 2;
            while (j < count && state_count < MAX_STATES) {
                // Clé de l'état : un chiffre sous forme de chaîne
                int id = atoi(json + tokens[j].start);
                state_t *s = &state_table[state_count];
                memset(s, 0, sizeof(state_t));
                s->id_state = (uint8_t)id;
                id_map[state_count].id = id;
                id_map[state_count].ptr = s;
                state_count++;

                jsmntok_t *state_obj = &tokens[j + 1];
                int rule_count = state_obj->size;
                int k = j + 2;

                // Pour chaque règle "red","green","yellow"
                for (int r = 0; r < rule_count; r++) {
                    // Récupération du mot-clé "red"/"green"/"yellow"
                    int key_len = tokens[k].end - tokens[k].start;
                    char key[16] = {0};
                    strncpy(key, json + tokens[k].start, key_len);

                    jsmntok_t *rule = &tokens[k + 1];
                    int m = k + 2;
                    color_t col = COLOR_RED;
                    move_t mov = RIGHT;
                    int next_id = 0;

                    // Parcours des champs de l'objet de règle
                    for (int f = 0; f < rule->size; f++) {
                        bool submatch = false;
                        // next_color
                        st_ok = json_token_eq(json, &tokens[m], "next_color", &submatch);
                        if (st_ok != PARSE_OK) { free(json); return st_ok; }
                        if (submatch) {
                            char cchar = json[tokens[m + 1].start];
                            st_ok = char_to_color(cchar, &col);
                            if (st_ok != PARSE_OK) {
                                free(json);
                                free_parsed_data();
                                return st_ok;
                            }
                            m += 2;
                            continue;
                        }
                        // movement
                        st_ok = json_token_eq(json, &tokens[m], "movement", &submatch);
                        if (st_ok != PARSE_OK) { free(json); return st_ok; }
                        if (submatch) {
                            char mchar = json[tokens[m + 1].start];
                            st_ok = char_to_move(mchar, &mov);
                            if (st_ok != PARSE_OK) {
                                free(json);
                                free_parsed_data();
                                return st_ok;
                            }
                            m += 2;
                            continue;
                        }
                        // state
                        st_ok = json_token_eq(json, &tokens[m], "state", &submatch);
                        if (st_ok != PARSE_OK) { free(json); return st_ok; }
                        if (submatch) {
                            next_id = atoi(json + tokens[m + 1].start);
                            m += 2;
                            continue;
                        }
                        // Si aucune clé connue
                        m += 2;
                    }

                    // Assignation selon la couleur lue ("red"/"green"/"yellow")
                    if (strcmp(key, "red") == 0) {
                        s->next_color_red = col;
                        s->next_movement_red = mov;
                        s->next_state_red = (state_t *)(intptr_t)next_id;
                    } else if (strcmp(key, "green") == 0) {
                        s->next_color_grn = col;
                        s->next_movement_grn = mov;
                        s->next_state_grn = (state_t *)(intptr_t)next_id;
                    } else if (strcmp(key, "yellow") == 0) {
                        s->next_color_yel = col;
                        s->next_movement_yel = mov;
                        s->next_state_yel = (state_t *)(intptr_t)next_id;
                    }

                    // Passage à la règle suivante
                    k += 2 + rule->size * 2;
                }

                // Passage à l'état JSON suivant
                j += 2 + rule_count * 2;
            }
            continue;
        }
    }

    // Résolution des pointeurs next_state_xxx
    for (int i = 0; i < state_count; i++) {
        state_t *s = &state_table[i];
        intptr_t ids[3] = {
            (intptr_t)s->next_state_red,
            (intptr_t)s->next_state_grn,
            (intptr_t)s->next_state_yel
        };
        for (int c = 0; c < 3; c++) {
            for (int j = 0; j < state_count; j++) {
                if (id_map[j].id == ids[c]) {
                    if (c == 0) s->next_state_red = id_map[j].ptr;
                    if (c == 1) s->next_state_grn = id_map[j].ptr;
                    if (c == 2) s->next_state_yel = id_map[j].ptr;
                }
            }
        }
    }

    // Déterminer l'état initial (id == 1)
    machine->current_state = NULL;
    for (int i = 0; i < state_count; i++) {
        if (state_table[i].id_state == 1) {
            machine->current_state = &state_table[i];
            break;
        }
    }
    if (!machine->current_state) {
        free(json);
        free_parsed_data();
        return PARSE_ERR_NO_STATE_1;
    }

    // Initialisation finale de la machine
    machine->current_pos = current_pos;
    machine->current_movement = initial_movement;
    machine->is_done = false;

    free(json);
    return PARSE_OK;
}

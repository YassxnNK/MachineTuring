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

static bool json_token_eq(const char *json, jsmntok_t *tok, const char *s) {
    return tok->type == JSMN_STRING &&
           (int)strlen(s) == tok->end - tok->start &&
           strncmp(json + tok->start, s, tok->end - tok->start) == 0;
}

static color_t char_to_color(char c) {
    switch (c) {
        case 'r': return COLOR_RED;
        case 'g': return COLOR_GRN;
        case 'y': return COLOR_YEL;
        default:  return COLOR_RED;
    }
}

static move_t char_to_move(char c) {
    switch (c) {
        case 'l': return LEFT;
        case 'r': return RIGHT;
        case 'c': return CONTINUE;
        default:  return RIGHT;
    }
}

void free_parsed_data(void) {
    if (state_table) {
        free(state_table);
        state_table = NULL;
    }
    state_count = 0;
}

parse_status_t parse_turing_file(const char *filename, algo_t *machine, led_t *leds) {
    free_parsed_data(); // Reset mémoire

    FILE *f = fopen(filename, "r");
    if (!f) return PARSE_ERR_FILE_NOT_FOUND;

    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    fseek(f, 0, SEEK_SET);
    char *json = malloc(len + 1);
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
    move_t current_movement = LEFT;

    for (int i = 1; i < count; i++) {
        if (json_token_eq(json, &tokens[i], "init_display")) {
            jsmntok_t *obj = &tokens[i + 1];
            int j = i + 2;
            for (int k = 0; k < obj->size; k++) {
                if (json_token_eq(json, &tokens[j], "pos")) {
                    current_pos = atoi(json + tokens[j + 1].start);
                }
                else if(json_token_eq(json, &tokens[j], "movement")) {
                    current_movement = char_to_move(json[tokens[j + 1].start]);
                } else {
                    int idx = atoi(json + tokens[j].start) - 1;
                    leds[idx].position = idx;
                    leds[idx].color = char_to_color(json[tokens[j + 1].start]);
                }
                j += 2;
            }
        }

        if (json_token_eq(json, &tokens[i], "state")) {
            int j = i + 2;
            while (j < count && state_count < MAX_STATES) {
                int id = atoi(json + tokens[j].start);
                state_t *s = &state_table[state_count];
                s->id_state = id;
                id_map[state_count].id = id;
                id_map[state_count].ptr = s;
                state_count++;

                jsmntok_t *state_obj = &tokens[j + 1];
                int rule_count = state_obj->size;
                int k = j + 2;

                for (int r = 0; r < rule_count; r++) {
                    char key[16] = {0};
                    strncpy(key, json + tokens[k].start, tokens[k].end - tokens[k].start);
                    key[tokens[k].end - tokens[k].start] = '\0';

                    jsmntok_t *rule = &tokens[k + 1];
                    int m = k + 2;
                    color_t col = COLOR_RED;
                    move_t mov = RIGHT;
                    int next_id = 0;

                    for (int f = 0; f < rule->size; f++) {
                        if (json_token_eq(json, &tokens[m], "next_color")) {
                            col = char_to_color(json[tokens[m + 1].start]);
                        } else if (json_token_eq(json, &tokens[m], "movement")) {
                            mov = char_to_move(json[tokens[m + 1].start]);
                        } else if (json_token_eq(json, &tokens[m], "state")) {
                            next_id = atoi(json + tokens[m + 1].start);
                        }
                        m += 2;
                    }

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

                    k = m;
                }

                j += 2 + rule_count * 2;
            }
        }
    }

    // Résolution des transitions
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

    machine->current_pos = current_pos;
    machine->is_done = false;
    
    
    machine->current_movement = current_movement;

    free(json);
    return PARSE_OK;
}
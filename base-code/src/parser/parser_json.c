/* 
 * File:   parserJSON.c
 * Author: ya.bolard
 *
 * Created on May 15, 2025, 11:59 AM
*/

#include "parser_json.h"
#include "jsmn.h"

char init_display[NB_CELLS];
int starting_pos;
next_step_t **state = NULL;
int nb_states = 0;

char* read_file(const char* filename) {
    FILE* f = fopen(filename, "rb");
    if (!f) {
        perror("Erreur ouverture fichier");
        exit(1);
    }
    fseek(f, 0, SEEK_END);
    long len = ftell(f);
    rewind(f);
    char* buf = malloc(len + 1);
    if (!buf) exit(1);
    fread(buf, 1, len, f);
    buf[len] = '\0';
    fclose(f);
    return buf;
}

color_t parse_color(const char* str) {
    if (strcmp(str, "red") == 0 || strcmp(str, "r") == 0) return RED;
    if (strcmp(str, "yellow") == 0 || strcmp(str, "y") == 0) return YELLOW;
    if (strcmp(str, "green") == 0 || strcmp(str, "g") == 0) return GREEN;
    return RED;
}

direction_t parse_direction(char c) {
    if (c == 'l') return LEFT;
    if (c == 'r') return RIGHT;
    return NONE;
}

int jsoneq(const char *json, jsmntok_t *tok, const char *s) {
    return tok->type == JSMN_STRING &&
           (int)strlen(s) == tok->end - tok->start &&
           strncmp(json + tok->start, s, tok->end - tok->start) == 0;
}

int json_to_int(const char* json, jsmntok_t* tok) {
    char buf[16];
    int len = tok->end - tok->start;
    strncpy(buf, json + tok->start, len);
    buf[len] = '\0';
    return atoi(buf);
}

void parse_json_with_jsmn(const char* filename) {
    char* json = read_file(filename);
    jsmn_parser parser;
    jsmntok_t tokens[512];
    jsmn_init(&parser);
    int tok_count = jsmn_parse(&parser, json, strlen(json), tokens, 512);

    if (tok_count < 0) {
        printf("Erreur jsmn: %d\n", tok_count);
        exit(1);
    }

    int i = 1; // token[0] = objet racine

    while (i < tok_count) {
        if (jsoneq(json, &tokens[i], "init_display")) {
            int size = tokens[i+1].size;
            i += 2;
            for (int j = 0; j < size; ++j) {
                char key[8];
                int len = tokens[i].end - tokens[i].start;
                strncpy(key, json + tokens[i].start, len);
                key[len] = '\0';

                if (strcmp(key, "pos") == 0) {
                    starting_pos = json_to_int(json, &tokens[i+1]);
                } else {
                    int index = atoi(key);
                    init_display[index] = json[tokens[i+1].start];
                }
                i += 2;
            }
        }

        else if (jsoneq(json, &tokens[i], "algorithm")) {
            i++; // => objet sous "algorithm"

            int algorithm_size = tokens[i].size;
            i++; // => maintenant chaque clef sous les objets "algorithm" donc "state"

            for (int j = 0; j < algorithm_size; ++j) {
                if (jsoneq(json, &tokens[i], "state")) {
                    int num_states = tokens[i+1].size;
                    nb_states = 0;

                    state = malloc((num_states + 1) * sizeof(next_step_t*));
                    for (int s = 0; s <= num_states; ++s) state[s] = NULL;

                    i += 2; // contenu objet "state"
                    for (int s = 0; s < num_states; ++s) {
                        int state_id = json_to_int(json, &tokens[i]);
                        i++; // valeur du state

                        int num_colors = tokens[i].size;
                        state[state_id] = malloc(3 * sizeof(next_step_t));
                        nb_states = (state_id > nb_states) ? state_id : nb_states;

                        i++; // couleur clef
                        for (int c = 0; c < num_colors; ++c) {
                            int color_index = -1;

                            if (jsoneq(json, &tokens[i], "red")) color_index = RED;
                            else if (jsoneq(json, &tokens[i], "yellow")) color_index = YELLOW;
                            else if (jsoneq(json, &tokens[i], "green")) color_index = GREEN;
                            else {
                                fprintf(stderr, "Couleur inconnue dans l'état %d\n", state_id);
                                exit(1);
                            }

                            i++; // valeur suivant la clef (next_color, movement, state)
                            int subfield_count = tokens[i].size;
                            i++; // champs des valeurs

                            for (int f = 0; f < subfield_count; ++f) {
                                if (jsoneq(json, &tokens[i], "next_color")) {
                                    char buf[8];
                                    int len = tokens[i+1].end - tokens[i+1].start;
                                    strncpy(buf, json + tokens[i+1].start, len);
                                    buf[len] = '\0';
                                    state[state_id][color_index].next_color = parse_color(buf);
                                    i += 2;
                                } else if (jsoneq(json, &tokens[i], "movement")) {
                                    char move = json[tokens[i+1].start];
                                    state[state_id][color_index].direction = parse_direction(move);
                                    i += 2;
                                } else if (jsoneq(json, &tokens[i], "state")) {
                                    state[state_id][color_index].next_state = json_to_int(json, &tokens[i+1]);
                                    i += 2;
                                } else {
                                    i += 2; // champ inconnu
                                }
                            }
                        }
                    }
                } else {
                    i += 2; // skip champ inconnu dans l'algorithme
                }
            }
        }

        else {
            i++; // skip unknown top-level field
        }
    }

    free(json);
}


void free_state(void) {
    for (int i = 1; i <= nb_states; ++i)
        if (state[i]) free(state[i]);
    free(state);
}

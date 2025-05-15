/* 
 * File:   parserJSON.c
 * Author: ya.bolard
 *
 * Created on May 15, 2025, 11:59 AM
*/

#include "parserJSON.h"
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
        printf("Erreur jsmn\n");
        exit(1);
    }

    int i = 1;  // token[0] = root object
    while (i < tok_count) {
        if (jsoneq(json, &tokens[i], "init_display")) {
            int nb = tokens[i+1].size;
            i += 2;
            for (int j = 0; j < nb; ++j) {
                int index = json_to_int(json, &tokens[i]);
                init_display[index] = json[tokens[i+1].start];
                i += 2;
            }
        } else if (jsoneq(json, &tokens[i], "pos")) {
            starting_pos = json_to_int(json, &tokens[i+1]);
            i += 2;
        } else if (jsoneq(json, &tokens[i], "algorithm")) {
            i++; // skip "algorithm"
            if (jsoneq(json, &tokens[i], "state")) {
                int states_count = tokens[i+1].size;
                nb_states = 0;
                state = malloc((states_count + 1) * sizeof(next_step_t*));
                for (int k = 0; k <= states_count; ++k) state[k] = NULL;

                i += 2;
                for (int s = 0; s < states_count; ++s) {
                    int state_id = json_to_int(json, &tokens[i]);
                    state[state_id] = malloc(3 * sizeof(next_step_t));
                    nb_states = (state_id > nb_states) ? state_id : nb_states;
                    i++;  // now object for state_id

                    int nb_colors = tokens[i].size;
                    i++;

                    for (int c = 0; c < nb_colors; ++c) {
                        int color_index = -1;
                        if (jsoneq(json, &tokens[i], "red")) color_index = 0;
                        else if (jsoneq(json, &tokens[i], "yellow")) color_index = 1;
                        else if (jsoneq(json, &tokens[i], "green")) color_index = 2;

                        i++;  // object
                        for (int f = 0; f < 3; ++f) {
                            if (jsoneq(json, &tokens[i], "next_color")) {
                                char color_buf[10];
                                int len = tokens[i+1].end - tokens[i+1].start;
                                strncpy(color_buf, json + tokens[i+1].start, len);
                                color_buf[len] = '\0';
                                state[state_id][color_index].next_color = parse_color(color_buf);
                                i += 2;
                            } else if (jsoneq(json, &tokens[i], "movement")) {
                                char m = json[tokens[i+1].start];
                                state[state_id][color_index].direction = parse_direction(m);
                                i += 2;
                            } else if (jsoneq(json, &tokens[i], "state")) {
                                state[state_id][color_index].next_state = json_to_int(json, &tokens[i+1]);
                                i += 2;
                            }
                        }
                    }
                }
            } else {
                i++;  // Skip unknown key under algorithm
            }
        } else {
            i++;
        }
    }

    free(json);
}

void free_state(void) {
    for (int i = 1; i <= nb_states; ++i)
        if (state[i]) free(state[i]);
    free(state);
}

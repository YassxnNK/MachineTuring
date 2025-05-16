#include "parserJSON.h"

int main(void) {
    parse_json_with_jsmn("algorithmes/algo2.json");

    const char* color_names[] = {"RED", "YELLOW", "GREEN"};
    const char* dir_names[] = {"RIGHT", "LEFT", "NONE"};

    printf("Init display:\n");
    for (int i = 1; i <= 16; ++i) {
        printf("%2d: %c\n", i, init_display[i]);
    }

    printf("\nStarting pos: %d\n\n", starting_pos);

    printf("Nombre état: %d\n", nb_states);
    
    for (int i = 1; i <= nb_states; i++) {
        printf("Etat %d:\n", i);
        for (int j=0; j < 3; j++){
            printf(" %s -> next_color: %s, direction: %s, next_state: %d\n",
            color_names[j],
            color_names[state[i][j].next_color],
            dir_names[state[i][j].direction],
            state[i][j].next_state
            );
        }
    }

    free_state();
    return 0;
}
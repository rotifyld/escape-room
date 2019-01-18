#ifndef ESCAPE_ROOM_PROPOSITIONS_H
#define ESCAPE_ROOM_PROPOSITIONS_H

#include <semaphore.h>
#include <stdbool.h>

#include "err.h"
#include "bitset.h"
#include "multiset.h"

#define MAX_ID          1024

typedef struct proposition {
    bool not_initialized;
    char room_type;
    int no_players;

    bitset players_by_id;
    multiset players_by_type;
} proposition;

typedef struct propositions {

    // implementation of a cyclic doubly linked list on a table with a guard element 0
    proposition prop[MAX_ID + 1];
    int next[MAX_ID + 1];
    int prev[MAX_ID + 1];

    // have all the propositions been checked = all the propositions are waiting for players
    bool checked;
} propositions;

void props_add(propositions *props, int defined_by, int no_players, char room_type,
               bitset players_by_id, multiset players_by_type) {
    proposition p;
    p.not_initialized = true;
    p.no_players = no_players;
    p.room_type = room_type;
    p.players_by_id = players_by_id;
    p.players_by_type = players_by_type;

    props->prop[defined_by] = p;

    props->next[defined_by] = 0;
    props->prev[defined_by] = props->prev[0];
    props->next[props->prev[0]] = defined_by;
    props->prev[0] = defined_by;
}

//void props_delete(propositions *props, int id) {
//}

void props_print(propositions *props) {
    printf("Propositions : \n");
    for (int i = props->next[0]; i != 0; i = props->next[i]) {
        printf("%d (not_initialized? %d in %d%c)\n",
               i,
               props->prop[i].not_initialized,
               props->prop[i].no_players,
               props->prop[i].room_type);
        multiset_print(&props->prop[i].players_by_type);
        bitset_printf(&props->prop[i].players_by_id);
    }
    if (props->next[0] == 0) {
        printf("[NOTHING :D]\n");
    }
    printf("\n");
}

void props_remove_from_list(propositions *props, int idx_rem) {
//    printf("Removing from listt");
//    props_print(props);
    props->next[props->prev[idx_rem]] = props->next[idx_rem];
    props->prev[props->next[idx_rem]] = props->prev[idx_rem];
//    props_print(props);
}

#endif //ESCAPE_ROOM_PROPOSITIONS_H

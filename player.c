#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <semaphore.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <assert.h>
#include <errno.h>
#include <limits.h>


// BIG TODO
// manager wypisywanie opuszczających dzieci

#include "err.h"
#include "shared_storage.h"

#define BUFFER_SIZE     (1024 * 5)

bool is_there_big_enough_room(storage *strg, char type, int no_players) {
    for (int i = 1; i <= strg->no_rooms; i++) {
        if (strg->rooms[i].type == type && strg->rooms[i].capacity >= no_players) {
            return true;
        }
    }
    return false;
}

int check_for_smallest_room(storage *strg, char type, int no_players) {
    int res = 0;
    int res_capacity = MAX_ID + 1;

    for (int i = 1; i <= strg->no_rooms; i++) {
        if (strg->rooms[i].is_free &&
            strg->rooms[i].type == type &&
            strg->rooms[i].capacity >= no_players &&
            strg->rooms[i].capacity < res_capacity) {

            res = i;
            res_capacity = strg->rooms[i].capacity;
        }
    }
    return res;
}

int find_player_of_type(storage *strg, char type) {
    for (int i = 1; i <= strg->no_players; i++) {
        if (strg->player_type[i] == type && strg->assigned_room[i] == 0) {
            return i;
        }
    }
    return 0;
}

enum ReturnValue {
    SUCCESS, INVALID_PROP, END_OF_PROPS
};

enum ReturnValue add_proposition(FILE *f_in, FILE *f_out, storage *strg, int id) {
    char buffer[BUFFER_SIZE];
    int i = 0;
    char room_type;
    int no_players = 1;
    int max_req_id = id;
    bitset req_by_id;
    multiset req_by_type;
    bitset_init(&req_by_id);
    multiset_init(&req_by_type);

    // add myself
    bitset_add(&req_by_id, id);
    multiset_add(&req_by_type, strg->player_type[id]);

    if (!fgets(buffer, BUFFER_SIZE, f_in)) {
        return END_OF_PROPS;
    };

    // get desired room info
    room_type = buffer[0];
    i += 2;

    while (buffer[i] != '\0' && (buffer[i] != ' ' || buffer[i+1] != '\0')) {
        no_players++;

        if (buffer[i] >= 'A') {
            multiset_add(&req_by_type, buffer[i]);
            i += 2;
        } else {
            int res = 0;
            while (buffer[i] > '0' && buffer[i] < '9') {
                res *= 10;
                res += (buffer[i] - '0');
                i++;
            }
            i++;
            bitset_add(&req_by_id, res);
            multiset_add(&req_by_type, strg->player_type[res]);
            if (max_req_id < res) max_req_id = res;
        }
    }

    if (max_req_id > strg->no_players ||
        !multiset_subset(&req_by_type, &strg->total_players_by_type) ||
        !is_there_big_enough_room(strg, room_type, no_players)) {
        fprintf(f_out, "Invalid game \"%s\"\n", buffer);
        return INVALID_PROP;
    }

    props_add(&strg->props, id, no_players, room_type, req_by_id, req_by_type);

    return SUCCESS;
}

int check_conditions(storage *strg, int game_id) {
    if (bitset_subset(&strg->props.prop[game_id].players_by_id, &strg->avl_players_by_id) &&
        multiset_subset(&strg->props.prop[game_id].players_by_type, &strg->avl_players_by_type)) {
        return check_for_smallest_room(strg,
                                       strg->props.prop[game_id].room_type,
                                       strg->props.prop[game_id].no_players);
    }
    return 0;
}

void initialize_game(FILE *f_out, storage *strg, int defined_by, int room_id) {
    proposition *p = &strg->props.prop[defined_by];
    game *g = &strg->rooms[room_id].curr_game;

    p->not_initialized = false;

    g->defined_by = defined_by;
    g->no_players_left_to_enter = p->no_players;
    g->no_players_left_to_exit = p->no_players;

    strg->rooms[room_id].is_free = false;

    props_remove_from_list(&strg->props, defined_by);

    // Inform all players
    for (int i = bitset_get_next(&p->players_by_id, 0);
         i != 0; i = bitset_get_next(&p->players_by_id, i)) {
        multiset_rem(&p->players_by_type, strg->player_type[i]);

        strg->assigned_room[i] = room_id;
        bitset_remove(&strg->avl_players_by_id, i);
        multiset_rem(&strg->avl_players_by_type, strg->player_type[i]);
//        printf("Budzę %d\n", i);
        if (sem_post(&strg->private_sem[i])) syserr("sem_post");
    }

    for (char type = multiset_draw(&p->players_by_type);
         type != 0; type = multiset_draw(&p->players_by_type)) {
        int player_id = find_player_of_type(strg, type);
        assert (player_id != 0);

        strg->assigned_room[player_id] = room_id;
        bitset_remove(&strg->avl_players_by_id, player_id);
        multiset_rem(&strg->avl_players_by_type, strg->player_type[player_id]);
        bitset_add(&p->players_by_id, player_id);
//        printf("Budzę %d (%c)\n", player_id, type);
        if (sem_post(&strg->private_sem[player_id])) syserr("sem_post");
    }

    g->awaited_players = p->players_by_id;
    assert(bitset_subset(&g->awaited_players, &p->players_by_id) &&
           bitset_subset(&p->players_by_id, &g->awaited_players));

    fprintf(f_out, "Game defined by %d is going to start: room %d, players ", defined_by, room_id);

    bitset_fprintf_content(&p->players_by_id, f_out);

}

// TODO
bool noone_needs_me(storage *strg, int id) {
    return strg->props.next[0] == 0;
}

void main_loop(FILE *f_in, FILE *f_out, storage *strg, int id) {
    bool added_everything = false;

    if (sem_wait(&strg->mutex)) syserr("sem_wait");
    bitset_add(&strg->avl_players_by_id, id);
    multiset_add(&strg->avl_players_by_type, strg->player_type[id]);

    strg->props.checked = false;
    if (sem_post(&strg->mutex)) syserr("sem_post");

    while (true) {

        if (sem_wait(&strg->mutex)) syserr("sem_wait");

        // If I can left
        if (added_everything && !strg->still_adding_propositions && noone_needs_me(strg, id)) {

            bitset_remove(&strg->avl_players_by_id, id);
            multiset_rem(&strg->avl_players_by_type, strg->player_type[id]);

            strg->leaving_queue[strg->no_players_left++] = id;

            if (sem_post(&strg->mutex)) syserr("sem_post");
            fprintf(f_out, "Player %d left after %d game(s).\n", id, strg->no_of_games[id]);
            return;
        }

        // If I want to add my proposition
        if (!added_everything && strg->props.prop[id].not_initialized == false) {
//            printf("Player %d adding a proposition\n", id);
            enum ReturnValue ret;
            while ((ret = add_proposition(f_in, f_out, strg, id)) == INVALID_PROP) {}
            if (ret == END_OF_PROPS) {
                added_everything = true;
                strg->still_adding_propositions--;
                if (!strg->still_adding_propositions) {
                    for (int i = 1; i <= strg->no_players; i++) { // todo szachrajstwo
                        if (strg->assigned_room[i] == 0) {
                            strg->assigned_room[i] = -1;
                            if (sem_post(&strg->private_sem[i])) syserr("sem_post");
                        }
                    }
                }
            } else { // ret == SUCCESS
                if (strg->props.checked) {
                    int room_id;
                    if ((room_id = check_conditions(strg, id))) {
//                        printf("Player %d initialize game no %d\n", id, id);
                        initialize_game(f_out, strg, id, room_id);
                    }
                }
            }
        }

        // If I want to check
        if (!strg->props.checked) {
            for (int i = strg->props.next[0]; i != 0; i = strg->props.next[i]) {
                int room_id;
                if ((room_id = check_conditions(strg, i))) {
//                    printf("Player %d initialize game no %d\n", id, i);
                    initialize_game(f_out, strg, i, room_id);
                }
            }
            strg->props.checked = true;
        }

//        printf("Player %d idzie spać\n", id);
        if (sem_post(&strg->mutex)) syserr("sem_post");
        if (sem_wait(&strg->private_sem[id])) syserr("sem_wait");

        if (sem_wait(&strg->mutex)) syserr("sem_wait");
        int goal_room = strg->assigned_room[id];

//        printf("Player %d: idę do pokoju %d!\n", id, goal_room);

        if (goal_room == -1) {
            if (sem_post(&strg->mutex)) syserr("sem_post");
            continue;
        }

        // I'm entering the game
        game *g = &strg->rooms[goal_room].curr_game;
        assert(bitset_check(&g->awaited_players, id));
        bitset_remove(&g->awaited_players, id);
        fprintf(f_out, "Entered room %d, game defined by %d, waiting for players ",
                goal_room, g->defined_by);

        bitset_fprintf_content(&g->awaited_players, f_out);

        if (bitset_get_next(&g->awaited_players, 0) != 0) {
            if (sem_post(&strg->mutex)) syserr("sem_post");
            if (sem_wait(&g->wait_for_the_rest)) syserr("sem_wait");
        }

        g->no_players_left_to_enter--;
        if (g->no_players_left_to_enter) {
//            printf("Player %d is in the game\n", id);
            if (sem_post(&g->wait_for_the_rest)) syserr("sem_post");
        } else {
//            printf("Player %d is (last) in the game\n", id);
            if (sem_post(&strg->mutex)) syserr("sem_post");
        }

        // I'm in the game
//        printf("Player %d is in the game\n", id);

        fprintf(f_out, "Left room %d\n", goal_room);
        // I'm exiting the game
        if (sem_wait(&strg->mutex)) syserr("sem_wait");
//        printf("Player %d exited the game in room %d\n", id, goal_room);

        g->no_players_left_to_exit--;
        if (!g->no_players_left_to_exit) {
            strg->rooms[goal_room].is_free = true;
        }

        bitset_add(&strg->avl_players_by_id, id);
        multiset_add(&strg->avl_players_by_type, strg->player_type[id]);
        strg->props.checked = false;
        strg->assigned_room[id] = 0;
        strg->no_of_games[id]++;

        if (sem_post(&strg->mutex)) syserr("sem_post");

    }
}

int main(int argc, char **argv) {

    int id;
    char fav_room;
    FILE *f_in;
    FILE *f_out;
    char filepath_in[15]; // 15 == strlen("player-((MAX_ID)).in")
    char filepath_out[15]; // 15 == strlen("player-((MAX_ID)).out")
    storage *strg;

    assert(argc == 2 && "Invalid number of program arguments. (Expected: 1)");
    id = atoi(argv[1]);

    // open files to read/write
    sprintf(filepath_in, "player-%d.in", id);
    f_in = fopen(filepath_in, "r");
    if (f_in == (FILE *) -1) syserr("Error opening file");

    sprintf(filepath_out, "player-%d.out", id);
    f_out = fopen(filepath_out, "w");
    if (f_out == (FILE *) -1) syserr("Error opening file");

    strg = get_storage();

    fscanf(f_in, "%c\n", &fav_room);
    strg->player_type[id] = fav_room;
    multiset_add(&strg->total_players_by_type, strg->player_type[id]);

//    if (DEBUG) printf("Player %d tries to enter\n", id);

    if (sem_post(&strg->enter_manager)) syserr("sem_post");
    if (sem_wait(&strg->enter_player)) syserr("sem_wait");

//    if (DEBUG) printf("Player %d entered\n", id);

    main_loop(f_in, f_out, strg, id);

    // cleanup
    fclose(f_in);
    fclose(f_out);
    clean_memory(strg);

//    if (DEBUG) printf("Player %d ded\n", id);
}

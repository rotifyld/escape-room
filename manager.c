#include <time.h>
#include <unistd.h>
#include <string.h>
#include <stdio.h>
#include <sys/mman.h>
#include <semaphore.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <assert.h>

#include "err.h"
#include "shared_storage.h"

void initialize_players(storage *strg) {
    pid_t pid;
    char id[5];
    char *args[] = {"./player", id, NULL};

    for (int i = 1; i <= strg->no_players; ++i) {
        pid = fork();

        if (pid == -1) {
            syserr("fork");
        } else if (pid == 0) {
            sprintf(id, "%d", i);

            execvp(args[0], args);
            syserr("exec");
        }
    }
}

void get_room_data(FILE *f_in, storage *strg) {
    char type;
    int capacity;
    for (int i = 1; i <= strg->no_rooms; ++i) {
        fscanf(f_in, "%c %d", &type, &capacity);
        room_new(strg, type, capacity, i);
        if (strg->max_capacity[TYPE_TO_INT(type)] < capacity) {
            strg->max_capacity[TYPE_TO_INT(type)] = capacity;
        }
        fgetc(f_in); // '\n', albo EOF
    }
}


int main() {

    FILE *f_in;
    FILE *f_out;
    storage *strg;

    // open files to read/write
    f_in = fopen("manager.in", "r");
    if (f_in == (FILE *) -1) syserr("Error opening file");

    f_out = fopen("manager.out", "w");
    if (f_out == (FILE *) -1) syserr("Error opening file");

    strg = initialize_storage();

    // get values of number of players and rooms
    fscanf(f_in, "%d %d\n", &strg->no_players, &strg->no_rooms);
    strg->still_adding_propositions = strg->no_players;

    initialize_players(strg);

    get_room_data(f_in, strg);

//    if (DEBUG) printf("Mgr tries to enter\n");

    // entering building
    for (int i = 0; i < strg->no_players; ++i) {
        if (sem_wait(&strg->enter_manager)) syserr("sem_wait");
    }
    for (int i = 0; i < strg->no_players; ++i) {
        if (sem_post(&strg->enter_player)) syserr("sem_post");
    }

    unlink_shm(strg);

    for (int i = 0; i < strg->no_players; ++i) {
        wait(0);
        fprintf(f_out, "Player %d left after %d game(s)\n",
                strg->leaving_queue[i],
                strg->no_of_games[strg->leaving_queue[i]]);
    }

    // cleanup
    fclose(f_in);
    fclose(f_out);

    free_storage(strg);

//    if (DEBUG) printf("Manager ded\n");

}
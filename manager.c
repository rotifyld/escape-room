#include <stdio.h>
#include <string.h>
#include <semaphore.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>

#include "err.h"
#include "shared_storage.h"

void initialize_players(storage *strg) {
    pid_t pid;
    char id[7];
    char *args[] = {"./player", id, NULL};

    for (short i = 1; i <= strg->no_players; ++i) {
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

void get_room_data(storage *strg) {
    char type;
    int capacity;
    for (int i = 1; i <= strg->no_rooms; ++i) {
        if (scanf("%c %d", &type, &capacity) == EOF) syserr("scanf");
        room_new(strg, type, capacity, i);
        if (strg->max_capacity[TYPE_TO_INT(type)] < capacity) {
            strg->max_capacity[TYPE_TO_INT(type)] = capacity;
        }
        getc(stdin); // '\n', albo EOF
    }
}

int main() {

    FILE *f_out;
    storage *strg;

    f_out = fopen("manager.out", "w");
    if (f_out == (FILE *) -1) syserr("Error opening file");

    strg = initialize_storage();

    // get values of number of players and rooms
    if (scanf("%d %d\n", &strg->no_players, &strg->no_rooms) == EOF) syserr("scanf");
    strg->still_adding_propositions = strg->no_players;

    initialize_players(strg);

    get_room_data(strg);

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
    fclose(f_out);
    free_storage(strg);
}
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

int main() {

    int players;
    int rooms;
    FILE *f_in;
    FILE *f_out;
    Storage *strg;
    sem_t *mutex = NULL;
    sem_t *enter_counter = NULL;
    sem_t *enter_wait = NULL;
    sem_t *manager = NULL;

    // open files to read/write
    f_in = fopen("manager.in", "r");
    if (f_in == (FILE *) -1) syserr("Error opening file");

    f_out = fopen("manager.out", "w");
    if (f_out == (FILE *) -1) syserr("Error opening file");

    strg = initialize_storage();
    open_semaphores(&mutex, &enter_counter, &enter_wait, &manager);

    // get data
    fscanf(f_in, "%d %d\n", &players, &rooms);
    for (int i = 0; i < rooms; ++i) {

    }

    if (DEBUG) printf("Mgr tries to enter\n");

    // entering building
    for (int i = 0; i < players; ++i) {
        if (DEBUG) cout_semaphores(mutex, enter_counter, enter_wait, manager);
        if (sem_wait(enter_counter)) syserr("sem_wait");
    }
    for (int i = 0; i < players; ++i) {
        if (DEBUG) cout_semaphores(mutex, enter_counter, enter_wait, manager);
        if (sem_post(enter_wait)) syserr("sem_post");
        if (DEBUG) cout_semaphores(mutex, enter_counter, enter_wait, manager);
    }

    if (DEBUG) printf("Mgr entered\n");

    // exiting building
    int written_exited_players = 0;
    for (int i = 0; i < players; i++) {
        if (sem_wait(manager)) syserr("sem_wait");
        int exiting_id = strg->exited_list[written_exited_players++];
        fprintf(f_out, "Player %d left after %d game(s).\n",
                exiting_id, strg->no_of_games[exiting_id]);
    }

    if (DEBUG) printf("Mgr exited\n");

    // cleanup
    fclose(f_in);
    fclose(f_out);

    close_semaphores(mutex, enter_counter, enter_wait, manager);
    free_storage(strg);

    if (DEBUG) printf("Manager ded\n");

}
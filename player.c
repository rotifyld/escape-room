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
#include <stdlib.h>

// todo syserr:
// in file: %s at line %d", __FILE__, __LINE__)

#include "err.h"
#include "shared_storage.h"


int main(int argc, char **argv) {

    int id;
    FILE *f_in;
    FILE *f_out;
    char filepath_in[15]; // 15 == strlen("player-((MAX_ID)).in")
    char filepath_out[15]; // 15 == strlen("player-((MAX_ID)).out")
    Storage *strg;
    sem_t *mutex = NULL;
    sem_t *enter_counter = NULL;
    sem_t *enter_wait = NULL;
    sem_t *manager = NULL;

    assert(argc == 2 && "Invalid number of program arguments. (Expected: 1)");
    id = atoi(argv[1]);

    // open files to read/write
    sprintf(filepath_in, "player-%d.in", id);
    f_in = fopen(filepath_in, "r");
    if (f_in == (FILE *) -1) syserr("Error opening file");

    sprintf(filepath_out, "player-%d.out", id);
    f_out = fopen(filepath_out, "w");
    if (f_out == (FILE *) -1) syserr("Error opening file");

    strg = initialize_storage();
    open_semaphores(&mutex, &enter_counter, &enter_wait, &manager);

    if (DEBUG) printf("Player %d tries to enter\n", id);

    // entering building
    if (sem_post(enter_counter)) syserr("sem_post");
    if (sem_wait(enter_wait)) syserr("sem_wait");

    if (DEBUG) printf("Player %d entered\n", id);

    // todo wszsytko



    // todo end wszsytko

    // exiting building
    if (sem_wait(mutex)) syserr("sem_wait");
    strg->exiting_buffer[strg->exited++] = id;
    if (sem_post(mutex)) syserr("sem_post");
    if (sem_post(manager)) syserr("sem_post");

    if (DEBUG) printf("Player %d exited\n", id);

    // cleanup
    fclose(f_in);
    fclose(f_out);

    close_semaphores(mutex, enter_counter, enter_wait, manager);

    if (DEBUG) printf("Player %d ded\n", id);
}
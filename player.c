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


    int value = -1;
    sem_getvalue(strg->mutex, &value);
    printf("before = %d\n", value);
    sem_post(strg->mutex);
    sem_getvalue(strg->mutex, &value);
    printf("after = %d\n", value);


//    if (id == 1) {
//        if (sem_wait(&strg->mutex)) syserr("sem_wait");
//    } else {
//        if (sem_post(&strg->mutex)) syserr("sem_post");
//    }


    // cleanup
    fclose(f_in);
    fclose(f_out);

    printf("Player %d ded\n", id);
}
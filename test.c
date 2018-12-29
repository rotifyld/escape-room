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

#define NO_PLAYERS 3

int main() {

    plst_t *pl = plst_new();
    int *playersA = malloc(sizeof(int) * 2);
    int *playersB = malloc(sizeof(int) * 2);
    int *playersC = malloc(sizeof(int) * 2);
    int *playersD = malloc(sizeof(int) * 2);

    plst_add(pl, 'A', 5, playersA);
    plst_add(pl, 'B', 4, playersB);
    plst_add(pl, 'C', 3, playersC);
    plst_add(pl, 'D', 2, playersD);
    plst_cout(pl);

    plst_rem(pl, pl->front);
    plst_cout(pl);

    plst_delete(pl);

}
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

#include "err.h"
#include "shared_storage.h"

#define NO_PLAYERS 2

int main() {
    int i;
    pid_t pid;

    Storage * strg = initialize_storage();

    for (i = 0; i < NO_PLAYERS + 1; ++i) {

        switch (pid = fork()) {
            case -1:
                syserr("fork");
            case 0:
                if (i == 0) {
                    char *args[] = {"./manager", NULL};
                    execvp(args[0], args);
                    syserr("exec");
                    // ./manager
                } else {
                    char next_id[5];
                    char *args[] = {"./player", next_id, NULL};

                    sprintf(next_id, "%d", i);

                    execvp(args[0], args);
                    syserr("exec");
                    // ./player i
                }
                return 0;
            default:
                break;
        }
    }

    for (i = 0; i < NO_PLAYERS + 1; ++i) wait(0);

    free_storage(strg);

}
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
    int i;
    FILE *manager_in;
    pid_t pid;

    clean_memory();

    manager_in = fopen("manager.in", "w");
    if (manager_in == (FILE *) -1) syserr("Error opening file");
    fprintf(manager_in, "%d 4\n"
                        "A 4\n"
                        "B 6\n"
                        "C 8\n"
                        "A 10", NO_PLAYERS);
    fclose(manager_in);

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
}
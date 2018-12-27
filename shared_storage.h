#ifndef ESCAPE_ROOM_SHARED_STORAGE_H
#define ESCAPE_ROOM_SHARED_STORAGE_H

#include <stdbool.h>
#include <semaphore.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/mman.h>
#include <unistd.h>
#include <stdio.h>

#include "err.h"

//#include "concurrent_proprositions_list.h"

#define MAX_ID 1024
#define MAX_ROOMS 1024

#define SHM_NAME        "/escape_room_shared_memory"
#define MUTEX_NAME      "/escape_room_mutex"
#define STORAGE_SIZE       (sizeof(Storage))


typedef struct Storage {

    sem_t * mutex;

    int entered_counter;

//    struct concurrent_propositions_list *propositions;

    /*
     * 0 - player doesn't have assigned room.
     * 1~1024 - room where the player is, or is about to be.
     * -1 - player can exit.
     * Assignment from 0 to any other number is made by manager.
     * Assignment to 0 is made by player at exiting a room.
     */
    int player_room[MAX_ID];

    /*
     * read - manager
     * write - players
     */
    bool ended_submiting[MAX_ID];

    int fd_memory;

} Storage;


Storage *initialize_storage() {
    Storage *strg;
    int fd_memory;

    fd_memory = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_memory == -1) syserr("Open shared memory");

    ftruncate(fd_memory, STORAGE_SIZE);
    if (fd_memory == -1) syserr("Truncate memory");

    strg = (Storage *) mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_memory, 0);



    if (strg == MAP_FAILED) syserr("mmap");

    strg->fd_memory = fd_memory;

    sem_t * mutex = sem_open(MUTEX_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (mutex == SEM_FAILED) syserr("Semaphore open");
    strg->mutex = mutex;

    return strg;
}


void free_storage(Storage *strg) {
    printf("unlinking %p\n", strg);
    close(strg->fd_memory);
    shm_unlink(SHM_NAME);

    sem_close(strg->mutex);
    sem_unlink(MUTEX_NAME);

    munmap(strg, STORAGE_SIZE);
}

#endif //ESCAPE_ROOM_SHARED_STORAGE_H

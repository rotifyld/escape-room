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

#define SHM_NAME                "/escape_room_shared_memory"
#define MUTEX_NAME              "/escape_room_mutex"
#define ENTER_COUNTER_NAME      "/escape_room_enter_exit_counter"
#define ENTER_WAIT_NAME         "/escape_room_enter_exit_wait"
#define MANAGER_NAME            "/escape_room_manager"
#define STORAGE_SIZE            (sizeof(Storage))

#define DEBUG   true
#define DEBUGDEBUG false

typedef struct Storage {


//    struct concurrent_propositions_list *propositions;


    /*
     * 0 - player doesn't have assigned room.
     * 1~1024 - room where the player is, or is about to be.
     * -1 - player can exit.
     * Assignment from 0 to any other number is made by manager.
     * Assignment to 0 is made by player at exiting a room.
     */
    int player_room[MAX_ID];

    int played_games[MAX_ID];

    int exiting_buffer[MAX_ID];

    int exited;

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

    return strg;
}


void free_storage(Storage *strg) {
    printf("unlinking %p\n", strg);
    close(strg->fd_memory);
    shm_unlink(SHM_NAME);

    sem_unlink(MUTEX_NAME);
    sem_unlink(ENTER_COUNTER_NAME);
    sem_unlink(ENTER_WAIT_NAME);
    sem_unlink(MANAGER_NAME);

    munmap(strg, STORAGE_SIZE);
}

void clean_memory(void) {
    shm_unlink(SHM_NAME);
    sem_unlink(MUTEX_NAME);
    sem_unlink(ENTER_COUNTER_NAME);
    sem_unlink(ENTER_WAIT_NAME);
    sem_unlink(MANAGER_NAME);
}

void cout_semaphores(sem_t *mutex, sem_t *enter_counter, sem_t *enter_wait, sem_t *manager) {
    if (DEBUGDEBUG) {
        int value;
        sem_getvalue(mutex, &value); // tu się wszystko zatrzymuje
        printf("mtx = %d ", value);
        sem_getvalue(enter_counter, &value);
        printf("ent_cnt = %d ", value);
        sem_getvalue(enter_wait, &value);
        printf("ent_wai = %d ", value);
        sem_getvalue(manager, &value);
        printf("mng = %d\n", value);
    }
}

void open_semaphores(sem_t **mutex, sem_t **enter_counter, sem_t **enter_wait, sem_t **manager) {

    *mutex = sem_open(MUTEX_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    if (*mutex == SEM_FAILED) syserr("Semaphore open");

    *enter_counter = sem_open(ENTER_COUNTER_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (*enter_counter == SEM_FAILED) syserr("Semaphore open");

    *enter_wait = sem_open(ENTER_WAIT_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (*enter_wait == SEM_FAILED) syserr("Semaphore open");

    *manager = sem_open(MANAGER_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (*manager == SEM_FAILED) syserr("Semaphore open");
}

void close_semaphores(sem_t *mutex, sem_t *enter_counter, sem_t *enter_wait, sem_t *manager) {

    sem_close(mutex);
    sem_close(enter_counter);
    sem_close(enter_wait);
    sem_close(manager);
}

#endif //ESCAPE_ROOM_SHARED_STORAGE_H

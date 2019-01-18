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
//#include "rooms.h"
#include "multiset.h"
#include "bitset.h"
#include "propositions.h"

#define MAX_ID          1024
#define MAX_ROOMS       1024
#define NO_ROOM_TYPES   ('Z' - 'A' + 1)
#define TYPE_TO_INT(c)  ((c) - 'A')


#define SHM_NAME                "/escape_room_shared_memory"
#define STORAGE_SIZE            (sizeof(storage))

#define DEBUG   true
#define DEBUGDEBUG false

typedef struct game {
    // room id is defined by index in a table
    sem_t wait_for_the_rest;
    int defined_by;
    bitset awaited_players;

    int no_players_left_to_enter;
    int no_players_left_to_exit;
} game;

typedef struct room {
    char type;
    int capacity;
    int id;
    bool is_free;

    game curr_game;
} room;

typedef struct storage {

    // Synchronization tools

    sem_t mutex;
    sem_t enter_manager;
    sem_t enter_player;
    sem_t private_sem[MAX_ID + 1];

    int still_adding_propositions;
    int no_players_left;
    int leaving_queue[MAX_ID];

    // Propositions

    propositions props;

    // Rooms:

    int no_rooms;
    int max_capacity[NO_ROOM_TYPES];
    room rooms[MAX_ROOMS + 1];

    // Players:

    int no_players;
    char player_type[MAX_ID + 1];
    multiset total_players_by_type;
    bitset avl_players_by_id;
    multiset avl_players_by_type;

    /*
     * 0 - player doesn't have assigned room.
     * 1~1024 - room where the player is, or is about to be.
     * -1 - player can exit.
     * Assignment to 0 is made by player at exiting a room.
     */
    int assigned_room[MAX_ID + 1];

    int no_of_games[MAX_ID + 1];

    // Cleanup.

    int fd_memory;

} storage;

void room_new(storage * strg, char type, int capacity, int id) {
    room r;
    r.type = type;
    r.capacity = capacity;
    r.id = id;
    r.is_free = true;
    game g;
    memset(&g, 0, sizeof(game));
    r.curr_game = g;
    strg->rooms[id] = r;
    if (sem_init(&strg->rooms[id].curr_game.wait_for_the_rest, 1, 0)) syserr("sem_init");
}


storage *get_storage() {
    storage *strg;
    int fd_memory;

    fd_memory = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_memory == -1) syserr("Open shared memory");

    strg = (storage *) mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_memory, 0);
    if (strg == MAP_FAILED) syserr("mmap");

    return strg;
}

// Invoked by manager
storage *initialize_storage() {
    storage *strg;
    int fd_memory;

    fd_memory = shm_open(SHM_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR);
    if (fd_memory == -1) syserr("Open shared memory");

    ftruncate(fd_memory, STORAGE_SIZE);
    if (fd_memory == -1) syserr("Truncate memory");

    strg = (storage *) mmap(NULL, STORAGE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd_memory, 0);
    if (strg == MAP_FAILED) syserr("mmap");

    memset(strg, 0, STORAGE_SIZE);

    if (sem_init(&strg->mutex, 1, 1)) syserr("sem_init");
    if (sem_init(&strg->enter_manager, 1, 0)) syserr("sem_init");
    if (sem_init(&strg->enter_player, 1, 0)) syserr("sem_init");
    for (int i = 1; i <= MAX_ID; i++) {
        if (sem_init(&strg->private_sem[i], 1, 0)) syserr("sem_init");
    }

    multiset_new(&strg->total_players_by_type);
    multiset_new(&strg->avl_players_by_type);
    bitset_new(&strg->avl_players_by_id);

    strg->fd_memory = fd_memory;

    return strg;
}

// invoked by parent process after all child processes acquired and mmaped shared memory
void unlink_shm(storage * strg) {
    close(strg->fd_memory);
    shm_unlink(SHM_NAME);
}

// invoked by parent process at the end of a program
void free_storage(storage *strg) {

    if (sem_destroy(&strg->mutex)) syserr("sem_destroy");
    if (sem_destroy(&strg->enter_manager)) syserr("sem_destroy");
    if (sem_destroy(&strg->enter_player)) syserr("sem_destroy");
    for (int i = 1; i <= MAX_ID; i++) {
        if (sem_destroy(&strg->private_sem[i])) syserr("sem_destroy");
    }

    munmap(strg, STORAGE_SIZE);
}

// invoked by child processes
void clean_memory(storage * strg) {
    munmap(strg, STORAGE_SIZE);
}

void cout_semaphores(storage *strg) {
    int value;
    sem_getvalue(&strg->mutex, &value); // tu się wszystko zatrzymuje
    printf("mtx = %d\n", value);

}

#endif //ESCAPE_ROOM_SHARED_STORAGE_H

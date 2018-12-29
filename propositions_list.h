#ifndef ESCAPE_ROOM_PROPROSITIONS_LIST_H
#define ESCAPE_ROOM_PROPROSITIONS_LIST_H

#include <stdlib.h>
#include <semaphore.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>

#include "err.h"

#define MAX_ID 1024

#define LIST_MUTEX_NAME "/escape_room_plst_mutex"
#define LIST_READ_NAME "/escape_room_plst_read"
#define LIST_WRITE_NAME "/escape_room_plst_write"

typedef struct plst_node_t {
    struct plst_node_t *next;
    struct plst_node_t *prev;
    char room_type;
    int no_players; // todo wyrzucić
    int *players_by_id;
    char *players_by_room;
} plst_node_t;

typedef struct plst_t {
    plst_node_t *front;
    plst_node_t *back;
    sem_t *mutex;
    sem_t *read;
    sem_t *write;
    int writing;
    int reading;
    int want_read;
    int want_write;
} plst_t;

plst_t *plst_new(void) {
    plst_t *pl = malloc(sizeof(plst_t));
    pl->front = NULL;
    pl->back = NULL;
    pl->mutex = sem_open(LIST_MUTEX_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 1);
    if (pl->mutex == SEM_FAILED) syserr("Semaphore open");
    pl->read = sem_open(LIST_READ_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (pl->read == SEM_FAILED) syserr("Semaphore open");
    pl->write = sem_open(LIST_WRITE_NAME, O_CREAT | O_RDWR, S_IRUSR | S_IWUSR, 0);
    if (pl->write == SEM_FAILED) syserr("Semaphore open");
    pl->want_read = 0;
    pl->want_write = 0;
    pl->writing = 0;
    pl->reading = 0;

    return pl;
}

void plst_delete(plst_t *pl) {
    sem_close(pl->mutex);
    sem_close(pl->read);
    sem_close(pl->write);
    sem_unlink(LIST_MUTEX_NAME);
    sem_unlink(LIST_READ_NAME);
    sem_unlink(LIST_WRITE_NAME);

    plst_node_t *pl_node = pl->front;
    while (pl_node != NULL) {
        free(pl_node->players_by_id);
        plst_node_t *tmp = pl_node;
        pl_node = pl_node->next;
        free(tmp);
    }
    free(pl);
}

void plst_start_read(plst_t *pl) {
}

void plst_end_read(plst_t *pl) {


}

void plst_start_write(plst_t *pl) {
    if (sem_wait(pl->mutex)) syserr("sem_wait");
    if (pl->writing || pl->reading) {
        pl->want_write++;
        if (sem_post(pl->mutex)) syserr("sem_post");
        if (sem_wait(pl->write)) syserr("sem_wait"); // dziedziczenie sekcji krytycznej
    }
    pl->want_write--;
    pl->writing++;
    if (sem_post(pl->mutex)) syserr("sem_post");
}

void plst_end_write(plst_t *pl) {
    if (sem_wait(pl->mutex)) syserr("sem_wait");
    pl->writing--;
    if (pl->want_write) {
        if (sem_post(pl->write)) syserr("sem_post");
    } else if (pl->want_read) {
        if (sem_post(pl->read)) syserr("sem_post");
    } else {
        if (sem_post(pl->mutex)) syserr("sem_post");
    }
}

//if (sem_wait(pl->mutex)) syserr("sem_wait");
//if (sem_post(pl->mutex)) syserr("sem_post");


void plst_add(plst_t *pl, char room_type, int no_players, int *req_players) {
    plst_node_t *pl_node = malloc(sizeof(plst_node_t));
    pl_node->room_type = room_type;
    pl_node->no_players = no_players;
    pl_node->players_by_id = req_players;

    pl_node->next = NULL;
    pl_node->prev = pl->back;
    if (pl->back) {
        pl->back->next = pl_node;
    } else {
        pl->front = pl_node;
    }
    pl->back = pl_node;
}

void plst_rem(plst_t *pl, plst_node_t *pl_node) {
    if (pl_node->prev) {
        if (pl_node->next) {
            pl_node->next->prev = pl_node->prev;
            pl_node->prev->next = pl_node->next;
        } else {
            pl->back = pl_node->prev;
            pl_node->prev->next = NULL;
        }
    } else {
        if (pl_node->next) {
            pl->front = pl_node->next;
            pl_node->next->prev = NULL;
        } else {
            pl->front = NULL;
            pl->back = NULL;
        }
    }

    free(pl_node->players_by_id);
    free(pl_node);
}

void plst_cout(plst_t *pl) {
    plst_node_t *pl_node = pl->front;
    printf("pl %p:\n"
           "   FRONT -> %c%d\n"
           "   BACK --> %c%d\n",
           pl,
           (pl->front) ? pl->front->room_type : ' ',
           (pl->front) ? pl->front->no_players : 0,
           (pl->back) ? pl->back->room_type : ' ',
           (pl->back) ? pl->back->no_players : 0);
    while (pl_node) {
        printf("   %c%d <- (%c%d) -> %c%d\n",
               (pl_node->prev) ? pl_node->prev->room_type : ' ',
               (pl_node->prev) ? pl_node->prev->no_players : 0,
               pl_node->room_type, pl_node->no_players,
               (pl_node->next) ? pl_node->next->room_type : ' ',
               (pl_node->next) ? pl_node->next->no_players : 0);
        pl_node = pl_node->next;
    }
}


#endif //ESCAPE_ROOM_PROPROSITIONS_LIST_H

#ifndef ESCAPE_ROOM_ROOMS_H
#define ESCAPE_ROOM_ROOMS_H

#define ROOMS_FIRST  'A'
#define ROOMS_LAST   'Z'
#define ROOMS_SIZE   (ROOMS_LAST - ROOMS_FIRST + 1)
#define TYPE_TO_INT(c)  ((c) - ROOMS_FIRST)

// todo TMP
#define ROOMS_PRINT     10

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <assert.h>

// one way linked-linked sorted descending
typedef struct rnode {
    int capacity;
    int id;
    struct rnode *next;
} rnode;

typedef struct rooms {
    rnode *available[ROOMS_SIZE];
} rooms;

void rooms_new(rooms *r) {
//    memset(r->available, 0, sizeof(r->available)); // todo nie wiem, czy to potrzebne
    for (int i = 0; i < ROOMS_SIZE; i++) {
        rnode *dummy = malloc(sizeof(rnode));
        dummy->capacity = 0;
        dummy->id = 0;
        dummy->next = NULL;
        r->available[i] = dummy;
    }
}

void rooms_delete(rooms *r) {
    // todo usunąć elementy listy
    free(r);
}

void rooms_add(rooms *r, char type, int capacity, int id) {
    rnode *head = r->available[TYPE_TO_INT(type)];
    rnode *to_add = malloc(sizeof(rnode));
    to_add->capacity = capacity;
    to_add->id = id;

    if (head->capacity <= capacity) {
        to_add->next = head;
        r->available[TYPE_TO_INT(type)] = to_add;
    } else {
        while (head->next) {
            if (head->next->capacity <= capacity) {
                to_add->next = head->next;
                head->next = to_add;
                return;
            }
            head = head->next;
        }
        head->next = to_add;
        to_add->next = NULL;
    }
}

int rooms_biggest(rooms* r, char type) {
    return r->available[TYPE_TO_INT(type)]->capacity;
}

// draws smallest available room of given type, removes it from structure, and returns its id
// assumes that the room of given size exists (must be checked by user)
int rooms_draw(rooms *r, char type, int min_capacity) {
    rnode *head = r->available[TYPE_TO_INT(type)];
    rnode *ret = NULL;

    if (head->next->capacity < min_capacity) {
        r->available[TYPE_TO_INT(type)] = head->next;
        ret = head;
    } else {
        while (head->next->next) {
            if (head->next->next->capacity < min_capacity) {
                ret = head->next;
                head->next = ret->next;
                break;
            }
            head = head->next;
        }
    }
    assert(ret);

    int id =  ret->id;
    free(ret);
    return id;
}

void rooms_print(rooms *r) {
    for (int i = 0; i < ROOMS_PRINT; i++) {
        printf("[%c] = ", i + ROOMS_FIRST);
        for (rnode *n = r->available[i]; n != NULL; n = n->next) {
            printf(" %d (%d)", n->capacity, n->id);
        }
        printf("   ");
    }
    printf("\n");
}


#endif //ESCAPE_ROOM_ROOMS_H

//void multiset_add(multiset *m, char c) {
//    m->contents[c - MULTISET_FIRST]++;
//}
//
//void multiset_rem(multiset *m, char c) {
//    m->contents[c - MULTISET_FIRST]--;
//}
//
//void multiset_print(multiset *m) {
//    printf("Multiset: ");
//    for (int i = MULTISET_FIRST; i <= MULTISET_LAST; i++) {
//        printf("%c:%d ", i, m->contents[i - MULTISET_FIRST]);
//    }
//    printf("\n");
//}
//
//// m1 -= m2
//void multiset_minus(multiset * m1, multiset * m2) {
//    for (int i = 0; i < MULTISET_SIZE; i++) {
//        m1->contents[i] -= m2->contents[i];
//    }
//}
//
//// m1 <= m2
//int multiset_subset(multiset *m1, multiset *m2) {
//    for (int i = 0; i < MULTISET_SIZE; i++) {
//        if (m1->contents[i] > m2->contents[i]) return 0;
//    }
//    return 1;
//}
//
//#endif //ESCAPE_ROOM_MULTISET_H

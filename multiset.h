#ifndef ESCAPE_ROOM_MULTISET_H
#define ESCAPE_ROOM_MULTISET_H

#define MULTISET_FIRST  'A'
#define MULTISET_LAST   'Z'
#define MULTISET_SIZE   (MULTISET_LAST - MULTISET_FIRST + 1)

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct multiset {
    int contents[MULTISET_SIZE];
} multiset;

void multiset_init(multiset * m) {
    memset(m->contents, 0, sizeof(m->contents));
}

void multiset_add(multiset *m, char c) {
    m->contents[c - MULTISET_FIRST]++;
}

void multiset_rem(multiset *m, char c) {
    m->contents[c - MULTISET_FIRST]--;
}

// m1 -= m2
void multiset_minus(multiset * m1, multiset * m2) {
    for (int i = 0; i < MULTISET_SIZE; i++) {
        m1->contents[i] -= m2->contents[i];
    }
}

// m1 <= m2
int multiset_subset(multiset *m1, multiset *m2) {
    for (int i = 0; i < MULTISET_SIZE; i++) {
        if (m1->contents[i] > m2->contents[i]) return 0;
    }
    return 1;
}

char multiset_draw(multiset * m) {
    for (int i = 0; i < MULTISET_SIZE; i++) {
        if (m->contents[i] > 0) {
            m->contents[i]--;
            return (char) (i + MULTISET_FIRST);
        }
    }
    return 0;
}

#endif //ESCAPE_ROOM_MULTISET_H

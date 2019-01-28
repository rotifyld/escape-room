#ifndef ESCAPE_ROOM_BITSET_H
#define ESCAPE_ROOM_BITSET_H

#define BITSET_SIZE 1024

#define CONTENT_T           unsigned int
#define BITS_IN_BLOCK       (sizeof(CONTENT_T) * 8)
#define NUMBER_OF_BLOCKS    (BITSET_SIZE / BITS_IN_BLOCK + 1)

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct bitset {
    CONTENT_T contents[NUMBER_OF_BLOCKS];
} bitset;

void bitset_new(bitset *b) {
    memset(b->contents, 0, sizeof(b->contents));
}

void bitset_init(bitset *b) {
    memset(b->contents, 0, sizeof(b->contents));
}

void bitset_add(bitset *b, int n) {
    b->contents[n / BITS_IN_BLOCK] |= (1 << (n % BITS_IN_BLOCK));
}

void bitset_remove(bitset *b, int n) {
    b->contents[n / BITS_IN_BLOCK] &= ~(1 << (n % BITS_IN_BLOCK));
}

int bitset_check(bitset *b, int n) {
    return b->contents[n / BITS_IN_BLOCK] & 1 << (n % BITS_IN_BLOCK);
}

// b1 -= b2
void bitset_minus(bitset *b1, bitset *b2) {
    for (unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++) {
        b1->contents[i] ^= b2->contents[i];
    }
}

// b1 <= b2
int bitset_subset(bitset *b1, bitset *b2) {
    for (unsigned int i = 0; i < NUMBER_OF_BLOCKS; i++) {
        if (b1->contents[i] & ~b2->contents[i]) return 0;
    }
    return 1;
}

int bitset_get_next(bitset *b, int n) {
    for (unsigned int i = (n + 1) / NUMBER_OF_BLOCKS; i < NUMBER_OF_BLOCKS; i++) {
        if (b->contents[i] != 0) {
            for (unsigned int j = (n + 1) % BITS_IN_BLOCK; j < BITS_IN_BLOCK; j++) {
                if (bitset_check(b, j)) {
                    return (BITS_IN_BLOCK * i) + j;
                }
            }
        }
    }
    return 0;
}

void bitset_fprintf_content(bitset *b, FILE *f) {

    fprintf(f, "(");
    int i = bitset_get_next(b, 0);
    if (i != 0) {
        fprintf(f, "%d", i);
        while ((i = bitset_get_next(b, i)) != 0) {
            fprintf(f, ", %d", i);
        }
    }
    fprintf(f, ")\n");
}

#endif //ESCAPE_ROOM_BITSET_H
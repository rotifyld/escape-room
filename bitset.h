#ifndef ESCAPE_ROOM_BITSET_H
#define ESCAPE_ROOM_BITSET_H

#define BITSET_SIZE 1024

#define CONTENT_T           unsigned int
#define BITS_IN_BLOCK       (sizeof(CONTENT_T) * 8)
#define NUMBER_OF_BLOCKS    (BITSET_SIZE / BITS_IN_BLOCK + 1)
#define PRINT_PARTS         2

#include <stdio.h>
#include <stdlib.h>

typedef struct bitset {
    CONTENT_T *contents;
} bitset;

bitset *bitset_new() {
    bitset *b = malloc(sizeof(bitset));
    b->contents = (CONTENT_T *) calloc(NUMBER_OF_BLOCKS, sizeof(CONTENT_T));  // calloc(32, 32);
    return b;
}

void bitset_delete(bitset *b) {
    free(b->contents);
    free(b);
}

void bitset_add(bitset *b, int n) {
    b->contents[n / BITS_IN_BLOCK] |= (1 << (n % BITS_IN_BLOCK));
}

void bitset_let(bitset *b, int n) {
    b->contents[n / BITS_IN_BLOCK] &= ~(1 << (n % BITS_IN_BLOCK));
}

void print_block(CONTENT_T block) {
    for (int j = 0; j < sizeof(CONTENT_T); j++) {
        printf("%c%c%c%c%c%c%c%c ",
               (block & 0x01 ? '1' : '0'),
               (block & 0x02 ? '1' : '0'),
               (block & 0x04 ? '1' : '0'),
               (block & 0x08 ? '1' : '0'),
               (block & 0x10 ? '1' : '0'),
               (block & 0x20 ? '1' : '0'),
               (block & 0x40 ? '1' : '0'),
               (block & 0x80 ? '1' : '0'));
        block >>= 8;
    }
}

void bitset_printf(bitset *b) {
    printf("Bitset: ");
    for (int i = 0; i < PRINT_PARTS; i++) {
        print_block(b->contents[i]);
        printf(" ");
    }
    printf("\n");
}

// b1 -= b2
void bitset_minus(bitset * b1, bitset * b2) {
    for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
        b1->contents[i] ^= b2->contents[i];
    }
}

// b1 <= b2
int bitset_subset(bitset *b1, bitset *b2) {
    for (int i = 0; i < NUMBER_OF_BLOCKS; i++) {
        if (b1->contents[i] & ~b2->contents[i]) return 0;
    }
    return 1;
}

#endif //ESCAPE_ROOM_BITSET_H
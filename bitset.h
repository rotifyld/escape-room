#ifndef ESCAPE_ROOM_BITSET_H
#define ESCAPE_ROOM_BITSET_H

#define BITSET_SIZE 1024

#define CONTENT_T           unsigned int
#define BITS_IN_BLOCK       (sizeof(CONTENT_T) * 8)
#define NUMBER_OF_BLOCKS    (BITSET_SIZE / BITS_IN_BLOCK + 1)
#define PRINT_PARTS         2

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

typedef struct bitset {
    CONTENT_T contents[NUMBER_OF_BLOCKS];
} bitset;

void bitset_new(bitset *b) {
    memset(b->contents, 0, sizeof(b->contents));
}

//bitset *bitset_new() {
//    bitset *b = malloc(sizeof(bitset));
//    b->contents = (CONTENT_T *) calloc(NUMBER_OF_BLOCKS, sizeof(CONTENT_T));  // calloc(32, 32);
//    return b;
//}

void bitset_init(bitset *b) {
    memset(b->contents, 0, sizeof(b->contents));
}

void bitset_delete(bitset *b) {
    free(b->contents);
    free(b);
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

void print_block(CONTENT_T block) {
    for (unsigned int j = 0; j < sizeof(CONTENT_T); j++) {
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
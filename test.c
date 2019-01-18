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
#include "bitset.h"
#include "multiset.h"
#include "rooms.h"

int main() {

}

//int main() {
//    rooms * r = rooms_new();
//    rooms_add(r, 'A', 2, 1);
//    printf("biggest is %d\n", rooms_biggest(r, 'A'));
//    rooms_add(r, 'A', 3, 2);
//    printf("biggest is %d\n", rooms_biggest(r, 'A'));
//    rooms_add(r, 'A', 1, 3);
//    printf("biggest is %d\n", rooms_biggest(r, 'A'));
//    rooms_add(r, 'A', 4, 4);
//    printf("biggest is %d\n", rooms_biggest(r, 'A'));
//    rooms_add(r, 'A', 2, 6);
//    rooms_add(r, 'A', 2, 7);
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("got id = %d, biggest is %d\n", rooms_draw(r, 'A', 1), rooms_biggest(r, 'A'));
//    rooms_print(r);
//    printf("biggest is %d\n", rooms_biggest(r, 'A'));
//}

//int main() {
//
//    multiset * m = multiset_new();
//    multiset_add(m, 'A');
//    multiset_add(m, 'B');
//    multiset_add(m, 'B');
//    multiset_add(m, 'C');
//    multiset_add(m, 'C');
//    multiset_add(m, 'C');
//    multiset_add(m, 'D');
//    multiset_add(m, 'D');
//    multiset_add(m, 'E');
//    multiset_add(m, 'E');
//    multiset_add(m, 'E');
//    multiset_add(m, 'F');
//    multiset_add(m, 'F');
//    multiset_add(m, 'G');
//    multiset_add(m, 'H');
//    multiset_add(m, 'H');
//    multiset_add(m, 'H');
//    multiset_add(m, 'H');
//    multiset_add(m, 'H');
//    multiset_print(m);
//
//
//
//    multiset * m1 = multiset_new();
//    multiset_add(m1, 'A');
//    multiset_add(m1, 'B');
//    multiset_add(m1, 'B');
//    multiset_add(m1, 'C');
//    multiset_add(m1, 'F');
//    multiset_add(m1, 'G');
//    multiset_add(m1, 'H');
//    multiset_add(m1, 'H');
//    multiset_add(m1, 'H');
//    multiset_add(m1, 'H');
//    multiset_add(m1, 'H');
//    multiset_print(m1);
//
//    multiset_minus(m, m1);
//    multiset_print(m);
//
//
//    multiset_delete(m);
//    multiset_delete(m1);
//
//}

//int main() {
//
//    bitset * b1 = bitset_new();
//    bitset * b2 = bitset_new();
//    b1->contents[0] = (1 << BITS_IN_BLOCK) - 1;
//    b2->contents[0] = 83000279;
//    bitset_printf(b1);
//    bitset_printf(b2);
//    bitset_minus(b1, b2);
//    bitset_printf(b1);
//
//}

//int main() {
//
//    plst_t *pl = plst_new();
//    int *playersA = malloc(sizeof(int) * 2);
//    int *playersB = malloc(sizeof(int) * 2);
//    int *playersC = malloc(sizeof(int) * 2);
//    int *playersD = malloc(sizeof(int) * 2);
//
//    plst_add(pl, 'A', 5, playersA);
//    plst_add(pl, 'B', 4, playersB);
//    plst_add(pl, 'C', 3, playersC);
//    plst_add(pl, 'D', 2, playersD);
//    plst_cout(pl);
//
//    plst_rem(pl, pl->front);
//    plst_cout(pl);
//
//    plst_delete(pl);
//
//}
#include <stdio.h>
#include "queue.h"

int main(void)
{
    queue l = queue_init();
    enqueue(&l, (void*)"Hola");
    enqueue(&l, (void*)"Diego");
    fprintf(stdout, "Hola, me llamo %s\n", (char*)peek(&l)->data);
    enqueue(&l, (void*)"JOJO");
    dequeue(&l);
    fprintf(stdout, "Hola, me llamo %s\n", (char*)peek(&l)->data);
}
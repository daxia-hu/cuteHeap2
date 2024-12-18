#include "cute_heap.h"
#include "stdio.h"
int main()
{
    heap_init();
    heap_foreach();
    uint8_t *p1 = heap_malloc(50);
    uint8_t *p2 = heap_malloc(20);
    uint8_t *p3 = heap_malloc(20);
    heap_foreach();
    heap_free(p2);
    heap_foreach();
    uint8_t *p4 = heap_malloc(70);
    heap_foreach();
    heap_free(p3);
    heap_foreach();
    uint8_t *p5 = heap_malloc(30);
    heap_foreach();
}

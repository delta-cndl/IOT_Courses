#include "ringbuffer.h"


void rb_init(ringbuffer_t* rb) {
    rb->head = rb->tail = 0;
    
}

int rb_empty(ringbuffer_t* rb) {
    return rb->head == rb->tail;
}

int rb_full(ringbuffer_t* rb) {
    return ((rb->head + 1) % RB_SIZE) == rb->tail;
}


void rb_put(ringbuffer_t* rb, uint8_t c)
{
    uint32_t next = (rb->head + 1) % RB_SIZE;

    if (next != rb->tail) {
        rb->data[rb->head] = c;
        rb->head = next;
    }
}

uint8_t rb_get(ringbuffer_t* rb)
{
    uint8_t c = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % RB_SIZE;
    return c;
}
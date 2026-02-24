#include "ringbuffer.h"



static ringbuffer_t rx_buffer;
static ringbuffer_t tx_buffer;

void rb_init(ringbuffer_t* rb) {
    rb->head = rb->tail = 0;
    
}

int rb_empty(ringbuffer_t* rb) {
    return rb->head == rb->tail;
}

int rb_full(ringbuffer_t* rb) {
    return ((rb->head + 1) % RB_SIZE) == rb->tail;
}

void rb_put(ringbuffer_t* rb, uint8_t c) {
    if (!rb_full(rb)) {
        rb->data[rb->head] = c;
        rb->head = (rb->head + 1) % RB_SIZE;
    }
}

uint8_t rb_get(ringbuffer_t* rb) {
    uint8_t c = rb->data[rb->tail];
    rb->tail = (rb->tail + 1) % RB_SIZE;
    return c;
}
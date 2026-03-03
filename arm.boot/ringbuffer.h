#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

#define RB_SIZE 128

typedef struct {
    uint8_t data[RB_SIZE];
    volatile uint32_t head;
    volatile uint32_t tail;
} ringbuffer_t;


void rb_init(ringbuffer_t* rb);
int rb_empty(ringbuffer_t* rb);
int rb_full(ringbuffer_t* rb);
void rb_put(ringbuffer_t* rb, uint8_t c);
uint8_t rb_get(ringbuffer_t* rb) ;

#endif
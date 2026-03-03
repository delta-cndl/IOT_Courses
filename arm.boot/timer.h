#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

#define TIMER0 ((void*)0x101E2000)
#define TIMER1 ((void*)0x101E2020)
#define TIMER2 ((void*)0x101E3000)
#define TIMER3 ((void*)0x101E3020)

void timer_init(void* timer_base);

void print_time(void);

uint32_t get_timeElapsed(void);

void timer_callback(uint32_t irq, void* cookie);


#endif
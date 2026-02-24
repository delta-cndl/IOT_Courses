#ifndef TIMER_H
#define TIMER_H

#include <stdint.h>

uint32_t __aeabi_uidiv(uint32_t n, uint32_t d);
void timer_init(uint32_t frequency_hz);
void timer_clear_interrupt(void);
uint32_t timer_get_time(void);


#endif
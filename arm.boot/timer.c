#include "timer.h"
#include "isr.h"
#include "stddef.h"

#define TIMER_BASE  0x101E2000   // à adapter selon ta carte
#define TIMER_LOAD  0x00
#define TIMER_VALUE 0x04
#define TIMER_CTRL  0x08
#define TIMER_INTCLR 0x0C

volatile uint32_t *timer = (volatile uint32_t*)TIMER_BASE;

static volatile uint32_t system_time = 0;

void timer_callback(uint32_t irq, void* cookie) {
    system_time++;
    timer_clear_interrupt();
}

uint32_t __aeabi_uidiv(uint32_t n, uint32_t d) {
    uint32_t q = 0;
    while (n >= d) {
        n -= d;
        q++;
    }
    return q;
}

void timer_init(uint32_t freq_hz) {
    uint32_t load = __aeabi_uidiv(1000,freq_hz ); // dépend de mon  clock

    timer[TIMER_LOAD/4] = load;
    timer[TIMER_CTRL/4] =
        (1 << 7) |  // enable
        (1 << 6) |  // periodic
        (1 << 5) |  // interrupt enable
        (1 << 1);   // 32-bit

    irq_enable(TIMER3_IRQ, timer_callback, NULL);
}

void timer_clear_interrupt() {
    timer[TIMER_INTCLR/4] = 1;
}

uint32_t timer_get_time() {
    return system_time;
}
#include "timer.h"
#include "isr.h"
#include <stdint.h>
#include "stddef.h"
#include "uart.h"
#include <stdio.h>
#include "event.h"


#define TIMER0_LOAD   0x00
#define TIMER0_VALUE  0x04
#define TIMER0_CTRL   0x08
#define TIMER0_CLRINT 0x0C

#define TIMER_CTRL_ENABLE   (1 << 7)
#define TIMER_CTRL_PERIODIC (1 << 6)
#define TIMER_CTRL_INTEN    (1 << 5)
#define TIMER_CTRL_32BIT    (1 << 1)
#define TIMER_CTRL_ONESHOT  0

volatile uint32_t time_elapsed = 0;  // Compteur global des millisecondes

static uint32_t last_display = 0;



uint32_t get_timeElapsed(void) {
    return time_elapsed / 1000;
}

void print_time(void) {
    
    uint32_t sec = get_timeElapsed();
    uint32_t min = sec / 60;
    sec = sec % 60;

    // Convertit minutes et secondes en ASCII
    char buf[32];
    uint32_t ev = event_get_count();

    buf[0] = '0' + (min / 10);
    buf[1] = '0' + (min % 10);
    buf[2] = ':';
    buf[3] = '0' + (sec / 10);
    buf[4] = '0' + (sec % 10);
    buf[5] = ' ';
    buf[6] = 'E';
    buf[7] = '=';

    // afficher seulement 2 digits pour rester simple
    buf[8] = '0' + ((ev / 10) % 10);
    buf[9] = '0' + (ev % 10);
    buf[10] = '\0';

    uart_send_string(UART0, "\r\x1b[k"); // pour ecrire sur la meme ligne
    uart_send_string(UART0, buf);
}

void timer_callback(uint32_t irq, void* cookie) {
    time_elapsed++;
   
    // Affiche le temps toutes les 1000 ms
    if (time_elapsed - last_display >= 500000) {
        last_display = time_elapsed;
        print_time();
        event_post(EVENT_TIMER);
    }
}


void timer_init(void* timer_base_void) {

    volatile uint32_t* timer_base = (volatile uint32_t*)timer_base_void;
    // Disable timer first
    timer_base[TIMER0_CTRL / 4] = 0;

    // Load value 
    timer_base[TIMER0_LOAD / 4] = 100000; 

    // Register IRQ callback
    irq_enable(TIMER0_IRQ, timer_callback, timer_base_void);

    // Enable timer: 
    timer_base[TIMER0_CTRL / 4] = TIMER_CTRL_ENABLE | TIMER_CTRL_PERIODIC | TIMER_CTRL_INTEN | TIMER_CTRL_32BIT;
}
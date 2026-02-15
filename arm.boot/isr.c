#include "isr.h"
#include "isr-mmio.h"
#include <stdint.h>

//#include "irq.S"

// déclarations des fonctions assembleur
extern void _irqs_setup(void);
extern void _irqs_enable(void);
extern void _irqs_disable(void);
extern void _wfi(void);


// type pour les callbacks IRQ
typedef void (*irq_callback_t)(uint32_t, void*);


void irqs_setup() {
    // Appelle la fonction assembleur pour configurer le CPU et la pile IRQ
    _irqs_setup(); // definie dans irq.s
}

void irqs_enable() {
    _irqs_enable(); // definie dans irq.s : clear le bit IRQ_FLAG dans CPSR
}

void irqs_disable() {
    _irqs_disable(); // definie dans irq.s 
}

void wfi(void) {
    _wfi();  // CPU attend interruption
}



// Table des callbacks pour les IRQs
static irq_callback_t irq_callbacks[NIRQS] = {0};
static void* irq_cookies[NIRQS] = {0};

void irq_enable(uint32_t irq, void (*callback)(uint32_t, void*), void* cookie) {
    irq_callbacks[irq] = callback;
    irq_cookies[irq] = cookie;

    volatile uint32_t *vic = (volatile uint32_t*)VIC_BASE_ADDR;
    vic[VICINTENABLE/4] |= (1 << irq);
}


void irq_disable(uint32_t irq) {
    irq_callbacks[irq] = 0;
    irq_cookies[irq] = 0;

    volatile uint32_t *vic = (volatile uint32_t*)VIC_BASE_ADDR;
    vic[VICINTCLEAR/4] = (1 << irq);  // clear la ligne
}


void irq_handler() {
    volatile uint32_t *vic = (volatile uint32_t*)VIC_BASE_ADDR;
    uint32_t irq_status = vic[VICIRQSTATUS/4];

    for (int i = 0; i < NIRQS; i++) {
        if (irq_status & (1 << i)) {
            if (irq_callbacks[i]) {
                irq_callbacks[i](i, irq_cookies[i]);
            }
        }
    }

    // (optionnel) clear all handled IRQs
    // vic[VICINTCLEAR/4] = irq_status;
}



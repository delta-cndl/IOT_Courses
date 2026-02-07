#include "main.h"
#include "uart.h"
#include "console.h"
#include "kprintf.h"
#include "isr.h"


/*
 * Define ECHO_ZZZ to have a periodic reminder that this code is polling
 * the UART, actively. This means the processor is running continuously.
 * Polling is of course not the way to go, the processor should halt in
 * a low-power state and wake-up only to handle an interrupt from the UART.
 * But this would require setting up interrupts...
 */
#define ECHO_ZZZ

void panic() {
  while (1)
	  ;
}

// faire une boucle de 1sec
void wait(){
	for (int i=0; i<1000000; i++){
	}
}

/**
 * This is the C entry point, upcalled once the hardware has been setup properly
 * in assembly language, see the startup.s file.
 */

/* -------------------------------------------------- */
/* ## Version1 : UART en polling                      */
/* -------------------------------------------------- */

// void _start() {

//   uart_send_string(UART0, "\nFor information:\n");
//   uart_send_string(UART0, "  - Quit with \"C-a c\" to get to the QEMU console.\n");
//   uart_send_string(UART0, "  - Then type in \"quit\" to stop QEMU.\n");
//   uart_send_string(UART0, "\nHello world!\n");

//   int i = 0;
//   int count = 0;
//   while (1) {
//     uint8_t c;
// #ifdef ECHO_ZZZ
//     while (0 == uart_receive(UART0, &c)) {
//       count++;
//       if (count > 50000000) {
//         uart_send_string(UART0, "\n\rZzzz....\n\r");
//         count = 0;
//       }
//     }
// #else
//     if (0==uart_receive(UART0,&c))
//       continue;
// #endif
//     if (c == 13) {
//       uart_send(UART0, '\r'); // Move the cursor to the beginning of the line
//       uart_send(UART0, '\n'); // Moves the cursor down one line
//     } else {
//       uart_send(UART0, c);
//     }
//   }
// }


/* --------------------------------------------------------------------*/
/* ## Version2  : UART en polling et affichage des codes ASCII         */
/* ------------------------------------------------------------------- */
// void _start() {

//   int i = 0;
//   int count = 0;
//   while (1) {
//     uint8_t c;
//     if (0==uart_receive(UART0,&c))
//       continue;
//     kprintf("%d ",c);
//    }
// }


/* -------------------------------------------------- */
/* ## version3 : Console avec gestion  des entrées    */
/* -------------------------------------------------- */

// void dummy_callback(char* line) {
//     // nothing to do here for now
// }

// void _start() {
//   int i = 0;
//   int count = 0;
//   uart_send_string(UART0, "\n Started Console\n");
//   console_init(dummy_callback);  
//    while (1) {
//     uint8_t c;
//     if (0 == uart_receive(UART0, &c))
//         continue;
//     console_echo(c);
//     }
// }

/* ------------------------------------------------------------ */
/* ## version4: introduction des interruptions(IRQ)             */
/* ------------------------------------------------------------ */

void uart_callback(uint32_t irq, void* cookie) {
    uint8_t c;
    if (uart_receive(UART0, &c)){
        uart_send(UART0, '#');
        console_echo(c);
    }
         
}

void _start() {
    uart_send_string(UART0, "\n Started Console\n");
    irqs_setup();
    irq_enable(UART0_IRQ, uart_callback, NULL);
    irqs_enable();

    while (1)
        wfi(); // CPU attend un caractère
}






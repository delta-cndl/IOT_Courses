#include "main.h"
#include "uart.h"
#include "console.h"
#include "kprintf.c"


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
// void _start() {

//   uart_send_string(UART0, "\nFor information:\n");
//   uart_send_string(UART0, "  - Quit with \"C-a c\" to get to the QEMU console.\n");
//   uart_send_string(UART0, "  - Then type in \"quit\" to stop QEMU.\n");

//   uart_send_string(UART0, "\nHello world!\n");

//   int i = 0;
//   int count = 0;
//   // while (1) {
//   //   uint8_t c;
//   //   uart_receive(UART0, &c);  // just receiving,  nothing else 
//   // }
//   while (1) {
//     uint8_t c;
//     if (0==uart_receive(UART0,&c))
//       continue;
//     kprintf("%d ",c);
//   }
// }
void dummy_callback(char* line) {
    // Ne fait rien pour l'instant
}

void _start() {
    console_init(dummy_callback);  // <-- passe la fonction ici
    while (1) {
        uint8_t c;
        if (0 == uart_receive(UART0, &c))
            continue;
        console_echo(c);
    }
}




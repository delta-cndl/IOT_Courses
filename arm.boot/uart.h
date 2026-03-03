#ifndef _UART_H_ 
#define _UART_H_ 

#include <stdint.h>

/**
 * Look at the document describing the Versatile Application Board:
 *
 *    Versatile Application Baseboard for ARM926EJ-S (HBI-0118)
 *
 * Also referenced as DUI0225, downloadable from:
 *
 *  http://infocenter.arm.com/help/index.jsp?topic=/com.arm.doc.dui0225d/I999714.html
 *
 * Look at the memory map, section 4.1, page 137, to find the base addresses for all
 * the devices. We only use here the UARTs.
 */
#define UART0 (void*)0x101f1000
#define UART1 (void*)0x101f2000
#define UART2 (void*)0x101f3000

#define UART0_IRQ 12   // documentation a regarder

#define UART_IMSC   0x038
#define UART_MIS    0x040
#define UART_IMSC_RX (1 << 4)   // RX interrupt enable
#define UART_IMSC_TX (1 << 5)  // TX interrupt enable



#define UART_ICR 0x044
#define UART_ICR_RX (1 << 4)


#define UART_INT_RX (1<<4)
#define UART_INT_TX  (1<<5)

#define UART_FR_TXFF (1<<5)
#define UART_FR_RXFE  (1<<4)

/*
 * Receive a byte from the given uart, this is a non-blocking call.
 * Returns 0 if there are no byte available.
 * Returns 1 if a character was read.
 */
int uart_receive(void* uart, uint8_t *b);

/*
 * Sends a byte through the given uart, this is a blocking call.
 * The code spins until there is room in the UART TX FIFO queue 
 * to send the given byte.
 */
void uart_send(void* uart, uint8_t b);

/*
 * This is a wrapper function, provided for simplicity,
 * it sends a C string through the given uart, assuming
 * the characters are 8-bit ASCII characters.
 */
void uart_send_string(void* uart, const unsigned char *s);

//void uart_enable_rx_interrupt(uint32_t uart);

void uart_enable_rx_interrupt(void* uart);


void uart_enable_tx_interrupt(void* uart);
void uart_disable_tx_interrupt(void* uart);

void uart_rx_callback(uint32_t irq, void* cookie) ;
void uart_tx_callback(uint32_t irq, void* cookie) ;

void uart_init(void* uart);

int uart_rx_available(void);
uint8_t uart_get_char(void);

void process_ring(void);


void uart_putc(void* uart, uint8_t c);

#endif /* _UART_H_ */

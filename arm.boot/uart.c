#include "main.h"
#include "uart.h"
#include "ringbuffer.h"
#include "isr.h"
#include "event.h"


/**
 * PL011_T UART
 *     http://infocenter.arm.com/help/topic/com.arm.doc.ddi0183f/DDI0183.pdf
 *
 * UARTDR: Data Register   (0x00)
 *    To read received bytes
 *    To write bytes to send
 *    Bit Fields:
 *      15:12 reserved
 *      11:08 error flags
 *       7:0  data bits
 * UARTFR:  Flag Register  (0x18)
 *    Bit Fields:
 *      7:  TXFE  transmit FIFO empty
 *      6:  RXFF  receive FIFO full
 *      5:  TXFF  transmit FIFO full
 *      4:  RXFE  receive FIFO empty
 *      3:  BUSY  set when the UART is busy transmitting data
 */

#define UART_DR 0x00
#define UART_FR 0x18

#define UART_TXFE (1<<7)
#define UART_RXFF (1<<6)
#define UART_TXFF (1<<5)
#define UART_RXFE (1<<4)
#define UART_BUSY (1<<3)

#define UART_IMSC   0x038
#define UART_IMSC_RX (1 << 4)   // RX interrupt enable
#define UART_IMSC_TX (1 << 5) // new





static ringbuffer_t rx_buffer;
static ringbuffer_t tx_buffer;


/*
 * See "uart.h"
 */
int uart_receive(void* uart, uint8_t *b) {
  uint16_t* uart_fr = (uint16_t*) (uart + UART_FR);
  uint16_t* uart_dr = (uint16_t*) (uart + UART_DR);
  if (*uart_fr & UART_RXFE)
    return 0;
  *b = (uint8_t)(*uart_dr & 0xff);
  return 1;
}

/*
 * See "uart.h"
 */
void uart_send(void* uart, uint8_t b) {
  uint16_t* uart_fr = (uint16_t*) (uart + UART_FR);
  uint16_t* uart_dr = (uint16_t*) (uart + UART_DR);
  while (*uart_fr & UART_TXFF)
    ;
  *uart_dr = (uint16_t)b;
}

/*
 * See "uart.h"
 */
void uart_send_string(void* uart, const unsigned char *s) {
  while (*s != '\0') {
    uart_send(uart, (uint8_t)*s);
    s++;
  }
}

void uart_enable_rx_interrupt(void* uart) {
    volatile uint32_t* base = (volatile uint32_t*)uart;
    base[UART_IMSC / 4] |= UART_INT_RX;
}



void uart_enable_tx_interrupt(void* uart) {
    volatile uint32_t* base = (volatile uint32_t*)uart;
    base[UART_IMSC / 4] |= UART_INT_TX;
}

void uart_disable_tx_interrupt(void* uart) {
    volatile uint32_t* base = (volatile uint32_t*)uart;
    base[UART_IMSC / 4] &= ~UART_INT_TX;
}

void uart_rx_callback(uint32_t irq, void* cookie)
{
    void* uart = cookie;
    volatile uint32_t* base = (volatile uint32_t*)uart;

    uint8_t c;
  
    while (uart_receive(uart, &c)) {
        rb_put(&rx_buffer, c);   // copie vers ring
    }
    event_post(EVENT_UART_RX);

    // Clear RX interrupt 
    base[UART_ICR / 4] = UART_ICR_RX;
}

void uart_interrupt_handler(uint32_t irq, void* cookie)
{
    void* uart = cookie;
    volatile uint32_t* base = (volatile uint32_t*)uart;

    uint32_t mis = base[UART_MIS / 4];

    /* ===== RX interrupt ===== */
    if (mis & UART_INT_RX) {

        uint8_t c;

        while (!(base[UART_FR / 4] & UART_FR_RXFE)) {
            c = base[UART_DR / 4];
            rb_put(&rx_buffer, c);
        }

        base[UART_ICR / 4] = UART_INT_RX;
    }

    /* ===== TX interrupt ===== */
    if (mis & UART_INT_TX) {

        while (!rb_empty(&tx_buffer)) {

            if (base[UART_FR / 4] & UART_FR_TXFF)
                break;

            uint8_t c = rb_get(&tx_buffer);
            base[UART_DR / 4] = c;
        }

        if (rb_empty(&tx_buffer)) {
            uart_disable_tx_interrupt(uart);
        }

        base[UART_ICR / 4] = UART_INT_TX;
    }
}

void uart_putc(void* uart, uint8_t c)
{
    rb_put(&tx_buffer, c);
    
    uart_enable_tx_interrupt(uart);
}

static char line[80];
static int offset = 0;

void process_ring(void)
{
    
    while (!rb_empty(&rx_buffer)) {

      uint8_t c;
      c = rb_get(&rx_buffer);

        //uart_send(UART0, c);   // echo
        uart_putc(UART0,c);
      
    }
}

//Callback TX: appelé par l'IRQ UARt quand le FIFO peut recevoir un caractere 
void uart_tx_callback(uint32_t irq, void* cookie) {
    uint8_t c;
    if (rb_get(&tx_buffer )) {
        uart_send(UART0, c);      // envoie le caractère
    } else {
        // désactive interruption TX
        uart_disable_tx_interrupt(UART0);
    }

}


void uart_init(void* uart) {
    rb_init(&rx_buffer);
    rb_init(&tx_buffer);

    // Enregistrer callback RX
    irq_enable(UART0_IRQ, uart_interrupt_handler, uart);

    // Activer interruption RX dans l’UART
    uart_enable_rx_interrupt(uart);
}

int uart_rx_available(void) {
    return !rb_empty(&rx_buffer);
}

uint8_t uart_get_char(void) {
    uint8_t c;
    c = rb_get(&rx_buffer );

    return c;
}


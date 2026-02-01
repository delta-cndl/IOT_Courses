#include "console.h"
//#include "uart.h"
#include "kprintf.c"


#define ESC "\033" // equal to 27 in decimal

static int cur_row = 1;
static int cur_col = 1;

static char line_buffer[256];
static int line_len = 0;

static void (*line_callback)(char*) = 0;

/* -------------------------------------------------- */
/* Cursor control                                     */
/* -------------------------------------------------- */

void cursor_left()  { kprintf(ESC "[1D"); } // ESC [ 1 D = 27 91  49 68 
                                            // the 1 is optionel
void cursor_right() { kprintf(ESC "[1C"); }
void cursor_up()    { kprintf(ESC "[1A"); }
void cursor_down()  { kprintf(ESC "[1B"); }

void cursor_at(int row, int col) {
  kprintf(ESC "[%d;%dH", row, col);
  cur_row = row;
  cur_col = col;
}

void cursor_position(int* row, int* col) {
  if (row) *row = cur_row;
  if (col) *col = cur_col;
}

void cursor_hide() { kprintf(ESC "[?25l"); }
void cursor_show() { kprintf(ESC "[?25h"); }

/* -------------------------------------------------- */
/* Console control                                    */
/* -------------------------------------------------- */

void console_color(uint8_t color) {
  kprintf(ESC "[%dm", color);
}

void console_clear() {
  kprintf(ESC "[H");   // home
  kprintf(ESC "[2J");  // clear screen
  cursor_at(1, 1);
}

/* -------------------------------------------------- */
/* Initialization                                     */
/* -------------------------------------------------- */

void console_init(void (*callback)(char*)) {
  line_callback = callback;
  line_len = 0;
  console_clear();
  cursor_show();
}

/* -------------------------------------------------- */
/* Echo / input handling                              */
/* -------------------------------------------------- */

void console_echo(uint8_t byte) {

  /* Ctrl-C : clear screen */
  if (byte == 3) {
    console_clear();
    line_len = 0;
    return;
  }

  /* Backspace (DEL or BS) */
  if (byte == 127 || byte == 8) {
    if (line_len > 0) {
      line_len--;
      cursor_left();
      uart_send(UART0, ' ');
      cursor_left();
    }
    return;
  }

  /* Enter */
  if (byte == '\r' || byte == '\n') {
    uart_send(UART0, '\r');
    uart_send(UART0, '\n');
    line_buffer[line_len] = '\0';
    if (line_callback)
      line_callback(line_buffer);
    line_len = 0;
    return;
  }

  /* Printable ASCII */
  if (byte >= 32 && byte <= 126) {
    if (line_len < (int)(sizeof(line_buffer) - 1)) {
      line_buffer[line_len++] = byte;
      uart_send(UART0, byte);
    }
  }

  
}

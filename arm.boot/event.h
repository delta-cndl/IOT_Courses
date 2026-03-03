#ifndef EVENT_H
#define EVENT_H

#include <stdint.h>

typedef enum {
    EVENT_NONE = 0,
    EVENT_TIMER,
    EVENT_UART_RX
} event_type_t;

void event_post(event_type_t type);
event_type_t event_get(void);

uint32_t event_get_count(void);

#endif
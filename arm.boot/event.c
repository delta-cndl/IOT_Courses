#include "event.h"

#define EVENT_QUEUE_SIZE 16

static volatile event_type_t queue[EVENT_QUEUE_SIZE];
static volatile int head = 0;
static volatile int tail = 0;

static volatile uint32_t event_count = 0;

void event_post(event_type_t type)
{
    int next = (head + 1) % EVENT_QUEUE_SIZE;

    if (next == tail) {
        // queue pleine 
        return;
    }

    queue[head] = type;
    head = next;
}

event_type_t event_get(void)
{
    if (head == tail)
        return EVENT_NONE;

    event_type_t type = queue[tail];
    tail = (tail + 1) % EVENT_QUEUE_SIZE;

    event_count ++;
    return type;
}

uint32_t event_get_count(void)
{
    return event_count;
}
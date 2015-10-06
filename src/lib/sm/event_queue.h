#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

// This is responsible for managing the system's event queue.
// The queue is a FIFO linked list.
// All states (including composite states) raise events in this queue.

typedef uint16_t Event;

#define NULL_EVENT (Event)UINT_MAX
#define QUEUE_EMPTY NULL

void init_event_queue(void);

// Adds an event to the system event queue unsafely. Use this for ISR
void event_raise_isr(Event e);

void event_raise(Event e);

Event get_next_event(void);

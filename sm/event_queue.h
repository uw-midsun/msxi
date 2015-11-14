#pragma once
#include <stdint.h>
#include <stddef.h>
#include <limits.h>

// This is responsible for managing the system's event queue.
// The queue is a FIFO linked list.
// All states (including composite states) raise events in this queue.


typedef uint16_t EventID;

struct Event {
  EventID id;
  uint64_t data;
};

#define NULL_EVENT_ID UINT_MAX
#define NULL_EVENT (struct Event) { NULL_EVENT_ID, 0 }
#define QUEUE_EMPTY NULL

void event_queue_init(void);

// Adds an event to the system event queue unsafely. Use this for ISR
void event_raise_isr(EventID id, uint64_t data);

void event_raise(EventID id, uint64_t data);

struct Event event_get_next(void);

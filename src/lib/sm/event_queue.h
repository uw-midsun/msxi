/*
  event_queue.h - Titus Chow
  
  This module is responsible for managing the system's event queue.
  The queue operates on the First In, First Out (FIFO) principle.
  
  All states (including substates) raise events in this queue.

  TODO: Add check for empty queue to allow sleeping/waking to conserve power?

*/
#ifndef EVENT_QUEUE_H_
#define EVENT_QUEUE_H_
#include <stdbool.h>
#include <stdint.h>
#include <limits.h>

typedef uint16_t Event;

#define NULL_EVENT (Event)UINT_MAX
#define QUEUE_EMPTY NULL

// init_event_queue() initializes the event queue.
void init_event_queue(void);

// unsafe_raise_event(e) adds an event to the system event queue unsafely.
//   That is, interrupts can cause race conditions.
void unsafe_raise_event(Event e);

// raise_event(e) adds an event to the system event queue.
// requires: init_event_queue() has been called.
void raise_event(Event e);

// get_next_event() returns the next event in the event queue.
// requires: init_event_queue() has been called.
Event get_next_event(void);

#endif

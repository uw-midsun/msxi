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

#define NULL_EVENT -1
#define QUEUE_EMPTY NULL

typedef int Event;

// init_event_queue() initializes the event queue.
void init_event_queue(void);

// raise_event(e) adds an event to the system event queue.
// requires: init_event_queue() has been called.
void raise_event(Event e);

// get_next_event() returns the next event in the event queue.
// requires: init_event_queue() has been called.
Event get_next_event(void);

#endif
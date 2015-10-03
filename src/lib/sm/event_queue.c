/*
  event_queue.c - Titus Chow

  The event queue is a FIFO linked list.
  It uses a pool of event nodes to hold events with a constant memory cost.
  Note that the pool size is a hand-tuned variable. It is currently a wild guess.

  Events can be raised from interrupts. There shouldn't be any problems.

*/

#include "event_queue.h"
#include "msp430.h"
#include <assert.h>

#define EVENT_POOL_SIZE 10

struct EventNode {
  Event e;
  struct EventNode *next;
};

struct EventQueue {
  struct EventNode *first;
  struct EventNode *last;
};

static struct EventNode node_pool[EVENT_POOL_SIZE];
static struct EventQueue event_queue;

// init_event_queue() initializes the event queue and pool.
void init_event_queue(void) {
  struct EventNode *temp_node;
  for (temp_node = node_pool; temp_node < node_pool + EVENT_POOL_SIZE; temp_node++) {
    temp_node->e = NULL_EVENT;
    temp_node->next = NULL;
  }
  event_queue.first = QUEUE_EMPTY;
  event_queue.last = QUEUE_EMPTY;
}

// get_free_node() returns a pointer to the next free node in the pool.
// requires: init_event_queue() has been called.
static struct EventNode *get_free_node(void) {
  struct EventNode *temp_node;
  for (temp_node = node_pool; temp_node < node_pool + EVENT_POOL_SIZE; temp_node++) {
    if (temp_node->e == NULL_EVENT) {
      return temp_node;
    }
  }
  // Should never reach here - increase pool if it does.
  assert(0 && "Event pool is too small!");
  return NULL;
}

// release_node(node) releases the specified node by setting it to default values.
static void release_node(struct EventNode *node) {
  node->e = NULL_EVENT;
  node->next = NULL;
}

// unsafe_raise_event(e) sets e as the queue's last node, using a node in the pool.
//   Note that this is unsafe - Interrupts may cause race conditions.
// Use this function in ISRs instead of raise_event(e)
//   so interrupts are not enabled from within the ISR.
void unsafe_raise_event(Event e) {
  struct EventNode *event_node = get_free_node();
  event_node->e = e;
  event_node->next = QUEUE_EMPTY;

  if (event_queue.last == QUEUE_EMPTY) {
    event_queue.first = event_node;
  } else {
    event_queue.last->next = event_node;
  }
  event_queue.last = event_node;
}

// raise_event(e) sets e as the queue's last node, using a node in the pool.
//   It disables interrupts to prevent race conditions.
void raise_event(Event e) {
  __disable_interrupt();
  unsafe_raise_event(e);
  __enable_interrupt();
}

// get_next_event() sets the 2nd node as the first, returning the first node's event and releasing its node.
//   If there aren't any events queued, it just returns an empty node.
Event get_next_event(void) {
  if (event_queue.first == QUEUE_EMPTY) {
    return NULL_EVENT;
  }

  __disable_interrupt();
  struct EventNode *first_node = event_queue.first;
  Event event = first_node->e;
  event_queue.first = first_node->next;
  if (first_node->next == QUEUE_EMPTY) {
    event_queue.last = QUEUE_EMPTY;
  }

  release_node(first_node);
  __enable_interrupt();

  return event;
}

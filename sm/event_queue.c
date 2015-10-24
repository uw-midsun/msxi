#include "event_queue.h"
#include "msp430.h"
#include <assert.h>

// The event queue is a FIFO linked list.
// It uses a pool of event nodes to hold events with a constant memory cost.
// Note that the pool size is a hand-tuned variable. It is currently a wild guess.

// TODO: Add check for empty queue to allow sleeping/waking to conserve power?

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

void init_event_queue(void) {

  struct EventNode *temp_node;
  for (temp_node = node_pool; temp_node < node_pool + EVENT_POOL_SIZE; temp_node++) {
    temp_node->e = NULL_EVENT;
    temp_node->next = NULL;
  }
  event_queue.first = QUEUE_EMPTY;
  event_queue.last = QUEUE_EMPTY;
}

// Returns a pointer to the next free node in the pool.
static struct EventNode *prv_get_free_node(void) {
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

// Nodes with the default values are assumed to be free.
static void prv_release_node(struct EventNode *node) {
  node->e = NULL_EVENT;
  node->next = NULL;
}

// Sets e as the queue's last node using a node in the pool.
//  Note that this is unsafe - Interrupts may cause race conditions.
void event_raise_isr(Event e) {
  struct EventNode *event_node = prv_get_free_node();
  event_node->e = e;
  event_node->next = QUEUE_EMPTY;

  if (event_queue.last == QUEUE_EMPTY) {
    event_queue.first = event_node;
  } else {
    event_queue.last->next = event_node;
  }
  event_queue.last = event_node;
}

// This disables interrupts to prevent race conditions.
void event_raise(Event e) {
  __disable_interrupt();
  event_raise_isr(e);
  __enable_interrupt();
}

// Pops the first node. If there aren't any events queued, it just returns an empty node.
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

  prv_release_node(first_node);
  __enable_interrupt();

  return event;
}

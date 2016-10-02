#pragma once
#include <stdint.h>
#include "msp430.h"

#define BUFFER_SIZE 16

struct RingBuffer {
  volatile uint16_t buffer[BUFFER_SIZE];
  volatile uint16_t *buffer_end;
  volatile uint16_t *head;
  volatile uint16_t inserts;
};

void ring_buffer_init(volatile struct RingBuffer *ring_buffer);

//Adds next value at the head of the ring buffer
inline void ring_buffer_push(volatile struct RingBuffer *ring_buffer, uint16_t data);

//Reads from the ring buffer starting at the tail
void ring_buffer_read(volatile struct RingBuffer *ring_buffer, uint16_t *return_buffer);

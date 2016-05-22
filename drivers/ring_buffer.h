#pragma once
#include <stdint.h>
//#include <stdbool.h>
#include "msp430.h"

#define BUFFER_SIZE 12

struct RingBuffer {
  uint16_t buffer[BUFFER_SIZE];
  uint16_t *buffer_end;
  uint16_t *head;
  uint16_t curr_size;
  uint16_t inserts;
};

void ring_buffer_init(struct RingBuffer *ring_buffer);

//Adds next value at the head of the ring buffer
void ring_buffer_push(struct RingBuffer *ring_buffer, uint16_t data);

//Reads from the ring buffer starting at the tail
void ring_buffer_read(struct RingBuffer *ring_buffer, uint16_t *return_buffer);

#include "ring_buffer.h"
#include "msp430.h"
#include <string.h>

void ring_buffer_init(struct RingBuffer *ring_buffer)
{
  memset(ring_buffer, 0, sizeof(*ring_buffer));
  ring_buffer->buffer_end = ring_buffer->buffer + BUFFER_SIZE;
  ring_buffer->head = ring_buffer->buffer;
  ring_buffer->inserts = 0;
};

void ring_buffer_push(struct RingBuffer *ring_buffer, uint16_t data)
{
  *ring_buffer->head = data;
  ring_buffer->head++;
  ring_buffer->inserts += 1;

  if (ring_buffer->head == ring_buffer->buffer_end){
    ring_buffer->head = ring_buffer->buffer;
  }
}

void ring_buffer_read(struct RingBuffer *ring_buffer, uint16_t *return_buffer)
{
  uint16_t *tmp_tail = ring_buffer->head;
  uint16_t i;

  for (i = 0; i < BUFFER_SIZE; i++, tmp_tail++) {
    if (tmp_tail >= ring_buffer->buffer_end) {
      tmp_tail = ring_buffer->buffer;
    }
    return_buffer[i] = *tmp_tail;
  }
}

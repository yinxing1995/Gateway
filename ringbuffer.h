#ifndef RINGBUFFER_H
#define RINGBUFFER_H

#include <stdint.h>

typedef struct buffer
{
    uint8_t *position;
    uint16_t writepointer;
    uint16_t readpointer;
    uint16_t max;
}Ringbuf;

Ringbuf Buffer;

void BufferInit(char *p, uint16_t size);
int BufferWrite(void *sour, uint16_t len);
int BufferRead(void *dest, uint16_t len);
int BufferSeek(void *dest, uint16_t len);

#endif

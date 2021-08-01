#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include "ringbuffer.h"

Ringbuf *BufferInit(char *p, uint16_t size)
{
    Ringbuf *Buffer = (Ringbuf *)malloc(sizeof(Ringbuf));
    Buffer->position = p;
    Buffer->writepointer = 0;
    Buffer->readpointer = 0;
    Buffer->max = size - 1;
    return Buffer;
}

static int IfOverflow(Ringbuf *Buffer, uint16_t move, uint16_t stay, uint16_t len)
{
    if(move < stay)
    {
        if(len > stay - move)
            return -1;         
    }
    else if(move > stay)
    {
        if(len > (Buffer->max - move + stay + 1))
            return -1;
    }
    return 0;
}

int BufferWrite(Ringbuf * Buffer, void *sour, uint16_t len)
{
    uint16_t diff = Buffer->max - Buffer->writepointer + 1;
    if(IfOverflow(Buffer,Buffer->writepointer,Buffer->readpointer,len))
        return -1;
    if(len < diff)
    {
        memcpy(&Buffer->position[Buffer->writepointer],sour,len);
        Buffer->writepointer += len;
    }
    else
    {
        memcpy(&Buffer->position[Buffer->writepointer],sour,diff);
        memcpy(Buffer->position,sour + diff,len - diff);
        Buffer->writepointer = len - diff;
    }
    return 0;
}

int BufferRead(Ringbuf * Buffer, void *dest, uint16_t len)
{
    uint16_t diff = Buffer->max - Buffer->readpointer + 1;
    if(IfOverflow(Buffer,Buffer->readpointer,Buffer->writepointer,len))
        return -1;
    if(Buffer->readpointer == Buffer->writepointer)
        return -1;
    if(len < diff)
    {
        memcpy(dest,&Buffer->position[Buffer->readpointer],len);
        Buffer->readpointer += len;
    }
    else
    {
        memcpy(dest,&Buffer->position[Buffer->readpointer],diff);
        memcpy(dest + diff,Buffer->position,len - diff);
        Buffer->readpointer = len - diff;
    }
    return 0;
}

uint16_t BufferShowRest(Ringbuf *Buffer)
{
    uint16_t len = 0;
    if(Buffer->readpointer <= Buffer->writepointer)
        len = Buffer->writepointer - Buffer->readpointer;
    else
	len = Buffer->max - Buffer->readpointer + Buffer->writepointer + 1;
    if(!len)
	return 0;
}

int BufferSeek(Ringbuf * Buffer, void *dest, uint16_t len)
{
    uint16_t diff = Buffer->max - Buffer->readpointer + 1;
    if(IfOverflow(Buffer,Buffer->readpointer,Buffer->writepointer,len))
        return -1;
    if(Buffer->readpointer == Buffer->writepointer)
        return -2;
    if(len < diff)
    {
        memcpy(dest,&Buffer->position[Buffer->readpointer],len);
    }
    else
    {
        memcpy(dest,&Buffer->position[Buffer->readpointer],diff);
        memcpy(dest + diff,Buffer->position,len - diff);
    }
    return 0;
}

void BufferRelease(Ringbuf *Buffer)
{
    free(Buffer);
}

void BufferReset(Ringbuf *Buffer)
{
    Buffer->writepointer = 0;
    Buffer->readpointer = 0;
}

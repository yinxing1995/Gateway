#ifndef USART_H
#define USART_H
#include "ringbuffer.h"

int Serial_fd;
char *Serial_addr;
char Text[500];
Ringbuf *USART_BUF;

int Serial_init(void);
void Serial_close(void);
void Serial_Transmit(const void* buff, int len);
void Serial_Set(void);

#endif

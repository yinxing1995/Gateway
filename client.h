#ifndef CLIENT_H
#define CLIENT_H

#include <arpa/inet.h>
#include <pthread.h>
#include "ringbuffer.h"

#define SERVADDR "130.61.250.117"
#define PORT 17480

int Connectflag;
int Socket_fd;
struct sockaddr_in servaddr;
char Datagram[500];
Ringbuf *NET_BUF;

enum
{
	Connected = 0x00,
	Disconnected = 0x01
};

pthread_mutex_t mutex_socket;

int Socket_Init(void);

#endif

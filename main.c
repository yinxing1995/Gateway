#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include "usart.h"
#include "ringbuffer.h"

void usart_send(void)
{
    char *p="I am gateway\r\n";
    while(1)
    {
        //Serial_Transmit(p,strlen(p));
        sleep(1);
	}
	{
		sleep(2);
	}
	printf("Error\r\n");
}

void usart_recv(void)
{
	char p;
	int num = 0;
	while(1)
	{
		num = read(Serial_fd,&p,1);
		if(num > 0)
		{
			printf("p = %#04x\r\n",p);
			//BufferWrite(&p,1);
		}
		num = 0;
	}
	printf("Error\r\n");
}

int main(void)
{
	Serial_init();	
	BufferInit(Text,sizeof(Text));
	fcntl(Serial_fd,F_SETFL,0);
	pthread_t t1, t2;
	char *msg1 = "task 1", *msg2 = "task 2";
	if (pthread_create(&t1, NULL, (void *)usart_send, (void *)msg1))
		exit(1);
    if (pthread_create(&t2, NULL, (void *)usart_recv, (void *)msg2))
        exit(1);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	return 0;
}

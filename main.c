#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <sys/select.h>
#include <sys/time.h>
#include "usart.h"
#include "ringbuffer.h"
#include "dataprocessing.h"
#include "client.h"

void *usart_processing(void)
{
    char p;
    while(1)
    {
		if(BufferSeek(&p,1))
			usleep(1000);
        StateMachine();
	}
	printf("Error\r\n");
}

void *usart_recv(void)//use select() to save resources
{
	char p;
	int num = 0;
	while(1)
	{
		num = read(Serial_fd,&p,1);
		if(num > 0)
		{
			//printf("p = %#04x\r\n",p);
			BufferWrite(&p,1);
		}
		num = 0;
	}
	printf("Error\r\n");
}

void *tcp_client(void)
{
	fd_set rfds;
	struct timeval timeout;
	int iret = 0,rret = 0;
	timeout.tv_sec = 0;
	timeout.tv_usec = 0;	
	//FD_ZERO(&rfds);
	//FD_SET(Socket_fd,&rfds);
	while(1)
	{
		/*
		if ((iret=recv(Socket_fd,Datagram,sizeof(Datagram),0))<=0)
		{
			printf("socket error\r\n");
			if (connect(Socket_fd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)  // 向服务端发起连接清求。
				perror("connect");
		}
		else
			printf("recv = %s\r\n",Datagram);
		*/
		FD_ZERO(&rfds);
		FD_SET(Socket_fd,&rfds);
		iret = select(Socket_fd+1,&rfds,NULL,NULL,&timeout);
		if(iret)
		{
			//printf("iret = %d\r\n",iret);
			rret = recv(Socket_fd,Datagram,sizeof(Datagram),0);
			if(rret > 0)
			{
				printf("recv = %s\r\n",Datagram);
				memset(Datagram,0,sizeof(Datagram));
			}
			else
			{
				//printf("rret = %d,error = %d\r\n",rret,errno);
				if(errno == EINTR)
					printf("do not close socket!\r\n");

				else
				{
					close(Socket_fd);
					Socket_Init();
					printf("Try to reconnect\r\n");
					//printf("Socket_fd = %d\r\n",Socket_fd);
					sleep(1);
				}
			}
		}
		else
		{
			write(Socket_fd,"I am client",strlen("I am client"));
			sleep(1);
		}
	}
	printf("exit\r\n");
}

int main(void)
{
	Serial_init();	
	BufferInit(Text,sizeof(Text));
	fcntl(Serial_fd,F_SETFL,0);
	Socket_Init();
	pthread_t t1, t2, t3;
	char *msg1 = "task 1", *msg2 = "task 2", *msg3 = "task 3";
	//if (pthread_create(&t1, NULL, (void *)usart_send, (void *)msg1))
		//exit(1);
    	if (pthread_create(&t1, NULL, (void *)usart_recv, (void *)msg2))
		exit(1);
	if (pthread_create(&t2, NULL, (void *)usart_processing, (void *)msg2))
        	exit(1);
	if (pthread_create(&t3, NULL, (void *)tcp_client, (void *)msg3))
        	exit(1);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	close(Serial_fd);
	close(Socket_fd);
	return 0;
}

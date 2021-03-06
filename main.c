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
#include <signal.h>

void alarmlocker(int sig)
{
    if(SIGALRM == sig)
    {
        printf("alarmlock!!\r\n");
		alarm_locker = UNLOCKED;
    }   
    return ;
}

void *usart_processing(void)
{
	char p;
	signal(SIGALRM, alarmlocker);
	alarm_locker = UNLOCKED;

	while(1)
	{
		
		if(BufferSeek(USART_BUF,&p,1))
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
			BufferWrite(USART_BUF,&p,1);
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
	char p[500];	
	//FD_ZERO(&rfds);
	//FD_SET(Socket_fd,&rfds);
	while(1)
	{
		FD_ZERO(&rfds);
		FD_SET(Socket_fd,&rfds);
		iret = select(Socket_fd+1,&rfds,NULL,NULL,&timeout);
		if(iret)
		{
			//printf("iret = %d\r\n",iret);
			rret = read(Socket_fd,p,sizeof(Datagram));
			if(rret > 0)
			{
				printf("recv = %d\r\n",rret);
				BufferWrite(NET_BUF,p,rret);
			}
			else
			{
				//printf("rret = %d,error = %d\r\n",rret,errno);
				if(errno == EINTR)
					printf("do not close socket!\r\n");
				else
				{
					close(Socket_fd);
					pthread_mutex_lock(&mutex_socket);
					Connectflag = Disconnected;
					pthread_mutex_unlock(&mutex_socket);
					Socket_Init();
					printf("Try to reconnect\r\n");
					sleep(1);
				}
			}
		}
	}
	printf("exit\r\n");
}

void tcp_processing(void)
{
	char p;
	while(1)
	{
		if(BufferSeek(NET_BUF,&p,1))
			usleep(1000);
		else
			printf("Command comes!!\r\n");
		CommandPush();
	}
}

int main(void)
{
	Serial_init();
	Serial_Set();	
	USART_BUF = BufferInit(Text,sizeof(Text));
	NET_BUF = BufferInit(Datagram,sizeof(Datagram));
	fcntl(Serial_fd,F_SETFL,0);
	pthread_mutex_init(&mutex_socket,NULL);
	Socket_Init();
	pthread_t t1, t2, t3, t4;
	char *msg1 = "task 1", *msg2 = "task 2", *msg3 = "task 3", *msg4 = "task 4";
	//if (pthread_create(&t1, NULL, (void *)usart_send, (void *)msg1))
		//exit(1);
    	if (pthread_create(&t1, NULL, (void *)usart_recv, (void *)msg2))
		exit(1);
	if (pthread_create(&t2, NULL, (void *)usart_processing, (void *)msg2))
        	exit(1);
	if (pthread_create(&t3, NULL, (void *)tcp_client, (void *)msg3))
        	exit(1);
	if (pthread_create(&t4, NULL, (void *)tcp_processing, (void *)msg4))
		exit(1);
	pthread_join(t1,NULL);
	pthread_join(t2,NULL);
	pthread_join(t3,NULL);
	close(Serial_fd);
	close(Socket_fd);
	return 0;
}

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <pthread.h>
#include <termios.h>
#include "usart.h"
#include "ringbuffer.h"

//extern int Serial_fd;
//extern char *Serial_addr;
/*
int Serial_init(void);
void Serial_close(void);
void Serial_Transmit(const void* buff, int len);

int Serial_init(void)
{
    Serial_fd = open("/dev/ttyAMA0", O_RDWR | O_NOCTTY | O_NDELAY);
    if (-1 == Serial_fd)
    {
        // ERROR
        return (-1);
    }
    return 0;
}

void Serial_close(void)
{
    close(Serial_fd);
}

void Serial_Transmit(const void* buff, int len)
{
    write(Serial_fd, buff, len);
}

void Serial_Set(void)
{
	struct termios options;
	tcgetattr(Serial_fd,&options);
	options.c_lflag &= ~(ICANON | ISIG);
	options.c_iflag &= ~(ICRNL | IGNCR);
	tcflush(Serial_fd,TCIFLUSH);
	tcsetattr(Serial_fd,TCSANOW,&options);
}
*/
void usart_send(void)
{
        char *p="I am gateway\r\n";
        while(1)
        {
                //Serial_Transmit(p,strlen(p));
                sleep(1);
	}
	{
		//Serial_Transmit(p,strlen(p));
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
			//printf("num = %d\r\n",num);
		}
		num = 0;
		//printf("%c",p);
		//tcflush(Serial_fd,TCIFLUSH);	
	}
	printf("Error\r\n");
}

int main(void)
{
	Serial_init();
	pthread_t t1, t2;
	//fcntl(Serial_fd,F_SETFL,FNDELAY);
	fcntl(Serial_fd,F_SETFL,0);
	char *msg1 = "task 1", *msg2 = "task 2";
	if (pthread_create(&t1, NULL, (void *)usart_send, (void *)msg1))
		exit(1);
    	if (pthread_create(&t2, NULL, (void *)usart_recv, (void *)msg2))
        	exit(1);
	while(1);
	return 0;
}

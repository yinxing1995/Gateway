#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <netdb.h>
#include <sys/types.h>
#include <sys/socket.h>
#include "client.h"

#define GATEWAY_ID "001"

int Socket_Init(void)
{
    if ((Socket_fd = socket(AF_INET,SOCK_STREAM,0))==-1)
    {
        perror("socket");
        return -1;
    }
    /*struct hostent* h;
    if (h = gethostbyaddr(SERVADDR) == 0 )   // 指定服务端的ip地址。
    {
        printf("gethostbyname failed.\n");
        close(Socket_fd);
        return -1;
    }
    */
    memset(&servaddr,0,sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(PORT); // 指定服务端的通信端口。
    inet_pton(AF_INET,SERVADDR,&servaddr.sin_addr.s_addr);
    //memcpy(&servaddr.sin_addr,h->h_addr,h->h_length);
    if (connect(Socket_fd, (struct sockaddr *)&servaddr,sizeof(servaddr)) != 0)  // 向服务端发起连接清求。
    {
        perror("connect");
        close(Socket_fd);
        return -1;
    }
    char p[20];
    sprintf(p,"Gateway:%s",GATEWAY_ID);
    pthread_mutex_lock(&mutex_socket);
    if(write(Socket_fd,p,strlen(p))<=0)
    {
	    perror("write");
	    close(Socket_fd);
    }
    pthread_mutex_unlock(&mutex_socket);
}

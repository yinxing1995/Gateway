#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include "dataprocessing.h"
#include "ringbuffer.h"
#include "client.h"
#include "usart.h"
#include "dsp.h"

#define FRAMEFLAG "Frame"


static int State = Init;
static char frameflag[sizeof(FRAMEFLAG)];
static uint16_t FrameLen = 0;
static ClusterArray *Head = NULL;
static char *fbuf = NULL;

static void DataUpload(char *buf, uint16_t len)
{
	char *p = (char *)malloc(len+sizeof(len)+strlen(FRAMEFLAG));
	char *temp = p;
	memcpy(temp,FRAMEFLAG,strlen(FRAMEFLAG));
	temp += strlen(FRAMEFLAG);
	memcpy(temp,(void *)&len,sizeof(len));
	temp += sizeof(len);
	memcpy(temp,(void *)buf,len-sizeof(len));
	pthread_mutex_lock(&mutex_socket);
	if(Connectflag == Connected)
		write(Socket_fd,p,strlen(FRAMEFLAG)+len);
	pthread_mutex_unlock(&mutex_socket);
	free(p);
}

static void DataPush(char *buf, int len)
{
    ClusterArray * Current = NULL;
    uint8_t node = *buf++;
    uint8_t cluster = *buf++;
    uint8_t endpoint = *buf++;
    if(!Head)
    {
        Head = (ClusterArray *)malloc(sizeof(ClusterArray));
        Current = Head;
        Current->next = NULL;
    }
    else
    {
        Current = Head;
        while(!((Current->NodeID == node)&&(Current->ClusterID == cluster)&&(Current->Endpoint == endpoint)))
        {
            if(Current->next == NULL)
            {
                Current->next = (ClusterArray *)malloc(sizeof(ClusterArray));
                Current = Current->next;
                Current->next = NULL;
                Current->Data = NULL;
                break;
            }
            Current = Current->next;
        }
    }
    Current->NodeID = node;
    Current->ClusterID = cluster;
    Current->Endpoint = endpoint;
    Current->ControlFlag = *buf++;
    Current->DataType = *buf++;
    Current->DataLength = *buf++;
    switch(Current->DataType)
    {
        case _INT32:
        if(!Current->Data)
            Current->Data = malloc(Current->DataLength*sizeof(int));
            break;
        case _FLOAT:
        if(!Current->Data)
            Current->Data = malloc(Current->DataLength*sizeof(float));
            break;
        default:
            printf("Unknown type\r\n");
            break;
    }
    memcpy(Current->Data,(void *)buf,Current->DataLength*sizeof(float));
    //Current->Timer = NULL;
    //temporary!!!
    printf("Node = %d",Current->NodeID);
    if(Current->NodeID == 2 && Current->ClusterID == Temperature)
        {
	    printf("tempadded!!\r\n");
	    addtemp(*(float *)Current->Data);
	}
    if(Current->NodeID == 2 && Current->ClusterID == Humidity)
    {
	    printf("humiadded!!\r\n");
	    addhumi(*(float *)Current->Data);
    }
}

void StateMachine()
{
    switch(State)
    {
        case Init:
            if(BufferSeek(USART_BUF,frameflag,sizeof(frameflag) - 1))
                break;//buffer not ready
	        frameflag[sizeof(frameflag)] = '\0';
            if(strcmp(FRAMEFLAG,frameflag))
            {
	        printf("flag = %s\r\n",frameflag);
                BufferRead(USART_BUF,frameflag,1);//readpointer + 1
                break;
            }
            BufferRead(USART_BUF,frameflag,sizeof(frameflag) - 1);
            memset(frameflag,0,sizeof(frameflag));//frame flag found
            State = GetLen;
            break;
        case GetLen:
            if(BufferRead(USART_BUF,&FrameLen,sizeof(FrameLen)))
                break;//buffer not ready
            State = FramePick;
            fbuf = (uint8_t *)malloc(FrameLen - sizeof(FrameLen));
            break;//len got
        case FramePick:
            if(BufferRead(USART_BUF,fbuf,FrameLen - sizeof(FrameLen)))
                break;//buffer not ready
            int i = 0;
	        uint8_t sum = 0;
            for(i=0;i<(FrameLen - sizeof(FrameLen) - 1);i++)
            {
                sum += fbuf[i];
            }
            sum += ((uint8_t)FrameLen + (uint8_t)(FrameLen>>8));
	        if(sum != fbuf[i])
            {
                printf("checksum incorrect!\r\n");
                FrameLen = 0;
                State = Init;
                free(fbuf);
                fbuf = NULL;
                break;
            }
            else
            {
                printf("checksum pass!\r\n");
                State = DataUpdate;
		        break;
            }
        case DataUpdate:
            if(*fbuf == REPORT)
	        {
		        DataUpload(fbuf,FrameLen);
		        FrameLen = 0;
		        fbuf++;
                DataPush(fbuf,FrameLen);
		        free(--fbuf);
	        }
	        else
                free(fbuf);
            fbuf = NULL;
	        State = Init;
            break;
        default:
            printf("unexpected error!\r\n");
            break;
    }
}

static void SendCommand(char * message,uint16_t len)
{
    uint16_t i = 0;
    printf("Send :");
    for(i=0;i<len;i++)
    {
        printf(" %02x ",message[i]);
    }
    printf("\r\n");
    i = write(Serial_fd,FRAMEFLAG,strlen(FRAMEFLAG));
    printf("I = %d\r\n",i);
    write(Serial_fd,message,len);
}

void CommandPush()
{
    char flag[sizeof(FRAMEFLAG)];
    uint16_t len,i;
    char *Message;
    while(1)
    {
        if(BufferSeek(NET_BUF,flag,sizeof(FRAMEFLAG) - 1))
            return;//buffer not ready
	flag[sizeof(flag)-1] = '\0';
	for(i=0;i<(sizeof(FRAMEFLAG)-1);i++)
	{
		printf("flag[i]=%02x\r\n",flag[i]);
	}
        if(strcmp(FRAMEFLAG,flag))
        {
            BufferRead(NET_BUF,flag,1);//readpointer + 1
            continue;
        }
        else
            break;
    }
    printf("Message!!\r\n");
    BufferRead(NET_BUF,flag,sizeof(flag) - 1);
    BufferSeek(NET_BUF,&len,sizeof(len));
    Message = (char *)malloc(len);
    BufferRead(NET_BUF,Message,len);
    SendCommand(Message,len);
    free(Message);
}

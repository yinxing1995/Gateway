#include <stdint.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include "dataprocessing.h"
#include "ringbuffer.h"

#define FRAMEFLAG "Frame"

static int State = Init;
static char frameflag[sizeof(FRAMEFLAG)];
static uint16_t FrameLen = 0;
static ClusterArray *Head = NULL;
static char *fbuf = NULL;

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
        case _INT:
            Current->Data = malloc(Current->DataLength*sizeof(int));
            break;
        case _FLOAT:
            Current->Data = malloc(Current->DataLength*sizeof(float));
            break;
        default:
            printf("Unknown type\r\n");
            break;
    }
    memcpy(Current->Data,(void *)buf,Current->DataLength*sizeof(float));
    //Current->Timer = NULL;
    printf("Endpoint = %d\r\n",Current->Endpoint);
}

void StateMachine()
{
    //printf("enter statemachine\r\n");
    switch(State)
    {
        case Init:
            if(BufferSeek(frameflag,sizeof(frameflag) - 1))
                break;//buffer not ready
	        frameflag[sizeof(frameflag)] = '\0';
            if(strcmp(FRAMEFLAG,frameflag))
            {
	        printf("flag = %s\r\n",frameflag);
                BufferRead(frameflag,1);//readpointer + 1
                break;
            }
	        //printf("Frame flag captured\r\n");
            BufferRead(frameflag,sizeof(frameflag) - 1);
            memset(frameflag,0,sizeof(frameflag));//frame flag found
            State = GetLen;
            break;
        case GetLen:
	        //printf("GetLen state\r\n");
            if(BufferRead(&FrameLen,sizeof(FrameLen)))
                break;//buffer not ready
            State = FramePick;
	        //printf("FrameLen = %d\r\n",FrameLen);
            fbuf = (uint8_t *)malloc(FrameLen - sizeof(FrameLen));
            break;//len got
        case FramePick:
            if(BufferRead(fbuf,FrameLen - sizeof(FrameLen)))
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
                FrameLen = 0;
                State = DataUpdate;
		        break;
            }
        case DataUpdate:
            if(*fbuf == REPORT)
	    {
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


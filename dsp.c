#include "dsp.h"
#include "stdio.h"
#include <stdlib.h>
#include <string.h>

#define WINDOW_SIZE 10

static int temppointer = -1;
static int humipointer = -1;
float temp_node2[15] = {0};
float humi_node2[15] = {0};
static int t = 0;//test

void addtemp(float value)
{
    temppointer++;
    if (temppointer == 15) temppointer = 0;
    temp_node2[temppointer] = value;
    //temp_node2[temppointer] = t;
    //t+=1;
    windowprocessing();
}

void addhumi(float value)
{
    humipointer++;
    if (humipointer == 15) humipointer = 0;
    humi_node2[humipointer] = value;
    windowprocessing();
}

void windowprocessing()
{
    float temp[WINDOW_SIZE];
    float humi[WINDOW_SIZE];
    if(temppointer >= (WINDOW_SIZE-1))
        memcpy(temp,&temp_node2[temppointer-(WINDOW_SIZE-1)],sizeof(float)*WINDOW_SIZE);
    else
    {
        memcpy(temp,&temp_node2[15-(WINDOW_SIZE-temppointer-1)],sizeof(float)*(WINDOW_SIZE-temppointer-1));
        memcpy(&temp[WINDOW_SIZE-temppointer-1],temp_node2,sizeof(float)*(temppointer+1));
    }
    if(humipointer >= (WINDOW_SIZE-1))
        memcpy(humi,&humi_node2[humipointer-(WINDOW_SIZE-1)],sizeof(float)*WINDOW_SIZE);
    else
    {
        memcpy(humi,&humi_node2[15-(WINDOW_SIZE-humipointer-1)],sizeof(float)*(WINDOW_SIZE-humipointer-1));
        memcpy(&humi[WINDOW_SIZE-humipointer-1],humi_node2,sizeof(float)*(humipointer+1));
    }
    /*
    int i = 0;
    for(i=0;i<10;i++)
    {
        printf(" %f ",temp[i]);
    }
    printf("\r\n");
    for(i=0;i<10;i++)
    {
        printf(" %f ",humi[i]);
    }
    */
   int i=0,j=0;
   int sum = 0;
   int temp_fa,humi_fa = 0;
   for(i=0;i<WINDOW_SIZE;i++)
   {
       for(j=i;j<WINDOW_SIZE;j++)
       {
           if((temp[WINDOW_SIZE-1-i]-temp[WINDOW_SIZE-1-j])<-0.01)temp_fa = -1;
           else if((temp[WINDOW_SIZE-1-i]-temp[WINDOW_SIZE-1-j])>=-0.01 || (temp[WINDOW_SIZE-1-i]-temp[WINDOW_SIZE-1-j])<=0.01)temp_fa = 0;
           else temp_fa = 1;
           if(humi[WINDOW_SIZE-1-i]-humi[WINDOW_SIZE-1-j]<-0.01)humi_fa = -1;
           else if((humi[WINDOW_SIZE-1-i]-humi[WINDOW_SIZE-1-j])>=-0.01 || (humi[WINDOW_SIZE-1-i]-humi[WINDOW_SIZE-1-j])<=0.01)humi_fa = 0;
           else humi_fa = 1;
           sum += temp_fa * humi_fa;
       }
   }
   float kendall = 0;
   float window = WINDOW_SIZE;
   kendall = (2.0f/(window*(window-1.0f)))*(float)sum;
   printf("Sum = %d ,Kendall = %f\r\n",sum,kendall);
}

#ifndef DARAPROCESSING_H
#define DARAPROCESSING_H

//#include <linux/timer.h>

//typedef struct timer_list timer; 

enum states
{
    Init = 0x00,
    GetLen = 0x01,
    FramePick = 0x02,
    DataUpdate = 0x03,
};

enum FrameType
{
    REPORT = 0x00,
    CHECK = 0x01,
    NETSTATUS = 0x02,
};

enum DataType
{
    _INT32 = 0x00,
    _FLOAT = 0x01,
};

typedef struct ClusterStructure
{
    //timer * Timer;
    uint8_t NodeID;
    uint8_t ClusterID;
    //char * Description;
    uint8_t Endpoint;
    uint8_t DataType;
    uint8_t DataLength;
    void * Data;
    uint8_t ControlFlag;
    struct ClusterStructure *next;
}ClusterArray;

void StateMachine();
void CommandPush();

#endif

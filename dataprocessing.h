#ifndef DARAPROCESSING_H
#define DARAPROCESSING_H
enum states
{
    Init = 0x00,
    GetLen = 0x01,
    FramePick = 0x02,
    DataUpdate = 0x03,
};

void StateMachine();
#endif

#ifndef DSP_H
#define DSP_H

#include<stdint.h>

float temp_node2[15];
float humi_node2[15];
int alarm_AMG8833;
int alarm_DHT22;
//int temppointer;
//int humipointer;

void addtemp(float value);
void addhumi(float value);
uint8_t addinfra(float *array,uint8_t len);

void windowprocessing();

#endif
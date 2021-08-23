#ifndef DSP_H
#define DSP_H

float temp_node2[15];
float humi_node2[15];
//int temppointer;
//int humipointer;

void addtemp(float value);
void addhumi(float value);

void windowprocessing();

#endif
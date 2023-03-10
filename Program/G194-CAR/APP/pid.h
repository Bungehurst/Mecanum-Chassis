#ifndef __PID_H
#define __PID_H
#include "sys.h"
typedef __packed struct
{
	float k_p;
	float k_i;
	float k_d;
	float error[2];
  float inte;
  float dire;
  int16_t output;	
}PID_GENERAL;

typedef __packed struct
{
	float k_p;
	float k_i;
	float k_d;
	float error[2];
  float inte;
  float dire;
  int16_t output;	
	const int16_t input_min;
  const int16_t input_max;
  const int16_t output_min;
  const int16_t output_max;
}PID_LINE;

void Wheel_PID(int16_t current,int16_t target,PID_GENERAL *pid);
float PID_General(float target,float current,PID_LINE *pid);

#endif

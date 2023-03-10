#include "pid.h"

#define 		TAR_MAX 			10000.0f       
#define 		ESC_MAX 			10000.0f
#define   	ENTER_MAX		  20000.0f						
#define 		INTE_MAX			10000.0f			//积分饱和阈值
#define			START_VAL			0.0f


void Wheel_PID(int16_t current,int16_t target,PID_GENERAL *pid)
{
	volatile float 	Kp = pid->k_p,\
									Ki = pid->k_i,\
									Kd = pid->k_d;
	//输入限幅
	if (current > ENTER_MAX) { current = ENTER_MAX; }
	if (current < -ENTER_MAX) { current = -ENTER_MAX; }
	
	if (target > TAR_MAX) { target = TAR_MAX; }
	if (target < -TAR_MAX) { target = -TAR_MAX; }
	
	pid->error[0] = pid->error[1];
	pid->error[1] = target - current;
	
	pid->inte += pid->error[1];
	
	//抗积分饱和
	if (pid->inte > INTE_MAX) { pid->inte = INTE_MAX; }
	if (pid->inte < -INTE_MAX) { pid->inte = -INTE_MAX; }
	
	pid->dire = pid->error[1] - pid->error[0];
	
	pid->output = pid->error[1] * Kp + pid->inte * Ki + pid->dire * Kd;
	
	if(pid->output > 0)	pid->output	=	pid->output + START_VAL;
	else pid->output	=	pid->output - START_VAL;
	//输出限幅
	if (pid->output > ESC_MAX) { pid->output = ESC_MAX; }
	if (pid->output < -ESC_MAX) { pid->output = -ESC_MAX; }

}

float PID_General(float target,float current,PID_LINE *pid)
{
  if(target>pid->input_max){target=pid->input_max;}
  if(target<pid->input_min){target=pid->input_min;}
	
  pid->error[0] = pid->error[1];
  pid->error[1] = target - current;
		
  pid->inte += pid->error[1];
  pid->dire = pid->error[1] - pid->error[0];
		
  pid->output = pid->error[1] * pid->k_p + pid->inte * pid->k_i + pid->dire * pid->k_d; 
	
  if(pid->output > pid->output_max)	pid->output = pid->output_max;
  if(pid->output < pid->output_min)	pid->output = pid->output_min;
	
  return pid->output;
}
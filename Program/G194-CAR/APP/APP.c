#include "APP.h"
#include <math.h>
#include "usart2.h"
#include "usart3.h"

#define Kp_LF 0.49f
#define Ki_LF 0.001f
#define Kd_LF 0.0f

#define Kp_RF 0.49f
#define Ki_RF 0.001f
#define Kd_RF 0.0f

#define Kp_LB	0.489f
#define Ki_LB 0.001f
#define Kd_LB 0.0f

#define Kp_RB 0.481f
#define Ki_RB 0.001f
#define Kd_RB 0.0f

PID_GENERAL PID_LF = {Kp_LF,\
											Ki_LF,\
											Kd_LF,\
											{0,0},\
											0,\
											0,\
											0				};
PID_GENERAL PID_RF = {Kp_RF,\
											Ki_RF,\
											Kd_RF,\
											{0,0},\
											0,\
											0,\
											0				};
PID_GENERAL PID_LB = {Kp_LB,\
											Ki_LB,\
											Kd_LB,\
											{0,0},\
											0,\
											0,\
											0				};
PID_GENERAL PID_RB = {Kp_RB,\
											Ki_RB,\
											Kd_RB,\
											{0,0},\
											0,\
											0,\
											0				};

#define Kp_LINE_RHO 30.0f
#define Ki_LINE_RHO 0.0f
#define Kd_LINE_RHO 0.0f

#define Kp_LINE_THETA 30.0f
#define Ki_LINE_THETA 0.0f
#define Kd_LINE_THETA 0.0f

											
#define 		RHO_MAX				90.0f
#define			THETA_MAX			80.0f
											
PID_LINE	PID_LINE_RHO = {	Kp_LINE_RHO,\
														Ki_LINE_RHO,\
														Kd_LINE_RHO,\
														{0,0},\
														0,\
														0,\
														0,\
														-RHO_MAX,\
														RHO_MAX,\
														-1500,\
														1500};
	
PID_LINE	PID_LINE_THETA = {	Kp_LINE_THETA,\
															Ki_LINE_THETA,\
															Kd_LINE_THETA,\
															{0,0},\
															0,\
															0,\
															0,\
															-THETA_MAX,\
															THETA_MAX,\
															-1500,\
															1500};
	
enum Wheel_Num{W_LF = 0,W_RF = 1,W_LB = 2,W_RB = 3};
void Set_Wheel_Speed(int16_t target,CAR_WHEEL *wheel,PID_GENERAL *pid,uint8_t Wheel_Num)
{
	Wheel_PID(wheel->speed,target,pid);
	switch(Wheel_Num)
	{
		case W_LF:
			if(pid->output > 0)	
			{
				DIR_CTL_LF(Bit_SET);
				PWM_F_L(fabs(pid->output));
			}
			else {
				DIR_CTL_LF(Bit_RESET);
				PWM_F_L(fabs(pid->output));
			}
			break;
		case W_RF:
			if(pid->output > 0)	
			{
				DIR_CTL_RF(Bit_SET);
				PWM_F_R(fabs(pid->output));
			}
			else {
				DIR_CTL_RF(Bit_RESET);
				PWM_F_R(fabs(pid->output));
			}
			break;
		case W_LB:
			if(pid->output > 0)	
			{
				DIR_CTL_LB(Bit_SET);
				PWM_B_L(fabs(pid->output));
			}
			else {
				DIR_CTL_LB(Bit_RESET);
				PWM_B_L(fabs(pid->output));
			}
			break;
		case W_RB:
			if(pid->output > 0)	
			{
				DIR_CTL_RB(Bit_SET);
				PWM_B_R(fabs(pid->output));
			}
			else {
				DIR_CTL_RB(Bit_RESET);
				PWM_B_R(fabs(pid->output));
			}
			break;
		default: break;
	}
}

void Motor_Control(void)
{
	static int16_t channel_1 = 0,channel_2 = 0,channel_4 = 0;//channel_3 = 0
	
	channel_1 = -rc_data.Channel_Current[2]/800.0f*10000*0.5f;	//throttle
	channel_2 = rc_data.Channel_Current[0]/800.0f*10000*0.5f;		//rotate
	//channel_3 = rc_data.Channel_Current[1]/800.0f*10000*0.5f;		// no use
	channel_4 = -rc_data.Channel_Current[3]/800.0f*10000*0.5f;	//shift-left/right

	Set_Wheel_Speed(channel_1-channel_4-channel_2,&Car_Wheel_LF,&PID_LF,W_LF);//-
	Set_Wheel_Speed(-channel_1-channel_4-channel_2,&Car_Wheel_RF,&PID_RF,W_RF);
	Set_Wheel_Speed(channel_1+channel_4-channel_2,&Car_Wheel_LB,&PID_LB,W_LB);//-
	Set_Wheel_Speed(-channel_1+channel_4-channel_2,&Car_Wheel_RB,&PID_RB,W_RB);

}
float output_rho = 0,output_theta = 0,channel_1 = 0;
void APP_Init(void)
{
	if(Openmv.theta > 90.0f && Openmv.theta < 175.0f)
	{
		Openmv.delta_theta = Openmv.theta - 180.0f ;
		if(Openmv.rho > 0)
		{
			Openmv.delta_rho = Openmv.rho - fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
			Openmv.delta_x = -Openmv.delta_rho / cosf(180.0f - Openmv.theta);	
		}else
		{
			Openmv.delta_rho = Openmv.rho + fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
			Openmv.delta_x = -Openmv.delta_rho / cosf(180.0f - Openmv.theta);	
		}
	}
	else if(Openmv.theta < 90.0f && Openmv.theta > 5.0f )
	{
		Openmv.delta_theta = Openmv.theta ;
		Openmv.delta_rho = Openmv.rho - fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
		Openmv.delta_x = Openmv.delta_rho / cosf(Openmv.theta);
	}else
	{
		Openmv.delta_theta = 0;
		if(Openmv.theta > 175.0f)
		{
			if(Openmv.rho > 0)
			{
				Openmv.delta_rho = Openmv.rho - fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
				Openmv.delta_x = -Openmv.delta_rho / cosf(180.0f - Openmv.theta);	
			}else
			{
				Openmv.delta_rho = Openmv.rho + fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
				Openmv.delta_x = -Openmv.delta_rho / cosf(180.0f - Openmv.theta);	
			}
		}
		else if(Openmv.theta < 5.0f)
		{
			Openmv.delta_rho = Openmv.rho - fabsf(40.0f*sinf(Openmv.theta) + 30.0f*cosf(Openmv.theta));
			Openmv.delta_x = Openmv.delta_rho / cosf(Openmv.theta);
		}
	}
	channel_1 = 800.0f;
	output_rho = PID_General(0,Openmv.delta_x,&PID_LINE_RHO);
	output_theta = PID_General(0,Openmv.delta_theta,&PID_LINE_THETA);

	Set_Wheel_Speed(channel_1-output_rho-output_theta,&Car_Wheel_LF,&PID_LF,W_LF);//-
	Set_Wheel_Speed(-channel_1-output_rho-output_theta,&Car_Wheel_RF,&PID_RF,W_RF);
	Set_Wheel_Speed(channel_1+output_rho-output_theta,&Car_Wheel_LB,&PID_LB,W_LB);//-
	Set_Wheel_Speed(-channel_1+output_rho-output_theta,&Car_Wheel_RB,&PID_RB,W_RB);
}


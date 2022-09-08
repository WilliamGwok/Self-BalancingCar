#include "control.h"

int Med_Angle=1.25;//机械中值
float //直立环参数
	Vertical_Kp=340,//520*0.6
	Vertical_Kd=1.73;//1.9*0.6
float //速度环参数
	Velocity_Kp=-0.39,
	Velocity_Ki=-0.00195;
float 
	Turn_Kp=0.65;

int Vertical_out,Velocity_out,Turn_out;

int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int encoder_left,int encoder_right);
int Turn(int gyro_Z);

void EXTI9_5_IRQHandler(void)
{
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//一级判定
	{
		int PWM_OUT;
		if(PBin(5))//二级判定
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//清除中断标志位
			
			/*采集编码器&MPU6050角度信息*/
			Encoder_Left=-Read_Speed(2);//注意这里加了负号
			Encoder_Right=Read_Speed(3);
			mpu_dmp_get_data(&Pitch,&Roll,&Yaw);			//角度
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//陀螺仪
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//加速度
			/*将数据压入闭环控制中，计算出控制输出量*/
			Vertical_out=Vertical(Med_Angle,Pitch,gyroy);
			Velocity_out=Velocity(Encoder_Left,Encoder_Right);
			Turn_out=Turn(gyroz);
			PWM_OUT=Vertical_out-Vertical_Kp*Velocity_out;
			
			//把控制输出量加载到电机上，完成最终的控制
			MOTO1=PWM_OUT-Turn_out;
			MOTO2=PWM_OUT+Turn_out;//符号不一样是因为电机安装的位置是相反的
			Limit(&MOTO1,&MOTO2);
			
			Load(MOTO1,MOTO2);
		}
	}
}


/*直立环PD:Kp*Ek=Kd*Ek_d*/
/**
  * @brief		直立环PD控制
  * @param		float Med    期望角度（机械中值）
  * @param		float Angle  实际角度
  * @param		gyro_Y       该此的角速度误差与上次的角速度误差
  * @retval		
  */
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_OUT;
	
	PWM_OUT=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_Y);//!
	
	return PWM_OUT;
}

/*速度环PI:Kp*Ek+Ki*Ek_S*/
int Velocity(int encoder_left,int encoder_right)
{
	static int PWM_OUT,Encoder_Err,Encoder_S,Enc_Err_Lowout,Enc_Err_Lowout_Last;
	float a=0.7;
	
	//计算速度偏差
	Encoder_Err=(encoder_left+encoder_right)-0;//舍去误差，0是因为期望速度为0
	//对速度偏差进行低通滤波,使得波形更加平滑，滤除高频干扰（防止速度突变），防止速度过大影响直立环的正常工作
	//low_out=(1-a)*Ek+a*low_out_last
	Enc_Err_Lowout=(1-a)*Encoder_Err+a*Enc_Err_Lowout_Last;
	Enc_Err_Lowout_Last=Enc_Err_Lowout;
	//对速度偏差积分，积分出位移
	Encoder_S+=Enc_Err_Lowout;
	//积分限幅
	Encoder_S=(Encoder_S>10000)?10000:(Encoder_S<-10000)?(-10000):Encoder_S;
	//速度环控制输出
	PWM_OUT=Velocity_Kp*Enc_Err_Lowout+Velocity_Ki*Encoder_S;
	
	return PWM_OUT;
}

/*转向环：系数*Z轴角速度*/
int Turn(int gyro_Z)
{
	int PWM_OUT;
	
	PWM_OUT=Turn_Kp*gyro_Z;
	return PWM_OUT;
}


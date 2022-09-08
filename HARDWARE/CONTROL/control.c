#include "control.h"

int Med_Angle=1.25;//��е��ֵ
float //ֱ��������
	Vertical_Kp=340,//520*0.6
	Vertical_Kd=1.73;//1.9*0.6
float //�ٶȻ�����
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
	if(EXTI_GetITStatus(EXTI_Line5)!=0)//һ���ж�
	{
		int PWM_OUT;
		if(PBin(5))//�����ж�
		{
			EXTI_ClearITPendingBit(EXTI_Line5);//����жϱ�־λ
			
			/*�ɼ�������&MPU6050�Ƕ���Ϣ*/
			Encoder_Left=-Read_Speed(2);//ע��������˸���
			Encoder_Right=Read_Speed(3);
			mpu_dmp_get_data(&Pitch,&Roll,&Yaw);			//�Ƕ�
			MPU_Get_Gyroscope(&gyrox,&gyroy,&gyroz);	//������
			MPU_Get_Accelerometer(&aacx,&aacy,&aacz);	//���ٶ�
			/*������ѹ��ջ������У���������������*/
			Vertical_out=Vertical(Med_Angle,Pitch,gyroy);
			Velocity_out=Velocity(Encoder_Left,Encoder_Right);
			Turn_out=Turn(gyroz);
			PWM_OUT=Vertical_out-Vertical_Kp*Velocity_out;
			
			//�ѿ�����������ص�����ϣ�������յĿ���
			MOTO1=PWM_OUT-Turn_out;
			MOTO2=PWM_OUT+Turn_out;//���Ų�һ������Ϊ�����װ��λ�����෴��
			Limit(&MOTO1,&MOTO2);
			
			Load(MOTO1,MOTO2);
		}
	}
}


/*ֱ����PD:Kp*Ek=Kd*Ek_d*/
/**
  * @brief		ֱ����PD����
  * @param		float Med    �����Ƕȣ���е��ֵ��
  * @param		float Angle  ʵ�ʽǶ�
  * @param		gyro_Y       �ô˵Ľ��ٶ�������ϴεĽ��ٶ����
  * @retval		
  */
int Vertical(float Med,float Angle,float gyro_Y)
{
	int PWM_OUT;
	
	PWM_OUT=Vertical_Kp*(Angle-Med)+Vertical_Kd*(gyro_Y);//!
	
	return PWM_OUT;
}

/*�ٶȻ�PI:Kp*Ek+Ki*Ek_S*/
int Velocity(int encoder_left,int encoder_right)
{
	static int PWM_OUT,Encoder_Err,Encoder_S,Enc_Err_Lowout,Enc_Err_Lowout_Last;
	float a=0.7;
	
	//�����ٶ�ƫ��
	Encoder_Err=(encoder_left+encoder_right)-0;//��ȥ��0����Ϊ�����ٶ�Ϊ0
	//���ٶ�ƫ����е�ͨ�˲�,ʹ�ò��θ���ƽ�����˳���Ƶ���ţ���ֹ�ٶ�ͻ�䣩����ֹ�ٶȹ���Ӱ��ֱ��������������
	//low_out=(1-a)*Ek+a*low_out_last
	Enc_Err_Lowout=(1-a)*Encoder_Err+a*Enc_Err_Lowout_Last;
	Enc_Err_Lowout_Last=Enc_Err_Lowout;
	//���ٶ�ƫ����֣����ֳ�λ��
	Encoder_S+=Enc_Err_Lowout;
	//�����޷�
	Encoder_S=(Encoder_S>10000)?10000:(Encoder_S<-10000)?(-10000):Encoder_S;
	//�ٶȻ��������
	PWM_OUT=Velocity_Kp*Enc_Err_Lowout+Velocity_Ki*Encoder_S;
	
	return PWM_OUT;
}

/*ת�򻷣�ϵ��*Z����ٶ�*/
int Turn(int gyro_Z)
{
	int PWM_OUT;
	
	PWM_OUT=Turn_Kp*gyro_Z;
	return PWM_OUT;
}


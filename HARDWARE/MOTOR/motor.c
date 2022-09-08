#include "motor.h"

void Motor_Init(void)
{
	GPIO_InitTypeDef GPIO_InitStruct;
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOB, ENABLE);
	
	/*初始化GPIO*/
	GPIO_InitStruct.GPIO_Mode=GPIO_Mode_Out_PP;
	GPIO_InitStruct.GPIO_Pin=GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStruct.GPIO_Speed=GPIO_Speed_50MHz;
	GPIO_Init(GPIOB, &GPIO_InitStruct);
}

//int PWM_MAX=7200,PWM_MIN=-7200;

void Limit(int *motorA,int *motorB)
{
	if(*motorA>PWM_MAX)*motorA=PWM_MAX;
	if(*motorA<PWM_MIN)*motorA=PWM_MIN;
	if(*motorB>PWM_MAX)*motorB=PWM_MAX;
	if(*motorB<PWM_MIN)*motorB=PWM_MIN;
}

/*绝对值函数*/
int GJW_abs(int p)
{
	int q;
	if(p>0)q=p;
	else q=-p;
	return q;
}

void Load(int moto1,int moto2)
{
	if(moto1>0) AIN1=1,AIN2=0;
	else AIN1=0,AIN2=1;
	TIM_SetCompare1(TIM1,GJW_abs(moto1));
	
	if(moto2>0) BIN1=1,BIN2=0;
	else BIN1=0,BIN2=1;
	TIM_SetCompare4(TIM1,GJW_abs(moto2));
}



#ifndef __MOTOR_H
#define __MOTOR_H

#include "sys.h"

#define AIN2   PBout(15)
#define AIN1   PBout(14)
#define BIN1   PBout(13)
#define BIN2   PBout(12)

void Motor_Init(void);
void Limit(int *motorA,int *motorB);
int GJW_abs(int p);
void Load(int moto1,int moto2);

#endif  /*__MOTOR_H*/



#ifndef __CONTROL_H
#define __CONTROL_H

#include "sys.h"

void EXTI9_5_IRQHandler(void);
int Vertical(float Med,float Angle,float gyro_Y);
int Velocity(int encoder_left,int encoder_right);
int Turn(int gyro_Z);

#endif  /*__CONTROL_H*/


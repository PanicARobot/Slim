#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include <Arduino.h>
#include <stdint.h>

void initMotors(void);
void setMotors(int32_t, int32_t);

#endif // __MOTOR_DRIVER_H

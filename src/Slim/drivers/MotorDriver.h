#ifndef __MOTOR_DRIVER_H
#define __MOTOR_DRIVER_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void initMotors(void);
void setMotors(int32_t, int32_t);

#ifdef __cplusplus
}
#endif

#endif // __MOTOR_DRIVER_H

#ifndef __ROBOT_STATE_CONTROL
#define __ROBOT_STATE_CONTROL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void setState(int);
void updateRobotState(uint32_t);
void indicateRobotState(uint32_t);
void handleRobotAction(void (*)(), void (*)(), void (*)());

#ifdef __cplusplus
}
#endif

#endif // __ROBOT_STATE_CONTROL

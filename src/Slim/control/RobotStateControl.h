#ifndef __ROBOT_STATE_CONTROL
#define __ROBOT_STATE_CONTROL

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

void setState(uint32_t);
void indicateRobotState(uint32_t);
void handleRobotAction(uint32_t, void (*)(void));

#ifdef __cplusplus
}
#endif

#endif // __ROBOT_STATE_CONTROL

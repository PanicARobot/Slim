#ifndef __UI_COMMANDER_H
#define __UI_COMMANDER_H

#include <stdint.h>

// Both should be < 0
#define UI_WAITING   -1
#define UI_FAILSAFE  -2

#ifdef __cplusplus
extern "C" {
#endif

int8_t getUICommand(void);

#ifdef __cplusplus
}
#endif

#endif // __UI_COMMANDER_H

#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

void initiateLinearMovement(int, int);
void initiateTurn(int, int, int);
void resetMovement(void);

void handleControlledMovement(float, float, float);
int isMovementComplete(void);

#ifdef __cplusplus
}
#endif

#endif //__MOVEMENT_H

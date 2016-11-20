#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

void handleControlledMovement(float, float);
int IsMovementComplete(void);

void initiateLinearMovement(int, int);
void initiateTurn(int, int, int);

#ifdef __cplusplus
}
#endif

#endif //__MOVEMENT_H

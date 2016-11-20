#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

void handleControlledMovement(float, float);
int IsMovementComplete(void);

void LinearMovement(int);
void Turn(int, int);

/*
void Push();
void WaitToPass();
void TurnTowards();
void TurnAround();
*/

#ifdef __cplusplus
}
#endif

#endif //__MOVEMENT_H

#ifndef __MOVEMENT_H
#define __MOVEMENT_H

#ifdef __cplusplus
extern "C" {
#endif

void enqueueLinearMovement(int, int);
void enqueueTurn(int, int, int);

void handleControlledMovement(float, float, float);
int isQueueEmpty(void);
void clearQueue(void);

#ifdef __cplusplus
}
#endif

#endif //__MOVEMENT_H

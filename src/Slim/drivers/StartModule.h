#ifndef __START_MODULE_H
#define __START_MODULE_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#define START_BUTTON_PIN   A4

void initStartModule(void);
uint8_t getStart(void);

#ifdef __cplusplus
}
#endif

#endif // __START_MODULE_H

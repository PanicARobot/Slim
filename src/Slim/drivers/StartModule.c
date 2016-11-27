#include "StartModule.h"

#include <Arduino.h>

#define START_BUTTON_PIN   A0

void initStartModule(void)
{
	pinMode(START_BUTTON_PIN, INPUT);
}

uint8_t remoteStarted(void)
{
	return digitalRead(START_BUTTON_PIN);
}

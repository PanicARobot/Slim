#include "ProximitySensors.h"

#include <Arduino.h>

#define LEFT_SENSOR_PIN         A4
#define RIGHT_SENSOR_PIN        A5

void initProximitySensors(void)
{
	pinMode(LEFT_SENSOR_PIN, INPUT);
	pinMode(RIGHT_SENSOR_PIN, INPUT);
}

uint8_t readLeftSensor(void)
{
	return digitalRead(LEFT_SENSOR_PIN) ^ 1;
}

uint8_t readRightSensor(void)
{
	return digitalRead(RIGHT_SENSOR_PIN) ^ 1;
}

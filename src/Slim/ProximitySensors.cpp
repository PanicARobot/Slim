#include "ProximitySensors.h"

#include <Arduino.h>

#define LEFT_SENSOR_PIN         A4
#define RIGHT_SENSOR_PIN        A5

void initProximitySensors()
{
	pinMode(LEFT_SENSOR_PIN, INPUT);
	pinMode(RIGHT_SENSOR_PIN, INPUT);
}

void readProximitySensors(uint8_t& left, uint8_t& right)
{
	left = digitalRead(LEFT_SENSOR_PIN) ^ 1;
	right = digitalRead(RIGHT_SENSOR_PIN) ^ 1;
}

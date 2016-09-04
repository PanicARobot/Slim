#include "DualEncoderDriver.h"

#include <Arduino.h>

#define LEFT_A_PIN 11
#define LEFT_B_PIN 12
#define RIGHT_A_PIN 9
#define RIGHT_B_PIN 10


Encoder::Encoder()
	: direction(0), lastReadingMicros(0) {}

int Encoder::getSpeed() // mm/s
{
	int currentMicros = micros();
	float speed = DISTANCE / (currentMicros - lastReadingMicros);
	lastReadingMicros = currentMicros;
	return direction == 0 ? -speed : speed; // 1 is forward, 0 is backward
}

Encoder leftEncoder, rightEncoder;

void leftHandler()
{
	int b = digitalRead(LEFT_B_PIN);
	leftEncoder.update(b); // Flip if direction is wrong
}

void rightHandler()
{
	int b = digitalRead(RIGHT_B_PIN);
	rightEncoder.update(b); // Flip if direction is wrong
}

void initDualEncoders()
{
	pinMode(LEFT_A_PIN, INPUT);
	pinMode(LEFT_B_PIN, INPUT);
	pinMode(RIGHT_B_PIN, INPUT);
	pinMode(RIGHT_B_PIN, INPUT);

	attachInterrupt(digitalPinToInterrupt(LEFT_A_PIN), leftHandler, RISING);
	attachInterrupt(digitalPinToInterrupt(RIGHT_A_PIN), rightHandler, RISING);
}

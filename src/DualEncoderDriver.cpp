#include "DualEncoderDriver.h"

#include "constants.h"

#include <Arduino.h>

static constexpr float DISTANCE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_ROUND;

Encoder::Encoder()
	: direction(0), lastMicros(0), impulseCounter(0) {}

void Encoder::update(int b)
{
	direction = b;
	++impulseCounter;
}

float Encoder::getSpeed() // mm/s
{
	uint32_t currentMicros = micros();
	float speed = DISTANCE * impulseCounter / (currentMicros - lastMicros);
	impulseCounter = 0;
	lastMicros = currentMicros;
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

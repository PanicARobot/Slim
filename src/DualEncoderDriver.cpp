#include "DualEncoderDriver.h"

#include "constants.h"

#include <Arduino.h>

static constexpr float DISTANCE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_ROUND;

Encoder::Encoder() :
	direction(0), last_micros(0),
	impulse_counter(0), speed(0) {}

void Encoder::update(int b)
{
	direction = b;
	++impulse_counter;
}

void Encoder::updateSpeed()
{
	uint32_t current_micros = micros();
	speed = DISTANCE * impulse_counter / (current_micros - last_micros);
	impulse_counter = 0;
	last_micros = current_micros;
	if(direction == 0) speed = -speed;
}

Encoder leftEncoder, rightEncoder;

void leftHandler()
{
	int b = digitalRead(LEFT_B_PIN);
	leftEncoder.update(b);
}

void rightHandler()
{
	int b = digitalRead(RIGHT_B_PIN);
	rightEncoder.update(b);
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

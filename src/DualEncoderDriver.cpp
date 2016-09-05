#include "DualEncoderDriver.h"

#include "constants.h"

#include <Arduino.h>

static constexpr float DISTANCE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_ROUND;

Encoder::Encoder() :
	last_micros(0), impulse_counter(0),
	speed(0), acceleration(0) {}

void Encoder::update(uint8_t b)
{
	impulse_counter += b * 2 - 1;
}

void Encoder::update()
{
	uint32_t current_micros = micros();

	float last_speed = speed;
	speed = DISTANCE * impulse_counter / (current_micros - last_micros);
	acceleration = (speed - last_speed) / (current_micros - last_micros);

	last_micros = current_micros;
	impulse_counter = 0;
}

Encoder leftEncoder, rightEncoder;

void leftHandler()
{
	uint8_t b = digitalRead(LEFT_B_PIN);
	leftEncoder.update(b);
}

void rightHandler()
{
	uint8_t b = digitalRead(RIGHT_B_PIN);
	rightEncoder.update(b);
}

void initDualEncoders()
{
	pinMode(LEFT_A_PIN, INPUT);
	pinMode(LEFT_B_PIN, INPUT);
	pinMode(RIGHT_A_PIN, INPUT);
	pinMode(RIGHT_B_PIN, INPUT);

	attachInterrupt(digitalPinToInterrupt(LEFT_A_PIN), leftHandler, RISING);
	attachInterrupt(digitalPinToInterrupt(RIGHT_A_PIN), rightHandler, RISING);
}

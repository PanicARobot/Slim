#include "DualEncoderDriver.h"

#include <Arduino.h>

#include <cmath>

#define LEFT_A_PIN              12
#define LEFT_B_PIN              11
#define RIGHT_A_PIN             9
#define RIGHT_B_PIN             10

#define MICROS_PER_SECOND       1000000

static constexpr float WHEEL_PERIMETER = 18 * M_PI;
static constexpr float IMPULSES_PER_ROUND = 12 * 29.86;
static constexpr float DISTANCE = MICROS_PER_SECOND * WHEEL_PERIMETER / IMPULSES_PER_ROUND;

Encoder::Encoder() :
	last_micros(0),
	impulse_counter(0),
	speed(0) {}

void Encoder::update(uint8_t b)
{
	impulse_counter += b * 2 - 1; // 1 or -1

	if(impulse_counter == 12 || impulse_counter == -12)
	{
		uint32_t current_micros = micros();

		speed = DISTANCE * impulse_counter / (current_micros - last_micros);

		last_micros = current_micros;
		impulse_counter = 0;
	}
}

void Encoder::update()
{
	uint32_t current_micros = micros();

	if(current_micros - last_micros > MICROS_PER_SECOND / 10)
	{
		speed = 0;
		impulse_counter = 0;
	}
}

Encoder leftEncoder, rightEncoder;

void leftAHandler()
{
	uint8_t b = digitalRead(LEFT_A_PIN) == digitalRead(LEFT_B_PIN);
	leftEncoder.update(b);
}

void leftBHandler()
{
	uint8_t b = digitalRead(LEFT_A_PIN) ^ digitalRead(LEFT_B_PIN);
	leftEncoder.update(b);
}

void rightAHandler()
{
	uint8_t b = digitalRead(RIGHT_A_PIN) == digitalRead(RIGHT_B_PIN);
	rightEncoder.update(b);
}

void rightBHandler()
{
	uint8_t b = digitalRead(RIGHT_A_PIN) ^ digitalRead(RIGHT_B_PIN);
	rightEncoder.update(b);
}

void initDualEncoders()
{
	pinMode(LEFT_A_PIN, INPUT);
	pinMode(LEFT_B_PIN, INPUT);
	pinMode(RIGHT_A_PIN, INPUT);
	pinMode(RIGHT_B_PIN, INPUT);

	attachInterrupt(digitalPinToInterrupt(LEFT_A_PIN), leftAHandler, CHANGE);
	attachInterrupt(digitalPinToInterrupt(LEFT_B_PIN), leftBHandler, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RIGHT_A_PIN), rightAHandler, CHANGE);
	attachInterrupt(digitalPinToInterrupt(RIGHT_B_PIN), rightBHandler, CHANGE);
}

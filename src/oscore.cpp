#include "OrientationSensorsWrapper.h"
#include "SDLogDriver.h"
#include "DualEncoderDriver.h"
#include "MotorDriver.h"
#include "PlanarAccelerationModule.h"
#include "TireContactModule.h"
#include "FrontLiftedDetection.h"

#include "constants.h"

#include <Wire.h>
#include <SD.h>

enum {
	WAITING_FOR_COMMAND,
	PREPARE_TO_FIGHT,
	FIGHT_MODE,
	RECALIBRATE,
	BRAINDEAD,
	TEST_MODE,
	SERIAL_MODE
} robot_state = WAITING_FOR_COMMAND;

void log_info()
{
	log_data_pack.timestamp = millis();

	log_data_pack.acc_x = position.getAccX();
	log_data_pack.acc_y = position.getAccY();
	log_data_pack.acc_z = position.getAccZ();

	log_data_pack.gyro_x = position.getGyroX();
	log_data_pack.gyro_y = position.getGyroY();
	log_data_pack.gyro_z = position.getGyroZ();

	log_data_pack.ahrs_x = position.getRoll();
	log_data_pack.ahrs_y = position.getPitch();
	log_data_pack.ahrs_z = position.getYaw();

	log_data_pack.left_speed = leftEncoder.getSpeed();
	log_data_pack.right_speed = rightEncoder.getSpeed();

	logDataPack();
}

void setup()
{
	Serial.begin(SERIAL_BAUD_RATE);

	SD.begin(SD_CHIP_SELECT);

	initLogger();

	initDualEncoders();
	initMotors();

	pinMode(LEFT_SENSOR_PIN, INPUT);
	pinMode(RIGHT_SENSOR_PIN, INPUT);

	Wire.begin();
	Wire.setClock(I2C_FREQUENCY);

	position.init();

	tiresContactInit(TIRE_LOST_OF_CONTACT_DEGREES);

	initFrontLifted(FRONT_LIFTED_THRESHOLD);
}

void getSerialCommand()
{
	if(Serial.available() == 0) return;

	static int left = 0;
	static int right = 0;

	char cmd = Serial.read();

	if(cmd == 'l' || cmd == 'r' || cmd == 'b')
	{
		int speed = 0;
		if(Serial.available() > 0)
		{
			int dir = 1;
			if(Serial.peek() == '-')
			{
				dir = -1;
				Serial.read();
			}
			while(Serial.available() > 0)
			{
				char digit = Serial.read();
				speed = speed * 10 + (digit - '0');
			}
			speed *= dir;
		}

		if(cmd == 'l' || cmd == 'b') left = speed;
		if(cmd == 'r' || cmd == 'b') right = speed;

		setMotors(left, right);
	}
	else if(cmd == 'd')
	{
		dumpLog();
	}
	else if(cmd == 'e')
	{
		dropLog();
	}
}

uint32_t event_micros;

void readProximitySensors(int& left, int& right)
{
	left = digitalRead(LEFT_SENSOR_PIN) ^ 1;
	right = digitalRead(RIGHT_SENSOR_PIN) ^ 1;
}

void getProximityCommand()
{
	static int left = 0;
	static int right = 0;

	static int last_left = 0;
	static int last_right = 0;

	static int command_counter = 0;

	readProximitySensors(left, right);

	if(robot_state == WAITING_FOR_COMMAND)
	{
		if(right)
		{
			if(left < last_left)
			{
				++command_counter;
				Serial.print("Counter: ");
				Serial.println(command_counter);
			}
		}
		else if(right < last_right)
		{
			if(!left)
			{
				switch(command_counter)
				{
					case 0:
						robot_state = PREPARE_TO_FIGHT;
						event_micros = micros();
						break;

					case 1:
						robot_state = RECALIBRATE;
						digitalWrite(LED_PIN, HIGH);
						position.calibrate();
						planarAcceleration.calibrate();
						robot_state = WAITING_FOR_COMMAND;
						break;

					case 2:
						robot_state = TEST_MODE;
						break;

					case 3:
						robot_state = SERIAL_MODE;
						break;
				}
			}

			command_counter = 0;
		}
	}
	else if(left && right)
	{
		if(robot_state == PREPARE_TO_FIGHT)
		{
			robot_state = WAITING_FOR_COMMAND;
			command_counter = 42; // Just something invalid
		}
		else
		{
			robot_state = BRAINDEAD;
			setMotors(0, 0);
		}
	}

	last_left = left;
	last_right = right;
}

bool set = false;
void loop()
{
	static uint32_t last_sample_micros = 0;
	static uint32_t last_log_micros = 0;
	static uint32_t last_blink_micros = 0;

	uint32_t current_micros = micros();

	{ // blink control
		uint32_t full_cycle = MICROS_PER_SECOND;
		uint32_t high_cycle = MICROS_PER_SECOND;

		switch(robot_state)
		{
			case BRAINDEAD:
				full_cycle *= 2;
				high_cycle *= 1;
				break;

			case PREPARE_TO_FIGHT:
				full_cycle /= 10;
				high_cycle /= 20;
				break;

			case SERIAL_MODE:
				break;

			default:
				full_cycle /= 2;
				high_cycle /= 5;
				break;
		}

		if(current_micros - last_blink_micros >= full_cycle)
		{
			digitalWrite(LED_PIN, HIGH);
			last_blink_micros = current_micros;
		}
		else if(current_micros - last_blink_micros >= high_cycle)
		{
			digitalWrite(LED_PIN, LOW);
		}
	}

	switch(robot_state)
	{
		case BRAINDEAD:
			setMotors(0, 0);
			return;

		case PREPARE_TO_FIGHT:
			if(current_micros - event_micros >= MICROS_PER_SECOND * 5)
			{
				robot_state = FIGHT_MODE;
			}
			break;

		case TEST_MODE:
			if(planarAcceleration.getAcc() > 0.2 && !set)
			{
				set = true;
				if(planarAcceleration.getAngle() < 0)
					setMotors(50, -50);
				else setMotors(-50, 50);
			}
			break;

		default:
			break;
	}

	// Read sensors
	if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
	{
		position.update();
		leftEncoder.updateSpeed();
		rightEncoder.updateSpeed();
		planarAcceleration.update();

		getProximityCommand();

		last_sample_micros = current_micros;

		if(robot_state == SERIAL_MODE && planarAcceleration.getAcc() > 0.2)
		{
			Serial.print(planarAcceleration.getAcc()); Serial.print("   ");
			Serial.print(planarAcceleration.getAngle()); Serial.println("");
		}
	}

	// Log data
	if(current_micros - last_log_micros >= MICROS_PER_SECOND / LOG_FREQUENCY)
	{
		log_info();
		last_log_micros = current_micros;

		if(robot_state == SERIAL_MODE)
		{
			getSerialCommand();
		}
	}
}

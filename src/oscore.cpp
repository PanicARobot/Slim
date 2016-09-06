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
	BRAINDEAD,
	TEST_MODE
} robot_state = WAITING_FOR_COMMAND;

// action states
enum{
    INITIAL_STATE,
    STATE_TURNING,
    STATE_FORWARD,
    EXIT_STATE_ON_SIDE,
    EXIT_STATE_ON_IMPACT,
    EXIT_STATE_OUT_OF_RING,
    EXIT_STATE_FRONT_LIFTED
}STATE =INITIAL_STATE;

void linearMovement(int32_t distanceInMMWithDirection);
void Turn(int turnRadius, int turnDegrees);
void handleControlledMovement();
bool IsMovementComplete();
void RoundPattern();
void TroughCernterPattern();

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
						digitalWrite(LED_PIN, HIGH);
						position.calibrate();
						break;

					case 2:
						robot_state = TEST_MODE;
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

			case TEST_MODE:
				full_cycle /= 2;
				high_cycle /= 3;

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
		case FIGHT_MODE:
		{
			TroughCernterPattern();
			handleControlledMovement();
		}
		break;

		case TEST_MODE:
			if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
			{
				static constexpr float GRAVITY = 9805.63;

				Serial.print("Accelerometer: ");
				Serial.print(position.getAccX() * GRAVITY); Serial.print(", ");
				Serial.print(position.getAccY() * GRAVITY); Serial.print("     ");

				Serial.print("Speed: ");
				Serial.print(leftEncoder.getSpeed()); Serial.print(", ");
				Serial.print(rightEncoder.getSpeed()); Serial.print("     ");

				Serial.print("Acc: ");
				Serial.print(leftEncoder.getAcc()); Serial.print(", ");
				Serial.print(rightEncoder.getAcc()); Serial.print("     ");

				Serial.print("Vector: ");
				Serial.print(planarAcceleration.getX()); Serial.print(", ");
				Serial.print(planarAcceleration.getY()); Serial.print("\n");
			}
			break;

		default:
			break;
	}

	// Read sensors
	if(current_micros - last_sample_micros >= MICROS_PER_SECOND / SAMPLE_FREQUENCY)
	{
		position.update();
		leftEncoder.update();
		rightEncoder.update();
		planarAcceleration.update();

		getProximityCommand();

		last_sample_micros = current_micros;
	}

	// Log data
	if(current_micros - last_log_micros >= MICROS_PER_SECOND / LOG_FREQUENCY)
	{
		log_info();
		last_log_micros = current_micros;
	}

	getSerialCommand();
}

void TroughCernterPattern()
{
    switch(STATE)
    {
		case INITIAL_STATE:
		{
			Turn(0,100);
			STATE = STATE_TURNING;
			break;
		}

		case STATE_FORWARD:
		{
			if(IsMovementComplete())
			{
				Turn(100,270);
				STATE = STATE_TURNING;
			}
			break;
		}

		case STATE_TURNING:
		{
			if(IsMovementComplete())
			{
				linearMovement(40);
				STATE = STATE_FORWARD;
			}
			break;
		}
    }

    // if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
    // {
	// 	STATE = EXIT_STATE_ON_SIDE;
	// }

    // if(OutOfRing())
	// {
    // 	STATE = EXIT_STATE_OUT_OF_RING;
	// }

    // if(Impact())
	// {
    // 	STATE = EXIT_STATE_ON_IMPACT;
	// }
}

void RoundPattern()
{
    if(IsMovementComplete())
    {
    	
    }
	switch(STATE)
    {
		case INITIAL_STATE:
		{
			Turn(200,360);
			STATE = STATE_TURNING;
			break;
		}

		case STATE_TURNING:
		{
			if(IsMovementComplete())
			{
				// keep turning
				Turn(200,360);
			}
			break;
		}
	}
    // if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
    // {
	// 	STATE = EXIT_STATE_ON_SIDE;
	// }

    // if(OutOfRing())
	// {
    // 	STATE = EXIT_STATE_OUT_OF_RING;
	// }

    // if(Impact())
	// {
    // 	STATE = EXIT_STATE_ON_IMPACT;
	// }
}

#define STANDARD_SPEED								160
#define SPEED_CHANGE_STEP							1
#define DISTANCE_BETWEEN_MOTORS						86.00
#define HALF_DISTANCE_BETWEEN_MOTORS				DISTANCE_BETWEEN_MOTORS / 2.00

#define MOVEMENT_SYSTEM_STATUS_OFF					0
#define MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT		1
#define MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT		2

uint8_t movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;

int32_t distancePassedByLeftTire, distancePassedByRightTire,
		distanceExpectedByLeftTire, distanceExpectedByRightTire;

int16_t leftMotorSpeed, rightMotorSpeed;

int8_t directionOfLinearMovement;

float circleMotorSpeedDifference;

bool IsMovementComplete()
{
	return movementSystemStatus == MOVEMENT_SYSTEM_STATUS_OFF;
}

void linearMovement(int32_t distanceInMMWithDirection)
{
	// evcaluate direction and pass to drivers the start state
	if(0 < distanceInMMWithDirection)
	{
		directionOfLinearMovement = 1;
		setMotors(STANDARD_SPEED, STANDARD_SPEED);
	}
	else
	{
		directionOfLinearMovement = -1;
		setMotors(-STANDARD_SPEED, -STANDARD_SPEED);
	}

	// set setpoints for end of movement, used in handler
	distanceExpectedByRightTire = distanceInMMWithDirection;
	distanceExpectedByLeftTire = distanceInMMWithDirection;

	// set motor speeds, used in handler
	rightMotorSpeed = STANDARD_SPEED;
	leftMotorSpeed = STANDARD_SPEED;

	// set state on state machine
	movementSystemStatus = MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT;
}

void Turn(int turnRadius, int turnDegrees)
{
	if(0 < turnDegrees)
	{
		distanceExpectedByLeftTire  = 2 * (turnRadius + HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * turnDegrees / 360;
		distanceExpectedByRightTire = 2 * (turnRadius - HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * turnDegrees / 360;

		circleMotorSpeedDifference = (float)((float)(distanceExpectedByLeftTire) / (float)(distanceExpectedByRightTire));
			
		// set state on state machine
		movementSystemStatus = MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT;
	}
	else
	{
		distanceExpectedByLeftTire  = 2 * (turnRadius - HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * turnDegrees / 360;
		distanceExpectedByRightTire = 2 * (turnRadius + HALF_DISTANCE_BETWEEN_MOTORS) * M_PI * turnDegrees / 360;

		circleMotorSpeedDifference = (float)((float)(distanceExpectedByLeftTire) / (float)(distanceExpectedByRightTire));
			
		// set state on state machine
		movementSystemStatus = MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT;
	}
}

void handleControlledMovement()
{
	switch(movementSystemStatus)
	{
		case MOVEMENT_SYSTEM_STATUS_OFF:
			// stop the motors, they should not be moving
			setMotors(0,0);
			break;
		
		case MOVEMENT_SYSTEM_STATUS_LINEAR_MOVEMENT:
		{
			int32_t leftTyreSpeed = leftEncoder.getSpeed();
			int32_t rightTyreSpeed = rightEncoder.getSpeed();

			// check if the speeds are the same if not correct the speed of the slower motors
			if(leftTyreSpeed > rightTyreSpeed)
			{
				leftMotorSpeed -= SPEED_CHANGE_STEP; 
			}
			else if(leftTyreSpeed < rightTyreSpeed)
			{
				leftMotorSpeed += SPEED_CHANGE_STEP; 
			}

			// update moved distance
			distanceExpectedByLeftTire -= leftTyreSpeed;
			distanceExpectedByRightTire -= rightTyreSpeed;

			if(	(distanceExpectedByLeftTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByLeftTire > 0 && directionOfLinearMovement < 0) )
			{
				leftMotorSpeed = 0;
			}
			
			
			if(	(distanceExpectedByRightTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByRightTire > 0 && directionOfLinearMovement < 0) )
			{
				rightMotorSpeed = 0;
			}

			if(leftMotorSpeed == 0 && rightMotorSpeed == 0)
			{
				movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;
			}

			setMotors(leftMotorSpeed, rightMotorSpeed);

			break;
		}
		
		case MOVEMENT_SYSTEM_STATUS_ROUND_MOVEMENT:
		{
			int32_t leftTyreSpeed = leftEncoder.getSpeed();
			int32_t rightTyreSpeed = rightEncoder.getSpeed();

			// check if the speeds are the same if not correct the speed of the slower motors
			if((float)(leftTyreSpeed) > (float)((float)(rightTyreSpeed) * (float)circleMotorSpeedDifference))
			{
				leftMotorSpeed -= SPEED_CHANGE_STEP; 
			}
			else if((float)(leftTyreSpeed) < (float)((float)(rightTyreSpeed) * (float)circleMotorSpeedDifference))
			{
				leftMotorSpeed += SPEED_CHANGE_STEP; 
			}

			// update moved distance
			distanceExpectedByLeftTire -= leftTyreSpeed;
			distanceExpectedByRightTire -= rightTyreSpeed;

			if(	(distanceExpectedByLeftTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByLeftTire > 0 && directionOfLinearMovement < 0) )
			{
				leftMotorSpeed = 0;
			}
			
			
			if(	(distanceExpectedByRightTire < 0 && directionOfLinearMovement > 0) ||
				(distanceExpectedByRightTire > 0 && directionOfLinearMovement < 0) )
			{
				rightMotorSpeed = 0;
			}

			if(leftMotorSpeed == 0 && rightMotorSpeed == 0)
			{
				movementSystemStatus = MOVEMENT_SYSTEM_STATUS_OFF;
			}

			setMotors(leftMotorSpeed, rightMotorSpeed);

			break;
		}
			
	}
}
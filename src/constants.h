#ifndef __CONSTANTS_H
#define __CONSTANTS_H

#define MICROS_PER_SECOND       1000000

// pins
#define LED_PIN                 13

#define LEFT_SENSOR_PIN         A4
#define RIGHT_SENSOR_PIN        A5

#define MOTOR_LEFT_DIR_PIN      1
#define MOTOR_LEFT_PWM_PIN      6
#define MOTOR_RIGHT_DIR_PIN     0
#define MOTOR_RIGHT_PWM_PIN     5

#define LEFT_A_PIN              12
#define LEFT_B_PIN              11
#define RIGHT_A_PIN             9
#define RIGHT_B_PIN             10

// logging
#define LOG_FILENAME            "LOG"
#define BEGIN_LIFE_BYTE         'B'
#define BEGIN_DATA_BYTE         'b'

// encoder config
#define WHEEL_PERIMETER         (18 * M_PI)
#define IMPULSES_PER_ROUND      358.32

// calibration config
#define SAMPLE_FREQUENCY        250
#define CALIBRATION_FREQUENCY   250
#define LOG_FREQUENCY           5

#define MOTORS_CALIBRATE_SPEED  80, 80

#define MOTOR_MAX_SPEED         255

// lift detection
#define TIRE_LOST_OF_CONTACT_DEGREES 5.00
#define FRONT_LIFTED_THRESHOLD       3.00
#define ZERO_G_DETECTION_THRESHOLD   0.75

#endif // __CONSTANTS_H

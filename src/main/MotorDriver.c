#define MOTOR_LEFT_DIR_PIN    9
#define MOTOR_LEFT_PWM_PIN    10
#define MOTOR_RIGHT_DIR_PIN   12
#define MOTOR_RIGHT_PWM_PIN   11

#define MOTOR_FORWARD_IO_SIGNAL LOW

#define MOTOR_LEFT_PWM_DEFAULT_VALUE   0
#define MOTOR_RIGHT_PWM_DEFAULT_VALUE  0

bool   motorLeftDir, motorRightDir;
uint32_t  motorLeftPWM, motorRightPWM;

static bool   motorLeftDirLocal, motorRightDirLocal;
static uint32_t  motorLeftPWMLocal, motorRightPWMLocal;

// init function for the port driver
uint8_t initMotorsPortDriver(void)
{
  // init all outputs as outputs....
  pinMode(MOTOR_LEFT_DIR_PIN, OUTPUT);
  pinMode(MOTOR_LEFT_PWM_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_DIR_PIN, OUTPUT);
  pinMode(MOTOR_RIGHT_PWM_PIN, OUTPUT);

  // set PWM to default value
  analogWrite(MOTOR_LEFT_PWM_PIN, MOTOR_LEFT_PWM_DEFAULT_VALUE);
  analogWrite(MOTOR_RIGHT_PWM_PIN, MOTOR_RIGHT_PWM_DEFAULT_VALUE);
}

// a function on periodic call used for update of port and PWM states as to required by the upper layer
uint8_t runMotorPortDriverMainFunction(void)
{
  // first we check for change on DIR inputs
  if(motorLeftDir != motorLeftDirLocal)
  {
    // we have a difference, we need to set the new value
    digitalWrite(MOTOR_LEFT_DIR_PIN, motorLeftDir);

    // update the local buffer
    motorLeftDirLocal = motorLeftDir;
  }
  
  // the other DIR input
  if(motorRightDir != motorRightDirLocal)
  {
    // we have a difference, we need to set the new value
    digitalWrite(MOTOR_RIGHT_DIR_PIN, motorRightDir);

    // update the local buffer
    motorRightDirLocal = motorRightDir;
  }

  // PWM signals need to be checked too
  if(motorLeftPWM != motorLeftPWMLocal)
  {
    // in case they are not the same we need to update the real value
    analogWrite(MOTOR_LEFT_PWM_PIN, motorLeftPWM);

    // update the local buffer
    motorLeftPWMLocal = motorLeftPWM;
  }

  // the other PWM signal
  if(motorRightPWM != motorRightPWMLocal)
  {
    // in case they are not the same we need to update the real value
    analogWrite(MOTOR_RIGHT_PWM_PIN, motorRightPWM);

    // update the local buffer
    motorRightPWMLocal = motorRightPWM;
  }

  // return NO_ERROR (0) since no failure is possible for now
  return 0;
}

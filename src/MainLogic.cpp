#include "MainLogic.h"
#include "MotorDriver.h"
#include "FrontLiftedDetection.h"

enum{
    DEFAULT_STATE,
    STATE_TURNING,
    STATE_FORWARD,
    EXIT_STATE_ON_SIDE,
    EXIT_STATE_ON_IMPACT,
    EXIT_STATE_OUT_OF_RING,
    EXIT_STATE_FRONT_LIFTED
}STATE =DEFAULT_STATE;
bool stateImpact = 0;
//Search Patterns
void TroughCernterPattern()
{
    switch(STATE)
    {
    case DEFAULT_STATE:
        if(!IsMovementComplete())
        {
            Turn(0,100);
            state = STATE_TURNING;
        }
        break;
    case STATE_FORWARD:
        if(IsMovementComplete())
        {
            Forward(0,40);
            state = STATE_TURNING;
        }
        break;
    case STATE_TURNING:
        if(IsMovementComplete())
        {
            Turn(100,270);
            state = STATE_FORWARD;
        }
        break;
    }
    if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
    STATE = EXIT_STATE_ON_SIDE; 
    if(OutOfRing())
    STATE = EXIT_STATE_OUT_OF_RING
    if(Impact())
    STATE = EXIT_STATE_ON_IMPACT;
}
void RoundPattern()
{
    if(IsMovementComplete())
    {
    Turn(200,-1);
    STATE = STATE_TURNING;
    }
    if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
    STATE = EXIT_STATE_ON_SIDE; 
    if(OutOfRing())
    STATE = EXIT_STATE_OUT_OF_RING
    if(Impact())
    STATE = EXIT_STATE_ON_IMPACT;
}

//Afster Collision Actions

void Push()
{
    if(STATE == EXIT_STATE_ON_IMPACT)
    {
        setMotors(MAX_SPEED,MAX_SPEED);
        
        //Check Traction TO DO
        if(!Traction)
        {
            Stop();
            //Wait 50 mil
            setMotors(MAX_SPEED - SPEED_CONST,MAX_SPEED - SPEED_CONST);
            //Check C used
            //if no Traction = 0
            //if yes Traction = 1
        }
    }
    
    
    //EXITS
    //Enemy lost 
    if( digitalRead(LEFT_SENSOR_PIN) ^ 1 == 1 || digitalRead(RIGHT_SENSOR_PIN) ^ 1 == 1)
    STATE = EXIT_STATE_ON_SIDE;

    //lifted front scoop
    if(getFrontLiftedState)
    STATE = EXIT_STATE_FRONT_LIFTED;
    //OUT OF RING 
    if(OutOfRing())
    STATE = EXIT_STATE_OUT_OF_RING;
    //Pushed Back TO DO 
}
void WaitToPass()
{
    //WTFFFFFF
}
void TurnTowards()
{
    if(digitalRead(RIGHT_SENSOR_PIN) ^ 1 && IsMovementComplete())
    Turn(0,90); //turn right

    if(digitalRead(LEFT_SENSOR_PIN) ^ 1 && IsMovementComplete())
    Turn(0,-90); // turn left

}
void TurnAround();

    

#include "UICommander.h"

#include "../drivers/ProximitySensors.h"

int8_t getUICommand(void)
{
	static int command_counter = 0;

	static uint8_t last_left = 0;
	static uint8_t last_right = 0;

	uint8_t left = readLeftSensor();
	uint8_t right = readRightSensor();

	int current_command = -1;

	if(right)
	{
		if(left < last_left)
		{
			++command_counter;
		}
	}
	else if(right < last_right)
	{
		if(!left)
			current_command = command_counter;
		command_counter = 0;
	}

	last_left = left;
	last_right = right;

	return current_command;
}

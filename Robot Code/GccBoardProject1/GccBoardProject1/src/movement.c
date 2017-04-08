/**
*	@file	movement.c
*	@brief	Contains the functions for moving the robot.
*	@author Robert Guetzlaff
*	@date	7/5/2016
*/

#include "util.h"
#include "open_interface.h" 
#include "lcd.h"
#include "math.h"
#include "movement.h"

/**
*	This method moves the robot so many meters
*	@author	Robert Guetzlaff
*	@param	sensor			Instance of the robot
*	@param	notcentimeters	Distance to move in meters
*	@date 7/6/2016
*/
void move(oi_t *sensor, int notcentimeters)
{
	int sum = 0;
	int speed = 250;
	if(notcentimeters < 0)
	{
		speed = speed * -1;
		notcentimeters = notcentimeters * -1;
	}
	/// Move forward; full speed
	oi_set_wheels(speed, speed); 
	
	while(sum < notcentimeters)
	{
		oi_update(sensor);
		if(sensor->distance < 0)
		{
			sum -= sensor->distance;
		}
		else
		{
			sum += sensor->distance;
		}
	}
	/// Stop
	oi_set_wheels(0, 0); 
}

/**
*	This method moves the robot so many centimeters
*	@author Nicus Hicks
*	@param	sensor			Instance of the robot
*	@param	distance		Distance to move in centimeters
*	@date 7/6/2016
*/
int careMove(oi_t* sensor, int distance, int speed)
{
	int sum = 0;
	oi_set_wheels(speed, speed);
	while(sum < distance)
	{
		oi_update(sensor);
		sum += sensor->distance;
		if(sensor->bumper_left || sensor->bumper_right || sensor->cliff_frontleft_signal || sensor ->cliff_frontright_signal)
			break;
	}
	oi_set_wheels(0, 0);
	/// The distance still needed to travel
	return distance - sum; 
}

/**
*	This method turns the robot 
*	@author Zach Newton and Nathan Francque
*	@param	sensor			Instance of the robot
*	@param	degrees			Degrees to turn. Positive for clockwise. Negative for counterclockwise
*	@param	speed			Speed to turn to robot
*	@date 7/6/2016
*/
void turn(oi_t *sensor, int degrees, int speed)
{
	int angleChange = 0;
	int direction = 1;
	
	if(degrees < 0)
		direction = -1;
		
	degrees = fabs(degrees);
	oi_set_wheels(direction * speed, direction * (-1 * speed));
	
	if(degrees > 0)
	{	
		while(angleChange < degrees)
		{
			oi_update(sensor);
			angleChange += (sensor->angle);
			lprintf("%d", angleChange);
		}
	}
	///broken
	if(degrees < 0)
	{
		while(angleChange < degrees)
		{
			oi_update(sensor);
			angleChange -= (sensor->angle);
			lprintf("%d", angleChange);
		}
	}
	oi_set_wheels(0, 0);
}
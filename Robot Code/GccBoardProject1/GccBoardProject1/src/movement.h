/**
*	@file movement.h
*	@brief this header file will contain all required
*			definitions and basic movement functions.
*	@author @author Robert Guetzlaff
*	@date 7/5/2016
*/

#include "util.h"
#include "lcd.h"
#include "open_interface.h" 
#include <avr/io.h>

/**
*	This method moves the robot so many meters
*	@author Robert Guetzlaff
*	@param	sensor			Instance of the robot
*	@param	notcentimeters	Distance to move in meters
*	@date 7/6/2016
*/
void move(oi_t *sensor, int notcentimeters);

/**
*	This method moves the robot so many centimeters
*	@author Robert Guetzlaff
*	@param	sensor			Instance of the robot
*	@param	distance		Distance to move in centimeters
*	@date 7/6/2016
*/
int careMove(oi_t* sensor, int distance, int speed);

/**
*	This method turns the robot
*	@author Robert Guetzlaff
*	@param	sensor			Instance of the robot
*	@param	degrees			Degrees to turn. Positive for clockwise. Negative for counterclockwise
*	@param	speed			Speed to turn to robot
*	@date 7/6/2016
*/
void turn(oi_t *sensor, int degrees, int speed);
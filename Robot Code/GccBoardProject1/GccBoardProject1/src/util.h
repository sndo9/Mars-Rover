/**
*	@file		util.h
*	@brief		this header file will contain all required
*				definitions and basic utilities functions.
*	@author		Robert Guetzlaff
*	@date		7/5/2016
*/

#include "open_interface.h"

/// Blocks for a specified number of milliseconds
void wait_ms(unsigned int time_val);

/// Shaft encoder initialization
void shaft_encoder_init(void);

/// Shaft encoder read function
char read_shaft_encoder(void);

/// Initialize Stepper Motor
void stepper_init(void);

/// Stepper motor move function
void move_stepper_motor_by_step(int num_steps, int direction);

/// Initialize PORTC, which is used by the push buttons
void init_push_buttons(void);

/// Return the position of button being pushed
/**
 *	Return the position of button being pushed.
 *	@return the position of the button being pushed.  A 1 is the rightmost button.  0 indicates no button being pressed
 */
char read_push_buttons(void);

/**
*	This method transmits data through serial communication with the Robot
*	@author		Robert Guetzlaff
*	@param		sensor		Oi sensor to transmit data
*	@param		speed		Speed at which to transmite the data
*	@date		7/6/2016
*/
void transmit_data(oi_t* sensor, int speed);

/**
*	This method initializes serial communication with the Robot
*	@author		Robert Guetzlaff
*	@date		7/6/2016
*/
void serial_init(void);

/**
*	This method gets a character via serial communication
*	@author		Robert Guetzlaff
*	@return		Char received from the USART
*	@date		7/6/2016
*/
char serial_getc(void);

/**
*	This method transmits a character via serial communication
*	@author		Robert Guetzlaff
*	@param		data	Char to transmit with the USART
*	@date		7/6/2016
*/
void serial_putc(char data);

/**
*	This method transmits a string via serial communication
*	@author		Robert Guetzlaff
*	@param		data	Char array to transmit with the USART
*	@date		7/6/2016
*/
void serial_puts(char* data);

/**
*	This method moves the Robot using the User Interface provided with the application
*	@author		Robert Guetzlaff
*	@param		sensor	Instance of oi_t, used to move the robot
*	@date		7/6/2016
*/
void movement(oi_t* sensor);

/**
 *	@file util.c
 *
 *	util.c: utility functions for the Atmel platform
 *	For an overview of how timer based interrupts work, see
 *	page 111 and 133-137 of the Atmel Mega128 User Guide
 *
 *  @brief This file holds all utility functions for using the
 *	Timer, Shaft Encoder, Robot Push Buttons, Stepper Motor, 
 *	USART Serial Communication, and User Interface movement
 *
 *	@author Zhao Zhang, Chad Nelson and Robert Guetzlaff
 *	@date 06/26/2012
 */

#include <avr/io.h>
#include <avr/interrupt.h>
#include "util.h"
#include "open_interface.h"
#include "lcd.h"
#include "sweep.h"
#include "string.h"
#include "stdio.h"
#include "movement.h"

#define TIMER_PRESCALER 8
#define CLOCK_SPEED 16000000
#define SOUND_SPEED 34000

/// Global used for interrupt driven delay functions
volatile unsigned int timer2_tick;
void timer2_start(char unit);
void timer2_stop(void);

/**
*	This method blocks for a specified number of milliseconds.
*	@author		Robert Guetzlaff
*	@param		time_val		The amount of time in milliseconds to block
*	@date		7/6/2016
*/
void wait_ms(unsigned int time_val) 
{
	/// Setting OC value for time requested
	/// Clock is 16 MHz. At a pre-scaler of 64, 250 timer ticks = 1ms.
	OCR2 = 250; 				
	timer2_tick = 0;
	timer2_start(0);
	//Waiting for time
	while(timer2_tick < time_val){};
	timer2_stop();
}

/**
*	This method starts Timer 2
*	@author		Robert Guetzlaff
*	@param		unit		Speed for Timer 2
*	@date		7/6/2016
*/
void timer2_start(char unit) {
	timer2_tick = 0;
	/// Unit = 0 is for slow 
	if ( unit == 0 ) 
	{ 		
		/// WGM:CTC, COM:OC2 disconnected, pre-scaler = 64
        TCCR2 = 0b00001011;	
		/// Enabling O.C. Interrupt for Timer2
        TIMSK |= 0b10000000;	
	}
	/// Unit = 1 is for fast
	if (unit == 1) 
	{ 		
		/// WGM:CTC, COM:OC2 disconnected, pre-scaler = 1
        TCCR2 = 0b00001001;
		/// Enabling O.C. Interrupt for Timer2	
        TIMSK |= 0b10000000;	
	}
	sei();
}

/**
*	This method stops Timer 2
*	@author		Robert Guetzlaff
*	@date		7/6/2016
*/
void timer2_stop(void) 
{
	/// Disabling O.C. Interrupt for Timer2
	TIMSK &= ~0b10000000;		
	/// Clearing O.C. settings
	TCCR2 &= 0b01111111;		
}

// 
/**
*	This method is an interrupt handler 
*	@author		Robert Guetzlaff
*	@param		TIMER2_COMP_vect		This interrupt rises every 1 ms
*	@date		7/6/2016
*/
ISR (TIMER2_COMP_vect) 
{
	timer2_tick++;
}

/**
*	This method initializes PORTC to accept push buttons as input
*	@author		Nicus Hicks
*	@date		7/6/2016
*/
void init_push_buttons(void) 
{
	/// Setting PC0-PC5 to input
	DDRC &= 0xC0; 
	/// Setting pins' pull up resistors 
	PORTC |= 0x3F; 
}

/// Return the position of button being pushed
/**
 *	Return the position of button being pushed.
 *	@return the position of the button being pushed.  
 *	A 1 is the rightmost button.  0 indicates no button being pressed
 */
char read_push_buttons(void) 
{
	return 0;
}

/**
*	This method initializes PORTC for input from the shaft encoder
*	@author		Robert Guetzlaff
*	@date		7/6/2016
*/
void shaft_encoder_init(void) 
{
	/// Setting PC6-PC7 to input
	DDRC &= 0x3F;	
	/// Setting pins' pull-up resistors
	PORTC |= 0xC0;	
}

/// Read the shaft encoder
/**
 *	Reads the two switches of the shaft encoder and compares the values
 *	to the previous read.  This function should be called very frequently
 *	for the best results.
 *
 *	@return a value indicating the shaft encoder has moved:
 *	0 = no rotation (switches did not change)
 *	1 = CW rotation
 *	-1 = CCW rotation
 */
char read_shaft_encoder(void) 
{
	return 0;
}

/**
*	This method initializes PORTE to control the stepper motor
*	@author		Nicus Hicks
*	@date		7/6/2016
*/
void stepper_init(void) 
{
	/// Setting PE4-PE7 to output
	DDRE |= 0xF0;  	
	/// Initial position (0b1000) PE4-PE7
	PORTE &= 0x8F;  
	wait_ms(2);
	/// Clear PE4-PE7
	PORTE &= 0x0F;  
}

/// Turn the Stepper Motor
/**
 *	Turn the stepper motor a given number of steps. 
 *
 *	@param		num_steps	A value between 1 and 200 steps (1.8 to 360 degrees)
 *	@param		direction	Indication of direction: 1 for CW and -1 for CCW 
 */
void  move_stepper_motor_by_step(int num_steps, int direction) 
{
}

/**
*	This method transmits data through serial communication with the Robot
*	@author Robert Guetzlaff
*	@param	sensor		Oi sensor to transmit data
*	@param	speed		Speed at which to transmite the data
*	@date	7/6/2016
*/
void transmit_data(oi_t* sensor, int speed)
{
	char output[100];
	oi_update(sensor);
	
	sprintf(output, "BL,%d\n", sensor->bumper_left);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "BR,%d\n", sensor->bumper_right);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "CL,%d\n", sensor->cliff_frontleft);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "CR,%d\n", sensor->cliff_frontright);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "LL,%d\n", sensor->cliff_frontleft_signal);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "LR,%d\n", sensor->cliff_frontright_signal);
	serial_puts(output);
	
	wait_ms(1);
	
	sprintf(output, "S,%d\n", speed);
	serial_puts(output);
	
	wait_ms(1);
	
	serial_puts("END\n");
}

/**
*	This method initializes serial communication with the Robot
*	@author		Robert Guetzlaff
*	@date		7/6/2016
*/
void serial_init(void)
{
	/// Set the baud rate
	unsigned int baud = 34;
	UBRR0H = (unsigned char) (baud >> 8);
	UBRR0L = (unsigned char) baud;
	
	/// Initializes the USART's
	UCSR0A = 0b00000010;
	UCSR0C = 0b00001110;
	UCSR0B = 0b00011000;
}

/**
*	This method gets a character via serial communication
*	@author		Robert Guetzlaff
*	@return		Char received from the USART
*	@date		7/6/2016
*/
char serial_getc(void)
{
	/// Wait for the receive complete flag(RXC)
	while((UCSR0A & 0b10000000) == 0);
	return UDR0;
}

/**
*	This method transmits a character via serial communication
*	@author		Robert Guetzlaff
*	@param		data	Char to transmit with the USART
*	@date		7/6/2016
*/
void serial_putc(char data)
{
	while((UCSR0A & 0b00100000) == 0){}
	UDR0 = data;
}

/**
*	This method transmits a string via serial communication
*	@author		Robert Guetzlaff
*	@param		data	Char array to transmit with the USART
*	@date		7/6/2016
*/
void serial_puts(char* data){	int i = 0;	while(data[i] != '\0' && data[i] != 0)	{		serial_putc(data[i]);		i++;	}	serial_putc('\r');	serial_putc('\n');}

/**
*	This method moves the Robot using the User Interface provided with the application
*	@author		Robert Guetzlaff
*	@param		sensor	Instance of oi_t, used to move the robot
*	@date		7/6/2016
*/
void movement(oi_t* sensor)
{
	int speed = 100;
	/// Output variable for printing the speed
	
	/// Current value coming in from serial communication
	char cur_val = 0;
	/// Number of loops it has received the same input
	int count = 0;
	/// String for outputing data
	char output[50];
	
	
	while(1)
	{
		//oi_update(sensor);
		//lprintf("%d %d %d %d", sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);
		//lprintf("%d", sensor->angle);
		//sprintf(speed_reading, "Current speed is set to %d", speed);
		/// Manual checks the register instead of using serial_getc() 
		while(((UCSR0A & 0b10000000) != 0))
		{
			/// Pull data out of the register
			cur_val = UDR0;
			
			/// Checking what was transmitted and moving the Robot accordingly
			if(cur_val == 'w')
			{
				oi_set_wheels(speed, speed);
				count++;
			}
			if(cur_val == 's')
			{
				oi_set_wheels(-speed, -speed);
				count++;
			}
			if(cur_val == 'a')
			{
				oi_set_wheels(speed, -speed);
				oi_update(sensor);
				sprintf(output, "anglen,%d", sensor->angle);
				serial_puts(output);
			}
			if(cur_val == 'd')
			{
				//Positive angle
				oi_set_wheels(-speed, speed);
				oi_update(sensor);
				sprintf(output, "anglep,%d", sensor->angle);
				serial_puts(output);
			}
			if(cur_val == '+')
			{
				speed += 25;
				count++;
			}
			if(cur_val == '-')
			{
				speed -= 25;
				count++;
			}
			if(cur_val == 'p') oi_play_song(1);
			if(cur_val == 32)
			{
				sweep();
				transmit_data(sensor,speed);
			}
			if(cur_val == 'e')
			{
				turn(sensor, 270, speed);
			}
			if(cur_val == 'q')
			{
				turn(sensor, 90, speed);
			}
			if(cur_val == 'z')
			{
				careMove(sensor, 20, 100);
			}
			
			oi_update(sensor);
			sprintf(output, "%u, %u, %u, %u", sensor->cliff_left_signal, sensor->cliff_frontleft_signal, sensor->cliff_frontright_signal, sensor->cliff_right_signal);
			serial_puts(output);
			/// Checking for cliff impact
			/// If the robot runs into a cliff, it reverse for 5 seconds
			if(sensor->bumper_left)
			{
				oi_set_wheels(-50,-50);
				serial_puts("W,Left Impact");
				wait_ms(5000);
				break;
			}
			if(sensor->bumper_right)
			{
				oi_set_wheels(-50,-50);
				serial_puts("W,Right Impact");
				wait_ms(5000);
				break;
			}
			if(sensor->cliff_frontleft)
			{
				oi_set_wheels(-50,-50);
				serial_puts("W,Cliff left");
				wait_ms(5000);
				break;
			}
			if(sensor->cliff_frontright)
			{
				oi_set_wheels(-50,-50);
				serial_puts("W,Cliff right");
				wait_ms(5000);
				break;
			}
			///Checking for the line boundary
			///If a boundary is found, it reverses
			if(sensor->cliff_frontleft_signal > 500 && sensor->cliff_frontleft_signal < 2000)
			{
				oi_set_wheels(-50, -50);
				serial_puts("W,line front left");
				wait_ms(50);
				break;
			}
			if(sensor->cliff_frontright_signal > 700  && sensor->cliff_frontright_signal < 2000)
			{
				oi_set_wheels(-50, -50);
				serial_puts("W,line front right");
				wait_ms(50);
				break;
			}
			if(sensor->cliff_left > 500  && sensor->cliff_left < 2000)
			{
				oi_set_wheels(-50, -50);
				serial_puts("W,line left signal");
				wait_ms(50);
				break;
			}
			if(sensor->cliff_right > 500  && sensor->cliff_right < 2000)
			{
				oi_set_wheels(-50, -50);
				serial_puts("W,line right signal");
				wait_ms(50);
				break;
			}
			if(sensor->cliff_right_signal < 120)
			{
				oi_play_song(1);
				serial_puts("WIN");
				oi_set_wheels(0,0);
				//wait_ms(10000);
			}
			if(sensor->cliff_frontleft_signal < 120)
			{
				oi_play_song(1);
				serial_puts("WIN");
				oi_set_wheels(0,0);
				//wait_ms(10000);
			}
// 			if(sensor->cliff_frontright_signal < 0)
// 			{
// 				oi_play_song(1);
// 				serial_puts("WIN");
// 				oi_set_wheels(0,0);
// 				wait_ms(10000);
// 			}
			if(sensor->cliff_left_signal < 120)
			{
				oi_play_song(1);
				serial_puts("WIN");
				oi_set_wheels(0,0);
				//wait_ms(10000);
			}
			oi_update(sensor);
			
			wait_ms(10);
		}
		//serial_getc();
		oi_set_wheels(0,0);
		count = 0;
	}
}
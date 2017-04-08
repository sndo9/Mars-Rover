/**
*	@file	sweep.c
*	@brief	Contains the functions for sweeping the infrared and sonar.
*	@author	Zach Newton and Nathan Francque
*	@date	7/5/2016
*/

#include <avr/io.h>
#include "util.h"
#include "lcd.h"
#include "open_interface.h"
#include "movement.h"
#include <math.h>
#include <string.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "sweep.h"

/*PROTOTYPES*/
void serial_puts(char* data);

/*-----------UTILITY FUNCTIONS, STRUCTS AND VARS-----------*/

int max = 4450;
int min = 1100;

#define TIMER_PRESCALER 8
#define CLOCK_SPEED 16000000
#define SOUND_SPEED 34000

/** 
*	This method initializes the servo.
*	@author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void servo_init(void)
{
	/// OC3A is pin 4
	DDRE |= _BV(4); 
	/// Set TOP for 46.51 hz
	OCR3A = 43000; 
	/// Set Servo to Fast PWM mode
	TCCR3A = 0b00100011;
	TCCR3B = 0b00011010;
}

/**
*	This method rotates the Servo 
*   @author		Zach Newton and Nathan Francque
*	@param		degrees		Amount to rotate the servo. 
*							Positive for clockwise and negative for counter-clockwise
*	@date 7/6/2016
*/
void rotate_servo(double degrees)
{
	/// Convert degrees to count, then rotate that many degrees
	int deg2count = min + degrees * (max-min)/180;
	pulse(deg2count);
}

/**
*	This method sends a pulse to the Servo
*   @author	Zach Newton and Nathan Francque
*	@param  count	Width of pulse
*	@date	7/6/2016
*/
void pulse(int count)
{
	OCR3B = count;
}

/**
*	This method returns the absolute value of a signed int
*   @author		Zach Newton and Nathan Francque
*	@param		value		The signed int to take absolute value of
*	@return		absolute value of the parameter, value
*	@date		7/6/2016
*/
unsigned absol(signed value)
{
	if (value < 0) 
	{
		return -value;
	}
	else 
	{
		return value;
	}
}

/**
*	This method initializes all components
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void init_all(void)
{
	servo_init();
	ADC_init();
	sei();
}

#define PI 3.14159265

volatile int counter = 800;
volatile double degrees = 0;
volatile int incdec = 1;

typedef struct reading
{
	int sonar_dist;
	int inf_dist;
	float degrees;
}reading_t;

typedef struct obj 
{
	int startDeg;
	int endDeg;
	int dist;
	int width;
}obj_t;

obj_t obj_rainbow[12];
int obj_gap = 100;
int dist_sum = 0;
int obj_count = 0;

#define read_increment 1
#define reading_rainbow_length 180/read_increment + 1

volatile reading_t reading_rainbow[reading_rainbow_length];

/*---------------SONAR READ VARS AND FUNCTIONS----------------*/
volatile unsigned last_time = 0;
volatile unsigned current_time = 0;
volatile unsigned long time_diff;
volatile int update_flag = 0;
volatile int num_overflows = 0;
long sonar_distance = 0;

/**
*	This method reads the sonar interrupt
*   @author		Zach Newton and Nathan Francque
*	@param		TIMER1_CAPT_vect		This interrupt rises when Timer1 captures input
*	@date		7/6/2016
*/
ISR (TIMER1_CAPT_vect)
{
	/// Update time variables
	last_time = current_time;
	current_time = ICR1;
	update_flag++; 
	/// Toggle clock edge read
	TCCR1B ^= 0b01000000; 
}

/**
*	This method initializes the sonar and reads the sonar data
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void sonar_start(void)
{
	wait_ms(2);
	read_sonar();
	
	while(update_flag != 2){}
		
	if(update_flag == 2)
	{
		/// Account for overflow
		if(last_time > current_time)
		{ 
			current_time += 65536;
			num_overflows += 1;
		}
		time_diff = current_time - last_time;
		sonar_distance = time2dist(time_diff);
		update_flag = 0;
	}
}

/**
*	This method initializes the sonar for reading
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void read_sonar(void)
{
	/// Disable interrupt
	TIMSK &= 0b11011111;
	ping();
	/// Clear ICF1
	TIFR |= 0b00100000; 
	/// Enable interrupt
	TIMSK |= 0b00100000; 
	TCCR1B &= 0b10111000;
	/// Set pre-scaler to 1024(101). To set to 256, change last 3 bits to (100), 8(010)
	TCCR1B |= 0b00000010; 
	/// Set clock edge read to rising edge
	TCCR1B |= 0b01000000; 
}

/**
*	This method pings the PING))) sensor
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void ping(void)
{
	DDRD |= 0b00010000;
	PORTD |= 0b00010000;
	wait_ms(1);
	PORTD &= 0b11101111;
	DDRD &= 0b11101111;
}

/**
*	This method calculates the distance based on the time
*   @author		Zach Newton and Nathan Francque
*	@param		time		
*	@return		distance
*	@date 7/6/2016
*/
unsigned time2dist(unsigned time)
{
	return ((unsigned long)time * SOUND_SPEED)/(CLOCK_SPEED/TIMER_PRESCALER)/2;
}

/*------------------INFRARED SCAN VARS AND FUNCTIONS-----------*/

unsigned int read;
unsigned long inf_distance;

long inf_filtered = 0;
long old = 0;
long read_old = 0;
long read_filter = 0;

/**
*	This method initializes the ADC
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void ADC_init(void)
{
	/// REFS=11, ADLAR=0, MUX don’t care
	ADMUX = _BV(REFS1) | _BV(REFS0);
	/// ADEN=1, ADFR=0, ADIE=0, ADSP=111
	ADCSRA = _BV(ADEN) | (7<<ADPS0);
}

/**
*	This method reads the ADC from the given channel
*   @author		Zach Newton and Nathan Francque
*	@param		channel		Channel to read input from
*	@date		7/6/2016
*/
unsigned ADC_read(char channel)
{
	/// Set channel to read from
	ADMUX |= (channel & 0x1F); 
	/// Start single conversion
	ADCSRA |= _BV(ADSC); 
	while (ADCSRA & _BV(6)){}
	return ADC;
}

/**
*	This method starts receiving data from infrared sensor
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void infrared_start(void)
{
	wait_ms(2);
	read = ADC_read(2);
	/// Equation from excel plot of voltage readings
	inf_distance =  25103 * pow(read, -1.133) + 2; 
	old = inf_filtered;
	/// Actual distance
	inf_filtered = 0.5*old + 0.5*inf_distance;  
	read_old = read_filter;
	/// Filter raw digital read to make calibration easier
	read_filter = 0.5*read_old + 0.5*read; 
}

/*---------------RAW READ OF SCANS-------------*/

/**
*	This method reads sonar wave
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void read_the_rainbow(void)
{
	int i = 0;
	degrees = 0;
	rotate_servo(degrees);
	wait_ms(1000);
	int k = 0;
	while(i < reading_rainbow_length)
	{
		wait_ms(1);
		while(k < 10)
		{
			infrared_start();
			k++;
		}
		k = 0;
		sonar_start();
		
		reading_rainbow[i].inf_dist = inf_filtered;
		reading_rainbow[i].sonar_dist = sonar_distance;
		reading_rainbow[i].degrees = degrees;
		
		wait_ms(1);
		degrees += read_increment;
		rotate_servo(degrees);
		i++;
	}
}

/**
*	This method prints the sonar wave
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void print_the_rainbow(void)
{
	int p = 0;
	char output[50];
	//char message[100] = "";
	//serial_puts("");
	//serial_puts("");
	//sprintf(message,"%8s %8s %8s", "Deg", "Infrared", "Sonar");
	//serial_puts(message);
	wait_ms(100);
	
	while(p < reading_rainbow_length)
	{
		sprintf(output, "IR,%d\n", reading_rainbow[p].inf_dist);
		serial_puts(output);
	  	sprintf(output, "SON,%d\n", reading_rainbow[p].sonar_dist);
		serial_puts(output);
		sprintf(output, "DEG,%lf\n", ((reading_rainbow[p].degrees * 3.1415926)/180));
		serial_puts(output);
		wait_ms(1);
		p += 1;
	}
}

/*--------------ANALYZE READINGS FOR OBJECTS-------------*/

/**
*	This method detects the objects using infrared 
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void obj_detect(void)
{
	int i = 0;
	obj_count = 0;
	/// Memory allocation for previous read
	reading_t previous_read; 
	/// Memory allocation for current read
	reading_t current_read; 
	/// Memory allocation for current object being edited
	obj_t current_obj; 
	/// Boolean variable to check that reading obj currently or not
	char on_object = 0; 
	
	for(i = 1; i < reading_rainbow_length; i++)
	{
		current_read = reading_rainbow[i];
		previous_read = reading_rainbow[i-1];
		/// If infrared jumps to within range, on object
		if(current_read.inf_dist < 80 && !on_object)
		{ 
			on_object = 1;
			/// Set the object start degrees
			current_obj.startDeg = current_read.degrees; 
		} 
		/// Infrared jumps out of range or end of servo range, off object
		else if((current_read.inf_dist >= 80 || current_read.degrees == 180) && on_object )
		{ 
			on_object = 0;
			/// Set the object end degrees
			current_obj.endDeg = current_read.degrees; 
			/// Set the distance of the object
			current_obj.dist = previous_read.inf_dist; 
			/// Calculate the width of the object
			current_obj.width = tan(((current_obj.endDeg-current_obj.startDeg)/2) * PI / 180.0) * current_obj.dist * 2; 
			/// Add object to object array
			obj_rainbow[obj_count] = current_obj; 
			obj_count++;
		}
	}
}

/**
*	This method prints the objects found
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void print_objects(void)
{
	int p = 0;
	char output[50];
	wait_ms(100);
	while(p < obj_count)
	{
		serial_puts("garbage");
		sprintf(output, "DD,%d\n", obj_rainbow[p].endDeg);
		serial_puts(output);
		wait_ms(10);
		sprintf(output, "Start,%d\n", obj_rainbow[p].startDeg);
		serial_puts(output);
		wait_ms(10);
		sprintf(output, "DIS,%d\n", obj_rainbow[p].dist);
		serial_puts(output);
		wait_ms(1);
		p += 1;
	}
}


/**
*	This is an test method emulating the main method in main.c
*	This method tests object detection
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void sweep(void)
{
	init_all();
	rotate_servo(0);
	wait_ms(10);
	read_the_rainbow();
	//print_the_rainbow();
	obj_detect();
	print_objects();
		
}

/**
* @file sweep.h
* @brief this header file will contain all required
*		definitions and functions for reading IR and Sonar.
* @author Zach Newton and Nathan Francque
* @date 7/5/2016
*/

/**
*	This method rotates the Servo
*   @author		Zach Newton and Nathan Francque
*	@param		degrees		Amount to rotate the servo.
*							Positive for clockwise and negative for counter-clockwise
*	@date 7/6/2016
*/
void rotate_servo(double degrees);

/**
*	This method returns the absolute value of a signed int
*   @author		Zach Newton and Nathan Francque
*	@param		value		The signed int to take absolute value of
*	@return		absolute value of the parameter, value
*	@date		7/6/2016
*/
unsigned absol(signed value);

/**
*	This method initializes all components
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void init_all(void);

/**
*	This method initializes the sonar and reads the sonar data
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void sonar_start(void);

/**
*	This method initializes the sonar for reading
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void read_sonar(void);

/**
*	This method pings the PING))) sensor
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void ping(void);

/**
*	This method calculates the distance based on the time
*   @author		Zach Newton and Nathan Francque
*	@param		time
*	@return		distance
*	@date 7/6/2016
*/
unsigned time2dist(unsigned time);

/**
*	This method initializes the ADC
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void ADC_init(void);

/**
*	This method reads the ADC from the given channel
*   @author		Zach Newton and Nathan Francque
*	@param		channel		Channel to read input from
*	@date		7/6/2016
*/
unsigned ADC_read(char channel);

/**
*	This method starts receiving data from infrared sensor
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void infrared_start(void);

/**
*	This method reads sonar wave
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void read_the_rainbow(void);

/**
*	This method prints the sonar wave
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void print_the_rainbow(void);

/**
*	This method detects the objects using infrared
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void obj_detect(void);

/**
*	This method prints the objects found
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void print_objects(void);

void pulse(int count);

/**
*	This is an test method emulating the main method in main.c
*	This method tests object detection
*   @author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void sweep(void);

/**
*	This method initializes the servo.
*	@author		Zach Newton and Nathan Francque
*	@date		7/6/2016
*/
void servo_init(void);
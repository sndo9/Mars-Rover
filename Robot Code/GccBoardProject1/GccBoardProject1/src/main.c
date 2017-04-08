/**
*	@file	main.c
*	@brief	Contains the main method.
*
*	@author Robert Guetzlaff
*
*	@date	7/6/2016
*/

#include <asf.h>
#include <avr/io.h>
#include "lcd.h"
#include "util.h"
#include "open_interface.h"
#include "sweep.h"

int i;

/**
*	This is the main method for the application.
*	@author		Robert Guetzlaff
*	@date		7/6/2016
*/
int main (void)
{
	/// Initial setup
	board_init();
	lcd_init();
	serial_init();
	oi_set_wheels(0,0);
	
	/// The note array to play the song
	/// The notes start at 31 to 127 
	unsigned char notes[] = {69,69,69, 65, 72, 69, 65, 72, 69};
	unsigned char notes_duration[] = {50,50, 50, 38, 13, 50, 38, 13, 100}; 
	
	oi_set_wheels(0,0);
	oi_t *sensor_data = oi_alloc();
	oi_init(sensor_data);
	
	lprintf("HI");
	
	oi_load_song(1, 9, notes, notes_duration); 
	// oi_play_song(1);
	oi_set_wheels(0,0);
	
	wait_ms(2000);
	
	/// Infinite loop for application
	while (1)
	{
		oi_update(sensor_data);
		movement(sensor_data);
		wait_ms(2000);
	}
}

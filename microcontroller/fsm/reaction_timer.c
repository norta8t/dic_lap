/*-------------------------------------------------------------------------*\
| File:         reaction_timer.c
| Version:      1.0
| Description:  Implementation of a reaction time game as finite 
|               state machine
| MEGACARD:     v6
| Autor:        LAP
| Created:      09/2024
|
| Changed:      -
\*-------------------------------------------------------------------------*/

#define __DELAY_BACKWARD_COMPATIBLE__	// To avoid error message "builtin_avr_delay_cycles expects a compile time integer constant"

#include <avr/io.h>
#include <util/delay.h>
#include <stdlib.h>			// For rand() function
#include "display.h"

// Define the ports and pins for the LEDs and switch
#define LED_DDR		DDRC	
#define LED_PORT	PORTC

// Define FSM states
typedef enum {
	WAITING,       // Waiting for user to start the game
	RANDOM_DELAY,  // Random delay before LED lights up
	LED_ON,        // LED is on, waiting for user input
	RESULT         // Display reaction result
} State;

void init (void)
{

   LED_DDR = 0xFF;	// Set output
   DDRA    &= ~(1 << PA0);	// Set PORTA as input (respective pin is cleared)
   PORTA   |= (1 << PA0);	// Enable pull-up resistor of PA0 (respective pin is set to 1)
   
}

uint8_t isButtonPressed(void){
	
    if (!(PINA & (1 << PA0))) {  //
		
		// Button is pressed
	    _delay_ms(50);  // Simple debounce
		
	    if (!(PINA & (1 << PA0))) {
		    return 1;
	    }
    }
	
    return 0;	
}

void randomDelay(void){
	int delayTime = (rand() % 2000 + 1000);
	_delay_ms(delayTime);
}

void lightRandomLED(){
	int randomLED = rand() % 5;
	LED_PORT = (1 << randomLED);
}

uint16_t measureReactionTime(void){
	uint16_t reactionTime = 0;
	
	while (PINA & (1 << PA0)) {
		_delay_ms(10);
		reactionTime += 10;
	}
	
	return(reactionTime);
}

int main (void)
{
   init ();        // Aufruf der Grundinitialisierungen
   display_init(); // Initialisierung der Anzeige
   
   display_string_pos (0, 2, "Reaction Timer");
   
   uint16_t reactionTime = 0;
   
   // Initial State is WAITING
   State currentState = WAITING;

   while (1)
   {
      // Main loop
	  switch (currentState){
		  
		case WAITING:
			// Turn off all LEDs and wait for button press
			display_string_pos (0, 6, "WAITING     ");
			LED_PORT = 0x00;
			
			if (isButtonPressed()){
				currentState = RANDOM_DELAY;
			}
			break;
			
		case RANDOM_DELAY:
			display_string_pos (0, 6, "RANDOM_DELAY");
			randomDelay();
			
			currentState = LED_ON;
			
			break;
		
		case LED_ON:
			
			display_string_pos (0, 6, "LED_ON      ");
			lightRandomLED();
			reactionTime = measureReactionTime();
			
			currentState = RESULT;

			break;
		
		case RESULT:
			// Show reaction time on display for 5 seconds
			display_printf_pos (0, 4, "Time: %i ms", reactionTime);
			display_string_pos (0, 6, "RESULT      ");
			_delay_ms(5000);
			display_printf_pos (0, 4, "                ", reactionTime);
			reactionTime = 0;
			currentState = WAITING;
			break;
	  }
   }

   return 0;
}

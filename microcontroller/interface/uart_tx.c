/*-------------------------------------------------------------------------*\
| File:         uart_tx.c
| Version:      1.0
| Description:  Simple implementation of sending a string via UART
| MEGACARD:     v6
| Autor:        LAP
| Created:      09/2024
|
| Changed:      -
\*-------------------------------------------------------------------------*/

#include <avr/io.h>
#include <util/delay.h>
#include "display.h"
#define BAUD 9600UL			//Baudrate
#include <util/setbaud.h>


void init (void)
{
	UBRRH = UBRRH_VALUE;
	UBRRL = UBRRL_VALUE;
	UCSRB |= (1 << TXEN);	//Transmit (TX) enable
	UCSRC |= (1 << URSEL) | (1 << UCSZ1) | (1 << UCSZ0);   //asynchron, 8 data bits, no parity, 1 stopbit
}

void send_char(char c)
{
	while(!(UCSRA & (1 << UDRE)))		//wait until send is possible
	{
	}
	UDR = c;
}

void send_string(char *s){
	while(*s)
	{
		send_char(*s);
		s++;
	}
}

int main (void)
{
	init ();  
	
	while (1)
	{
		// Hauptschleife
		send_string("Test ");
		_delay_ms(1000);
	}
	
	return 0;
}
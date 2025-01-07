/*-------------------------------------------------------------------------*\
| File:         spi_tc77.c
| Version:      1.0
| Description:  Interfacing with the PICkit SPI Demo Board (TC77-5.0)
| MEGACARD:     v6
| Autor:        LAP
| Created:      01/2025
|
| Changed:      -
\*-------------------------------------------------------------------------*/
#include <avr/io.h>
#include <util/delay.h>
#include "display.h"

#define SCK PB7
#define MISO PB6
#define MOSI PB5
#define CS PB4

void init (void)
{
	// Init
	DDRB |= (1 << SCK) | (1 << MOSI) | (1 << CS); // Outputs
	SPCR |= (1 << SPE) | (1 << MSTR) | (1 << SPR0);
	PORTB |= (1 << CS);
}

// Function to send dummy data to receive two bytes of temperature data via SPI
int spi_tx(void){
	
	int temp_raw;
	PORTB &= ~(1 << CS);
	
	SPDR = 0x00; // Send dummy byte
	while (!(SPSR & (1 << SPIF))); // Wait for transmission complete
	temp_raw = SPDR << 8; // Read high byte
	
	SPDR = 0x00; // Send dummy byte
	while (!(SPSR & (1 << SPIF))); // Wait for transmission complete
	temp_raw |= SPDR; // Read low byte
	
	PORTB |= (1 << CS);
	return(temp_raw);
}

// Convert raw SPI data to temperature in °C
float calc_temp(int temp_raw){
	float temperature;
	int16_t sign_mask = 0x1000;
	
	temp_raw = (temp_raw >> 3);
	
	// Check if number is negative
	if (temp_raw & sign_mask){
		temp_raw = ~temp_raw + 1;
		temp_raw = temp_raw * -1;
	}
	
	temperature = temp_raw * 0.0625;
	
	return (temperature);
}

int main (void)
{
	uint16_t temp_raw;
	float temperature;
	
	init ();        // Aufruf der Grundinitialisierungen
	display_init(); // Initialisierung der Anzeige
	display_string_pos (0, 4, "  SPI TC77");
	
	while (1)
	{
		temp_raw = spi_tx();
		temperature = calc_temp(temp_raw);
		display_printf_pos(0, 6, "Temp: %0.1f C", temperature);
		_delay_ms(1000);
	}
	
	return 0;
}

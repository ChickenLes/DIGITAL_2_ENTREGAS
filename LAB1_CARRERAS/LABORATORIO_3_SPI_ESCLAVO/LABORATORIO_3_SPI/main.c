/*
 * LABORATORIO_3_SPI.c
 *
 * Created: 2/3/2026 10:33:04 AM
 * Author : itzle
 */ 


#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include "SPI_LIB/SPI_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART/UART_LIB.h"


volatile uint8_t POTE1 = 0;
uint8_t POTE2 = 0;

void setup(){
	DDRD = 0xFF;
	PORTD = 0x00;
}


int main(void)
{
	setup();
	ADC_CONF();
	SPI_INIT(SPI_SLAVE, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();
	
    /* Replace with your application code */
    while (1) 
    {
		POTE1 = ADC_READ(1)/4;
		POTE2 = ADC_READ(2)/4;
		
		PORTD =(POTE2>>4)<<2;
    }
}

ISR(SPI_STC_vect){
	
	uint8_t ORDEN = SPDR;
	
	if(ORDEN == '1')
	{
	SPDR = POTE1;
	}
	else
	{
		SPDR = 0;
	}
	
}
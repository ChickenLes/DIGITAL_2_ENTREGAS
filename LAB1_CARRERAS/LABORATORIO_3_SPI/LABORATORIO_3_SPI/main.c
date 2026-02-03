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

#define SS_LOW() PORTB &= ~(1<<DDB2);
#define SS_HIGH() PORTB |= (1<<DDB2);


void setup(){
	DDRD = 0xFF;
	PORTD = 0x00;
}


int main(void)	
{
	setup();
	
	uint8_t dato_recibido = 0;	
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	
	
    /* Replace with your application code */
    while (1) 
    {
		SS_LOW();
		SPI_WRITE('1');
		_delay_us(50);
		
		SPI_WRITE(0x00);
		dato_recibido = SPDR;
		SS_HIGH();
		
		PORTD = (dato_recibido >>4) << 2;
		_delay_ms(100);
		
    }
}


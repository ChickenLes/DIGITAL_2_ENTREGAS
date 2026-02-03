//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_SPI_MASTER.c
// AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
//AUTOR1: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: LABORATORIO SPI
// HARDWARE: ATMEGA328P
// CREADO: 02/02/2026
// ULTIMA MODIFICACION: 02/03/2025
// DESCRIPCIÓN: el siguiente código tiene como objetivo el utillizar un arduino nano como receptor maestro de datos y presentar los valores en leds y en
//la terminal UART de los valores del esclavo.
//-----------------------------------------------

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "SPI_LIB/SPI_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART/UART_LIB.h"

#define SS_LOW() PORTB &= ~(1<<DDB2);
#define SS_HIGH() PORTB |= (1<<DDB2);

uint16_t dato_recibido1 = 0;
uint16_t dato_recibido2 = 0;


char buffer_rx[20];
char buffer[40];

void setup(){
	DDRD = 0xFF;
	PORTD = 0x00;
}


int main(void)	
{
	setup();
	UART_CONF();
	sei();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	
	
	

	
	UART_PrintString("Leyendo del esclavo");
	UART_PrintString("1 y 2 para lectura");
	
	
	
    /* Replace with your application code */
    while (1) 
    {
		
		
		
		SS_LOW();
		SPI_WRITE('1');
		_delay_us(50);
		
		SPI_WRITE(0x00);
		dato_recibido1 = SPDR;
		SS_HIGH();
	
		_delay_ms(2);
	
		SS_LOW();
		SPI_WRITE('2');
		_delay_us(50);
		
		SPI_WRITE(0x00);
		dato_recibido2 = SPDR;
		SS_HIGH();
	
				
	
		_delay_ms(100);
		
    }
}

void ENVIAR_UART(uint8_t dato_recibido1, uint8_t dato_recibido2){
	if (COMANDO_NUEVO())
	{
		RECIBIR_COMANDO(buffer_rx);
		if(buffer_rx[0] == 'S' )
		{
			printf(buffer, "%d", dato_recibido1);
			UART_PrintString(buffer);
		}
		else if (buffer_rx[0] == 'P' )
		{
			printf(buffer, "%d", dato_recibido2);
			UART_PrintString(buffer);
		}
		
		
	}
}
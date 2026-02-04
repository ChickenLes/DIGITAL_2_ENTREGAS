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

//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// LAB_SPI_MASTER.c
//-----------------------------------------------

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdlib.h> // Necesario para atoi()
#include <stdio.h>
#include <string.h>

#include "SPI_LIB/SPI_LIB.h"
#include "UART/UART_LIB.h"

#define SS_LOW()  PORTB &= ~(1 << DDB2);
#define SS_HIGH() PORTB |= (1 << DDB2);

int main(void)
{
	char buffer_recepcion[20];
	char buffer_salida[50];
	uint8_t valor_spi = 0;
	
	UART_CONF();
	SPI_INIT(SPI_MASTER_DIV16, DATA_MSB, CLOCK_LOW, FIRST_EDGE);
	sei();
	
	UART_PrintString("1 o 2");

	while (1)
	{
		if (COMANDO_NUEVO())
		{
			RECIBIR_COMANDO(buffer_recepcion);
			
			int numero = atoi(buffer_recepcion);

			if (numero == 1 || numero == 2)
			{
				SS_LOW();
				SPI_WRITE(numero);
				_delay_us(50);
				SPI_WRITE(0x00);
				valor_spi = SPDR;
				SS_HIGH();
				uint32_t LECTURA_ADC = ((uint32_t)valor_spi * 5000) / 255;
				uint8_t PARTE_ENTERA = LECTURA_ADC / 1000;
				uint8_t PARTE_DECIMAL = (LECTURA_ADC % 1000) / 10;
				sprintf(buffer_salida, "Lectura Pote %d: %d.%02dV\r\n",
				numero,  PARTE_ENTERA, PARTE_DECIMAL);
				UART_PrintString(buffer_salida);
			}
			else
			{
				UART_PrintString("Error: Ingrese numero 1 o 2.\r\n");
			}
		}
	}
}
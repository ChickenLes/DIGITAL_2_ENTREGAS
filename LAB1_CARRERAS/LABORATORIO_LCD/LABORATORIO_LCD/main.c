/*
 * LABORATORIO_LCD.c
 *
 * Created: 1/26/2026 9:29:35 AM
 * Author : itzle
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdio.h>
#include "LCD/LCD_LIB.h"
#include "ADC/ADC_LIB.h"
#include "UART_LIB/UART_LIB.h"


//VARIABLES
char buffer[16]; //BUFFER PARA ALMACENAR DATOS PARA LCD
char buffer_rx[20]; //BUFFER PARA EL UART
//VARIABLES ADC
uint16_t valor_adc = 0;
uint16_t valor_adc2 = 0;


//CONTADOR UART
uint8_t CONTADOR_UART = 0;


void MODULO_LCD(uint16_t VALOR_S1, uint16_t VALOR_S2, uint8_t CONTADOR_UART);
void CONTADOR(void);

int main(void) {
	


	INICIAR_LCD();
	ADC_CONF();
	UART_CONF();
	sei();
	
	//MENSAJE DE BIENVENIDA LCD
	CURSOR(0,3);
	LCD_WRITE_STRING("BIENVENIDO");
	CURSOR(1,1);
	LCD_WRITE_STRING("PC0=S1");
	CURSOR(1, 9);
	LCD_WRITE_STRING("PC1=S2");
	//MENSJE DE BIENVENIDA UART
	UART_PrintString("'+' para incrementar y '-' para decrementar:");
	
	_delay_ms(1500);
	
	//RESET DEL LCD
	COMANDO_LCD(0x01);
	_delay_ms(2);

	while (1) {
		
		//VARIABLES PARA LEER PC0 Y PC1
		valor_adc = ADC_READ(0);
		valor_adc2 = ADC_READ(1);
		
	
		CONTADOR();
		MODULO_LCD(valor_adc, valor_adc2, CONTADOR_UART);

		_delay_ms(100);
		
	}
}

//MODULO PARA ACTUALIZAR VALOR PANTALLA LCD
void MODULO_LCD(uint16_t VALOR_S1, uint16_t VALOR_S2, uint8_t CONTADOR_UART){
	
	
	uint32_t LECTURA_ADC = ((uint32_t)VALOR_S1 * 5000) / 1023;
	
	uint8_t PARTE_ENTERA = LECTURA_ADC / 1000;
	uint8_t PARTE_DECIMAL = (LECTURA_ADC % 1000) / 10;

	CURSOR(0, 1);
	LCD_WRITE_STRING("S1");
	
	CURSOR(1,0);
	sprintf(buffer, "%d.%02dV", PARTE_ENTERA, PARTE_DECIMAL);
	LCD_WRITE_STRING(buffer);

	CURSOR(0, 7);
	LCD_WRITE_STRING("S2");
	
	CURSOR(1, 6);
	sprintf(buffer, "%4d", valor_adc2);
	LCD_WRITE_STRING(buffer);
	
	CURSOR(0, 12);
	LCD_WRITE_STRING("S3");
	
	CURSOR(1,11);
	sprintf(buffer, "%3d", CONTADOR_UART);
	LCD_WRITE_STRING(buffer);
	
}

//MODULO CONTADOR UART
void CONTADOR(void){
	if (COMANDO_NUEVO())
	{
		RECIBIR_COMANDO(buffer_rx);

		if (buffer_rx[0] == '+')
		{
			CONTADOR_UART++;
		}
		else if (buffer_rx[0] == '-')
		{
			CONTADOR_UART--;
		}
		
	}
	if(CONTADOR_UART >= 64){
		
		CONTADOR_UART = 0;
		
	}
	
	
}

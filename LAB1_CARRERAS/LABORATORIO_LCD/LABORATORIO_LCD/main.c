/*
 * LABORATORIO_LCD.c
 *
 * Created: 1/26/2026 9:29:35 AM
 * Author : itzle
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <stdio.h>
#include "LCD/LCD_LIB.h"
#include "ADC/ADC_LIB.h"



int main(void) {
	
	
	char buffer[16];
	//ADC_1
	uint16_t valor_adc = 0;
	uint32_t LECTURA_ADC = 0;
	uint8_t PARTE_ENTERA = 0;
	uint8_t PARTE_DECIMAL = 0;

	//ADC_2
	uint16_t valor_adc2 = 0;
	uint32_t LECTURA_ADC2 = 0;
	uint8_t PARTE_ENTERA2 = 0;
	uint8_t PARTE_DECIMAL2 = 0;


	INICIAR_LCD();
	ADC_CONF();
	
	CURSOR(0,0);
	LCD_WRITE_STRING("BIENVENIDO");
	_delay_ms(1000);
	
	
	COMANDO_LCD(0x01);
	_delay_ms(2);

	while (1) {
		
		valor_adc = ADC_READ(0);

		LECTURA_ADC = ((uint32_t)valor_adc * 5000) / 1023;

		
		PARTE_ENTERA = LECTURA_ADC / 1000;
		PARTE_DECIMAL = (LECTURA_ADC % 1000) / 10;

		CURSOR(0, 2);
		LCD_WRITE_STRING("S1");
		
		CURSOR(1,1);
		sprintf(buffer, "%d.%02d V   ", PARTE_ENTERA, PARTE_DECIMAL);
		LCD_WRITE_STRING(buffer);

		_delay_ms(200);
	}
}


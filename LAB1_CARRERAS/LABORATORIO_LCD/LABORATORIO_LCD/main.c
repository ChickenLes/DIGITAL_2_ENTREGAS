/*
 * LABORATORIO_LCD.c
 *
 * Created: 1/26/2026 9:29:35 AM
 * Author : itzle
 */ 

#include <avr/io.h>
#include "LCD/LCD_LIB.h"
#include "ADC/ADC_LIB.h"

int main(void)
{
    INICIAR_LCD();
	ADC_CONF();
	
	char buffer[10];
	uint16_t VALOR_ADC;
	
    while (1) 
    {
		
		
    }
}


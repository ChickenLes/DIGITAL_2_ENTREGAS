/*
 * LABORATORIO_LCD.c
 *
 * Created: 1/26/2026 9:29:35 AM
 * Author : itzle
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include "LCD/LCD_LIB.h"
#include "ADC/ADC_LIB.h"

int main(void)
{
    INICIAR_LCD();
	
	CURSOR(0, 0);
	LCD_WRITE_STRING("HOLA MUNDO!");
	
	CURSOR(1, 2);
	LCD_WRITE_STRING(":D");
	
    while (1) 
    {	
		
    }
}


/*
 * LCD_LIB.c
 *
 * Created: 1/26/2026 9:46:14 AM
 *  Author: itzle
 */ 

#include "LCD_LIB.h"

void LCD_PORT(void){
	
	uint8_t PORTD_TEMP = ()
	
	
	
}
void LCD_LECTURA(void){
	
	LCD_CONTROL |= (1<<PIN_ENABLE); //ENABLE = 1
	
	
	
}

void INICIAR_LCD(void){
	
	//////////////CONFIGURAR SALIDAS////////////
	//CONTROLES
	CONTROL_DDR |= (1<<PIN_RS)|(1<<PIN_WR)|(1<<PIN_ENABLE);
	LCD_DDRD |= PORTD_MASK;
	LCD_DDRB |= PORTB_MASK;
	
	//ESTADO INICIAL
	LCD_CONTROL &= ~((1<<PIN_RS)|(1<<PIN_WR)|(1<<PIN_ENABLE));
	_delay_ms(20);
	
	COMANDO_LCD(0x38); //MODO 8 bits (0011 1000) 
	_delay_ms(5);
	COMANDO_LCD(0x38); //MODO 8 bits (0011 1000)
	_delay_ms(100);
	
	COMANDO_LCD(0x0C); //DISPLAY ON, CURSOR OFF, BLINK OFF
	COMANDO_LCD(0x01); //CLEAN
	_delay_ms(2);
	COMANDO_LCD(0x06); //INCREMENTAR CURSOR DERECHA
}

void COMANDO_LCD(uint8_t COMANDO){
	
	LCD_CONTROL |= (1<<PIN_RS); //ENCENDEMOS EL RST
	LCD_CONTROL &= (1<<PIN_WR); //APAGAMOS ESCRITURA Y LECTURA
	
	
	
}
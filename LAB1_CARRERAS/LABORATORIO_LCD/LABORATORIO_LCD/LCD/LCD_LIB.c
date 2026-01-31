/*
 * LCD_LIB.c
 *
 * Created: 1/26/2026 9:46:14 AM
 *  Author: itzle
 */ 

#include "LCD_LIB.h"

void LCD_PORT(char DATO){
	
	//PORTD D2 - D7
	uint8_t PORTD_TEMP = (LCD_PORTD & ~PORTD_MASK) | (DATO & PORTD_MASK);
	LCD_PORTD = PORTD_TEMP;
	
	//PORTB D0 - D1
	uint8_t PORTB_TEMP = (LCD_PORTB & ~PORTB_MASK) | (DATO & PORTB_MASK);
	LCD_PORTB = PORTB_TEMP;
	
	
	
}

//OBTENIDO DE CHATGPT
void LCD_LECTURA(void){
	ENABLE_1();
	_delay_us(1);
	ENABLE_0();
	_delay_us(50);
}

void INICIAR_LCD(void){
	
	//////////////CONFIGURAR SALIDAS////////////
	//CONTROLES
	CONTROL_DDR |= (1<<PIN_RS)|(1<<PIN_WR)|(1<<PIN_ENABLE);
	LCD_DDRD |= PORTD_MASK;
	LCD_DDRB |= PORTB_MASK;
	
	//ESTADO INICIAL
	LCD_CONTROL &= ~((1<<PIN_RS)|(1<<PIN_WR)|(1<<PIN_ENABLE));
	_delay_ms(200);
	
	COMANDO_LCD(0x38); //MODO 8 bits (0011 1000) 
	_delay_ms(5);
	COMANDO_LCD(0x38); //MODO 8 bits (0011 1000)
	_delay_ms(200);
	
	COMANDO_LCD(0x0C); //DISPLAY ON, CURSOR OFF, BLINK OFF
	COMANDO_LCD(0x01); //CLEAN
	_delay_ms(2);
	COMANDO_LCD(0x06); //INCREMENTAR CURSOR DERECHA
}

void COMANDO_LCD(uint8_t COMANDO){
	
	RS_0();	//RS = 0
	WR_WRITE(); //WR = 0
	LCD_PORT(COMANDO);	//MANDAR LOS COMANDOS A LCD_PORT
	LCD_LECTURA();
	
}

void LCD_WRITE_CHAR(char DATO){
	
	RS_1();	//RS = 1
	WR_WRITE(); //WR = 0
	LCD_PORT(DATO); //MANDAR DATOS A LCD_PORT
	LCD_LECTURA();
	
	
}

void LCD_WRITE_STRING(char *TEXTO){
	
	while(*TEXTO){
		LCD_WRITE_CHAR(*TEXTO);
			TEXTO++;

	}
		
}

void CURSOR(uint8_t FILA, uint8_t COLUMNA){
	
	uint8_t LOCATION = 0;
	if(FILA == 0){
		LOCATION = 0x80 + COLUMNA;
		
	}
	
	else
	{
		LOCATION = 0xC0 + COLUMNA;
		
	}
	COMANDO_LCD(LOCATION);
	
	
}


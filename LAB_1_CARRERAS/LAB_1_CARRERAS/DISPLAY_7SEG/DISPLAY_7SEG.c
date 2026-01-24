/*
 * DISPLAY_7SEG.c
 * Created: 1/24/2026 9:48:56 AM
 *  Author: Anthony Boteo
 */ 

#include "DISPLAY_7SEG.h"


const uint8_t TABLA[] = {
	//DEL 1 AL 9
	0x3F, //0
	0x0C, //1
	0x5B, //2
	0x5E, //3
	0x6C, //4
	0x76, //5
	0x77, //6
	0x1C, //7
	0x7F, //8
	0x7E, //9
	//HEXA
	0x7D, //A
	0x67, //b
	0x33, //C
	0x4F, //d
	0x73, //E
	0x71//F
};

void iniciar_display(void){
	
	
	//DEJAMOS SOLO LOS PINES QUE ESTÁ UTILIZANDO EL DISPLAY
	DISPLAY_DDR |= DISPLAY_MASK;
	
}

void mostrar_display(uint8_t numero){
	
	//SOLO MOSTRAR HASTA F
	numero &= 15;
	
	uint8_t dibujo = TABLA[numero];
	
	
	uint8_t VALOR_DISPLAY_T = DISPLAY_PORT & ~(DISPLAY_MASK);
	VALOR_DISPLAY_T |= (dibujo & DISPLAY_MASK);
	
	DISPLAY_PORT = VALOR_DISPLAY_T;
	
	
}
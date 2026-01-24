//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// ELECTRÓNICA DIGITAL 2 - SECCIÓN - 20 -
// LAB_CARRERAS.c
// AUTOR: ANTHONY ALEJANDRO BOTEO LÓPEZ
// PROYECTO: PRELABORATORIO 1
// HARDWARE: ATMEGA328P
// CREADO: 01/19/2026
// ULTIMA MODIFICACION: 04/04/2025
// DESCRIPCIÓN:
//-----------------------------------------------


///////////////////////LIBRERIAS//////////////////////
#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h> //SI UTILIZAMOS BOOL
#include "DISPLAY_7SEG/DISPLAY_7SEG.h"
///////////////////////VARIABLES_GLOBALES//////////////////////
volatile uint8_t C1 = 0x00;//VALOR CONTADOR 1
volatile uint8_t C2 = 0x00;//VALOR CONTADOR 2
volatile bool Play_time = 0;//BANDERA HABILITAR CARRERA

volatile uint8_t PC1ANTI = 0x00;//ANTIREBOTE BOTON START
volatile uint8_t PC2ANTI = 0x00;//ANTIREBOTE BOTON P1
volatile uint8_t PC3ANTI = 0x00;//ANTIREBOTE BOTON P2

volatile uint8_t DISPLAY_VALUE = 5;
volatile uint8_t ESTADO_DISPLAY = 0; //0: OFF, 1: ON

volatile uint16_t contador_segundos = 0x00; //VARIABLE PARA CONTAR LOS SEGUNDOS 1000MS = 1S



void setup(){

	//OUTPUTS
	DDRD= 0xFF;	//PORTD OUTPUT
	PORTD = 0x00;
	DDRB= 0xFF;	//PORTB OUTPUT
	PORTB = 0x00;
	DDRC|= (1 << PINC0);	//PINC0 OUTPUT
	PORTC &= ~(1 << PINC0);

	//INPUT
	DDRC&= ~((1 << PINC1)|(1 << PINC2)|(1 << PINC3));	//PINC1, PINC2 Y PINC3 COMO INPUTS
	PORTC |= (1 << PINC1)|(1 << PINC2)|(1 << PINC3) ;	//PULL UP ACTIVADO

}

//////////////////CONFIGURACION INTERRUPCIONES/////////////////////////
//CONFIGURACION PINCHANGE
void setup_pinchange(){

	//HABILITAR INTERRUPCION EN EL PUERTO C
	PCICR |= (1 << PCIE1);

	//LOS PINES A LOS QUE VAMOS A UTILIZAR PARA LA INTERRUPCION

	PCMSK1 |= (1 << PCINT9)|(1 << PCINT10)|(1 << PCINT11); //PC1, PC2, PC3
}

//CONFIGURACION TIMER0 1MS
void setup_TIMER0(){
	//CONFIGURACION TIMER0
	TCCR0A |= (1 << WGM01);	//CTC
	TCCR0B |= (1<<CS01)|(1<<CS00);	//PRESCALER 64
	OCR0A = 249;	//1ms
	TIMSK0 |= (1<<OCIE0A);	//INTERRUPCION POR COMPARACION
}


/////////CONTROL LEDS CONTADORES//////////////
void MUESTRA_LEDS(){
	
	
	uint8_t NUEVO_PORTB = 0x00;
	
	if (C2 == 2)
	{
		NUEVO_PORTB |= (1<<PINB0);
	}
	if(C2 == 3){
		NUEVO_PORTB |= (1<<PINB1);
	}
	if(C2 == 4){
		NUEVO_PORTB |= (1<<PINB2);
		Play_time = 0;
	}
	
	
	if(C1 == 1){
		NUEVO_PORTB |= (1<<PINB3);
	}
	
	if(C1 == 2){
		NUEVO_PORTB |= (1<<PINB4);
	}
	if(C1 == 4){
		NUEVO_PORTB |= (1<<PINB5);
		Play_time = 0;
	}
	
	if(C1 == 3){
		PORTC |= (1<<PINC0);
	}
	else{
		PORTC &= ~(1<<PINC0);
	}
	
	if (C1 == 4 && Play_time == 0)
	{

		PORTC |= (1<<PINC0);
		NUEVO_PORTB = 0x78;
		DISPLAY_VALUE = 2;
		ESTADO_DISPLAY = 0;
	}
	if (C2 == 4 && Play_time ==0)
	{
		PORTC &= ~(1<<PINC0);
		
		NUEVO_PORTB = 0x07;
		DISPLAY_VALUE = 1;
		ESTADO_DISPLAY = 0;
	}
	
	
	
	PORTB = NUEVO_PORTB;
	
}



////////////////MAIN///////////////////////
int main(void){
	setup();
	setup_pinchange();
	setup_TIMER0();
	iniciar_display();
	sei();
	
	
	while(1){

	}
}

///////////////////////////VECTOR INTERRUPCION PIN CHANGE////////////////////
ISR(PCINT1_vect){


//ANTIREBOTE JUGADOR 1
	if((!(PINC&(1<<PINC1)))&&(Play_time == 1)){
			if(PC1ANTI == 0){
			if(C1 < 4)
			{
				C1++;
				MUESTRA_LEDS();
				PC1ANTI = 200;
			}
	}
}

//ANTIREBOTE JUGADOR 2
	if((!(PINC&(1<<PINC2)))&&(Play_time == 1)){
			if(PC2ANTI == 0){
				if (C2 < 4)
				{
					C2++;
					MUESTRA_LEDS();
					PC2ANTI = 200;	
				}
				
	}
}

//PINC3 BOTON START
	if(!(PINC & (1 << PINC3))){
		if(PC3ANTI == 0){
			Play_time = 0;
			ESTADO_DISPLAY = 1;		//CAMBIAR EL ESTADO DEL DISPLAY A "CONTANDO"
			C1 = 0;
			C2 = 0;
			PORTB = 0x00;
			contador_segundos = 0;
			PORTC &= ~(1<<PINC0);
			DISPLAY_VALUE = 5;		//VALOR INICIAL DEL DISPLAY
			PC3ANTI = 200;	//CONTAR 200ms para antirebote
	}
}


}
///////////////////////////VECTOR INTERRUPCION TIMER////////////////////
ISR(TIMER0_COMPA_vect){

	if(PC1ANTI > 0) PC1ANTI--;
	if(PC2ANTI > 0) PC2ANTI--;
	if(PC3ANTI > 0) PC3ANTI--;


	mostrar_display(DISPLAY_VALUE);
	
	if (C2 == 1 || C2 == 4 )
	{
		PORTD |= (1<<PIND7);
		if(C1 == 4){
			PORTD &= ~(1<<PIND7);
		}
	} 
	else
	{
		PORTD &= ~(1<<PIND7);
	}


	switch(ESTADO_DISPLAY){


	//CARRERA NO INICIADA
	case 0:
	
	break;
	//CARRERA INICIADA
	case 1:	
	if (contador_segundos >= 1000){
		contador_segundos = 0;
		if (DISPLAY_VALUE > 0)
		{
			DISPLAY_VALUE--;

		if (DISPLAY_VALUE == 0)
		{
			Play_time = 1;
		}
	}
	}

	else
	{
		contador_segundos++;
	}


break;
}

}
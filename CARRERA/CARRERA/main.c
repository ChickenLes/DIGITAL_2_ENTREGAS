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

///////////////////////VARIABLES_GLOBALES//////////////////////
//volatile uint8_t C1 = 0x00;        //VALOR CONTADOR 1
//volatile uint8_t C2 = 0x00;        //VALOR CONTADOR 2
volatile uint8_t Play_time = 0;      //BANDERA HABILITAR CARRERA

volatile uint8_t PC1ANTI = 0x00;	//ANTIREBOTE BOTON START
volatile uint8_t PC2ANTI = 0x00;	//ANTIREBOTE BOTON P1
volatile uint8_t PC3ANTI = 0x00;	//ANTIREBOTE BOTON P2

volatile uint8_t DISPLAY_VALUE = 0;
volatile uint8_t ESTADO_DISPLAY = 0; //0: OFF, 1: ON

volatile uint16_t contador_segundos = 0x00; //VARIABLE PARA CONTAR LOS SEGUNDOS 1000MS = 1S

//TABLA DEL DISPLAY
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
	0x71  //F
};

void setup(){
	
	//OUTPUTS
	DDRD  = 0xFF;//PORTD OUTPUT
	PORTD = 0x00;
	DDRB  = 0xFF;//PORTB OUTPUT
	PORTB = 0x00;
	DDRC  |= (1 << PINC0); //PINC0 OUTPUT
	PORTC &= ~(1 << PINC0);
	
	//INPUT
	DDRC  &= ~((1 << PINC1)|(1 << PINC2)|(1 << PINC3)); //PINC1, PINC2 Y PINC3 COMO INPUTS
	PORTC |= (1 << PINC1)|(1 << PINC2)|(1 << PINC3) ;    //PULL UP ACTIVADO

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
	TCCR0A |= (1 << WGM01);            //CTC
	TCCR0B |= (1<<CS01)|(1<<CS00);    //PRESCALER 64
	OCR0A = 249;                    //1ms
	TIMSK0 |= (1<<OCIE0A);            //INTERRUPCION POR COMPARACION
}



////////////////MAIN///////////////////////
int main(void){
	setup();
	setup_pinchange();
	setup_TIMER0();
	sei();
	
	while(1){
		
	}
}

///////////////////////////VECTOR INTERRUPCION PIN CHANGE////////////////////
ISR(PCINT1_vect){
	
	
	//ANTIREBOTE JUGADOR 1
	if((!(PINC&(1<<PINC1)))&&(Play_time == 1)){
		if(PC1ANTI == 0){
			PORTB = 0x01;
			PC1ANTI = 200;
		}
	}
	
	//ANTIREBOTE JUGADOR 2
	if((!(PINC&(1<<PINC2)))&&(Play_time == 1)){
		if(PC2ANTI == 0){
			PORTB = 0x90;
			PC2ANTI = 200;
		}
	}
	
	//PINC3 BOTON START
	if(!(PINC & (1 << PINC3))){
		if(PC3ANTI == 0){
			ESTADO_DISPLAY = 1; //CAMBIAR EL ESTADO DEL DISPLAY A "CONTANDO"
			DISPLAY_VALUE = 9; //VALOR INICIAL DEL DISPLAY
			PC3ANTI = 200;        //CONTAR 200ms para antirebote
		}
	}
	
	
}
///////////////////////////VECTOR INTERRUPCION TIMER////////////////////
ISR(TIMER0_COMPA_vect){
	
	if(PC1ANTI > 0) PC1ANTI--;
	if(PC2ANTI > 0) PC2ANTI--;
	if(PC3ANTI > 0) PC3ANTI--;
	
	switch(ESTADO_DISPLAY){
		
		//CARRERA NO INICIADA
		case 0:
		PORTD = TABLA[9];  //VALOR DE INICIO AL ENCENDER TODO
		
		break;
		//CARRERA INICIADA
		case 1:
		PORTD = TABLA[DISPLAY_VALUE];
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

/*
 * ARDUINO_LAB_8.c
 *
 * Created: 3/23/2026 9:50:10 AM
 * Author : itzle
 */ 

#define F_CPU 16000000UL
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>

volatile uint8_t boton_presionado = 0;
volatile char caracter_a_enviar = 0;

//CHAT GPT ME AYUDÓ A CAMBIAR EL BAUDRATE A 115200
void setup_UART(void) {
	// Configurar velocidad doble (U2X0 = 1)
	UCSR0A |= (1 << U2X0);
	UBRR0H = 0;
	// 16 equivale a 115200 baudios con F_CPU a 16MHz y U2X0 en 1
	UBRR0L = 16;
	
	// Habilitar transmisor
	UCSR0B = (1 << TXEN0);
	// Formato: 8 bits de datos, 1 bit de parada
	UCSR0C = (1 << UCSZ01) | (1 << UCSZ00);
}

void UART_Transmit(char data) {
	while (!(UCSR0A & (1 << UDRE0)));
	UDR0 = data;
}

void setup_pinchange(void) {
	
	//CONFIGURACION
	DDRC &= ~((1 << PINC0) | (1 << PINC1) | (1 << PINC2) | (1 << PINC3) | (1 << PINC4) | (1 << PINC5));
	
	//PULL UP
	PORTC |= ((1 << PINC0) | (1 << PINC1) | (1 << PINC2) | (1 << PINC3) | (1 << PINC4) | (1 << PINC5));

	//SEI
	PCICR |= (1 << PCIE1);
	
	//MASCARA PUERTO
	PCMSK1 |= ((1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10) | (1 << PCINT11) | (1 << PCINT12) | (1 << PCINT13));
}

int main(void) {
	setup_UART();
	setup_pinchange();
	sei(); 

	while(1) {
		if (boton_presionado) {
			UART_Transmit(caracter_a_enviar);
			
			_delay_ms(200);
			boton_presionado = 0;
		}
	}
}


ISR(PCINT1_vect) {

	uint8_t botones = ~PINC & 0x3F;

	if (botones == 0) 


	switch (botones) {
		case (1 << PINC0): 
		caracter_a_enviar = 'U'; 
		break; //UP
		
		case (1 << PINC1):
		 caracter_a_enviar = 'D';
		  break; //DOWN
		  
		case (1 << PINC2): 
		caracter_a_enviar = 'R'; 
		break; //RIGHT
		
		case (1 << PINC3): 
		caracter_a_enviar = 'L'; 
		break; //LEFT
		
		case (1 << PINC4): 
		caracter_a_enviar = 'B'; 
		break; //B
		
		case (1 << PINC5): 
		caracter_a_enviar = 'A'; 
		break; //A
		
		default: 
		return;
		
	}

	boton_presionado = 1;
}

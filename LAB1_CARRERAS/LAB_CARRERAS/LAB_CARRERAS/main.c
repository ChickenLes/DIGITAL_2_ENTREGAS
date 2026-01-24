//-----------------------------------------------
// UNIVERSIDAD DEL VALLE DE GUATEMALA
// PROYECTO: PRELABORATORIO 1 - JUEGO DE CARRERAS (FIXED)
//-----------------------------------------------

#define F_CPU 16000000
#include <avr/io.h>
#include <util/delay.h>
#include <avr/interrupt.h>
#include <stdbool.h>

///////////////////////VARIABLES_GLOBALES//////////////////////
volatile uint8_t C1 = 0;        // Contador Jugador 1 (0 a 4)
volatile uint8_t C2 = 0;        // Contador Jugador 2 (0 a 4)
volatile bool Play_time = 0;    // BANDERA HABILITAR CARRERA

// ANTIREBOTES INDIVIDUALES
volatile uint16_t PC1ANTI = 0;   // JUGADOR 1
volatile uint16_t PC2ANTI = 0;   // JUGADOR 2
volatile uint16_t PC3ANTI = 0;   // START

volatile uint8_t DISPLAY_VALUE = 0;
volatile uint8_t ESTADO_DISPLAY = 0; // 0: OFF/ESPERA, 1: CUENTA REGRESIVA
volatile uint16_t contador_segundos = 0;

// TU TABLA PERSONALIZADA (Bits 0-6 usados, Bit 7 LIBRE para LED)
const uint8_t TABLA[] = {
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
	0x7D, //A
	0x67, //b
	0x33, //C
	0x4F, //d
	0x73, //E
	0x71  //F
};

// --- FUNCIÓN AUXILIAR PARA LEDS EN PORTB Y PORTC ---
// Esta función se llama cada vez que alguien gana un punto.
// Maneja: PB0, PB1, PB2 (Jugador 1)
// Maneja: PB3, PB4, PB5, PC0 (Jugador 2)
void actualizar_leds_restantes() {
	
	// --- LIMPIAR PORTB ---
	// Dejamos en 0 para volver a calcular
	uint8_t nuevo_portb = 0x00;

	// --- JUGADOR 1 (PB0, PB1, PB2) ---
	// Recordamos que el punto 1 está en PD7. Aquí vemos del 2 al 4.
	if (C1 >= 2) nuevo_portb |= (1 << PB0); // 2do punto
	if (C1 >= 3) nuevo_portb |= (1 << PB1); // 3er punto
	if (C1 >= 4) nuevo_portb |= (1 << PB2); // 4to punto

	// --- JUGADOR 2 (PB3, PB4, PB5) ---
	// Puntos 1, 2 y 3 están en PORTB
	if (C2 >= 1) nuevo_portb |= (1 << PB3); // 1er punto
	if (C2 >= 2) nuevo_portb |= (1 << PB4); // 2do punto
	if (C2 >= 3) nuevo_portb |= (1 << PB5); // 3er punto

	PORTB = nuevo_portb; // Escribimos al puerto B

	// --- JUGADOR 2 (PC0) ---
	// El 4to punto está en el puerto C
	if (C2 >= 4) {
		PORTC |= (1 << PINC0); // Encender PC0
		} else {
		PORTC &= ~(1 << PINC0); // Apagar PC0
	}
}

void setup(){
	// OUTPUTS
	DDRD  = 0xFF;    // PORTD Salida (Display + LED PD7)
	PORTD = 0x00;
	
	DDRB  = 0xFF;    // PORTB Salida (LEDs J1 y J2)
	PORTB = 0x00;

	// PORTC: PC0 Salida (LED J2), PC1-PC3 Entradas (Botones)
	DDRC  |= (1 << PINC0);
	PORTC &= ~(1 << PINC0); // PC0 apagado

	DDRC  &= ~((1 << PINC1)|(1 << PINC2)|(1 << PINC3)); // Inputs
	PORTC |= (1 << PINC1)|(1 << PINC2)|(1 << PINC3);    // Pull-ups
}

void setup_pinchange(){
	PCICR |= (1 << PCIE1);
	PCMSK1 |= (1 << PCINT9)|(1 << PCINT10)|(1 << PCINT11);
}

void setup_TIMER0(){
	TCCR0A |= (1 << WGM01);         // CTC
	TCCR0B |= (1<<CS01)|(1<<CS00);  // Prescaler 64
	OCR0A = 249;                    // 1ms
	TIMSK0 |= (1<<OCIE0A);
}

int main(void){
	setup();
	setup_pinchange();
	setup_TIMER0();
	sei();
	
	while(1){
	}
}

/////////////////////////// INTERRUPCION BOTONES ////////////////////
ISR(PCINT1_vect){
	
	// --- JUGADOR 1 (PIN A1) ---
	if( (!(PINC & (1<<PINC1))) && (Play_time == 1) ){
		if(PC1ANTI == 0){
			if(C1 < 4) C1++;    // Sumamos punto (max 4)
			actualizar_leds_restantes(); // Prender luces B y C
			PC1ANTI = 500;      // Antirebote
		}
	}
	
	// --- JUGADOR 2 (PIN A2) ---
	if( (!(PINC & (1<<PINC2))) && (Play_time == 1) ){
		if(PC2ANTI == 0){
			if(C2 < 4) C2++;    // Sumamos punto (max 4)
			actualizar_leds_restantes(); // Prender luces B y C
			PC2ANTI = 500;
		}
	}
	
	// --- BOTON START (PIN A3) ---
	if(!(PINC & (1 << PINC3))){
		if(PC3ANTI == 0){
			// RESET TOTAL DEL JUEGO
			ESTADO_DISPLAY = 1;
			DISPLAY_VALUE = 9;
			Play_time = 0;
			
			C1 = 0; // Reset Jugador 1
			C2 = 0; // Reset Jugador 2
			actualizar_leds_restantes(); // Apagar todos los LEDs B y C
			
			contador_segundos = 0;
			PC3ANTI = 500;
		}
	}
}

/////////////////////////// INTERRUPCION TIMER ////////////////////
ISR(TIMER0_COMPA_vect){
	// Antirebotes
	if(PC1ANTI > 0) PC1ANTI--;
	if(PC2ANTI > 0) PC2ANTI--;
	if(PC3ANTI > 0) PC3ANTI--;
	
	// --- AQUÍ ESTÁ LA MAGIA DEL DISPLAY + LED PD7 ---
	// 1. Tomamos el dibujo del número actual
	uint8_t valor_puerto = TABLA[DISPLAY_VALUE];

	// 2. Si el JUGADOR 1 tiene al menos 1 punto, encendemos PD7
	//    usando OR bitwise para no borrar el número.
	if (C1 >= 1) {
		valor_puerto |= (1 << 7); // Prender PD7 (Bit 7)
	}
	
	// 3. Enviamos la mezcla al puerto
	PORTD = valor_puerto;
	// ------------------------------------------------

	switch(ESTADO_DISPLAY){
		case 0: // ESPERA (Muestra el número fijo, con o sin el LED PD7)
		break;

		case 1: // CUENTA REGRESIVA
		if (contador_segundos >= 1000){
			contador_segundos = 0;
			if (DISPLAY_VALUE > 0){
				DISPLAY_VALUE--;
				if (DISPLAY_VALUE == 0){
					Play_time = 1; // ¡A JUGAR!
				}
			}
			} else {
			contador_segundos++;
		}
		break;
	}
}
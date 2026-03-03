/*
 * DISPLAY_7SEG.c
 *
 *  Created on: Mar 3, 2026
 *      Author: itzle
 */


#include "DISPLAY_7SEG.h"


const uint8_t TABLA[] = {
    0x3F,
	0x0C,
	0x5B,
	0x5E,
	0x6C,
	0x76,
	0x77,
	0x1C,
	0x7F,
	0x7E, // 0-9
    0x7D,
	0x67,
	0x33,
	0x4F,
	0x73,
	0x71 // A-F
};

void iniciar_display(void) {
    // Apaga todos los segmentos al iniciar usando tus etiquetas de main.h
    HAL_GPIO_WritePin(GPIOA, DISPLAY_1_Pin|DISPLAY_2_Pin|DISPLAY_3_Pin|DISPLAY_4_Pin|
                             DISPLAY_5_Pin|DISPLAY_6_Pin|DISPLAY_7_Pin, GPIO_PIN_RESET);
}

void mostrar_display(uint8_t numero) {
    numero &= 0x0F; // Seguridad (0-15)
    uint8_t dibujo = TABLA[numero];

    // Mapeo bit a bit usando tus nombres exactos de main.h
    HAL_GPIO_WritePin(GPIOA, DISPLAY_1_Pin, (dibujo & (1 << 0)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_2_Pin, (dibujo & (1 << 1)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_3_Pin, (dibujo & (1 << 2)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_4_Pin, (dibujo & (1 << 3)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_5_Pin, (dibujo & (1 << 4)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_6_Pin, (dibujo & (1 << 5)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
    HAL_GPIO_WritePin(GPIOA, DISPLAY_7_Pin, (dibujo & (1 << 6)) ? GPIO_PIN_SET : GPIO_PIN_RESET);
}

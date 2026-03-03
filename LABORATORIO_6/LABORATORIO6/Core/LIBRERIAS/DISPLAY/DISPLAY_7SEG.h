/*
 * DISPLAY_7SEG.h
 *
 *  Created on: Mar 3, 2026
 *      Author: itzle
 */

#ifndef LIBRERIAS_DISPLAY_DISPLAY_7SEG_H_
#define LIBRERIAS_DISPLAY_DISPLAY_7SEG_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include "main.h"




void iniciar_display(void);
void mostrar_display(uint8_t numero);


#endif /* LIBRERIAS_DISPLAY_DISPLAY_7SEG_H_ */

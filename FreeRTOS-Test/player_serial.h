/*
 * player_serial.h
 *
 * Created: 13/12/2016 12:43:29
 *  Author: David
 */ 


#ifndef PLAYER_SERIAL_H_
#define PLAYER_SERIAL_H_

#include "game.h"

#include <FreeRTOS.h>
#include <semphr.h>

void *p_serial();
void *init_p_serial(QueueHandle_t *que, SemaphoreHandle_t *scr_mtx,SemaphoreHandle_t *pl_mtx, uint8_t *p_pos, uint16_t *scr_buff);


#endif /* PLAYER_SERIAL_H_ */
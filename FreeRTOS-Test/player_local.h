/*
 * player_local.h
 *
 * Created: 2016. 12. 12. 23:22:29
 *  Author: David
 */ 


#ifndef PLAYER_LOCAL_H_
#define PLAYER_LOCAL_H_

#include "game.h"

#include <FreeRTOS.h>
#include <semphr.h>

void *p_local();
void *init_p_local(SemaphoreHandle_t *m, uint8_t *p, uint16_t s);

#endif /* PLAYER_LOCAL_H_ */
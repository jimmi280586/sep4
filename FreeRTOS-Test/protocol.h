/*
 * protocol.h
 *
 * Created: 2016. 12. 12. 18:30:22
 *  Author: David
 */ 


#ifndef PROTOCOL_H_
#define PROTOCOL_H_

// FfreeRTOS Includes
#include <FreeRTOS.h>
#include <queue.h>


typedef void *(*prot_StateFunc)(uint8_t);

typedef struct frame_t{
	uint8_t bytes[4];	//max 4 byte long frames
	uint8_t size;
}frame_t;

void *prot_idle(uint8_t byte);
void *prot_data(uint8_t byte);
void *prot_esc(uint8_t byte);

void *init_protocol(QueueHandle_t *q);



#endif /* PROTOCOL_H_ */
/*
* $safeprojectname$.c
*
* Created: 26/10/2016 13:55:41
* Author : IHA
*/

#include <avr/sfr_defs.h>
#include <avr/io.h>
// FfreeRTOS Includes
#include <FreeRTOS.h>
#include <task.h>
#include <timers.h>
#include <queue.h>

#include "src/board/board.h"

#define task1_prio (tskIDLE_PRIORITY+4)
#define task2_prio (tskIDLE_PRIORITY+3)
#define task2_prio (tskIDLE_PRIORITY+2)

static const uint8_t _COM_RX_QUEUE_LENGTH = 30;
static QueueHandle_t _x_com_received_chars_queue = NULL;
uint16_t col_value[14] = {48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48};
//uint16_t col_value[14] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};
uint8_t col_index = 0;

//-----------------------------------------
void startup_task(void *pvParameters)
{
	// The parameters are not used
	( void ) pvParameters;

	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	#endif
	
	
	
	// Initialization of tasks etc. can be done here
	
	// Lets send a start message to the console
	com_send_bytes((uint8_t *)"Then we Start!\n", 15);
	com_send_bytes((uint8_t *) 0xff, 1);
	
	while(1)
	{
		// Maybe something usefiúll could be done her :)
	}
}
void echo_task(void *pvParameters)
{
	(void) pvParameters;

	uint8_t data;
	
	while(1)
	{
		if(_x_com_received_chars_queue != 0)
		{
			xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 10);
			//com_send_bytes(&(data), 1);

			if (data == 0x61)
			{
			
			xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 10);
			
			while(data != 0x61)
			{
				if (data == 0x41){
					xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 10);
				}
				com_send_bytes(&(data), 1);
				if (data == 0x62){
					col_value[0] >>= 1;
				}
				xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 10);
			}
			}
		}	
	}
	
}

void game_task(void *pvParameters)
{
	(void) pvParameters;

	
	
	
	while(1)
	{
		if (!(PINC & (1<<6))){
			if ( col_value[0] > 3){
				col_value[0] >>= 1;
			}
			
		}
		if (!(PINC & (1<<0))){
			if ( col_value[0] < 768){
				col_value[0] <<= 1;
			}
			
		}
		vTaskDelay(60);
	}
	
}

void ball_task(void *pvParameters)
{
	(void) pvParameters;

	uint8_t pos[2] = {7, 5};
	
	
	while(1)
	{
		if (!(PINC & (1<<6))){
			if ( col_value[0] > 3){
				col_value[0] >>= 1;
			}
			
		}
		if (!(PINC & (1<<0))){
			if ( col_value[0] < 768){
				col_value[0] <<= 1;
			}
			
		}
		vTaskDelay(60);
	}
	
}


//-----------------------------------------
void handle_display(void)
{
	if (col_index == 0){
		PORTD |= _BV(PORTD2);
	}

	// one SCK pulse
	PORTD |= _BV(PORTD5);
	PORTD &= ~_BV(PORTD5);

	// one RCK pulse
	PORTD |= _BV(PORTD4);
	PORTD &= ~_BV(PORTD4);

	PORTA = ~(col_value[col_index] & 0xFF);
	
	// Manipulate only with PB0 and PB1
	PORTB |= 0x03;
	PORTB &= ~((col_value[col_index] >> 8) & 0x03);

	// Set SER to 0
	PORTD &= ~_BV(PORTD2);

	++col_index;
	if (col_index > 13){
		col_index = 0;
	}

}

//-----------------------------------------
void vApplicationIdleHook( void )
{
	//
}

//-----------------------------------------
int main(void)
{
	
	init_board();
	
	// Shift register Enable output (G=0)
	PORTD &= ~_BV(PORTD6);

	_x_com_received_chars_queue = xQueueCreate( _COM_RX_QUEUE_LENGTH, ( unsigned portBASE_TYPE ) sizeof( uint8_t ) );
	init_com(_x_com_received_chars_queue);

	
	
	//Create task to blink gpio
	//xTaskCreate(startup_task, (const char *)"Startup", configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(echo_task,(const char *)"echo", configMINIMAL_STACK_SIZE, (void *)NULL, task1_prio, NULL);
	xTaskCreate(game_task,(const char *)"game", configMINIMAL_STACK_SIZE, (void *)NULL, task2_prio, NULL);
	
	// Start the display handler timer
	init_display_timer(handle_display);
	
	//Start the scheduler
	vTaskStartScheduler();
	
	//Should never reach here
	while (1)
	{
	}
}



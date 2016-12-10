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
#include <semphr.h>

#include "src/board/board.h"

#define task1_prio (tskIDLE_PRIORITY+4)
#define task2_prio (tskIDLE_PRIORITY+3)
#define task3_prio (tskIDLE_PRIORITY+2)

static const uint8_t _COM_RX_QUEUE_LENGTH = 30;
static QueueHandle_t _x_com_received_chars_queue = NULL;
static SemaphoreHandle_t _col_0_mutex = NULL;
static SemaphoreHandle_t _player_position_mutex = NULL;
static SemaphoreHandle_t _ball_position_mutex = NULL;

static player_position = 4;
static uint16_t col_value[14] = {48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48}; //display
//uint16_t col_value[14] = {1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023, 1023};
static col_index = 0;

typedef struct {
	uint8_t data;
	struct msg_byte *next;
}msg_byte_t;

typedef struct msg{
	uint8_t *data;
	uint8_t size;
}msg_t;

static msg_t *frame = NULL;

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

void serial_task(void *pvParameters){

	(void) pvParameters;
	
	TickType_t lastWakeTime;
	
	uint8_t state = 0;
	uint8_t data = 0;
	const uint8_t flag = 0x61;
	const uint8_t esc = 0xff;

	msg_byte_t *received_byte = NULL;
	msg_byte_t *head;
	uint8_t count = 0;
	lastWakeTime = xTaskGetTickCount();
	while (1)
	{
		while(xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 0) ){
			
			switch (state){
				case 0:
					if (data == flag){
						state = 1;
					}
					break;
				case 1:
					if (data == flag){
						msg_t *received_msg;
						received_msg = (sizeof(msg_t));
						uint8_t bytes[count];
						

						for (uint8_t i; i < count; ++i)
						{
							bytes[i] = head->data;
							head = head->next;
						}
						received_msg->data = &bytes;
						count = 0;
						frame = &received_msg;

						state = 0;
					}
					else if (data == esc){
						state = 2;
					}
					else{
						msg_byte_t *incoming;
						incoming = malloc(sizeof(msg_byte_t));
						incoming->data =data;
						if (count != 0){
							received_byte->next = &incoming;
						}
						else
						{
							head = &incoming;
						}
						++count;
						received_byte = &incoming;
					}
					break;
				case 2:
					xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 0);
					//save to payload
					break;
				default:
					state = 0;
					break;
			}
			/*
			if (data == 0x61)
			{
				xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 0);
				if (data == 0x41){
					xQueueReceive(_x_com_received_chars_queue, &(data),(TickType_t) 0);
				}
				com_send_bytes(&(data), 1);  //com_send_bytes
				if (data == 0x62){
					col_value[0] >>= 1;
				}
			}*/
		}
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 20);
	}

}

void echo_task(void *pvParameters)
{
	(void) pvParameters;

	uint8_t data;
	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if (frame != NULL)
		{
		com_send_bytes((uint8_t *) "hello", 5);
		com_send_bytes(frame->data,frame->size);
		frame = NULL;
		}
		
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 40);
	}
	
}

void move_player(uint8_t *position, uint8_t direction){
	uint16_t mask = 5;
	if (direction == 0){
		mask <<= *position;
	}
	else{
		mask <<= *position-1;
	}
	
	xSemaphoreTake(_col_0_mutex, (TickType_t) 10);
	col_value[0] ^= mask;
	xSemaphoreGive(_col_0_mutex);
}

void local_player_task(void *pvParameters)
{
	(void) pvParameters;
	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	while (1)
	{
		//xSemaphoreTake(_player_position_mutex, (TickType_t) 2);
		if (!(PINC & (1<<6)) && player_position > 0){
			xSemaphoreTake(_player_position_mutex, (TickType_t) 10);
			--player_position;
			xSemaphoreGive(_player_position_mutex);
			move_player(&player_position, 0);
		}
		else if (!(PINC & (1<<0)) && player_position < 8){
			xSemaphoreTake(_player_position_mutex, (TickType_t) 10);
			++player_position;
			xSemaphoreGive(_player_position_mutex);
			move_player(&player_position, 1);
		}
		//xSemaphoreGive(_player_position_mutex);
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 40);
	}	
	
}

/*
void bounce(uint8_t *direction){
	uint8_t r = rand()%3;
	
	r += 3;
	r += *direction;
	r %= 8;
	*direction = r;
}
*/

void bounce(uint8_t *direction, uint8_t side){

	if ( (*(direction)%2)==0){
		uint8_t r = rand()%3;
		r += 3;
		r += *direction;
		r %= 8;
		*direction = r;
	}
	else{
		if (side == 0){
			switch (*direction){
				case 1:
				*direction = 3;
				break;
				case 3:
				*direction = 1;
				break;
				case 5:
				*direction = 7;
				break;
				case 7:
				*direction = 5;
				break;
			}
		}
		else{
			switch (*direction){
				case 1:
				*direction = 7;
				break;
				case 3:
				*direction = 5;
				break;
				case 5:
				*direction = 3;
				break;
				case 7:
				*direction = 1;
				break;
			}
		}
	}
	
	
}

void move_ball(uint8_t *current, uint8_t *next){
	uint16_t mask = 1<< current[1];
	mask = ~mask;

	if (current[0] == 0){
		xSemaphoreTake(_col_0_mutex, (TickType_t) 10);
		col_value[current[0]] &= mask;
		xSemaphoreGive(_col_0_mutex);
	}
	else{
		col_value[current[0]] &= mask;
	}
	//col_value[current[0]] &= mask;
	
	current[0] = next[0];
	current[1] = next[1];
	
	mask = 1 << current[1];
	if (current[0] == 0){
		xSemaphoreTake(_col_0_mutex, (TickType_t) 10);
		col_value[current[0]] |= mask;
		xSemaphoreGive(_col_0_mutex);
	}
	else{
		col_value[current[0]] |= mask;
	}

	
}

void calc_next(uint8_t *current, uint8_t *next, uint8_t *direction){
	
	switch (*direction)
	{
		case 0:
			--next[1];
			break;
		case 1:
		++next[0];
		--next[1];
		break;
		case 2:
		++next[0];
		break;
		case 3:
		++next[0];
		++next[1];
		break;
		case 4:
		++next[1];
		break;
		case 5:
		--next[0];
		++next[1];
		break;
		case 6:
		--next[0];
		break;
		case 7:
		--next[0];
		--next[1];
		break;
	}
	
}

void ball_task(void *pvParameters)
{
	(void) pvParameters;
	TickType_t lastWakeTime;
	uint8_t pos[2] = {7, 5};
	uint8_t direction = 0;
	lastWakeTime = xTaskGetTickCount();
	while(1)
	{

		uint8_t next[2] = {pos[0],pos[1]};
		uint8_t is_bounced = 1;

		while (is_bounced != 0)
		{	
			is_bounced = 0;
			calc_next( &pos, &next, &direction);
			
			if (next[0] > 12){
				bounce(&direction, 1);
			}
			else if ( next[1] > 9){
				bounce(&direction, 0);
			}
			else if (next[0] == 0 && (next[1] == player_position || next[1] == (player_position+1))){
				bounce(&direction, 1);
			}
			else{
				move_ball(pos, next);
				is_bounced = 0;
			}
		}
		
<<<<<<< HEAD
		vTaskDelay(20);
=======
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 80);
>>>>>>> origin/master
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
	_col_0_mutex = xSemaphoreCreateMutex();
	_player_position_mutex = xSemaphoreCreateMutex();
	_ball_position_mutex = xSemaphoreCreateMutex();
	init_com(_x_com_received_chars_queue);

	
	
	//Create task to blink gpio
	//xTaskCreate(startup_task, (const char *)"Startup", configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(serial_task,(const char *)"serial", configMINIMAL_STACK_SIZE, (void *)NULL, task1_prio, NULL);
	xTaskCreate(ball_task,(const char *)"ball", configMINIMAL_STACK_SIZE, (void *)NULL, task2_prio, NULL);
	xTaskCreate(local_player_task,(const char *)"lplayer", configMINIMAL_STACK_SIZE, (void *)NULL, task3_prio, NULL);
	xTaskCreate(echo_task,(const char *)"echo", configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	
	
	// Start the display handler timer
	init_display_timer(handle_display);
	
	//Start the scheduler
	vTaskStartScheduler();
	
	//Should never reach here
	while (1)
	{
	}
}



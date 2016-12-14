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
#include "protocol.h"
#include "game.h"
#include "player_local.h"

#define task1_prio (tskIDLE_PRIORITY+4)
#define task2_prio (tskIDLE_PRIORITY+3)
#define task3_prio (tskIDLE_PRIORITY+2)
#define task4_prio (tskIDLE_PRIORITY+1)

static const uint8_t _COM_RX_QUEUE_LENGTH = 30;
static QueueHandle_t _x_com_received_chars_queue = NULL;

static SemaphoreHandle_t _screen_mutex = NULL;
static SemaphoreHandle_t _local_p_mutex = NULL;
static SemaphoreHandle_t _serial_p_mutex = NULL;

static l_player = 4;
static s_player = 4;
static ball_position[2] = {7, 5};
static uint16_t col_value[14] = {48, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 48}; //display
static col_index = 0;

static QueueHandle_t _frames_received = NULL;

//-----------------------------------------
/*
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
		// Maybe something usefull could be done her :)
	}
}
*/

void serial_task(void *pvParameters){

	(void) pvParameters;

	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 4 );
	#endif
	
	TickType_t lastWakeTime;
	uint8_t byte = 0;
	prot_StateFunc state = init_protocol(&_frames_received);

	lastWakeTime = xTaskGetTickCount();
	while(1){
		while(xQueueReceive(_x_com_received_chars_queue,&byte, (TickType_t) 0)){
			state = (prot_StateFunc)(*state)(byte);
		}
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 20);
	}

}


void echo_task(void *pvParameters)
{
	(void) pvParameters;

	frame_t buff;
	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	while(1)
	{
		if (xQueueReceive(_frames_received, &buff, (TickType_t) 0))
		{
		//com_send_bytes((uint8_t *) "hello", 5);
		com_send_bytes(buff.bytes,buff.size);
		}
		
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 40);
	}
	
}

void local_player_task(void *pvParameters)
{
	(void) pvParameters;

	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 2 );
	#endif

	TickType_t lastWakeTime;

	void *(*loop)() = init_p_local(&_screen_mutex, &_local_p_mutex, &l_player, &col_value);

	lastWakeTime = xTaskGetTickCount();
	while(1){
		loop = (*loop)();
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 60);
	}
	
}
/*
void move_player2(uint8_t *position, uint8_t direction){
	uint16_t mask = 5;
	if (direction == 0){
		mask <<= *position;
	}
	else{
		mask <<= *position-1;
	}
	
	xSemaphoreTake(_col_13_mutex, (TickType_t) 1);
	col_value[13] ^= mask;
	xSemaphoreGive(_col_13_mutex);
}
*/
void serial_player_task(void *pvParameters)
{
	(void) pvParameters;

	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 1 );
	#endif
	
	void *(*loop)() = init_p_serial(&_frames_received, &_screen_mutex, &_serial_p_mutex, &s_player, &col_value);

	TickType_t lastWakeTime;
	lastWakeTime = xTaskGetTickCount();
	while (1)
	{
		loop = (*loop)();
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 60);
	}
	
}

void game_task(void *pvParameters)
{
	(void) pvParameters;

	#if (configUSE_APPLICATION_TASK_TAG == 1)
	// Set task no to be used for tracing with R2R-Network
	vTaskSetApplicationTaskTag( NULL, ( void * ) 3 );
	#endif

	TickType_t lastWakeTime;
	game_stateFunc state = init_game(&col_value, &_screen_mutex, &l_player, &_local_p_mutex, &s_player, &_serial_p_mutex);

	lastWakeTime = xTaskGetTickCount();

	while(1){
		state = (game_stateFunc)(*state)();
		vTaskDelayUntil(&lastWakeTime, (TickType_t) 140);
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
	_frames_received = xQueueCreate( 1, ( unsigned portBASE_TYPE ) sizeof( frame_t ) );
	//_isBallAtLeft = xSemaphoreCreateBinary();

	_screen_mutex = xSemaphoreCreateMutex();
	//_col_13_mutex = xSemaphoreCreateMutex();
	_local_p_mutex = xSemaphoreCreateMutex();
	_serial_p_mutex = xSemaphoreCreateMutex();
	//_ball_position_mutex = xSemaphoreCreateMutex();
	init_com(_x_com_received_chars_queue);
	
	
	
	//Create task to blink gpio
	//xTaskCreate(startup_task, (const char *)"Startup", configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	xTaskCreate(serial_task,(const char *)"serial", configMINIMAL_STACK_SIZE, (void *)NULL, task1_prio, NULL);
	xTaskCreate(game_task,(const char *)"game", configMINIMAL_STACK_SIZE, (void *)NULL, task2_prio, NULL);
	xTaskCreate(local_player_task,(const char *)"lplayer", configMINIMAL_STACK_SIZE, (void *)NULL, task3_prio, NULL);
	xTaskCreate(serial_player_task,(const char *)"splayer", configMINIMAL_STACK_SIZE, (void *)NULL, task4_prio, NULL);
	//xTaskCreate(echo_task,(const char *)"echo", configMINIMAL_STACK_SIZE, (void *)NULL, tskIDLE_PRIORITY, NULL);
	
	// Start the display handler timer
	init_display_timer(handle_display);
	
	//Start the scheduler
	vTaskStartScheduler();
	
	//Should never reach here
	while (1)
	{
	}
}



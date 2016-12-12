/*
 * player_local.c
 *
 * Created: 2016. 12. 12. 20:44:57
 *  Author: David
 */ 

 #include "player_local.h"

 //static uint16_t *screen_buffer;
 static uint8_t *p_l_pos;
 static SemaphoreHandle_t *mut;

 void *p_local(){
	 if (joy_UP){
		p_up();
	 }
	 else if (joy_DOWN)
	 {
		p_down();
	 }
 } 

 void *init_p_local(SemaphoreHandle_t *m, uint8_t *p, uint16_t s){
	
	p_l_pos = p;
	mut = m;
	//screen_buffer = s;
	return p_local;
 }

 void p_up(){
	if (xSemaphoreTake(*mut, (TickType_t) 1) == pdTRUE)
	{
		if (*p_l_pos > 0){
			--(*p_l_pos);
		}
		xSemaphoreGive(*mut);
	}
	else{
	}
 }

 void p_down(){
	 if (xSemaphoreTake(*mut, (TickType_t) 1) == pdTRUE)
	 {
		 if (*p_l_pos < 8){
			 ++(*p_l_pos);
		 }
		 xSemaphoreGive(*mut);
	 }
	 else{
	 }
 }

 void refresh_p_l(){
	
	//screen_buffer[0] &= !(11 << *p_l_pos);
	//screen_buffer[ball_curr_pos[0]] = 0;

	//screen_buffer[0] |= (11 << *p_l_pos);
	//screen_buffer[ball_curr_pos[0]] = 1023;
 }


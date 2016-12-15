/*
 * player_local.c
 *
 * Created: 2016. 12. 12. 20:44:57
 *  Author: David
 */ 

 #include "player_local.h"

 static uint8_t *pl_pos;
 static uint8_t local_pos = 4;
 static SemaphoreHandle_t *pl_mutex;
 static SemaphoreHandle_t *screen_mutex;
 static uint16_t *screen_buffer;
 static BaseType_t is_blocked = 0;

  void *p_idle(){
	  if (xSemaphoreTake(*screen_mutex, (TickType_t) 0))
	  {
		  xSemaphoreGive(*screen_mutex);
		  is_blocked = 0;
		  return p_local;
	  }
	  return p_idle;
  }

 void *p_local(){
	if (is_blocked)
	{
		return p_idle;
	}
	 if (joy_UP){
		clr_pl();
		p_up();
		draw_pl();
	 }
	 else if (joy_DOWN)
	 {
		clr_pl();
		p_down();
		draw_pl();
	 }

	 return p_local;
 }

 void *init_p_local(SemaphoreHandle_t *scr_mtx,SemaphoreHandle_t *pl_mtx, uint8_t *p_pos, uint16_t *scr_buff){
	
	screen_mutex = scr_mtx;
	pl_pos = p_pos;
	pl_mutex = pl_mtx;
	screen_buffer = scr_buff;
	return p_idle;
 }

 void p_up(){
	if (xSemaphoreTake(*pl_mutex, (TickType_t) 2) == pdTRUE)
	{
		if (*pl_pos > 0){
			--(*pl_pos);
			local_pos = *pl_pos;
		}
		xSemaphoreGive(*pl_mutex);
	}
	else{
	}
 }

 void p_down(){
	 if (xSemaphoreTake(*pl_mutex, (TickType_t) 2) == pdTRUE)
	 {
		 if (*pl_pos < 8){
			 ++(*pl_pos);
			 local_pos = *pl_pos;
		 }
		 xSemaphoreGive(*pl_mutex);
	 }
	 else{
	 }
 }

 void clr_pl(){
	uint16_t mask = 0x0003;
	mask <<= local_pos;
	mask = ~mask;
	if (xSemaphoreTake(*screen_mutex, (TickType_t) 10) == pdTRUE)
	{
		screen_buffer[0] &= mask;
		//screen_buffer[ball_curr_pos[0]] = 0;
		xSemaphoreGive(*screen_mutex);
	}
	else{
		//blocked by game idle
		is_blocked = 1;
	}
 }

 void draw_pl(){
	uint16_t mask = 0x0003;
	mask <<= local_pos;

	if (xSemaphoreTake(*screen_mutex, (TickType_t) 10) == pdTRUE)
	{
		screen_buffer[0] |= mask;
		//screen_buffer[ball_curr_pos[0]] = 0;
		xSemaphoreGive(*screen_mutex);
	}
	else{
		//blocked by game idle
		is_blocked = 1;
	}
 }


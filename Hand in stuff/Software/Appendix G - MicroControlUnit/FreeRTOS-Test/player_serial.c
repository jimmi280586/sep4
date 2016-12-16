/*
 * player_serial.c
 *
 * Created: 13/12/2016 12:38:23
 *  Author: David
 */ 

 #include "player_serial.h"
 #include "protocol.h"

  static uint8_t *pl_pos;
  static uint8_t local_pos = 4;
  static SemaphoreHandle_t *pl_mutex;
  static SemaphoreHandle_t *screen_mutex;
  static uint16_t *screen_buffer;
  static QueueHandle_t *queue;
  static frame_t data_frame;

  void *p_serial(){

	if (xQueueReceive(*queue, &data_frame, (TickType_t) 1)){
			if (data_frame.bytes[0] == 0x77){
			clr_p_s();
			p_s_up();
			draw_p_s();
		}
		else if (data_frame.bytes[0] == 0x73)
		{
			clr_p_s();
			p_s_down();
			draw_p_s();
		}
	}
	  return p_serial;
  }

  void *p_serial_idle(){
	  if (xSemaphoreTake(*screen_mutex, (TickType_t) 1))
	  {
		  xSemaphoreGive(*screen_mutex);
		  return p_serial;
	  }
	  return p_serial_idle;
  }

  void *init_p_serial(QueueHandle_t *que, SemaphoreHandle_t *scr_mtx,SemaphoreHandle_t *pl_mtx, uint8_t *p_pos, uint16_t *scr_buff){
	  
	  screen_mutex = scr_mtx;
	  pl_pos = p_pos;
	  pl_mutex = pl_mtx;
	  screen_buffer = scr_buff;
	  queue = que;
	  return p_serial_idle;
  }

  void p_s_up(){
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

  void p_s_down(){
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

  void clr_p_s(){
	  uint16_t mask = 0x0003;
	  mask <<= local_pos;
	  mask = ~mask;
	  if (xSemaphoreTake(*screen_mutex, (TickType_t) 2) == pdTRUE)
	  {
		  screen_buffer[13] &= mask;
		  //screen_buffer[ball_curr_pos[0]] = 0;
		  xSemaphoreGive(*screen_mutex);
	  }
  }

  void draw_p_s(){
	  uint16_t mask = 0x0003;
	  mask <<= local_pos;

	  if (xSemaphoreTake(*screen_mutex, (TickType_t) 2) == pdTRUE)
	  {
		  screen_buffer[13] |= mask;
		  //screen_buffer[ball_curr_pos[0]] = 0;
		  xSemaphoreGive(*screen_mutex);
	  }
  }
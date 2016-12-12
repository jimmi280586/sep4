/*
 * protocol.c
 *
 * Created: 2016. 12. 12. 18:02:25
 *  Author: David
 */ 
  #include "protocol.h"

  #define prot_ESC 0xFF
  #define prot_FLAG 0x61

  static frame_t _frame;
  static QueueHandle_t *frames;

  void *prot_idle(uint8_t byte){
	  if (byte == prot_FLAG ){
		  return prot_data;
	  }
	  else{
		  return prot_idle;
	  }
  }

  void *prot_idle_entry(){
	clr_frm_buff();
	return prot_idle;
  }


  void *prot_data(uint8_t byte){
	if (byte == prot_ESC)
	{
		return prot_esc;
	}
	else if (byte == prot_FLAG)
	{
		finish_frame();
		return prot_idle_entry;
	}
	else
	{
		add_byte_to_frame(byte);
		return prot_data;
	}
  }

  void *prot_esc(uint8_t byte) {
	  if (0)
	  {
		  return prot_idle_entry;
	  }
	  else{
		  add_byte_to_frame(byte);
		  return prot_data;
	  }
  }

  void clr_frm_buff(){
	_frame.size = 0;
  }

 void add_byte_to_frame(uint8_t byte){
	_frame.bytes[_frame.size] = byte;
	++(_frame.size);
}

void finish_frame(){
	xQueueSend(*frames, &_frame, (TickType_t) 1);
}

void *init_protocol(QueueHandle_t *q){
	frames = q;
	_frame.bytes[0] = 0;
	_frame.bytes[1] = 0;
	_frame.bytes[2] = 0;
	_frame.bytes[3] = 0;
	_frame.size = 0;
	return prot_idle;
}


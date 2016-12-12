/*
 * game.c
 *
 * Created: 2016. 12. 12. 18:01:57
 *  Author: David
 */ 

 #include "game.h"

 static uint16_t *screen_buffer;
 static uint8_t direction = 0;
 static uint8_t ball_curr_pos[] = {7,5};
 static uint8_t ball_next_pos[] = {0,0};

 void *game_idle(){
	 //wait for any button to cont.
	 //joystick pins: PC0, PC1, PC6, PC7, PD3
	 // PC mask 11000011 - 0xc3
	 if (joy_PUSH ){
		 return game_run;
	 }
	 else{
		 return game_idle;
	 }
 }

 void *game_idle_entry(){
	clr_scr();
	return game_idle;
 }

 void *game_idle_exit(){
	return NULL;
 }

 void *game_run(){
	//move ball
	uint8_t is_bounced = 1;

	while(is_bounced != 0){
		is_bounced = 0;
		calc_next();
		if (ball_next_pos[0] > 13 || ball_next_pos[1] > 9){
			bounce();
			is_bounced = 1;
		}
		
	}
	refresh_ball();
	 return game_run;
 }

 void *game_score() {
	 if (joy_PUSH)
	 {
		 return game_idle_entry;
	 }
	 else{
		 return game_score;
	 }
 }

 void *init_game(uint8_t *p){
	 screen_buffer = p;
	 display_title();
	 vTaskDelay(2000);
	 return game_idle_entry;
 }

 void display_title(){
	screen_buffer[13] = 240;
	screen_buffer[12] = 336;
	screen_buffer[11] = 288;
	screen_buffer[10] = 0;
	screen_buffer[9] = 224;
	screen_buffer[8] = 16;
	screen_buffer[7] = 240;
	screen_buffer[6] = 0;
	screen_buffer[5] = 96;
	screen_buffer[4] = 144;
	screen_buffer[3] = 96;
	screen_buffer[2] = 12;
	screen_buffer[1] = 18;
	screen_buffer[0] = 254;
 }

 void clr_scr(){
	 screen_buffer[13] = 0;
	 screen_buffer[12] = 0;
	 screen_buffer[11] = 0;
	 screen_buffer[10] = 0;
	 screen_buffer[9] = 0;
	 screen_buffer[8] = 0;
	 screen_buffer[7] = 0;
	 screen_buffer[6] = 0;
	 screen_buffer[5] = 0;
	 screen_buffer[4] = 0;
	 screen_buffer[3] = 0;
	 screen_buffer[2] = 0;
	 screen_buffer[1] = 0;
	 screen_buffer[0] = 0;
 }

 void calc_next(){
	ball_next_pos[0] = ball_curr_pos[0];
	ball_next_pos[1] = ball_curr_pos[1];

	switch (direction)
	{
		case 0:
		--ball_next_pos[1];
		break;
		case 1:
		++ball_next_pos[0];
		--ball_next_pos[1];
		break;
		case 2:
		++ball_next_pos[0];
		break;
		case 3:
		++ball_next_pos[0];
		++ball_next_pos[1];
		break;
		case 4:
		++ball_next_pos[1];
		break;
		case 5:
		--ball_next_pos[0];
		++ball_next_pos[1];
		break;
		case 6:
		--ball_next_pos[0];
		break;
		case 7:
		--ball_next_pos[0];
		--ball_next_pos[1];
		break;
	}
 }

 void bounce(){
	
	if ( (direction%2)==0){
		uint8_t r = rand()%3;
		r += 3;
		r += direction;
		r %= 8;
		direction = r;
	}
	else{
		if (ball_next_pos[1] > 9){
			switch (direction){
				case 1:
				direction = 3;
				break;
				case 3:
				direction = 1;
				break;
				case 5:
				direction = 7;
				break;
				case 7:
				direction = 5;
				break;
			}
		}
		else{
			switch (direction){
				case 1:
				direction = 7;
				break;
				case 3:
				direction = 5;
				break;
				case 5:
				direction = 3;
				break;
				case 7:
				direction = 1;
				break;
			}
		}
	}
 }

 void refresh_ball(){
	screen_buffer[ball_curr_pos[0]] &= !(1 << ball_curr_pos[1]);
	//screen_buffer[ball_curr_pos[0]] = 0;
	ball_curr_pos[0] = ball_next_pos[0];
	ball_curr_pos[1] = ball_next_pos[1];

	screen_buffer[ball_curr_pos[0]] |= (1 << ball_curr_pos[1]);
	//screen_buffer[ball_curr_pos[0]] = 1023;
 }
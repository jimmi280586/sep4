/*
 * game.c
 *
 * Created: 2016. 12. 12. 18:01:57
 *  Author: David
 */ 

 #include <stdint.h>
 #include <stddef.h>
 #include <avr/io.h>

 #define joy_UP !(PINC & 0x40)
 #define joy_DOWN !(PINC & 0x01)
 #define joy_LEFT !(PINC & 0x80)
 #define joy_RIGHT !(PINC & 0x02)
 #define joy_PUSH !(PIND & 0x08)
 #define joy_ALLDIR !(PINC & 0xC3)

 typedef void *(*StatFunc)();

 void *idle();
 void *run();
 void *score();

 void *idle(){
	 //wait for any button to cont.
	 //joystick pins: PC0, PC1, PC6, PC7, PD3
	 // PC mask 11000011 - 0xc3
	 if (joy_PUSH ){
		 return run;
	 }
	 else{
		 return idle;
	 }
 }

 void *run(){
	 return score;
 }

 void *score() {
	 if (joy_PUSH)
	 {
		 return idle;
	 }
	 else{
		 return score;
	 }
 }
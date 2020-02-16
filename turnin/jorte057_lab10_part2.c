/*	Author: jorte057
 *  Partner(s) Name: Duke Pham dpham073@ucr.edu
 *	Lab Section:
 *	Assignment: Lab #  Exercise #
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 */
#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

volatile unsigned char TimerFlag = 0; // TimerISR() sets this to 1. C programmer should clear to 0.

//Internal variables for mapping AVR's ISR to our cleaner TimerISR model.
unsigned long _avr_timer_M = 1; // Start count from here, down to 0. Default 1 ms. 
unsigned long _avr_timer_cntcurr = 0; // Current internal count of 1ms ticks 
unsigned char threeLEDs;
unsigned char blinkingLED;

void TimerOn(){

TCCR1B = 0x0B;

OCR1A = 125; 

TIMSK1 = 0x02;

TCNT1 = 0;

_avr_timer_cntcurr = _avr_timer_M;

SREG |= 0x80;
}

void TimerOff(){
	TCCR1B = 0x00; // bit3bit1bit0: timer off

}

void TimerISR(){
	TimerFlag = 1;
}

enum ThreeLEDsSM {three_init, three_zero, three_one, three_two} three_states;

void TickFct_ThreeLEDs(){
	switch(three_states){
		case three_init:
			three_states = three_zero;
			break;
		case three_zero:
			three_states = three_one;
			break;
		case three_one:
			three_states = three_two;
			break;
		case three_two:
			three_states = three_zero;
			break;
		default:
			break;
	}
	switch (three_states){
		case three_init:
			break;
		case three_zero:
			threeLEDs = 0x01;
			break;
		case three_one:
			threeLEDs = 0x02;
			break;
		case three_two:
			threeLEDs = 0x04;
			break;
		default:
			break;
	}
	
}

enum BlinkingLEDSM {blink_init, blink_on, blink_off} blink_state;
void TickFct_BlinkingLED(){
	switch(blink_state){
		case blink_init:
			blink_state = blink_on;
			break;
		case blink_on:
			blink_state = blink_off;
			break;
		case blink_off:
			blink_state = blink_on;
			break;
		default:
			break;
	}
	switch(blink_state){
		case blink_init:
			break;
		case blink_on:
			blinkingLED = 0x08;
			break;
		case blink_off:
			blinkingLED = 0x00;
			break;
		default:
			break;
	}
}

enum CombineLEDsSM {combine_init, combine_logic} combine_state;
void TickFct_CombineLED(){
	switch(combine_state){
		case combine_init:
			combine_state = combine_logic;
			break;
		case combine_logic:
			PORTB = (blinkingLED | threeLEDs);
			combine_state = combine_logic;
			break;
		default:
			break;
	}
}
ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if(_avr_timer_cntcurr == 0){
		TimerISR();
		_avr_timer_cntcurr = _avr_timer_M;
	
	}
}

		void TimerSet(unsigned long M){
		_avr_timer_M = M;
		_avr_timer_cntcurr = _avr_timer_M;
	}

int main(void) {
    /* Insert DDR and PORT initializations */
	unsigned long TL_elapsedTime = 300;
	unsigned long BL_elapsedTime = 1000;
	const unsigned long timerPeriod = 100;
	DDRB = 0xFF; PORTB = 0x00;
	TimerSet(timerPeriod);
	TimerOn();
	three_states = three_init;
	blink_state = blink_init;
	
    /* Insert your solution below */
    while (1) {
	if (TL_elapsedTime >= 300){
		TickFct_ThreeLEDs();
		TL_elapsedTime = 0;
	}
	if (BL_elapsedTime >= 1000){
		TickFct_BlinkingLED();
		BL_elapsedTime = 0;
	}
	TickFct_CombineLED();
	while (!TimerFlag){}	
	TimerFlag = 0;
	TL_elapsedTime  += timerPeriod;
	BL_elapsedTime += timerPeriod;
    }
    return 1;
}

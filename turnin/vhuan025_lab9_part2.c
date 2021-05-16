/*	Author: lab
 *  Partner(s) Name: 
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

volatile unsigned char TimerFlag = 0;

unsigned long timer = 1;
unsigned long timer_count = 0;
void TimerOn() {
	TCCR1B = 0x0B;
	OCR1A = 125;
	TIMSK1 = 0x02;
	TCNT1 = 0;
	timer_count = timer;
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B = 0x00; 
}

void TimerISR() {
	TimerFlag = 1;
}

ISR(TIMER1_COMPA_vect) {
	timer_count--;
	if(timer_count == 0) {
		TimerISR();
		timer_count = timer;
	}
}

void TimerSet(unsigned long M) {
	timer = M;
	timer_count = timer;
}

unsigned char threeLEDs = 0;
enum T_STATES { T_Start, T_0, T_1, T_2 } T_State;
void ThreeLEDsSM() {
	switch(T_State) {
		case T_Start:
			T_State = T_0;
			break;
		case T_0:
			T_State = T_1;
			break;
		case T_1:
			T_State = T_2;
			break;
		case T_2:
			T_State = T_0;
			break;
	}
	switch(T_State) {
		case T_Start:
			break;
		case T_0:
			threeLEDs = 0x01;
			break;
		case T_1:
			threeLEDs = 0x02;
			break;
		case T_2:
			threeLEDs = 0x04;
			break;
	}
}

unsigned char blinkingLED = 0;
enum B_STATES { B_Start, B_Off, B_On } B_State;
void BlinkingLEDSM() {
	switch(B_State) {
		case B_Start:
			B_State = B_Off;
			break;
		case B_Off:
			B_State = B_On;
			break;
		case B_On:
			B_State = B_Off;
                        break;
	}
	switch(B_State) {
		case B_Start:
                        break;
                case B_Off:
                        blinkingLED = 0;
                        break;
                case B_On:
                        blinkingLED = 0x08;
                        break;
	}
}

enum C_STATES { C_Start, C_Combine } C_State;
void CombineLEDsSM() {
	unsigned char tmpB = 0;
        switch(C_State) {
                case C_Start:
                        C_State = C_Combine;
                        break;
		case C_Combine:
			C_State = C_Combine;
			break;
        }
        switch(C_State) {
                case C_Start:
                        break;
                case C_Combine:
                        tmpB = blinkingLED | threeLEDs;
                        break;
        }
	PORTB = tmpB;
}


int main(void) {
    /* Insert DDR and PORT initializations */
	DDRA = 0x00; PORTA = 0xFF;
	DDRB = 0xFF; PORTB = 0x00;
	unsigned long T_Time = 300;
	unsigned long B_Time = 1000;
	const unsigned long period = 100;
	TimerSet(period);
	TimerOn();
	T_State = T_Start;
	B_State = B_Start;
	C_State = C_Start;
    /* Insert your solution below */
    while (1) {
	if(T_Time >= 300) {
		ThreeLEDsSM();
		T_Time = 0;
	}
	if(B_Time >= 1000) {
		BlinkingLEDSM();
		B_Time = 0;
	}
	CombineLEDsSM();
	while(!TimerFlag);
	TimerFlag = 0;
	T_Time += period;
	B_Time += period;
    }
    return 1;
}

/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #12  Exercise #3
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: https://youtu.be/Qomv-zIvL2g
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

typedef struct task{
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int(*TickFct)(int);
}task;

task tasks[1];
const unsigned short num_task=1;
const unsigned long period_task=1;

volatile unsigned char TimerFlag = 0; 
unsigned long _avr_timer_M = 1; 
unsigned long _avr_timer_cntcurr = 0; 
void TimerSet(unsigned long M) {
	_avr_timer_M = M;
	_avr_timer_cntcurr = _avr_timer_M;
}
void TimerOn() {
	TCCR1B 	= 0x0B;	
	OCR1A 	= 125;	
	TIMSK1 	= 0x02; 
	TCNT1 = 0;
	_avr_timer_cntcurr = _avr_timer_M;	
	SREG |= 0x80;
}

void TimerOff() {
	TCCR1B 	= 0x00; 
}

void TimerISR() {
	unsigned char i;
	for(i = 0;i < num_task; ++i){
		if(tasks[i].elapsedTime >= tasks[i].period){
			tasks[i].state=tasks[i].TickFct(tasks[i].state);
			tasks[i].elapsedTime=0;
		}
		tasks[i].elapsedTime += period_task;
	}
}

ISR(TIMER1_COMPA_vect){
	_avr_timer_cntcurr--;
	if (_avr_timer_cntcurr == 0) { 	
		TimerISR(); 				
		_avr_timer_cntcurr = _avr_timer_M;
	}
}


enum SR_state {start_1, rect};

int tick_1(int state){



static const unsigned char r_pattern[3]={0x3C,0x24,0x3C};	
static const unsigned char r_row[3]={0xF7,0xFB,0xFD};  	
static unsigned char i;	
static unsigned char j; 
static unsigned char c = 0;
                   
                    
switch (state) {
    case start_1:
        state = start_1;
        break;
    default:
        break;
}

switch (state) {
    case start_1:
        i = r_pattern[c%3];
        j = r_row[c%3];
        c++;
        break;

    default:
        break;
}
	PORTC = i;	
	PORTD = j;	
	return state;

}


int main(void) {
	DDRC=0xFF; PORTC=0x00;
	DDRD=0xFF; PORTD=0x00;

	unsigned char i = 0;
	tasks[i].state = start_1;
	tasks[i].period = 1;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &tick_1;
	TimerSet(1);
	TimerOn();

	while (1) {}
	return 1;
}

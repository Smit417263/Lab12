/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #12  Exercise #2
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
const unsigned long period_task=100;

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


enum SR_state {start_1,mid,inc,dec};

int tick_1(int state){

static unsigned char pattern = 0xFF;
static unsigned char row = 0xFB;


switch (state) {
    case start_1:
        state = mid;
        pattern = 0x80;
        row = 0xE0;
        break;
    case mid:
        if(~PINA&0x01 && !((~PINA>>1)&0x01)){
            state = inc;
            if(pattern != 0x80){
                pattern = pattern << 1;
            }
        }
        else if((!(~PINA&0x01)) && ((~PINA>>1)&0x01)){
            state = dec;
            if(pattern != 0x01){
                pattern = pattern >> 1;
            }
        }
        else{
            state = mid;
        }
        break;
    case inc:
        if((~PINA&0x01) && !((~PINA>>1)&0x01)){
            state = inc;
        }
        else if(!(~PINA&0x01) && !((~PINA>>1)&0x01)){
            state = mid;
        }
        break;

    case dec:
        if(!(~PINA&0x01) && ((~PINA>>1)&0x01)){
            state = dec;
        }
        else if(!(~PINA&0x01) && !((~PINA>>1)&0x01)){
            state = mid;
        }
        break;
    default:
        break;
}
	PORTC = pattern;
	PORTD = row;
	return state;
}


int main(void) {
	DDRA=0x00; PORTA=0xFF;
	DDRC=0xFF; PORTC=0x00;
	DDRD=0xFF; PORTD=0x00;

	unsigned char i = 0;
	tasks[i].state = start_1;
	tasks[i].period = 100;
	tasks[i].elapsedTime = 0;
	tasks[i].TickFct = &tick_1;
	TimerSet(100);
	TimerOn();

	while (1) {}
	return 1;
}

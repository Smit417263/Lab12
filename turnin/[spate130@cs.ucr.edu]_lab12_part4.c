/*	Author: lab
 *  Partner(s) Name: 
 *	Lab Section: 023
 *	Assignment: Lab #12  Exercise #4
 *	Exercise Description: [optional - include for your own benefit]
 *
 *	I acknowledge all content contained herein, excluding template or example
 *	code, is my own original work.
 *
 *	Demo Link: 
 */


#include <avr/io.h>
#include <avr/interrupt.h>
#ifdef _SIMULATE_
#include "simAVRHeader.h"
#endif

#define b_up (~PINA&0x01)
#define b_dow ((~PINA>>1) & 0x01)
#define b_lef ((~PINA>>2) & 0x01)
#define b_rig ((~PINA>>3) & 0x01)

unsigned char r_pattern[3]={0x3C,0x24,0x3C};
unsigned char r_row[3]={0xF7,0xFB,0xFD};

typedef struct task{
    int state;
    unsigned long period;
    unsigned long elapsedTime;
    int(*TickFct)(int);
}task;

task tasks[2];
const unsigned short num_task = 2;
const unsigned long period_task = 1;

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



enum SR_state {start_1};
int tick_1(int state){
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


enum DirectionState {start_2,wait,UP,DOWN,LEFT,RIGHT};

int tick_2(int state_2) {
switch (state_2) {
case start_2:
    state_2 = wait;
    break;

case wait:
    if(b_up&&(!b_dow)&&(!b_lef)&&(!b_rig)){
        state_2 = UP;
        if(r_row[2] != 0xFE){
            r_row[0] = (r_row[0]>>1)|0x80;
            r_row[1] = (r_row[1]>>1)|0x80;
            r_row[2] = (r_row[2]>>1)|0x80;
        }
    }
    else if((!b_up)&&(!b_dow)&&b_lef&&(!b_rig)){
        state_2 = LEFT;
        if(r_pattern[0] != 0xF0){
            r_pattern[0] = r_pattern[0]<<1;
            r_pattern[1] = r_pattern[1]<<1;
            r_pattern[2] = r_pattern[2]<<1;
        }
    }
    else if((!b_up)&&(!b_dow)&&(!b_lef)&&b_rig){
        state_2 = RIGHT;
        if(r_pattern[2] != 0x0F){
            r_pattern[0] = r_pattern[0]>>1;
            r_pattern[1] = r_pattern[1]>>1;
            r_pattern[2] = r_pattern[2]>>1;
        }
    }
    else if((!b_up)&&b_dow&&(!b_lef)&&(!b_rig)){
        state_2 = DOWN;
        if(r_row[0] != 0xEF){
            r_row[0] = (r_row[0]<<1)|0x01;
            r_row[1] = (r_row[1]<<1)|0x01;
            r_row[2] = (r_row[2]<<1)|0x01;
        }

    }
    else{
        state_2 = wait;
    }
    break;

case UP:
    if(b_up&&(!b_dow)&&(!b_lef)&&(!b_rig)){
        state_2 = UP;
    }
    else if((!b_up)&&(!b_dow)&&(!b_lef)&&(!b_rig)){
        state_2 = wait;
    }
    break;

case DOWN:
    if((!b_up)&&b_dow&&(!b_lef)&&(!b_rig)){
            state_2 = DOWN;
    }
    else if((!b_up)&&(!b_dow)&&(!b_lef)&&(!b_rig)){
            state_2 = wait;
    }
    break;

case LEFT:
    if((!b_up)&&(!b_dow)&&b_lef&&(!b_rig)){
            state_2 = LEFT;
    }
    else if((!b_up)&&(!b_dow)&&(!b_lef)&&(!b_rig)){
            state_2 = wait;
    }
    break;

case RIGHT:
    if((!b_up)&&(!b_dow)&&(!b_lef)&&b_rig){
        state_2 = RIGHT;
    }
    else if((!b_up)&&(!b_dow)&&(!b_lef)&&(!b_rig)){
        state_2 = wait;
    }
    break;

default:
    break;
}

return state_2;
}


int main(void) {
	DDRA=0x00;PORTA=0xFF;
	DDRC=0xFF;PORTC=0x00;
	DDRD=0xFF;PORTD=0x00;
	unsigned char i=0;
	tasks[i].state=start_1;
	tasks[i].period=1;
	tasks[i].elapsedTime=0;
	tasks[i].TickFct=&tick_1;
	i++;
	tasks[i].state = start_2;
    tasks[i].period=1;
    tasks[i].elapsedTime=0;
    tasks[i].TickFct=&tick_2;
	TimerSet(1);
	TimerOn();
	while (1) {}
	return 1;
}

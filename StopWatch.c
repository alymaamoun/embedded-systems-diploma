/*
 *F_CPU: 1MHZ
 *Timer:TIMER1 CTC												 (initializing is done)
 *7447 on PC0->PC4 (OUT)										 (initializing is done)
 *display enable on PA0->PA5(lSec-hHour) (OUT)				     (initializing is done)
 *INT0 reset counting 	(internal pull up)	(falling edge)	(PD2)(initializing is done)
 *INT1 pause counting 	(external pull down)(rising edge)	(PD3)(initializing is done)
 *INT2 resume counting 	(internal pull up)	(falling edge)	(PB2)(initializing is done)
 *Remaining:Timer configuration
 *compare value: 977 (OCR1)
 *Prescalar:1024 (Set CS12 and CS10 in TCCR1B)
 *initial value:0 (TCNT1)
 *not PWM : (set FOC1 in TCCR1A)
 *CTC mode: (set WGM12 and clear WGM11 in TCCR1B)
 */
#include<avr/io.h>
#include"util/delay.h"
#include <avr/interrupt.h>

short lSec=0,hSec=0,lMin=0,hMin=0,lHour=0,hHour=0;
char Iflag=1,i=0;


void printer();
void incrementer();




void Int0(){

	DDRD&=~(1<<PD2);				//configuring PD2 to be input
	PORTD|=(1<<PD2);				//enabling internal pull up resistance
	MCUCR|=(1<<ISC01);
	MCUCR&=~(1<<ISC00);				//INT0 on falling edge
	GICR|=(1<<INT0);				//INT0 module interrupt enabled




}

void Int1(){

	DDRD&=(~(1<<PD3));				//configuring PD3 to be input
	PORTD&=~(1<<PD3);
	MCUCR|=(1<<ISC11)|(1<<ISC10);	//INT1 on rising edge
	GICR|=(1<<INT1);				//INT1 module interrupt enabled


}

void Int2(){


	DDRB   &= (~(1<<PB2));   // Configure INT2/PB2 as input pin
	PORTB |=(1<<PB2);		//enabling internal pull up resistance
	MCUCSR &=~ (1<<ISC2);     // Trigger INT2 with the raising edge
	GICR   |= (1<<INT2);	 // Enable external interrupt pin INT2



}

void InitMisc(){
	DDRC|=0x0f;
	PORTC&=~0x0f;
	DDRA|=0x7f;
	PORTA&=~0x7f;

	//timer led tester
	DDRD|=0x01;
	PORTD&=~0x01;
}

void Timer1init(){
	TCNT1=0;
	OCR1A=15625;
	TIMSK = (1<<OCIE1A);
	TCCR1A=(1<<FOC1A);
	TCCR1B=(1<<WGM12)|(1<<CS11)|(1<<CS10);

}

ISR(TIMER1_COMPA_vect){

	if(Iflag==1){
		OCR1A=15625;
		incrementer();
		PORTD|=0x01;}
	if(Iflag==2){
		PORTD^=0x01;
	}
	if(Iflag==3){
		OCR1A=977;
		PORTD^=(1<<PD0);
		i++;
		if(i>6){
			OCR1A=15625;
			Iflag=1;
			i=0;}
	}


}

ISR(INT0_vect){
	lSec=0,hSec=0,lMin=0,hMin=0,lHour=0,hHour=0;
	Iflag=3;
}

ISR(INT1_vect){
		Iflag=2;
}

ISR(INT2_vect){
	Iflag=1;
}

int main(void){
	SREG&=~(1<<7);
	Iflag=1;
	Timer1init();
	SREG|=(1<<7);
	Int0();
	InitMisc();
	Int2();
	Int1();



	while(1){

		printer();

}
}

void incrementer(){
	if((lSec+1)==10){
		lSec=0;
		if((hSec+1)==6){
			hSec=0;
			if((lMin+1)==10){
				lMin=0;
				if((hMin+1)==6){
					hMin=0;
					if((lHour+1)==10){
						lHour=0;
						if((hHour+1)==10){
							lSec=0,hSec=0,lMin=0,hMin=0,lHour=0,hHour=0;
						}
						else{
							hHour++;
						}

					}
					else{
						lHour++;
					}
				}
				else{
					hMin++;
				}
			}
			else{
				lMin++;
			}
		}
		else{
			hSec++;
		}
	}
	else{
		lSec++;
	}
}
void printer(){
	PORTA=0b01000001,PORTC=lSec;


	_delay_ms(1);
	PORTA=0b01000010,PORTC=hSec;


	_delay_ms(1);
	PORTA=0b00000100,PORTC=lMin;


	_delay_ms(1);
	PORTA=0b01001000,PORTC=hMin;


	_delay_ms(1);
	PORTA=0b00010000,PORTC=lHour;


	_delay_ms(1);
	PORTA=0b01100000,PORTC=hHour;


	_delay_ms(1);




}

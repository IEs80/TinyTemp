/*
 * timer0_driver.c
 *
 * Created: 10/3/2026 09:06:52
 *  Author: IESz
 */ 

#include "i2c.h"
#include "timer0_driver.h"

//global variables
extern volatile uint8_t usi_state;	//defined in i2c_driver.c

//ISRs
ISR(TIMER0_COMPA_vect)
{
	//  USICR |= (1<<USITC);
	
	//clean the interrupt flag ()
	TIFR |= (1<<OCF0A);
	
	if(usi_state!=usi_idle)
	{
		
		USICR |= (1<<USITC);   // un tick del USI
	}

	//set the count to 0
	TCNT0 = 0x00;
}




//Function definitions
void attiny_timer_init()
{
	//we need Timer0 Compare Match
	//note that frec_scl = fclk/2 => frec_scl = 100KHz , fclk = 200KHz


	//Select the "compare match" mode
	
	TCCR0A &= ~(0x01 << 0); //write 0 to WGM00
	TCCR0A |= (0x01 << 1);  //write 1 to WGM01
	TCCR0B &= ~(0x01 << 3); //write 0 to WGM02

	//set the TOP value for the counter
	OCR0A = 0x27;
	//OCR0A = 0x05;

	//set the prescaler to 1
	TCCR0B = 0x01;

	//set the count to 0
	TCNT0 = 0x00;

	//habilita TIMER0 COMPA interrupt
	TIMSK |= (1 << OCIE0A);
}

void attiny_timer_deinit()
{
	//disable TIMER0 COMPA interrupt
	TIMSK &= ~(1 << OCIE0A);
	
	//disable clock source for TIMER0
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
}


void attiny_timer_interrupt_disable()
{
	TIMSK &= ~(1 << OCIE0A);
}


void attiny_timer_interrupt_enable()
{
	TIMSK |= (1 << OCIE0A);
}
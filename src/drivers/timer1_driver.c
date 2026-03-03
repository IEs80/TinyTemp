/*
 * timer1_driver.c
 *
 * Created: 3/3/2026 19:05:07
 *  Author: IESz
 */ 


#include "timer1_driver.h"

/*	
	@fn:  timer1_init
	@brief: inits timer1 for it to increment TCNT1 every uSeg
	@params: none
	@returns: none
*/
void timer1_init()
{
	//TCCR1
	/*
		bit7:	0 (Don't clear on compare match)
		bit6:	0 (PWM Enable off)
		bit5-4: 00 (0 because we don't use the compare match)
		bit3-0:	0100 (CK/8 = 1MHz)
	*/
	TCCR1 = 0x04;
	
	/*PLL Control and Status Register
	Bit 7: 0
	Bit 2: 0 (System Clock)
	Bit 1: 1
	Bit 0: 0
	*/
	//PLLCSR = 0x02;
	 
	//set the count to 0
	TCNT1 = 0x00;
}

/*	
	@fn:  timer1_deinit
	@brief: de-inits timer1 for it to increment TCNT1 every uSeg
	@params: none
	@returns: none
*/
void timer1_deinit()
{
	//TCCR1
	/*
		bit7:	0 (Don't clear on compare match)
		bit6:	0 (PWM Enable off)
		bit5-4: 00 (0 because we don't use the compare match)
		bit3-0:	0100 (CK/8 = 1MHz)
	*/
	TCCR1 = 0x00;
	
	/*PLL Control and Status Register
	Bit 7: 0
	Bit 2: 0 (System Clock)
	Bit 1: 1
	Bit 0: 0
	*/
	//PLLCSR = 0x02;
	 
	//set the count to 0
	TCNT1 = 0x00;
}

void timer1_set_prescaler(uint16_t prescaler)
{
	switch(prescaler)
	{
		case 8:
			TCCR1 = 0x04;
			break;
		case 4096:
			TCCR1 = 0x0E;
			break;
		default:
			TCCR1 = 0x00; //stops the timers
			break;
		
	}
	
}

void timer1_reset_count()
{
	TCNT1=0x00;
}

uint8_t timer1_get_count()
{
	return TCNT1;
}
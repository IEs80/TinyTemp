/*
 * GccApplication1.c
 *
 * Created: 10/1/2026 17:07:28
 * Author : IESz
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//includes
#include "i2c.h"

//Defines
#define LED PB4
//Macros
#define LED_ON     PORTB|=(0x01<<LED) //LED on

//Global variables
static char a = 0;
extern volatile uint8_t usi_state;	//defined in i2c_driver.c
extern volatile uint8_t f_nack;		//defined in i2c_driver.c

//Function prototypes
void attiny_timer_init(void);


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

/*
	Attiny Init
*/
void attiny_init()
{
	  //init Timer 0
	  attiny_timer_init();
	  //init USI 
	  attiny_i2c_init();

	  // enable interrupts 
	  sei();


	  //Test LED
	  DDRB |= (0x01<<LED); //LED as Out
	  PORTB&=~(0x01<<LED); //LED off
}



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



int main(void)
{
	//Init ATtiny
	attiny_init();
	
    /* Replace with your application code */
    while (1) 
    {
		if (a==0)
		{
			//attiny_i2c_tx();
			//turn on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xAF);
			//full-on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA5);
			//display sleep mode
			//attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			
			a=1;
		}
		
		if(f_nack)
		{
			f_nack = 0;
			LED_ON;
		}
	
	}
}


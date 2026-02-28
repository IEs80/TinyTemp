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
#include "oled.h"
#include "dht.h"

//Defines
#define LED PB4
//Macros
#define GLED_ON     PORTB|=(0x01<<LED) //LED on
#define YLED_ON     PORTB|=(0x01<<LED) //LED on
#define RLED_ON     PORTB|=(0x01<<LED) //LED on

//Global variables
static char a = 1;
static char	b = 0;
extern volatile uint8_t usi_state;	//defined in i2c_driver.c
extern volatile uint8_t f_nack;		//defined in i2c_driver.c

extern volatile uint8_t _v_dht_data_ready;
extern volatile uint8_t	_v_dht_temp_int
extern volatile uint8_t	_v_dht_temp_dec
extern volatile uint8_t	_v_dht_rh_int 
extern volatile uint8_t	_v_dht_rh_dec 




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

void attiny_timer_deinit()
{
	//disable TIMER0 COMPA interrupt
	TIMSK &= ~(1 << OCIE0A);	
	
	//disable clock source for TIMER0
	TCCR0B &= ~((1 << CS02) | (1 << CS01) | (1 << CS00));
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
	
	DDRB |= (0x01<<LED); //LED as Out
	DDRB |= (0x01<<PB3);
	PORTB&=~(0x01<<LED); //LED off
	PORTB&=~(0x01<<PB3);

	
	//Init ATtiny
	//attiny_init();
	TWI_DELAY();
	TWI_DELAY();
	TWI_DELAY();
	TWI_DELAY();
	
	attiny_dht_init();
    /* Replace with your application code */
    while (1) 
    {
		
		
		if(b==0)
		{

			//PORTB|=(0x01<<PB3); //ON
			//_delay_ms(500);
			//PORTB &= ~((0x01 << PB3)); //OFF
			//_delay_ms(500);
					
					
					
			//dht_start();
			dht_read();
			if(_v_dht_data_ready)
			{
				a=0;	
			}
			//b=1;
		}
		
		if (a==0)
		{
			//attiny_i2c_tx();
			//turn on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xAF);
			//full-on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA5);
			//display sleep mode
			//attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			oled_clean(standar_mode);
			
			//write text
			//oled_print_text("TEMP: 0°C",2,32);
			//oled_print_text("HUM:  70%",5,32);
			oled_draw_weather(sunny,2,32);
			//full-on display (using gdram)
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			
			attiny_timer_deinit();

			a=1;
		}
		
		


		
		
		if(f_nack)
		{
			f_nack = 0;
			GLED_ON;
		}
	
	}
}


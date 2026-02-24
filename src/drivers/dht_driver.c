/*
 * dht_driver.c
 *
 * Created: 22/2/2026 11:45:23
 *  Author: IESz
 */ 


#include "dht.h"


//Global variables
volatile uint8_t dht_temp_int = 0;
volatile uint8_t dht_temp_dec = 0;
volatile uint8_t dht_rh_int = 0;
volatile uint8_t dht_rh_dec = 0;

volatile uint8_t dht_data_shift_index = 7;

volatile uint16_t dht_temporizer = 0;

volatile uint8_t f_dht_error = 0;	 

#define HIGH 1
#define LOW 0


enum DHT_ERROR_FLAGS{START_ERROR,COM_ERROR};

void setPin(uint8_t pin,uint8_t state)
{
	if(state==HIGH)
	{
		PORTB|=(0x01<<pin);
	}
	else if (state==LOW)
	{
		PORTB&=~(0x01<<pin); 		
	}
}



/*
	Initializes DHT resources
*/
void attiny_dht_init()
{
	//set DHT_PIN as output
	DDRB |= (0x01<<DHT_PIN); //DHT_PIN as Out
	PORTB|=(0x01<<DHT_PIN); //DHT_PIN ON (line in idle state)
}


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
	@fn:  dht_start
	@brief: generates the 18mS start signal for the DHT
	@params: none
	@returns: none
*/
void dht_start()
{
	//set TIMER1 pre-scaler to 4096
	TCCR1 = 0x0E;
	
	//set DHT_PIN to low
	PORTB&=~(0x01<<DHT_PIN); 
	//set counter to 0
	TCNT1=0x00;	
	
	//wait for the 18mS
	while(TCNT1<18);

	//set DHT_PIN as input...
	DDRB&=~(0x01<<DHT_PIN); 
	//and release the line
	PORTB&=~(0x01<<DHT_PIN); 
	
	while(TCNT1<20); //wait
	PORTB|=(0x01<<PB4) ;
	//set TIMER1 pre-scaler to 8
	TCCR1 = 0x04;
	TCNT1 = 0;
	
	//check for DHT response signal:
	//	80uS LOW, then 80uS HIGH
	while(!f_dht_error)
	{
		//check for LOW state
		while(TCNT1<80)
		{
			//check DHT_PIN status
			if((PINB&(0x01<<DHT_PIN)>>DHT_PIN)==1)
			{
				f_dht_error = START_ERROR;
			}
		}
		
		//rest timer count 
		TCNT1 = 0;
		//check for HIGH state
		while(TCNT1<80)
		{
			//check DHT_PIN status
			if((PINB&(0x01<<DHT_PIN)>>DHT_PIN)==0)
			{
				f_dht_error = START_ERROR;
			}
		}
				
	}
	
	if(f_dht_error){
		//stop timer1
		//return 1
	}

}

/*	
	@fn:  dht_read
	@brief: updates the temp and rh values with new sensor data
	@params: none
	@returns: none
*/
//void dht_read()
//{

	//sends start signal
	
	//reads rh and temp values
	
	//check for stop signal
	 
//}
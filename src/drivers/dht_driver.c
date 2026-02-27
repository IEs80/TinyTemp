/*
 * dht_driver.c
 *
 * Created: 22/2/2026 11:45:23
 *  Author: IESz
 */ 


#include "dht.h"


//Global variables
volatile uint8_t	_v_dht_temp_int = 0;
volatile uint8_t	_v_dht_temp_dec = 0;
volatile uint8_t	_v_dht_rh_int = 0;
volatile uint8_t	_v_dht_rh_dec = 0;
volatile uint8_t	_v_dht_data_shift_index = 7;
volatile uint16_t	_v_dht_temporizer = 0;
volatile uint8_t	_v_response_pulses = 0;
volatile uint8_t	f_dht_error = NO_ERROR;	 
volatile uint8_t	f_pcint = 0;
volatile uint8_t	f_measuring = 0;
volatile uint8_t	f_measure_ready = 0;

volatile	uint8_t dht_state = dht_idle;

volatile uint8_t _v_dht_time = 0;

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
	
	
	//RED LED
	//DDRB |= (0x01<<PB5); //DHT_PIN as Out
	//PORTB&=~(0x01<<PB5); //DHT_PIN ON (line in idle state)
}


/*	
	@fn:  init_pcint
	@brief: enables pin change interrupts for PB3
	@params: none
	@returns: none
*/
void init_pcint()
{
	
	
	// General Interrupt Mask Register
	//bit 6: 0 (INT0 disabled)
	//bit 5: 1 (PCINT enabled)
	GIMSK|=(0x01<<PCIE);
	
	//GIFR: General Interrupt Flag Register
	
	
	//Pin Change Mask Register
	//bit 3: PCINT3 (PB3)
	PCMSK |= (0x01<<DHT_PIN);
	
	
	GIFR = (1<<PCIF);
	
	//enables global interrupts
	sei();
}


/*	
	@fn:  init_pcint
	@brief: disables pin change interrupts for PB3
	@params: none
	@returns: none
*/
void deinit_pcint()
{
	

	//Pin Change Mask Register
	//bit 3: PCINT3 (PB3)
	PCMSK &=~(0x01<<DHT_PIN);
	
	// General Interrupt Mask Register
	//bit 6: 0 (INT0 disabled)
	//bit 5: 1 (PCINT enabled)
	GIMSK&=~(0x01<<5);
	
	//GIFR: General Interrupt Flag Register
	
}


ISR(PCINT0_vect)
{
	
	//clear interrupt flag
	GIFR|=(0x01<<5);
	
	//raise flag
	f_pcint = 1;
	
	
	switch(dht_state)
	{
		case dht_send_start:
			break;
		case dht_read_response:
			if(f_measuring)
			{
				
				_v_dht_temporizer = TCNT1;
				TCNT1 = 0;
				f_measure_ready = 1;
			}
			else
			{
				
				TCNT1 = 0;
				_v_dht_temporizer = 0;
				f_measuring = 1;
			}
			break;
		
	}
	
		
	

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
	
	
	//set TIMER1 pre-scaler to 8, so one tick equals 1uS
	TCCR1 = 0x04;
	//set counter to 0
	TCNT1=0x00;
}

/*	
	@fn:  dht_response
	@brief: read the 80uS response signals from the DHT
	@params: none
	@returns: none
*/
void dht_response()
{
		//the DHT 
		//while((PINB&(0x01<<DHT_PIN)>>DHT_PIN)==1);
		

		

		
		//check for DHT response signal:
		//	80uS LOW, then 80uS HIGH
		if(f_measure_ready)
		{
			f_measure_ready = 0;
			//check if the obtained measure is between accepted time limits (time limits obtained empirically with logic analyzer)
			if( (78<=_v_dht_temporizer) && (87>=_v_dht_temporizer))
			{
				//re-init temporizer
				_v_dht_temporizer = 0;
				//we have to read to pulses, so before continuing in the state machine, we repeat for the second pulse 
				if(_v_response_pulses<2)
				{
					_v_response_pulses++;
				}
				
			}
			else
			{
				f_dht_error = RESPONSE_ERROR;
			}
			
		}	
}



/*	
	@fn:  dht_read
	@brief: updates the temp and rh values with new sensor data
	@params: none
	@returns: none
*/
void dht_read()
{
	//dht_start();
	
	switch(dht_state)
	{
		case dht_idle:
			timer1_init();
			dht_state = dht_send_start;
			break;
			
		case dht_send_start:
			//sends start signal
			dht_start();
			if(f_dht_error!=NO_ERROR)
			{
				dht_state = dht_error;
			}
			else
			{
				while( !(PINB & (0x01 << DHT_PIN)) );
				//inits Pin Change Interrupts
				init_pcint();
				dht_state = dht_read_response;
			}
			break;
		
		//reads rh and temp values
		case dht_read_response:
			
			dht_response();
			if(f_dht_error!=NO_ERROR)
			{
				
				dht_state = dht_error;
			}
			else
			{
				
				if(_v_response_pulses == 2)
				{
					
					dht_state = dht_read_bits;	
				}
				
			}
			break;
		case dht_read_bits:
			
			break;
		
		//check for stop signal
		case dht_stop:
			dht_state = dht_idle;
			break;
		
		case dht_error:
			PORTB|=(0x01<<PB4) ;
			break;
		default:
			break; 
	}
	
}
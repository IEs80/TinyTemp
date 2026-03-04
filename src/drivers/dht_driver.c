/*
 * dht_driver.c
 *
 * Created: 22/2/2026 11:45:23
 *  Author: IESz
 */ 


#include "dht.h"
#include "gpio_driver.h"
#include "timer1_driver.h"

//Global variables
volatile uint8_t	_v_dht_temp_int = 0;
volatile uint8_t	_v_dht_temp_dec = 0;
volatile uint8_t	_v_dht_rh_int = 0;
volatile uint8_t	_v_dht_rh_dec = 0;
volatile uint32_t	_v_dht_data = 0;
volatile uint32_t	_v_dht_chk = 0;

volatile uint8_t	_v_dht_data_shift_index = 40;

volatile uint16_t	_v_dht_temporizer = 0;
volatile uint8_t	_v_response_pulses = 0;
volatile uint8_t	f_dht_error = NO_ERROR;	 
//volatile uint8_t	f_pcint = 0;
//volatile uint8_t	f_measuring = 0;
//volatile uint8_t	f_measure_ready = 0;

volatile	uint8_t dht_state = dht_idle;

volatile uint8_t _v_dht_data_ready = 0;





/*
	Initializes DHT resources
*/
void attiny_dht_init()
{
	//set DHT_PIN as output
	//DDRB |= (0x01<<DHT_PIN); //DHT_PIN as Out
	//PORTB|=(0x01<<DHT_PIN); //DHT_PIN ON (line in idle state)
	setPinDir(DHT_PIN,OUTPUT);
	setPin(DHT_PIN,HIGH);
	
	
	//RED LED
	//DDRB |= (0x01<<PB4); //PB4 as Out
	//PORTB&=~(0x01<<PB4); //PB4 OFF (line in idle state)
	//setPinDir(PB4,OUTPUT);
	//setPin(PB4,LOW);
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

/*
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
*/

/*	
	@fn:  dht_start
	@brief: generates the 18mS start signal for the DHT
	@params: none
	@returns: none
*/
void dht_start()
{
	
	
	//set TIMER1 pre-scaler to 4096
	//TCCR1 = 0x0E; //OG
	timer1_set_prescaler(4096);
	
	//set DHT_PIN to low
	//PORTB&=~(0x01<<DHT_PIN); 
	setPin(DHT_PIN,LOW);
	
	//set counter to 0
	//TCNT1=0x00;	
	timer1_reset_count();
	
	//wait for the 18mS
	while(timer1_get_count()<18);

	//set DHT_PIN as input...
	//DDRB&=~(0x01<<DHT_PIN); 
	setPinDir(DHT_PIN,INPUT);
	
	//and release the line
	//PORTB&=~(0x01<<DHT_PIN); 
	setPin(DHT_PIN,LOW);
	
	
	//set TIMER1 pre-scaler to 8, so one tick equals 1uS
	//TCCR1 = 0x04;
	timer1_set_prescaler(8);
	//set counter to 0
	//TCNT1=0x00;
	timer1_reset_count();
}

/*	
	@fn:  dht_response
	@brief: read the 80uS response signals from the DHT
	@params: none
	@returns: none
*/
void dht_response()
{
	uint8_t timer_val = 0;
	//the DHT 
	//while((PINB&(0x01<<DHT_PIN)>>DHT_PIN)==1);
		timer1_reset_count();
		//the DHT takes 10-20uS to start responding: we wait for the line to go low
		while( getPinState(DHT_PIN) )
		{
			if(timer1_get_count()>TIMEOUT)
			{
				f_dht_error = GET_DATA_ERROR;
				return;
			}
		}
		
			
		//re-init counter
		//TCNT1 = 0;
		timer1_reset_count();
			
		//wait while the line is low
		while( !getPinState(DHT_PIN) )
		{
			if(timer1_get_count()>TIMEOUT)
			{
				f_dht_error = GET_DATA_ERROR;
				return;
			}
			
		}
		timer_val = timer1_get_count();				
		//check the counter values
		if( (75<=timer_val) && (90>=timer_val))
		{
			//TCNT1=0;
			timer1_reset_count();
			timer_val = 0;
			//we have to read to pulses, so before continuing in the state machine, we repeat for the second pulse
			_v_response_pulses++;
				
			//restart count
				
			//now we wait while the line is high
			while( getPinState(DHT_PIN))
			{
				if(timer1_get_count()>TIMEOUT)
				{
					f_dht_error = GET_DATA_ERROR;
					return;
				}
				
			}	
			timer_val = timer1_get_count();	
			if( (75<=timer_val) && (90>=timer_val))
			{
				_v_response_pulses++;
			}
			else
			{					
				f_dht_error = RESPONSE_ERROR;
			}
				
						
		}
		else
		{
			f_dht_error = RESPONSE_ERROR;
		}
}


/*	
	@fn:  dht_gets_values
	@brief: reads the values from the sensor
	@params: none
	@returns: none
*/
void dht_gets_data()
{
	uint8_t timer_value = 0;
	while(_v_dht_data_shift_index>0)
	{
		//decrement index
		_v_dht_data_shift_index--;	
		
		timer1_reset_count();
		timer_value = 0;		
		//wait while the line is low
		while( !getPinState(DHT_PIN) )
		{
			if(timer1_get_count()>TIMEOUT)				
			{
				f_dht_error = GET_DATA_ERROR; 
				return;
			}
		}
			
		//restart counter
		//TCNT1 = 0;
		timer1_reset_count();
		timer_value = 0;
		//now we wait while the line is high
		while(getPinState(DHT_PIN))
		{
			if(timer1_get_count()>TIMEOUT)
			{
				f_dht_error = GET_DATA_ERROR;
				return;
			}
		}
		timer_value = timer1_get_count();
		if(_v_dht_data_shift_index>7)
		{
			
			//we only check for the ones (given the values variables are started at 0
			if(timer_value>=45)
			{
				_v_dht_data |= ((uint32_t)1<<(_v_dht_data_shift_index-8));
			}			
			
		}
		else{
			
			//we only check for the ones (given the values variables are started at 0
			if(timer_value>=45)
			{
				_v_dht_chk |= (1<<_v_dht_data_shift_index);
			}			
		}
		

					
	}

	
}

/*	
	@fn:  dht_read
	@brief: updates the temp and rh values with new sensor data
	@params: none
	@returns: none
*/
uint8_t dht_read()
{
	//dht_start();
	
	switch(dht_state)
	{
		case dht_idle:
			
			//initialize variables
			_v_dht_rh_int = 0;
			_v_dht_rh_dec = 0;
			_v_dht_temp_int = 0;
			_v_dht_temp_dec = 0;
			_v_dht_data = 0;
			_v_dht_data_ready = 0;
			
			//
			attiny_dht_init();
			//start timer
			timer1_init();
			
			//star FSM
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
				//while( !(PINB & (0x01 << DHT_PIN)) );
				//inits Pin Change Interrupts
				//init_pcint();
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
			dht_gets_data();
			if(f_dht_error!=NO_ERROR)
			{
				dht_state = dht_error;
			}
			else
			{
				dht_state = dht_stop;	
			}
			break;
		
		//check for stop signal
		case dht_stop:
			
			//PORTB|=(0x01<<PB4); 
			_v_dht_temp_dec = (_v_dht_data & 0xFF);
			_v_dht_temp_int = ((_v_dht_data>>8) & 0xFF);
			_v_dht_rh_dec = ((_v_dht_data>>16) & 0xFF);
			_v_dht_rh_int = ((_v_dht_data>>24) & 0xFF);
			
			if(_v_dht_chk==((_v_dht_temp_dec+_v_dht_temp_int+_v_dht_rh_dec+_v_dht_rh_int) & 0xFF))
			{
				_v_dht_data_ready = 1;
			}

			
			deinit_pcint();
			timer1_deinit();
			dht_state = dht_idle;
			return DHT_OK_STATUS;
			break;
		
		case dht_error:
			
			deinit_pcint();
			timer1_deinit();
			return GET_DATA_ERROR;
			break;
		default:
		return GET_DATA_ERROR;
			break; 
	}
	return GET_DATA_ERROR;
}



uint8_t dht_read_2()
{
	//dht_start();

	//initialize variables
	_v_dht_rh_int = 0;
	_v_dht_rh_dec = 0;
	_v_dht_temp_int = 0;
	_v_dht_temp_dec = 0;
	_v_dht_data = 0;
	_v_dht_data_ready = 0;
	f_dht_error = NO_ERROR;
	_v_dht_chk = 0; 
	_v_dht_data_shift_index = 40;
	_v_response_pulses = 0;
	
	setPinDir(DHT_PIN,OUTPUT);
	setPin(DHT_PIN,HIGH);
	
	//
	//attiny_dht_init();
	//start timer
	timer1_init();
	
	//star FSM
	//sends start signal
	dht_start();
	if(f_dht_error!=NO_ERROR)
	{
		return GET_DATA_ERROR;
	}

	
	//reads rh and temp values
	dht_response();
	if(f_dht_error!=NO_ERROR)
	{
		return GET_DATA_ERROR;
	}


	dht_gets_data();
	if(f_dht_error!=NO_ERROR)
	{
		return GET_DATA_ERROR;
	}

	
	//check for stop signal
	//deinit_pcint();
	timer1_deinit();

	
	//PORTB|=(0x01<<PB4);
	_v_dht_temp_dec = (_v_dht_data & 0xFF);
	_v_dht_temp_int = ((_v_dht_data>>8) & 0xFF);
	_v_dht_rh_dec = ((_v_dht_data>>16) & 0xFF);
	_v_dht_rh_int = ((_v_dht_data>>24) & 0xFF);
	
	if(_v_dht_chk==((_v_dht_temp_dec+_v_dht_temp_int+_v_dht_rh_dec+_v_dht_rh_int) & 0xFF))
	{
		return DHT_OK_STATUS;	//_v_dht_data_ready = 1;
	}
	else
	{
		return GET_DATA_ERROR;	//_v_dht_data_ready = 1;
	}
	
}
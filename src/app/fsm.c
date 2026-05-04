/*
 * fsm.c
 *
 * Created: 28/4/2026 10:08:14
 *  Author: IESz
 */ 

#include "fsm.h"

//global variables
volatile uint8_t _v_init_sequencer_counter = 0;		//keeps track of init sequence
volatile uint8_t _v_init_sequencer_ticks = 0;		//keeps track of the measures taken on init cycle
volatile uint8_t _v_dht_status = DHT_WORKING;		//keeps track of the actual state of the DHT after a reading operation
volatile uint8_t _v_measures = 0;					//keeps track of the measures taken on a cycle

volatile uint8_t _v_general_app_state = INIT;
volatile uint8_t init_app_state = STARTUP;
volatile uint8_t normal_app_state = UPDATE_RESULT;	//we start in UPDATE_RESULT to average and print the first result, obtained in the INIT_FSM

extern volatile uint16_t _v_debounced_temp_int;
extern volatile uint16_t _v_debounced_temp_dec;
extern volatile uint16_t _v_debounced_rh_int;
extern volatile uint16_t _v_debounced_rh_dec;

/*
	@fn:		init_fsm
	@brief:		implements the initial FSM
	@param:		None
	@return:	None
*/
void init_fsm()
{
	switch(init_app_state)
	{
		
		case STARTUP:

		//in this state we initialize the diplay
		timer0_start();
		oled_on();
		oled_full_on();
		oled_clean(standar_mode);
		
		oled_print_text("MEASURING",3,32);
		attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
		
		init_app_state = REFRESH_SCREEN;
		case REFRESH_SCREEN:
		GLED_OFF;
		//state that implements the initial screen "animation", so the user knows the program is running
		if(_v_init_sequencer_counter==0)
		{
			_v_init_sequencer_counter+=1;
			oled_print_text("MEASURING.  ",3,32);
		}
		else if(_v_init_sequencer_counter==1)
		{
			_v_init_sequencer_counter+=1;
			oled_print_text("MEASURING.. ",3,32);
		}
		else if(_v_init_sequencer_counter==2)
		{
			oled_print_text("MEASURING...",3,32);
			_v_init_sequencer_counter=0;
		}
		init_app_state = MEASURE;
		
		break;
		case MEASURE:
		
		//in this state we first wait INIT_MEAS_TIME then, if we have already waited 5 times for that time, we take a measure.
		//	Otherwise, we go back to the REFRESH_SCREEN state an update the screen
		_delay_ms(INIT_MEAS_TIME);
		_v_init_sequencer_ticks++;
		if(_v_init_sequencer_ticks<REFRESH_SCREEN_RATE)
		{
			//we go back to refresh the screen
			init_app_state = REFRESH_SCREEN;
			break;
		}
		else if (_v_init_sequencer_ticks>=5)
		{
			_v_init_sequencer_ticks = 0;
			//we need to take a measure, then, if finished the measuring process, we exit the "init fsm" an go to the "normal operation mode"
			
			GLED_ON;
			//stop the timer0 interrupts while communicating with the DHT
			timer0_interrupt_disable();
			//update the temperature and humidity values
			_v_dht_status = getMeasures();
			//increment the measures counter
			_v_measures+=1;
			//restore the timer0 interrupts
			timer0_interrupt_enable();
			
			//if the reading was OK and we reached the quantity of measures, we exit the init_fsm
			if ((_v_dht_status==DHT_OK_STATUS) &&(_v_measures>=MEASURES))
			{
				_v_general_app_state = NORMAL_OPERATON;
			}
			else if ((_v_dht_status==DHT_OK_STATUS) &&(_v_measures<MEASURES))
			{
				//if the reading was OK, but we didn't reach the quantity of measures yet, we go back to the REFRESH_SCREEN state
				_v_dht_status = DHT_WORKING;
				init_app_state = REFRESH_SCREEN;
				
			}
			else if(_v_dht_status!=DHT_OK_STATUS)
			{
				//if something went wrong while reading the DHT
				_v_measures = 0;
				_v_dht_status = DHT_WORKING;
				init_app_state = REFRESH_SCREEN;
				//TODO: take actions
			}
			
			break;
		}
		
		break;
		default:
		break;
		
	}
	
}

/*
	@fn:		normal_fsm
	@brief:		implements the normal operation FSM
	@param:		None
	@return:	None
*/
void normal_fsm()
{
	switch(normal_app_state)
	{
		case IDLE:
			//TODO: in this state we must go to a power saving state between readings
			GLED_OFF;
			//wait for the time between readings to pass
			_delay_ms(MEAS_TIME);
			//switch to the reading state
			normal_app_state = MEASURING;
			break;
		
		
		case MEASURING:
			GLED_ON;
			//stop the timer0 interrupts while communicating with the DHT
			timer0_interrupt_disable();
			//update the temperature and humidity values
			_v_dht_status = getMeasures();
			//increment the measures counter
			_v_measures+=1;
			//restore the timer0 interrupts
			timer0_interrupt_enable();
		
			//if the reading was OK and we reached the quantity of measures, we update the result on the screen
			if ((_v_dht_status==DHT_OK_STATUS) &&(_v_measures>=MEASURES))
			{
				normal_app_state = UPDATE_RESULT;
			}
			else if ((_v_dht_status==DHT_OK_STATUS) &&(_v_measures<MEASURES))
			{
				//if the reading was OK, but we didn't reach the quantity of measures yet, we go back to the IDLE state
				_v_dht_status = DHT_WORKING;
				normal_app_state = IDLE;
			
			}
			else if(_v_dht_status!=DHT_OK_STATUS)
			{
				//if something went wrong while reading the DHT
				_v_measures = 0;
				_v_dht_status = DHT_WORKING;
				normal_app_state = IDLE;
				//TODO: take actions
			}
			break;
		
		case UPDATE_RESULT:
			//average all the measures taken
			average_measures(_v_measures);
			_v_measures = 0; //reset the measures counter

			oled_on();
		
			//full-on display
			oled_full_on();
		
			//display sleep mode
			//attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			oled_clean(standar_mode);
		
			//write text
			//oled_print_text("TEMP: 0°C",2,32);
			print_temperature(_v_debounced_temp_int,_v_debounced_temp_dec);
			print_humidity(_v_debounced_rh_int,_v_debounced_rh_dec);
			//oled_print_text("HUM:  70%",5,32);
			//oled_draw_weather(sunny,2,32);
			//draw_thermometer(_v_debounced_temp_int);
		
			//full-on display (using gdram)
			oled_show_data();
		
			//clean all the averaged values
			reset_measures();
		
			//set state to IDLE
			normal_app_state = IDLE;
			break;
		default:
		break;
		
	}
}


/*	TODO
	@fn		f_update_dht_values
	@brief:	triggers a read of the dht sensor and updates the measured values
	@param none
	@returns: 
	
*/
//uint8_t dht_fsm()
//{
	
	
//}


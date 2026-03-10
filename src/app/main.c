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
#include <string.h>


//includes
#include "i2c.h"
#include "oled.h"
#include "dht_primitive.h"
#include "gpio_driver.h"
#include "timer0_primitive.h"

//Defines
#define MEASURES 10

enum FSM_STATES{IDLE,PRINT,READ,INIT};
//Macros


//Global variables
volatile uint8_t app_state = INIT;

extern volatile uint8_t usi_state;	//defined in i2c_driver.c
extern volatile uint8_t f_nack;		//defined in i2c_driver.c


extern volatile uint16_t _v_debounced_temp_int;
extern volatile uint16_t _v_debounced_temp_dec;
extern volatile uint16_t _v_debounced_rh_int;
extern volatile uint16_t _v_debounced_rh_dec;





/*
	Attiny Init
*/
void attiny_init()
{
	//init Timer 0
	//attiny_timer_init();
	//init USI 
	attiny_i2c_init();
	
	app_state = INIT;		
	// enable interrupts 
	sei();

	attiny_dht_init();	
	//Test LED
	//DDRB |= (0x01<<LED); //LED as Out
	//PORTB&=~(0x01<<LED); //LED off
	setPinDir(LED,OUTPUT);
	setPin(LED,LOW);
	
	setPinDir(RLED,OUTPUT);
	setPin(RLED,LOW);
	
	//setPinDir(PB4,OUTPUT);
	//setPin(PB4,LOW);
	
}







int main(void)
{
	
	//Init ATtiny
	//char	a = 1;
	//char	b = 0;
	//uint8_t init_oled = 0;
	uint8_t dht_status = DHT_WORKING;
	uint8_t measures = 0;
	app_state = INIT;	
	
	_delay_ms(2000);
	
	attiny_init();

	//attiny_dht_init();
    while (1) 
    {
		
		
		switch(app_state)
		{
			case IDLE:
				//in idle state, we just wait to measure again
				GLED_OFF;
				_delay_ms(5000);
				app_state = READ;
				break;
			case READ:
				GLED_ON;
				timer0_interrupt_disable();
				//attiny_timer_deinit();
				
				//dht_status = dht_read();
				dht_status = getMeasures();
				measures+=1;
				timer0_interrupt_enable();
				if ((dht_status==DHT_OK_STATUS) &&(measures>=MEASURES))
				{
					average_measures(measures);
					measures = 0;
								
					dht_status = DHT_WORKING;
					app_state = PRINT;
				}
				else if ((dht_status==DHT_OK_STATUS) &&(measures<MEASURES))
				{
					dht_status = DHT_WORKING;
					app_state = IDLE;
					
				}
				else if(dht_status!=DHT_OK_STATUS)
				{
					measures = 0;
					dht_status = DHT_WORKING;
					app_state = IDLE;
					//TODO: take actions
				}
			
				break;
			case PRINT:
				
				

				//attiny_timer_init();
				
				//attiny_i2c_tx();
				//turn on display
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
			
			
				//full-on display (using gdram)
				oled_show_data();
				
				reset_measures();
				
				
				//init_oled = 0;
				
				//set state to IDLE
				app_state = IDLE;
				
				break;
			case INIT:
				timer0_start();
				oled_on();
				oled_full_on();
				oled_clean(standar_mode);
				
				oled_print_text("MEASURING...",3,32);
				attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
				app_state = READ;
			default:
				app_state = IDLE;
				break;
			
		}
		
		
		/*if(b==0)
		{	
			
			dht_read();
			if(_v_dht_data_ready)
			{
				
				a=0;	
				b=1;
				
			}
			
		}
		
		if (a==0)
		{
			GLED_ON;
			if(init_oled==0)
			{
				attiny_init();
				init_oled = 1;
				
			}
			
			
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
			print_temperature(_v_dht_temp_int,_v_dht_temp_dec);
			print_humidity(_v_dht_rh_int,_v_dht_rh_dec);
			//oled_print_text("HUM:  70%",5,32);
			//oled_draw_weather(sunny,2,32);
			
			
			//full-on display (using gdram)
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			
			//attiny_timer_deinit();

			a=1;
		}
		
		if(f_nack)
		{
			f_nack = 0;
			//
		}*/
	
	}
}



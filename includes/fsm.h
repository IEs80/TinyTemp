/*
 * fsm.h
 *
 * Created: 28/4/2026 10:12:14
 *  Author: IESz
 */ 

#include <avr/io.h>
#include <avr/pgmspace.h>
#include <stdlib.h>
#include "dht.h"
#include "timer0_primitive.h"
#include "i2c.h"
#include "oled.h"
#include "dht_primitive.h"
#include "gpio_driver.h"


//Defines
#define MEASURES 10				//measures to be averaged
#define INIT_MEAS_TIME 200		//time between measures in the init
#define REFRESH_SCREEN_RATE 5
#define MEAS_TIME	5000		//time between measures in normal operation

//states of each of the state machines that we have
enum GENERAL_FSM_STATES{INIT,NORMAL_OPERATON};
enum INIT_FSM_STATES{STARTUP,REFRESH_SCREEN,MEASURE,AVERAGE};
enum NORMAL_FSM_STATES{IDLE,UPDATE_RESULT,MEASURING};
	
	
/*
	@fn:		init_fsm
	@brief:		implements the initial FSM
	@param:		None
	@return:	None
*/
void init_fsm();


/*
	@fn:		normal_fsm
	@brief:		implements the normal operation FSM
	@param:		None
	@return:	None
*/
void normal_fsm();


/*
	@fn		dht_fsm
	@brief:	implements the internal DHT measuring FSM
	@param none
	@returns: 
	
*/
//uint8_t dht_fsm();
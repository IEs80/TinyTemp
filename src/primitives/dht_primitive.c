/*
 * dht_primitive.c
 *
 * Created: 3/3/2026 18:52:54
 *  Author: IESz
 */ 

#include "dht_primitive.h"

uint16_t _v_debounced_temp_int = 0;
uint16_t _v_debounced_temp_dec = 0;
uint16_t _v_debounced_rh_int	  = 0;
uint16_t _v_debounced_rh_dec   = 0;

extern volatile uint8_t	_v_dht_temp_int;
extern volatile uint8_t	_v_dht_temp_dec;
extern volatile uint8_t	_v_dht_rh_int;
extern volatile uint8_t	_v_dht_rh_dec;

/*	
	@fn:  getMeasures
	@brief: gets DHT sensor measures
	@params: none
	@returns: DHT state
*/
uint8_t getMeasures()
{
	uint8_t state = 0;
	
	state = dht_read();
	
	_v_debounced_temp_int += _v_dht_temp_int;
	_v_debounced_temp_dec += _v_dht_temp_dec;
	_v_debounced_rh_int   += _v_dht_rh_int;
	_v_debounced_rh_dec   += _v_dht_rh_int;
	
	return state;
}

/*	
	@fn:  average_measures
	@brief: averages the measures obtained of subsequent sensor readings
	@params: measures takes
	@returns: None
*/
void average_measures(uint8_t averager)
{
	_v_debounced_temp_int = _v_debounced_temp_int/averager;
	_v_debounced_temp_dec = _v_debounced_temp_dec/averager;
	_v_debounced_rh_int   = _v_debounced_rh_int/averager;
	_v_debounced_rh_dec   = _v_debounced_rh_dec/averager;
	
	if(_v_debounced_rh_dec>=10)
	{
		_v_debounced_rh_dec%=10;
		_v_debounced_rh_int+=1;
	}
	
		if(_v_debounced_temp_dec>=10)
		{
			_v_debounced_temp_dec%=10;
			_v_debounced_temp_int+=1;
		}
}

/*	
	@fn:  reset_measures
	@brief: resets all the measure variables, to get them set-up for the next reading phase
	@params: None
	@returns: None
*/
void reset_measures()
{
	
	_v_debounced_temp_int = 0;
	_v_debounced_temp_dec =	0;
	_v_debounced_rh_int   =	0;
	_v_debounced_rh_dec   =	0;
}
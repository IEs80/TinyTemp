/*
 * gpio_driver.c
 *
 * Created: 3/3/2026 18:37:36
 *  Author: IESz
 */ 

#include "gpio_driver.h"

/*	
	@fn:  setPin
	@brief: sets pin in the desired state
	@params: pin: pin to set; state: state for the pin to ve seted
	@returns: none
*/
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
	@fn:  setPinDir
	@brief: sets direction of the desired pin
	@params: pin: pin to set, dir: desired direction for the pin
	@returns: none
*/
void setPinDir(uint8_t pin, uint8_t dir)
{
	if(dir==INPUT)
	{
		DDRB &= ~(0x01<<pin);
	}
	else if(dir==OUTPUT){
		DDRB |= (0x01<<pin);
	}
	
}

/*	
	@fn:  getPinState
	@brief: returns a given pin state 
	@params: pin to be read
	@returns: 0x01 if pin is "H" or 0x00 if pin is "L"
*/
uint8_t getPinState(uint8_t pin)
{
	return (PINB & (0x01 << pin));
}
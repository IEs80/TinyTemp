/*
 * gpio_driver.h
 *
 * Created: 3/3/2026 18:37:56
 *  Author: IESz
 */ 
#include <avr/io.h>

//defines
#define INPUT	1
#define OUTPUT	0

#define HIGH	1
#define LOW		0

#define	 DHT_PIN PB3 



//Functions
void setPin(uint8_t pin,uint8_t state);

void setPinDir(uint8_t pin, uint8_t dir);

uint8_t getPinState(uint8_t pin);

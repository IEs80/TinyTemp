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

#define GLED_ON     PORTB|=(0x01<<LED) //LED on
#define GLED_OFF    PORTB&=~(0x01<<LED) //LED on
#define YLED_ON     PORTB|=(0x01<<LED) //LED on
#define RLED_ON     PORTB|=(0x01<<LED) //LED on

#define LED  PB4
#define RLED PB5


//Functions
void setPin(uint8_t pin,uint8_t state);

void setPinDir(uint8_t pin, uint8_t dir);

uint8_t getPinState(uint8_t pin);

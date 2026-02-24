/*
 * dht.h
 *
 * Created: 22/2/2026 11:46:10
 *  Author: IESz
 */ 

#ifndef DHT_H_
#define DHT_H_
#define F_CPU 8000000UL

#include <avr/io.h>

//defines
#define DHT_PIN PB3 

//macros

//Function prototypes
void attiny_dht_init();
void timer1_init();
void dht_start();
#endif /* DHT_H_ */
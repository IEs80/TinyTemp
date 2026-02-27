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
#include <avr/interrupt.h>

//defines
#define DHT_PIN PB3 
#define HIGH 1
#define LOW 0


enum DHT_ERROR_FLAGS{NO_ERROR,START_ERROR,COM_ERROR};
enum {dht_idle,dht_send_start,dht_read_response,dht_read_bits,dht_stop,dht_error};
//macros

//Function prototypes
void attiny_dht_init();
void timer1_init();
void dht_start();
#endif /* DHT_H_ */
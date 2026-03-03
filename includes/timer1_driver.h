/*
 * timer1_driver.h
 *
 * Created: 3/3/2026 19:05:50
 *  Author: IESz
 */ 

#include <avr/io.h>

//Function prototypes
void timer1_init();
void timer1_deinit();
void timer1_set_prescaler(uint16_t);
void timer1_reset_count();
uint8_t timer1_get_count();
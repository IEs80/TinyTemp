/*
 * timer0_promitives.c
 *
 * Created: 10/3/2026 09:11:30
 *  Author: IESz
 */ 

#include "timer0_primitive.h"



void timer0_start()
{
	attiny_timer_init();
}


void timer0_stop()
{
	attiny_timer_deinit();
}

void timer0_interrupt_enable()
{
	attiny_timer_interrupt_enable();
}

void timer0_interrupt_disable()
{
	attiny_timer_interrupt_disable();
}

/*
 * timer0_driver.h
 *
 * Created: 10/3/2026 09:07:09
 *  Author: IESz
 */ 




//Function prototypes
void attiny_timer_init(void);

void attiny_timer_deinit(void);

void attiny_timer_interrupt_enable();

void attiny_timer_interrupt_disable();
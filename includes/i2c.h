/*
 * i2c.h
 *
 * Created: 13/1/2026 18:24:06
 *  Author: IESz
 */ 


#ifndef I2C_H_
#define I2C_H_

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//defines
#define SDA PB0
#define SCL PB2

enum {usi_idle,usi_byte_sent,usi_address_sent,usi_nack,usi_read_ack};

#define I2C_BUFF_LEN 3
#define OLED_ADDR_W	0x78 //Device Address: 0x3C. Write: 0x78 | Read:

//macros
#define TWI_DELAY() _delay_us(5);


#define SDA_LOW		PORTB &= ~(0x01<<SDA);
#define SDA_HIGH	PORTB |= (0x01<<SDA);

#define SCL_LOW		PORTB &= ~(0x01<<SCL);
#define SCL_HIGH	PORTB |=  (0x01<<SCL);

//Global variables
//extern volatile uint8_t usi_status = 0x00;
//extern volatile uint8_t usi_state = usi_idle;
//extern volatile uint8_t i2c_buff[I2C_BUFF_LEN];
//extern volatile uint8_t i2c_indx = 0;
//extern volatile uint8_t f_nack = 0;

//Function prototypes
void attiny_i2c_init(void);
void attiny_init(void);
void attiny_i2c_tx(void);
void attiny_i2c_send_byte(char addr, char reg, char data);
#endif /* I2C_H_ */
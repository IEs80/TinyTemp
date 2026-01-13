/*
 * GccApplication1.c
 *
 * Created: 10/1/2026 17:07:28
 * Author : IESz
 */ 
#define F_CPU 8000000UL

#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>

//Defines

#define SDA PB0
#define SCL PB2
#define LED PB4

enum {usi_idle,usi_byte_sent,usi_address_sent,usi_nack,usi_read_ack};

#define I2C_BUFF_LEN 3
#define OLED_ADDR_W	0x78 //Device Address: 0x3C. Write: 0x78 | Read: 
	
//Macros
#define TWI_DELAY() _delay_us(5);
#define LED_ON     PORTB|=(0x01<<LED) //LED on

#define SDA_LOW		PORTB &= ~(0x01<<SDA);
#define SDA_HIGH	PORTB |= (0x01<<SDA);

#define SCL_LOW		PORTB &= ~(0x01<<SCL);
#define SCL_HIGH	PORTB |=  (0x01<<SCL);

//Global variables
volatile uint8_t usi_status = 0x00;
volatile uint8_t usi_state = usi_idle;
static char a = 0;

volatile uint8_t i2c_buff[I2C_BUFF_LEN];
volatile uint8_t i2c_indx = 0;

volatile uint8_t f_nack = 0;

//Function prototypes
void attiny_timer_init(void);
void attiny_i2c_init(void);
void attiny_init(void);
void attiny_i2c_tx(void);
void attiny_i2c_send_byte(char addr, char reg, char data);

//Function definitions
void attiny_timer_init()
{
	//we need Timer0 Compare Match
	//note that frec_scl = fclk/2 => frec_scl = 100KHz , fclk = 200KHz


	//Select the "compare match" mode
	
	TCCR0A &= ~(0x01 << 0); //write 0 to WGM00
	TCCR0A |= (0x01 << 1);  //write 1 to WGM01
	TCCR0B &= ~(0x01 << 3); //write 0 to WGM02

	//set the TOP value for the counter
	OCR0A = 0x27;
	//OCR0A = 0x05;

	//set the prescaler to 1
	TCCR0B = 0x01;

	//set the count to 0
	TCNT0 = 0x00;

	//habilita TIMER0 COMPA interrupt
	TIMSK |= (1 << OCIE0A);
}

/*
  Register configuration for the USI in Two-Wire mode
*/
void attiny_i2c_init()
{

  //activate PULL-UPS for SDA and SCL
  
  /*it's not correct to set as input and active pull-up
  DDRB &= ~(0x01<<SDA);
  DDRB &= ~(0x01<<SCL);
  PORTB |= (1 << SDA) | (1 << SCL);
  */
  

/*
Para liberar:  DDRB = 0 (entrada) y PORTB = 0 (la línea se va a alto)
Para conducir: DDRB = 1 (salida)  y PORTB = 0 (la línea se va a bajo)
*/

  //Free SDA and SCL lines
  DDRB &= ~(0x01<<SDA);
  DDRB &= ~(0x01<<SCL);
  //Write a 0 to release the lines (lines HIGH)
  //SDA High
  PORTB &= ~(0x01<<SDA);
  //SCL High
  PORTB &= ~(0x01<<SCL);

  //two-wiere mode for USI
  //Selecting the USI in two-wiere mode, it will automatically take control of PB0 and PB2, and switch them to the desired functionality of SCL and SDA
  /*
  USICR
  b7:   Start condition Interrupt Enable
  b6:   Counter Overflow Interrupt Enable
  b5-4: Wire Mode (10 = Two Wire)
  b3-2: Clock Source Select (01 = Timer 0 (config del timer) | 00 = Software clock strobe (USICLK))
  b1: Clock Strobe (1 = Software clock strobe (USICLK))
  b0: 0
  */
  USICR = 0x68; 


  /*
    The USI is being configured with "external clock source". So, it's "watching" the USITC bit as if it's an external clock source. 
      With the Timer0 compare match, we generate the desired frequency for the communication. Then in the ISR we write a 1 to the USITC, so 
      the module interprets it as a new edge of the clock an it shifts USIDR and toogles SCL. That way SDA and SCL are synched

  */

}

/*
	Initializes I2C Tx
*/
void attiny_i2c_tx ()
{
	//set the
	//uint8_t i2c_data = 0x78; //0xC4; //(0x62<<1) | 0x00;


	
	//Free SDA and SCL lines
	DDRB &= ~(0x01<<SDA);
	DDRB &= ~(0x01<<SCL);
	//Write a 0 to release the lines (lines HIGH)
	//SDA High
	PORTB &= ~(0x01<<SDA);
	//SCL High
	PORTB &= ~(0x01<<SCL);

	
	//generate start condition: SDA low, SCL High
	DDRB |= (1<<SDA); //SDA as Output
	PORTB &= ~(0x01<<SDA);   //SDA Low
	TWI_DELAY();
	DDRB |= (0x01 << SCL); //set SCL as output so it can toogle
	TWI_DELAY();

	// Free SDA so the USI takes control of the line
	//DDRB &= ~(1<<SDA);
	PORTB |= (0x01<<SDA);

	//set the address to be tx
	USIDR = i2c_buff[i2c_indx];//i2c_data;
	//advance i2c index
	i2c_indx+=1;
	//clean flag counter OV and restart counter to start transmission
	USISR = (1 << USISIF) | (1 << USIOIF) | (0x00 << USICNT3);

	usi_state = usi_byte_sent;

	
}

/*
	Attiny Init
*/
void attiny_init()
{
	  //init Timer 0
	  attiny_timer_init();
	  //init USI 
	  attiny_i2c_init();

	  // enable interrupts 
	  sei();


	  //initialize usi STATE
	  usi_state = usi_idle;


	  //Test LED
	  DDRB |= (0x01<<LED); //LED as Out
	  PORTB&=~(0x01<<LED); //LED off
}


/*	
	@fn: void attiny_i2c_send_byte
	@params:
		addr: device address
		register: register to be addressed
		data: byte to be transmitted
	@returns: none
*/
void attiny_i2c_send_byte(char addr, char reg, char data)
{
	//TODO: change blocking wait
	while(usi_state!=usi_idle);
	//reset i2c index
	i2c_indx = 0;
	//load i2c data buffer
	i2c_buff[0]=addr;
	i2c_buff[1]=reg;
	i2c_buff[2]=data;
	//start Tx
	attiny_i2c_tx();
}

//ISRs
ISR(TIMER0_COMPA_vect)
{
	//  USICR |= (1<<USITC);
	
	//clean the interrupt flag ()
	TIFR |= (1<<OCF0A);
	
	if(usi_state!=usi_idle)
	{
		
		USICR |= (1<<USITC);   // un tick del USI
	}

	//set the count to 0
	TCNT0 = 0x00;
}

ISR(USI_OVF_vect)
{

	usi_status = USISR;

	//USI counter OVF
	if(((usi_status&0x40)>>6)==1)
	{
		//clean USI counter OVF flag
		USISR = (1 << USIOIF);

	}
	
	
	//USI counter SIF
	if(((usi_status&0x80)>>7)==1)
	{
		//clean USI counter OVF flag
		USISR = (1 << USISIF);
	}

	//I2C FSM
	switch (usi_state)
	{
		
		//idle state: nothing to do, keep there
		case usi_idle:
			usi_state = usi_idle;
			break;
		//Tx started and address+w/r sent	
		case usi_byte_sent:
			
		
			//we sent the start and address, now we prepare to read de ACK
			USISR = (USISR&0xF0)|(0x0E); //set the counter at 14, to read 1 bit of ack
			DDRB&=~(0x01<<SDA); //set SDA as input...
			PORTB&=~(0x01<<SDA); //an release the line
			
			
			usi_state = usi_read_ack;
		

			break;

		case usi_read_ack:
			//check of ACK of the device
			if(!(USIDR&0x01))
			{
				//check if there is available data in the buffer. Else, exit
				if (i2c_indx<I2C_BUFF_LEN)
				{
					

					//pop next data from buffer
					USIDR = i2c_buff[i2c_indx];
					//advance i2c index
					i2c_indx+=1;					
					//ACK
					//prepare to send next byte
					PORTB |= (0x01<<SDA);
					DDRB |= (1<<SDA); //SDA as Output
					// Free SDA so the USI takes control of the line
					//DDRB &= ~(1<<SDA);
					
					//set counter to original value
					USISR = (1 << USISIF) | (1 << USIOIF) | (0x00 << USICNT3);
									
					usi_state = usi_byte_sent;
				}
				else
				{
					//if we reicived ACK and there is not another byte to Tx, then STOP sequence
					
					//SCL HIGH and SDA TRANSITIONS TO HIGH
					PORTB &= ~(0x01<<SDA);
					DDRB |= (1<<SDA); //SDA as Output
								
					//Free SCL line (goes to high)
					DDRB &= ~(0x01<<SCL);
					PORTB &=~(0x01<<SCL);
					TWI_DELAY()
					//Free SDA line (goes to high)
					DDRB &= ~(0x01<<SDA);
					PORTB &=~(0x01<<SDA);
					usi_state = usi_idle;
				}

			}
			else
			{
				//NACK
				usi_state = usi_nack;
			}

			break;
			
		case usi_nack:
			f_nack = 1; //to be handled outside the ISR
			
			//STOP TX
			//SCL HIGH and SDA TRANSITIONS TO HIGH
			PORTB &= ~(0x01<<SDA);
			DDRB |= (1<<SDA); //SDA as Output
								
			//Free SCL line (goes to high)
			DDRB &= ~(0x01<<SCL);
			PORTB &=~(0x01<<SCL);
			TWI_DELAY()
			//Free SDA line (goes to high)
			DDRB &= ~(0x01<<SDA);
			PORTB &=~(0x01<<SDA);
			
			usi_state = usi_idle;
			break;
		default:
			usi_state = usi_idle;
			break;
	}




}


int main(void)
{
	//Init ATtiny
	attiny_init();
	
    /* Replace with your application code */
    while (1) 
    {
		if (a==0)
		{
			//attiny_i2c_tx();
			//turn on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xAF);
			//full-on display
			attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			//display sleep mode
			//attiny_i2c_send_byte(OLED_ADDR_W,0x00,0xA4);
			
			a=1;
		}
		
		if(f_nack)
		{
			f_nack = 0;
			LED_ON;
		}
	
	}
}


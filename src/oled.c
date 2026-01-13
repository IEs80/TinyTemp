/*
 * oled.c
 *
 * Created: 12/1/2026 23:45:12
 *  Author: IESz
 */ 


#include "oled.h"


//Function prototypes
void set_shift(uint8_t *);


/*
	@fn: set_page
	@params: page numer (0 to 7)
	@returns: -
	
*/
void set_page(uint8_t page)
{
	//0xb0 - 0xb7
	if(page >= 0 && page <=7)
	{
		attiny_i2c_send_byte(OLED_ADDR_W,OLED_CONTROL_BYTE,OLED_SET_PAG_ADDR);	
	}
	
	
}
//void set_column(uint8_t column);
//
//void set_row(uint8_t row);
//
//void set_addr_mode(uint8_t mode);
//
//void set_addr_mode(uint8_t mode);

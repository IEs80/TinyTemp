/*
 * oled.c
 *
 * Created: 12/1/2026 23:45:12
 *  Author: IESz
 */ 


#include "oled.h"
#include "i2c.h"



//Global variables

// [0] Command: Right Horizontal Scroll
// [1] A: Dummy
// [2] B: Start at Page 0
// [3] C: Slow Speed (256 frames)
// [4] D: End at Page 7 (Bottom of screen)
// [5] E: Dummy
// [6] F: Dummy
volatile uint8_t shift_setup_r[]  = { 0x26, 0x00, 0x00, 0x03, 0x07, 0x00, 0xFF };

// [0] Command: Right Horizontal Scroll
// [1] A: Dummy
// [2] B: Start at Page 0
// [3] C: Slow Speed (256 frames)
// [4] D: End at Page 7 (Bottom of screen)
// [5] E: Vertical Scrolling Offset
volatile uint8_t scroll_setup_r[]  = { 0x29, 0x00, 0x00, 0x03, 0x07, 0x00};






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
		//attiny_i2c_send_byte(OLED_ADDR_W,OLED_CONTROL_BYTE,OLED_CMD_SET_PAG_ADDR,7);	
	}
	
	
}
//void set_column(uint8_t column);
//
//void set_row(uint8_t row);
//
//void set_addr_mode(uint8_t mode);
//
//void set_addr_mode(uint8_t mode);


/*
	@fn:	oled_clean
	@brief:	Set memory all memory to 0 (or 1 if inverted mode is selected)
	@param: 
		mode: defines if device is in standard or inverted mode
	@return:
*/
void oled_clean(uint8_t mode)
{
	//We need to send 0s to all the screen
	uint8_t page_index = 0;
	uint8_t colu_index = 0;


	
	for(page_index=0;page_index<OLED_PAGES;page_index++)
	{
			//increase page
			attiny_i2c_send_byte(OLED_ADDR_W,OLED_CONTROL_BYTE,(0xB0|(0x0F&page_index)));
			
			//set col address to 0
			attiny_i2c_send_byte(OLED_ADDR_W,OLED_CONTROL_BYTE,(0x02));
			attiny_i2c_send_byte(OLED_ADDR_W,OLED_CONTROL_BYTE,(0x10));
		
			//write all 00s to memory
			for(colu_index=0;colu_index<OLED_WIDTH;colu_index++)
			{
				attiny_i2c_send_byte(OLED_ADDR_W,OLED_GDDRAMW_BYTE,0x00);
			}
			

			TWI_DELAY(); //it needs to be addedd
		
	}
	
	

}
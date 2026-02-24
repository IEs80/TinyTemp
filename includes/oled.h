/*
 * oled.h
 *
 * Created: 12/1/2026 23:45:53
 *  Author: IESz
 * brief: SSH1106 oled utility functions
 */ 
#include <avr/io.h>
#include <avr/pgmspace.h>
#include "string.h"


#ifndef OLED_H_
#define OLED_H_

//Defines

#ifndef	OLED_ADDR_W
	#define OLED_ADDR_W 0x78 //Device Address: 0x3C. Write: 0x78 | Read: 
#endif //OLED_ADDR_W

#define OLED_WIDTH	128
#define OLED_HIGHT	64
#define OLED_PAGES 8

#define OLED_CONTROL_BYTE 0x00
#define OLED_GDDRAMW_BYTE 0x40 

#define OLED_CMD_WAKEUP			0xAF
#define OLED_CMD_SLEEP			0xAE
#define OLED_CMD_ON				0xA4 //Output follows RAM content
#define OLED_CMD_ON_NORAM		0xA5 //Output ignores RAM content	
#define OLED_CMD_SETCONTRAST	0x81 //double byte cmd

#define OLED_CMD_SHIFT_ON		0x2F
#define OLED_CMD_SHIFT_OFF		0x2E

#define OLED_CMD_SET_ADDR_MODE	0x20
#define OLED_CMD_SET_COL_ADDR	0x21 //vertical or horizontal addressing modes
#define OLED_CMD_SET_PAG_ADDR	0x22 //vertical or horizontal addressing modes

#define FONT_TABLE_OFFSET		32
#define FONT_TABBLE_CHAR_LEN	5

enum {standar_mode,inverted_mode}; 
enum {sunny,cloudy,rainy,snowy};
//Addressing mode


//Macros


//Function prototypes

/*
	@fn:	set_shift
	@brief:	sets the configuration for the horizontal screen shift
		byte order:
					29h/2Ah: vertical and right horizontal /vertical and left horizotnal scroll
					00h
					xxh: start page address (000b-111b)
					xxh: time interval
					xxh: end page address
					xxh: vertical scrolling offset
					
	@param: 
	@return:
	
	
	
*/
void set_shift(uint8_t *);

/*
	@fn:	set_shift
	@brief:	sets the configuration for the horizontal screen shift
		byte order:
					26h/27h: right/left scroll
					00h
					xxh: start page address (000b-111b)
					xxh: time interval
					xxh: end page address
					00h
					ffh
	@param: 
	@return:
	
	
	
*/
void set_shift(uint8_t *);

//0xb0 - 0xb7
void set_page(uint8_t page);

void set_column(uint8_t column);

void set_row(uint8_t row);

void set_addr_mode(uint8_t mode);



/*
	@fn:	oled_clean
	@brief:	Set memory all memory to 0 (or 1 if inverted mode is selected)
	@param: 
		mode: defines if device is in standard or inverted mode
	@return:
*/
void oled_clean(uint8_t mode);


/*
	@fn:	oled_write_byte
	@brief:	writes one byte to OLED GDRAM
	@param: 
		c: byte to be written to GDRAM
	@return:
*/
void oled_write_byte(uint8_t c);


/*
	@fn:	oled_print
	@brief:	prints text on the display
	@param: 
		data:	text to be printed ()
		page:	the page on to write the text
		column: the column on to write the text
	@return:
*/
void oled_print_text(char* data,uint8_t page, uint8_t column);


/*
	@fn:	oled_draw_weather
	@brief:	draws weather icon on the display
	@param: 
		data:	icon to be drawn
		page:	the page on to write the text
		column: the column on to write the text
	@return:
*/
void oled_draw_weather(uint8_t id,uint8_t page, uint8_t column);

//tables
// "PROGMEM" tells the compiler: "Keep this in Flash, do not load into RAM"
extern const uint8_t font5x7[] PROGMEM;

#endif /* OLED_H_ */


/*
	TODO: 
		definir funciones para implementar comandos de seteo de página y columna
		modificar i2c para poder enviar comandos multi-byte (no sólo 3 bytes). Utilizar memcpy 
		
*/
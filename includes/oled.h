/*
 * oled.h
 *
 * Created: 12/1/2026 23:45:53
 *  Author: IESz
 */ 


#ifndef OLED_H_
#define OLED_H_

//Defines

#ifndef	OLED_ADDR_W
	#define OLED_ADDR_W 0x78 //Device Address: 0x3C. Write: 0x78 | Read: 
#endif //OLED_ADDR_W

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


enum {horizontal_addressing,vertical_addressing,page_addressing}; //RESET: page_addressing
//Addressing mode


//Macros


//Global variables
// [0] Command: Right Horizontal Scroll
// [1] A: Dummy
// [2] B: Start at Page 0
// [3] C: Slow Speed (256 frames)
// [4] D: End at Page 7 (Bottom of screen)
// [5] E: Dummy
// [6] F: Dummy
volatile uint8_t shift_setup_r[]  = { 0x26, 0x00, 0x00, 0x03, 0x07, 0x00, 0xFF };


//Global variables
// [0] Command: Right Horizontal Scroll
// [1] A: Dummy
// [2] B: Start at Page 0
// [3] C: Slow Speed (256 frames)
// [4] D: End at Page 7 (Bottom of screen)
// [5] E: Vertical Scrolling Offset
volatile uint8_t scroll_setup_r[]  = { 0x29, 0x00, 0x00, 0x03, 0x07, 0x00};	

volatile uint8_t addressing_mode = page_addressing; //keep track of the selected addressing mode

//Function prototypes
void set_shift(uint8_t *);

//0xb0 - 0xb7
void set_page(uint8_t page);

void set_column(uint8_t column);

void set_row(uint8_t row);

void set_addr_mode(uint8_t mode);

void set_addr_mode(uint8_t mode);

#endif /* OLED_H_ */


/*
	TODO: 
		definir funciones para implementar comandos de seteo de página y columna
		modificar i2c para poder enviar comandos multi-byte (no sólo 3 bytes). Utilzar memcpy 
		
*/
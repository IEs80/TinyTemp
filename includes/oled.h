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
#define OLED_GDDRAMW_BYTE 0xFF //TODO: CHECK!

#define OLED_CMD_WAKEUP			0xAF
#define OLED_CMD_SLEEP			0xAE
#define OLED_CMD_ON				0xA4 //Output follows RAM content
#define OLED_CMD_ON_NORAM		0xA5 //Output ignores RAM content	
#define OLED_CMD_SETCONTRAST	0x81 //double byte cmd

//Macros


//Global variables

//Function prototypes



#endif /* OLED_H_ */
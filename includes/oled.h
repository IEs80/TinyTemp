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

#define OLED_CMD_WAKEUP 0xAF
#define OLED_CMD_ON		0xA4
#define OLED_CMD_SLEEP	0xA5

//Macros


//Global variables

//Function prototypes



#endif /* OLED_H_ */
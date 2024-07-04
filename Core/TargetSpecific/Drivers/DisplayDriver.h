#ifndef DisplayDriver_H_H
#define DisplayDriver_H_H

#include <stdint.h>
#include "main.h"     // Import Port and Pin definitions


#define DISPLAY_RESET_Pin 			DSP_RST_Pin          //XXXX SysBrenner 
#define DISPLAY_RESET_GPIO_Port 	DSP_RST_GPIO_Port    //XXXX SysBrenner 
#define DISPLAY_DCX_Pin 			DSP_DC_Pin           //XXXX SysBrenner 
#define DISPLAY_DCX_GPIO_Port 		DSP_DC_GPIO_Port     //XXXX SysBrenner 
#define DISPLAY_CSX_Pin 			DSP_CS_Pin           //XXXX SysBrenner 
#define DISPLAY_CSX_GPIO_Port		DSP_CS_GPIO_Port     //XXXX SysBrenner 


// SSD1331
#define	SSD_SET_DISPLAY_OFF	        0xAE
#define	SSD_SET_DISPLAY_ON	        0xAF
#define	SSD_SET_REMAP	            0xA0
#define SSD_SET_COLUMN    		    0x15
#define SSD_SET_ROW    		        0x75

/*  FYIO
// SSD1332: list of the registers
#define _CMD_DRAWLINE 		0x21
#define _CMD_DRAWRECT 		0x22
#define _CMD_DRAWCOPY 		0x23 
#define _CMD_DIMWINDOW 		0x24 
#define _CMD_CLRWINDOW 		0x25 
#define _CMD_FILL 			0x26
#define _CMD_PHASEPERIOD 	0x12 
#define _CMD_SETCOLUMN 		0x15 
#define _CMD_SETROW    		0x75 
#define _CMD_CONTRASTA 		0x81 
#define _CMD_CONTRASTB 		0x82 
#define _CMD_CONTRASTC		0x83 
#define _CMD_MASTERCURRENT 	0x87 
#define _CMD_SETREMAP 		0xA0 
#define _CMD_STARTLINE 		0xA1 
#define _CMD_DISPLAYOFFSET 	0xA2 
#define _CMD_NORMALDISPLAY 	0xA4
#define _CMD_DISPLAYALLON  	0xA5
#define _CMD_DISPLAYALLOFF 	0xA6
#define _CMD_INVERTDISPLAY 	0xA7
#define _CMD_SETMULTIPLEX  	0xA8
#define _CMD_SETMASTER 		0xAD
#define _CMD_DISPLAYOFF 	0xAE
#define _CMD_DISPLAYON     	0xAF
#define _CMD_POWERMODE 		0xB0
#define _CMD_PRECHARGE 		0xB1
#define _CMD_CLOCKDIV 		0xB3
#define _CMD_GRAYSCALE 		0xB8
#define _CMD_PRECHARGEA 	0x8A 
#define _CMD_PRECHARGEB 	0x8B 
#define _CMD_PRECHARGEC 	0x8C 
#define _CMD_PRECHARGELEVEL 0xBB 
#define _CMD_VCOMH 			0xBE
#define	_CMD_VPACOLORLVL	0xBB 
#define	_CMD_VPBCOLORLVL	0xBC 
#define	_CMD_VPCCOLORLVL	0xBD 
#define _CMD_NOP			0xE3 
*/

void DisplayDriver_PeripherieInit(void);
void DisplayDriver_DisplayOn(void);
void DisplayDriver_DisplayOff(void);
void DisplayDriver_DisplayReset(void);
void DisplayDriver_DisplayInit(void);
void DisplayDriver_TransmitRectangle(const uint16_t *bitmap, uint16_t posx, uint16_t posy, uint16_t sizex, uint16_t sizey);
int  DisplayDriver_TransmitActive(void);
void DisplayDriver_DmaCallback(void);

#endif

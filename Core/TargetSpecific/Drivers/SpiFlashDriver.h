#ifndef DataReader_H
#define DataReader_H

#include <stdint.h>

#define FLASH_CS_Pin GPIO_PIN_9
#define FLASH_CS_GPIO_Port GPIOB

#define CMD_RDID 0x9F
#define CMD_READ 0x03
#define CMD_WREN 0x06
#define CMD_PP   0x02
#define CMD_RDSR 0x05
#define CMD_SE   0xD8

#define STATUS_WIP 0x01

void SpiFlashDriver_PeripherieInit(void);
void SpiFlashDriver_ReadData(uint32_t address24, uint8_t* buffer, uint32_t length);
void SpiFlashDriver_ReadDataDma(uint32_t address24, uint8_t* buffer, uint32_t length);
void SpiFlashDriver_DmaCallback(void);
int  SpiFlashDriver_ReceiveActive(void);

#endif

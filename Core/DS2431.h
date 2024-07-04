/**
 ******************************************************************************
 * @file    DS2431.h
 * @author  ama
 * @brief   Header file for epprom functions, commands, and architecture
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "stdint.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_i2c.h"

typedef enum { Page1 = 2, Page2 = 3, Page3 = 4, Page4 = 5, PageAll = 1 } owPage;

typedef enum { ChipOn = 1, ChipOff = 2 } ChipConnectStatus;

/*Functions declaration*/
void update_chipstatus(HAL_StatusTypeDef state);
HAL_StatusTypeDef ow_one_wire_reset_ds2484(void);

HAL_StatusTypeDef ow_write_scratchpad_ds2431(uint16_t Address, uint8_t bytes[], uint8_t *Read_CRC);
HAL_StatusTypeDef ow_read_scratchpad_ds2431(uint8_t *Read_Scratchpad);
HAL_StatusTypeDef ow_copy_scratchpad_ds2431(uint8_t Authorization_Code[], uint8_t *Copy_Status);

HAL_StatusTypeDef ow_read_memory_address_ds2431(uint16_t MemAddress, uint8_t *ReadMemory, uint8_t Size);
HAL_StatusTypeDef ow_write_memory_address_ds2431(uint16_t MemAddress, uint64_t Value);

HAL_StatusTypeDef ow_write_memory_row_ds2431(uint8_t row, uint64_t Value);
HAL_StatusTypeDef ow_read_memory_row_ds2431(uint8_t row, uint64_t *ReadRow);

uint8_t ow_clear_memory_ds2431(owPage localpage);
HAL_StatusTypeDef ow_read_all_memory_ds2431(uint64_t *ReadAllMemory);

// #############  DS2431 Commands  ###################

#define DS2431_WRITESCRATCH   0x0F // Write Scratchpad
#define DS2431_READSCRATCH    0xAA // Read Scratchpad
#define DS2431_COPYSCRATCH    0x55 // Copy Scratchpad
#define DS2431_READMEM        0xF0 // Read Memory
#define DS2431_READROM        0x33 // Read ROM
#define DS2431_MATCHROM       0x55 // Match ROM
#define DS2431_SEARCHROM      0xF0 // Search ROM
#define DS2431_SKIPROM        0xCC // Skip ROM
#define DS2431_RESUME         0xA5
#define DS2431_OVERDRIVESKIP  0x3C
#define DS2431_OVERDRIVEMATCH 0x69

// #############  DS2431 EPPROM Architecture  ###################
#define SEG_SIZE_BITS      8
#define SEG_SIZE_HEX       0x0008
#define BYTES_PER_SEG      1
#define BYTES_PER_ROW      8
#define NUM_PAGES          17 // 136 Bytes
#define BASE_ADDRESS       0x0000
#define ADDRESS_SIZE_BYTES 2 // 16-bit address
#define CLEAR_BYTE         0x00
#define Num_BYTES          144 // Can not Write last 2 Bytes
#define ds2431_tPROG       16

// #############  DS2431 Operations Output  ###################
#define Read_CRC_BYTES           2  // Read CRC to check for data integrity <2 Bytes CRC-16>
#define Read_Scratchpad_BYTES    13 // Read TA1, TA2, E/S, <8 Data Bytes> , <2 Bytes CRC-16>
#define Copy_Scratchpad_BYTES    1  // Read copy status, AAh = success
#define Authorization_Code_BYTES 3  // Read TA1, TA2, E/S

typedef struct {
    uint16_t TEST;       // for Testing of BKC
    uint16_t OwnVERSION; // Version of the following struct
    uint16_t dummy1;
    uint16_t dummy2;
    uint32_t Reserved1[6];
} owParamPage1;

typedef struct {
    uint32_t Weldtime[2]; // Weldtime of this torch in s, two Values for Failsave
    uint32_t Reserved2[6];
} owParamPage2;

typedef struct {
    uint32_t Writterflag;
    uint32_t TorchType; // Type of Torch see TorchType enum
    uint32_t Reserved3[6];
} owParamPage3;

typedef struct { // Page4 will be used for BKC test.
    uint32_t Reserved4[8];
} owParamPage4;

typedef enum {
    undef = 0x00000000,
    u300G = 0x00000001,
    u320W = 0x00000002,
    u350G = 0x00000003,
    u400W = 0x00000004,
    u420W = 0x00000005,
    u500W = 0x00000006
} TorchType;

#define SIZEOF_OWPARAMPAGE1       sizeof(st_OwParamPage1)
#define SIZEOF_OWPARAMPAGE2       sizeof(st_OwParamPage2)
#define SIZEOF_OWPARAMPAGE3       sizeof(st_OwParamPage3)
#define SIZEOF_OWPARAMPAGE4       sizeof(st_OwParamPage4)
#define CMD_BKCTEST_STARTTEST     0x01
#define CMD_EEPROMID_READ         0x01
#define CMD_EEPROMID_READ_PACKET2 0x02
#define CMD_BKCTEST_CLEARALL      0x01
#define CMD_TORCHTYPE_WRITE       0x01
#define CMD_TORCHTYPE_READ        0x02
#define CMD_BKCTEST_CLEAR_PAGE1   0x02
#define CMD_BKCTEST_CLEAR_PAGE2   0x03
#define CMD_BKCTEST_CLEAR_PAGE3   0x04
#define CMD_BKCTEST_CLEAR_PAGE4   0x05
#define CMD_BKCTEST_PASS          0x01
#define CMD_BKCTEST_FAIL          0x02
#define CMD_BKCTORCH_TYPE_WRITTEN 0x03

HAL_StatusTypeDef ow_read_ROMID_ds2431(void);
HAL_StatusTypeDef ow_param_Verify_ds2431(void);

HAL_StatusTypeDef ow_write_mem_row_ds2431_offset(owPage localpage, uint64_t Value, uint8_t offset);
HAL_StatusTypeDef ow_read_mem_row_ds2431_offset(owPage localpage, uint8_t *ReadMemoryPage, uint8_t offset);
HAL_StatusTypeDef owParamPage_init_ds2431(void);
HAL_StatusTypeDef owParamPage_write_ds2431(owPage localpage);
HAL_StatusTypeDef owParamPage_read_ds2431(owPage localpage);
uint8_t ow_Write_TorchType(uint32_t enumTorchType);
uint8_t ow_read_TorchType(uint32_t *enumTorchType);
uint8_t ow_BKCTest(void);
void ow_split_ROMID_ds2431(uint8_t *idslitbufer);
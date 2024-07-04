/**
 ******************************************************************************
 * @file    DS2484.h
 * @author  ama
 * @brief   Header file for 1-wire interface functions and commands
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "stdint.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_hal_i2c.h"
/*Functions declaration*/
HAL_StatusTypeDef ow_reset_ds2484(void);
HAL_StatusTypeDef ow_write_device_configuration_ds2484(uint8_t Config_Feature);
HAL_StatusTypeDef ow_set_read_pointer_status_register_ds2484(void);
HAL_StatusTypeDef ow_read_status_register_ds2484(void);
HAL_StatusTypeDef ow_adjust_one_wire_port_ds2484(void);
HAL_StatusTypeDef ow_set_read_pointer_port_configuration_register_ds2484(void);
HAL_StatusTypeDef ow_read_port_configuration_register_ds2484(void);

HAL_StatusTypeDef ow_single_bit_ds2484(void);
HAL_StatusTypeDef ow_write_byte_ds2484(uint8_t Byte_Data);
HAL_StatusTypeDef ow_read_byte_ds2484(void);
HAL_StatusTypeDef ow_set_read_pointer_read_data_register(void);
HAL_StatusTypeDef ow_triplet_ds2431(void);
HAL_StatusTypeDef ow_set_read_pointer_ds2484(uint8_t Register_Code);
HAL_StatusTypeDef ow_read_register_ds2484(uint8_t Size);
HAL_StatusTypeDef ow_setup_ds2484(void);

#define BKC_EN_DS2484       GPIO_PIN_12
#define DS2484_ADDRESS      ((uint8_t)0X30) /* Device  Address */
#define DS2484_ADDRESS_READ ((uint8_t)0X31) /* Device  Address */
#define Time_OUT            1000
#define DS2484_DELAY        1

#define DS2484_CMD_RESET            0xF0 /* No param */
#define DS2484_CMD_SET_READ_PTR     0xE1 /* Param: DS2484_PTR_CODE_xxx */
#define DS2484_CMD_ADJUST_PORT      0xC3 /* Param: Control byte  */
#define DS2484_CMD_WRITE_CONFIG     0xD2 /* Param: Config byte */
#define DS2484_CMD_1WIRE_RESET      0xB4 /* Param: None */
#define DS2484_CMD_1WIRE_SINGLE_BIT 0x87 /* Param: Bit byte (bit7) */
#define DS2484_CMD_1WIRE_WRITE_BYTE 0xA5 /* Param: Data byte */
#define DS2484_CMD_1WIRE_READ_BYTE  0x96 /* Param: None */
/* Note to read the byte, Set the ReadPtr to Data then read (any addr) */
#define DS2484_CMD_1WIRE_TRIPLET 0x78 /* Param: Dir byte (bit7) */

/* Values for DS2484_CMD_SET_READ_PTR */
#define DS2484_PTR_CODE_STATUS      0xF0
#define DS2484_PTR_CODE_DATA        0xE1
#define DS2484_PTR_CODE_PORT_CONFIG 0xB4
#define DS2484_PTR_CODE_CONFIG      0xC3

/*
 * Configure Register bit definitions
 * The top 4 bits always read 0.
 * To write, the top nibble must be the 1's compl. of the low nibble.
 */
#define DS2484_REG_CFG_1WS 0x78 /* 1-wire speed */
#define DS2484_REG_CFG_SPU 0xB4 /* strong pull-up */
#define DS2484_REG_CFG_PDN 0xD2 /*1-Wire Power-Down */
#define DS2484_REG_CFG_APU 0xE1 /* active pull-up */

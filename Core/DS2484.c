/**
 ******************************************************************************
 * @file    DS2484.c
 * @author  ama
 * @brief   DS2484 one wire interface initialization and functions
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "DS2484.h"
#include <stdio.h>

extern I2C_HandleTypeDef hi2cOneWire; /*I2C peripheral handeller*/
#define DS2484_I2C &hi2cOneWire
/******************************************************************************
** Name               : @fn ow_setup_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function calls other function to initialize ds2484 one-wire interface as explained
*in the datasheet I2C example.
**
** Calling            : @Aus main.c
**
** InputValues        : @param none
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_setup_ds2484(void) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = ow_reset_ds2484(); // Reset Device
    I2C_Return =
        ow_write_device_configuration_ds2484(DS2484_REG_CFG_APU); // Configure device with Active Pullup (APU) feature.
    I2C_Return = ow_adjust_one_wire_port_ds2484();                // Adjust port configuration register
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_reset_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function write a "Device Reset" command to ds2484 one wire interface and reads its
*status register after device reset.
**
** Calling            : @remark ow_setup_ds2484
**
** InputValues        : @param none
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_reset_ds2484(void) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t Device_Reset[1] = {
        DS2484_CMD_RESET, // Device Reset
    };
    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, Device_Reset, 1, Time_OUT); // reset device
    //	I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_STATUS); //Optional after Device Reset CMD
    I2C_Return = ow_read_register_ds2484(1); // read Status register in after reset
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_set_read_pointer_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function sets the read pointer of ds2484 one wire interface to a specific Device
*Register
**                       by writing a "Set Read Pointer" command followed by the register code.
**
** Calling            : @remark general use after different function commands.
**
** InputValues        : @param uint8_t Register_Code which is a unique code for each device register
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_set_read_pointer_ds2484(uint8_t Register_Code) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;

    uint8_t Set_Read_Pointer[2];
    Set_Read_Pointer[0] = DS2484_CMD_SET_READ_PTR;
    Set_Read_Pointer[1] = Register_Code; // set read pointer for configuration register

    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, Set_Read_Pointer, 2,
                                         Time_OUT); // Set pointer to configuration register
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_read_register_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function reads the a specific number of bytes from the register at which the device
*read pointer is set.
**
** Calling            : @remark general use (for debuging) after different function commands.
**
** InputValues        : @param uint8_t Size which is number of bytes to be read.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_register_ds2484(uint8_t Size) {
    uint8_t Read_Byte[10] = // Maximum size for reading from register
        {
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
            0x00, // byte for reading data
        };
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, Read_Byte, Size,
                                        Time_OUT); // read current pointed register
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_write_device_configuration_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes a specific device configuration to ds2484 one wire interface
**                       by writing a "Write Device Configuration" command followed by a certain configuration feature.
**
** Calling            : @remark ow_setup_ds2484
**
** InputValues        : @param uint8_t Config_Feature which is device configuration feature APU,PDN,SPU, or 1WS.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_write_device_configuration_ds2484(uint8_t Config_Feature) {
    uint8_t Device_Config[2];
    Device_Config[0] = DS2484_CMD_WRITE_CONFIG; /* Data 0 */ // send Write Device Configuration "WCFG"
    Device_Config[1] = Config_Feature; /* Data 1 */          // data from the datasheet for specific configuration

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, Device_Config, 2, Time_OUT); // configure devide
    // I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_CONFIG); //Optional after Device Config CMD
    I2C_Return = ow_read_register_ds2484(1); // Read Device Configuration register (for debuging)

    I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_STATUS);
    I2C_Return = ow_read_register_ds2484(1); // Read Status register after Device Config CMD (for debuging)

    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_adjust_one_wire_port_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes default port adjustment to ds2484 one wire interface
**                       by writing an "Adjust 1-Wire Port" command followed by the default port configuration.
**
** Calling            : @remark ow_setup_ds2484
**
** InputValues        : @param none
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_adjust_one_wire_port_ds2484(void) {
    uint8_t Adjust_Port[6];
    Adjust_Port[0] = DS2484_CMD_ADJUST_PORT; /* Data 0 */ // ADJP
    Adjust_Port[1] = 0x06;
    /* Data 1 */ // selects tRSTL /*lower nipple = 6 for power-on default values, bit 4 = 0 (OD=0)*/
    Adjust_Port[2] = 0x26; /* Data 2 */ // selects tMSP
    Adjust_Port[3] = 0x46; /* Data 3 */ // selects tW0L
    Adjust_Port[4] = 0x66; /* Data 4 */ // selects tREC0
    Adjust_Port[5] = 0x86; /* Data 5 */ // selects RWPU

    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, Adjust_Port, 6, Time_OUT);
    //  I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_PORT_CONFIG); //Optional after Adjust port CMD
    I2C_Return = ow_read_register_ds2484(8); // Read Port Configuration Register (for debuging)

    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_one_wire_reset_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes a "1-Wire Reset" command (To Begin or End 1-Wire Communication).
**
** Calling            : @remark general use before initiating any one wire communication with the slave device.
**
** InputValues        : @param none
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_one_wire_reset_ds2484(void) {
    uint8_t OneWire_Reset[1] = {
        DS2484_CMD_1WIRE_RESET, // 1WRS
    };

    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return =
        HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, OneWire_Reset, 1, Time_OUT); // reset 1-Wire mbv 1WRS byte
    //	I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_STATUS); //Optional after Device Reset CMD
    I2C_Return = ow_read_register_ds2484(1); // Read Status register in after reset (for debuging)
    HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_single_bit_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes "1-Wire Single Bit" command (To Generate a Single Time Slot on the
*1-Wire Line).
**
** Calling            : @remark currently not used
**
** InputValues        : @param none
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_single_bit_ds2484(void) {
    uint8_t OneWire_SingleBit[2];
    OneWire_SingleBit[0] = DS2484_CMD_1WIRE_SINGLE_BIT; /* Data 0 */ // send 1WBS
    OneWire_SingleBit[1] = 0x10; /* Data 1 */                        // V value (1 of 0)

    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, OneWire_SingleBit, 2, Time_OUT);
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_write_byte_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes a "1-Wire Write Byte" command (To Send a Command Code or Data Byte
*to the 1-Wire Line).
**
** Calling            : @Aus DS2431.c
**
** InputValues        : @param uint8_t Byte_Data which is a Command Code or Data Byte to the 1-Wire Line
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_write_byte_ds2484(uint8_t Byte_Data) {
    uint8_t OneWire_WriteByte[2];
    OneWire_WriteByte[0] = DS2484_CMD_1WIRE_WRITE_BYTE; /* Data 0 */ // send 1WBS
    OneWire_WriteByte[1] = Byte_Data; /* Data 1 */                   // V value
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, OneWire_WriteByte, 2, Time_OUT); // Write Byte data
    HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_read_byte_ds2484
**
** Created from /on   : @ama / @16.06.2022
**
** Description        : @brief This function writes a "1-Wire Read Byte" command (To Read a Byte from the 1-Wire Line).
**
** Calling            : @Aus DS2431.c
**
** InputValues        : @param uint8_t Byte_Data which is a Command Code or Data Byte to the 1-Wire Line
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_byte_ds2484(void) {
    uint8_t OneWire_ReadByte[1] = {
        DS2484_CMD_1WIRE_READ_BYTE, // 1WRB
    };
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    I2C_Return = HAL_I2C_Master_Transmit(DS2484_I2C, DS2484_ADDRESS, OneWire_ReadByte, 1, Time_OUT); // read byte
    // HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}

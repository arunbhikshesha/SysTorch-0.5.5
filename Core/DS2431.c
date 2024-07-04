/**
 ******************************************************************************
 * @file    DS2431.c
 * @author  ama
 * @brief   DS2484 epprom functions
 ******************************************************************************
 *
 ******************************************************************************
 */
#include "DS2484.h"
#include "DS2431.h"

#include <stdio.h>

uint64_t ds2431ROMid = 0x00000000;
owParamPage1 st_OwParamPage1;
owParamPage2 st_OwParamPage2;
owParamPage3 st_OwParamPage3;
owParamPage4 st_OwParamPage4;
TorchType enumTorchType;
owPage page;
ChipConnectStatus ds2431connectstatus;

extern I2C_HandleTypeDef hi2cOneWire; /*I2C peripheral handeller*/
#define DS2484_I2C &hi2cOneWire

/******************************************************************************
** Name               : @fn update_chipstatus
**
** Created from /on   : @spa / @26.03.2024
**
** Description        : @brief This function update the ds2431 connect status
*epprom.
**
** Calling            : @ null
**
** InputValues        : @param HAL_StatusTypeDef state
**
** OutputValues       : @ null
******************************************************************************/
void update_chipstatus(HAL_StatusTypeDef state) {
    if (state == HAL_OK)
        ds2431connectstatus = ChipOn;
    else
        ds2431connectstatus = ChipOff;
}

/******************************************************************************
** Name               : @fn ow_write_scratchpad_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function write 8-byte data and their start address to the scratchpad of ds2431
*epprom.
**
** Calling            : @Aus ow_write_memory_address_ds2431
**
** InputValues        : @param uint16_t Address which is a start address of the writing operation
**                             uint8_t bytes[] which is an array of bytes to be written
**                             uint8_t *Read_CRC which is pointer to Read_CRC array to read the returned 2-byte CRC
*after writing the scratchpad.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_write_scratchpad_ds2431(uint16_t Address, uint8_t bytes[], uint8_t *Read_CRC) {

    uint8_t targetAdress[ADDRESS_SIZE_BYTES]; // 0x1234
    for (int i = 0; i < ADDRESS_SIZE_BYTES; i++)
        targetAdress[i] = *((uint8_t *)&(Address) + i); // TA1  (0x34) //TA2  (0x12)

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = ow_one_wire_reset_ds2484();                // Reset pulse
    I2C_Return = ow_write_byte_ds2484(DS2431_SKIPROM);      // Issue �Skip ROM� command
    I2C_Return = ow_write_byte_ds2484(DS2431_WRITESCRATCH); // Issue �Write Scratchpad� command
    I2C_Return = ow_write_byte_ds2484(targetAdress[0]);
    I2C_Return = ow_write_byte_ds2484(targetAdress[1]);

    for (int i = 0; i < BYTES_PER_ROW; i++) // Write 8-byte data
        I2C_Return = ow_write_byte_ds2484(bytes[i]);
    for (int i = 0; i < Read_CRC_BYTES; i++) // Read CRC to check for data integrity
    {
        I2C_Return = ow_read_byte_ds2484();
        I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_DATA);
        HAL_Delay(DS2484_DELAY);
        I2C_Return =
            HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &Read_CRC[i], 1, Time_OUT); // read databyte
    }
    HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_read_scratchpad_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function read 8-byte data, their authorization code, and CRC bytes from the
*scratchpad of ds2431 epprom.
**
** Calling            : @Aus ow_write_memory_address_ds2431
**
** InputValues        : @param uint8_t *Read_Scratchpad which is a pointer to the array in which the read scratchpad
*process return value is stored.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_scratchpad_ds2431(uint8_t *Read_Scratchpad) {

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = ow_one_wire_reset_ds2484();               // Reset pulse
    I2C_Return = ow_write_byte_ds2484(DS2431_SKIPROM);     // Issue �Skip ROM� command
    I2C_Return = ow_write_byte_ds2484(DS2431_READSCRATCH); // Issue �Read Scratchpad� command
    for (int i = 0; i < Read_Scratchpad_BYTES; i++) // Read 3-byte authorization code, 8-byte data, and 2-byte CRC
    {
        I2C_Return = ow_read_byte_ds2484();
        I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_DATA);
        HAL_Delay(DS2484_DELAY);
        I2C_Return =
            HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &Read_Scratchpad[i], 1, Time_OUT); // read databyte
    }
    HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_copy_scratchpad_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function copies 8-byte data from the scratchpad of ds2431 epprom to their start
*address included in authorization code.
**
** Calling            : @Aus ow_write_memory_address_ds2431
**
** InputValues        : @param uint16_t Address which is start address of the writing operation
**                             uint8_t Authorization_Code[] which is array of authorization code read from the
*scratchpad of ds2431 epprom.
**                             uint8_t *Copy_Status which is pointer to Copy_Status array to read the returned 1-byte
*after reading the scratchpad.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_copy_scratchpad_ds2431(uint8_t Authorization_Code[], uint8_t *Copy_Status) {

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = ow_one_wire_reset_ds2484();                  // Reset pulse
    I2C_Return = ow_write_byte_ds2484(DS2431_SKIPROM);        // Issue �Skip ROM� command
    I2C_Return = ow_write_byte_ds2484(DS2431_COPYSCRATCH);    // Issue �Copy Scratchpad� command
    I2C_Return = ow_write_byte_ds2484(Authorization_Code[0]); // TA1
    I2C_Return = ow_write_byte_ds2484(Authorization_Code[1]); // TA2
    I2C_Return = ow_write_byte_ds2484(Authorization_Code[2]); // E/s
    HAL_Delay(DS2484_DELAY);
    for (int i = 0; i < Copy_Scratchpad_BYTES; i++) // Read copy status byte  (AAh = success).
    {
        I2C_Return = ow_read_byte_ds2484();
        I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_DATA);
        HAL_Delay(DS2484_DELAY);
        I2C_Return =
            HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &Copy_Status[i], 1, Time_OUT); // read databyte
    }
    HAL_Delay(DS2484_DELAY);
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_write_memory_address_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function write 8-byte data starting from a certain memory address.
**
** Calling            : @Aus main.c
** InputValues        : @param uint16_t MemAddress which is start address of the writing operation
**                             uint64_t Value which is 8-byte value to be written.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_write_memory_address_ds2431(uint16_t MemAddress, uint64_t Value) {

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    uint8_t bytes[BYTES_PER_ROW];
    for (int i = 0; i < BYTES_PER_ROW; i++) // Convert from uint64_t to uint8_t
        bytes[i] = *((uint8_t *)&(Value) + i);
    uint8_t Read_CRC[Read_CRC_BYTES];
    I2C_Return = ow_write_scratchpad_ds2431(MemAddress, &bytes[0], Read_CRC); // Write Scratchpad
    uint8_t Read_Scratchpad[Read_Scratchpad_BYTES];
    I2C_Return = ow_read_scratchpad_ds2431(Read_Scratchpad); // Read Scratchpad
    uint8_t Authorization_Code[Authorization_Code_BYTES] = {Read_Scratchpad[0], Read_Scratchpad[1],
                                                            Read_Scratchpad[2]}; // TA1, TA2, E/S (Authorization code)
    uint8_t Copy_Status[Copy_Scratchpad_BYTES];
    I2C_Return = ow_copy_scratchpad_ds2431(&Authorization_Code[0], Copy_Status); // Copy Scratchpad
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_read_memory_address_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function reads a certain number of bytes starting from a certain address.
**
** Calling            : @Aus main.c
**
** InputValues        : @param uint16_t MemAddress which is start address of the reading operation
**                             uint8_t *ReadMemory which is pointer to an array to which the return value of the read
*operation is stored
**                             uint8_t Size which is the number of bytes to be read
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_memory_address_ds2431(uint16_t MemAddress, uint8_t *ReadMemory, uint8_t Size) {

    uint8_t targetAdress[ADDRESS_SIZE_BYTES]; // 0x1234
    for (int i = 0; i < ADDRESS_SIZE_BYTES; i++)
        targetAdress[i] = *((uint8_t *)&(MemAddress) + i); // TA1  (0x34) //TA2  (0x12)

    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = ow_one_wire_reset_ds2484();            // Reset Pulse
    I2C_Return = ow_write_byte_ds2484(DS2431_SKIPROM);  // Issue �Skip ROM� command
    I2C_Return = ow_write_byte_ds2484(DS2431_READMEM);  // Issue �Read Memory� command
    I2C_Return = ow_write_byte_ds2484(targetAdress[0]); // TA1
    I2C_Return = ow_write_byte_ds2484(targetAdress[1]); // TA2
    for (int i = 0; i < Size; i++)                      // Read bytes from memory
    {
        I2C_Return = ow_read_byte_ds2484();
        I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_DATA);
        HAL_Delay(DS2484_DELAY);
        I2C_Return =
            HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &ReadMemory[i], 1, Time_OUT); // read databyte
    }
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_write_memory_row_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function write 8-byte data  to a certain memory page (1-17).
**
** Calling            : @Aus main.c
**
** InputValues        : @param uint8_t page which is number of page to be written.
**                             uint64_t Value which is 8-byte value to be written.
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_write_memory_row_ds2431(uint8_t row, uint64_t Value) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint16_t MemAddress = (row - 1) * SEG_SIZE_HEX; // Page Numbering (1-17)
    I2C_Return = ow_write_memory_address_ds2431(MemAddress, Value);
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_read_memory_row_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function reads a page.
**
** Calling            : @Aus main.c
**
** InputValues        : @param uint8_t page number
**                             uint64_t *ReadPage page memory pointer to store
*operation is stored
**                             uint8_t Size which is the number of bytes to be read
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_memory_row_ds2431(uint8_t row, uint64_t *ReadRow) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t offset = (row - 1);
    uint16_t MemAddress = offset * SEG_SIZE_HEX; // Page Numbering (1-17)
    uint8_t ReadMemoryRow[BYTES_PER_ROW];
    uint64_t temp = 0ULL;
    I2C_Return = ow_read_memory_address_ds2431(MemAddress, ReadMemoryRow, BYTES_PER_ROW);
    for (int i = (SEG_SIZE_BITS - 1); i >= 0; --i) // Convert from uint8_t to uint64_t
        temp = (temp << SEG_SIZE_BITS) | ReadMemoryRow[i];
    ReadRow[0] = temp;
    return I2C_Return;
}
/******************************************************************************
** Name               : @fn ow_clear_memory_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function clears the entire epprom by writing 0x00 in all memory addresses.
**
** Calling            : @Aus main.c
**
** InputValues        : @param owPage localpage
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
uint8_t ow_clear_memory_ds2431(owPage localpage) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint16_t MemAddress;
    uint8_t bytes[BYTES_PER_ROW], start, end, i, returntype; // 8-byte clear bytes

    if (ds2431connectstatus == ChipOff) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }

    for (int i = 0; i < BYTES_PER_ROW; i++)
        bytes[i] = CLEAR_BYTE;

    if (localpage == Page1) {
        st_OwParamPage1.TEST = 0x0000;
        st_OwParamPage1.OwnVERSION = 0x0000;
        st_OwParamPage1.dummy1 = 0x0000;
        st_OwParamPage1.dummy2 = 0x0000;
        for (i = 0; i < 6; i++)
            st_OwParamPage1.Reserved1[i] = 0x0000;

        MemAddress = 0x0000; // Page Numbering (1-17)
        start = 0;
        end = 4;
    } else if (localpage == Page2) {
        MemAddress = 0x0020; // Page Numbering (1-17)
        start = 4;
        end = 8;
        st_OwParamPage2.Weldtime[0] = 0x00000000;
        st_OwParamPage2.Weldtime[1] = 0x00000000;
        for (i = 0; i < 6; i++)
            st_OwParamPage2.Reserved2[i] = 0x0000;

    } else if (localpage == Page3) {
        MemAddress = 0x0040; // Page Numbering (1-17)
        start = 8;
        end = 12;
        st_OwParamPage3.Writterflag = 0x00000000;
        st_OwParamPage3.TorchType = 0x00000000;
        for (i = 0; i < 6; i++)
            st_OwParamPage3.Reserved3[i] = 0x0000;
    } else if (localpage == Page4) {
        MemAddress = 0x0060; // Page Numbering (1-17)
        start = 12;
        end = 16;
        for (i = 0; i < 8; i++)
            st_OwParamPage4.Reserved4[i] = 0x0000;

    } else if (localpage == PageAll) {
        MemAddress = 0x0000; // Page Numbering (1-17)
        start = 0;
        end = NUM_PAGES;
    }

    //
    for (i = start; i < end; i++) // Iterate over memory pages
    {
        uint8_t Read_CRC[2];
        I2C_Return = ow_write_scratchpad_ds2431(MemAddress, &bytes[0], Read_CRC); // Write scratchpad
        uint8_t Read_Scratchpad[13];
        I2C_Return = ow_read_scratchpad_ds2431(Read_Scratchpad); // Read scratchpad
        uint8_t Authorization_Code[3] = {Read_Scratchpad[0], Read_Scratchpad[1], Read_Scratchpad[2]};
        uint8_t Copy_Status[1];
        I2C_Return = ow_copy_scratchpad_ds2431(&Authorization_Code[0], Copy_Status); // Copy scratchpad
        MemAddress += (SEG_SIZE_HEX);
    }
    if (I2C_Return == HAL_OK) {
        returntype = CMD_BKCTEST_PASS; // clear success
        return returntype;
    } else {
        returntype = CMD_BKCTEST_FAIL; // clear fail
        return returntype;
    }

    I2C_Return = owParamPage_init_ds2431(); // read all eeprom member element

    if (I2C_Return == HAL_OK) {
        returntype = CMD_BKCTEST_PASS; // clear success
    } else {
        returntype = CMD_BKCTEST_FAIL; // clear fail
    }
    return returntype;
}
/******************************************************************************
** Name               : @fn ow_read_all_memory_ds2431
**
** Created from /on   : @ama / @30.11.2022
**
** Description        : @brief This function reads the entire epprom pages.
**
** Calling            : @Aus main.c
**
** InputValues        : @param uint64_t *ReadAllMemory which is pointer to an array to which the return value of the
*read operation is stored
**
** OutputValues       : @retval I2C_Return which is return stauts for I2C read/write operation.
******************************************************************************/
HAL_StatusTypeDef ow_read_all_memory_ds2431(uint64_t *ReadAllMemory) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;

    uint16_t MemAddress = BASE_ADDRESS;       // Memory Base address
    uint8_t targetAdress[ADDRESS_SIZE_BYTES]; // 0x1234
    uint8_t ReadMemory[BYTES_PER_ROW];
    uint64_t temp = 0ULL;
    for (int j = 0; j < NUM_PAGES; j++) // Iterate over the entire memory pages
    {
        MemAddress = SEG_SIZE_HEX * j;

        for (int i = 0; i < ADDRESS_SIZE_BYTES; i++)
            targetAdress[i] = *((uint8_t *)&(MemAddress) + i); // TA1  (0x34) //TA2  (0x12)

        I2C_Return = ow_one_wire_reset_ds2484();            // Reset Pulse
        I2C_Return = ow_write_byte_ds2484(DS2431_SKIPROM);  // Issue �Skip ROM� command
        I2C_Return = ow_write_byte_ds2484(DS2431_READMEM);  // Issue �Read Memory� command
        I2C_Return = ow_write_byte_ds2484(targetAdress[0]); // TA1
        I2C_Return = ow_write_byte_ds2484(targetAdress[1]); // TA2
        for (int i = 0; i < BYTES_PER_ROW; i++)             // Read one memory page
        {
            I2C_Return = ow_read_byte_ds2484();
            I2C_Return = ow_set_read_pointer_ds2484(DS2484_PTR_CODE_DATA);
            HAL_Delay(DS2484_DELAY);
            I2C_Return =
                HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &ReadMemory[i], 1, Time_OUT); // read databyte
        }
        for (int i = (SEG_SIZE_BITS - 1); i >= 0; --i) // Convert from uint8_t to uint64_t
            temp = (temp << SEG_SIZE_BITS) | ReadMemory[i];
        ReadAllMemory[j] = temp;
    }
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_write_mem_row_ds2431_offset
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function write 8-byte data any page address
**                             with offset indexing, in epprom.
** InputValues        : @param uint8_t page - page no: 1 or 2 or 3 or 4
**                             uint64_t Value - 8 byte, converted to 64 bit value
**                             uint8_t offset - each page has 4 rows, which row to write
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

HAL_StatusTypeDef ow_write_mem_row_ds2431_offset(owPage localpage, uint64_t Value, uint8_t offset) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint16_t MemAddress;
    if (localpage == Page1)
        MemAddress = 0x0000 + offset; // Page Numbering (1-17)
    else if (localpage == Page2)
        MemAddress = 0x0020 + offset; // Page Numbering (1-17)
    else if (localpage == Page3)
        MemAddress = 0x0040 + offset; // Page Numbering (1-17)
    else if (localpage == Page4)
        MemAddress = 0x0060 + offset; // Page Numbering (1-17)
    I2C_Return = ow_write_memory_address_ds2431(MemAddress, Value);
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_read_mem_row_ds2431_offset
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function write 8-byte data any page address
**                             with offset indexing, in epprom.
** InputValues        : @param uint8_t page - page no: 1 or 2 or 3 or 4
**                             uint8_t *ReadMemoryPage - 8 byte to read memory pointer
**                             uint8_t offset - each page has 4 rows, which row to write
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

HAL_StatusTypeDef ow_read_mem_row_ds2431_offset(owPage localpage, uint8_t *ReadMemoryPage, uint8_t offset) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint16_t MemAddress;

    if (localpage == Page1)
        MemAddress = 0x0000 + offset; // Page Numbering (1-17)
    else if (localpage == Page2)
        MemAddress = 0x0020 + offset; // Page Numbering (1-17)
    else if (localpage == Page3)
        MemAddress = 0x0040 + offset; // Page Numbering (1-17)
    else if (localpage == Page4)
        MemAddress = 0x0060 + offset; // Page Numbering (1-17)
    I2C_Return = ow_read_memory_address_ds2431(MemAddress, ReadMemoryPage, BYTES_PER_ROW);
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_read_ROMID_ds2431
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function  read the ROMID
** InputValues        : @param null
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

HAL_StatusTypeDef ow_read_ROMID_ds2431(void) {

    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t Read_Scratchpad[8], offcount;
    uint64_t sentdata1, sentdata2;

    for (int ii = 0; ii < 8; ii++)
        Read_Scratchpad[ii] = 0;

    sentdata1 = 0;
    sentdata2 = 0;
    offcount = 0;

    I2C_Return = ow_one_wire_reset_ds2484(); // Reset pulse
    HAL_Delay(100);
    I2C_Return = ow_write_byte_ds2484(DS2431_READROM); // Issue �Read Memory� command
    HAL_Delay(200);

    for (int j = 0; j < 8; j++) {
        ow_read_byte_ds2484();
        HAL_Delay(DS2484_DELAY);
        ow_set_read_pointer_ds2484(0xE1);
        HAL_Delay(DS2484_DELAY);
        I2C_Return = HAL_I2C_Master_Receive(DS2484_I2C, DS2484_ADDRESS_READ, &Read_Scratchpad[j], 1, Time_OUT);
        HAL_Delay(DS2484_DELAY);
    }

    if (I2C_Return == HAL_OK) {
        sentdata1 = Read_Scratchpad[0] | Read_Scratchpad[1] << 8 | Read_Scratchpad[2] << 16 | Read_Scratchpad[3] << 24;
        sentdata2 = Read_Scratchpad[4] | Read_Scratchpad[5] << 8 | Read_Scratchpad[6] << 16 | Read_Scratchpad[7] << 24;
        ds2431ROMid = sentdata2 << 32;
        ds2431ROMid += sentdata1;
    }

    for (int ii = 0; ii < 8; ii++) {
        if (Read_Scratchpad[ii] == 0xff)
            offcount++;
    }

    if (offcount >= 6)
        I2C_Return = HAL_ERROR;

    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_split_ROMID_ds2431
**
** Created from /on   : @spa / @01.03.2024
**
** Description        : @brief This function  split the ROMID
** InputValues        : @param null
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

void ow_split_ROMID_ds2431(uint8_t *idslitbufer) {

    uint8_t *ptrDataRead = (uint8_t *)&ds2431ROMid;

    if (ds2431connectstatus == ChipOn) {
        for (int j = 0; j < 8; j++)
            *idslitbufer++ = *ptrDataRead++;
    } else {
        for (int j = 0; j < 8; j++)
            *idslitbufer++ = 0x00;
    }
}

/******************************************************************************
** Name               : @fn owParamPage_read_ds2431
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function read the page - 32 bytes(4 rows)
**                             Each row is 8 byte in size.
** InputValues        : @param uint8_t page - page no: 1 or 2 or 3 or 4
**                             read the entire page
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

HAL_StatusTypeDef owParamPage_read_ds2431(owPage localpage) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t *ptrDataRead, i, ii, offset;
    uint8_t ReadMemoryPage[BYTES_PER_ROW];

    if (localpage == Page1)
        ptrDataRead = (uint8_t *)&st_OwParamPage1;
    else if (localpage == Page2)
        ptrDataRead = (uint8_t *)&st_OwParamPage2;
    else if (localpage == Page3)
        ptrDataRead = (uint8_t *)&st_OwParamPage3;
    else if (localpage == Page4)
        ptrDataRead = (uint8_t *)&st_OwParamPage4;

    for (i = 0; i < 4; i++) {
        for (ii = 0; ii < 8; ii++)
            ReadMemoryPage[ii] = 0;
        offset = (i * 8);
        I2C_Return = ow_read_mem_row_ds2431_offset(localpage, &ReadMemoryPage[0], offset);
        if (I2C_Return == HAL_ERROR)
            return I2C_Return;
        for (ii = 0; ii < 8; ii++)
            *ptrDataRead++ = ReadMemoryPage[ii];
    }

    return I2C_Return;
}

/******************************************************************************
** Name               : @fn owParamPage_init_ds2431
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function read all the pages.
**                             No. of pages = 4
**                             No. of bytes per page = 32
**                             No. of rows per page = 4
**                             No. of bytes per row = 8
**                             Initialize the page content structures
** InputValues        : @param Nil
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/
HAL_StatusTypeDef owParamPage_init_ds2431(void) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;

    // pagesied read from eeprom and initialized
    I2C_Return = owParamPage_read_ds2431(Page1);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page2);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page3);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page4);
    if (I2C_Return != HAL_OK)
        return I2C_Return;

    if (st_OwParamPage3.TorchType == undef)
        enumTorchType = u300G;

    return I2C_Return;
}

/******************************************************************************
** Name               : @fn owParamPage_write_ds2431
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function writes the page - 32 bytes(4 rows)
**                             Each row is 8 byte in size.
** InputValues        : @param uint8_t page - page no: 1 or 2 or 3 or 4
**                             read the entire page
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

HAL_StatusTypeDef owParamPage_write_ds2431(owPage localpage) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t *ptrDataWrite, i, ii, offset, localarr[8];
    uint64_t sentdata1, sentdata2;
    uint64_t sentdata;

    if (localpage == Page1)
        ptrDataWrite = (uint8_t *)&st_OwParamPage1;
    else if (localpage == Page2)
        ptrDataWrite = (uint8_t *)&st_OwParamPage2;
    else if (localpage == Page3)
        ptrDataWrite = (uint8_t *)&st_OwParamPage3;
    else if (localpage == Page4)
        ptrDataWrite = (uint8_t *)&st_OwParamPage4;

    for (i = 0; i < 4; i++) {
        for (ii = 0; ii < 8; ii++)
            localarr[ii] = *ptrDataWrite++;
        sentdata1 = localarr[0] | localarr[1] << 8 | localarr[2] << 16 | localarr[3] << 24;
        sentdata2 = localarr[4] | localarr[5] << 8 | localarr[6] << 16 | localarr[7] << 24;
        sentdata = sentdata2 << 32;
        sentdata |= sentdata1;
        offset = (i * 8);
        I2C_Return = ow_write_mem_row_ds2431_offset(localpage, sentdata, offset);
    }

    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_param_Verify_ds2431
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function verifies the eeprom full memory.
**                             total eeprom = 128 bytes( 4 pages )
**                              write known values to eeprom, read back
**                              and verify the content.
** InputValues        : @param Nil
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/
HAL_StatusTypeDef ow_param_Verify_ds2431(void) {
    uint8_t ii;
    HAL_StatusTypeDef I2C_Return = HAL_OK;

    I2C_Return = ow_clear_memory_ds2431(PageAll);
    if (I2C_Return != HAL_OK)
        return I2C_Return;

    st_OwParamPage1.TEST = 0x1234;       // for Testing of BKC
    st_OwParamPage1.OwnVERSION = 0x5678; // assinging dummy variables
    st_OwParamPage1.dummy1 = 0x5a5a;
    st_OwParamPage1.dummy2 = 0x1122;
    st_OwParamPage1.Reserved1[0] = 0x1190;
    st_OwParamPage1.Reserved1[1] = 0x1191;
    st_OwParamPage1.Reserved1[2] = 0x1192;
    st_OwParamPage1.Reserved1[3] = 0x1193;
    st_OwParamPage1.Reserved1[4] = 0x1194;
    st_OwParamPage1.Reserved1[5] = 0x1195;

    st_OwParamPage2.Weldtime[0] = 0x27654321;
    st_OwParamPage2.Weldtime[1] = 0x37373737;
    st_OwParamPage2.Reserved2[0] = 0x2290;
    st_OwParamPage2.Reserved2[1] = 0x2291;
    st_OwParamPage2.Reserved2[2] = 0x2292;
    st_OwParamPage2.Reserved2[3] = 0x2293;
    st_OwParamPage2.Reserved2[4] = 0x2294;
    st_OwParamPage2.Reserved2[5] = 0x2295;

    st_OwParamPage3.Writterflag = 0x33443344;
    st_OwParamPage3.TorchType = 0x55667788;
    st_OwParamPage3.Reserved3[0] = 0x3390;
    st_OwParamPage3.Reserved3[1] = 0x3391;
    st_OwParamPage3.Reserved3[2] = 0x3392;
    st_OwParamPage3.Reserved3[3] = 0x3393;
    st_OwParamPage3.Reserved3[4] = 0x3394;
    st_OwParamPage3.Reserved3[5] = 0x3395;

    st_OwParamPage4.Reserved4[0] = 0x4490;
    st_OwParamPage4.Reserved4[1] = 0x4491;
    st_OwParamPage4.Reserved4[2] = 0x4492;
    st_OwParamPage4.Reserved4[3] = 0x4493;
    st_OwParamPage4.Reserved4[4] = 0x4494;
    st_OwParamPage4.Reserved4[5] = 0x4495;
    st_OwParamPage4.Reserved4[6] = 0x4496;
    st_OwParamPage4.Reserved4[7] = 0x4497;

    I2C_Return = owParamPage_write_ds2431(Page1);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_write_ds2431(Page2);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_write_ds2431(Page3);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_write_ds2431(Page4);
    if (I2C_Return != HAL_OK)
        return I2C_Return;

    HAL_Delay(DS2484_DELAY * 100);

    st_OwParamPage1.TEST = 0x0000;
    st_OwParamPage1.OwnVERSION = 0x0000;
    st_OwParamPage1.dummy1 = 0x0000;
    st_OwParamPage1.dummy2 = 0x0000;
    for (ii = 0; ii < 6; ii++)
        st_OwParamPage1.Reserved1[ii] = 0x0000;

    st_OwParamPage2.Weldtime[0] = 0x00000000;
    st_OwParamPage2.Weldtime[1] = 0x00000000;
    for (ii = 0; ii < 6; ii++)
        st_OwParamPage2.Reserved2[ii] = 0x0000;

    st_OwParamPage3.Writterflag = 0x00000000;
    st_OwParamPage3.TorchType = 0x00000000;
    for (ii = 0; ii < 6; ii++)
        st_OwParamPage3.Reserved3[ii] = 0x0000;

    for (ii = 0; ii < 8; ii++)
        st_OwParamPage4.Reserved4[ii] = 0x0000;

    // I2C_Return = owParamPage_int_ds2431();

    I2C_Return = owParamPage_read_ds2431(Page1);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page2);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page3);
    if (I2C_Return != HAL_OK)
        return I2C_Return;
    I2C_Return = owParamPage_read_ds2431(Page4);
    if (I2C_Return != HAL_OK)
        return I2C_Return;

    if (st_OwParamPage1.TEST != 0x1234 || st_OwParamPage1.Reserved1[0] != 0x1190 ||
        st_OwParamPage1.OwnVERSION != 0x5678 || st_OwParamPage1.Reserved1[1] != 0x1191 ||
        st_OwParamPage1.dummy1 != 0x5a5a || st_OwParamPage1.Reserved1[2] != 0x1192 ||
        st_OwParamPage1.dummy2 != 0x1122 || st_OwParamPage1.Reserved1[3] != 0x1193 ||
        st_OwParamPage1.Reserved1[4] != 0x1194 || st_OwParamPage1.Reserved1[5] != 0x1195) {
        I2C_Return = HAL_ERROR;
        return I2C_Return;
    }
    if (st_OwParamPage2.Weldtime[0] != 0x27654321 || st_OwParamPage2.Reserved2[0] != 0x2290 ||
        st_OwParamPage2.Weldtime[1] != 0x37373737 || st_OwParamPage2.Reserved2[1] != 0x2291 ||
        st_OwParamPage2.Reserved2[2] != 0x2292 || st_OwParamPage2.Reserved2[3] != 0x2293 ||
        st_OwParamPage2.Reserved2[4] != 0x2294 || st_OwParamPage2.Reserved2[5] != 0x2295) {
        I2C_Return = HAL_ERROR;
        return I2C_Return;
    }
    if (st_OwParamPage3.Writterflag != 0x33443344 || st_OwParamPage3.Reserved3[0] != 0x3390 ||
        st_OwParamPage3.TorchType != 0x55667788 || st_OwParamPage3.Reserved3[1] != 0x3391 ||
        st_OwParamPage3.Reserved3[2] != 0x3392 || st_OwParamPage3.Reserved3[3] != 0x3393 ||
        st_OwParamPage3.Reserved3[4] != 0x3394 || st_OwParamPage3.Reserved3[5] != 0x3395) {
        I2C_Return = HAL_ERROR;
        return I2C_Return;
    }

    if (st_OwParamPage4.Reserved4[0] != 0x4490 || st_OwParamPage4.Reserved4[1] != 0x4491 ||
        st_OwParamPage4.Reserved4[2] != 0x4492 || st_OwParamPage4.Reserved4[3] != 0x4493 ||
        st_OwParamPage4.Reserved4[4] != 0x4494 || st_OwParamPage4.Reserved4[5] != 0x4495 ||
        st_OwParamPage4.Reserved4[6] != 0x4496 || st_OwParamPage4.Reserved4[7] != 0x4497) {
        I2C_Return = HAL_ERROR;
        return I2C_Return;
    }
    return I2C_Return;
}

/******************************************************************************
** Name               : @fn ow_Write_TorchType
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function write the torch type parameter
**                             to eeprom, if not written once
** InputValues        : @param TorchType enumTorchType
**                             uint32_t *retwriteflag
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/
uint8_t ow_Write_TorchType(uint32_t enumTorchType) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t returntype;

    if (ds2431connectstatus == ChipOff) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }

    if (st_OwParamPage3.Writterflag == 0) {
        st_OwParamPage3.Writterflag = 1;
        st_OwParamPage3.TorchType = enumTorchType;
        I2C_Return = owParamPage_write_ds2431(Page3);
    }

    if (I2C_Return == HAL_OK) {
        returntype = CMD_BKCTEST_PASS; // write success
        if (st_OwParamPage3.Writterflag == 1)
            returntype = CMD_BKCTORCH_TYPE_WRITTEN; // already existing
    } else {
        returntype = CMD_BKCTEST_FAIL; // write fail
    }
    return returntype;
}

/******************************************************************************
** Name               : @fn ow_read_TorchType
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function read the torch type parameter
**                             from eeprom
** InputValues        : @param TorchType enumTorchType
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/
uint8_t ow_read_TorchType(uint32_t *enumTorchType) {
    HAL_StatusTypeDef I2C_Return = HAL_OK;
    uint8_t returntype;

    if (ds2431connectstatus == ChipOff) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }
    I2C_Return = owParamPage_read_ds2431(Page3);
    *enumTorchType = st_OwParamPage3.TorchType;

    if (I2C_Return == HAL_OK)
        returntype = CMD_BKCTEST_PASS; // write success
    else
        returntype = CMD_BKCTEST_FAIL; // write fail
    return returntype;
}

/******************************************************************************
** Name               : @fn ow_BKCTest
**
** Created from /on   : @spa / @23.02.2024
**
** Description        : @brief This function test the eeprom page 4.
**                             write known pattern, read back and verify
** InputValues        : @param Nil
**
** OutputValues       : @retval I2C_Return HAL_StatusTypeDef.
******************************************************************************/

uint8_t ow_BKCTest(void) {
    uint8_t ii, returntype;
    HAL_StatusTypeDef I2C_Return = HAL_OK;

    if (ds2431connectstatus == ChipOff) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }

    for (ii = 0; ii < 8; ii++)
        st_OwParamPage4.Reserved4[ii] = 0x0000;

    I2C_Return = owParamPage_write_ds2431(Page4);
    if (I2C_Return != HAL_OK) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }
    HAL_Delay(DS2484_DELAY * 10);

    st_OwParamPage4.Reserved4[0] = 0x4400;
    st_OwParamPage4.Reserved4[1] = 0x4401;
    st_OwParamPage4.Reserved4[2] = 0x4402;
    st_OwParamPage4.Reserved4[3] = 0x4403;
    st_OwParamPage4.Reserved4[4] = 0x4404;
    st_OwParamPage4.Reserved4[5] = 0x4405;
    st_OwParamPage4.Reserved4[6] = 0x4406;
    st_OwParamPage4.Reserved4[7] = 0x4407;

    I2C_Return = owParamPage_write_ds2431(Page4);
    if (I2C_Return != HAL_OK) {
        returntype = CMD_BKCTEST_FAIL;
        return returntype;
    }
    HAL_Delay(DS2484_DELAY * 10);

    for (ii = 0; ii < 8; ii++)
        st_OwParamPage4.Reserved4[ii] = 0x0000;

    I2C_Return = owParamPage_read_ds2431(Page4);

    if (st_OwParamPage4.Reserved4[0] == 0x4400 && st_OwParamPage4.Reserved4[1] == 0x4401 &&
        st_OwParamPage4.Reserved4[2] == 0x4402 && st_OwParamPage4.Reserved4[3] == 0x4403 &&
        st_OwParamPage4.Reserved4[4] == 0x4404 && st_OwParamPage4.Reserved4[5] == 0x4405 &&
        st_OwParamPage4.Reserved4[6] == 0x4406 && st_OwParamPage4.Reserved4[7] == 0x4407) {
        returntype = CMD_BKCTEST_PASS;
    } else {
        returntype = CMD_BKCTEST_FAIL;
    }
    return returntype;
}

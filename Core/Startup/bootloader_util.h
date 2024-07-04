
/**
 ******************************************************************************
 * @file           : bootloader_util.h
 * @brief          : Header for bootloader_util.c file.
 *                   This file contains the common defines for bootloader functoins.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2020 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __BOOTLOADER_UTIL_H
    #define __BOOTLOADER_UTIL_H

    #ifdef __cplusplus
extern "C" {
    #endif

    #include "stdbool.h"
    #include "string.h"

    /* Includes ------------------------------------------------------------------*/
    #if defined(G4xx) || defined(G4xx_BL)
        #include "stm32g4xx_ll_rtc.h"
        #include "stm32g4xx_ll_bus.h"
        #include "stm32g4xx_ll_pwr.h"
    #elif defined(H7xx) || defined(H7xx_BL)
        #include "stm32h7xx_hal.h"
        #include "stm32h7xx_hal_pwr.h"
    #elif defined(F7xx) || defined(F7xx_BL)
        #include "stm32f7xx_hal.h"
        #include "stm32f7xx_hal_pwr.h"
    #else
        #error "Configuration undefined/Unknown configuration"
    #endif // Configuration

    #if defined(USE_FULL_ASSERT)
        #include "stm32_assert.h"
    #endif /* USE_FULL_ASSERT */

/* Exported functions ------------------------------------------------------- */
void BLUtil_nvic_system_reset(void);
void BLUtil_enable_pwr_bkp_clock(void);
void BLUtil_disable_pwr_bkp_clock(void);
uint32_t BLUtil_read_backup_register(uint32_t BKP_REGISTER);
void BLUtil_write_backup_register(uint32_t BKP_REGISTER, uint32_t Data);
bool BLUtil_check_magic_code(void);
void BLUtil_Reset_magic_code(void);

    /* Exported Defines  ------------------------------------------------------- */
    #if defined(G4xx) || defined(G4xx_BL)

        #define GLOBAL_TAMP_REG         0x00 // TAMP_BKP0R
        #define GLOBAL_MAGIC_CODE       0x1FA0
        #define GLOBAL_MAGIC_CODE_RESET 0x0000 // Code used to Reset BKP after checking

        #define CHECK_MAGIC_CODE(Code)                    \
            BLUtil_enable_pwr_bkp_clock(void);            \
            BLUtil_read_backup_register(GLOBAL_TAMP_REG); \
            BLUtil_enable_pwr_bkp_clock(void);

    #elif defined(H7xx) || defined(H7xx_BL)
        #define GLOBAL_TAMP_REG         0x00   // TAMP_BKP0R
        #define GLOBAL_MAGIC_CODE       0x0000 // Bootloader Magic Code, Siehe Application Magic code in HMI_Projekt
        #define GLOBAL_MAGIC_CODE_RESET 0x0000 // Code used to Reset BKP after checking
        #define CHECK_MAGIC_CODE(Code)         /* Nicht definiert */
    #elif defined(F7xx) || defined(F7xx_BL)
        #define CHECK_MAGIC_CODE(Code)
    #else
        #error "Configuration undefined/Unknown configuration"
    #endif // Configuration

    #include <stdbool.h>

    /* EXCHGDATA Defines */

    /* FW-EXCHANGE-DATA Shared Memory Block between FW and Application */
    /* See: sfu_boot.c, FW_EXCHGDATA_START and FW_EXCHGDATA_END */

    /*!!WICHTIG!! - Für Applikationsprojekt
     * Die Definitionen in Applikation und SBSFU-Embedded-Projekt müssen immer konsistent sein.
     * Die ursprüngliche Definition muss immer in SBSFU-Embedded-Bootloader-Projekt überprüft werden!
     **/
    #define __EXCHGDATA_RAM __attribute__((section(".exchgdata")))

    #define SE_EXCHGDATA_LEN (sizeof(SE_FwExchgData_TypeDef)) /*!< FW ExchgData Length */

    /*!!WICHTIG!!
     * Die Definitionen in Applikation und Bootloader müssen immer konsistent sein.
     * Bei der Änderung der untenstehenden Struktur und Defines ist zu beachten, dass die Struktur
     * und Defines in mehreren Applikationen verwendet werden.
     **/
    #define SE_EXCHGDATA_RESERVED_SIZE \
        20U /* Size of Data reserved for future use (64 Bytes - Bytes used - ALIGN(4) Padding */

/* Kompatibel mit SE_FwRawHeaderTypeDef Elemente */
typedef struct {
    uint16_t BLVersion_Major;                     /*!< Bootloader Version - MAJOR */
    uint16_t BLVersion_Minor;                     /*!< Bootloader Version - MINOR */
    uint16_t BLVersion_Patch;                     /*!< Bootloader Version - PATCH */
    uint16_t FwImgVersion_Major;                  /*!< FwImage version - MAJOR */
    uint16_t FwImgVersion_Minor;                  /*!< FwImage version - MINOR */
    uint16_t FwImgVersion_Patch;                  /*!< FwImage version - PATCH */
    uint32_t BLVersion_Date;                      /*!< Bootloader Version - DATE (K_VERSION) */
    uint32_t FwImgVersion_Date;                   /*!< FwImage version - DATE (K_VERSION) */
    uint32_t FwImgType;                           /*!< FwImage Type */
    uint32_t BL_LastExecStatus;                   /* Last Firmware Installation/Download Status*/
    uint32_t BL_LastExecError;                    /* Last Firmware Execution Status */
    uint32_t ExchgData_SecurityCode;              /* ExchangeData Security Code -
                                                   * used by the Application to verify that the structure is correctly pointed  */
    uint32_t KeyLockState;                        // Value of eKeyLockState
    uint8_t Reserved[SE_EXCHGDATA_RESERVED_SIZE]; /*!< Reserved for future use */
} SE_FwExchgData_TypeDef;                         // Firmware Exchange Data

    #ifdef __cplusplus
}
    #endif

#endif /* __BOOTLOADER_UTIL_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

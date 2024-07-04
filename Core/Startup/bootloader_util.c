//=============================================================================
// Project name:  MultiDMR - Master
//=============================================================================
//  Functional description:
//  file:         bootloader_util.c
//  description:  Bootloader Utilities - Additional Bootloader related functions
//
//=============================================================================
//  $Author: ygo $
//  $LastChangedDate: 2021-07-30 11:14:48 +0100 (Fr, 30 Jul 2021) $
//  $Rev: 1 $
//=============================================================================

/**
 ******************************************************************************
 * @file           : bootloader_util.c
 * @brief          : Bootloader Utility Functions
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

/*** Include *****************************************************************/
#include "bootloader_util.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/******************************************************************************
 ** Name            : @fn BLUtil_nvic_system_reset
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief System neu starten
 **
 ** Calling         : @remark um in den Bootloader-Modus zu wechseln
 **								oder nach einem erfolgreichen Fw-Download
 **
 ** InputValues     : @param none
 ** OutputValues    : @retval none
 ******************************************************************************/
void BLUtil_nvic_system_reset(void)
{
  NVIC_SystemReset();
}

/******************************************************************************
 ** Name            : @fn BLUtil_enable_pwr_bkp_clock
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief Zugang zum PWR und BKP Register aktivieren
 **
 ** Calling         : @remark Typischerweise vor dem Zugriff auf BKP-Register
 **
 ** InputValues     : @param none
 ** OutputValues    : @retval none
 ******************************************************************************/
void BLUtil_enable_pwr_bkp_clock(void)
{
#if defined(G4xx) || defined(G4xx_BL)
  LL_APB1_GRP1_EnableClock(LL_APB1_GRP1_PERIPH_PWR); // Enable BKP and PWR clocks
  LL_PWR_EnableBkUpAccess();                         // Enable access to the RTC and backup registers
#elif defined(H7xx) || defined(H7xx_BL)
  /* Power management peripheral is active by default at startup level in STM32h7xx */
  HAL_PWREx_EnableBkUpReg();
  HAL_PWR_EnableBkUpAccess();
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
#else
#error "Configuration undefined/Unknown configuration"
#endif
}

/******************************************************************************
 ** Name            : @fn BLUtil_disable_pwr_bkp_clock
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief Zugang zum PWR und BKP Register aktivieren
 **
 ** Calling         : @remark Nach zugriff auf BKP-Register
 **
 ** InputValues     : @param none
 ** OutputValues    : @retval none
 ******************************************************************************/
void BLUtil_disable_pwr_bkp_clock(void)
{
#if defined(G4xx) || defined(G4xx_BL)
  LL_APB1_GRP1_DisableClock(LL_APB1_GRP1_PERIPH_PWR); // Enable BKP and PWR clocks
  LL_PWR_DisableBkUpAccess();                         // Enable access to the RTC and backup registers
#elif defined(H7xx) || defined(H7xx_BL)
  /* Power management peripheral is active by default at startup level in STM32h7xx */
  HAL_PWREx_DisableBkUpReg();
  HAL_PWR_DisableBkUpAccess();
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
#else
#error "Configuration undefined/Unknown configuration"
#endif
}

/******************************************************************************
 ** Name            : @fn BLUtil_read_backup_register
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief Backup Register lesen
 **
 ** Calling         : @remark Wird für den Infoaustausch zwischen
 **								Bootloader und Fw Verwendet
 **
 ** InputValues     : @param BackupRegister
 ** OutputValues    : @retval Wert aus Backup Register
 ******************************************************************************/
uint32_t BLUtil_read_backup_register(uint32_t BKP_REGISTER)
{
#if defined(G4xx) || defined(G4xx_BL)
  RTC_TypeDef *RTCx = NULL;
  return LL_RTC_BKP_GetRegister(RTCx, BKP_REGISTER);
#elif defined(H7xx) || defined(H7xx_BL)
  RTC_HandleTypeDef RtcHandle;
  RtcHandle.Instance = RTC;
  return HAL_RTCEx_BKUPRead(&RtcHandle, BKP_REGISTER);
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
#else
#error "Configuration undefined/Unknown configuration"
#endif
}

/******************************************************************************
 ** Name            : @fn BLUtil_write_backup_register
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief Backup Register schreiben
 **
 ** Calling         : @remark Wird für den Infoaustausch zwischen
 **								Bootloader und Fw Verwendet
 **
 ** InputValues     : @param BKP_REGISTER : BackupRegister
 ** 				  @param Data : Daten zu schreiben
 ** OutputValues    : @retval none
 ******************************************************************************/
void BLUtil_write_backup_register(uint32_t BKP_REGISTER, uint32_t Data)
{
#if defined(G4xx) || defined(G4xx_BL)
  RTC_TypeDef *RTCx = NULL;
  return LL_RTC_BKP_SetRegister(RTCx, BKP_REGISTER, Data);
#elif defined(H7xx) || defined(H7xx_BL)
  RTC_HandleTypeDef RtcHandle;
  RtcHandle.Instance = RTC;
  HAL_RTCEx_BKUPWrite(&RtcHandle, BKP_REGISTER, Data);
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
#else
#error "Configuration undefined/Unknown configuration"
#endif
}

/******************************************************************************
 ** Name            : @fn BLUtil_check_magic_code
 **
 ** Created from/on : @author ygo / @date 30.07.2021
 **
 ** Description     : @brief check Magic Code (with necessary clock configs)
 **
 ** Calling         : @remark vor Applikationsstart
 **
 ** InputValues     : @param none
 **
 ** OutputValues    : @retval true : Magic Code OK
 **							  false : Magic Code NOK
 ******************************************************************************/
bool BLUtil_check_magic_code(void)
{
  bool check_result = false;

#if defined(G4xx) || defined(G4xx_BL)
  BLUtil_enable_pwr_bkp_clock();

  check_result = ((BLUtil_read_backup_register(GLOBAL_TAMP_REG) == GLOBAL_MAGIC_CODE) ? true : false);

  BLUtil_enable_pwr_bkp_clock();

#elif defined(H7xx) || defined(H7xx_BL)
  BLUtil_enable_pwr_bkp_clock();
  check_result = ((BLUtil_read_backup_register(GLOBAL_TAMP_REG) == GLOBAL_MAGIC_CODE) ? true : false);
  BLUtil_enable_pwr_bkp_clock();
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
  check_result = true;
#else
#error "Configuration undefined/Unknown configuration"
#endif

  return check_result;
}

/******************************************************************************
 ** Name            : @fn BLUtil_Reset_magic_code
 **
 ** Created from/on : @author ygo / @date 23.09.2021
 **
 ** Description     : @brief Reset Magic Code (with necessary clock configs)
 **
 ** Calling         : @remark nach checkMagicCode
 **
 ** InputValues     : @param none
 **
 ** OutputValues    : @retval none
 ******************************************************************************/
void BLUtil_Reset_magic_code(void)
{

#if defined(G4xx) || defined(G4xx_BL)
  BLUtil_enable_pwr_bkp_clock();
  BLUtil_write_backup_register(GLOBAL_TAMP_REG, GLOBAL_MAGIC_CODE_RESET);
  BLUtil_enable_pwr_bkp_clock();

#elif defined(H7xx) || defined(H7xx_BL)
  BLUtil_enable_pwr_bkp_clock();
  BLUtil_write_backup_register(GLOBAL_TAMP_REG, GLOBAL_MAGIC_CODE_RESET);
  BLUtil_enable_pwr_bkp_clock();
#elif defined(F7xx) || defined(F7xx_BL)
#warning "Backup Register, MAGIC Code Storage not available for this Configuration"
#else
#error "Configuration undefined/Unknown configuration"
#endif
}

/* ----------- BOOTLOADER FIRMWARE DATA EXCHANGE FUNCTIONS - START ------------ */

/* ----------- BOOTLOADER FIRMWARE DATA EXCHANGE FUNCTIONS - END------------ */

#ifdef USE_FULL_ASSERT
/**
 * @brief  Reports the name of the source file and the source line number
 *         where the assert_param error has occurred.
 * @param  file: pointer to the source file name
 * @param  line: assert_param error line source number
 * @retval None
 */
void assert_failed(uint8_t *file, uint32_t line)
{

  /* User can add his own implementation to report the file name and line number,
     tex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
}
#endif /* USE_FULL_ASSERT */
/* NO MORE DEFINITIONS */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : TestBoard.h
 * @brief          : Header for TestBoard.c file.
 *                   This file contains the common defines of the Test function.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __TESTBOARD_H
    #define __TESTBOARD_H

    #ifdef __cplusplus
extern "C" {
    #endif

    /* Includes ------------------------------------------------------------------*/
    #include "stm32g4xx_hal.h"
    #include "stm32g4xx_ll_rcc.h"
    #include "stm32g4xx_ll_bus.h"
    #include "stm32g4xx_ll_crs.h"
    #include "stm32g4xx_ll_system.h"
    #include "stm32g4xx_ll_exti.h"
    #include "stm32g4xx_ll_cortex.h"
    #include "stm32g4xx_ll_utils.h"
    #include "stm32g4xx_ll_pwr.h"
    #include "stm32g4xx_ll_dma.h"
    #include "stm32g4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
    #include "s4-config/peripherie/SoftwareEnums.h"
    #include "msg.h"
    #include "serial.h"
    #include "version.h"
/* USER CODE END Includes */

    #define KEY_TIMEOUT        100 * 10 * 6 // 1 sec  = 100, 10 = sec 6- 5keys time 1 min
    #define CANTEST_TIMEOUT    100 * 5      // 100 - 1sec, 5 sec
    #define BKC_TIMEOUT        100 * 60 * 1 // 1 sec  = 100, 60 = 1min 1 = 3 min
    #define PASS_BLINK_COUNT   10
    #define FAIL_BLINK_COUNT   20
    #define LED_TIMEOUT        50   // 500 ms
    #define LED_NORMAL_TIMEOUT 100  // 1 sec
    #define TEST_MODE_T        0x54 // 0x54 correponds  to 'T',
    #define TEST_MODE_C        0x43 // 0x43 correspons to 'C'.
    #define TEST_MODE_2        0x32 // 0x32 correspons to '2'.

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern FDCAN_HandleTypeDef hfdcan2;
extern TIM_HandleTypeDef htim15;

/* USER CODE BEGIN EFP */

typedef enum {
    Unknown, ///< "Unknown"
    Pass,    ///< "Pass"
    Fail     ///< "UP keyTest Flag"
} test_result_t;

typedef enum {
    InitOnOff = 0, ///< "Led test Init on/off all led during Board Test"
    OnOff,         ///< "After completion of test red/green blink based on test result"
    Off            ///< "greeen/red led off"
} LED_Blink_on_off_t;

typedef enum {
    NoState,          ///< "No State "
    CANTest,          ///< "CAN Test"
    BKCTest,          ///< "BKC Test"
    UPKeyTest,        ///< "UP Key Test"
    DOWNKeyTest,      ///< "DOWN Key Test"
    LEFTKeyTest,      ///< "LEFT Key Test"
    RIGHTKeyTest,     ///< "RIGHT Key Test"
    STARTWELDKeyTest, ///< "STARTWELD Key Test"
    TestOver          ///< "Test Over"
} test_stage_t;

typedef enum {
    NotDone,    ///< "NotDone"
    InProgress, ///< "InProgress"
    Completed   ///< "Completed"
} test_state_t;

typedef struct {
    test_result_t CANTestResult;          ///< "CAN Test Flag"
    test_result_t BKCTestResult;          ///< "BKC Test Flag"
    test_result_t UPKeyTestResult;        ///< "UP keyTest Flag"
    test_result_t DOWNKeyTestResult;      ///< "DOWN Key Test Flag"
    test_result_t LEFTKeyTestResult;      ///< "Left Key Test Flag"
    test_result_t RIGHTKeyTestResult;     ///< "Right Key Test Flag"
    test_result_t StartWeldKeyTestResult; ///< "Start Weld Key Test Flag"
    test_result_t None;
} test_result_status_t;

typedef struct {
    test_state_t CANTestState;          ///< "CAN Test Flag"
    test_state_t BKCTestState;          ///< "BKC Test Flag"
    test_state_t UPKeyTestState;        ///< "UP keyTest Flag"
    test_state_t DOWNKeyTestState;      ///< "DOWN Key Test Flag"
    test_state_t LEFTKeyTestState;      ///< "Left Key Test Flag"
    test_state_t RIGHTKeyTestState;     ///< "Right Key Test Flag"
    test_state_t StartWeldKeyTestState; ///< "Start Weld Key Test Flag"
    test_state_t None;
} test_state_status_t;

/* USER CODE END EFP */

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */
/*** Prototypes of functions *************************************************/

void TestMode_KeyDetect(void);
void TestResult_CAN_Message_Send(void);
void TestMode_Init(void);
void BoardTest(void);
void TestResult_LEDToggle(test_stage_t stageresult);
void TestResult_summary(void);
uint8_t Get_TestMode(void);
uint32_t Get_Testresults(void);
uint32_t Get_TestState(void);
void TestmodeStart(void);
void TIM15_interrupt_handler(void);

    #ifdef __cplusplus
}
    #endif

#endif /* __TESTBOARD_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

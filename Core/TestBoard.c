/*
******************************************************************************
* @file TestBoard.c
* @author SPA
* @brief
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "types.h"
#include "tms.h"
#include "init.h"
#include "gData.h"
#include "gError.h"
#include <stdio.h>

// START of project specific includes
#include "main.h"
#include "ewmain.h"
#include "fdcan2.h"
#include "timers.h"
#include "i2c.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "gui.h"
#include "inout.h"
#include "TestBoard.h"
// END of project specific includes

#include "stm32g4xx.h"
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

// START of project specific includes
// ...
#include "DS2484.h"
#include "DS2431.h"

#include <string.h>
#include "stm32g4xx_it.h"

// END of project specific includes

/* NO MORE DEFINITIONS */
/*** Definition of variables *************************************************/
/* Private variables ---------------------------------------------------------*/
extern param_info_to_gui mapro_to_gui_update_param;
extern uint8_t rx_buff[8]; // used for CAN receive message
test_result_status_t TestResult;
test_state_status_t TestState;

uint32_t Test_Results_for_display;
uint32_t Test_State_for_display;
uint8_t KeyTestCompletflag = 0;
uint8_t TestMode = 0;
uint8_t PassFailflg = 0;
LED_Blink_on_off_t LEDResultDisplay = InitOnOff; // 0
uint16_t TestModeTimeout = 0;
uint16_t TestModeLEDTimeout = 0;
uint8_t Test_Tx_Buff[8];
uint16_t KeyPressDetect = 0;
HAL_StatusTypeDef TestBKCstatus;
uint64_t test_temp = 0x0;
test_stage_t e_TestStage = NoState;

/******************************************************************************
** Name               : @fn TestmodeStart
**
** Created from /on   : @author SPA / @date 17.01.2024
**
** Description        : @brief Test start
**
** Calling            : @remark called in fdcand2, when test message received
**
** InputValues        : @param  none
** OutputValues       : @retval none
******************************************************************************/
void TestmodeStart(void) {

    TestMode = 1;
    // reusing mapro_to_gui_update_param by GUI to enter into test mode
    mapro_to_gui_update_param.ID = rx_buff[0];
    mapro_to_gui_update_param.unit_id = rx_buff[1];
    mapro_to_gui_update_param.type = rx_buff[2];
    mapro_to_gui_update_param.image = rx_buff[3];
    mapro_to_gui_update_param.flag = MSG_0x110_setID;
    e_TestStage = CANTest;
    TestModeLEDTimeout = LED_NORMAL_TIMEOUT; // 100; // 1s toggle LED
    TestModeTimeout = CANTEST_TIMEOUT;       // 100 - 1sec, 5 sec
    TestState.CANTestState = InProgress;
    Serial_COM_PutString("\r\nEntering Test Mode ");
    Serial_COM_PutString("\r\nCAN TEST in Progress");
    rx_buff[0] = 'C'; //
    rx_buff[1] = 'A';
    rx_buff[2] = 'N';
    rx_buff[3] = 'T';
    rx_buff[4] = 'E';
    rx_buff[5] = 'S';
    rx_buff[6] = 'T';
    rx_buff[7] = 0;
    fdcan2_send(MSG_0x200, rx_buff);
}

/******************************************************************************
** Name               : @fn TIM15_interrupt_handler
**
** Created from /on   : @author SPA / @date 17.01.2024
**
** Description        : @brief Test start
**
** Calling            : @remark called in fdcand2
**
** InputValues        : @param  none
** OutputValues       : @retval none
******************************************************************************/
void TIM15_interrupt_handler(void) {
    static uint8_t toggleLedCount = 0;
    static uint16_t toggleLEDtime = 0;

    if (TestModeTimeout > 0)
        TestModeTimeout--;
    if (TestModeLEDTimeout > 0) {
        TestModeLEDTimeout--;
    }
    if (toggleLEDtime > 0) {
        toggleLEDtime--;
    }
    if (TestMode == 1) { //&& e_TestStage != TestOver && e_TestStage != NoState) {
        if (toggleLEDtime == 0) {
            toggleLEDtime = LED_NORMAL_TIMEOUT; // 1sec
            Toggle_LED(LED_1_GPIO_Port, LED_1_Pin);
            Toggle_LED(LED_2_GPIO_Port, LED_2_Pin);
            Toggle_LED(LED_EXT_GPIO_Port, LED_EXT_Pin);
        }
        if (TestModeLEDTimeout == 0 && LEDResultDisplay == InitOnOff) { // 0
            TestModeLEDTimeout = LED_NORMAL_TIMEOUT;                    // 1sec
            Toggle_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
            Toggle_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
            toggleLedCount++;
            if (toggleLedCount > 5) {
                LEDResultDisplay = Off; // 2;
                toggleLedCount = 0;
                TestModeLEDTimeout = 0;
                Reset_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
                Reset_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
            }
        } else if (LEDResultDisplay == OnOff) { // 1
            if (TestModeLEDTimeout == 0) {
                LEDResultDisplay = Off; // 2;
                PassFailflg = 0;
                Reset_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
                Reset_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
            }
        }
    }
}
/******************************************************************************
** Name               : @fn BoardTest
**
** Created from /on   : @author SPA / @date 30.10.2023
**
** Description        : @brief Test the Board Test
**
** Calling            : @remark called in while(1)
**
** InputValues        : @param  none
** OutputValues       : @retval none
******************************************************************************/
void BoardTest(void) {

    uint16_t sentdata, recvdata;

    if (TestMode == 1 && e_TestStage != TestOver) {
        switch (e_TestStage) {
        case NoState:
            break;
        case CANTest: {
            TestMode_KeyDetect();
            if (TestState.CANTestState == Completed && TestResult.CANTestResult == Pass) {
                e_TestStage = BKCTest;
            }
            if (TestModeTimeout == 0 && TestState.CANTestState == InProgress) {
                TestResult.CANTestResult = Fail;
                TestState.CANTestState = Completed;
                e_TestStage = BKCTest;
                // Serial_COM_PutString("\r\nCAN Test Completed, FAIL ");
            }

            if (e_TestStage == BKCTest) {
                // BKC Test
                TestResult_LEDToggle(CANTest);
                if (PassFailflg == FAIL_BLINK_COUNT) {
                    LEDResultDisplay = OnOff;                    // 1;
                    TestModeLEDTimeout = LED_NORMAL_TIMEOUT * 5; // 5sec ON
                    Reset_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
                    Set_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
                }
                TestResult_CAN_Message_Send();
                Serial_COM_PutString("\r\nBKC TEST in Progress ");
                TestModeTimeout = BKC_TIMEOUT; // 1 sec  = 100, 60 = 1min 3 = 3 min
                TestState.BKCTestState = InProgress;
                TestBKCstatus = HAL_OK;
                ow_setup_ds2484();
            }
        } break;
        case BKCTest: {
            TestMode_KeyDetect();
            sentdata = (uint16_t)0xAAAAAAAAAAAAAAAA;
            TestBKCstatus = ow_write_memory_row_ds2431(1, sentdata);
            // HAL_Delay(2000);
            TestMode_KeyDetect();
            if (TestBKCstatus != HAL_OK) {
                TestResult.BKCTestResult = Fail;
                TestState.BKCTestState = Completed;
                e_TestStage = UPKeyTest;
                // Serial_COM_PutString("\r\nBKC Test Completed, FAIL ");
            } else {
                TestBKCstatus = HAL_OK;
                test_temp = 0;
                TestBKCstatus = ow_read_memory_row_ds2431(1, &test_temp);
                // HAL_Delay(10000);
                TestMode_KeyDetect();
                if (TestBKCstatus != HAL_OK) {
                    TestResult.BKCTestResult = Fail;
                    TestState.BKCTestState = Completed;
                    // Serial_COM_PutString("\r\nBKC Test Completed, FAIL ");
                    e_TestStage = UPKeyTest;
                }
                recvdata = (uint16_t)test_temp;
                if (recvdata != sentdata) {
                    TestResult.BKCTestResult = Fail;
                    TestState.BKCTestState = Completed;
                    // Serial_COM_PutString("\r\nBKC Test Completed, FAIL ");
                    e_TestStage = UPKeyTest;
                } else {
                    TestResult.BKCTestResult = Pass;
                    TestState.BKCTestState = Completed;
                    e_TestStage = UPKeyTest;
                }
            } // else
        }
            if (e_TestStage == UPKeyTest) {

                TestResult_LEDToggle(BKCTest);
                if (PassFailflg == FAIL_BLINK_COUNT) {
                    LEDResultDisplay = OnOff;                    // 1;
                    TestModeLEDTimeout = LED_NORMAL_TIMEOUT * 5; // 5sec ON
                    Reset_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
                    Set_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
                }
                TestResult_CAN_Message_Send();
                // UPKey Test
                Serial_COM_PutString("\r\nKey Test in Progress ");
                if (KeyTestCompletflag == 0) // Keytest completed flag
                {
                    TestModeTimeout = KEY_TIMEOUT; // 1 sec  = 100, 60 = 1min 2=2 min
                    TestState.UPKeyTestState = InProgress;
                    TestState.DOWNKeyTestState = InProgress;
                    TestState.LEFTKeyTestState = InProgress;
                    TestState.RIGHTKeyTestState = InProgress;
                    TestState.StartWeldKeyTestState = InProgress;
                }
            }
            break;
        case UPKeyTest:
        case DOWNKeyTest:
        case LEFTKeyTest:
        case RIGHTKeyTest:
        case STARTWELDKeyTest: {
            TestMode_KeyDetect();
            if (e_TestStage == TestOver) {
                TestResult_CAN_Message_Send();
                TestResult_summary();
            }
        } break;

        default:
            break;
        } // switch
    }     // Test Mode
}

/******************************************************************************
** Name               : @fn TestMode_Init
**
** Created from /on   : @author SPA / @date 30.10.2023
**
** Description        : @brief Initialize parameter
**
** Calling            : @remark once at starting
**
** InputValues        : @param  none
** OutputValues       : @retval none
******************************************************************************/

void TestMode_Init(void) {
    TestResult.CANTestResult = Unknown;
    TestResult.BKCTestResult = Unknown;
    TestResult.UPKeyTestResult = Unknown;
    TestResult.DOWNKeyTestResult = Unknown;
    TestResult.LEFTKeyTestResult = Unknown;
    TestResult.RIGHTKeyTestResult = Unknown;
    TestResult.StartWeldKeyTestResult = Unknown;

    TestState.CANTestState = NotDone;
    TestState.BKCTestState = NotDone;
    TestState.UPKeyTestState = NotDone;
    TestState.DOWNKeyTestState = NotDone;
    TestState.LEFTKeyTestState = NotDone;
    TestState.RIGHTKeyTestState = NotDone;
    TestState.StartWeldKeyTestState = NotDone;

    e_TestStage = NoState;
    TestModeTimeout = 0;
    TestModeLEDTimeout = 0;
    TestMode = 0;
    MX_TIM15_Init();
    HAL_TIM_Base_Start_IT(&htim15);
}

/******************************************************************************
** Name               : @fn TestMode_KeyDetect
**
** Created from /on   : @author SPA / @date 30.10.2023
**
** Description        : @brief Detect the key press
**
** Calling            : @remark called in BoardTest()
**
** InputValues        : @param uint16_t  TobedetectKey
** OutputValues       : @retval none
******************************************************************************/
void TestMode_KeyDetect(void) {
    uint16_t KeyPressDetect = 0;

    inout_get_inputs(&KeyPressDetect);

    if (KeyPressDetect) {
        if (KeyPressDetect == INOUT_BUTTON_UP && TestState.UPKeyTestState != Completed) {
            TestResult.UPKeyTestResult = Pass;
            TestState.UPKeyTestState = Completed;
            TestResult_LEDToggle(UPKeyTest);
            Serial_COM_PutString("\r\nUp Key Test Completed, PASS ");
            TestResult_CAN_Message_Send();
        }
        if (KeyPressDetect == INOUT_BUTTON_DOWN && TestState.DOWNKeyTestState != Completed) {
            TestResult.DOWNKeyTestResult = Pass;
            TestState.DOWNKeyTestState = Completed;
            TestResult_LEDToggle(DOWNKeyTest);
            Serial_COM_PutString("\r\nDown Key Test Completed, PASS ");
            TestResult_CAN_Message_Send();
        }
        if (KeyPressDetect == INOUT_BUTTON_LEFT && TestState.LEFTKeyTestState != Completed) {
            TestResult.LEFTKeyTestResult = Pass;
            TestState.LEFTKeyTestState = Completed;
            TestResult_LEDToggle(LEFTKeyTest);
            Serial_COM_PutString("\r\nLeft Key Test Completed, PASS ");
            TestResult_CAN_Message_Send();
        }
        if (KeyPressDetect == INOUT_BUTTON_RIGHT && TestState.RIGHTKeyTestState != Completed) {
            TestResult.RIGHTKeyTestResult = Pass;
            TestState.RIGHTKeyTestState = Completed;
            TestResult_LEDToggle(RIGHTKeyTest);
            Serial_COM_PutString("\r\nRight Key Test Completed, PASS ");
            TestResult_CAN_Message_Send();
        }
        if (KeyPressDetect == INOUT_TORCH_SWITCH && TestState.StartWeldKeyTestState != Completed) {
            TestResult.StartWeldKeyTestResult = Pass;
            TestState.StartWeldKeyTestState = Completed;
            TestResult_LEDToggle(STARTWELDKeyTest);
            Serial_COM_PutString("\r\nTrigger Key Test Completed, PASS ");
            TestResult_CAN_Message_Send();
        }
    }
    if ((TestModeTimeout == 0 || KeyTestCompletflag == 1) &&
        (TestState.CANTestState == Completed && TestState.BKCTestState == Completed)) {
        if (TestState.UPKeyTestState == InProgress) {
            TestResult.UPKeyTestResult = Fail;
            TestState.UPKeyTestState = Completed;
            TestResult_LEDToggle(UPKeyTest);
            Serial_COM_PutString("\r\nUp Key Test Completed, FAIL ");
            TestResult_CAN_Message_Send();
        }
        if (TestState.DOWNKeyTestState == InProgress) {
            TestResult.DOWNKeyTestResult = Fail;
            TestState.DOWNKeyTestState = Completed;
            TestResult_LEDToggle(DOWNKeyTest);
            Serial_COM_PutString("\r\nDown Key Test Completed, FAIL ");
            TestResult_CAN_Message_Send();
        }
        if (TestState.LEFTKeyTestState == InProgress) {
            TestResult.LEFTKeyTestResult = Fail;
            TestState.LEFTKeyTestState = Completed;
            TestResult_LEDToggle(LEFTKeyTest);
            Serial_COM_PutString("\r\nLeft Key Test Completed, FAIL ");
            TestResult_CAN_Message_Send();
        }
        if (TestState.RIGHTKeyTestState == InProgress) {
            TestResult.RIGHTKeyTestResult = Fail;
            TestState.RIGHTKeyTestState = Completed;
            TestResult_LEDToggle(RIGHTKeyTest);
            Serial_COM_PutString("\r\nRight Key Test Completed, FAIL ");
            TestResult_CAN_Message_Send();
        }
        if (TestState.StartWeldKeyTestState == InProgress) {
            TestResult.StartWeldKeyTestResult = Fail;
            TestState.StartWeldKeyTestState = Completed;
            TestResult_LEDToggle(STARTWELDKeyTest);
            Serial_COM_PutString("\r\nTrigger Key Test Completed, FAIL ");
            TestResult_CAN_Message_Send();
        }
        if (PassFailflg == FAIL_BLINK_COUNT) {
            LEDResultDisplay = OnOff;                    // 1;
            TestModeLEDTimeout = LED_NORMAL_TIMEOUT * 5; // 5sec ON
            Reset_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
            Set_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
        }
    }
    if (TestState.StartWeldKeyTestState == Completed && TestState.RIGHTKeyTestState == Completed &&
        TestState.LEFTKeyTestState == Completed && TestState.DOWNKeyTestState == Completed &&
        TestState.UPKeyTestState == Completed)
        KeyTestCompletflag = 1;
    if (KeyTestCompletflag == 1 && TestState.CANTestState == Completed && TestState.BKCTestState == Completed)
        e_TestStage = TestOver;
}

/******************************************************************************
** Name               : @fn TestResult_CAN_Message_Send
**
** Created from /on   : @author SPA / @date 30.10.2023
**
** Description        : @brief Send the Test result status via CAN message
**
** Calling            : @remark in BoardTest() function
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void TestResult_CAN_Message_Send(void) {
    Test_Tx_Buff[0] = TestResult.CANTestResult;
    Test_Tx_Buff[1] = TestResult.BKCTestResult;
    Test_Tx_Buff[2] = TestResult.UPKeyTestResult;
    Test_Tx_Buff[3] = TestResult.DOWNKeyTestResult;
    Test_Tx_Buff[4] = TestResult.LEFTKeyTestResult;
    Test_Tx_Buff[5] = TestResult.RIGHTKeyTestResult;
    Test_Tx_Buff[6] = TestResult.StartWeldKeyTestResult;
    Test_Tx_Buff[7] = 0;
    fdcan2_send(MSG_0x200, Test_Tx_Buff);
}

/******************************************************************************
** Name               : @fn TestResult_LEDToggle
**
** Created from /on   : @author SPA / @date 2.11.2023
**
** Description        : @brief Toggle LED 250ms for 5 times for key press
**
** Calling            : @remark in BoardTest() function
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void TestResult_LEDToggle(test_stage_t stageresult) {

    if (stageresult == CANTest && TestResult.CANTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == CANTest && TestResult.CANTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == BKCTest && TestResult.BKCTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == BKCTest && TestResult.BKCTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == UPKeyTest && TestResult.UPKeyTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == UPKeyTest && TestResult.UPKeyTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == DOWNKeyTest && TestResult.DOWNKeyTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == DOWNKeyTest && TestResult.DOWNKeyTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == LEFTKeyTest && TestResult.LEFTKeyTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == LEFTKeyTest && TestResult.LEFTKeyTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == RIGHTKeyTest && TestResult.RIGHTKeyTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == RIGHTKeyTest && TestResult.RIGHTKeyTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (stageresult == STARTWELDKeyTest && TestResult.StartWeldKeyTestResult == Pass)
        PassFailflg = PASS_BLINK_COUNT;
    else if (stageresult == STARTWELDKeyTest && TestResult.StartWeldKeyTestResult == Fail)
        PassFailflg = FAIL_BLINK_COUNT;
    if (PassFailflg == PASS_BLINK_COUNT) {
        LEDResultDisplay = OnOff;                // 1;
        TestModeLEDTimeout = LED_NORMAL_TIMEOUT; // 1sec ON
        Set_LED(LED_STAT_GPIO_Port, LED_STAT_Pin);
        Reset_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);
    }
}

/******************************************************************************
** Name               : @fn TestResult_summary
**
** Created from /on   : @author SPA / @date 6.11.2023
**
** Description        : @brief Summary the result through serial
**
** Calling            : @remark in BoardTest() function
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void TestResult_summary(void) {

    Serial_COM_PutString("\r\n\t");
    Serial_COM_PutString("\r\nTest Summary: ");
    Serial_COM_PutString("\r\n\t");

    if (TestResult.CANTestResult == Pass)
        Serial_COM_PutString("\r\nCAN Test - Pass ");
    else
        Serial_COM_PutString("\r\nCAN Test - Fail ");
    if (TestResult.BKCTestResult == Pass)
        Serial_COM_PutString("\r\nBKC Test - Pass ");
    else
        Serial_COM_PutString("\r\nBKC Test - Fail ");
    if (TestResult.UPKeyTestResult == Pass)
        Serial_COM_PutString("\r\nUp Key - Pass ");
    else
        Serial_COM_PutString("\r\nUp Key - Fail ");
    if (TestResult.DOWNKeyTestResult == Pass)
        Serial_COM_PutString("\r\nDown Key - Pass ");
    else
        Serial_COM_PutString("\r\nDown Key - Fail ");
    if (TestResult.LEFTKeyTestResult == Pass)
        Serial_COM_PutString("\r\nLeft Key - Pass ");
    else
        Serial_COM_PutString("\r\nLeft Key - Fail ");
    if (TestResult.RIGHTKeyTestResult == Pass)
        Serial_COM_PutString("\r\nRight Key - Pass ");
    else
        Serial_COM_PutString("\r\nRight Key - Fail ");
    if (TestResult.StartWeldKeyTestResult == Pass)
        Serial_COM_PutString("\r\nTrigger Key - Pass ");
    else
        Serial_COM_PutString("\r\nTrigger Key - Fail ");

    Serial_COM_PutString("\r\n -------End-----------");
}

/******************************************************************************
** Name               : @fn Get_TestMode
**
** Created from /on   : @author APR / @date 07.11.2023
**
** Description        : @brief Returns whether or not in Test Mode
**
** Calling            : @remark To check the Test Mode state
**
** InputValues        : @param none
** OutputValues       : @retval uint8_t. 1 = TestMode. 0 = Not in TestMode
******************************************************************************/
uint8_t Get_TestMode(void) {
    return TestMode;
}

/******************************************************************************
** Name               : @fn Get_Testresults
**
** Created from /on   : @author APR / @date 07.11.2023
**
** Description        : @brief Returns the results of tests
**
** Calling            : @remark To get test results
**
** InputValues        : @param none
** OutputValues       : @retval uint32_t. Information of test results
******************************************************************************/
uint32_t Get_Testresults(void) {
    Test_Results_for_display = 0;
    Test_Results_for_display |= TestResult.StartWeldKeyTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.RIGHTKeyTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.LEFTKeyTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.DOWNKeyTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.UPKeyTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.BKCTestResult;
    Test_Results_for_display = (Test_Results_for_display << 2);
    Test_Results_for_display |= TestResult.CANTestResult;

    return Test_Results_for_display;
}

/******************************************************************************
** Name               : @fn Get_TestState
**
** Created from /on   : @author APR / @date 10.11.2023
**
** Description        : @brief Returns the current state of tests
**
** Calling            : @remark To get test states
**
** InputValues        : @param none
** OutputValues       : @retval uint32_t. Information of states
******************************************************************************/
uint32_t Get_TestState(void) {
    Test_State_for_display = 0;
    Test_State_for_display |= TestState.StartWeldKeyTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.RIGHTKeyTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.LEFTKeyTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.DOWNKeyTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.UPKeyTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.BKCTestState;
    Test_State_for_display = (Test_State_for_display << 2);
    Test_State_for_display |= TestState.CANTestState;

    return Test_State_for_display;
}

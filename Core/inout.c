/**
 ******************************************************************************
 * @file    inout.c
 * @author  WBO
 * @brief   Push button and led handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#include "main.h"

#include "inout.h"

#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_gpio.h"

#define WAIT_CNT         30
#define STAT_LED_CNT_MAX 250 // 25 * 4 msec = 100 msec
#define TORCH_BT_LED_OUT 1

static int trigger_cnt = 0;
static uint16_t output_mask = 0;
static int trigger_stat_led_cnt = 0;

static void refresh_outputs();

extern uint8_t TestMode;

/******************************************************************************
** Name               : @fn Toggle_LED
** Created from /on   : @author SPA / @date 18.01.2024
** Description        : @brief Toggle gpio pin
** Calling            : @
** InputValues        : @param gpio pin, and gpio port
******************************************************************************/
void Toggle_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_TogglePin(GPIOx, GPIO_Pin);
}

/******************************************************************************
** Name               : @fn Reset_LED
** Created from /on   : @author SPA / @date 18.01.2024
** Description        : @brief reset gpio pin
** Calling            : @
** InputValues        : @param gpio pin, and gpio port
******************************************************************************/

void Reset_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_RESET);
}

/******************************************************************************
** Name               : @fn Set_LED
** Created from /on   : @author SPA / @date 18.01.2024
** Description        : @brief set gpio pin
** Calling            : @
** InputValues        : @param gpio pin, and gpio port
******************************************************************************/

void Set_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin) {
    HAL_GPIO_WritePin(GPIOx, GPIO_Pin, GPIO_PIN_SET);
}

/******************************************************************************
** Name               : @fn inout_get_inputs
** Created from /on   : @author SGE / @date 27.09.2021
** Description        : @brief input reader and coversion to a p_inputs Word
** Calling            : @
** InputValues        : @param p_inputs
******************************************************************************/
void inout_get_inputs(uint16_t *p_inputs) {

    *p_inputs = 0;

    if (HAL_GPIO_ReadPin(BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin)) {
        *p_inputs |= INOUT_BUTTON_DOWN;
    }

    if (HAL_GPIO_ReadPin(BUTTON_UP_GPIO_Port, BUTTON_UP_Pin)) {
        *p_inputs |= INOUT_BUTTON_UP;
    }

    if (HAL_GPIO_ReadPin(BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin)) {
        *p_inputs |= INOUT_BUTTON_RIGHT;
    }

    if (HAL_GPIO_ReadPin(BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin)) {
        *p_inputs |= INOUT_BUTTON_LEFT;
    }

    if (!HAL_GPIO_ReadPin(TORCH_SWITCH_GPIO_Port, TORCH_SWITCH_Pin)) {
        *p_inputs |= INOUT_TORCH_SWITCH;
    }
}

/******************************************************************************
** Name               : @fn inout_set_outputs
** Created from /on   : @author ´WBO(IBV) / @date 00.00.2021
** Description        : @brief setter for output bits
** Calling            : @
** InputValues        : @param new Output values
******************************************************************************/
void inout_set_outputs(uint16_t outputs) {
    output_mask = outputs;
}

/******************************************************************************
** Name               : @fn inout_trigger_outputs
** Created from /on   : @author ´WBO(IBV) / @date 00.00.2021
** Description        : @brief trigger / update all outputs with interleaved PWM
** Calling            : @ cyclic >200Hz

******************************************************************************/
void inout_trigger_outputs(void) {
    if (!(output_mask & INOUT_LED_EXT) && (output_mask & INOUT_LED_RH)) {
        /* switch between left and right LED */
        trigger_cnt++;
        trigger_cnt %= 2;
    }

    if (trigger_stat_led_cnt > STAT_LED_CNT_MAX) {
        trigger_stat_led_cnt = 0;
    }
    trigger_stat_led_cnt++;

    if (TestMode == 0)
        refresh_outputs();
}

/******************************************************************************
** Name               : @fn refresh_outputs
** Created from /on   : @author ´WBO(IBV) / @date 00.00.2021
** Description        : @brief cyclic output for GPIOs, needs to be called frequence > 200Hz.for RH LEDs PWM
** Calling            : @
** InputValues        : @param none
******************************************************************************/
static void refresh_outputs() {
    GPIO_PinState Ext_Out;
    static uint16_t count = 0;
    if (output_mask & INOUT_LED_EXT) {
        // EXT LED overrides RH LEDs
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        Ext_Out = GPIO_PIN_SET;
    } else if (output_mask & INOUT_LED_RH) {
        // Switch red hood LEDS interleaved
        if (!trigger_cnt) {
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_SET);
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        } else {
            HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
            HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_SET);
        }
        // Clear EXT Led
        Ext_Out = GPIO_PIN_RESET;
    } else {
        HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
        HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
        // Clear EXT Led
        Ext_Out = GPIO_PIN_RESET;
    }
#ifdef TORCH_BT_LED_OUT
    if (!HAL_GPIO_ReadPin(TORCH_SWITCH_GPIO_Port, TORCH_SWITCH_Pin)) {
        Ext_Out = GPIO_PIN_SET;
        count = 500; // value 500 is based on 4ms timer interval. 500*4ms =2000ms
    } else if (count) {
        count--;
        if (count == 0) {
            Ext_Out = GPIO_PIN_RESET;
        } else {
            Ext_Out = GPIO_PIN_SET;
        }
    }
#endif
    HAL_GPIO_WritePin(LED_EXT_GPIO_Port, LED_EXT_Pin, Ext_Out);

    if (trigger_stat_led_cnt == STAT_LED_CNT_MAX) {
        HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);
    }
}

/******************************************************************************
** Name               : @fn IO_Init
** Created from /on   : @author sge / @date 23.09.2022
** Description        : @brief Init von IOs
** Calling            : @remark at init
** InputValues        : @param
******************************************************************************/
void IO_Init() {

    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    // Init of Input Pins #####################################################
    GPIO_InitStruct.Pin = BUTTON_DOWN_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BUTTON_DOWN_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = BUTTON_UP_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BUTTON_UP_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = BUTTON_RIGHT_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BUTTON_RIGHT_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = BUTTON_LEFT_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_DOWN;
    LL_GPIO_Init(BUTTON_LEFT_GPIO_Port, &GPIO_InitStruct);

    // Init of Input Pins #####################################################
}

/******************************************************************************
** Name               : @fn IOProcessKeys
** Created from /on   : @author sge / @date 23.09.2022
** Description        : @brief Cyclic calling to process key inputs to GUI
** Calling            : @remark von GUI in EW Process
** InputValues        : @param
**
** OutputValues       : @retval uint32_t Anzahl der Events
******************************************************************************/

uint32_t IOProcessKeys(CoreRoot RootObject) {
    uint32_t ret = 0;
    uint16_t inputs_sum;
    static uint16_t inputs_last;
    uint16_t temp;
    inout_get_inputs(&inputs_sum);

    if (inputs_sum != inputs_last) // trigger only if changed
    {
        temp = inputs_sum ^ inputs_last;
        if (temp & INOUT_BUTTON_DOWN) {
            if (inputs_sum & INOUT_BUTTON_DOWN) {
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeDown, 0, 1);
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeDown, 0, 0);
            } else {
                // CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeDown, 0, 0);
            }
            ret++;
        }
        if (temp & INOUT_BUTTON_UP) {
            if (inputs_sum & INOUT_BUTTON_UP) {
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeUp, 0, 1);
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeUp, 0, 0);
            } else {
                // CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeUp, 0, 0);
            }
            ret++;
        }
        if (temp & INOUT_BUTTON_LEFT) {
            if (inputs_sum & INOUT_BUTTON_LEFT) {
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeLeft, 0, 1);
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeLeft, 0, 0);
            } else {
                // CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeLeft, 0, 0);
            }
            ret++;
        }
        if (temp & INOUT_BUTTON_RIGHT) {
            if (inputs_sum & INOUT_BUTTON_RIGHT) {
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeRight, 0, 1);
                CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeRight, 0, 0);
            } else {
                // CoreRoot__DriveKeyboardHitting(RootObject, CoreKeyCodeRight, 0, 0);
            }
            ret++;
        }

        inputs_last = inputs_sum;
    }
    return ret;
}

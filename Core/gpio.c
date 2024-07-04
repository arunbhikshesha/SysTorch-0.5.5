/*
******************************************************************************
* @file: gpio.c
* @author: APR
* @brief: GPIO initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "gpio.h"
#include "stm32g4xx_ll_gpio.h"
#include "stm32g4xx_ll_rcc.h"
#include "stm32g4xx_ll_bus.h"
#include "main.h"

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : MX_GPIO_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise SPI 1
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
uint8_t PCB_Detection_Result;
uint32_t LED_EXT_Pin;
GPIO_TypeDef *LED_EXT_GPIO_Port;
uint32_t LED_1_Pin;
GPIO_TypeDef *LED_1_GPIO_Port;
uint32_t LED_2_Pin;
GPIO_TypeDef *LED_2_GPIO_Port;
void MX_GPIO_Init(void) {
    LL_GPIO_InitTypeDef GPIO_InitStruct = {0};

    /* GPIO Ports Clock Enable */
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOC);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOA);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOB);
    LL_AHB2_GRP1_EnableClock(LL_AHB2_GRP1_PERIPH_GPIOD);

    /**/
    LL_GPIO_ResetOutputPin(DSP_DC_GPIO_Port, DSP_DC_Pin);

    /**/
    LL_GPIO_ResetOutputPin(DSP_RST_GPIO_Port, DSP_RST_Pin);

    /**/
    LL_GPIO_ResetOutputPin(LED_1_GPIO_Port, LED_1_Pin);
    LL_GPIO_ResetOutputPin(LED_2_GPIO_Port, LED_2_Pin);
    LL_GPIO_ResetOutputPin(LED_EXT_GPIO_Port, LED_EXT_Pin);

    /**/

    /**/
    GPIO_InitStruct.Pin = TORCH_SWITCH_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(TORCH_SWITCH_GPIO_Port, &GPIO_InitStruct);

    /*Detect Old and New PCB*/
    GPIO_InitStruct.Pin = PCB_Detection_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_INPUT;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(PCB_Detection_PORT, &GPIO_InitStruct);
    HAL_Delay(1); // Delay before Pin reading
    PCB_Detection_Result = HAL_GPIO_ReadPin(PCB_Detection_PORT, PCB_Detection_Pin);
    if (PCB_Detection_Result) { // OLD PCB TC22-E01B

        LED_EXT_Pin = LL_GPIO_PIN_8;
        LED_EXT_GPIO_Port = GPIOC;

        LED_1_Pin = LL_GPIO_PIN_9;
        LED_1_GPIO_Port = GPIOC;

        LED_2_Pin = LL_GPIO_PIN_10;
        LED_2_GPIO_Port = GPIOC;

    } else { // New PCB TC22-V01A

        LED_EXT_Pin = LL_GPIO_PIN_10;
        LED_EXT_GPIO_Port = GPIOC;

        LED_1_Pin = LL_GPIO_PIN_10;
        LED_1_GPIO_Port = GPIOD;

        LED_2_Pin = LL_GPIO_PIN_11;
        LED_2_GPIO_Port = GPIOD;
    }
    /**/
    GPIO_InitStruct.Pin = DSP_DC_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(DSP_DC_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = DSP_CS_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_VERY_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_UP;
    LL_GPIO_Init(DSP_DC_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = DSP_RST_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_HIGH;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(DSP_RST_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = LED_EXT_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_EXT_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = LED_1_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_1_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = LED_2_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_2_GPIO_Port, &GPIO_InitStruct);

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

    /**/
    GPIO_InitStruct.Pin = LED_STAT_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_STAT_GPIO_Port, &GPIO_InitStruct);

    /**/
    GPIO_InitStruct.Pin = LED_ERR_Pin;
    GPIO_InitStruct.Mode = LL_GPIO_MODE_OUTPUT;
    GPIO_InitStruct.Speed = LL_GPIO_SPEED_FREQ_LOW;
    GPIO_InitStruct.OutputType = LL_GPIO_OUTPUT_PUSHPULL;
    GPIO_InitStruct.Pull = LL_GPIO_PULL_NO;
    LL_GPIO_Init(LED_ERR_GPIO_Port, &GPIO_InitStruct);
    HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_RESET);

    HAL_GPIO_WritePin(LED_ERR_GPIO_Port, LED_ERR_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_EXT_GPIO_Port, LED_EXT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_STAT_GPIO_Port, LED_STAT_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_1_GPIO_Port, LED_1_Pin, GPIO_PIN_RESET);
    HAL_GPIO_WritePin(LED_2_GPIO_Port, LED_2_Pin, GPIO_PIN_RESET);
}

/* NO DEFINITIONS */

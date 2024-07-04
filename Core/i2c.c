/*
******************************************************************************
* @file: i2c.c
* @author: APR
* @brief: I2C initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "i2c.h"
#include "main.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
I2C_HandleTypeDef hi2cOneWire;

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : MX_I2C2_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise I2C 2
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_I2C2_Init(void) {
    hi2cOneWire.Instance = I2C2;
    hi2cOneWire.Init.Timing = 0x00303D5B;
    hi2cOneWire.Init.OwnAddress1 = 0;
    hi2cOneWire.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2cOneWire.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2cOneWire.Init.OwnAddress2 = 0;
    hi2cOneWire.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2cOneWire.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2cOneWire.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2cOneWire) != HAL_OK) {
        Error_Handler();
    }
    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2cOneWire, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }
    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2cOneWire, 0) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : MX_I2C3_Init
 **
 ** Created from/on : APR / 22.05.2024
 **
 ** Description     : Initialise I2C 3
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_I2C3_Init(void) {
    hi2cOneWire.Instance = I2C3;
    hi2cOneWire.Init.Timing = 0x00303D5B;
    hi2cOneWire.Init.OwnAddress1 = 0;
    hi2cOneWire.Init.AddressingMode = I2C_ADDRESSINGMODE_7BIT;
    hi2cOneWire.Init.DualAddressMode = I2C_DUALADDRESS_DISABLE;
    hi2cOneWire.Init.OwnAddress2 = 0;
    hi2cOneWire.Init.OwnAddress2Masks = I2C_OA2_NOMASK;
    hi2cOneWire.Init.GeneralCallMode = I2C_GENERALCALL_DISABLE;
    hi2cOneWire.Init.NoStretchMode = I2C_NOSTRETCH_DISABLE;
    if (HAL_I2C_Init(&hi2cOneWire) != HAL_OK) {
        Error_Handler();
    }
    /** Configure Analogue filter
     */
    if (HAL_I2CEx_ConfigAnalogFilter(&hi2cOneWire, I2C_ANALOGFILTER_ENABLE) != HAL_OK) {
        Error_Handler();
    }
    /** Configure Digital filter
     */
    if (HAL_I2CEx_ConfigDigitalFilter(&hi2cOneWire, 0) != HAL_OK) {
        Error_Handler();
    }
}
/* NO DEFINITIONS */

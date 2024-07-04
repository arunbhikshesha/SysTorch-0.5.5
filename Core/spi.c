/*
******************************************************************************
* @file: spi.c
* @author: APR
* @brief: SPI initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "spi.h"
#include "main.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
SPI_HandleTypeDef hspi1;

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : MX_SPI1_Init
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
void MX_SPI1_Init(void) {

    /* SPI1 parameter configuration*/
    hspi1.Instance = SPI1;
    hspi1.Init.Mode = SPI_MODE_MASTER;
    hspi1.Init.Direction = SPI_DIRECTION_2LINES;
    hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
    hspi1.Init.CLKPolarity = SPI_POLARITY_LOW;
    hspi1.Init.CLKPhase = SPI_PHASE_1EDGE;
    hspi1.Init.NSS = SPI_NSS_SOFT;
    hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_32;
    hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
    hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
    hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
    hspi1.Init.CRCPolynomial = 7;
    hspi1.Init.CRCLength = SPI_CRC_LENGTH_DATASIZE;
    hspi1.Init.NSSPMode = SPI_NSS_PULSE_DISABLE;
    if (HAL_SPI_Init(&hspi1) != HAL_OK) {
        Error_Handler();
    }
}

/* NO DEFINITIONS */

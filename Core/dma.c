/*
******************************************************************************
* @file: dma.c
* @author: APR
* @brief: DMA initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "dma.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : MX_DMA_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise DMA
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_DMA_Init(void) {

    /* DMA controller clock enable */
    __HAL_RCC_DMAMUX1_CLK_ENABLE();
    __HAL_RCC_DMA1_CLK_ENABLE();

    /* DMA interrupt init */
    /* DMA1_Channel1_IRQn interrupt configuration */
    HAL_NVIC_SetPriority(DMA1_Channel1_IRQn, 0, 0);
    HAL_NVIC_EnableIRQ(DMA1_Channel1_IRQn);
}
/* NO DEFINITIONS */

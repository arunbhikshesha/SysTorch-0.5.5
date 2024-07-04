/*
******************************************************************************
* @file: serial.c
* @author: APR
* @brief: Serial initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include <string.h>
#include "serial.h"
#include "stm32g4xx_hal.h"
#include "main.h"

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
#define SERIAL_COM_TRACE_TIME_OUT ((uint32_t)100U) /*!< Serial PutString Timeout*/
#define BUFFER_SIZE               10

/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
UART_HandleTypeDef huart1;

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : MX_UART1_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise UART 1
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_UART1_Init(void) {
    huart1.Instance = USART1;
    huart1.Init.BaudRate = 115200U;
    huart1.Init.WordLength = UART_WORDLENGTH_8B;
    huart1.Init.StopBits = UART_STOPBITS_1;
    huart1.Init.Parity = UART_PARITY_NONE;
    huart1.Init.HwFlowCtl = UART_HWCONTROL_NONE;
    huart1.Init.Mode = UART_MODE_RX | UART_MODE_TX;

    huart1.AdvancedInit.AdvFeatureInit = UART_ADVFEATURE_RXOVERRUNDISABLE_INIT;
    huart1.AdvancedInit.OverrunDisable = UART_ADVFEATURE_OVERRUN_DISABLE;

    if (HAL_UART_Init(&huart1) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : Serial_COM_PutString
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : print the string provided as parameter
 **
 ** Calling         : to print a message
 **
 ** InputValues     : uint8_t *
 ** OutputValues    : none
 **********************************************************/
void Serial_COM_PutString(char *pString) {
    uint16_t length = 0U;

    /* Check the pointers allocation */
    if (pString == NULL) {
        Error_Handler();
    }

    while (pString[length] != (uint8_t)'\0') {
        length++;
    }
    if (HAL_UART_Transmit(&huart1, (uint8_t *)pString, length, SERIAL_COM_TRACE_TIME_OUT) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : ConvertInt64toASCIIstring
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Converts integer to ASCII
 **
 ** Calling         :
 **
 ** InputValues     : uint8_t *, uint64
 ** OutputValues    : none
 **********************************************************/
void ConvertInt64toASCIIstring(uint8_t *ASCIIstring, uint64_t Value) {
    uint8_t c = BUFFER_SIZE;
    while (c > 0 && Value != 0) {
        ASCIIstring[--c] = Value % 10 + '0';
        Value /= 10;
    }
    while (c > 0)
        ASCIIstring[--c] = '0';
}
/* NO DEFINITIONS */

/*
******************************************************************************
* @file: serial.h
* @author: APR
* @brief: Initialise UART and related functions
******************************************************************************
*
******************************************************************************
*/

#ifndef _SERIAL_H
#define _SERIAL_H

/*** Include *****************************************************************/
#include "types.h"

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void MX_UART1_Init(void);
void Serial_COM_PutString(char *pString);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

#endif //_SERIAL_H
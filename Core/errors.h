/*
******************************************************************************
* @file errors.h
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

#ifndef _ERRORS_H
#define _ERRORS_H

/*** Include *****************************************************************/
#include "types.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
typedef struct {
    uint8_t ucErrorMainID;   // The main ID with is shown as "E__"
    uint8_t ucErrorSubID;    // The sub ID with is shown as "-__"
    uint8_t ucErrorPriority; // Showing priority
} ErrorTableStruct;

#define ERROR_TABLE_CNT 64
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void errorHandler(void);
void errors_Handler(void);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

#endif //_ERRORS_H

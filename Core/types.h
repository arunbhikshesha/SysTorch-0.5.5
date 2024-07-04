/*
******************************************************************************
* @file types.h
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

#ifndef _TYPES_H
#define _TYPES_H

/*** Include *****************************************************************/
#include <stdint.h>
#include <stdbool.h>
#include <float.h>
#include <math.h>
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
typedef float float32_t;
typedef enum stdRetVal {
    E_OK = 0u,
    E_NOK
    // ...
} stdRetVal_t;
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
bool close_zero(float32_t fl);
/* NO MORE DEFINITIONS */

#endif //_TYPES_H

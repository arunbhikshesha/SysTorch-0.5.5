/*
******************************************************************************
* @file init.h
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

#ifndef _INIT_H
#define _INIT_H

/*** Include *****************************************************************/
#include "types.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
typedef enum init_states {
    e_INIT_ALL_DONE = 0u,
    // START of project specific enum entries
    // ...
    // END of project specific enum entries
    e_INIT_N
} init_states_t;

/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void init_initUcAndSubModules(void);
bool init_getInitState(init_states_t peripheral);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

#endif //_INIT_H

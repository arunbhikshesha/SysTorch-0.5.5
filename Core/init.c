/*
******************************************************************************
* @file init.c
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "init.h"
#include "tms.h"
#include "gData.h"

// START of project specific includes
// ...
// END of project specific includes

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/

static bool init_states[e_INIT_N] = {false};
gData_coredata_t gData;
// START of project specific definitions
// ...
// END of project specific definitions
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void sysclockInit(void);
static void peripheralInit(void);
static void hwInit(void);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/******************************************************************************
** Name               : @fn  init_initUcAndSubModules
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void init_initUcAndSubModules(void) {
    TMS_Init();

    hwInit();
    peripheralInit();
    // START of project specific code
    // ...
    // END of project specific code
}

/******************************************************************************
** Name               : @fn  init_getInitState
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
bool init_getInitState(init_states_t peripheral) {
    return init_states[peripheral];
}

/******************************************************************************
** Name               : @fn  init_sysclock
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void sysclockInit(void) {
    // START of project specific code
    // ...
    // END of project specific code
}

/******************************************************************************
** Name               : @fn  peripheralInit
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static void peripheralInit(void) {
    // START of project specific code
    // ...
    // END of project specific code
}

/******************************************************************************
** Name               : @fn  hwInit
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static void hwInit(void) {
    // START of project specific code
    // ...
    // END of project specific code
}
/* NO MORE DEFINITIONS */

/**
 ******************************************************************************
 * File Name          : iwdg.c
 * Description        : This file provides code for the configuration
 *                      of the IWDG instances.
 ******************************************************************************
 *
 ******************************************************************************
 */

/*** Include *****************************************************************/
#include "iwdg.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/

/******************************************************************************
** Name               : @fn MX_IWDG_Init
**
** Created from /on   : @author / @date
**
** Description        : @brief  Watchdog initialization. Generaded by STM32Cube.
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void MX_IWDG_Init(void) {
// Disable IWDG during debug halt
#ifdef DEBUG
// DBGMCU->APB1FZR1 |= DBGMCU_APB1FZR1_DBG_IWDG_STOP;
#endif
}

/******************************************************************************
 ** Name            : @fn iwdg_reload
 **
 ** Created from/on : @author sge /06.08.2021
 **
 ** Description     : @brief Reloads the watchdog. as replacement for unfunctional static inline
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  none
 **
 ** OutputValues    : @retval none
 ******************************************************************************/
void iwdg_reload(void) {
    // LL_IWDG_ReloadCounter(IWDG);
}

// START of project specific code
// ...
// END of project specific code

/* NO MORE DEFINITIONS */

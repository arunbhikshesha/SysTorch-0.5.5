/*
******************************************************************************
* @file main.c
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef _IWDG_H
#define _IWDG_H

/*** Include *****************************************************************/
#ifdef G4XX
    #include "stm32g4xx.h"
    #include "stm32g4xx_ll_iwdg.h"
#endif // G4XX
#ifdef H7XX
    #include "stm32h7xx.h"
    #include "stm32h7xx_ll_iwdg.h"
#endif // H7XX

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/

void MX_IWDG_Init(void);
void iwdg_reload(void);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/******************************************************************************
 ** Name            : @fn iwdg_reload
 **
 ** Created from/on : @author mke / @date 20.04.2021
 **
 ** Description     : @brief Reloads the watchdog.
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  none
 **
 ** OutputValues    : @retval none
 ******************************************************************************/
//__STATIC_INLINE void iwdg_reload(void)
//{
//  LL_IWDG_ReloadCounter(IWDG);
//}
/* NO MORE DEFINITIONS */

#endif /* _IWDG_H */

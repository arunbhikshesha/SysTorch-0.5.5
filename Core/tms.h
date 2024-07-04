/*
******************************************************************************
* @file tms.h
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

#ifndef _TMS_H
#define _TMS_H

/*** Include *****************************************************************/
#include "types.h"

#ifdef G4XX
    #include "stm32g4xx.h"
#endif // G4XX
#ifdef H7XX
    #include "stm32h7xx.h"
#endif // H7XX
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/**
 * @brief  Indizes Millisekunden-Timer-Array
 */
typedef enum TMS_Index {
    TMS_mSek,
    TMS_SekTimer,  // Hilfstimer für Sekunden-Messung
    TMS_StatusLED, // µC-Status LED-Timer
    // START of project specific reload values
    // ...
    // END of suer specific reload values
    TMS_N // Anzahl Konstanten (immer letztes!)
} TMS_Index_t;

/**
 * @brief  Indizes Sekunden-Timer-Array
 */
typedef enum TS_Index {
    TS_Sek,
    // START of project specific timers
    // ...
    // END of project specific timers
    TS_N // Anzahl Konstanten (immer letztes!)
} TS_Index_t;

#define TMS_RELOAD_STATUS_LED_MSM_INIT     1000u
#define TMS_RELOAD_STATUS_LED_MSM_RUN      1000u
#define TMS_RELOAD_STATUS_LED_MSM_SHUTDOWN 1000u
#define TMS_RELOAD_STATUS_LED_MSM_ERROR    500u
#define TMS_RELOAD_MS_TICK                 1u
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
extern volatile uint32_t gTMS[TMS_N], gTS[TS_N]; // Global: Timer-Arrays ms- und s-Timer
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
void TMS_Init(void);
void TMS_Tick(void);

// Ersatzfunktionen für Static inline
bool Tms_isTmsTimerElapsed(TMS_Index_t tmsTimerIndex);
bool Tms_isTsTimerElapsed(TS_Index_t tsTimerIndex);
void Tms_reloadTsTimer(TS_Index_t tsTimerIndex, uint32_t reloadVal);

#if (0) // Static inline aus templeate nicht erkannt
/******************************************************************************
 ** Name            : @fn Tms_isTmsTimerElapsed
 **
 ** Created from/on : @author mke / @date 20.04.2021
 **
 ** Description     : @brief Returns whether or not a TMS (milliseconds)
 **                          timer is elapsed or still running.
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  tmsTimerIndex  Timer that gets evaluated.
 ** OutputValues    : @retval bool           TRUE  if timer is elapsed.
 **                                          FALSE if timer is still running.
 *****************************************************************************/
__STATIC_INLINE bool Tms_isTmsTimerElapsed(TMS_Index_t tmsTimerIndex) {
    return (0u == gTMS[tmsTimerIndex]);
}

/******************************************************************************
 ** Name            : @fn Tms_reloadTmsTimer
 **
 ** Created from/on : @author mke / @date 20.04.2021
 **
 ** Description     : @brief Reloads a TMS (milliseconds) timer with a desired
 **                          reload value.
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  tmsTimerIndex  Index of timer that gets reloaded.
 ** InputValues     : @param  reloadVal      Desired reload value.
 ** OutputValues    : @retval none
 *****************************************************************************/
__STATIC_INLINE void Tms_reloadTmsTimer(TMS_Index_t tmsTimerIndex, uint32_t reloadVal) {
    gTMS[tmsTimerIndex] = reloadVal;
}

/******************************************************************************
 ** Name            : @fn Tms_isTsTimerElapsed
 **
 ** Created from/on : @author mke / @date 20.04.2021
 **
 ** Description     : @brief Returns whether or not a TS (seconds)
 **                          timer is elapsed or still running.
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  tmsTimerIndex  Timer that gets evaluated.
 ** OutputValues    : @retval bool           TRUE  if timer is elapsed.
 **                                          FALSE if timer is still running.
 *****************************************************************************/
__STATIC_INLINE bool Tms_isTsTimerElapsed(TS_Index_t tsTimerIndex) {
    return (0u == gTS[tsTimerIndex]);
}

/******************************************************************************
 ** Name            : @fn Tms_reloadTsTimer
 **
 ** Created from/on : @author mke / @date 20.04.2021
 **
 ** Description     : @brief Reloads a TS (seconds) timer with a desired
 **                          reload value.
 **
 ** Calling         : @remark
 **
 ** InputValues     : @param  tmsTimerIndex  Index of timer that gets reloaded.
 ** InputValues     : @param  reloadVal      Desired reload value.
 ** OutputValues    : @retval none
 *****************************************************************************/
__STATIC_INLINE void Tms_reloadTsTimer(TS_Index_t tsTimerIndex, uint32_t reloadVal) {
    gTS[tsTimerIndex] = reloadVal;
}
#endif
/* NO MORE DEFINITIONS */

#endif //_TMS_H

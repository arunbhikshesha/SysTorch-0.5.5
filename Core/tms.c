/*
******************************************************************************
* @file tms.c
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "tms.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
volatile uint32_t gTMS[TMS_N], gTS[TS_N]; // Global: Timer-Arrays ms- und s-Timer
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

/******************************************************************************
** Name               : @fn TMS_Init
**
** Created from /on   : @author mha / @date 05.01.2021
**
** Description        : @brief Initalisierung Timer für Milli- & Sekunden
**
** Calling            : @remark
**
** InputValues        : @param none
**
** OutputValues       : @retval none
******************************************************************************/
void TMS_Init(void) {
    uint32_t tmsIndex;

    for (tmsIndex = 0U; tmsIndex < TMS_N; ++tmsIndex) {
        gTMS[tmsIndex] = 0U; // Timer 1ms initialisieren
    }
    for (tmsIndex = 0U; tmsIndex < TS_N; ++tmsIndex) {
        gTS[tmsIndex] = 0U; // Timer 1s initialisieren
    }
}

/******************************************************************************
** Name               : @fn TMS_Tick
**
** Created from /on   : @author mha / @date 05.01.2021
**
** Description        : @brief Aktualisierung Timer für Milli- & Sekunden
**
** Calling            : @remark
**
** InputValues        : @param none
**
** OutputValues       : @retval none
******************************************************************************/
void TMS_Tick(void) {
    uint32_t index;

    // hier alle 1ms!
    for (index = 0U; index < TMS_N; ++index) {
        if (0U != gTMS[index]) {
            --gTMS[index]; // Timer 1ms aktualisieren
        }
    }

    if (0U == gTMS[TMS_SekTimer]) {
        // -----Timer 1s-----
        gTMS[TMS_SekTimer] = 1000U;
        for (index = 0U; index < TS_N; ++index) {
            if (0U != gTS[index]) {
                --gTS[index]; // Timer 1s aktualisieren
            }
        }
    }
}

// Ersatzfunktionen für Static inline
bool Tms_isTmsTimerElapsed(TMS_Index_t tmsTimerIndex) {
    return (0u == gTMS[tmsTimerIndex]);
}

void Tms_reloadTmsTimer(TMS_Index_t tmsTimerIndex, uint32_t reloadVal) {
    gTMS[tmsTimerIndex] = reloadVal;
}

bool Tms_isTsTimerElapsed(TS_Index_t tsTimerIndex) {
    return (0u == gTS[tsTimerIndex]);
}

void Tms_reloadTsTimer(TS_Index_t tsTimerIndex, uint32_t reloadVal) {
    gTS[tsTimerIndex] = reloadVal;
}

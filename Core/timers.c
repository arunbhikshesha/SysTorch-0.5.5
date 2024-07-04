/*
******************************************************************************
* @file: timers.c
* @author: APR
* @brief: Timers initialization and related functions.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "timers.h"
#include "main.h"
#include "fdcan2.h"
#include "inout.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
TIM_HandleTypeDef htim15;
TIM_HandleTypeDef htim16;
TIM_HandleTypeDef htim17;
static uint8_t can_busoff_count = 0;

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/
/**********************************************************
 ** Name            : MX_TIM15_Init
 **
 ** Created from/on : SP / 30.10.2023
 **
 ** Description     : Initialise Timer 15
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_TIM15_Init(void) {

    htim15.Instance = TIM15;
    htim15.Init.Prescaler = 40000;
    htim15.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim15.Init.Period = 40; /* 10 msec */
    htim15.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim15.Init.RepetitionCounter = 0;
    htim15.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim15) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : MX_TIM16_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise Timer 16
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_TIM16_Init(void) {

    htim16.Instance = TIM16;
    htim16.Init.Prescaler = 40000;
    htim16.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim16.Init.Period = 40; /* 10 msec */
    htim16.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim16.Init.RepetitionCounter = 0;
    htim16.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim16) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : MX_TIM17_Init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialise Timer 17
 **
 ** Calling         : main
 **
 ** InputValues     : none
 ** OutputValues    : none
 **********************************************************/
void MX_TIM17_Init(void) {
    htim17.Instance = TIM17;
    htim17.Init.Prescaler = 40000;
    htim17.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim17.Init.Period = 16; /* 4 msec */
    htim17.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    htim17.Init.RepetitionCounter = 0;
    htim17.Init.AutoReloadPreload = TIM_AUTORELOAD_PRELOAD_DISABLE;
    if (HAL_TIM_Base_Init(&htim17) != HAL_OK) {
        Error_Handler();
    }
}

/**********************************************************
 ** Name            : HAL_TIM_PeriodElapsedCallback
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Timer period elapsed call back function
 **
 ** Calling         : Timer interrupt function
 **
 ** InputValues     : TIM_HandleTypeDef *
 ** OutputValues    : none
 **********************************************************/
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim) {
    if (htim == &htim16) {
        static int cnt = 0;
        static uint16_t inputs_sum = 0;
        static uint16_t inputs_sum_last = 0xFFFF;
        static uint16_t inputs;

        /* all 10 msec */

        // recover from bus off
        if (fdcan2_bus_status()) {
            can_busoff_count++;
            if (can_busoff_count == 10) {
                HAL_FDCAN_Start(&hfdcan2);
                Serial_COM_PutString("CAN BusOff recovered");
                can_busoff_count = 0;
                fdcan2_clear_busoffflag();
            }
        }

        /* check for inputs */
        inout_get_inputs(&inputs);
        inputs_sum |= inputs;

        /* all 50 msec */
        if (!(cnt % 5)) {
#if 0 // commented as it is requesting the config every 5 seconds
			/* check for time out */
			state_check_tmo(&b_request_cfg);


			/* check if a torch configuration shall be requested */
			if (b_request_cfg)
			{
				/* request a torch configuration */
			//	msg_send_cfg_request();

				b_request_cfg = 0;
			}
			else
			{
#endif
            /* send message with inputs each second or on changed inputs */
            if ((inputs_sum != inputs_sum_last) || !(cnt % 100)) {
                if (!((get_param_id() == 0) && ((inputs_sum == 0x0100) || (inputs_sum == 0x0200)))) {
                    msg_send_inputs(inputs_sum);
                } else {
                    // do nothing for now, to prevent QA requesting information through up and down buttons
                }

                inputs_sum_last = inputs_sum;
                cnt = 0;
            }
            //	}

            inputs_sum = 0;
        }

        cnt++;
        cnt %= 1000;
    } else if (htim == &htim17) {
        /* all 4 msec */
        inout_trigger_outputs();
    }
}

/* NO DEFINITIONS */

/*
******************************************************************************
* @file: data_handler_parameter.c
* @author: APR
* @brief: Exchange data between middleware and gui.
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "data_handler_parameter.h"
#include "fdcan2.h"
#include "Parameter.h"
#include "TestBoard.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO DEFINITIONS */

/*** Definition of variables *************************************************/
static void *p_gui_par_dev_class = NULL;

/*** Prototypes of functions *************************************************/

/*** Definitions of functions ************************************************/

/**********************************************************
 ** Name            : param_init
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Initialises the parameter object class
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : void pointer
 ** OutputValues    : none
 **********************************************************/
void param_init(void *p_gui_par_class) {
    p_gui_par_dev_class = p_gui_par_class;
}

/**********************************************************
 ** Name            : process_param_update
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : Updates the GUI from CAN message data
 **
 ** Calling         : EW Process
 **
 ** InputValues     : void
 ** OutputValues    : int. 1=Success. 0=Failure
 **********************************************************/
int process_param_update(void) {
    if (p_gui_par_dev_class == NULL) {
        return 0;
    }
    uint32_t test_results;
    param_info_to_gui can_params;
    param_update_to_gui(&can_params);
    if (Get_TestMode()) {
        test_results = Get_Testresults();
        can_params.ID = TEST_MODE_T;
        can_params.unit_id = TEST_MODE_C;
        // reusing the below fields for test results updates, only in TestMode
        can_params.type = test_results;
        can_params.image = (test_results >> 8);

        ParameterDeviceClass__update_par_from_dev(p_gui_par_dev_class, can_params.ID, can_params.value, can_params.max,
                                                  can_params.min, can_params.unit_id, can_params.type, can_params.image,
                                                  can_params.text, can_params.total_params);
        param_clear_flag();
        return 1;
    } else {
        if (can_params.flag != MSG_0x110_clearall) {
            ParameterDeviceClass__update_par_from_dev(
                p_gui_par_dev_class, can_params.ID, can_params.value, can_params.max, can_params.min,
                can_params.unit_id, can_params.type, can_params.image, can_params.text, can_params.total_params);
            param_clear_flag();
            return 1;
        }
    }
    return 0;
}

/**********************************************************
 ** Name            : get_can_bus_status
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus status
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_bus_status(void) {
    return fdcan2_bus_status();
}

/**********************************************************
 ** Name            : get_can_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint32_t
 **********************************************************/
uint32_t get_can_error_count(void) {
    return fdcan2_error_count();
}

/**********************************************************
 ** Name            : get_can_ack_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN ack error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_ack_error_count(void) {
    return fdcan2_ack_error_count();
}

/**********************************************************
 ** Name            : get_can_bit0_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus bit0 error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_bit0_error_count(void) {
    return fdcan2_bit0_error_count();
}

/**********************************************************
 ** Name            : get_can_bit1_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus bit1 error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_bit1_error_count(void) {
    return fdcan2_bit1_error_count();
}

/**********************************************************
 ** Name            : get_can_crc_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus crc error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_crc_error_count(void) {
    return fdcan2_crc_error_count();
}

/**********************************************************
 ** Name            : get_can_stuff_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus stuff error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_stuff_error_count(void) {
    return fdcan2_stuff_error_count();
}

/**********************************************************
 ** Name            : get_can_form_error_count
 **
 ** Created from/on : APR / 10.08.2023
 **
 ** Description     : get CAN bus form error count
 **
 ** Calling         : Emb Wiz
 **
 ** InputValues     : none
 ** OutputValues    : uint8_t
 **********************************************************/
uint8_t get_can_form_error_count(void) {
    return fdcan2_form_error_count();
}

/* NO DEFINITIONS */

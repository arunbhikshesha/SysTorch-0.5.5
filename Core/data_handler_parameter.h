/*
******************************************************************************
* @file: data_handler_paramater.h
* @author: APR
* @brief: Exchange data between middleware and gui
******************************************************************************
*
******************************************************************************
*/

#ifndef _DATA_HANDLER_PARAMETER_H
#define _DATA_HANDLER_PARAMETER_H

/*** Include *****************************************************************/
#include "types.h"

/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void param_init(void *);
int process_param_update(void);
uint32_t get_can_error_count(void);
uint8_t get_can_bus_status(void);
uint8_t get_can_ack_error_count(void);
uint8_t get_can_bit0_error_count(void);
uint8_t get_can_bit1_error_count(void);
uint8_t get_can_crc_error_count(void);
uint8_t get_can_stuff_error_count(void);
uint8_t get_can_form_error_count(void);

/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

#endif //_DATA_HANDLER_PARAMETER_H
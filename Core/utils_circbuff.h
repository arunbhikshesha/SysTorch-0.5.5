/******************************************************************************
** @file utils_circbuff.h
** @author Arun Prasad Bhikshesha
** @brief circular buffer enqueue and dequeue functions
******************************************************************************/

#ifndef _UTILS_CIRCBUFF_H
#define _UTILS_CIRCBUFF_H

/*** Include *****************************************************************/
#include "types.h"
#include "tms.h"
#include "init.h"
#include "gData.h"
#include "gError.h"
#include <stdio.h>
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
// Circular buffer
typedef struct utils_circbuff_circ_buffer {
    uint8_t *buffer;
    uint16_t head;
    uint16_t tail;
    uint16_t maxlen;
} utils_circbuff_handle_t;
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
bool utils_circbuff_init(utils_circbuff_handle_t *const circbuff_hdl, uint8_t *const buffer, const uint16_t max_size);
bool utils_circbuff_dequeue(utils_circbuff_handle_t *const circbuff_hdl, uint8_t destination[], const uint8_t size);
bool utils_circbuff_enqueue(const uint8_t source[], utils_circbuff_handle_t *const circbuff_hdl, const uint8_t size);
bool utils_circbuff_full(utils_circbuff_handle_t const *const circbuff_hdl);
bool utils_circbuff_empty(utils_circbuff_handle_t const *const circbuff_hdl);
uint16_t utils_circbuff_size(utils_circbuff_handle_t const *const circbuff_hdl);
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

#endif
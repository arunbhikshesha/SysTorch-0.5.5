/******************************************************************************
** @file utils_circbuff.c
** @author Arun Prasad Bhikshesha
** @brief circular buffer enqueue and dequeue functions
******************************************************************************/

/*** Include *****************************************************************/
#include "utils_circbuff.h"
/* NO MORE DEFINITIONS */

/*** Preprocessor definitions ************************************************/
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
/* NO MORE DEFINITIONS */

/*** Definitions of functions ************************************************/
/******************************************************************************
** Name               : @fn utils_circbuff_init
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 16.08.2022
**
** Description        : @brief initialize the circular buffer.
**
** Calling            : @remark
**
** InputValues        : @param circbuff_hdl Pointer to circbuff handle
                        @param destination  Pointer to queue buffer
                        @param max_size     Size (bytes) of the queue buffer
**
** OutputValues       : @retval bool status. SUCCESS = true; FAILURE = false.
******************************************************************************/
bool utils_circbuff_init(utils_circbuff_handle_t *const circbuff_hdl, uint8_t *const buffer, const uint16_t max_size) {
    if ((buffer == NULL) || (max_size == 0)) {
        circbuff_hdl->buffer = NULL;
        circbuff_hdl->head = 0u;
        circbuff_hdl->tail = 0u;
        circbuff_hdl->maxlen = 0u;
        return false;
    }
    circbuff_hdl->buffer = buffer;
    circbuff_hdl->head = 0;
    circbuff_hdl->tail = 0;
    circbuff_hdl->maxlen = max_size;
    return true;
}

/******************************************************************************
** Name               : @fn utils_circbuff_dequeue
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 16.08.2022
**
** Description        : @brief get the contents from circular buffer.
**
** Calling            : @remark
**
** InputValues        : @param circbuff_hdl Pointer to circbuff handle
                        @param destination  Pointer to destination buffer
                        @param size         Number of elements (bytes) to dequeue
**
** OutputValues       : @retval bool status. SUCCESS = true; FAILURE = false.
******************************************************************************/

bool utils_circbuff_dequeue(utils_circbuff_handle_t *const circbuff_hdl, uint8_t destination[], const uint8_t size) {
    if ((circbuff_hdl == NULL) || (circbuff_hdl->buffer == NULL) || (destination == NULL)) {
        return false;
    }
    for (uint8_t i = 0; i < size; i++) {
        uint16_t next;
        if (circbuff_hdl->head == circbuff_hdl->tail) {
            // Circbuff is empty, nothing to dequeue
            return false;
        }
        next = circbuff_hdl->tail + 1;
        if (next >= circbuff_hdl->maxlen) {
            next = 0;
        }
        destination[i] = circbuff_hdl->buffer[circbuff_hdl->tail];
        circbuff_hdl->tail = next;
    }
    return true;
}

/******************************************************************************
** Name               : @fn utils_circbuff_enqueue
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 16.08.2022
**
** Description        : @brief put the contents to circular buffer.
**
** Calling            : @remark
**
** InputValues        : @param source       Pointer to source array
                        @param circbuff_hdl Pointer to circbuff handle
                        @param size         Number of elements (bytes) to enqueue
**
** OutputValues       : @retval bool status. SUCCESS = true; FAILURE = false.
******************************************************************************/
bool utils_circbuff_enqueue(const uint8_t source[], utils_circbuff_handle_t *const circbuff_hdl, const uint8_t size) {
    if ((circbuff_hdl == NULL) || (circbuff_hdl->buffer == NULL) || (source == NULL)) {
        return false;
    }
    for (uint8_t i = 0; i < size; i++) {
        uint16_t next;
        if (utils_circbuff_full(circbuff_hdl)) {
            // check whether the buffer is full and return. Re-check whether we need to overwrite the buffer in case
            // of new data(SGE, MKE)
            return false;
        }
        next = circbuff_hdl->head + 1;
        if (next >= circbuff_hdl->maxlen) {
            next = 0;
        }
        circbuff_hdl->buffer[circbuff_hdl->head] = source[i];
        circbuff_hdl->head = next;
    }
    return true;
}

/******************************************************************************
** Name               : @fn utils_circbuff_full
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 16.08.2022
**
** Description        : @brief checks the circular buffer full state.
**
** Calling            : @remark
**
** InputValues        : @param circbuff_hdl Pointer to circbuff handle
**
** OutputValues       : @retval bool status. FULL = true; NOT FULL = false.
******************************************************************************/
bool utils_circbuff_full(utils_circbuff_handle_t const *const circbuff_hdl) {
    return ((circbuff_hdl->head + 1) % circbuff_hdl->maxlen) == circbuff_hdl->tail;
}

/******************************************************************************
** Name               : @fn utils_circbuff_empty
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 16.08.2022
**
** Description        : @brief checks the circular buffer empty state.
**
** Calling            : @remark
**
** InputValues        : @param circbuff_hdl Pointer to circbuff handle
**
** OutputValues       : @retval bool status. EMPTY = true; NOT EMPTY = false.
******************************************************************************/
bool utils_circbuff_empty(utils_circbuff_handle_t const *const circbuff_hdl) {
    return (circbuff_hdl->head == circbuff_hdl->tail);
}

/******************************************************************************
** Name               : @fn utils_circbuff_size
**
** Created from /on   : @author Arun Prasad Bhikshesha / @date 20.04.2023
**
** Description        : @brief Returns the circular buffer size.
**
** Calling            : @remark
**
** InputValues        : @param circbuff_hdl Pointer to circbuff handle
**
** OutputValues       : @retval uint16_t size.
******************************************************************************/
uint16_t utils_circbuff_size(utils_circbuff_handle_t const *const circbuff_hdl) {
    uint16_t size = circbuff_hdl->maxlen;

    if (!utils_circbuff_full(circbuff_hdl)) {
        if (circbuff_hdl->head >= circbuff_hdl->tail) {
            size = (circbuff_hdl->head - circbuff_hdl->tail);
        } else {
            size = (circbuff_hdl->maxlen + circbuff_hdl->head - circbuff_hdl->tail);
        }
    }

    return size;
}

/* NO MORE DEFINITIONS */

/**
 ******************************************************************************
 * @file    gui.h
 * @author  WBO
 * @brief   Header file for GUI functions
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef _GUI_H
#define _GUI_H

#include <stddef.h>
#include <stdint.h>
#include "main.h"

/**
 * @brief Initialize GUI
 * @param p_appl_dev_class: pointer to embedded wizard application device class
 * @retval none
 */
void gui_init(void *p_appl_dev_class);

/**
 * @brief Inform GUI that a message received
 * @retval none
 */
void gui_signal_received_msg(void);

/**
 * @brief Update quick access data
 * @param pb_on: pointer to QA state (on=1, off=0)
 * @param pb_show_value_bar: pointer to flag if value bar shall be shown
 * @param p_frame_number: pointer to image frame number
 * @param p_value: pointer to QA value
 * @retval 1 on success
 */
int gui_data_qa_update(uint8_t *pb_on, uint8_t *pb_show_value_bar, uint8_t *p_frame_number, float *p_value);

/**
 * @brief Update parameter data
 * @param pb_is_toggle: pointer to flag if paramter is a toggle parameter
 * @param p_frame_number: pointer to image frame number
 * @param pb_value_string_changed: pointer to flag if value string (may be) changed
 * @param pb_text_string_changed: pointer to flag if text string (may be) changed
 * @retval 1 on success
 */
int gui_data_param_update(uint8_t *pb_is_toggle, uint8_t *p_frame_number, uint8_t *pb_value_string_changed,
                          uint8_t *pb_text_string_changed);

/**
 * @brief Check if parameter is a toggle parameter
 * @param type: parameter type
 * @retval 1 if parameter is a toggle parameter, 0 if not
 */
int gui_param_type_is_toggle(uint8_t type);

#endif //_GUI_H
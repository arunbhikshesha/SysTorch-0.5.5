/**
 ******************************************************************************
 * @file    datatypes.h
 * @author  WBO
 * @brief   Header file for data handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef _DATA_H
#define _DATA_H

#include <stdint.h>

/**
 * @brief Enum for texts
 */
typedef enum {
    DATA_TEXT_NONE, ///< ""
    DATA_TEXT_QJOB, ///< "QJOB"
    DATA_TEXT_DYN,  ///< "DYN"

} data_text_t;

/**
 * @brief Structure for parameter configuration
 */
typedef struct {
    uint8_t unit;
    uint8_t type;
    uint8_t image;
    data_text_t text;
} data_param_cfg_t;

/**
 * @brief Structure for parameter data
 */
typedef struct {
    int id;
    float value;
    float min;
    float max;
    data_param_cfg_t cfg;
} data_param_t;

/**
 * @brief Structure for quick access data
 */
typedef struct {
    int id;
    float value;
    uint8_t state;
    uint8_t image;
    data_text_t text;
} data_qa_t;

typedef void(data_cb_t)(void);

/**
 * @brief Set number of parameters
 * @param num: number of parameters
 * @retval none
 */
void data_set_param_num(uint8_t num);

/**
 * @brief Set parameter value
 * @param id:       ID of the parameter
 * @param value:    value to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_value(uint8_t id, float value, int b_select);

/**
 * @brief Set parameter minimum
 * @param id:       ID of the parameter
 * @param min:      minimum to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_min(uint8_t id, float min, int b_select);

/**
 * @brief Set parameter maximum
 * @param id:       ID of the parameter
 * @param max:      maximum to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_max(uint8_t id, float max, int b_select);

/**
 * @brief Set parameter configuration
 * @param id:       ID of the parameter
 * @param p_cfg:    pointer to configuration to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_cfg(uint8_t id, data_param_cfg_t *p_cfg, int b_select);

/**
 * @brief Set number of quick accesses
 * @param num: number of quick accesses
 * @retval none
 */
void data_set_qa_num(uint8_t num);

/**
 * @brief Set quick access data
 * @param id:        ID of the QA
 * @param p_data_qa: pointer to the QA data to set
 * @param b_select:  flag if this QA shall be shown
 * @retval none
 */
void data_set_qa(uint8_t id, data_qa_t *p_qa, int b_select);

/**
 * @brief Set parameter callback
 * @param p_cb: callback which will be called if a parameter was selected or data of a selected parameter was changed
 * @retval none
 */
void data_set_cb_param(data_cb_t *p_cb);

/**
 * @brief Set quick access callback
 * @param p_cb: callback which will be called if a QA was selected or data of a selected QA was changed
 * @retval none
 */
void data_set_cb_qa(data_cb_t *p_cb);

/**
 * @brief Get data of selected parameter
 * @param p_data_param: pointer to the data
 * @retval 0 on success, -1 if no selected parameter could be found
 */
int data_get_param(data_param_t *p_param);

/**
 * @brief Get data of selected quick access
 * @param p_data_qa: pointer to the data
 * @retval 0 on success, -1 if no selected parameter could be found
 */
int data_get_qa(data_qa_t *p_qa);

#endif //_DATA_H

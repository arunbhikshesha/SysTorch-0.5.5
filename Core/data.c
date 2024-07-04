/**
 ******************************************************************************
 * @file    data.c
 * @author  WBO
 * @brief   Data handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stdint.h>
#include <string.h>

#include "datatypes.h"

#define DATA_PARAM_NUM_MAX (32 + 1) // maximum number of parameters
#define DATA_QA_NUM_MAX    32       // maximum number of quick accesses

#define DATA_PARAM_ID_LAST 0 // ID reserved for last parameter

#define DATA_QA_ID_INVAL 0 // invalid quick access ID

static data_param_t data_param[DATA_PARAM_NUM_MAX];
static data_qa_t data_qa[DATA_QA_NUM_MAX];

static int data_param_num = 0;
static int data_qa_num = 0;
static uint8_t data_param_id_act = DATA_PARAM_ID_LAST;
static uint8_t data_qa_id_act = DATA_QA_ID_INVAL;

static data_cb_t *p_cb_param = NULL;
static data_cb_t *p_cb_qa = NULL;

static data_param_t *get_param(uint8_t id);
static data_qa_t *get_qa(uint8_t id);

/**
 * @brief Set number of parameters
 * @param num: number of parameters
 * @retval none
 */
void data_set_param_num(uint8_t num) {
    /* save number of parameters limited to maximum */
    if (num > DATA_PARAM_NUM_MAX) {
        data_param_num = DATA_PARAM_NUM_MAX;
    } else {
        data_param_num = num;
    }

    /* reset all paramters */
    memset(data_param, 0, sizeof(data_param));

    for (int i = 0; i < data_param_num; i++) {
        data_param[i].id = DATA_PARAM_ID_LAST;
    }
}

/**
 * @brief Set parameter value
 * @param id:       ID of the parameter
 * @param value:    value to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_value(uint8_t id, float value, int b_select) {
    int b_call_cb = 0;

    data_param_t *p_param = get_param(id);

    if (p_param) {
        if (b_select) {
            if (data_qa_id_act != DATA_QA_ID_INVAL || data_param_id_act != id || p_param->value != value) {
                b_call_cb = 1;
            }

            data_param_id_act = id;
            data_qa_id_act = DATA_QA_ID_INVAL;
        }

        p_param->value = value;
    }

    if (b_call_cb && p_cb_param) {
        p_cb_param();
    }
}

/**
 * @brief Set parameter minimum
 * @param id:       ID of the parameter
 * @param min:      minimum to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_min(uint8_t id, float min, int b_select) {
    int b_call_cb = 0;

    data_param_t *p_param = get_param(id);

    if (p_param) {
        if (b_select) {
            if (data_qa_id_act != DATA_QA_ID_INVAL || data_param_id_act != id || p_param->min != min) {
                b_call_cb = 1;
            }

            data_param_id_act = id;
            data_qa_id_act = DATA_QA_ID_INVAL;
        }

        p_param->min = min;
    }

    if (b_call_cb && p_cb_param) {
        p_cb_param();
    }
}

/**
 * @brief Set parameter maximum
 * @param id:       ID of the parameter
 * @param max:      maximum to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_max(uint8_t id, float max, int b_select) {
    int b_call_cb = 0;

    data_param_t *p_param = get_param(id);

    if (p_param) {
        if (b_select) {
            if (data_qa_id_act != DATA_QA_ID_INVAL || data_param_id_act != id || p_param->max != max) {
                b_call_cb = 1;
            }

            data_param_id_act = id;
            data_qa_id_act = DATA_QA_ID_INVAL;
        }

        p_param->max = max;
    }

    if (b_call_cb && p_cb_param) {
        p_cb_param();
    }
}

/**
 * @brief Set parameter configuration
 * @param id:       ID of the parameter
 * @param p_cfg:    pointer to configuration to set
 * @param b_select: flag if this parameter shall be shown
 * @retval none
 */
void data_set_param_cfg(uint8_t id, data_param_cfg_t *p_cfg, int b_select) {
    int b_call_cb = 0;

    data_param_t *p_param = get_param(id);

    if (p_param) {
        if (b_select) {
            if (data_qa_id_act != DATA_QA_ID_INVAL || data_param_id_act != id ||
                memcmp(&p_param->cfg, p_cfg, sizeof(data_param_cfg_t))) {
                b_call_cb = 1;
            }

            data_param_id_act = id;
            data_qa_id_act = DATA_QA_ID_INVAL;
        }

        memcpy(&p_param->cfg, p_cfg, sizeof(data_param_cfg_t));
    }

    if (b_call_cb && p_cb_param) {
        p_cb_param();
    }
}

/**
 * @brief Set number of quick accesses
 * @param num: number of quick accesses
 * @retval none
 */
void data_set_qa_num(uint8_t num) {
    /* save number of QAs limited to maximum */
    if (num > DATA_QA_NUM_MAX) {
        data_qa_num = DATA_QA_NUM_MAX;
    } else {
        data_qa_num = num;
    }

    /* reset all QAs */
    memset(data_qa, 0, sizeof(data_qa));

    for (int i = 0; i < data_qa_num; i++) {
        data_qa[i].id = DATA_QA_ID_INVAL;
    }
}

/**
 * @brief Set quick access data
 * @param id:        ID of the QA
 * @param p_data_qa: pointer to the QA data to set
 * @param b_select:  flag if this QA shall be shown
 * @retval none
 */
void data_set_qa(uint8_t id, data_qa_t *p_data_qa, int b_select) {
    int b_call_cb = 0;

    data_qa_t *p_qa = get_qa(id);

    if (p_qa) {
        if (b_select) {
            if (data_qa_id_act != id || memcmp(p_qa, p_data_qa, sizeof(data_qa_t))) {
                b_call_cb = 1;
            }

            data_qa_id_act = id;
            data_param_id_act = DATA_PARAM_ID_LAST;
        }

        memcpy(p_qa, p_data_qa, sizeof(data_qa_t));
    }

    if (b_call_cb && p_cb_qa) {
        p_cb_qa();
    }
}

/**
 * @brief Set parameter callback
 * @param p_cb: callback which will be called if a parameter was selected or data of a selected parameter was changed
 * @retval none
 */
void data_set_cb_param(data_cb_t *p_cb) {
    p_cb_param = p_cb;
}

/**
 * @brief Set quick access callback
 * @param p_cb: callback which will be called if a QA was selected or data of a selected QA was changed
 * @retval none
 */
void data_set_cb_qa(data_cb_t *p_cb) {
    p_cb_qa = p_cb;
}

/**
 * @brief Get data of selected parameter
 * @param p_data_param: pointer to the data
 * @retval 0 on success, -1 if no selected parameter could be found
 */
int data_get_param(data_param_t *p_data_param) {
    data_param_t *p_param;

    p_param = get_param(data_param_id_act);

    if (p_param) {
        memcpy(p_data_param, p_param, sizeof(data_param_t));
    }

    return (p_param ? 0 : -1);
}

/**
 * @brief Get data of selected quick access
 * @param p_data_qa: pointer to the data
 * @retval 0 on success, -1 if no selected parameter could be found
 */
int data_get_qa(data_qa_t *p_data_qa) {
    data_qa_t *p_qa;

    p_qa = get_qa(data_qa_id_act);

    if (p_qa) {
        memcpy(p_data_qa, p_qa, sizeof(data_qa_t));
    }

    return (p_qa ? 0 : -1);
}

/**
 * @brief Get pointer to parameter data for an ID
 * @param id: parameter ID
 * @retval pointer to the parameter data
 */
static data_param_t *get_param(uint8_t id) {
    data_param_t *param = NULL;
    int i;

    for (i = 0; i < data_param_num; i++) {
        if (i < data_param_num - 1 && data_param[i].id == DATA_PARAM_ID_LAST) {
            data_param[i].id = id;
        }

        if (data_param[i].id == id) {
            param = &data_param[i];
            break;
        }
    }

    return param;
}

/**
 * @brief Get pointer to quick access data for an ID
 * @param id: QA ID
 * @retval pointer to the QA data
 */
static data_qa_t *get_qa(uint8_t id) {
    data_qa_t *qa = NULL;
    int i;

    for (i = 0; i < data_qa_num; i++) {
        if (data_qa[i].id == DATA_QA_ID_INVAL) {
            data_qa[i].id = id;
        }

        if (data_qa[i].id == id) {
            qa = &data_qa[i];
            break;
        }
    }

    return qa;
}

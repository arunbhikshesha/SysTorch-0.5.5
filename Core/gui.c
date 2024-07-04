/**
 ******************************************************************************
 * @file    gui.c
 * @author  WBO
 * @brief   Functions for GUI
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "_ApplicationDeviceClass.h"
#include "datatypes.h"
#include "gui.h"

#define UNITS_NUM 18 // number of units

#define DECIMAL_MARKER ','

static int text_string_changed(void);
static int value_string_changed(void);
static void format_value(float value, uint8_t type, char *p_buf, size_t length);
static void cb_param(void);
static void cb_qa(void);

#if 0
/* unit strings */
static const char *p_units[UNITS_NUM][2] = {
    {"#", "#"},     {"A", "A"},   {"s", "s"},       {"Hz", "Hz"},   {"%%", "%%"},   {"mm", "\""},
    {"A/s", "A/s"}, {"ms", "ms"}, {"m/min", "IPM"}, {"V", "V"},     {"V/s", "V/s"}, {"l/min", "gal/h"},
    {"m", "yd"},    {"W", "W"},   {"J", "J"},       {"kWh", "kWh"}, {"h", "h"},     {"°C", "°F"}};
#endif

/* last parameter data */
static data_param_t gui_data_param_last;

/* actual parameter data */
static data_param_t gui_data_param;

/* pointer to embedded wizard application device class */
static void *p_gui_appl_dev_class = NULL;

/**
 * @brief Initialize GUI
 * @param p_appl_dev_class: pointer to embedded wizard application device class
 * @retval none
 */
void gui_init(void *p_appl_dev_class) {
    /* save pointer to pointer to embedded wizard application device class */
    p_gui_appl_dev_class = p_appl_dev_class;

    /* set callbacks for data changes */
    data_set_cb_param(cb_param);
    data_set_cb_qa(cb_qa);
}

/**
 * @brief Inform GUI that a message received
 * @retval none
 */
void gui_signal_received_msg(void) {
    static unsigned int cnt = 0;

    if (p_gui_appl_dev_class) {
        ApplicationDeviceClass__UpdateReceiveCounter(p_gui_appl_dev_class, ++cnt);
    }
}

/**
 * @brief Update quick access data
 * @param pb_on: pointer to QA state (on=1, off=0)
 * @param pb_show_value_bar: pointer to flag if value bar shall be shown
 * @param p_frame_number: pointer to image frame number
 * @param p_value: pointer to QA value
 * @retval 1 on success
 */
int gui_data_qa_update(uint8_t *pb_on, uint8_t *pb_show_value_bar, uint8_t *p_frame_number, float *p_value) {
    int result = 0;
    data_qa_t data_qa;

    if (data_get_qa(&data_qa) == 0) {
        *pb_on = data_qa.state & 1;
        *pb_show_value_bar = data_qa.state & (1 << 1);

        *p_frame_number = data_qa.image;

        *p_value = data_qa.value;

        result = 1;
    }

    return result;
}

/**
 * @brief Update parameter data
 * @param pb_is_toggle: pointer to flag if paramter is a toggle parameter
 * @param p_frame_number: pointer to image frame number
 * @param pb_value_string_changed: pointer to flag if value string (may be) changed
 * @param pb_text_string_changed: pointer to flag if text string (may be) changed
 * @retval 1 on success
 */
int gui_data_param_update(uint8_t *pb_is_toggle, uint8_t *p_frame_number, uint8_t *pb_value_string_changed,
                          uint8_t *pb_text_string_changed) {
    int result = 0;

    memcpy(&gui_data_param_last, &gui_data_param, sizeof(data_param_t));

    if (data_get_param(&gui_data_param) == 0) {
        *pb_is_toggle = gui_param_type_is_toggle(gui_data_param.cfg.type);

        *p_frame_number = gui_data_param.cfg.image;

        /* use next frame is parameter value is toggled */
        if (gui_param_type_is_toggle(gui_data_param.cfg.type) && gui_data_param.value) {
            (*p_frame_number)++;
        }

        *pb_value_string_changed = value_string_changed();
        *pb_text_string_changed = text_string_changed();

        result = 1;
    }

    return result;
}

/**
 * @brief Get value string for actual parameter data
 * @retval value string
 */
const char *gui_param_value_string(char *p_buf, size_t length) {
    p_buf[0] = '\0';

    /* value string is not used for toggle parameters */
    if (!gui_param_type_is_toggle(gui_data_param.cfg.type)) {
        format_value(gui_data_param.value, gui_data_param.cfg.type, p_buf, length);
    }

    return p_buf;
}

/**
 * @brief Check if parameter is a toggle parameter
 * @param type: parameter type
 * @retval 1 if parameter is a toggle parameter, 0 if not
 */
int gui_param_type_is_toggle(uint8_t type) {
    return (type & (1 << 4));
}

/**
 * @brief Callback for changed parameter data to show
 * @retval none
 */
static void cb_param(void) {
    static unsigned int param_cnt = 0;

    ApplicationDeviceClass__UpdateParamCounter(p_gui_appl_dev_class, ++param_cnt);
}

/**
 * @brief Callback for changed quick access data to show
 * @retval none
 */
static void cb_qa(void) {
    static unsigned int qa_cnt = 0;

    ApplicationDeviceClass__UpdateQACounter(p_gui_appl_dev_class, ++qa_cnt);
}

/**
 * @brief Check if text string (may be) changed
 * @retval 0: text string not changed, 1: text string (may be) changed
 */
static int text_string_changed(void) {
    int result = 0;

    /* text string is not used for toggle parameters */
    if (!gui_param_type_is_toggle(gui_data_param.cfg.type)) {
        /* value string has to be updated if text string was not used or if text or unit changed */
        if (gui_param_type_is_toggle(gui_data_param_last.cfg.type) ||
            gui_data_param.cfg.unit != gui_data_param_last.cfg.unit ||
            gui_data_param.cfg.text != gui_data_param_last.cfg.text) {
            /* text string changed */
            result = 1;
        }
    }

    return result;
}

/**
 * @brief Check if value string (may be) changed
 * @retval 0: value string not changed, 1: value string (may be) changed
 */
static int value_string_changed(void) {
    int result = 0;

    /* value string is not used for toggle parameters */
    if (gui_param_type_is_toggle(gui_data_param.cfg.type)) {
        result = 0;
    } else {
        result = 1;
    }

    return result;
}

/**
 * @brief Format a value
 * @param value: value to format
 * @param type: parameter type with formatting information
 * @param p_buf: string buffer
 * @param length: length of string buffer
 * @retval none
 */
static void format_value(float value, uint8_t type, char *p_buf, size_t length) {
    uint8_t decimal_places = type & 0x3;
    int b_relative = (type >> 3) & 0x1;

    if (length && b_relative && value >= 0.0) {
        p_buf[0] = '+';
        p_buf++;
        length--;
    }

    /* workaround for rounding problems */
    if (value > 0) {
        value += 0.001;
    } else {
        value -= 0.001;
    }

    switch (decimal_places) {
    case 0:
        if (value > 0) {
            value += 0.5;
        } else {
            value -= 0.5;
        }

        snprintf(p_buf, length, "%d", (int)value);

        break;

    case 1:
        if (value > 0) {
            value += 0.05;
        } else {
            value -= 0.05;
        }

        snprintf(p_buf, length, "%s%d%c%d", value < 0.0 ? "-" : "", abs((int)value), DECIMAL_MARKER,
                 abs(((int)(value * 10)) % 10));

        break;

    case 2:
        if (value > 0) {
            value += 0.005;
        } else {
            value -= 0.005;
        }

        snprintf(p_buf, length, "%s%d%c%02d", value < 0.0 ? "-" : "", abs((int)value), DECIMAL_MARKER,
                 abs(((int)(value * 100)) % 100));

        break;
    }
}

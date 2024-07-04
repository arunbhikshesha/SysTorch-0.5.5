/**
 ******************************************************************************
 * @file    fdcan2.h
 * @author  WBO
 * @brief   Header file for FDCAN2 configuration, receive and transmit
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef _FDCAN2
#define _FDCAN2

#include <stdint.h>
#include "stm32g4xx_hal.h"

/**
 * @brief Define FDCAN2_TEST_COMMUNICATION to only answer 0x120 messages
 *        with 0x121 messages
 */
// #define FDCAN2_TEST_COMMUNICATION

/**
 * @brief size of FDCAN2 data
 */
#define FDCAN2_DATA_SIZE 8

typedef enum {
    MSG_0x110_clearall,
    MSG_0x110_setID,
    MSG_0x110_setvalue,
    MSG_0x110_setmin,
    MSG_0x110_setmax,
    MSG_0x110_setformat
} MSG_0x110_Flag;

typedef struct {
    uint32_t last_error;
    uint8_t stuff_error;
    uint8_t form_error;
    uint8_t ack_error;
    uint8_t bit1_error;
    uint8_t bit0_error;
    uint8_t crc_error;
} can_error_counters;

typedef struct {
    uint8_t ID;
    uint8_t unit_id;
    uint8_t type;
    uint8_t image;
    uint8_t text;
    uint8_t flag;
    uint8_t total_params;
    uint8_t total_qas;
    float value;
    float min;
    float max;
} param_info_to_gui;

/**
 * @brief Initialise FDCAN2
 * @param none
 * @retval none
 */
void MX_FDCAN2_Init(void);

/**
 * @brief Configure FDCAN2
 * @param p_HFDCAN: pointer to FDCAN handle structure definition
 * @retval none
 */
void fdcan2_config(FDCAN_HandleTypeDef *p_HFDCAN);

/**
 * @brief Try to receive a message
 * @param p_msg_id: pointer to the ID of the received message
 * @param data: data of the received message
 * @retval 1 if a message received, 0 if no message received
 */
int fdcan2_receive(int *p_msg_id, uint8_t data[FDCAN2_DATA_SIZE]);

/**
 * @brief Send a message
 * @param msg_id: message ID
 * @param data: message data
 * @retval none
 */
void fdcan2_send(int msg_id, uint8_t data[FDCAN2_DATA_SIZE]);

uint8_t fdcan2_bus_status(void);
uint32_t fdcan2_error_count(void);
uint8_t fdcan2_ack_error_count(void);
uint8_t fdcan2_bit0_error_count(void);
uint8_t fdcan2_bit1_error_count(void);
uint8_t fdcan2_crc_error_count(void);
uint8_t fdcan2_stuff_error_count(void);
uint8_t fdcan2_form_error_count(void);
void fdcan2_clear_busoffflag(void);
void param_update_to_gui(param_info_to_gui *);
void param_clear_flag(void);
uint8_t get_param_id(void);
#endif //_FDCAN2
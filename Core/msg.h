/**
 ******************************************************************************
 * @file    msg.h
 * @author  WBO
 * @brief   Header file for message handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef _MSG_H
#define _MSG_H

#include <stdint.h>

/**
 * @brief size of message data
 */
#define MSG_DATA_SIZE 8

/** @defgroup message IDs
 * @{
 */
#define MSG_INPUTS          0x100
#define MSG_REQUESTS        0x101
#define MSG_VALUE_UPDATE    0x110
#define MSG_TORCH_STATE     0x111
#define MSG_COMMTEST        0x120
#define MSG_COMMTEST_ANSWER 0x121
#define MSG_0x400           0x400 // Lorch standard communication with a PC
#define MSG_0x401           0x401 // Lorch standard communication with a PC - response
#define MSG_0x200           0x200 // Test status CAN message
#define MSG_0x1FF           0x1FF // Test Start CAN message

#define MSG_0x1FB 0x1FB // EEPROM ID request
#define MSG_0x1FA 0x1FA // Ackowledgement for EEPROM ID request message
#define MSG_0x1E0 0x1E0 // Write/read Torch type
#define MSG_0x1E1 0x1E1 // Write/read Ack. Torch type
#define MSG_0x1E2 0x1E2 // Clear EEprom Memory
#define MSG_0x1E3 0x1E3 // Clear EEprom Memory Ack.
#define MSG_0x1E4 0x1E4 // Test eeprom
#define MSG_0x1E5 0x1E5 // Test eeprom Acknowledgement

#define MSG_0x110_ActPar 0x00
#define MSG_0x110_PrePar 0x01
#define MSG_0x110_value  0x01
#define MSG_0x110_min    0x02
#define MSG_0x110_max    0x04
#define MSG_0x110_format 0x08

#define MSG_0x110_ID_11      0x0B
#define MSG_0x110_ID_12      0x0C
#define MSG_0x110_ID_11_step (0.0334)
#define MSG_0x110_ID_12_step (0.1)
#define MSG_0x110_ID_11_MAX  30
#define MSG_0x110_ID_12_MAX  10

/**
 * @}
 */

/*** Preprocessor definitions ************************************************/

// Commands in frame 0x400
#define CO_GET_OSVERSION  11 // SW version request
#define CO_GET_PRGVERSION 12 // SW Semantic Version of FW Image
#define CO_SET_DETACH     18 // Jump into bootloader request
#define CO_GET_LOCKSTATE  42 // Jump into bootloader request
#define GEN3_ID           0xAA

#define GLOBAL_TAMP_REG   0x00 // TAMP_BKP0R
#define GLOBAL_MAGIC_CODE 0x1FA0

/**
 * @brief Try to receive a message
 * @param p_msg_id: pointer to the ID of the received message
 * @param data: data of the received message
 * @retval 1 if a message received, 0 if no message received
 */
int msg_receive(int *msg_id, uint8_t data[MSG_DATA_SIZE]);

/**
 * @brief Send a configuration request
 * @retval none
 */
void msg_send_cfg_request(void);

/**
 * @brief Send inputs
 * @param inputs: mask of pressed push buttons
 * @retval none
 */
void msg_send_inputs(uint16_t inputs);

/**
 * @brief Send a message
 * @param msg_id: message ID
 * @param data: message data
 * @retval none
 */
void msg_send(int msg_id, uint8_t data[MSG_DATA_SIZE]);

/**
 * @brief convert to float
 * @param : uint8_t *ptr
 * @retval float value
 */
float get_float(uint8_t *p_data);

#endif //_MSG_H

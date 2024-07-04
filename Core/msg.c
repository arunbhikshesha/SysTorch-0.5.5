/**
 ******************************************************************************
 * @file    msg.c
 * @author  WBO
 * @brief   Message handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <string.h>

#include "datatypes.h"
#include "fdcan2.h"
#include "inout.h"
#include "main.h"

#include "msg.h"
#include "version.h"

#include "s4-config/peripherie/SoftwareEnums.h"

#if defined(G4xx_BL)
    #include "bootloader_util.h"
#endif

static uint8_t tx_data[MSG_DATA_SIZE];

static void send(int msg_id);

/**
 * @brief Send a configuration request
 * @retval none
 */
void msg_send_cfg_request(void) {
    memset(tx_data, 0, sizeof(tx_data));

    tx_data[1] = 0x0F;

    send(MSG_REQUESTS);
}

/**
 * @brief Send inputs
 * @param inputs: mask of pressed push buttons
 * @retval none
 */
void msg_send_inputs(uint16_t inputs) {
    memset(tx_data, 0, sizeof(tx_data));

    tx_data[1] = inputs >> 8;
    tx_data[2] = inputs & 0xFF;

    send(MSG_INPUTS);
}

/**
 * @brief Send a message
 * @param msg_id: message ID
 * @param data: message data
 * @retval none
 */
void msg_send(int msg_id, uint8_t data[MSG_DATA_SIZE]) {
    memcpy(tx_data, data, MSG_DATA_SIZE);

    send(msg_id);
}

/**
 * @brief Send a message
 * @param msg_id: message ID
 * @retval none
 */
static void send(int msg_id) {

    tx_data[7] = TORCH_ID;
    fdcan2_send(msg_id, tx_data);
}

/**
 * @brief Get a float value
 * @param p_data: pointer to 4 bytes containing the float value
 * @retval float value
 */
float get_float(uint8_t *p_data) {
    uint32_t data = 0;
    int i;

    for (i = 0; i < 4; i++) {
        data = data << 8;
        data += p_data[i];
    }

    return *((float *)&data);
}

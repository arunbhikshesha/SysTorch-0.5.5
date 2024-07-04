/**
 ******************************************************************************
 * @file    fdcan2.c
 * @author  WBO
 * @brief   FDCAN2 configuratio, receive and transmit
 ******************************************************************************
 *
 ******************************************************************************
 */

#include <string.h>
#include "main.h"
#include "fdcan2.h"
#include "bootloader_util.h"
#include "TestBoard.h"
#include "DS2431.h"

// CAN transmit instance struct
typedef struct mcal_can_tx_ins {
    FDCAN_TxHeaderTypeDef header;
    uint32_t buf_index;
    uint8_t buff[8];
} mcal_can_tx_ins_t;

typedef enum mcal_can2_tx_idx {
    MCAL_CAN2_TX_0x100, // Data sent out
    MCAL_CAN2_TX_0x101, // Data sent out
    MCAL_CAN2_TX_0x401, // Commands, flash function
    MCAL_CAN2_TX_0x1FF, // Test Commands, flash function
    MCAL_CAN2_TX_0x200, // Test Acknowledge Commands, flash function
    MCAL_CAN2_TX_0x1FA, // Ackowledgement for EEPROM ID request message
    MCAL_CAN2_TX_0x1E1, // Write/read Ack. Torch type
    MCAL_CAN2_TX_0x1E3, // Clear EEprom Memory Ack.
    MCAL_CAN2_TX_0x1E5, // Test eeprom Acknowledgement
    MCAL_CAN2_TX_NUM
} mcal_can1_tx_idx_t;

FDCAN_HandleTypeDef hfdcan2;
param_info_to_gui mapro_to_gui_update_param;
uint8_t rx_buff[8];

static mcal_can_tx_ins_t tx[MCAL_CAN2_TX_NUM];

static FDCAN_HandleTypeDef *p_hfdcan;
static FDCAN_RxHeaderTypeDef rx_header;

static FDCAN_ProtocolStatusTypeDef fdcan_error_status;
static FDCAN_ErrorCountersTypeDef fdcan_error_counters;
static can_error_counters fdcan_error_type_counters;
static uint8_t can_busoff_flag = 0;

extern test_result_status_t TestResult;
extern test_state_status_t TestState;
extern uint8_t TestMode;
extern uint16_t TestModeTimeout;
extern uint16_t TestModeLEDTimeout;
extern test_stage_t e_TestStage;
extern SE_FwExchgData_TypeDef BL_ExcData; // Excange Data von Bootloader

// FDCAN2 Initialization Function
void MX_FDCAN2_Init(void) {
    hfdcan2.Instance = FDCAN2;
    hfdcan2.Init.FrameFormat = FDCAN_FRAME_CLASSIC;
    hfdcan2.Init.Mode = FDCAN_MODE_NORMAL;
    hfdcan2.Init.AutoRetransmission = ENABLE;
    hfdcan2.Init.TransmitPause = DISABLE;
    hfdcan2.Init.ProtocolException = DISABLE;
    hfdcan2.Init.NominalPrescaler = 84;
    hfdcan2.Init.NominalSyncJumpWidth = 2;
    hfdcan2.Init.NominalTimeSeg1 = 10;
    hfdcan2.Init.NominalTimeSeg2 = 5;
    hfdcan2.Init.DataPrescaler = 1;
    hfdcan2.Init.DataSyncJumpWidth = 2;
    hfdcan2.Init.DataTimeSeg1 = 10;
    hfdcan2.Init.DataTimeSeg2 = 5;
    hfdcan2.Init.StdFiltersNbr = 1;
    hfdcan2.Init.ExtFiltersNbr = 0;
    hfdcan2.Init.TxFifoQueueMode = FDCAN_TX_FIFO_OPERATION;
    if (HAL_FDCAN_Init(&hfdcan2) != HAL_OK) {
        Serial_COM_PutString("\r\nCAN init failed");
    }

    p_hfdcan = &hfdcan2;
    FDCAN_FilterTypeDef p_flt;
    p_flt.IdType = FDCAN_STANDARD_ID;
    p_flt.FilterIndex = 0x00;
    p_flt.FilterType = FDCAN_FILTER_RANGE;
    p_flt.FilterConfig = FDCAN_FILTER_TO_RXFIFO0;
    p_flt.FilterID1 = 0x00;
    p_flt.FilterID2 = 0x7FF;

    if (HAL_FDCAN_ConfigFilter(p_hfdcan, &p_flt) != HAL_OK) {
        Serial_COM_PutString("\r\nCAN config failed");
    }

    for (int i = 0; i < MCAL_CAN2_TX_NUM; i++) {
        if (i == MCAL_CAN2_TX_0x100) {
            tx[i].header.Identifier = 0x100;
        }
        if (i == MCAL_CAN2_TX_0x101) {
            tx[i].header.Identifier = 0x101;
        }
        if (i == MCAL_CAN2_TX_0x401) {
            tx[i].header.Identifier = 0x401;
        }
        if (i == MCAL_CAN2_TX_0x1FF) {
            tx[i].header.Identifier = 0x1ff;
        }
        if (i == MCAL_CAN2_TX_0x200) {
            tx[i].header.Identifier = 0x200;
        }
        if (i == MCAL_CAN2_TX_0x1FA) {
            tx[i].header.Identifier = 0x1FA;
        }
        if (i == MCAL_CAN2_TX_0x1E1) {
            tx[i].header.Identifier = 0x1E1;
        }
        if (i == MCAL_CAN2_TX_0x1E3) {
            tx[i].header.Identifier = 0x1E3;
        }
        if (i == MCAL_CAN2_TX_0x1E5) {
            tx[i].header.Identifier = 0x1E5;
        }
        tx[i].header.IdType = FDCAN_STANDARD_ID;
        tx[i].header.TxFrameType = FDCAN_DATA_FRAME;
        tx[i].header.DataLength = FDCAN_DLC_BYTES_8;
        tx[i].header.ErrorStateIndicator = FDCAN_ESI_ACTIVE;
        tx[i].header.BitRateSwitch = FDCAN_BRS_OFF;
        tx[i].header.FDFormat = FDCAN_CLASSIC_CAN;
        tx[i].header.TxEventFifoControl = FDCAN_NO_TX_EVENTS;
        tx[i].header.MessageMarker = 0U;
    }

    for (int i = 0; i < MCAL_CAN2_TX_NUM; i++) {
        tx[i].buf_index = 0;
    }
    if (HAL_FDCAN_Start(p_hfdcan)) {
        Serial_COM_PutString("\r\nCAN start failed");
    }

    HAL_NVIC_SetPriority(FDCAN2_IT0_IRQn, 1, 1);
    HAL_NVIC_EnableIRQ(FDCAN2_IT0_IRQn);
    if (HAL_FDCAN_ActivateNotification(p_hfdcan,
                                       FDCAN_IT_RX_FIFO0_NEW_MESSAGE | FDCAN_IT_ERROR_LOGGING_OVERFLOW | FDCAN_IR_EP |
                                           FDCAN_IR_EW | FDCAN_IR_BO,
                                       FDCAN_IT_TX_COMPLETE)) {
        Serial_COM_PutString("\r\nCAN active Notification failed");
    }
}

/**
 * @brief Try to receive a message
 * @param p_msg_id: pointer to the ID of the received message
 * @param data: data of the received message
 * @retval 1 if a message received, 0 if no message received
 */
int fdcan2_receive(int *p_msg_id, uint8_t data[FDCAN2_DATA_SIZE]) {
    int received = 0;

    /* Check if a message received on FDCAN instance */
    if (HAL_FDCAN_GetRxFifoFillLevel(p_hfdcan, FDCAN_RX_FIFO0) > 0) {
        memset(data, 0, FDCAN2_DATA_SIZE);

        /* Retrieve Rx message from RX FIFO0 */
        if (HAL_FDCAN_GetRxMessage(p_hfdcan, FDCAN_RX_FIFO0, &rx_header, data) == HAL_OK) {
            *p_msg_id = rx_header.Identifier;

            received = 1;
        } else {
            Serial_COM_PutString("\r\nCAN receive failed");
        }
    }
    return received;
}

/**
 * @brief Send a message
 * @param msg_id: message ID
 * @param data: message data
 * @retval none
 */
void fdcan2_send(int msg_id, uint8_t data[FDCAN2_DATA_SIZE]) {
    mcal_can_tx_ins_t tx_instance;
    switch (msg_id) {
    case MSG_INPUTS:
        tx_instance = tx[MCAL_CAN2_TX_0x100];
        break;
    case MSG_REQUESTS:
        tx_instance = tx[MCAL_CAN2_TX_0x101];
        break;
    case MSG_0x401:
        tx_instance = tx[MCAL_CAN2_TX_0x401];
        break;
    case MSG_0x1FF:
        tx_instance = tx[MCAL_CAN2_TX_0x1FF];
        break;
    case MSG_0x200:
        tx_instance = tx[MCAL_CAN2_TX_0x200];
        break;
    case MSG_0x1FA:
        tx_instance = tx[MCAL_CAN2_TX_0x1FA];
        break;
    case MSG_0x1E1:
        tx_instance = tx[MCAL_CAN2_TX_0x1E1];
        break;
    case MSG_0x1E3:
        tx_instance = tx[MCAL_CAN2_TX_0x1E3];
        break;
    case MSG_0x1E5:
        tx_instance = tx[MCAL_CAN2_TX_0x1E5];
        break;

    default:
        return;
    }
    memcpy(&tx_instance.buff[0], &data[0], 8U);

    if (!HAL_FDCAN_IsTxBufferMessagePending(p_hfdcan, tx_instance.buf_index)) {
        if (HAL_FDCAN_AddMessageToTxFifoQ(p_hfdcan, &tx_instance.header, &tx_instance.buff[0])) {
            Serial_COM_PutString("\r\nTransmission request Error");
        } else {
            tx_instance.buf_index = HAL_FDCAN_GetLatestTxFifoQRequestBuffer(p_hfdcan);
        }
    }
}

/**
 * @brief  param_clear_flag
 * @param  void.
 * @retval void.
 */
void param_clear_flag() {
    mapro_to_gui_update_param.flag = MSG_0x110_clearall;
}

/**
 * @brief  get_param_id
 * @param  void.
 * @retval uint8_t Paramter ID.
 */
uint8_t get_param_id() {
    return mapro_to_gui_update_param.ID;
}

/**
 * @brief  param_update_to_gui
 * @param  param_info_to_gui.
 * @retval void.
 */
void param_update_to_gui(param_info_to_gui *can_param_data) {
    can_param_data->flag = mapro_to_gui_update_param.flag;
    can_param_data->ID = mapro_to_gui_update_param.ID;
    can_param_data->image = mapro_to_gui_update_param.image;
    can_param_data->max = mapro_to_gui_update_param.max;
    can_param_data->min = mapro_to_gui_update_param.min;
    can_param_data->text = mapro_to_gui_update_param.text;
    can_param_data->type = mapro_to_gui_update_param.type;
    can_param_data->unit_id = mapro_to_gui_update_param.unit_id;
    can_param_data->value = mapro_to_gui_update_param.value;
    can_param_data->total_params = mapro_to_gui_update_param.total_params;
}

/**
 * @brief  FDCAN2 ISR
 * @param  None.
 * @retval void.
 */
void FDCAN2_IT0_IRQHandler(void) {
    HAL_FDCAN_IRQHandler(&hfdcan2);
}

/**
 * @brief  FDCAN2 RXFIFO0 message callback
 * @param  CAN handler, RXFIFO0 interrupts.
 * @retval void.
 */
void HAL_FDCAN_RxFifo0Callback(FDCAN_HandleTypeDef *hfdcan, uint32_t RxFifo0ITs) {
    FDCAN_RxHeaderTypeDef can_rx_head;
    uint8_t *ptrDataRead;
    uint32_t u32torchtpetowrite;
    uint8_t splitromid[8];

    if (HAL_FDCAN_GetRxMessage(&hfdcan2, FDCAN_RX_FIFO0, &can_rx_head, rx_buff)) {
    }

    switch (can_rx_head.Identifier) {
    case MSG_VALUE_UPDATE: {
        if (rx_buff[0] == MSG_0x110_ActPar) {
            if (mapro_to_gui_update_param.flag == MSG_0x110_clearall) {
                mapro_to_gui_update_param.ID = rx_buff[1];
                mapro_to_gui_update_param.flag = MSG_0x110_setID;
            }
            if ((rx_buff[2] == MSG_0x110_value) && (mapro_to_gui_update_param.ID == rx_buff[1])) {
                mapro_to_gui_update_param.value = get_float(&rx_buff[3]);
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_11) {
                    mapro_to_gui_update_param.value =
                        (mapro_to_gui_update_param.value / MSG_0x110_ID_11_step) - MSG_0x110_ID_11_MAX;
                    mapro_to_gui_update_param.value = round(mapro_to_gui_update_param.value);
                }
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_12) {
                    mapro_to_gui_update_param.value =
                        (mapro_to_gui_update_param.value / MSG_0x110_ID_12_step) - MSG_0x110_ID_12_MAX;
                }
                mapro_to_gui_update_param.flag = MSG_0x110_setvalue;
            } else if ((rx_buff[2] == MSG_0x110_min) && (mapro_to_gui_update_param.ID == rx_buff[1])) {
                mapro_to_gui_update_param.min = get_float(&rx_buff[3]);
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_11) {
                    mapro_to_gui_update_param.min =
                        (mapro_to_gui_update_param.min / MSG_0x110_ID_11_step) - MSG_0x110_ID_11_MAX;
                    mapro_to_gui_update_param.min = round(mapro_to_gui_update_param.min);
                }
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_12) {
                    mapro_to_gui_update_param.min =
                        (mapro_to_gui_update_param.min / MSG_0x110_ID_12_step) - MSG_0x110_ID_12_MAX;
                }
                mapro_to_gui_update_param.flag = MSG_0x110_setmin;
            } else if ((rx_buff[2] == MSG_0x110_max) && (mapro_to_gui_update_param.ID == rx_buff[1])) {
                mapro_to_gui_update_param.max = get_float(&rx_buff[3]);
                // below logic is not 100% fail proof but for current scenerio works well as the max value more than 2
                // will not be received at all. Modify/change later
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_11) {
                    mapro_to_gui_update_param.max =
                        (mapro_to_gui_update_param.max / MSG_0x110_ID_11_step) - MSG_0x110_ID_11_MAX;
                    mapro_to_gui_update_param.max = round(mapro_to_gui_update_param.max);
                }
                if (mapro_to_gui_update_param.ID == MSG_0x110_ID_12) {
                    mapro_to_gui_update_param.max =
                        (mapro_to_gui_update_param.max / MSG_0x110_ID_12_step) - MSG_0x110_ID_12_MAX;
                }
                mapro_to_gui_update_param.flag = MSG_0x110_setmax;
            } else if ((rx_buff[2] == MSG_0x110_format) && (mapro_to_gui_update_param.ID == rx_buff[1])) {
                {
                    mapro_to_gui_update_param.unit_id = rx_buff[3];
                    mapro_to_gui_update_param.type = rx_buff[4];
                    mapro_to_gui_update_param.image = rx_buff[5];
                    mapro_to_gui_update_param.text = rx_buff[6];
                    mapro_to_gui_update_param.flag = MSG_0x110_setformat;
                }
            }
        }
    } break;
    case MSG_TORCH_STATE: {
        mapro_to_gui_update_param.total_params = rx_buff[1];
        mapro_to_gui_update_param.total_qas = rx_buff[2];
    } break;
    case MSG_0x200:
        if (rx_buff[3] == 'A' && rx_buff[4] == 'C' && rx_buff[5] == 'K') {
            TestState.CANTestState = Completed;
            TestResult.CANTestResult = Pass;
            // Serial_COM_PutString("\r\nCAN Test Completed, PASS \n");
        }
    case MSG_0x1FF: // 0x1FF test message receuved
    {
        if (rx_buff[0] == TEST_MODE_T && rx_buff[1] == TEST_MODE_C && rx_buff[2] == TEST_MODE_2 &&
            rx_buff[3] == TEST_MODE_2) {
            if (e_TestStage == NoState && TestMode == 0) { // to be
                TestmodeStart();
            }
        }
    } break;
    case MSG_0x1E4: // Test BKC
    {
        if (rx_buff[6] == CAN_SW_ID_SystemTorch_FW && rx_buff[7] == TORCH_ID && rx_buff[0] == CMD_BKCTEST_STARTTEST) {
            rx_buff[1] = ow_BKCTest();
            rx_buff[0] = CMD_BKCTEST_STARTTEST;
            rx_buff[2] = 0;
            rx_buff[3] = 0;
            rx_buff[4] = 0;
            rx_buff[5] = 0;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1E5, rx_buff); // ack
        }
    } break;
    case MSG_0x1FB: // EEPROM ID request
    {
        if (rx_buff[6] == CAN_SW_ID_SystemTorch_FW && rx_buff[7] == TORCH_ID && rx_buff[0] == CMD_EEPROMID_READ) {
            ow_split_ROMID_ds2431(&(splitromid[0]));
            // packet 1
            rx_buff[0] = CMD_EEPROMID_READ;
            rx_buff[1] = splitromid[0];
            rx_buff[2] = splitromid[1];
            rx_buff[3] = splitromid[2];
            rx_buff[4] = splitromid[3];
            rx_buff[5] = 0x00;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1FA, rx_buff);
            HAL_Delay(100);
            // packet 2
            rx_buff[0] = CMD_EEPROMID_READ_PACKET2;
            rx_buff[1] = splitromid[4];
            rx_buff[2] = splitromid[5];
            rx_buff[3] = splitromid[6];
            rx_buff[4] = splitromid[7];
            rx_buff[5] = 0x00;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1FA, rx_buff);
        }
    } break;
    case MSG_0x1E0: // torch type read/write
    {
        if (rx_buff[6] == CAN_SW_ID_SystemTorch_FW && rx_buff[7] == TORCH_ID && rx_buff[0] == CMD_TORCHTYPE_WRITE) {
            u32torchtpetowrite = (rx_buff[1] | rx_buff[2] << 8 | rx_buff[3] << 16 | rx_buff[4] << 24);
            rx_buff[1] = ow_Write_TorchType(u32torchtpetowrite);
            rx_buff[0] = CMD_TORCHTYPE_WRITE;
            rx_buff[2] = 0;
            rx_buff[3] = 0;
            rx_buff[4] = 0;
            rx_buff[5] = 0;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1E1, rx_buff);
        }
        if (rx_buff[6] == CAN_SW_ID_SystemTorch_FW && rx_buff[7] == TORCH_ID && rx_buff[0] == CMD_TORCHTYPE_READ) {

            rx_buff[5] = ow_read_TorchType(&u32torchtpetowrite);
            if (rx_buff[5] == CMD_BKCTEST_PASS) {
                ptrDataRead = (uint8_t *)&u32torchtpetowrite;
                rx_buff[1] = *ptrDataRead++;
                rx_buff[2] = *ptrDataRead++;
                rx_buff[3] = *ptrDataRead++;
                rx_buff[4] = *ptrDataRead++;
            } else {
                rx_buff[1] = 0;
                rx_buff[2] = 0;
                rx_buff[3] = 0;
                rx_buff[4] = 0;
            }

            rx_buff[0] = CMD_TORCHTYPE_READ;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1E1, rx_buff);
        }
    } break;
    case MSG_0x1E2: // EEPROM clear
    {
        if (rx_buff[6] == CAN_SW_ID_SystemTorch_FW && rx_buff[7] == TORCH_ID) {
            rx_buff[1] = ow_clear_memory_ds2431((owPage)rx_buff[0]);
            rx_buff[2] = 0;
            rx_buff[3] = 0;
            rx_buff[4] = 0;
            rx_buff[5] = 0;
            rx_buff[6] = CAN_SW_ID_SystemTorch_FW;
            rx_buff[7] = TORCH_ID;
            fdcan2_send(MSG_0x1E3, rx_buff);
        }
    } break;
    case MSG_COMMTEST: {
        msg_send(MSG_COMMTEST_ANSWER, rx_buff);
    } break;
    case MSG_0x400: {
        if (rx_buff[0] == CAN_SW_ID_SystemTorch_FW) {
            switch (rx_buff[1]) {
#if defined(G4xx_BL)
            case CO_SET_DETACH:
                // DETACH Befehl, Schreiben in Backup Register,  um im Bootloader Moduls zu starten
                BLUtil_enable_pwr_bkp_clock();
                BLUtil_write_backup_register(GLOBAL_TAMP_REG, GLOBAL_MAGIC_CODE);
                BLUtil_nvic_system_reset();
                break;
#endif // G4xx_BL

            case CO_GET_PRGVERSION:
                // leave data[0] and data[1] untouched
                rx_buff[2] = VERSION_MAJOR;
                rx_buff[3] = VERSION_MINOR;
                rx_buff[4] = VERSION_PATCHLEVEL;
                rx_buff[5] = 0x0;
                rx_buff[6] = 0x0;
                rx_buff[7] = TORCH_ID;
                fdcan2_send(MSG_0x401, rx_buff);
                break;

            case CO_GET_OSVERSION:
                // leave data[0] and data[1] untouched
                rx_buff[2] = (uint8_t)(K_VERSION & 0xff);
                rx_buff[3] = (uint8_t)((K_VERSION & 0xff00) >> 8);
                rx_buff[4] = (uint8_t)((K_VERSION & 0xff0000) >> 16);
                rx_buff[5] = (uint8_t)((K_VERSION & 0xff000000) >> 24);
                rx_buff[6] = GEN3_ID;
                rx_buff[7] = TORCH_ID;
                fdcan2_send(MSG_0x401, rx_buff);
                break;

            case CO_GET_LOCKSTATE:
                if (rx_buff[7] == TORCH_ID) {
                    // leave data[0] and data[1] untouched
                    rx_buff[2] = GEN3_ID;
                    rx_buff[3] = (uint8_t)BL_ExcData.KeyLockState;
                    rx_buff[4] = 0;
                    rx_buff[5] = 0;
                    rx_buff[6] = 0;
                    rx_buff[7] = 0;
                    fdcan2_send(MSG_0x401, rx_buff);
                }
                break;
            }
        }
    } break;
    default:
        break;
    }
}

/**
 * @brief  FDCAN2 Error Status callback
 * @param  CAN handler, Error Status interrupts.
 * @retval void.
 */
void HAL_FDCAN_ErrorStatusCallback(FDCAN_HandleTypeDef *hfdcan, uint32_t ErrorStatusITs) {
    if (!can_busoff_flag) {
        if (HAL_FDCAN_GetErrorCounters(&hfdcan2, &fdcan_error_counters) != HAL_OK) {
            Serial_COM_PutString("\r\nget error counter failure");
        }

        if (HAL_FDCAN_GetProtocolStatus(&hfdcan2, &fdcan_error_status) == HAL_OK) {
            if (fdcan_error_status.LastErrorCode) {
                switch (fdcan_error_status.LastErrorCode) {
                case FDCAN_PROTOCOL_ERROR_STUFF:
                    fdcan_error_type_counters.stuff_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_STUFF;
                    break;
                case FDCAN_PROTOCOL_ERROR_FORM:
                    fdcan_error_type_counters.form_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_FORM;
                    break;
                case FDCAN_PROTOCOL_ERROR_ACK:
                    fdcan_error_type_counters.ack_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_ACK;
                    break;
                case FDCAN_PROTOCOL_ERROR_BIT1:
                    fdcan_error_type_counters.bit1_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_BIT1;
                    break;
                case FDCAN_PROTOCOL_ERROR_BIT0:
                    fdcan_error_type_counters.bit0_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_BIT0;
                    break;
                case FDCAN_PROTOCOL_ERROR_CRC:
                    fdcan_error_type_counters.crc_error++;
                    fdcan_error_type_counters.last_error = FDCAN_PROTOCOL_ERROR_CRC;
                    break;
                case FDCAN_PROTOCOL_ERROR_NO_CHANGE: {
                    switch (fdcan_error_type_counters.last_error) {
                    case FDCAN_PROTOCOL_ERROR_STUFF:
                        fdcan_error_type_counters.stuff_error++;
                        break;
                    case FDCAN_PROTOCOL_ERROR_FORM:
                        fdcan_error_type_counters.form_error++;
                        break;
                    case FDCAN_PROTOCOL_ERROR_ACK:
                        fdcan_error_type_counters.ack_error++;
                        break;
                    case FDCAN_PROTOCOL_ERROR_BIT1:
                        fdcan_error_type_counters.bit1_error++;
                        break;
                    case FDCAN_PROTOCOL_ERROR_BIT0:
                        fdcan_error_type_counters.bit0_error++;
                        break;
                    case FDCAN_PROTOCOL_ERROR_CRC:
                        fdcan_error_type_counters.crc_error++;
                        break;
                    }
                } break;
                default:
                    break;
                }
            }
            if (fdcan_error_status.BusOff) {
                Serial_COM_PutString("\r\nCAN entered BusOff");
                HAL_FDCAN_Stop(&hfdcan2);
                can_busoff_flag = 1;
            }
        }
    }
}

/**
 * @brief  All below functions are called from gui.c to update CAN information to display
 * @param  void
 * @retval uint9_t .
 */
uint8_t fdcan2_bus_status(void) {
    return can_busoff_flag;
}
uint32_t fdcan2_error_count(void) {
    return fdcan_error_counters.ErrorLogging;
}
uint8_t fdcan2_ack_error_count(void) {
    return fdcan_error_type_counters.ack_error;
}
uint8_t fdcan2_bit0_error_count(void) {
    return fdcan_error_type_counters.bit0_error;
}
uint8_t fdcan2_bit1_error_count(void) {
    return fdcan_error_type_counters.bit1_error;
}
uint8_t fdcan2_crc_error_count(void) {
    return fdcan_error_type_counters.crc_error;
}
uint8_t fdcan2_stuff_error_count(void) {
    return fdcan_error_type_counters.stuff_error;
}
uint8_t fdcan2_form_error_count(void) {
    return fdcan_error_type_counters.form_error;
}
void fdcan2_clear_busoffflag(void) {
    can_busoff_flag = 0;
}

/**
 ******************************************************************************
 * @file    inout.h
 * @author  WBO
 * @brief   Header file for button and led handling
 ******************************************************************************
 *
 ******************************************************************************
 */

#ifndef _INOUT_H
#define _INOUT_H

#include "Core.h"
#include "types.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_ll_gpio.h"

/** @defgroup inputs (push buttons and torch switch)
 * @{
 */
#define INOUT_BUTTON_DOWN  0x0100
#define INOUT_BUTTON_UP    0x0200
#define INOUT_BUTTON_LEFT  0x0400
#define INOUT_BUTTON_RIGHT 0x0800
#define INOUT_TORCH_SWITCH 0x1000
/**
 * @}
 */

/** @defgroup outputs (LEDs)
 * @{
 */
#define INOUT_LED_RH  0x0001
#define INOUT_LED_EXT 0x0010
/**
 * @}
 */

void Toggle_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Reset_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);
void Set_LED(GPIO_TypeDef *GPIOx, uint16_t GPIO_Pin);

/**
 * @brief Get mask of inputs (pressed push buttons)
 * @param p_inputs: pointer to mask of pressed buttons
 * @retval none
 */
void inout_get_inputs(uint16_t *p_inputs);
void IO_Init();

/**
 * @brief Set mask of outputs (LEDs).
 *
 * If INOUT_LED_EXT is on INOUT_LED_RH is off.
 *
 * @param outputs: mask of outputs
 * @retval none
 */
void inout_set_outputs(uint16_t outputs);

/**
 * @brief Trigger outputs. Function has to be called cyclic with frequence > 200Hz.
 * @retval none
 */
void inout_trigger_outputs(void);

uint32_t IOProcessKeys(CoreRoot RootObject);

#endif //_INOUT_H
/* USER CODE BEGIN Header */
/**
 ******************************************************************************
 * @file           : main.h
 * @brief          : Header for main.c file.
 *                   This file contains the common defines of the application.
 ******************************************************************************
 * @attention
 *
 * <h2><center>&copy; Copyright (c) 2021 STMicroelectronics.
 * All rights reserved.</center></h2>
 *
 * This software component is licensed by ST under BSD 3-Clause license,
 * the "License"; You may not use this file except in compliance with the
 * License. You may obtain a copy of the License at:
 *                        opensource.org/licenses/BSD-3-Clause
 *
 ******************************************************************************
 */
/* USER CODE END Header */

/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef __MAIN_H
    #define __MAIN_H

    #ifdef __cplusplus
extern "C" {
    #endif

    /* Includes ------------------------------------------------------------------*/
    #include "stm32g4xx_hal.h"
    #include "stm32g4xx_ll_rcc.h"
    #include "stm32g4xx_ll_bus.h"
    #include "stm32g4xx_ll_crs.h"
    #include "stm32g4xx_ll_system.h"
    #include "stm32g4xx_ll_exti.h"
    #include "stm32g4xx_ll_cortex.h"
    #include "stm32g4xx_ll_utils.h"
    #include "stm32g4xx_ll_pwr.h"
    #include "stm32g4xx_ll_dma.h"
    #include "stm32g4xx_ll_gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
    #include "s4-config/peripherie/SoftwareEnums.h"
    #include "msg.h"
    #include "serial.h"
    #include "version.h"
/* USER CODE END Includes */

/* Exported types ------------------------------------------------------------*/
/* USER CODE BEGIN ET */
extern FDCAN_HandleTypeDef hfdcan2;
extern TIM_HandleTypeDef htim15;
extern TIM_HandleTypeDef htim16;
extern TIM_HandleTypeDef htim17;
extern uint32_t LED_EXT_Pin;
extern GPIO_TypeDef *LED_EXT_GPIO_Port;
extern uint32_t LED_1_Pin;
extern GPIO_TypeDef *LED_1_GPIO_Port;
extern uint32_t LED_2_Pin;
extern GPIO_TypeDef *LED_2_GPIO_Port;

    /* USER CODE END ET */

    /* Exported constants --------------------------------------------------------*/
    /* USER CODE BEGIN EC */

    /* USER CODE END EC */

    /* Exported macro ------------------------------------------------------------*/
    /* USER CODE BEGIN EM */
    #define TORCH_ID 0x01
/* USER CODE END EM */

/* Exported functions prototypes ---------------------------------------------*/
void Error_Handler(void);

    /* USER CODE BEGIN EFP */

    /* USER CODE END EFP */

    /* Private defines -----------------------------------------------------------*/
    #define ERR_LED_TOGGLE_TIME    12500000
    #define BUTTON_DOWN_Pin        LL_GPIO_PIN_1
    #define BUTTON_DOWN_GPIO_Port  GPIOD
    #define BUTTON_UP_Pin          LL_GPIO_PIN_2
    #define BUTTON_UP_GPIO_Port    GPIOB
    #define BUTTON_RIGHT_Pin       LL_GPIO_PIN_0
    #define BUTTON_RIGHT_GPIO_Port GPIOC
    #define BUTTON_LEFT_Pin        LL_GPIO_PIN_3
    #define BUTTON_LEFT_GPIO_Port  GPIOC
    #define TORCH_SWITCH_Pin       LL_GPIO_PIN_14
    #define TORCH_SWITCH_GPIO_Port GPIOC
    #define DSP_CS_Pin             LL_GPIO_PIN_4
    #define DSP_CS_GPIO_Port       GPIOA
    #define DSP_SCK_Pin            LL_GPIO_PIN_5
    #define DSP_SCK_GPIO_Port      GPIOA
    #define DSP_MOSI_Pin           LL_GPIO_PIN_7
    #define DSP_MOSI_GPIO_Port     GPIOA
    #define DSP_DC_Pin             LL_GPIO_PIN_10
    #define DSP_DC_GPIO_Port       GPIOB
    #define DSP_RST_Pin            LL_GPIO_PIN_11
    #define DSP_RST_GPIO_Port      GPIOB
    #define PCB_Detection_Pin      LL_GPIO_PIN_10
    #define PCB_Detection_PORT     GPIOA
    #define LED_STAT_Pin           LL_GPIO_PIN_7
    #define LED_STAT_GPIO_Port     GPIOB
    #define LED_ERR_Pin            LL_GPIO_PIN_9
    #define LED_ERR_GPIO_Port      GPIOB

/* USER CODE BEGIN Private defines */

/* USER CODE END Private defines */

    #ifdef __cplusplus
}
    #endif

#endif /* __MAIN_H */

/************************ (C) COPYRIGHT STMicroelectronics *****END OF FILE****/

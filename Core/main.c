/*
******************************************************************************
* @file main.c
* @author
* @brief
******************************************************************************
*
******************************************************************************
*/

/*** Include *****************************************************************/
#include "types.h"
#include "tms.h"
#include "init.h"
#include "gData.h"
#include "gError.h"
#include <stdio.h>

// START of project specific includes
#include "main.h"
#include "ewmain.h"
#include "fdcan2.h"
#include "timers.h"
#include "i2c.h"
#include "spi.h"
#include "dma.h"
#include "gpio.h"
#include "gui.h"
#include "inout.h"
#include "TestBoard.h"
#include "bootloader_util.h"
// END of project specific includes

#include "stm32g4xx.h"
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

// START of project specific includes
// ...
#include "DS2484.h"
#include "DS2431.h"
#include "bootloader_util.h"

#include <string.h>

// END of project specific includes

/* NO MORE DEFINITIONS */

/*** data structures ************************************************/
typedef enum msm { E_MSM_INIT, E_MSM_RUNNING, E_MSM_SHUTDOWN, E_MSM_ERROR, E_MSM_N } main_msm_t;
/* NO MORE DEFINITIONS */

/*** Definition of variables *************************************************/
__EXCHGDATA_RAM SE_FwExchgData_TypeDef BL_ExcData; // Excange Data von Bootloader
/* Private variables ---------------------------------------------------------*/
extern owParamPage1 st_OwParamPage1;
// Private function prototypes
static void SystemClock_Config(void);

// State variable for main-statemachine
static main_msm_t main_msmState = E_MSM_INIT;

/* NO MORE DEFINITIONS */

/*** Prototypes of functions *************************************************/
void mainStatemachine(void);
static stdRetVal_t msmInit(void);
static stdRetVal_t msmRun(void);
static stdRetVal_t msmError(void);
static stdRetVal_t msmFlyRestart(void);
static void msmShutdown(void);
void statusLedHandler(void);

void Error_Handler(void);

/* NO MORE DEFINITIONS */
#include "stm32g4xx_it.h"

/*** Definitions of functions ************************************************/
/* NO MORE DEFINITIONS */

/******************************************************************************
** Name               : @fn main
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark from startupcode
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
int main(void) {

    HAL_StatusTypeDef I2C_Return = HAL_OK;
    /* Init HAL and sub-modules. */
    init_initUcAndSubModules();

    /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
    HAL_Init();

    /* Configure the system clock */
    SystemClock_Config();

    /* Test Mode parameter initialization*/
    TestMode_Init();

    /* Initialize all configured peripherals */
    MX_GPIO_Init();
    MX_UART1_Init();
    MX_DMA_Init();
    MX_SPI1_Init();
    MX_TIM16_Init();
    MX_TIM17_Init();
    MX_FDCAN2_Init();
    if (PCB_Detection_Result)
        MX_I2C2_Init(); // OLD PCB TC22-E01B
    else
        MX_I2C3_Init(); // New PCB TC22-V01A
    EwInit();

    HAL_TIM_Base_Start_IT(&htim16);
    HAL_TIM_Base_Start_IT(&htim17);
    msg_send_cfg_request(); // request all of the configuration at the begining

    // ---- how to do error handling to be discussed
    I2C_Return = ow_setup_ds2484();
    update_chipstatus(I2C_Return);

    I2C_Return = ow_read_ROMID_ds2431();
    update_chipstatus(I2C_Return);

    owParamPage_init_ds2431(); // read all eeprom member element
    update_chipstatus(I2C_Return);

    while (1) {
        /* TIM16 IRQ has to been disabled during EwProcess() */
        HAL_NVIC_DisableIRQ(TIM1_UP_TIM16_IRQn);
        EwProcess();
        HAL_NVIC_EnableIRQ(TIM1_UP_TIM16_IRQn);

        /* Board Test function*/
        BoardTest();
        // MSM
        //    mainStatemachine();
        // Ruecksetzten des Watchdogs
        // watchdogReload();
        // iwdg_reload();  TODO: SGE nach template aktualisierung updaten
        // Blinky Life
        //    statusLedHandler();
        // START of project specific code
        // ...
        // END of project specific code
    } /* Infinite loop end*/
}

/******************************************************************************
** Name               : @fn mainStatemachine
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark in while 1
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void mainStatemachine(void) {
    switch (main_msmState) {
    case E_MSM_INIT: {
        if (msmInit() == E_OK) {
            main_msmState = E_MSM_RUNNING;
        } else {
            main_msmState = E_MSM_ERROR;
        }
        break;
    }
    case E_MSM_RUNNING: {
        if (msmRun() != E_OK) // Alles OK
        {
            main_msmState = E_MSM_ERROR;
        }
        // TODO: Übergang in Shutdown
        // else // Kein Fehler, normales Herunterfahren
        // {
        //   main_msmState = E_MSM_SHUTDOWN;
        // }
        break;
    }
    case E_MSM_SHUTDOWN: {
        if (msmFlyRestart() == E_OK) // Wiederanlaufen
        {
            main_msmState = E_MSM_INIT;
        } else {
            msmShutdown(); // System in sicheren Zustand bringen
        }
        break;
    }
    case E_MSM_ERROR: {
        if (msmError() == E_OK) {
            main_msmState = E_MSM_INIT;
        }
        break;
    }
    default: {
        main_msmState = E_MSM_ERROR;
        break;
    }
    }
}

/******************************************************************************
** Name               : @fn  msmInit
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static stdRetVal_t msmInit(void) {
    stdRetVal_t retVal = E_OK;
    // START of project specific code
    // ...
    // END of project specific code
    return retVal;
}

/******************************************************************************
** Name               : @fn  msn_error
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static stdRetVal_t msmRun(void) {
    // Definition of local data
    stdRetVal_t retVal = E_OK;
    // START of project specific code
    // ...
    // END of project specific code
    return retVal;
}

/******************************************************************************
** Name               : @fn  msmError
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static stdRetVal_t msmError(void) {
    // Definition of local data
    stdRetVal_t retVal = E_NOK;
    // START of project specific code
    // ..
    // END of project specific code
    return retVal;
}

/******************************************************************************
** Name               : @fn  msmFlyRestart
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static stdRetVal_t msmFlyRestart(void) {
    // Definition of local data
    stdRetVal_t retVal = E_NOK;
    // START of project specific code
    // ...
    // END of project specific code
    return retVal;
}

/******************************************************************************
** Name               : @fn  msmShutdown
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
static void msmShutdown(void) {
    // START of project specific code
    // ...
    // END of porject specific code
}

/******************************************************************************
** Name               : @fn  statusLedHandler
**
** Created from /on   : @author / @date
**
** Description        : @brief
**
** Calling            : @remark
**
** InputValues        : @param none
** OutputValues       : @retval none
******************************************************************************/
void statusLedHandler(void) {
    if (true == Tms_isTmsTimerElapsed(TMS_StatusLED)) {
        // TODO    Tms_reloadTmsTimer(TMS_StatusLED, main_statusLedTmsVal[main_msmState]);
        //  START of project specific code
        //  ...
        //  END of project specific code
    }
}

/******************************************************************************
** Name               : @fn SystemClock_Config
** Description        : system clock initialisation
** Parameters         : void
** return type        : void
******************************************************************************/
void SystemClock_Config(void) {
    RCC_OscInitTypeDef RCC_OscInitStruct = {0};
    RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};
    RCC_PeriphCLKInitTypeDef PeriphClkInit = {0};

    /** Configure the main internal regulator output voltage
     */
    HAL_PWREx_ControlVoltageScaling(PWR_REGULATOR_VOLTAGE_SCALE1_BOOST);
    /** Initializes the CPU, AHB and APB busses clocks
     */

    RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_LSI | RCC_OSCILLATORTYPE_HSE;
    RCC_OscInitStruct.HSEState = RCC_HSE_ON;
    RCC_OscInitStruct.LSIState = RCC_LSI_ON;
    RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
    RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
    RCC_OscInitStruct.PLL.PLLM = RCC_PLLM_DIV1;
    RCC_OscInitStruct.PLL.PLLN = 42;
    RCC_OscInitStruct.PLL.PLLP = RCC_PLLP_DIV2;
    RCC_OscInitStruct.PLL.PLLQ = RCC_PLLQ_DIV2;
    RCC_OscInitStruct.PLL.PLLR = RCC_PLLR_DIV2;

    if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the CPU, AHB and APB busses clocks
     */
    RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK | RCC_CLOCKTYPE_SYSCLK | RCC_CLOCKTYPE_PCLK1 | RCC_CLOCKTYPE_PCLK2;
    RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
    RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
    RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
    RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

    if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_8) != HAL_OK) {
        Error_Handler();
    }
    /** Initializes the peripherals clocks
     */
    PeriphClkInit.PeriphClockSelection = RCC_PERIPHCLK_RTC | RCC_PERIPHCLK_FDCAN;
    PeriphClkInit.FdcanClockSelection = RCC_FDCANCLKSOURCE_PCLK1;
    PeriphClkInit.RTCClockSelection = RCC_RTCCLKSOURCE_LSI;
    if (HAL_RCCEx_PeriphCLKConfig(&PeriphClkInit) != HAL_OK) {
        Error_Handler();
    }
    /** Enables the Clock Security System
     */
    // HAL_RCC_EnableCSS();

    /* Update the time base */
    if (HAL_InitTick(TICK_INT_PRIORITY) != HAL_OK) {
        Error_Handler();
    }
}

/******************************************************************************
** Name               : @fn Error_Handler
** Description        : Error handing
** Parameters         : void
** return type        : void
******************************************************************************/
void Error_Handler(void) {
    /* USER CODE BEGIN Error_Handler_Debug */
    /* User can add his own implementation to report the HAL error return state */
    __disable_irq();
    Serial_COM_PutString("\r\nHard Fault occured");
    // Report Error with Red LEDs
    Set_LED(LED_ERR_GPIO_Port, LED_ERR_Pin);

    while (1) {
        HAL_GPIO_TogglePin(LED_STAT_GPIO_Port, LED_STAT_Pin);
        for (int i = 0; i < ERR_LED_TOGGLE_TIME; i++)
            ;
    }
    /* USER CODE END Error_Handler_Debug */
}

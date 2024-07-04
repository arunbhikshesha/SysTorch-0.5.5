/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This software is delivered "as is" and shows the usage of other software
* components. It is provided as an example software which is intended to be
* modified and extended according to particular requirements.
*
* TARA Systems hereby disclaims all warranties and conditions with regard to the
* software, including all implied warranties and conditions of merchantability
* and non-infringement of any third party IPR or other rights which may result
* from the use or the inability to use the software.
*
********************************************************************************
*
* DESCRIPTION:
*   This file is part of the interface (glue layer) between an Embedded Wizard
*   generated UI application and the board support package (BSP) of a dedicated
*   target.
*   Please note: The implementation of this module is partially based on
*   examples that are provided within the STM32 cube firmware. In case you want
*   to adapt this module to your custom specific hardware, please adapt the
*   hardware initialization code according your needs or integrate the generated
*   initialization code created by using the tool CubeMX.
*   This template provides access to some LEDs and buttons of the board.
*
*******************************************************************************/

#include "ewconfig.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_nucleo.h"
#include "ewrte.h"

#include "ew_bsp_inout.h"

//#define EW_LED                LED4  --> LED used by SPI for display communication

/* joystick configuration */
#define BUTTON_CENTER_Pin           GPIO_PIN_8
#define BUTTON_CENTER_GPIO_Port     GPIOC
#define BUTTON_RIGHT_Pin            GPIO_PIN_11
#define BUTTON_RIGHT_GPIO_Port      GPIOC
#define BUTTON_UP_Pin               GPIO_PIN_12
#define BUTTON_UP_GPIO_Port         GPIOC
#define BUTTON_LEFT_Pin             GPIO_PIN_9
#define BUTTON_LEFT_GPIO_Port       GPIOC
#define BUTTON_DOWN_Pin             GPIO_PIN_10
#define BUTTON_DOWN_GPIO_Port       GPIOC

static TButtonCallback        ButtonCallback = NULL;


/*******************************************************************************
* FUNCTION:
*   EwBspInOutInitButton
*
* DESCRIPTION:
*   Configures one hardware button of the board used for demo applications.
*
* ARGUMENTS:
*   aButtonCallback - The button callback.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspInOutInitButton( TButtonCallback aButtonCallback )
{
#if 0 //XXXX Nicht für SysBrenner

  GPIO_InitTypeDef GPIO_InitStructure;

  /* Enable GPIOC clock */
  __HAL_RCC_GPIOC_CLK_ENABLE();

  /* Configure GPIO pins : USER_BUTTON_PIN */
  GPIO_InitStructure.Mode = GPIO_MODE_IT_RISING_FALLING;
  GPIO_InitStructure.Pull = GPIO_PULLDOWN;
  GPIO_InitStructure.Pin = USER_BUTTON_PIN;
  HAL_GPIO_Init( GPIOC, &GPIO_InitStructure );

  /* Configure GPIO pins : BUTTON_RIGHT_Pin BUTTON_UP_Pin BUTTON_CENTER_Pin BUTTON_LEFT_Pin BUTTON_DOWN_Pin */
  GPIO_InitStructure.Pin = BUTTON_RIGHT_Pin | BUTTON_UP_Pin | BUTTON_CENTER_Pin | BUTTON_LEFT_Pin | BUTTON_DOWN_Pin;
  GPIO_InitStructure.Mode = GPIO_MODE_INPUT;
  GPIO_InitStructure.Pull = GPIO_PULLUP;
  HAL_GPIO_Init(GPIOC, &GPIO_InitStructure);
#endif //0 //XXXX Nicht für SysBrenner


#if 0 //XXXX
  /* Enable and set line 4_15 Interrupt to the lowest priority */
  HAL_NVIC_SetPriority( EXTI4_15_IRQn, 2, 0 );
  HAL_NVIC_EnableIRQ( EXTI4_15_IRQn );

  ButtonCallback = aButtonCallback;
#endif //0 XXXX
}


/*******************************************************************************
* FUNCTION:
*   EwBspInOutInitLed
*
* DESCRIPTION:
*   Configures one LED of the board used for demo applications.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspInOutInitLed( void )
{
  // BSP_LED_Init( EW_LED );  LED used by SPI for display communication
}


/*******************************************************************************
* FUNCTION:
*   EwBspInOutLedOn
*
* DESCRIPTION:
*   Switch LED on (used for demo applications).
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspInOutLedOn( void )
{
  // BSP_LED_On( EW_LED );  LED used by SPI for display communication
}


/*******************************************************************************
* FUNCTION:
*   EwBspInOutLedOff
*
* DESCRIPTION:
*   Switch LED off (used for demo applications).
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspInOutLedOff( void )
{
  // BSP_LED_Off( EW_LED ); LED used by SPI for display communication
}


/*******************************************************************************
* FUNCTION:
*   EwBspInOutEventHandler
*
* DESCRIPTION:
*   The function EwBspInOutEventHandler is called from the touch screen driver
*   in case of an input event.
*
* ARGUMENTS:
*   aEventId - An optional target specific event ID.
*
* RETURN VALUE:
*   None.
*
*******************************************************************************/
void EwBspInOutEventHandler( int aEventId )
{
  uint16_t GPIO_Pin = ( uint16_t ) aEventId;

  if ( GPIO_Pin == USER_BUTTON_PIN )
  {
    GPIO_PinState pinState;

    pinState = HAL_GPIO_ReadPin( USER_BUTTON_GPIO_PORT, GPIO_Pin );

    if ( ButtonCallback )
      ButtonCallback( pinState );
  }
}


/*******************************************************************************
* FUNCTION:
*   EwBspInOutGetJoystickState
*
* DESCRIPTION:
*   The function EwBspInOutGetJoystickState returns the current state of the
*   joystick - adjusted to the current screen orientation.
*
* ARGUMENTS:
*   None.
*
* RETURN VALUE:
*   Returns the current joystick state or EW_JOYSTICK_NONE
*
*******************************************************************************/
int EwBspInOutGetJoystickState( void )
{
  if ( HAL_GPIO_ReadPin( BUTTON_CENTER_GPIO_Port, BUTTON_CENTER_Pin ) == GPIO_PIN_RESET )
    return EW_JOYSTICK_CENTER;

  else if ( HAL_GPIO_ReadPin( BUTTON_RIGHT_GPIO_Port, BUTTON_RIGHT_Pin ) == GPIO_PIN_RESET )
#if EW_SURFACE_ROTATION == 90
    return EW_JOYSTICK_UP;
#elif EW_SURFACE_ROTATION == 180
    return EW_JOYSTICK_LEFT;
#elif EW_SURFACE_ROTATION == 270
    return EW_JOYSTICK_DOWN;
#else
    return EW_JOYSTICK_RIGHT;
#endif

  else if ( HAL_GPIO_ReadPin( BUTTON_LEFT_GPIO_Port, BUTTON_LEFT_Pin ) == GPIO_PIN_RESET )
#if EW_SURFACE_ROTATION == 90
    return EW_JOYSTICK_DOWN;
#elif EW_SURFACE_ROTATION == 180
    return EW_JOYSTICK_RIGHT;
#elif EW_SURFACE_ROTATION == 270
    return EW_JOYSTICK_UP;
#else
    return EW_JOYSTICK_LEFT;
#endif

  else if ( HAL_GPIO_ReadPin( BUTTON_DOWN_GPIO_Port, BUTTON_DOWN_Pin ) == GPIO_PIN_RESET )
#if EW_SURFACE_ROTATION == 90
    return EW_JOYSTICK_RIGHT;
#elif EW_SURFACE_ROTATION == 180
    return EW_JOYSTICK_UP;
#elif EW_SURFACE_ROTATION == 270
    return EW_JOYSTICK_LEFT;
#else
    return EW_JOYSTICK_DOWN;
#endif

  else if ( HAL_GPIO_ReadPin( BUTTON_UP_GPIO_Port, BUTTON_UP_Pin ) == GPIO_PIN_RESET )
#if EW_SURFACE_ROTATION == 90
    return EW_JOYSTICK_LEFT;
#elif EW_SURFACE_ROTATION == 180
    return EW_JOYSTICK_DOWN;
#elif EW_SURFACE_ROTATION == 270
    return EW_JOYSTICK_RIGHT;
#else
    return EW_JOYSTICK_UP;
#endif

  else return EW_JOYSTICK_NONE;
}


/* msy */

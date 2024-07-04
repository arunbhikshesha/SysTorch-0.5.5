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
*   This template provides access to some LEDs and buttons of the board.
*
*******************************************************************************/

#ifndef EW_BSP_INOUT_H
#define EW_BSP_INOUT_H

#ifdef __cplusplus
  extern "C"
  {
#endif

#define EW_JOYSTICK_NONE      0
#define EW_JOYSTICK_UP        1
#define EW_JOYSTICK_DOWN      2
#define EW_JOYSTICK_LEFT      3
#define EW_JOYSTICK_RIGHT     4
#define EW_JOYSTICK_CENTER    5

/*******************************************************************************
* CALLBACK:
*   TButtonCallback
*
* DESCRIPTION:
*   A function of this type has to be set with EwBspButtonConfig() to get
*   notified, everytime the hardware button is pressed or released.
*
* ARGUMENTS:
*   aButtonPresssed
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
typedef void ( * TButtonCallback ) ( int aButtonPresssed );


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
void EwBspInOutInitButton
(
  TButtonCallback             aButtonCallback
);


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
void EwBspInOutInitLed
(
  void
);


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
void EwBspInOutLedOn
(
  void
);


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
void EwBspInOutLedOff
(
  void
);


/*******************************************************************************
* FUNCTION:
*   EwBspInOutEventHandler
*
* DESCRIPTION:
*   The function EwBspInOutEventHandler is called from GPIO driver in case of
*   an input event.
*
* ARGUMENTS:
*   aEventId - An optional target specific event ID.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBspInOutEventHandler
(
  int aEventId
);


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
int EwBspInOutGetJoystickState
(
  void
);


#ifdef __cplusplus
  }
#endif

#endif /* EW_BSP_INOUT_H */


/* msy */

/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This file was generated automatically by Embedded Wizard Studio.
*
* Please do not make any modifications of this file! The modifications are lost
* when the file is generated again by Embedded Wizard Studio!
*
* The template of this heading text can be found in the file 'head.ewt' in the
* directory 'Platforms' of your Embedded Wizard installation directory. If you
* wish to adapt this text, please copy the template file 'head.ewt' into your
* project directory and edit the copy only. Please avoid any modifications of
* the original template file!
*
* Version  : 11.00
* Profile  : SysBrenner
* Platform : STM.STM32.RGB565
*
*******************************************************************************/

#ifndef Data_H
#define Data_H

#ifdef __cplusplus
  extern "C"
  {
#endif

#include "ewrte.h"
#if EW_RTE_VERSION != 0x000B0000
  #error Wrong version of Embedded Wizard Runtime Environment.
#endif

#include "ewgfx.h"
#if EW_GFX_VERSION != 0x000B0000
  #error Wrong version of Embedded Wizard Graphics Engine.
#endif

/* User defined enumeration: 'Data::ValueBarType' */
typedef enum
{
  DataValueBarTypePositive              = 0,
  DataValueBarTypePositiveNegative      = 1
} DataValueBarType;

/* User defined enumeration: 'Data::TextLineType' */
typedef enum
{
  DataTextLineTypeOnlyText              = 0,
  DataTextLineTypeIcon_Unit             = 1
} DataTextLineType;

/* User defined constant: 'Data::gScreenSize' */
extern const XRect DatagScreenSize;

#ifdef __cplusplus
  }
#endif

#endif /* Data_H */

/* Embedded Wizard */

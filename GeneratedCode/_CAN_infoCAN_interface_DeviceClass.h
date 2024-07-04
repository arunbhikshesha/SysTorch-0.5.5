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

#ifndef _CAN_infoCAN_interface_DeviceClass_H
#define _CAN_infoCAN_interface_DeviceClass_H

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

#include "_TemplatesDeviceClass.h"

/* Forward declaration of the class CAN_info::CAN_interface_DeviceClass */
#ifndef _CAN_infoCAN_interface_DeviceClass_
  EW_DECLARE_CLASS( CAN_infoCAN_interface_DeviceClass )
#define _CAN_infoCAN_interface_DeviceClass_
#endif


/* Deklaration of class : 'CAN_info::CAN_interface_DeviceClass' */
EW_DEFINE_FIELDS( CAN_infoCAN_interface_DeviceClass, TemplatesDeviceClass )
EW_END_OF_FIELDS( CAN_infoCAN_interface_DeviceClass )

/* Virtual Method Table (VMT) for the class : 'CAN_info::CAN_interface_DeviceClass' */
EW_DEFINE_METHODS( CAN_infoCAN_interface_DeviceClass, TemplatesDeviceClass )
EW_END_OF_METHODS( CAN_infoCAN_interface_DeviceClass )

/* 'C' function for method : 'CAN_info::CAN_interface_DeviceClass.Done()' */
void CAN_infoCAN_interface_DeviceClass_Done( CAN_infoCAN_interface_DeviceClass _this );

/* 'C' function for method : 'CAN_info::CAN_interface_DeviceClass.Init()' */
void CAN_infoCAN_interface_DeviceClass_Init( CAN_infoCAN_interface_DeviceClass _this, 
  XHandle aArg );

#ifdef __cplusplus
  }
#endif

#endif /* _CAN_infoCAN_interface_DeviceClass_H */

/* Embedded Wizard */

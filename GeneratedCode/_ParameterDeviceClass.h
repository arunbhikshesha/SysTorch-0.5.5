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

#ifndef _ParameterDeviceClass_H
#define _ParameterDeviceClass_H

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

/* Forward declaration of the class Parameter::DeviceClass */
#ifndef _ParameterDeviceClass_
  EW_DECLARE_CLASS( ParameterDeviceClass )
#define _ParameterDeviceClass_
#endif

/* Forward declaration of the class Parameter::Parameter */
#ifndef _ParameterParameter_
  EW_DECLARE_CLASS( ParameterParameter )
#define _ParameterParameter_
#endif


/* Deklaration of class : 'Parameter::DeviceClass' */
EW_DEFINE_FIELDS( ParameterDeviceClass, TemplatesDeviceClass )
  EW_VARIABLE( active_par,      ParameterParameter )
EW_END_OF_FIELDS( ParameterDeviceClass )

/* Virtual Method Table (VMT) for the class : 'Parameter::DeviceClass' */
EW_DEFINE_METHODS( ParameterDeviceClass, TemplatesDeviceClass )
EW_END_OF_METHODS( ParameterDeviceClass )

/* 'C' function for method : 'Parameter::DeviceClass.Done()' */
void ParameterDeviceClass_Done( ParameterDeviceClass _this );

/* 'C' function for method : 'Parameter::DeviceClass.Init()' */
void ParameterDeviceClass_Init( ParameterDeviceClass _this, XHandle aArg );

/* 'C' function for method : 'Parameter::DeviceClass.update_par_from_dev()' */
void ParameterDeviceClass_update_par_from_dev( ParameterDeviceClass _this, XUInt8 
  aNewID, XFloat aNewValue, XFloat aNewMax, XFloat aNewMin, XUInt8 aNewUnitID, XUInt8 
  aNewType, XUInt8 aNewImage, XUInt8 aNewText, XUInt8 aNewTotalParams );

/* Wrapper function for the non virtual method : 'Parameter::DeviceClass.update_par_from_dev()' */
void ParameterDeviceClass__update_par_from_dev( void* _this, XUInt8 aNewID, XFloat 
  aNewValue, XFloat aNewMax, XFloat aNewMin, XUInt8 aNewUnitID, XUInt8 aNewType, 
  XUInt8 aNewImage, XUInt8 aNewText, XUInt8 aNewTotalParams );

/* The following define announces the presence of the method Parameter::DeviceClass.update_par_from_dev(). */
#define _ParameterDeviceClass__update_par_from_dev_

#ifdef __cplusplus
  }
#endif

#endif /* _ParameterDeviceClass_H */

/* Embedded Wizard */

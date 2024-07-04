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

#ifndef _ParameterParameter_H
#define _ParameterParameter_H

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

/* Forward declaration of the class Parameter::Parameter */
#ifndef _ParameterParameter_
  EW_DECLARE_CLASS( ParameterParameter )
#define _ParameterParameter_
#endif


/* Deklaration of class : 'Parameter::Parameter' */
EW_DEFINE_FIELDS( ParameterParameter, XObject )
  EW_PROPERTY( Value,           XFloat )
  EW_PROPERTY( Min,             XFloat )
  EW_PROPERTY( Max,             XFloat )
  EW_PROPERTY( DecimalPoint,    XInt32 )
  EW_PROPERTY( totalparams,     XUInt8 )
  EW_PROPERTY( QAIndicator,     XBool )
  EW_PROPERTY( Relative,        XBool )
  EW_PROPERTY( Toggle,          XBool )
  EW_PROPERTY( text,            XUInt8 )
  EW_PROPERTY( image,           XUInt8 )
  EW_PROPERTY( type,            XUInt8 )
  EW_PROPERTY( unit_id,         XUInt8 )
  EW_PROPERTY( Id,              XUInt8 )
EW_END_OF_FIELDS( ParameterParameter )

/* Virtual Method Table (VMT) for the class : 'Parameter::Parameter' */
EW_DEFINE_METHODS( ParameterParameter, XObject )
EW_END_OF_METHODS( ParameterParameter )

/* 'C' function for method : 'Parameter::Parameter.Init()' */
void ParameterParameter_Init( ParameterParameter _this, XHandle aArg );

/* 'C' function for method : 'Parameter::Parameter.GetValueFormatted()' */
XString ParameterParameter_GetValueFormatted( ParameterParameter _this );

/* 'C' function for method : 'Parameter::Parameter.GetValuePozent()' */
XInt32 ParameterParameter_GetValuePozent( ParameterParameter _this );

/* 'C' function for method : 'Parameter::Parameter.UpdateParameter()' */
void ParameterParameter_UpdateParameter( ParameterParameter _this, XUInt32 aID, 
  XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, 
  XUInt8 atext, XUInt8 atotalparams );

/* Wrapper function for the non virtual method : 'Parameter::Parameter.UpdateParameter()' */
void ParameterParameter__UpdateParameter( void* _this, XUInt32 aID, XFloat aValue, 
  XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, XUInt8 atext, 
  XUInt8 atotalparams );

/* The following define announces the presence of the method Parameter::Parameter.UpdateParameter(). */
#define _ParameterParameter__UpdateParameter_

/* 'C' function for method : 'Parameter::Parameter.CalcAdvanced()' */
void ParameterParameter_CalcAdvanced( ParameterParameter _this );

/* 'C' function for method : 'Parameter::Parameter.UpdateParameterSim()' */
void ParameterParameter_UpdateParameterSim( ParameterParameter _this, XUInt32 aID, 
  XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, 
  XUInt8 atext );

/* Wrapper function for the non virtual method : 'Parameter::Parameter.UpdateParameterSim()' */
void ParameterParameter__UpdateParameterSim( void* _this, XUInt32 aID, XFloat aValue, 
  XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, XUInt8 atext );

/* The following define announces the presence of the method Parameter::Parameter.UpdateParameterSim(). */
#define _ParameterParameter__UpdateParameterSim_

#ifdef __cplusplus
  }
#endif

#endif /* _ParameterParameter_H */

/* Embedded Wizard */

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

#ifndef _SimulatorSimulatorClass_H
#define _SimulatorSimulatorClass_H

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

#include "_CoreTimer.h"

/* Forward declaration of the class Parameter::Parameter */
#ifndef _ParameterParameter_
  EW_DECLARE_CLASS( ParameterParameter )
#define _ParameterParameter_
#endif

/* Forward declaration of the class Simulator::SimulatorClass */
#ifndef _SimulatorSimulatorClass_
  EW_DECLARE_CLASS( SimulatorSimulatorClass )
#define _SimulatorSimulatorClass_
#endif


/* Deklaration of class : 'Simulator::SimulatorClass' */
EW_DEFINE_FIELDS( SimulatorSimulatorClass, XObject )
  EW_ARRAY   ( Parameter,       ParameterParameter, [16])
  EW_OBJECT  ( Timer,           CoreTimer )
  EW_PROPERTY( NoOfParameter,   XInt32 )
EW_END_OF_FIELDS( SimulatorSimulatorClass )

/* Virtual Method Table (VMT) for the class : 'Simulator::SimulatorClass' */
EW_DEFINE_METHODS( SimulatorSimulatorClass, XObject )
EW_END_OF_METHODS( SimulatorSimulatorClass )

/* 'C' function for method : 'Simulator::SimulatorClass.Init()' */
void SimulatorSimulatorClass_Init( SimulatorSimulatorClass _this, XHandle aArg );

/* 'C' function for method : 'Simulator::SimulatorClass.InitParameter()' */
void SimulatorSimulatorClass_InitParameter( SimulatorSimulatorClass _this );

/* 'C' function for method : 'Simulator::SimulatorClass.AddParameter()' */
void SimulatorSimulatorClass_AddParameter( SimulatorSimulatorClass _this, XInt32 
  aID, XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 
  aimage, XUInt8 atext );

/* 'C' function for method : 'Simulator::SimulatorClass.OnSetNoOfParameter()' */
void SimulatorSimulatorClass_OnSetNoOfParameter( SimulatorSimulatorClass _this, 
  XInt32 value );

/* 'C' function for method : 'Simulator::SimulatorClass.UpdateActiveParameter()' */
void SimulatorSimulatorClass_UpdateActiveParameter( SimulatorSimulatorClass _this );

#ifdef __cplusplus
  }
#endif

#endif /* _SimulatorSimulatorClass_H */

/* Embedded Wizard */

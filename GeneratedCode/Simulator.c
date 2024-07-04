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

#include "ewlocale.h"
#include "_ApplicationDeviceClass.h"
#include "_CoreTimer.h"
#include "_ParameterParameter.h"
#include "_SimulatorSimulatorClass.h"
#include "Application.h"
#include "Parameter.h"
#include "Simulator.h"

#ifndef EW_DONT_CHECK_INDEX
  /* This function is used to check the indices when accessing an array.
     If you don't want this verification add the define EW_DONT_CHECK_INDEX
     to your Makefile or project settings. */
  static int EwCheckIndex( int aIndex, int aRange, const char* aFile, int aLine )
  {
    if (( aIndex < 0 ) || ( aIndex >= aRange ))
    {
      EwPrint( "[FATAL ERROR in %s:%d] Array index %d out of bounds %d",
                aFile, aLine, aIndex, aRange );
      EwPanic();
    }
    return aIndex;
  }

  #define EwCheckIndex( aIndex, aRange ) \
    EwCheckIndex( aIndex, aRange, __FILE__, __LINE__ )
#else
  #define EwCheckIndex( aIndex, aRange ) aIndex
#endif

/* Initializer for the class 'Simulator::SimulatorClass' */
void SimulatorSimulatorClass__Init( SimulatorSimulatorClass _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  XObject__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( SimulatorSimulatorClass );

  /* ... then construct all embedded objects */
  CoreTimer__Init( &_this->Timer, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( SimulatorSimulatorClass );

  /* Call the user defined constructor */
  SimulatorSimulatorClass_Init( _this, aArg );
}

/* Re-Initializer for the class 'Simulator::SimulatorClass' */
void SimulatorSimulatorClass__ReInit( SimulatorSimulatorClass _this )
{
  /* At first re-initialize the super class ... */
  XObject__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  CoreTimer__ReInit( &_this->Timer );
}

/* Finalizer method for the class 'Simulator::SimulatorClass' */
void SimulatorSimulatorClass__Done( SimulatorSimulatorClass _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( XObject );

  /* Finalize all embedded objects */
  CoreTimer__Done( &_this->Timer );

  /* Don't forget to deinitialize the super class ... */
  XObject__Done( &_this->_.Super );
}

/* 'C' function for method : 'Simulator::SimulatorClass.Init()' */
void SimulatorSimulatorClass_Init( SimulatorSimulatorClass _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  SimulatorSimulatorClass_InitParameter( _this );
  CoreTimer_OnSetEnabled( &_this->Timer, 1 );
}

/* 'C' function for method : 'Simulator::SimulatorClass.InitParameter()' */
void SimulatorSimulatorClass_InitParameter( SimulatorSimulatorClass _this )
{
  SimulatorSimulatorClass_AddParameter( _this, 1, 12.500000f, 25.000000f, 0.500000f, 
  7, 0, 1, 0 );
  SimulatorSimulatorClass_AddParameter( _this, 2, 2.000000f, 10.000000f, 0.500000f, 
  4, 0, 3, 0 );
  SimulatorSimulatorClass_AddParameter( _this, 3, 17.400000f, 30.000000f, 0.500000f, 
  8, 0, 0, 0 );
  SimulatorSimulatorClass_AddParameter( _this, 4, 1.000000f, 2.000000f, 0.000000f, 
  22, 8, 2, 0 );
  SimulatorSimulatorClass_AddParameter( _this, 5, 1.000000f, 2.000000f, 0.000000f, 
  24, 8, 0, 1 );
  SimulatorSimulatorClass_AddParameter( _this, 6, 0.000000f, 1.000000f, 0.000000f, 
  0, 16, 4, 1 );
  SimulatorSimulatorClass_AddParameter( _this, 7, 1.000000f, 1.000000f, 0.000000f, 
  0, 16, 8, 1 );
  SimulatorSimulatorClass_AddParameter( _this, 8, 0.000000f, 0.000000f, 0.000000f, 
  0, 128, 0, 0 );
  ApplicationDeviceClass_UpdateCurrentParameter( EwGetAutoObject( &ApplicationDevice, 
  ApplicationDeviceClass ), 0 );
  ApplicationDeviceClass_UpdateNoOfParameter( EwGetAutoObject( &ApplicationDevice, 
  ApplicationDeviceClass ), _this->NoOfParameter );
  SimulatorSimulatorClass_UpdateActiveParameter( _this );
}

/* 'C' function for method : 'Simulator::SimulatorClass.AddParameter()' */
void SimulatorSimulatorClass_AddParameter( SimulatorSimulatorClass _this, XInt32 
  aID, XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 
  aimage, XUInt8 atext )
{
  ParameterParameter temp;

  temp = EwNewObject( ParameterParameter, 0 );
  ParameterParameter_UpdateParameterSim( temp, (XUInt32)aID, aValue, aMax, aMin, 
  aUnit, atype, aimage, atext );

  if ( _this->NoOfParameter < 16 )
  {
    _this->Parameter[ EwCheckIndex( _this->NoOfParameter, 16 )] = temp;
    SimulatorSimulatorClass_OnSetNoOfParameter( _this, (XInt32)( _this->NoOfParameter 
    + 1 ));
  }
}

/* 'C' function for method : 'Simulator::SimulatorClass.OnSetNoOfParameter()' */
void SimulatorSimulatorClass_OnSetNoOfParameter( SimulatorSimulatorClass _this, 
  XInt32 value )
{
  if ( _this->NoOfParameter == value )
    return;

  _this->NoOfParameter = value;
  ApplicationDeviceClass_UpdateNoOfParameter( EwGetAutoObject( &ApplicationDevice, 
  ApplicationDeviceClass ), _this->NoOfParameter );
}

/* 'C' function for method : 'Simulator::SimulatorClass.UpdateActiveParameter()' */
void SimulatorSimulatorClass_UpdateActiveParameter( SimulatorSimulatorClass _this )
{
  ParameterParameter_UpdateParameter( EwGetAutoObject( &ParameterActiveParameter, 
  ParameterParameter ), _this->Parameter[ 0 ]->Id, _this->Parameter[ 0 ]->Value, 
  _this->Parameter[ 0 ]->Max, _this->Parameter[ 0 ]->Min, _this->Parameter[ 0 ]->unit_id, 
  _this->Parameter[ 0 ]->type, _this->Parameter[ 0 ]->image, _this->Parameter[ 0 ]->text, 
  _this->Parameter[ 0 ]->totalparams );
}

/* Variants derived from the class : 'Simulator::SimulatorClass' */
EW_DEFINE_CLASS_VARIANTS( SimulatorSimulatorClass )
EW_END_OF_CLASS_VARIANTS( SimulatorSimulatorClass )

/* Virtual Method Table (VMT) for the class : 'Simulator::SimulatorClass' */
EW_DEFINE_CLASS( SimulatorSimulatorClass, XObject, Parameter, Timer, Timer, Timer, 
                 NoOfParameter, NoOfParameter, "Simulator::SimulatorClass" )
EW_END_OF_CLASS( SimulatorSimulatorClass )

/* User defined auto object: 'Simulator::Simulator' */
EW_DEFINE_AUTOOBJECT( SimulatorSimulator, SimulatorSimulatorClass )

/* Initializer for the auto object 'Simulator::Simulator' */
void SimulatorSimulator__Init( SimulatorSimulatorClass _this )
{
  EW_UNUSED_ARG( _this );
}

/* Table with links to derived variants of the auto object : 'Simulator::Simulator' */
EW_DEFINE_AUTOOBJECT_VARIANTS( SimulatorSimulator )
EW_END_OF_AUTOOBJECT_VARIANTS( SimulatorSimulator )

/* Embedded Wizard */

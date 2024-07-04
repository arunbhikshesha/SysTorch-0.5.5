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
#include "_ParameterDeviceClass.h"
#include "_ParameterParameter.h"
#include "Parameter.h"

/* Strings for the language 'Default'. */
EW_CONST_STRING_PRAGMA static const unsigned short _StringsDefault0[] =
{
  0xFFFF, 0xFFFF, 0xC557, 0x002D, 0x0030, 0x0000, 0xC557, 0x0030, 0x0000
};

/* Constant values used in this 'C' module only. */
static const XStringRes _Const0000 = { _StringsDefault0, 0x0003 };
static const XStringRes _Const0001 = { _StringsDefault0, 0x0007 };

/* User defined inline code: 'Parameter::Inline' */
#include "data_handler_parameter.h"

/* Initializer for the class 'Parameter::Parameter' */
void ParameterParameter__Init( ParameterParameter _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  XObject__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ParameterParameter );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ParameterParameter );

  /* ... and initialize objects, variables, properties, etc. */
  _this->Value = 12.500000f;
  _this->Id = 1;
  _this->Min = 0.500000f;
  _this->Max = 25.000000f;
  _this->unit_id = 25;
  _this->type = 25;
  _this->image = 25;
  _this->text = 25;
  _this->totalparams = 25;

  /* Call the user defined constructor */
  ParameterParameter_Init( _this, aArg );
}

/* Re-Initializer for the class 'Parameter::Parameter' */
void ParameterParameter__ReInit( ParameterParameter _this )
{
  /* At first re-initialize the super class ... */
  XObject__ReInit( &_this->_.Super );
}

/* Finalizer method for the class 'Parameter::Parameter' */
void ParameterParameter__Done( ParameterParameter _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( XObject );

  /* Don't forget to deinitialize the super class ... */
  XObject__Done( &_this->_.Super );
}

/* 'C' function for method : 'Parameter::Parameter.Init()' */
void ParameterParameter_Init( ParameterParameter _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  EwNotifyObjObservers((XObject)_this, 0 );
}

/* 'C' function for method : 'Parameter::Parameter.GetValueFormatted()' */
XString ParameterParameter_GetValueFormatted( ParameterParameter _this )
{
  XString temp;

  switch ( _this->unit_id )
  {
    case 22 :
      temp = EwNewStringFloat( _this->Value, 0, 0 );
    break;

    case 24 :
      temp = EwNewStringFloat( _this->Value, 0, 0 );
    break;

    default : 
      temp = EwNewStringFloat( _this->Value, 0, 1 );
  }

  if ( !EwCompString( temp, EwLoadString( &_Const0000 )))
    return EwLoadString( &_Const0001 );
  else
    return temp;
}

/* 'C' function for method : 'Parameter::Parameter.GetValuePozent()' */
XInt32 ParameterParameter_GetValuePozent( ParameterParameter _this )
{
  XFloat temp;
  XFloat area;

  area = _this->Max - _this->Min;

  if ( area == 0.000000f )
    return 0;

  if ( _this->Relative )
  {
    area = _this->Value / area;
    area = ( area * 100 ) - 50;
    temp = area;
  }
  else
  {
    area = _this->Value / area;
    area = area * 100;
    temp = area;
  }

  return (XInt32)(XInt32)temp;
}

/* 'C' function for method : 'Parameter::Parameter.UpdateParameter()' */
void ParameterParameter_UpdateParameter( ParameterParameter _this, XUInt32 aID, 
  XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, 
  XUInt8 atext, XUInt8 atotalparams )
{
  _this->Id = (XUInt8)aID;
  _this->Value = aValue;
  _this->Max = aMax;
  _this->Min = aMin;
  _this->unit_id = aUnit;
  _this->type = atype;
  _this->image = aimage;
  _this->text = atext;
  _this->totalparams = atotalparams;

  if ( _this->unit_id == 24 )
  {
    _this->Value = ( aValue - 1 ) * 10;
    _this->Max = 10.000000f;
    _this->Min = -10.000000f;
  }
  else
    if ( _this->unit_id == 22 )
    {
      _this->Value = ( aValue - 1 ) * 30;
      _this->Max = 30.000000f;
      _this->Min = -30.000000f;
    }
    else
      if ( _this->unit_id == 7 )
      {
        _this->Max = 22.000000f;
        _this->Min = 2.000000f;
      }

  ParameterParameter_CalcAdvanced( _this );
  EwNotifyObjObservers((XObject)_this, 0 );
}

/* Wrapper function for the non virtual method : 'Parameter::Parameter.UpdateParameter()' */
void ParameterParameter__UpdateParameter( void* _this, XUInt32 aID, XFloat aValue, 
  XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, XUInt8 atext, 
  XUInt8 atotalparams )
{
  ParameterParameter_UpdateParameter((ParameterParameter)_this, aID, aValue, aMax
  , aMin, aUnit, atype, aimage, atext, atotalparams );
}

/* 'C' function for method : 'Parameter::Parameter.CalcAdvanced()' */
void ParameterParameter_CalcAdvanced( ParameterParameter _this )
{
  _this->Toggle = !!( _this->type & 16 );
  _this->Relative = !!( _this->type & 8 );
  _this->DecimalPoint = _this->type & 3;
  _this->QAIndicator = !!( _this->type & 128 );
}

/* 'C' function for method : 'Parameter::Parameter.UpdateParameterSim()' */
void ParameterParameter_UpdateParameterSim( ParameterParameter _this, XUInt32 aID, 
  XFloat aValue, XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, 
  XUInt8 atext )
{
  _this->Id = (XUInt8)aID;
  _this->Value = aValue;
  _this->Max = aMax;
  _this->Min = aMin;
  _this->unit_id = aUnit;
  _this->type = atype;
  _this->image = aimage;
  _this->text = atext;
  ParameterParameter_CalcAdvanced( _this );
  EwNotifyObjObservers((XObject)_this, 0 );
}

/* Wrapper function for the non virtual method : 'Parameter::Parameter.UpdateParameterSim()' */
void ParameterParameter__UpdateParameterSim( void* _this, XUInt32 aID, XFloat aValue, 
  XFloat aMax, XFloat aMin, XUInt8 aUnit, XUInt8 atype, XUInt8 aimage, XUInt8 atext )
{
  ParameterParameter_UpdateParameterSim((ParameterParameter)_this, aID, aValue, 
  aMax, aMin, aUnit, atype, aimage, atext );
}

/* Variants derived from the class : 'Parameter::Parameter' */
EW_DEFINE_CLASS_VARIANTS( ParameterParameter )
EW_END_OF_CLASS_VARIANTS( ParameterParameter )

/* Virtual Method Table (VMT) for the class : 'Parameter::Parameter' */
EW_DEFINE_CLASS( ParameterParameter, XObject, _.VMT, _.VMT, _.VMT, _.VMT, _.VMT, 
                 _.VMT, "Parameter::Parameter" )
EW_END_OF_CLASS( ParameterParameter )

/* User defined auto object: 'Parameter::ActiveParameter' */
EW_DEFINE_AUTOOBJECT( ParameterActiveParameter, ParameterParameter )

/* Initializer for the auto object 'Parameter::ActiveParameter' */
void ParameterActiveParameter__Init( ParameterParameter _this )
{
  _this->unit_id = 0;
  _this->type = 0;
  _this->image = 1;
  _this->text = 2;
}

/* Table with links to derived variants of the auto object : 'Parameter::ActiveParameter' */
EW_DEFINE_AUTOOBJECT_VARIANTS( ParameterActiveParameter )
EW_END_OF_AUTOOBJECT_VARIANTS( ParameterActiveParameter )

/* Initializer for the class 'Parameter::DeviceClass' */
void ParameterDeviceClass__Init( ParameterDeviceClass _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  TemplatesDeviceClass__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ParameterDeviceClass );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ParameterDeviceClass );

  /* ... and initialize objects, variables, properties, etc. */
  _this->active_par = EwGetAutoObject( &ParameterActiveParameter, ParameterParameter );

  /* Call the user defined constructor */
  ParameterDeviceClass_Init( _this, aArg );
}

/* Re-Initializer for the class 'Parameter::DeviceClass' */
void ParameterDeviceClass__ReInit( ParameterDeviceClass _this )
{
  /* At first re-initialize the super class ... */
  TemplatesDeviceClass__ReInit( &_this->_.Super );
}

/* Finalizer method for the class 'Parameter::DeviceClass' */
void ParameterDeviceClass__Done( ParameterDeviceClass _this )
{
  /* Call the user defined destructor of the class */
  ParameterDeviceClass_Done( _this );

  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( TemplatesDeviceClass );

  /* Don't forget to deinitialize the super class ... */
  TemplatesDeviceClass__Done( &_this->_.Super );
}

/* 'C' function for method : 'Parameter::DeviceClass.Done()' */
void ParameterDeviceClass_Done( ParameterDeviceClass _this )
{
  XObject thisObject = ((XObject)_this );

  {
    /*
       TO DO:

       Depending on your application case you call functions of the underlying
       middleware (or access the device directly) in order to perform the necessary
       de-initialization steps. For example, you invoke some 'C' function:

         YourDevice_DeInitialize();

       IMPORTANT:
       ----------

       The variable 'thisObject' represents the actually de-initialized instance of the
       Parameter::DeviceClass. If you have stored this object at the initialization
       time (in the 'Init' method) in some global C variable or registered it by the
       middleware, it is important to perform now the opposite operation. Set the
       global variable to NULL or de-register 'thisObject' object from the middleware.

    */
  }
}

/* 'C' function for method : 'Parameter::DeviceClass.Init()' */
void ParameterDeviceClass_Init( ParameterDeviceClass _this, XHandle aArg )
{
  XObject thisObject;

  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  thisObject = ((XObject)_this );
  {
    param_init(thisObject);
      /*
         TO DO:

         Depending on your application case you call functions of the underlying
         middleware (or access the device directly) in order to perform the necessary
         initialization steps. For example, you invoke some 'C' function:

           YourDevice_Initialize();

         The variable 'thisObject' represents the actually initialized instance of the
         Parameter::DeviceClass. You can store this variable e.g. in the middleware
         and use it whenever the middleware needs to notify the GUI application about
         some state alternation or events. In this manner, the middleware will be able
         to invoke methods of the interface device object.

         For example, you can store 'thisObject' in some global C variable:

           // Declaration of the global C variable
           XObject theDeviceObject;

           // Store the instance in the global variable
           theDeviceObject = thisObject;

         Later use the global variable e.g. to provide the GUI application with events:

           ApplicationDeviceClass__TriggerSomeEvent( theDeviceObject );

         IMPORTANT:
         ----------

         If you store 'thisObject' for later use, don't forget to implement the opposite
         operation in the method 'Done'. Concrete, 'Done' should set the global variable
         again to the value NULL.

      */
  }
}

/* 'C' function for method : 'Parameter::DeviceClass.update_par_from_dev()' */
void ParameterDeviceClass_update_par_from_dev( ParameterDeviceClass _this, XUInt8 
  aNewID, XFloat aNewValue, XFloat aNewMax, XFloat aNewMin, XUInt8 aNewUnitID, XUInt8 
  aNewType, XUInt8 aNewImage, XUInt8 aNewText, XUInt8 aNewTotalParams )
{
  ParameterParameter_UpdateParameter( _this->active_par, aNewID, aNewValue, aNewMax, 
  aNewMin, aNewUnitID, aNewType, aNewImage, aNewText, aNewTotalParams );
}

/* Wrapper function for the non virtual method : 'Parameter::DeviceClass.update_par_from_dev()' */
void ParameterDeviceClass__update_par_from_dev( void* _this, XUInt8 aNewID, XFloat 
  aNewValue, XFloat aNewMax, XFloat aNewMin, XUInt8 aNewUnitID, XUInt8 aNewType, 
  XUInt8 aNewImage, XUInt8 aNewText, XUInt8 aNewTotalParams )
{
  ParameterDeviceClass_update_par_from_dev((ParameterDeviceClass)_this, aNewID, 
  aNewValue, aNewMax, aNewMin, aNewUnitID, aNewType, aNewImage, aNewText, aNewTotalParams );
}

/* Variants derived from the class : 'Parameter::DeviceClass' */
EW_DEFINE_CLASS_VARIANTS( ParameterDeviceClass )
EW_END_OF_CLASS_VARIANTS( ParameterDeviceClass )

/* Virtual Method Table (VMT) for the class : 'Parameter::DeviceClass' */
EW_DEFINE_CLASS( ParameterDeviceClass, TemplatesDeviceClass, active_par, _.VMT, 
                 _.VMT, _.VMT, _.VMT, _.VMT, "Parameter::DeviceClass" )
EW_END_OF_CLASS( ParameterDeviceClass )

/* User defined auto object: 'Parameter::Device' */
EW_DEFINE_AUTOOBJECT( ParameterDevice, ParameterDeviceClass )

/* Initializer for the auto object 'Parameter::Device' */
void ParameterDevice__Init( ParameterDeviceClass _this )
{
  EW_UNUSED_ARG( _this );
}

/* Table with links to derived variants of the auto object : 'Parameter::Device' */
EW_DEFINE_AUTOOBJECT_VARIANTS( ParameterDevice )
EW_END_OF_AUTOOBJECT_VARIANTS( ParameterDevice )

/* Embedded Wizard */

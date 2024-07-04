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
#include "_ApplicationApplication.h"
#include "_ApplicationDeviceClass.h"
#include "_ApplicationParamBase.h"
#include "_ApplicationParamDialog.h"
#include "_ApplicationParamQAIndicator.h"
#include "_ApplicationParamToggle.h"
#include "_ApplicationParamValue.h"
#include "_ApplicationReceiveDialog.h"
#include "_ApplicationScrollbar.h"
#include "_ApplicationSplashScreen.h"
#include "_ApplicationTestDialog.h"
#include "_ApplicationTextLine.h"
#include "_ApplicationValueBar.h"
#include "_CoreKeyPressHandler.h"
#include "_CorePropertyObserver.h"
#include "_CoreTimer.h"
#include "_CoreView.h"
#include "_ParameterDeviceClass.h"
#include "_ParameterParameter.h"
#include "_ResourcesBitmap.h"
#include "_ResourcesFont.h"
#include "_SimulatorSimulatorClass.h"
#include "_ViewsImage.h"
#include "_ViewsLine.h"
#include "_ViewsRectangle.h"
#include "_ViewsText.h"
#include "Application.h"
#include "Core.h"
#include "Data.h"
#include "Parameter.h"
#include "Res.h"
#include "Resources.h"
#include "Simulator.h"
#include "Views.h"

/* Strings for the language 'Default'. */
EW_CONST_STRING_PRAGMA static const unsigned short _StringsDefault0[] =
{
  0xFFFF, 0xFFFF, 0xC557, 0x0043, 0x0041, 0x004E, 0x003A, 0x0020, 0x004F, 0x004B,
  0x0000, 0xC557, 0x0043, 0x0041, 0x004E, 0x003A, 0x0020, 0x0046, 0x0000, 0xC557,
  0x0043, 0x0041, 0x004E, 0x003A, 0x0020, 0x0055, 0x0000, 0xC557, 0x0042, 0x004B,
  0x0043, 0x003A, 0x0020, 0x004F, 0x004B, 0x0000, 0xC557, 0x0042, 0x004B, 0x0043,
  0x003A, 0x0020, 0x0046, 0x0000, 0xC557, 0x0042, 0x004B, 0x0043, 0x003A, 0x0020,
  0x0055, 0x0000, 0xC557, 0x0055, 0x0050, 0x003A, 0x0020, 0x004F, 0x004B, 0x0000,
  0xC557, 0x0055, 0x0050, 0x003A, 0x0020, 0x0046, 0x0000, 0xC557, 0x0055, 0x0050,
  0x003A, 0x0020, 0x0055, 0x0000, 0xC557, 0x0044, 0x004F, 0x0057, 0x004E, 0x003A,
  0x0020, 0x004F, 0x004B, 0x0000, 0xC557, 0x0044, 0x004F, 0x0057, 0x004E, 0x003A,
  0x0020, 0x0046, 0x0000, 0xC557, 0x0044, 0x004F, 0x0057, 0x004E, 0x003A, 0x0020,
  0x0055, 0x0000, 0xC557, 0x004C, 0x0045, 0x0046, 0x0054, 0x003A, 0x0020, 0x004F,
  0x004B, 0x0000, 0xC557, 0x004C, 0x0045, 0x0046, 0x0054, 0x003A, 0x0020, 0x0046,
  0x0000, 0xC557, 0x004C, 0x0045, 0x0046, 0x0054, 0x003A, 0x0020, 0x0055, 0x0000,
  0xC557, 0x0052, 0x0049, 0x0047, 0x0048, 0x0054, 0x003A, 0x0020, 0x004F, 0x004B,
  0x0000, 0xC557, 0x0052, 0x0049, 0x0047, 0x0048, 0x0054, 0x003A, 0x0020, 0x0046,
  0x0000, 0xC557, 0x0052, 0x0049, 0x0047, 0x0048, 0x0054, 0x003A, 0x0020, 0x0055,
  0x0000, 0xC557, 0x0054, 0x0052, 0x0047, 0x003A, 0x0020, 0x004F, 0x004B, 0x0000,
  0xC557, 0x0054, 0x0052, 0x0047, 0x003A, 0x0020, 0x0046, 0x0000, 0xC557, 0x0054,
  0x0052, 0x0047, 0x003A, 0x0020, 0x0055, 0x0000, 0xC557, 0x0049, 0x006E, 0x0066,
  0x006F, 0x0000, 0xC557, 0x00C4, 0x0000, 0xC557, 0x0025, 0x0025, 0x0000, 0xC557,
  0x0044, 0x0079, 0x006E, 0x0000, 0xC557, 0x002B, 0x0030, 0x0000, 0xC557, 0x006D,
  0x002F, 0x006D, 0x0069, 0x006E, 0x0000, 0xC557, 0x0053, 0x0000
};

/* Constant values used in this 'C' module only. */
static const XRect _Const0000 = {{ 0, 0 }, { 96, 64 }};
static const XColor _Const0001 = { 0xFF, 0xFF, 0xFF, 0xFF };
static const XColor _Const0002 = { 0x00, 0x00, 0x00, 0xFF };
static const XRect _Const0003 = {{ 0, 0 }, { 18, 18 }};
static const XColor _Const0004 = { 0x1A, 0xA2, 0x34, 0xFF };
static const XStringRes _Const0005 = { _StringsDefault0, 0x0003 };
static const XColor _Const0006 = { 0xFF, 0x0F, 0x0F, 0xFF };
static const XStringRes _Const0007 = { _StringsDefault0, 0x000C };
static const XColor _Const0008 = { 0xFF, 0x85, 0x25, 0xFF };
static const XStringRes _Const0009 = { _StringsDefault0, 0x0014 };
static const XStringRes _Const000A = { _StringsDefault0, 0x001C };
static const XStringRes _Const000B = { _StringsDefault0, 0x0025 };
static const XStringRes _Const000C = { _StringsDefault0, 0x002D };
static const XStringRes _Const000D = { _StringsDefault0, 0x0035 };
static const XStringRes _Const000E = { _StringsDefault0, 0x003D };
static const XStringRes _Const000F = { _StringsDefault0, 0x0044 };
static const XStringRes _Const0010 = { _StringsDefault0, 0x004B };
static const XStringRes _Const0011 = { _StringsDefault0, 0x0055 };
static const XStringRes _Const0012 = { _StringsDefault0, 0x005E };
static const XStringRes _Const0013 = { _StringsDefault0, 0x0067 };
static const XStringRes _Const0014 = { _StringsDefault0, 0x0071 };
static const XStringRes _Const0015 = { _StringsDefault0, 0x007A };
static const XStringRes _Const0016 = { _StringsDefault0, 0x0083 };
static const XStringRes _Const0017 = { _StringsDefault0, 0x008E };
static const XStringRes _Const0018 = { _StringsDefault0, 0x0098 };
static const XStringRes _Const0019 = { _StringsDefault0, 0x00A2 };
static const XStringRes _Const001A = { _StringsDefault0, 0x00AB };
static const XStringRes _Const001B = { _StringsDefault0, 0x00B3 };
static const XStringRes _Const001C = { _StringsDefault0, 0x00BB };
static const XRect _Const001D = {{ 1, 1 }, { 17, 17 }};
static const XRect _Const001E = {{ 0, 0 }, { 100, 100 }};
static const XRect _Const001F = {{ 8, 4 }, { 35, 11 }};
static const XPoint _Const0020 = { 182, -63 };
static const XPoint _Const0021 = { 182, -163 };
static const XRect _Const0022 = {{ 0, 0 }, { 0, 0 }};
static const XPoint _Const0023 = { 4, 6 };
static const XRect _Const0024 = {{ 368, 48 }, { 384, 64 }};
static const XStringRes _Const0025 = { _StringsDefault0, 0x00C1 };
static const XRect _Const0026 = {{ 107, -11 }, { 195, 4 }};
static const XStringRes _Const0027 = { _StringsDefault0, 0x00C4 };
static const XRect _Const0028 = {{ 90, -46 }, { 180, -31 }};
static const XStringRes _Const0029 = { _StringsDefault0, 0x00C8 };
static const XRect _Const002A = {{ 0, 0 }, { 96, 3 }};
static const XRect _Const002B = {{ 78, 0 }, { 85, 3 }};
static const XColor _Const002C = { 0x80, 0x80, 0x80, 0xFF };
static const XRect _Const002D = {{ 0, 0 }, { 96, 33 }};
static const XStringRes _Const002E = { _StringsDefault0, 0x00CD };
static const XRect _Const002F = {{ 0, 32 }, { 96, 42 }};
static const XRect _Const0030 = {{ 0, 40 }, { 96, 60 }};
static const XRect _Const0031 = {{ 0, 60 }, { 96, 63 }};
static const XStringRes _Const0032 = { _StringsDefault0, 0x00D1 };
static const XStringRes _Const0033 = { _StringsDefault0, 0x00D8 };
static const XRect _Const0034 = {{ 0, 0 }, { 96, 60 }};
static const XRect _Const0035 = {{ 0, 61 }, { 96, 64 }};
static const XRect _Const0036 = {{ -2, 0 }, { 96, 64 }};
static const XColor _Const0037 = { 0xC4, 0x00, 0x00, 0xFF };
static const XRect _Const0038 = {{ 0, 0 }, { 46, 17 }};
static const XRect _Const0039 = {{ 46, 0 }, { 96, 17 }};
static const XRect _Const003A = {{ 0, 17 }, { 47, 32 }};
static const XRect _Const003B = {{ 47, 17 }, { 96, 32 }};
static const XRect _Const003C = {{ 0, 32 }, { 47, 48 }};
static const XRect _Const003D = {{ 47, 32 }, { 96, 48 }};
static const XRect _Const003E = {{ 0, 48 }, { 47, 64 }};
static const XRect _Const003F = {{ 0, 2 }, { 48, 17 }};
static const XRect _Const0040 = {{ 48, 2 }, { 94, 17 }};
static const XRect _Const0041 = {{ 0, 33 }, { 48, 48 }};
static const XRect _Const0042 = {{ 47, 34 }, { 95, 49 }};
static const XRect _Const0043 = {{ 0, 18 }, { 48, 33 }};
static const XRect _Const0044 = {{ 47, 18 }, { 95, 33 }};
static const XRect _Const0045 = {{ 0, 48 }, { 48, 63 }};

/* Initializer for the class 'Application::Application' */
void ApplicationApplication__Init( ApplicationApplication _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreRoot__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationApplication );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Background, &_this->_.XObject, 0 );
  CoreKeyPressHandler__Init( &_this->KeyHandler, &_this->_.XObject, 0 );
  ApplicationReceiveDialog__Init( &_this->ReceiveDialog, &_this->_.XObject, 0 );
  CorePropertyObserver__Init( &_this->ShowQAObserver, &_this->_.XObject, 0 );
  ApplicationParamDialog__Init( &_this->ParamDialog, &_this->_.XObject, 0 );
  ApplicationSplashScreen__Init( &_this->SplashScreen, &_this->_.XObject, 0 );
  CoreTimer__Init( &_this->SpashScreenTimer, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationApplication );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( _this, _Const0000 );
  CoreRectView__OnSetBounds( &_this->Background, _Const0000 );
  ViewsRectangle_OnSetColorBL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorBR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColor( &_this->Background, _Const0002 );
  ViewsRectangle_OnSetAlphaBlended( &_this->Background, 0 );
  CoreRectView__OnSetBounds( &_this->ReceiveDialog, _Const0003 );
  CoreRectView__OnSetBounds( &_this->ParamDialog, _Const0000 );
  CoreRectView__OnSetBounds( &_this->SplashScreen, _Const0000 );
  CoreTimer_OnSetPeriod( &_this->SpashScreenTimer, 1000 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Background ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ReceiveDialog ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ParamDialog ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->SplashScreen ), 0 );
  _this->KeyHandler.OnPress = EwNewSlot( _this, ApplicationApplication_OnKeyPress );
  _this->ShowQAObserver.OnEvent = EwNewSlot( _this, ApplicationApplication_onShowQA );
  CorePropertyObserver_OnSetOutlet( &_this->ShowQAObserver, EwNewRef( EwGetAutoObject( 
  &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetShowQA, 
  ApplicationDeviceClass_OnSetShowQA ));
  _this->Simulator = EwGetAutoObject( &SimulatorSimulator, SimulatorSimulatorClass );
  _this->SpashScreenTimer.OnTrigger = EwNewSlot( _this, ApplicationApplication_RemoveSplash );
  _this->par_device_instance = EwGetAutoObject( &ParameterDevice, ParameterDeviceClass );

  /* Call the user defined constructor */
  ApplicationApplication_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::Application' */
void ApplicationApplication__ReInit( ApplicationApplication _this )
{
  /* At first re-initialize the super class ... */
  CoreRoot__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Background );
  CoreKeyPressHandler__ReInit( &_this->KeyHandler );
  ApplicationReceiveDialog__ReInit( &_this->ReceiveDialog );
  CorePropertyObserver__ReInit( &_this->ShowQAObserver );
  ApplicationParamDialog__ReInit( &_this->ParamDialog );
  ApplicationSplashScreen__ReInit( &_this->SplashScreen );
  CoreTimer__ReInit( &_this->SpashScreenTimer );
}

/* Finalizer method for the class 'Application::Application' */
void ApplicationApplication__Done( ApplicationApplication _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreRoot );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Background );
  CoreKeyPressHandler__Done( &_this->KeyHandler );
  ApplicationReceiveDialog__Done( &_this->ReceiveDialog );
  CorePropertyObserver__Done( &_this->ShowQAObserver );
  ApplicationParamDialog__Done( &_this->ParamDialog );
  ApplicationSplashScreen__Done( &_this->SplashScreen );
  CoreTimer__Done( &_this->SpashScreenTimer );

  /* Don't forget to deinitialize the super class ... */
  CoreRoot__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationApplication_Init( ApplicationApplication _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  ApplicationDeviceClass_GUIInit( EwGetAutoObject( &ApplicationDevice, ApplicationDeviceClass ));
  CoreTimer_OnSetEnabled( &_this->SpashScreenTimer, 1 );
}

/* 'C' function for method : 'Application::Application.OnKeyPress()' */
void ApplicationApplication_OnKeyPress( ApplicationApplication _this, XObject sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( sender );
}

/* This slot method is executed when the associated property observer 'PropertyObserver' 
   is notified. */
void ApplicationApplication_onShowQA( ApplicationApplication _this, XObject sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( sender );

  return;
}

/* 'C' function for method : 'Application::Application.RemoveSplash()' */
void ApplicationApplication_RemoveSplash( ApplicationApplication _this, XObject 
  sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  CoreGroup_OnSetVisible((CoreGroup)&_this->SplashScreen, 0 );
}

/* Variants derived from the class : 'Application::Application' */
EW_DEFINE_CLASS_VARIANTS( ApplicationApplication )
EW_END_OF_CLASS_VARIANTS( ApplicationApplication )

/* Virtual Method Table (VMT) for the class : 'Application::Application' */
EW_DEFINE_CLASS( ApplicationApplication, CoreRoot, Simulator, Background, Background, 
                 Background, _.VMT, _.VMT, "Application::Application" )
  CoreRectView_initLayoutContext,
  CoreRoot_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreRoot_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreRoot_OnSetFocus,
  CoreRoot_DispatchEvent,
  CoreRoot_BroadcastEvent,
  CoreRoot_InvalidateArea,
EW_END_OF_CLASS( ApplicationApplication )

/* Initializer for the class 'Application::ParamDialog' */
void ApplicationParamDialog__Init( ApplicationParamDialog _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationParamDialog );

  /* ... then construct all embedded objects */
  CorePropertyObserver__Init( &_this->ParamCounterObserver, &_this->_.XObject, 0 );
  ApplicationParamToggle__Init( &_this->ParamToggle, &_this->_.XObject, 0 );
  ApplicationTestDialog__Init( &_this->TestDialog, &_this->_.XObject, 0 );
  ApplicationParamValue__Init( &_this->ParamValue, &_this->_.XObject, 0 );
  ApplicationParamQAIndicator__Init( &_this->ParamQA, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationParamDialog );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( _this, _Const0000 );
  CoreRectView__OnSetBounds( &_this->ParamToggle, _Const0000 );
  CoreRectView__OnSetBounds( &_this->TestDialog, _Const0000 );
  CoreRectView__OnSetBounds( &_this->ParamValue, _Const0000 );
  CoreRectView__OnSetBounds( &_this->ParamQA, _Const0000 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ParamToggle ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->TestDialog ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ParamValue ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ParamQA ), 0 );
  _this->ParamCounterObserver.OnEvent = EwNewSlot( _this, ApplicationParamDialog_onParamCounter );
  CorePropertyObserver_OnSetOutlet( &_this->ParamCounterObserver, EwNewRef( EwGetAutoObject( 
  &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetParamCounter, 
  ApplicationDeviceClass_OnSetParamCounter ));
  _this->Param = EwGetAutoObject( &ParameterActiveParameter, ParameterParameter );

  /* Call the user defined constructor */
  ApplicationParamDialog_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ParamDialog' */
void ApplicationParamDialog__ReInit( ApplicationParamDialog _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  CorePropertyObserver__ReInit( &_this->ParamCounterObserver );
  ApplicationParamToggle__ReInit( &_this->ParamToggle );
  ApplicationTestDialog__ReInit( &_this->TestDialog );
  ApplicationParamValue__ReInit( &_this->ParamValue );
  ApplicationParamQAIndicator__ReInit( &_this->ParamQA );
}

/* Finalizer method for the class 'Application::ParamDialog' */
void ApplicationParamDialog__Done( ApplicationParamDialog _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  CorePropertyObserver__Done( &_this->ParamCounterObserver );
  ApplicationParamToggle__Done( &_this->ParamToggle );
  ApplicationTestDialog__Done( &_this->TestDialog );
  ApplicationParamValue__Done( &_this->ParamValue );
  ApplicationParamQAIndicator__Done( &_this->ParamQA );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationParamDialog_Init( ApplicationParamDialog _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  EwAttachObjObserver( EwNewSlot( _this, ApplicationParamDialog_OnUpdate ), (XObject)EwGetAutoObject( 
    &ParameterActiveParameter, ParameterParameter ), 0 );
  EwAttachRefObserver( EwNewSlot( _this, ApplicationParamDialog_OnChangeConfig ), 
    EwNewRef( EwGetAutoObject( &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetNoOfParameter, 
    ApplicationDeviceClass_OnSetNoOfParameter ), 0 );
  EwAttachRefObserver( EwNewSlot( _this, ApplicationParamDialog_OnChangeConfig ), 
    EwNewRef( EwGetAutoObject( &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetCurrentParameter, 
    ApplicationDeviceClass_OnSetCurrentParameter ), 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamToggle, 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamValue, 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamQA, 0 );
  EwPostSignal( EwNewSlot( _this, ApplicationParamDialog_OnChangeConfig ), ((XObject)_this ));
}

/* This slot method is executed when the associated property observer 'PropertyObserver' 
   is notified. */
void ApplicationParamDialog_onParamCounter( ApplicationParamDialog _this, XObject 
  sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( sender );

  return;
}

/* 'C' function for method : 'Application::ParamDialog.OnUpdate()' */
void ApplicationParamDialog_OnUpdate( ApplicationParamDialog _this, XObject sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamToggle, 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamValue, 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->ParamQA, 0 );
  CoreGroup_OnSetVisible((CoreGroup)&_this->TestDialog, 0 );
  ApplicationScrollbar_OnSetMaximum( &_this->ParamQA.Scrollbar, EwGetAutoObject( 
  &ParameterActiveParameter, ParameterParameter )->totalparams );

  if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->Id == 0 )
  {
    CoreGroup_OnSetVisible((CoreGroup)&_this->ParamQA, 1 );
    ViewsImage_OnSetFrameNumber( &_this->ParamQA.Image, 14 );
    ApplicationScrollbar_OnSetSelected( &_this->ParamQA.Scrollbar, _this->ParamValue.Scrollbar.Maximum );
  }
  else
    if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
        >= 100 )
    {
      CoreGroup_OnSetVisible((CoreGroup)&_this->ParamQA, 1 );

      if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
          == 100 )
      {
        if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
            == 1 )
          ViewsImage_OnSetFrameNumber( &_this->ParamQA.Image, 5 );
        else
          ViewsImage_OnSetFrameNumber( &_this->ParamQA.Image, 4 );

        ApplicationScrollbar_OnSetSelected( &_this->ParamQA.Scrollbar, (XUInt32)( 
        EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->Id - 1 ));
      }
      else
        if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
            == 101 )
        {
          if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
              == 1 )
            ViewsImage_OnSetFrameNumber( &_this->ParamQA.Image, 9 );
          else
            ViewsImage_OnSetFrameNumber( &_this->ParamQA.Image, 8 );

          ApplicationScrollbar_OnSetSelected( &_this->ParamQA.Scrollbar, (XUInt32)( 
          EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->Id - 
          1 ));
        }
    }
    else
      if (( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->Id 
          == 84 ) && ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->unit_id 
          == 67 ))
      {
        CoreGroup_OnSetVisible((CoreGroup)&_this->TestDialog, 1 );

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
            & 1 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_CAN, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_CAN, EwLoadString( &_Const0005 ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
              & 2 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_CAN, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_CAN, EwLoadString( &_Const0007 ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_CAN, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_CAN, EwLoadString( &_Const0009 ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
            & 4 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_BKC, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_BKC, EwLoadString( &_Const000A ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
              & 8 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_BKC, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_BKC, EwLoadString( &_Const000B ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_BKC, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_BKC, EwLoadString( &_Const000C ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
            & 16 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_UP, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_Up, EwLoadString( &_Const000D ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
              & 32 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_UP, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Up, EwLoadString( &_Const000E ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_UP, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Up, EwLoadString( &_Const000F ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
            & 64 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_DOWN, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_Down, EwLoadString( &_Const0010 ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->type 
              & 128 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_DOWN, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Down, EwLoadString( &_Const0011 ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_DOWN, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Down, EwLoadString( &_Const0012 ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
            & 1 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_LEFT, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_Left, EwLoadString( &_Const0013 ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
              & 2 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_LEFT, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Left, EwLoadString( &_Const0014 ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_LEFT, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Left, EwLoadString( &_Const0015 ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
            & 4 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_RIGHT, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_Right, EwLoadString( &_Const0016 ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
              & 8 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_RIGHT, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Right, EwLoadString( 
            &_Const0017 ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_RIGHT, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Right, EwLoadString( 
            &_Const0018 ));
          }

        if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
            & 16 ))
        {
          ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_TRG, _Const0004 );
          ViewsText_OnSetString( &_this->TestDialog.Text_Trigger, EwLoadString( 
          &_Const0019 ));
        }
        else
          if ( !!( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->image 
              & 32 ))
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_TRG, _Const0006 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Trigger, EwLoadString( 
            &_Const001A ));
          }
          else
          {
            ViewsRectangle_OnSetColor( &_this->TestDialog.Rectangle_TRG, _Const0008 );
            ViewsText_OnSetString( &_this->TestDialog.Text_Trigger, EwLoadString( 
            &_Const001B ));
          }
      }
      else
        CoreGroup_OnSetVisible((CoreGroup)&_this->ParamValue, 1 );

  return;
}

/* 'C' function for method : 'Application::ParamDialog.OnChangeConfig()' */
void ApplicationParamDialog_OnChangeConfig( ApplicationParamDialog _this, XObject 
  sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  ViewsText_OnSetString( &_this->TestDialog.Text_BKC, EwLoadString( &_Const001C ));
}

/* Variants derived from the class : 'Application::ParamDialog' */
EW_DEFINE_CLASS_VARIANTS( ApplicationParamDialog )
EW_END_OF_CLASS_VARIANTS( ApplicationParamDialog )

/* Virtual Method Table (VMT) for the class : 'Application::ParamDialog' */
EW_DEFINE_CLASS( ApplicationParamDialog, CoreGroup, Param, ParamCounterObserver, 
                 ParamCounterObserver, ParamCounterObserver, _.VMT, _.VMT, "Application::ParamDialog" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationParamDialog )

/* User defined inline code: 'Application::Inline' */
#include "string.h"
#include "gui.h"

/* Initializer for the class 'Application::DeviceClass' */
void ApplicationDeviceClass__Init( ApplicationDeviceClass _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  TemplatesDeviceClass__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationDeviceClass );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationDeviceClass );

  /* ... and initialize objects, variables, properties, etc. */
}

/* Re-Initializer for the class 'Application::DeviceClass' */
void ApplicationDeviceClass__ReInit( ApplicationDeviceClass _this )
{
  /* At first re-initialize the super class ... */
  TemplatesDeviceClass__ReInit( &_this->_.Super );
}

/* Finalizer method for the class 'Application::DeviceClass' */
void ApplicationDeviceClass__Done( ApplicationDeviceClass _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( TemplatesDeviceClass );

  /* Don't forget to deinitialize the super class ... */
  TemplatesDeviceClass__Done( &_this->_.Super );
}

/* 'C' function for method : 'Application::DeviceClass.GUIInit()' */
void ApplicationDeviceClass_GUIInit( ApplicationDeviceClass _this )
{
  {
    #define this _this

    gui_init(this);

    #undef this
  }
}

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateReceiveCounter( ApplicationDeviceClass _this, 
  XUInt32 aNewValue )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( aNewValue );

  return;
}

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateReceiveCounter()' */
void ApplicationDeviceClass__UpdateReceiveCounter( void* _this, XUInt32 aNewValue )
{
  ApplicationDeviceClass_UpdateReceiveCounter((ApplicationDeviceClass)_this, aNewValue );
}

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateParamCounter( ApplicationDeviceClass _this, XUInt32 
  aNewValue )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( aNewValue );

  return;
}

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateParamCounter()' */
void ApplicationDeviceClass__UpdateParamCounter( void* _this, XUInt32 aNewValue )
{
  ApplicationDeviceClass_UpdateParamCounter((ApplicationDeviceClass)_this, aNewValue );
}

/* This method is intended to be called by the device to notify the GUI application 
   about an alternation of its setting or state value. */
void ApplicationDeviceClass_UpdateQACounter( ApplicationDeviceClass _this, XUInt32 
  aNewValue )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( _this );
  EW_UNUSED_ARG( aNewValue );

  return;
}

/* Wrapper function for the non virtual method : 'Application::DeviceClass.UpdateQACounter()' */
void ApplicationDeviceClass__UpdateQACounter( void* _this, XUInt32 aNewValue )
{
  ApplicationDeviceClass_UpdateQACounter((ApplicationDeviceClass)_this, aNewValue );
}

/* 'C' function for method : 'Application::DeviceClass.OnSetNoOfParameter()' */
void ApplicationDeviceClass_OnSetNoOfParameter( ApplicationDeviceClass _this, XInt32 
  value )
{
  if ( _this->NoOfParameter == value )
    return;

  _this->NoOfParameter = value;
}

/* 'C' function for method : 'Application::DeviceClass.UpdateNoOfParameter()' */
void ApplicationDeviceClass_UpdateNoOfParameter( ApplicationDeviceClass _this, XInt32 
  value )
{
  _this->NoOfParameter = value;
  EwNotifyRefObservers( EwNewRef( _this, ApplicationDeviceClass_OnGetNoOfParameter, 
    ApplicationDeviceClass_OnSetNoOfParameter ), 0 );
}

/* 'C' function for method : 'Application::DeviceClass.OnSetCurrentParameter()' */
void ApplicationDeviceClass_OnSetCurrentParameter( ApplicationDeviceClass _this, 
  XInt32 value )
{
  if ( _this->CurrentParameter == value )
    return;

  _this->CurrentParameter = value;
}

/* 'C' function for method : 'Application::DeviceClass.UpdateCurrentParameter()' */
void ApplicationDeviceClass_UpdateCurrentParameter( ApplicationDeviceClass _this, 
  XInt32 value )
{
  _this->CurrentParameter = value;
  EwNotifyRefObservers( EwNewRef( _this, ApplicationDeviceClass_OnGetCurrentParameter, 
    ApplicationDeviceClass_OnSetCurrentParameter ), 0 );
}

/* Default onget method for the property 'ReceiveCounter' */
XUInt32 ApplicationDeviceClass_OnGetReceiveCounter( ApplicationDeviceClass _this )
{
  return _this->ReceiveCounter;
}

/* Default onset method for the property 'ReceiveCounter' */
void ApplicationDeviceClass_OnSetReceiveCounter( ApplicationDeviceClass _this, XUInt32 
  value )
{
  _this->ReceiveCounter = value;
}

/* Default onget method for the property 'ParamCounter' */
XUInt32 ApplicationDeviceClass_OnGetParamCounter( ApplicationDeviceClass _this )
{
  return _this->ParamCounter;
}

/* Default onset method for the property 'ParamCounter' */
void ApplicationDeviceClass_OnSetParamCounter( ApplicationDeviceClass _this, XUInt32 
  value )
{
  _this->ParamCounter = value;
}

/* Default onget method for the property 'ShowQA' */
XBool ApplicationDeviceClass_OnGetShowQA( ApplicationDeviceClass _this )
{
  return _this->ShowQA;
}

/* Default onset method for the property 'ShowQA' */
void ApplicationDeviceClass_OnSetShowQA( ApplicationDeviceClass _this, XBool value )
{
  _this->ShowQA = value;
}

/* Default onget method for the property 'NoOfParameter' */
XInt32 ApplicationDeviceClass_OnGetNoOfParameter( ApplicationDeviceClass _this )
{
  return _this->NoOfParameter;
}

/* Wrapper function for the non virtual method : 'Application::DeviceClass.OnGetNoOfParameter()' */
XInt32 ApplicationDeviceClass__OnGetNoOfParameter( void* _this )
{
  return ApplicationDeviceClass_OnGetNoOfParameter((ApplicationDeviceClass)_this );
}

/* Default onget method for the property 'CurrentParameter' */
XInt32 ApplicationDeviceClass_OnGetCurrentParameter( ApplicationDeviceClass _this )
{
  return _this->CurrentParameter;
}

/* Wrapper function for the non virtual method : 'Application::DeviceClass.OnGetCurrentParameter()' */
XInt32 ApplicationDeviceClass__OnGetCurrentParameter( void* _this )
{
  return ApplicationDeviceClass_OnGetCurrentParameter((ApplicationDeviceClass)_this );
}

/* Variants derived from the class : 'Application::DeviceClass' */
EW_DEFINE_CLASS_VARIANTS( ApplicationDeviceClass )
EW_END_OF_CLASS_VARIANTS( ApplicationDeviceClass )

/* Virtual Method Table (VMT) for the class : 'Application::DeviceClass' */
EW_DEFINE_CLASS( ApplicationDeviceClass, TemplatesDeviceClass, _.VMT, _.VMT, _.VMT, 
                 _.VMT, _.VMT, _.VMT, "Application::DeviceClass" )
EW_END_OF_CLASS( ApplicationDeviceClass )

/* User defined auto object: 'Application::Device' */
EW_DEFINE_AUTOOBJECT( ApplicationDevice, ApplicationDeviceClass )

/* Initializer for the auto object 'Application::Device' */
void ApplicationDevice__Init( ApplicationDeviceClass _this )
{
  EW_UNUSED_ARG( _this );
}

/* Table with links to derived variants of the auto object : 'Application::Device' */
EW_DEFINE_AUTOOBJECT_VARIANTS( ApplicationDevice )
EW_END_OF_AUTOOBJECT_VARIANTS( ApplicationDevice )

/* Initializer for the class 'Application::ReceiveDialog' */
void ApplicationReceiveDialog__Init( ApplicationReceiveDialog _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationReceiveDialog );

  /* ... then construct all embedded objects */
  ViewsImage__Init( &_this->Image, &_this->_.XObject, 0 );
  CorePropertyObserver__Init( &_this->ReceiveCounterObserver, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationReceiveDialog );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( _this, _Const0000 );
  CoreRectView__OnSetBounds( &_this->Image, _Const001D );
  ViewsImage_OnSetColor( &_this->Image, _Const0001 );
  ViewsImage_OnSetFrameNumber( &_this->Image, 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Image ), 0 );
  ViewsImage_OnSetBitmap( &_this->Image, EwLoadResource( &ApplicationIconLoad, ResourcesBitmap ));
  _this->ReceiveCounterObserver.OnEvent = EwNewSlot( _this, ApplicationReceiveDialog_onReceiveCounter );
  CorePropertyObserver_OnSetOutlet( &_this->ReceiveCounterObserver, EwNewRef( EwGetAutoObject( 
  &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetReceiveCounter, 
  ApplicationDeviceClass_OnSetReceiveCounter ));
}

/* Re-Initializer for the class 'Application::ReceiveDialog' */
void ApplicationReceiveDialog__ReInit( ApplicationReceiveDialog _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsImage__ReInit( &_this->Image );
  CorePropertyObserver__ReInit( &_this->ReceiveCounterObserver );
}

/* Finalizer method for the class 'Application::ReceiveDialog' */
void ApplicationReceiveDialog__Done( ApplicationReceiveDialog _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsImage__Done( &_this->Image );
  CorePropertyObserver__Done( &_this->ReceiveCounterObserver );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* This slot method is executed when the associated property observer 'PropertyObserver' 
   is notified. */
void ApplicationReceiveDialog_onReceiveCounter( ApplicationReceiveDialog _this, 
  XObject sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  ViewsImage_OnSetFrameNumber( &_this->Image, ( _this->Image.FrameNumber + 1 ) % 
  4 );
}

/* Variants derived from the class : 'Application::ReceiveDialog' */
EW_DEFINE_CLASS_VARIANTS( ApplicationReceiveDialog )
EW_END_OF_CLASS_VARIANTS( ApplicationReceiveDialog )

/* Virtual Method Table (VMT) for the class : 'Application::ReceiveDialog' */
EW_DEFINE_CLASS( ApplicationReceiveDialog, CoreGroup, Image, Image, Image, Image, 
                 _.VMT, _.VMT, "Application::ReceiveDialog" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationReceiveDialog )

/* Include a file containing the bitmap resource : 'Application::IconLoad' */
#include "_ApplicationIconLoad.h"

/* Table with links to derived variants of the bitmap resource : 'Application::IconLoad' */
EW_RES_WITHOUT_VARIANTS( ApplicationIconLoad )

/* Initializer for the class 'Application::ValueBar' */
void ApplicationValueBar__Init( ApplicationValueBar _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationValueBar );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->EmtyBarPos, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->EmtyBarNeg, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->FilledBar, &_this->_.XObject, 0 );
  ViewsLine__Init( &_this->Line, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->CenterRect, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationValueBar );

  /* ... and initialize objects, variables, properties, etc. */
  _this->ValueProz = -50;
  _this->Type = DataValueBarTypePositiveNegative;
  _this->ColorEmpty = ResColor_InactiveGrey;
  _this->ColorFill = ResColor_RED;
  CoreRectView__OnSetBounds( &_this->EmtyBarPos, _Const001E );
  CoreRectView__OnSetBounds( &_this->EmtyBarNeg, _Const001F );
  CoreRectView__OnSetBounds( &_this->FilledBar, _Const001E );
  CoreLineView_OnSetPoint2((CoreLineView)&_this->Line, _Const0020 );
  CoreLineView_OnSetPoint1((CoreLineView)&_this->Line, _Const0021 );
  ViewsLine_OnSetWidth( &_this->Line, 2 );
  CoreRectView__OnSetBounds( &_this->CenterRect, _Const0022 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->EmtyBarPos ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->EmtyBarNeg ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->FilledBar ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Line ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->CenterRect ), 0 );

  /* Call the user defined constructor */
  ApplicationValueBar_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ValueBar' */
void ApplicationValueBar__ReInit( ApplicationValueBar _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->EmtyBarPos );
  ViewsRectangle__ReInit( &_this->EmtyBarNeg );
  ViewsRectangle__ReInit( &_this->FilledBar );
  ViewsLine__ReInit( &_this->Line );
  ViewsRectangle__ReInit( &_this->CenterRect );
}

/* Finalizer method for the class 'Application::ValueBar' */
void ApplicationValueBar__Done( ApplicationValueBar _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->EmtyBarPos );
  ViewsRectangle__Done( &_this->EmtyBarNeg );
  ViewsRectangle__Done( &_this->FilledBar );
  ViewsLine__Done( &_this->Line );
  ViewsRectangle__Done( &_this->CenterRect );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationValueBar_Init( ApplicationValueBar _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  ApplicationValueBar_DrawBars( _this );
  ApplicationValueBar_DrawCenter( _this );
  ApplicationValueBar_Animate( _this );
  ApplicationValueBar_DrawHelpline( _this );
}

/* 'C' function for method : 'Application::ValueBar.OnSetValueProz()' */
void ApplicationValueBar_OnSetValueProz( ApplicationValueBar _this, XInt32 value )
{
  if ( _this->ValueProz == value )
    return;

  _this->ValueProz = value;
  ApplicationValueBar_Animate( _this );
}

/* 'C' function for method : 'Application::ValueBar.OnSetType()' */
void ApplicationValueBar_OnSetType( ApplicationValueBar _this, XEnum value )
{
  if ( _this->Type == value )
    return;

  _this->Type = value;
  ApplicationValueBar_DrawBars( _this );
  ApplicationValueBar_Animate( _this );
}

/* 'C' function for method : 'Application::ValueBar.Animate()' */
void ApplicationValueBar_Animate( ApplicationValueBar _this )
{
  XFloat temp;

  temp = (XFloat)EwGetInt32Abs( _this->ValueProz ) / 100.000000f;
  temp = temp * EwGetRectW( _this->EmtyBarPos.Super1.Bounds );
  ApplicationValueBar_DrawCenter( _this );

  if ( _this->Type == DataValueBarTypePositive )
    CoreRectView__OnSetBounds( &_this->FilledBar, EwSetRectW( _this->FilledBar.Super1.Bounds, 
    (XInt32)temp ));
  else
    if ( _this->ValueProz > 0 )
    {
      CoreRectView__OnSetBounds( &_this->FilledBar, _this->EmtyBarPos.Super1.Bounds );
      CoreRectView__OnSetBounds( &_this->FilledBar, EwSetRectW( _this->FilledBar.Super1.Bounds, 
      (XInt32)( temp * 2 )));
    }
    else
    {
      CoreRectView__OnSetBounds( &_this->FilledBar, _this->EmtyBarNeg.Super1.Bounds );
      CoreRectView__OnSetBounds( &_this->FilledBar, EwSetRectX1( _this->FilledBar.Super1.Bounds, 
      (XInt32)( _this->FilledBar.Super1.Bounds.Point2.X - ( temp * 2 ))));
    }
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationValueBar_DrawBars( ApplicationValueBar _this )
{
  CoreRectView__OnSetBounds( _this, EwSetRectW( _this->Super2.Bounds, 96 ));
  CoreRectView__OnSetBounds( _this, EwSetRectH( _this->Super2.Bounds, 10 ));

  if ( _this->Type == DataValueBarTypePositive )
  {
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectX( _this->EmtyBarPos.Super1.Bounds, 
    3 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectW( _this->EmtyBarPos.Super1.Bounds, 
    90 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectH( _this->EmtyBarPos.Super1.Bounds, 
    2 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectY( _this->EmtyBarPos.Super1.Bounds, 
    5 ));
    CoreRectView__OnSetBounds( &_this->FilledBar, _this->EmtyBarPos.Super1.Bounds );
    ViewsRectangle_OnSetVisible( &_this->EmtyBarNeg, 0 );
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectH( _this->EmtyBarNeg.Super1.Bounds, 
    0 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectW( _this->EmtyBarNeg.Super1.Bounds, 
    0 ));
  }
  else
  {
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectX( _this->EmtyBarPos.Super1.Bounds, 
    ( EwGetRectW( _this->Super2.Bounds ) / 2 ) + 3 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectX2( _this->EmtyBarPos.Super1.Bounds, 
    EwGetRectW( _this->Super2.Bounds ) - 3 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectX( _this->EmtyBarNeg.Super1.Bounds, 
    3 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectX2( _this->EmtyBarNeg.Super1.Bounds, 
    ( EwGetRectW( _this->Super2.Bounds ) / 2 ) - 3 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectY( _this->EmtyBarPos.Super1.Bounds, 
    5 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectY( _this->EmtyBarNeg.Super1.Bounds, 
    5 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarPos, EwSetRectH( _this->EmtyBarPos.Super1.Bounds, 
    2 ));
    CoreRectView__OnSetBounds( &_this->EmtyBarNeg, EwSetRectH( _this->EmtyBarNeg.Super1.Bounds, 
    2 ));
    CoreRectView__OnSetBounds( &_this->FilledBar, _this->EmtyBarPos.Super1.Bounds );
    ViewsRectangle_OnSetVisible( &_this->EmtyBarNeg, 1 );
  }

  ViewsRectangle_OnSetColor( &_this->FilledBar, _this->ColorFill );
  ViewsRectangle_OnSetColor( &_this->EmtyBarPos, _this->ColorEmpty );
  ViewsRectangle_OnSetColor( &_this->EmtyBarNeg, _this->ColorEmpty );
}

/* 'C' function for method : 'Application::ValueBar.DrawHelpline()' */
void ApplicationValueBar_DrawHelpline( ApplicationValueBar _this )
{
  ViewsLine_OnSetVisible( &_this->Line, 0 );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationValueBar_DrawCenter( ApplicationValueBar _this )
{
  if ( _this->Type == DataValueBarTypePositive )
    ViewsRectangle_OnSetVisible( &_this->CenterRect, 0 );
  else
  {
    ViewsRectangle_OnSetVisible( &_this->CenterRect, 1 );
    CoreRectView__OnSetBounds( &_this->CenterRect, EwSetRectSize( _this->CenterRect.Super1.Bounds, 
    _Const0023 ));
    CoreRectView__OnSetBounds( &_this->CenterRect, EwSetRectX( _this->CenterRect.Super1.Bounds, 
    ( EwGetRectW( _this->Super2.Bounds ) / 2 ) - 2 ));
    CoreRectView__OnSetBounds( &_this->CenterRect, EwSetRectY( _this->CenterRect.Super1.Bounds, 
    3 ));

    if ( _this->ValueProz == 0 )
      ViewsRectangle_OnSetColor( &_this->CenterRect, _this->ColorFill );
    else
      ViewsRectangle_OnSetColor( &_this->CenterRect, _this->ColorEmpty );
  }
}

/* Variants derived from the class : 'Application::ValueBar' */
EW_DEFINE_CLASS_VARIANTS( ApplicationValueBar )
EW_END_OF_CLASS_VARIANTS( ApplicationValueBar )

/* Virtual Method Table (VMT) for the class : 'Application::ValueBar' */
EW_DEFINE_CLASS( ApplicationValueBar, CoreGroup, EmtyBarPos, EmtyBarPos, EmtyBarPos, 
                 EmtyBarPos, ValueProz, ValueProz, "Application::ValueBar" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationValueBar )

/* Initializer for the class 'Application::TextLine' */
void ApplicationTextLine__Init( ApplicationTextLine _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationTextLine );

  /* ... then construct all embedded objects */
  ViewsImage__Init( &_this->ParamImage, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->UnitText, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->InfoText, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationTextLine );

  /* ... and initialize objects, variables, properties, etc. */
  _this->Type = DataTextLineTypeIcon_Unit;
  CoreRectView__OnSetBounds( &_this->ParamImage, _Const0024 );
  _this->IconId = 1;
  _this->Unit_Text = EwShareString( EwLoadString( &_Const0025 ));
  CoreView_OnSetLayout((CoreView)&_this->UnitText, CoreLayoutAlignToRight | CoreLayoutAlignToTop );
  CoreRectView__OnSetBounds( &_this->UnitText, _Const0026 );
  ViewsText_OnSetAlignment( &_this->UnitText, ViewsTextAlignmentAlignHorzRight | 
  ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->UnitText, EwLoadString( &_Const0027 ));
  CoreView_OnSetLayout((CoreView)&_this->InfoText, CoreLayoutAlignToLeft | CoreLayoutAlignToTop );
  CoreRectView__OnSetBounds( &_this->InfoText, _Const0028 );
  ViewsText_OnSetAlignment( &_this->InfoText, ViewsTextAlignmentAlignHorzCenter 
  | ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->InfoText, EwLoadString( &_Const0029 ));
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ParamImage ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->UnitText ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->InfoText ), 0 );
  ViewsImage_OnSetBitmap( &_this->ParamImage, EwLoadResource( &ResIcon_Parameter_16x16_V3, 
  ResourcesBitmap ));
  ViewsText_OnSetFont( &_this->UnitText, EwLoadResource( &ResFont_2, ResourcesFont ));
  ViewsText_OnSetFont( &_this->InfoText, EwLoadResource( &ResFont_2, ResourcesFont ));

  /* Call the user defined constructor */
  ApplicationTextLine_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::TextLine' */
void ApplicationTextLine__ReInit( ApplicationTextLine _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsImage__ReInit( &_this->ParamImage );
  ViewsText__ReInit( &_this->UnitText );
  ViewsText__ReInit( &_this->InfoText );
}

/* Finalizer method for the class 'Application::TextLine' */
void ApplicationTextLine__Done( ApplicationTextLine _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsImage__Done( &_this->ParamImage );
  ViewsText__Done( &_this->UnitText );
  ViewsText__Done( &_this->InfoText );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTextLine_Init( ApplicationTextLine _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, EwSetRectW( _this->Super2.Bounds, 90 ));
  CoreRectView__OnSetBounds( _this, EwSetRectH( _this->Super2.Bounds, 25 ));
  CoreRectView__OnSetBounds( &_this->ParamImage, EwSetRectX( _this->ParamImage.Super1.Bounds, 
  14 ));
  CoreRectView__OnSetBounds( &_this->ParamImage, EwSetRectY( _this->ParamImage.Super1.Bounds, 
  EwGetRectH( _this->Super2.Bounds ) - 22 ));
  ViewsImage_OnSetFrameNumber( &_this->ParamImage, (XInt32)_this->IconId );
  CoreRectView__OnSetBounds( &_this->UnitText, EwSetRectH( _this->UnitText.Super1.Bounds, 
  _this->UnitText.Font->Ascent + _this->UnitText.Font->Descent ));
  CoreRectView__OnSetBounds( &_this->UnitText, EwSetRectY( _this->UnitText.Super1.Bounds, 
  ( EwGetRectH( _this->Super2.Bounds ) - 9 ) - _this->UnitText.Font->Ascent ));
  CoreRectView__OnSetBounds( &_this->UnitText, EwSetRectX( _this->UnitText.Super1.Bounds, 
  ( EwGetRectW( _this->Super2.Bounds ) - EwGetRectW( _this->UnitText.Super1.Bounds )) 
  - 5 ));
  CoreRectView__OnSetBounds( &_this->UnitText, EwSetRectY( _this->UnitText.Super1.Bounds, 
  ( EwGetRectH( _this->Super2.Bounds ) - 9 ) - _this->UnitText.Font->Ascent ));
  CoreRectView__OnSetBounds( &_this->InfoText, _this->Super2.Bounds );
  CoreRectView__OnSetBounds( &_this->InfoText, EwSetRectH( _this->InfoText.Super1.Bounds, 
  _this->InfoText.Font->Ascent + _this->InfoText.Font->Descent ));
  CoreRectView__OnSetBounds( &_this->InfoText, EwSetRectY( _this->InfoText.Super1.Bounds, 
  ( EwGetRectH( _this->Super2.Bounds ) - 9 ) - _this->InfoText.Font->Ascent ));
  ApplicationTextLine_Animate( _this );
}

/* 'C' function for method : 'Application::TextLine.OnSetType()' */
void ApplicationTextLine_OnSetType( ApplicationTextLine _this, XEnum value )
{
  if ( _this->Type == value )
    return;

  _this->Type = value;
  ApplicationTextLine_Animate( _this );
}

/* 'C' function for method : 'Application::TextLine.OnSetIconId()' */
void ApplicationTextLine_OnSetIconId( ApplicationTextLine _this, XUInt32 value )
{
  if ( _this->IconId == value )
    return;

  _this->IconId = value;
  ViewsImage_OnSetFrameNumber( &_this->ParamImage, (XInt32)_this->IconId );
  ApplicationTextLine_Animate( _this );
}

/* 'C' function for method : 'Application::TextLine.OnSetUnit_Text()' */
void ApplicationTextLine_OnSetUnit_Text( ApplicationTextLine _this, XString value )
{
  if ( !EwCompString( _this->Unit_Text, value ))
    return;

  _this->Unit_Text = EwShareString( value );
  ApplicationTextLine_Animate( _this );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTextLine_Animate( ApplicationTextLine _this )
{
  if ( _this->IconId == 0 )
  {
    ViewsText_OnSetVisible( &_this->InfoText, 1 );
    ViewsText_OnSetVisible( &_this->UnitText, 0 );
    ViewsImage_OnSetVisible( &_this->ParamImage, 0 );
    ViewsText_OnSetString( &_this->InfoText, _this->Unit_Text );
  }
  else
  {
    ViewsText_OnSetVisible( &_this->InfoText, 0 );
    ViewsText_OnSetVisible( &_this->UnitText, 1 );
    ViewsImage_OnSetVisible( &_this->ParamImage, 1 );
    ViewsText_OnSetString( &_this->UnitText, _this->Unit_Text );
  }

  ViewsText_OnSetString( &_this->UnitText, _this->Unit_Text );
}

/* Variants derived from the class : 'Application::TextLine' */
EW_DEFINE_CLASS_VARIANTS( ApplicationTextLine )
EW_END_OF_CLASS_VARIANTS( ApplicationTextLine )

/* Virtual Method Table (VMT) for the class : 'Application::TextLine' */
EW_DEFINE_CLASS( ApplicationTextLine, CoreGroup, ParamImage, ParamImage, ParamImage, 
                 ParamImage, Unit_Text, IconId, "Application::TextLine" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationTextLine )

/* Initializer for the class 'Application::Scrollbar' */
void ApplicationScrollbar__Init( ApplicationScrollbar _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationScrollbar );

  /* ... then construct all embedded objects */
  ViewsImage__Init( &_this->Line, &_this->_.XObject, 0 );
  ViewsImage__Init( &_this->Dot, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationScrollbar );

  /* ... and initialize objects, variables, properties, etc. */
  _this->Selected = 10;
  _this->Maximum = 10;
  CoreRectView__OnSetBounds( &_this->Line, _Const002A );
  CoreRectView__OnSetBounds( &_this->Dot, _Const002B );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Line ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Dot ), 0 );
  ViewsImage_OnSetBitmap( &_this->Line, EwLoadResource( &ResScrollbar_Line_96x1px, 
  ResourcesBitmap ));
  ViewsImage_OnSetBitmap( &_this->Dot, EwLoadResource( &ResScrollbar_Dot_18x3px, 
  ResourcesBitmap ));

  /* Call the user defined constructor */
  ApplicationScrollbar_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::Scrollbar' */
void ApplicationScrollbar__ReInit( ApplicationScrollbar _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsImage__ReInit( &_this->Line );
  ViewsImage__ReInit( &_this->Dot );
}

/* Finalizer method for the class 'Application::Scrollbar' */
void ApplicationScrollbar__Done( ApplicationScrollbar _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsImage__Done( &_this->Line );
  ViewsImage__Done( &_this->Dot );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationScrollbar_Init( ApplicationScrollbar _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, EwSetRectW( _this->Super2.Bounds, 96 ));
  CoreRectView__OnSetBounds( _this, EwSetRectH( _this->Super2.Bounds, 3 ));
  ViewsImage_OnSetColor( &_this->Line, _Const002C );
  ViewsImage_OnSetColor( &_this->Dot, _Const0001 );
  ApplicationScrollbar_Animate( _this );
}

/* 'C' function for method : 'Application::Scrollbar.OnSetSelected()' */
void ApplicationScrollbar_OnSetSelected( ApplicationScrollbar _this, XUInt32 value )
{
  if ( _this->Selected == value )
    return;

  _this->Selected = value;
  ApplicationScrollbar_Animate( _this );
}

/* 'C' function for method : 'Application::Scrollbar.OnSetMaximum()' */
void ApplicationScrollbar_OnSetMaximum( ApplicationScrollbar _this, XUInt32 value )
{
  if ( _this->Maximum == value )
    return;

  _this->Maximum = value;
  ApplicationScrollbar_Animate( _this );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationScrollbar_Animate( ApplicationScrollbar _this )
{
  XFloat AcriveArea;
  XFloat temp;

  AcriveArea = (XFloat)( EwGetRectW( _this->Super2.Bounds ) - 30 );
  temp = (XFloat)_this->Selected / (XFloat)_this->Maximum;
  temp = temp * AcriveArea;
  CoreRectView__OnSetBounds( &_this->Dot, EwSetRectX( _this->Dot.Super1.Bounds, 
  (XInt32)(( temp + 15 ) - ( EwGetRectW( _this->Dot.Super1.Bounds ) / 2 ))));
}

/* Variants derived from the class : 'Application::Scrollbar' */
EW_DEFINE_CLASS_VARIANTS( ApplicationScrollbar )
EW_END_OF_CLASS_VARIANTS( ApplicationScrollbar )

/* Virtual Method Table (VMT) for the class : 'Application::Scrollbar' */
EW_DEFINE_CLASS( ApplicationScrollbar, CoreGroup, Line, Line, Line, Line, Selected, 
                 Selected, "Application::Scrollbar" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationScrollbar )

/* Initializer for the class 'Application::ParamValue' */
void ApplicationParamValue__Init( ApplicationParamValue _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  ApplicationParamBase__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationParamValue );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Background, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->ValueText, &_this->_.XObject, 0 );
  ApplicationValueBar__Init( &_this->ValueBar, &_this->_.XObject, 0 );
  ApplicationTextLine__Init( &_this->TextLine, &_this->_.XObject, 0 );
  ApplicationScrollbar__Init( &_this->Scrollbar, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationParamValue );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( &_this->Background, _Const0000 );
  ViewsRectangle_OnSetColorBL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorBR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColor( &_this->Background, _Const0002 );
  ViewsRectangle_OnSetAlphaBlended( &_this->Background, 0 );
  CoreRectView__OnSetBounds( &_this->ValueText, _Const002D );
  ViewsText_OnSetString( &_this->ValueText, EwLoadString( &_Const002E ));
  ViewsText_OnSetColor( &_this->ValueText, _Const0001 );
  CoreRectView__OnSetBounds( &_this->ValueBar, _Const002F );
  ApplicationValueBar_OnSetType( &_this->ValueBar, DataValueBarTypePositive );
  CoreRectView__OnSetBounds( &_this->TextLine, _Const0030 );
  CoreRectView__OnSetBounds( &_this->Scrollbar, _Const0031 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Background ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ValueText ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->ValueBar ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->TextLine ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Scrollbar ), 0 );
  _this->Param = EwGetAutoObject( &ParameterActiveParameter, ParameterParameter );
  ViewsText_OnSetFont( &_this->ValueText, EwLoadResource( &ResFont_1, ResourcesFont ));

  /* Call the user defined constructor */
  ApplicationParamValue_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ParamValue' */
void ApplicationParamValue__ReInit( ApplicationParamValue _this )
{
  /* At first re-initialize the super class ... */
  ApplicationParamBase__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Background );
  ViewsText__ReInit( &_this->ValueText );
  ApplicationValueBar__ReInit( &_this->ValueBar );
  ApplicationTextLine__ReInit( &_this->TextLine );
  ApplicationScrollbar__ReInit( &_this->Scrollbar );
}

/* Finalizer method for the class 'Application::ParamValue' */
void ApplicationParamValue__Done( ApplicationParamValue _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( ApplicationParamBase );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Background );
  ViewsText__Done( &_this->ValueText );
  ApplicationValueBar__Done( &_this->ValueBar );
  ApplicationTextLine__Done( &_this->TextLine );
  ApplicationScrollbar__Done( &_this->Scrollbar );

  /* Don't forget to deinitialize the super class ... */
  ApplicationParamBase__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationParamValue_Init( ApplicationParamValue _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, DatagScreenSize );
  ApplicationParamValue_ArrangeText( _this );
  EwAttachObjObserver( EwNewSlot( _this, ApplicationParamValue_OnUpdate_debug ), 
    (XObject)EwGetAutoObject( &ParameterActiveParameter, ParameterParameter ), 0 );
  EwAttachRefObserver( EwNewSlot( _this, ApplicationParamValue_OnChangeConfig ), 
    EwNewRef( EwGetAutoObject( &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetNoOfParameter, 
    ApplicationDeviceClass_OnSetNoOfParameter ), 0 );
  EwAttachRefObserver( EwNewSlot( _this, ApplicationParamValue_OnChangeConfig ), 
    EwNewRef( EwGetAutoObject( &ApplicationDevice, ApplicationDeviceClass ), ApplicationDeviceClass_OnGetCurrentParameter, 
    ApplicationDeviceClass_OnSetCurrentParameter ), 0 );
  EwAttachObjObserver( EwNewSlot( _this, ApplicationParamValue_OnChangeConfig ), 
    (XObject)EwGetAutoObject( &ParameterActiveParameter, ParameterParameter ), 0 );
  _this->Param->Id = 3;
  EwPostSignal( EwNewSlot( _this, ApplicationParamValue_OnChangeConfig ), ((XObject)_this ));
}

/* 'C' function for method : 'Application::ParamValue.OnUpdate_debug()' */
void ApplicationParamValue_OnUpdate_debug( ApplicationParamValue _this, XObject 
  sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  ViewsText_OnSetString( &_this->ValueText, ParameterParameter_GetValueFormatted( 
  EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )));
  ApplicationValueBar_OnSetValueProz( &_this->ValueBar, ParameterParameter_GetValuePozent( 
  EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )));
}

/* 'C' function for method : 'Application::ParamValue.OnChangeConfig()' */
void ApplicationParamValue_OnChangeConfig( ApplicationParamValue _this, XObject 
  sender )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( sender );

  ApplicationTextLine_OnSetIconId( &_this->TextLine, EwGetAutoObject( &ParameterActiveParameter, 
  ParameterParameter )->image );
  ApplicationScrollbar_OnSetMaximum( &_this->Scrollbar, EwGetAutoObject( &ParameterActiveParameter, 
  ParameterParameter )->totalparams );

  if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->unit_id 
      == 7 )
  {
    ApplicationTextLine_OnSetUnit_Text( &_this->TextLine, EwLoadString( &_Const0032 ));
    ApplicationTextLine_OnSetIconId( &_this->TextLine, 1 );
    ApplicationTextLine_OnSetType( &_this->TextLine, DataTextLineTypeIcon_Unit );
    ApplicationValueBar_OnSetType( &_this->ValueBar, DataValueBarTypePositive );
  }

  if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->unit_id 
      == 22 )
  {
    ApplicationTextLine_OnSetUnit_Text( &_this->TextLine, EwLoadString( &_Const0027 ));
    ApplicationTextLine_OnSetIconId( &_this->TextLine, 2 );
    ApplicationTextLine_OnSetType( &_this->TextLine, DataTextLineTypeIcon_Unit );
    ApplicationValueBar_OnSetType( &_this->ValueBar, DataValueBarTypePositiveNegative );
  }

  if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->unit_id 
      == 24 )
  {
    ApplicationTextLine_OnSetUnit_Text( &_this->TextLine, EwLoadString( &_Const0029 ));
    ApplicationTextLine_OnSetIconId( &_this->TextLine, 0 );
    ApplicationTextLine_OnSetType( &_this->TextLine, DataTextLineTypeOnlyText );
    ApplicationValueBar_OnSetType( &_this->ValueBar, DataValueBarTypePositiveNegative );
  }

  if ( EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->unit_id 
      == 2 )
  {
    ApplicationTextLine_OnSetUnit_Text( &_this->TextLine, EwLoadString( &_Const0033 ));
    ApplicationTextLine_OnSetIconId( &_this->TextLine, 5 );
    ApplicationTextLine_OnSetType( &_this->TextLine, DataTextLineTypeIcon_Unit );
    ApplicationValueBar_OnSetType( &_this->ValueBar, DataValueBarTypePositive );
  }

  if ( !!EwGetAutoObject( &ParameterActiveParameter, ParameterParameter )->Id )
    ApplicationScrollbar_OnSetSelected( &_this->Scrollbar, (XUInt32)( EwGetAutoObject( 
    &ParameterActiveParameter, ParameterParameter )->Id - 1 ));
  else
    ApplicationScrollbar_OnSetSelected( &_this->Scrollbar, _this->Scrollbar.Maximum );
}

/* 'C' function for method : 'Application::ParamValue.ArrangeText()' */
void ApplicationParamValue_ArrangeText( ApplicationParamValue _this )
{
  ViewsText_OnSetFont( &_this->ValueText, EwLoadResource( &ResFont_1, ResourcesFont ));
  CoreRectView__OnSetBounds( &_this->ValueText, EwSetRectH( _this->ValueText.Super1.Bounds, 
  _this->ValueText.Font->Ascent + _this->ValueText.Font->Descent ));
  CoreRectView__OnSetBounds( &_this->ValueText, EwSetRectY( _this->ValueText.Super1.Bounds, 
  33 - _this->ValueText.Font->Ascent ));
}

/* Variants derived from the class : 'Application::ParamValue' */
EW_DEFINE_CLASS_VARIANTS( ApplicationParamValue )
EW_END_OF_CLASS_VARIANTS( ApplicationParamValue )

/* Virtual Method Table (VMT) for the class : 'Application::ParamValue' */
EW_DEFINE_CLASS( ApplicationParamValue, ApplicationParamBase, Param, Background, 
                 Background, Background, _.VMT, _.VMT, "Application::ParamValue" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationParamValue )

/* Initializer for the class 'Application::ParamToggle' */
void ApplicationParamToggle__Init( ApplicationParamToggle _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  ApplicationParamBase__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationParamToggle );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Background, &_this->_.XObject, 0 );
  ViewsImage__Init( &_this->Image, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationParamToggle );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( &_this->Background, _Const0000 );
  ViewsRectangle_OnSetColorBL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorBR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColor( &_this->Background, _Const0002 );
  ViewsRectangle_OnSetAlphaBlended( &_this->Background, 0 );
  CoreRectView__OnSetBounds( &_this->Image, _Const0034 );
  ViewsImage_OnSetFrameNumber( &_this->Image, 3 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Background ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Image ), 0 );
  ViewsImage_OnSetBitmap( &_this->Image, EwLoadResource( &ResIcon_Function_A_96x40, 
  ResourcesBitmap ));

  /* Call the user defined constructor */
  ApplicationParamToggle_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ParamToggle' */
void ApplicationParamToggle__ReInit( ApplicationParamToggle _this )
{
  /* At first re-initialize the super class ... */
  ApplicationParamBase__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Background );
  ViewsImage__ReInit( &_this->Image );
}

/* Finalizer method for the class 'Application::ParamToggle' */
void ApplicationParamToggle__Done( ApplicationParamToggle _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( ApplicationParamBase );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Background );
  ViewsImage__Done( &_this->Image );

  /* Don't forget to deinitialize the super class ... */
  ApplicationParamBase__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationParamToggle_Init( ApplicationParamToggle _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  ApplicationParamToggle_Animate( _this );
}

/* 'C' function for method : 'Application::ParamToggle.Animate()' */
void ApplicationParamToggle_Animate( ApplicationParamToggle _this )
{
  ViewsImage_OnSetFrameNumber( &_this->Image, 0 );
}

/* Variants derived from the class : 'Application::ParamToggle' */
EW_DEFINE_CLASS_VARIANTS( ApplicationParamToggle )
EW_END_OF_CLASS_VARIANTS( ApplicationParamToggle )

/* Virtual Method Table (VMT) for the class : 'Application::ParamToggle' */
EW_DEFINE_CLASS( ApplicationParamToggle, ApplicationParamBase, Background, Background, 
                 Background, Background, _.VMT, _.VMT, "Application::ParamToggle" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationParamToggle )

/* Initializer for the class 'Application::ParamBase' */
void ApplicationParamBase__Init( ApplicationParamBase _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationParamBase );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationParamBase );

  /* Call the user defined constructor */
  ApplicationParamBase_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ParamBase' */
void ApplicationParamBase__ReInit( ApplicationParamBase _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );
}

/* Finalizer method for the class 'Application::ParamBase' */
void ApplicationParamBase__Done( ApplicationParamBase _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationParamBase_Init( ApplicationParamBase _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, DatagScreenSize );
}

/* Variants derived from the class : 'Application::ParamBase' */
EW_DEFINE_CLASS_VARIANTS( ApplicationParamBase )
EW_END_OF_CLASS_VARIANTS( ApplicationParamBase )

/* Virtual Method Table (VMT) for the class : 'Application::ParamBase' */
EW_DEFINE_CLASS( ApplicationParamBase, CoreGroup, _.VMT, _.VMT, _.VMT, _.VMT, _.VMT, 
                 _.VMT, "Application::ParamBase" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationParamBase )

/* Initializer for the class 'Application::ParamQAIndicator' */
void ApplicationParamQAIndicator__Init( ApplicationParamQAIndicator _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  ApplicationParamBase__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationParamQAIndicator );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Background, &_this->_.XObject, 0 );
  ViewsImage__Init( &_this->Image, &_this->_.XObject, 0 );
  ApplicationScrollbar__Init( &_this->Scrollbar, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationParamQAIndicator );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( &_this->Background, _Const0000 );
  ViewsRectangle_OnSetColorBL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorBR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTR( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColorTL( &_this->Background, _Const0001 );
  ViewsRectangle_OnSetColor( &_this->Background, _Const0002 );
  ViewsRectangle_OnSetAlphaBlended( &_this->Background, 0 );
  CoreRectView__OnSetBounds( &_this->Image, _Const0034 );
  CoreRectView__OnSetBounds( &_this->Scrollbar, _Const0035 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Background ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Image ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Scrollbar ), 0 );
  ViewsImage_OnSetBitmap( &_this->Image, EwLoadResource( &ResIcon_Function_A_96x40, 
  ResourcesBitmap ));

  /* Call the user defined constructor */
  ApplicationParamQAIndicator_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::ParamQAIndicator' */
void ApplicationParamQAIndicator__ReInit( ApplicationParamQAIndicator _this )
{
  /* At first re-initialize the super class ... */
  ApplicationParamBase__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Background );
  ViewsImage__ReInit( &_this->Image );
  ApplicationScrollbar__ReInit( &_this->Scrollbar );
}

/* Finalizer method for the class 'Application::ParamQAIndicator' */
void ApplicationParamQAIndicator__Done( ApplicationParamQAIndicator _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( ApplicationParamBase );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Background );
  ViewsImage__Done( &_this->Image );
  ApplicationScrollbar__Done( &_this->Scrollbar );

  /* Don't forget to deinitialize the super class ... */
  ApplicationParamBase__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationParamQAIndicator_Init( ApplicationParamQAIndicator _this, XHandle 
  aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  ViewsImage_OnSetFrameNumber( &_this->Image, 14 );
}

/* Variants derived from the class : 'Application::ParamQAIndicator' */
EW_DEFINE_CLASS_VARIANTS( ApplicationParamQAIndicator )
EW_END_OF_CLASS_VARIANTS( ApplicationParamQAIndicator )

/* Virtual Method Table (VMT) for the class : 'Application::ParamQAIndicator' */
EW_DEFINE_CLASS( ApplicationParamQAIndicator, ApplicationParamBase, Background, 
                 Background, Background, Background, _.VMT, _.VMT, "Application::ParamQAIndicator" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationParamQAIndicator )

/* Initializer for the class 'Application::SplashScreen' */
void ApplicationSplashScreen__Init( ApplicationSplashScreen _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationSplashScreen );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Rectangle, &_this->_.XObject, 0 );
  ViewsImage__Init( &_this->Image, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationSplashScreen );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( &_this->Rectangle, _Const0036 );
  CoreRectView__OnSetBounds( &_this->Image, _Const0000 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Image ), 0 );
  ViewsImage_OnSetBitmap( &_this->Image, EwLoadResource( &ResSplashScreen, ResourcesBitmap ));

  /* Call the user defined constructor */
  ApplicationSplashScreen_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::SplashScreen' */
void ApplicationSplashScreen__ReInit( ApplicationSplashScreen _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Rectangle );
  ViewsImage__ReInit( &_this->Image );
}

/* Finalizer method for the class 'Application::SplashScreen' */
void ApplicationSplashScreen__Done( ApplicationSplashScreen _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Rectangle );
  ViewsImage__Done( &_this->Image );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationSplashScreen_Init( ApplicationSplashScreen _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, DatagScreenSize );
  ViewsImage_OnSetColor( &_this->Image, _Const0001 );
  ViewsRectangle_OnSetColor( &_this->Rectangle, _Const0037 );
}

/* Variants derived from the class : 'Application::SplashScreen' */
EW_DEFINE_CLASS_VARIANTS( ApplicationSplashScreen )
EW_END_OF_CLASS_VARIANTS( ApplicationSplashScreen )

/* Virtual Method Table (VMT) for the class : 'Application::SplashScreen' */
EW_DEFINE_CLASS( ApplicationSplashScreen, CoreGroup, Rectangle, Rectangle, Rectangle, 
                 Rectangle, _.VMT, _.VMT, "Application::SplashScreen" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationSplashScreen )

/* Initializer for the class 'Application::TestDialog' */
void ApplicationTestDialog__Init( ApplicationTestDialog _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  CoreGroup__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( ApplicationTestDialog );

  /* ... then construct all embedded objects */
  ViewsRectangle__Init( &_this->Base_rectangle, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_CAN, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_BKC, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_UP, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_DOWN, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_LEFT, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_RIGHT, &_this->_.XObject, 0 );
  ViewsRectangle__Init( &_this->Rectangle_TRG, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_CAN, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_BKC, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_Left, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_Right, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_Up, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_Down, &_this->_.XObject, 0 );
  ViewsText__Init( &_this->Text_Trigger, &_this->_.XObject, 0 );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( ApplicationTestDialog );

  /* ... and initialize objects, variables, properties, etc. */
  CoreRectView__OnSetBounds( &_this->Base_rectangle, _Const0000 );
  CoreRectView__OnSetBounds( &_this->Rectangle_CAN, _Const0038 );
  ViewsRectangle_OnSetColor( &_this->Rectangle_CAN, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_BKC, _Const0039 );
  ViewsRectangle_OnSetColor( &_this->Rectangle_BKC, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_UP, _Const003A );
  ViewsRectangle_OnSetColor( &_this->Rectangle_UP, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_DOWN, _Const003B );
  ViewsRectangle_OnSetColor( &_this->Rectangle_DOWN, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_LEFT, _Const003C );
  ViewsRectangle_OnSetColor( &_this->Rectangle_LEFT, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_RIGHT, _Const003D );
  ViewsRectangle_OnSetColor( &_this->Rectangle_RIGHT, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Rectangle_TRG, _Const003E );
  ViewsRectangle_OnSetColor( &_this->Rectangle_TRG, _Const0008 );
  CoreRectView__OnSetBounds( &_this->Text_CAN, _Const003F );
  ViewsText_OnSetAlignment( &_this->Text_CAN, ViewsTextAlignmentAlignHorzLeft | 
  ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_CAN, EwLoadString( &_Const0009 ));
  ViewsText_OnSetColor( &_this->Text_CAN, _Const0002 );
  ViewsText_OnSetOpacity( &_this->Text_CAN, 255 );
  CoreRectView__OnSetBounds( &_this->Text_BKC, _Const0040 );
  ViewsText_OnSetAlignment( &_this->Text_BKC, ViewsTextAlignmentAlignHorzLeft | 
  ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_BKC, EwLoadString( &_Const000C ));
  ViewsText_OnSetColor( &_this->Text_BKC, _Const0002 );
  CoreRectView__OnSetBounds( &_this->Text_Left, _Const0041 );
  ViewsText_OnSetAlignment( &_this->Text_Left, ViewsTextAlignmentAlignHorzLeft | 
  ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_Left, EwLoadString( &_Const0015 ));
  ViewsText_OnSetColor( &_this->Text_Left, _Const0002 );
  CoreRectView__OnSetBounds( &_this->Text_Right, _Const0042 );
  ViewsText_OnSetAlignment( &_this->Text_Right, ViewsTextAlignmentAlignHorzLeft 
  | ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_Right, EwLoadString( &_Const0018 ));
  ViewsText_OnSetColor( &_this->Text_Right, _Const0002 );
  CoreRectView__OnSetBounds( &_this->Text_Up, _Const0043 );
  ViewsText_OnSetAlignment( &_this->Text_Up, ViewsTextAlignmentAlignHorzLeft | ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_Up, EwLoadString( &_Const000F ));
  ViewsText_OnSetColor( &_this->Text_Up, _Const0002 );
  CoreRectView__OnSetBounds( &_this->Text_Down, _Const0044 );
  ViewsText_OnSetAlignment( &_this->Text_Down, ViewsTextAlignmentAlignHorzLeft | 
  ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_Down, EwLoadString( &_Const0012 ));
  ViewsText_OnSetColor( &_this->Text_Down, _Const0002 );
  CoreRectView__OnSetBounds( &_this->Text_Trigger, _Const0045 );
  ViewsText_OnSetAlignment( &_this->Text_Trigger, ViewsTextAlignmentAlignHorzLeft 
  | ViewsTextAlignmentAlignVertCenter );
  ViewsText_OnSetString( &_this->Text_Trigger, EwLoadString( &_Const001B ));
  ViewsText_OnSetColor( &_this->Text_Trigger, _Const0002 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Base_rectangle ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_CAN ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_BKC ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_UP ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_DOWN ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_LEFT ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_RIGHT ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Rectangle_TRG ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_CAN ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_BKC ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_Left ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_Right ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_Up ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_Down ), 0 );
  CoreGroup_Add((CoreGroup)_this, ((CoreView)&_this->Text_Trigger ), 0 );
  ViewsText_OnSetFont( &_this->Text_CAN, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_BKC, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_Left, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_Right, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_Up, EwLoadResource( &ResourcesDefaultFont, ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_Down, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));
  ViewsText_OnSetFont( &_this->Text_Trigger, EwLoadResource( &ResourcesDefaultFont, 
  ResourcesFont ));

  /* Call the user defined constructor */
  ApplicationTestDialog_Init( _this, aArg );
}

/* Re-Initializer for the class 'Application::TestDialog' */
void ApplicationTestDialog__ReInit( ApplicationTestDialog _this )
{
  /* At first re-initialize the super class ... */
  CoreGroup__ReInit( &_this->_.Super );

  /* ... then re-construct all embedded objects */
  ViewsRectangle__ReInit( &_this->Base_rectangle );
  ViewsRectangle__ReInit( &_this->Rectangle_CAN );
  ViewsRectangle__ReInit( &_this->Rectangle_BKC );
  ViewsRectangle__ReInit( &_this->Rectangle_UP );
  ViewsRectangle__ReInit( &_this->Rectangle_DOWN );
  ViewsRectangle__ReInit( &_this->Rectangle_LEFT );
  ViewsRectangle__ReInit( &_this->Rectangle_RIGHT );
  ViewsRectangle__ReInit( &_this->Rectangle_TRG );
  ViewsText__ReInit( &_this->Text_CAN );
  ViewsText__ReInit( &_this->Text_BKC );
  ViewsText__ReInit( &_this->Text_Left );
  ViewsText__ReInit( &_this->Text_Right );
  ViewsText__ReInit( &_this->Text_Up );
  ViewsText__ReInit( &_this->Text_Down );
  ViewsText__ReInit( &_this->Text_Trigger );
}

/* Finalizer method for the class 'Application::TestDialog' */
void ApplicationTestDialog__Done( ApplicationTestDialog _this )
{
  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( CoreGroup );

  /* Finalize all embedded objects */
  ViewsRectangle__Done( &_this->Base_rectangle );
  ViewsRectangle__Done( &_this->Rectangle_CAN );
  ViewsRectangle__Done( &_this->Rectangle_BKC );
  ViewsRectangle__Done( &_this->Rectangle_UP );
  ViewsRectangle__Done( &_this->Rectangle_DOWN );
  ViewsRectangle__Done( &_this->Rectangle_LEFT );
  ViewsRectangle__Done( &_this->Rectangle_RIGHT );
  ViewsRectangle__Done( &_this->Rectangle_TRG );
  ViewsText__Done( &_this->Text_CAN );
  ViewsText__Done( &_this->Text_BKC );
  ViewsText__Done( &_this->Text_Left );
  ViewsText__Done( &_this->Text_Right );
  ViewsText__Done( &_this->Text_Up );
  ViewsText__Done( &_this->Text_Down );
  ViewsText__Done( &_this->Text_Trigger );

  /* Don't forget to deinitialize the super class ... */
  CoreGroup__Done( &_this->_.Super );
}

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTestDialog_Init( ApplicationTestDialog _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  CoreRectView__OnSetBounds( _this, DatagScreenSize );
}

/* Variants derived from the class : 'Application::TestDialog' */
EW_DEFINE_CLASS_VARIANTS( ApplicationTestDialog )
EW_END_OF_CLASS_VARIANTS( ApplicationTestDialog )

/* Virtual Method Table (VMT) for the class : 'Application::TestDialog' */
EW_DEFINE_CLASS( ApplicationTestDialog, CoreGroup, Base_rectangle, Base_rectangle, 
                 Base_rectangle, Base_rectangle, _.VMT, _.VMT, "Application::TestDialog" )
  CoreRectView_initLayoutContext,
  CoreGroup_Draw,
  CoreGroup_CursorHitTest,
  CoreRectView_ArrangeView,
  CoreRectView_MoveView,
  CoreRectView_GetExtent,
  CoreGroup_ChangeViewState,
  CoreGroup_OnSetBounds,
  CoreGroup_OnSetFocus,
  CoreGroup_DispatchEvent,
  CoreGroup_BroadcastEvent,
  CoreGroup_InvalidateArea,
EW_END_OF_CLASS( ApplicationTestDialog )

/* Embedded Wizard */

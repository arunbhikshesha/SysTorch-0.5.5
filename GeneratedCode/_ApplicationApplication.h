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

#ifndef _ApplicationApplication_H
#define _ApplicationApplication_H

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

#include "_ApplicationParamDialog.h"
#include "_ApplicationReceiveDialog.h"
#include "_ApplicationSplashScreen.h"
#include "_CoreKeyPressHandler.h"
#include "_CorePropertyObserver.h"
#include "_CoreRoot.h"
#include "_CoreTimer.h"
#include "_ViewsRectangle.h"

/* Forward declaration of the class Application::Application */
#ifndef _ApplicationApplication_
  EW_DECLARE_CLASS( ApplicationApplication )
#define _ApplicationApplication_
#endif

/* Forward declaration of the class Core::Group */
#ifndef _CoreGroup_
  EW_DECLARE_CLASS( CoreGroup )
#define _CoreGroup_
#endif

/* Forward declaration of the class Core::LayoutContext */
#ifndef _CoreLayoutContext_
  EW_DECLARE_CLASS( CoreLayoutContext )
#define _CoreLayoutContext_
#endif

/* Forward declaration of the class Core::View */
#ifndef _CoreView_
  EW_DECLARE_CLASS( CoreView )
#define _CoreView_
#endif

/* Forward declaration of the class Graphics::Canvas */
#ifndef _GraphicsCanvas_
  EW_DECLARE_CLASS( GraphicsCanvas )
#define _GraphicsCanvas_
#endif

/* Forward declaration of the class Parameter::DeviceClass */
#ifndef _ParameterDeviceClass_
  EW_DECLARE_CLASS( ParameterDeviceClass )
#define _ParameterDeviceClass_
#endif

/* Forward declaration of the class Simulator::SimulatorClass */
#ifndef _SimulatorSimulatorClass_
  EW_DECLARE_CLASS( SimulatorSimulatorClass )
#define _SimulatorSimulatorClass_
#endif


/* This is the root component of the entire GUI application. */
EW_DEFINE_FIELDS( ApplicationApplication, CoreRoot )
  EW_VARIABLE( Simulator,       SimulatorSimulatorClass )
  EW_VARIABLE( par_device_instance, ParameterDeviceClass )
  EW_OBJECT  ( Background,      ViewsRectangle )
  EW_OBJECT  ( KeyHandler,      CoreKeyPressHandler )
  EW_OBJECT  ( ReceiveDialog,   ApplicationReceiveDialog )
  EW_OBJECT  ( ShowQAObserver,  CorePropertyObserver )
  EW_OBJECT  ( ParamDialog,     ApplicationParamDialog )
  EW_OBJECT  ( SplashScreen,    ApplicationSplashScreen )
  EW_OBJECT  ( SpashScreenTimer, CoreTimer )
EW_END_OF_FIELDS( ApplicationApplication )

/* Virtual Method Table (VMT) for the class : 'Application::Application' */
EW_DEFINE_METHODS( ApplicationApplication, CoreRoot )
  EW_METHOD( initLayoutContext, void )( CoreRectView _this, XRect aBounds, CoreOutline 
    aOutline )
  EW_METHOD( Draw,              void )( CoreRoot _this, GraphicsCanvas aCanvas, 
    XRect aClip, XPoint aOffset, XInt32 aOpacity, XBool aBlend )
  EW_METHOD( CursorHitTest,     CoreCursorHit )( CoreGroup _this, XRect aArea, XInt32 
    aFinger, XInt32 aStrikeCount, CoreView aDedicatedView, XSet aRetargetReason )
  EW_METHOD( ArrangeView,       XPoint )( CoreRectView _this, XRect aBounds, XEnum 
    aFormation )
  EW_METHOD( MoveView,          void )( CoreRectView _this, XPoint aOffset, XBool 
    aFastMove )
  EW_METHOD( GetExtent,         XRect )( CoreRectView _this )
  EW_METHOD( ChangeViewState,   void )( CoreRoot _this, XSet aSetState, XSet aClearState )
  EW_METHOD( OnSetBounds,       void )( CoreGroup _this, XRect value )
  EW_METHOD( OnSetFocus,        void )( CoreRoot _this, CoreView value )
  EW_METHOD( DispatchEvent,     XObject )( CoreRoot _this, CoreEvent aEvent )
  EW_METHOD( BroadcastEvent,    XObject )( CoreRoot _this, CoreEvent aEvent, XSet 
    aFilter )
  EW_METHOD( InvalidateArea,    void )( CoreRoot _this, XRect aArea )
EW_END_OF_METHODS( ApplicationApplication )

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationApplication_Init( ApplicationApplication _this, XHandle aArg );

/* 'C' function for method : 'Application::Application.OnKeyPress()' */
void ApplicationApplication_OnKeyPress( ApplicationApplication _this, XObject sender );

/* This slot method is executed when the associated property observer 'PropertyObserver' 
   is notified. */
void ApplicationApplication_onShowQA( ApplicationApplication _this, XObject sender );

/* 'C' function for method : 'Application::Application.RemoveSplash()' */
void ApplicationApplication_RemoveSplash( ApplicationApplication _this, XObject 
  sender );

#ifdef __cplusplus
  }
#endif

#endif /* _ApplicationApplication_H */

/* Embedded Wizard */

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

#ifndef _ApplicationTextLine_H
#define _ApplicationTextLine_H

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

#include "_CoreGroup.h"
#include "_ViewsImage.h"
#include "_ViewsText.h"

/* Forward declaration of the class Application::TextLine */
#ifndef _ApplicationTextLine_
  EW_DECLARE_CLASS( ApplicationTextLine )
#define _ApplicationTextLine_
#endif

/* Forward declaration of the class Core::KeyPressHandler */
#ifndef _CoreKeyPressHandler_
  EW_DECLARE_CLASS( CoreKeyPressHandler )
#define _CoreKeyPressHandler_
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


/* Deklaration of class : 'Application::TextLine' */
EW_DEFINE_FIELDS( ApplicationTextLine, CoreGroup )
  EW_OBJECT  ( ParamImage,      ViewsImage )
  EW_OBJECT  ( UnitText,        ViewsText )
  EW_OBJECT  ( InfoText,        ViewsText )
  EW_PROPERTY( Unit_Text,       XString )
  EW_PROPERTY( IconId,          XUInt32 )
  EW_PROPERTY( Type,            XEnum )
EW_END_OF_FIELDS( ApplicationTextLine )

/* Virtual Method Table (VMT) for the class : 'Application::TextLine' */
EW_DEFINE_METHODS( ApplicationTextLine, CoreGroup )
  EW_METHOD( initLayoutContext, void )( CoreRectView _this, XRect aBounds, CoreOutline 
    aOutline )
  EW_METHOD( Draw,              void )( CoreGroup _this, GraphicsCanvas aCanvas, 
    XRect aClip, XPoint aOffset, XInt32 aOpacity, XBool aBlend )
  EW_METHOD( CursorHitTest,     CoreCursorHit )( CoreGroup _this, XRect aArea, XInt32 
    aFinger, XInt32 aStrikeCount, CoreView aDedicatedView, XSet aRetargetReason )
  EW_METHOD( ArrangeView,       XPoint )( CoreRectView _this, XRect aBounds, XEnum 
    aFormation )
  EW_METHOD( MoveView,          void )( CoreRectView _this, XPoint aOffset, XBool 
    aFastMove )
  EW_METHOD( GetExtent,         XRect )( CoreRectView _this )
  EW_METHOD( ChangeViewState,   void )( CoreGroup _this, XSet aSetState, XSet aClearState )
  EW_METHOD( OnSetBounds,       void )( CoreGroup _this, XRect value )
  EW_METHOD( OnSetFocus,        void )( CoreGroup _this, CoreView value )
  EW_METHOD( DispatchEvent,     XObject )( CoreGroup _this, CoreEvent aEvent )
  EW_METHOD( BroadcastEvent,    XObject )( CoreGroup _this, CoreEvent aEvent, XSet 
    aFilter )
  EW_METHOD( InvalidateArea,    void )( CoreGroup _this, XRect aArea )
EW_END_OF_METHODS( ApplicationTextLine )

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTextLine_Init( ApplicationTextLine _this, XHandle aArg );

/* 'C' function for method : 'Application::TextLine.OnSetType()' */
void ApplicationTextLine_OnSetType( ApplicationTextLine _this, XEnum value );

/* 'C' function for method : 'Application::TextLine.OnSetIconId()' */
void ApplicationTextLine_OnSetIconId( ApplicationTextLine _this, XUInt32 value );

/* 'C' function for method : 'Application::TextLine.OnSetUnit_Text()' */
void ApplicationTextLine_OnSetUnit_Text( ApplicationTextLine _this, XString value );

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTextLine_Animate( ApplicationTextLine _this );

#ifdef __cplusplus
  }
#endif

#endif /* _ApplicationTextLine_H */

/* Embedded Wizard */

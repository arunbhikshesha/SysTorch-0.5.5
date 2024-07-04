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

#ifndef _ApplicationTestDialog_H
#define _ApplicationTestDialog_H

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
#include "_ViewsRectangle.h"
#include "_ViewsText.h"

/* Forward declaration of the class Application::TestDialog */
#ifndef _ApplicationTestDialog_
  EW_DECLARE_CLASS( ApplicationTestDialog )
#define _ApplicationTestDialog_
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


/* Deklaration of class : 'Application::TestDialog' */
EW_DEFINE_FIELDS( ApplicationTestDialog, CoreGroup )
  EW_OBJECT  ( Base_rectangle,  ViewsRectangle )
  EW_OBJECT  ( Rectangle_CAN,   ViewsRectangle )
  EW_OBJECT  ( Rectangle_BKC,   ViewsRectangle )
  EW_OBJECT  ( Rectangle_UP,    ViewsRectangle )
  EW_OBJECT  ( Rectangle_DOWN,  ViewsRectangle )
  EW_OBJECT  ( Rectangle_LEFT,  ViewsRectangle )
  EW_OBJECT  ( Rectangle_RIGHT, ViewsRectangle )
  EW_OBJECT  ( Rectangle_TRG,   ViewsRectangle )
  EW_OBJECT  ( Text_CAN,        ViewsText )
  EW_OBJECT  ( Text_BKC,        ViewsText )
  EW_OBJECT  ( Text_Left,       ViewsText )
  EW_OBJECT  ( Text_Right,      ViewsText )
  EW_OBJECT  ( Text_Up,         ViewsText )
  EW_OBJECT  ( Text_Down,       ViewsText )
  EW_OBJECT  ( Text_Trigger,    ViewsText )
EW_END_OF_FIELDS( ApplicationTestDialog )

/* Virtual Method Table (VMT) for the class : 'Application::TestDialog' */
EW_DEFINE_METHODS( ApplicationTestDialog, CoreGroup )
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
EW_END_OF_METHODS( ApplicationTestDialog )

/* The method Init() is invoked automatically after the component has been created. 
   This method can be overridden and filled with logic containing additional initialization 
   statements. */
void ApplicationTestDialog_Init( ApplicationTestDialog _this, XHandle aArg );

#ifdef __cplusplus
  }
#endif

#endif /* _ApplicationTestDialog_H */

/* Embedded Wizard */

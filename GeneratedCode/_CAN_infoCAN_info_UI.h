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

#ifndef _CAN_infoCAN_info_UI_H
#define _CAN_infoCAN_info_UI_H

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
#include "_CoreTimer.h"
#include "_ViewsRectangle.h"
#include "_ViewsText.h"

/* Forward declaration of the class CAN_info::CAN_info_UI */
#ifndef _CAN_infoCAN_info_UI_
  EW_DECLARE_CLASS( CAN_infoCAN_info_UI )
#define _CAN_infoCAN_info_UI_
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


/* Deklaration of class : 'CAN_info::CAN_info_UI' */
EW_DEFINE_FIELDS( CAN_infoCAN_info_UI, CoreGroup )
  EW_OBJECT  ( Timer,           CoreTimer )
  EW_OBJECT  ( Rectangle,       ViewsRectangle )
  EW_OBJECT  ( Text_BusState,   ViewsText )
  EW_OBJECT  ( Text_ErrCnt,     ViewsText )
  EW_OBJECT  ( Text_Stuff_err,  ViewsText )
  EW_OBJECT  ( Text_form_err,   ViewsText )
  EW_OBJECT  ( Text_Bit1_err,   ViewsText )
  EW_OBJECT  ( Text_CRC_err,    ViewsText )
  EW_OBJECT  ( Text_ACK_err,    ViewsText )
  EW_OBJECT  ( Text_Bit0_err,   ViewsText )
EW_END_OF_FIELDS( CAN_infoCAN_info_UI )

/* Virtual Method Table (VMT) for the class : 'CAN_info::CAN_info_UI' */
EW_DEFINE_METHODS( CAN_infoCAN_info_UI, CoreGroup )
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
EW_END_OF_METHODS( CAN_infoCAN_info_UI )

/* 'C' function for method : 'CAN_info::CAN_info_UI.display_update()' */
void CAN_infoCAN_info_UI_display_update( CAN_infoCAN_info_UI _this, XObject sender );

#ifdef __cplusplus
  }
#endif

#endif /* _CAN_infoCAN_info_UI_H */

/* Embedded Wizard */

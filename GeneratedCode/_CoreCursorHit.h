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

#ifndef _CoreCursorHit_H
#define _CoreCursorHit_H

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

/* Forward declaration of the class Core::CursorHit */
#ifndef _CoreCursorHit_
  EW_DECLARE_CLASS( CoreCursorHit )
#define _CoreCursorHit_
#endif

/* Forward declaration of the class Core::View */
#ifndef _CoreView_
  EW_DECLARE_CLASS( CoreView )
#define _CoreView_
#endif


/* The class Core::CursorHit provides a helper object used to identify the view 
   tapped on the touch screen by the user. When the user touches the screen, the 
   framework searches for a view lying at the touched position and willing to start 
   the touch interaction. This search process is performed by the method Core::View.CursorHitTest(). */
EW_DEFINE_FIELDS( CoreCursorHit, XObject )
  EW_VARIABLE( View,            CoreView )
EW_END_OF_FIELDS( CoreCursorHit )

/* Virtual Method Table (VMT) for the class : 'Core::CursorHit' */
EW_DEFINE_METHODS( CoreCursorHit, XObject )
EW_END_OF_METHODS( CoreCursorHit )

#ifdef __cplusplus
  }
#endif

#endif /* _CoreCursorHit_H */

/* Embedded Wizard */

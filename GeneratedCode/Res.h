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

#ifndef Res_H
#define Res_H

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

/* Font resource : 'Res::Font_1' */
EW_DECLARE_FONT_RES( ResFont_1 )

/* Font resource : 'Res::Font_2' */
EW_DECLARE_FONT_RES( ResFont_2 )

/* Bitmap resource : 'Res::SplashScreen' */
EW_DECLARE_BITMAP_RES( ResSplashScreen )

/* Bitmap resource : 'Res::Scrollbar_Dot_18x3px' */
EW_DECLARE_BITMAP_RES( ResScrollbar_Dot_18x3px )

/* Bitmap resource : 'Res::Scrollbar_Line_96x1px' */
EW_DECLARE_BITMAP_RES( ResScrollbar_Line_96x1px )

/* Bitmap resource : 'Res::Icon_Function_A_96x40' */
EW_DECLARE_BITMAP_RES( ResIcon_Function_A_96x40 )

/* Bitmap resource : 'Res::Icon_Parameter_16x16_V3' */
EW_DECLARE_BITMAP_RES( ResIcon_Parameter_16x16_V3 )

/* User defined constant: 'Res::Color_RED' */
extern const XColor ResColor_RED;

/* User defined constant: 'Res::Color_InactiveGrey' */
extern const XColor ResColor_InactiveGrey;

#ifdef __cplusplus
  }
#endif

#endif /* Res_H */

/* Embedded Wizard */

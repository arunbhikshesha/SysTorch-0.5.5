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

#ifndef _WidgetSetVerticalValueBarConfig_H
#define _WidgetSetVerticalValueBarConfig_H

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

#include "_WidgetSetWidgetConfig.h"

/* Forward declaration of the class Resources::Bitmap */
#ifndef _ResourcesBitmap_
  EW_DECLARE_CLASS( ResourcesBitmap )
#define _ResourcesBitmap_
#endif

/* Forward declaration of the class WidgetSet::VerticalValueBarConfig */
#ifndef _WidgetSetVerticalValueBarConfig_
  EW_DECLARE_CLASS( WidgetSetVerticalValueBarConfig )
#define _WidgetSetVerticalValueBarConfig_
#endif


/* This class implements the functionality permitting you to simply customize the 
   look and feel of a 'vertical value bar' widget (WidgetSet::VerticalValueBar). 
   In the practice, you will create an instance of this class, configure its properties 
   with bitmaps, colors and other relevant attributes according to your design expectations 
   and assign such prepared configuration object to the property 'Appearance' of 
   every affected vertical value bar widget. Thereupon the widgets will use the 
   configuration information provided in the object.
   Since with the configuration object you determine the appearance of the value 
   bar widget you should know from which views the widget is composed of:
   - 'Face' is a bitmap frame view (Views::Frame) filling horizontally centered 
   the entire height in the background of the widget. In the configuration object 
   you can specify the desired bitmap (@Face), the frame number within the bitmap 
   (@FaceFrame) and opacity or color to tint the bitmap (@FaceTint). The value bar 
   can automatically play animated bitmaps if the specified frame number is -1.
   - 'TrackBelow' is a bitmap frame view (Views::Frame) filling horizontally centered 
   the background of the widget between its bottom edge and the actual position 
   of the needle. In the configuration object you can specify the desired bitmap 
   (@TrackBelow), the frame number within the bitmap (@TrackBelowFrame) and opacity 
   or color to tint the bitmap (@TrackBelowTint). With the property @TrackBelowWithEdge 
   you can configure how the top edge of the track (the edge at the needle position) 
   should appear. The value bar can automatically play animated bitmaps if the specified 
   frame number is -1.
   - 'TrackAbove' is a bitmap frame view (Views::Frame) filling horizontally centered 
   the background of the widget between the actual position of the needle and the 
   top edge of the widget. In the configuration object you can specify the desired 
   bitmap (@TrackAbove), the frame number within the bitmap (@TrackAboveFrame) and 
   opacity or color to tint the bitmap (@TrackAboveTint). With the property @TrackAboveWithEdge 
   you can configure how the bottom edge of the track (the edge at the needle position) 
   should appear. The value bar can automatically play animated bitmaps if the specified 
   frame number is -1.
   - 'Needle' is an image view (Views::Image) displayed horizontally centered at 
   the actual needle position. In the configuration object you can specify the desired 
   bitmap (@Needle), the frame number within the bitmap (@NeedleFrame) and opacity 
   or color to tint the bitmap (@NeedleTint). If necessary, additional margins below 
   and above the needle can be specified by using the properties @NeedleMarginBelow 
   and @NeedleMarginAbove. The value bar can automatically play animated bitmaps 
   if the specified frame number is -1.
   - 'Cover' is a bitmap frame view (Views::Frame) filling horizontally centered 
   the entire height of the widget and covering so eventually the needle and track. 
   In the configuration object you can specify the desired bitmap (@Cover), the 
   frame number within the bitmap (@CoverFrame) and opacity or color to tint the 
   bitmap (@CoverTint). The value bar can automatically play animated bitmaps if 
   the specified frame number is -1.
   The value bar widget can move the needle with a smooth animation. This can be 
   configured in the properties @SwingDuration and @SwingElastic.
   With the properties @WidgetMinSize and @WidgetMaxSize you can configure size 
   constraints for the widget itself. You can, for example, limit the value bar 
   widget to not shrink below a specified width or height. */
EW_DEFINE_FIELDS( WidgetSetVerticalValueBarConfig, WidgetSetWidgetConfig )
  EW_PROPERTY( TrackAbove,      ResourcesBitmap )
  EW_PROPERTY( TrackBelow,      ResourcesBitmap )
  EW_PROPERTY( SwingDuration,   XInt32 )
  EW_PROPERTY( NeedleMarginAbove, XInt32 )
  EW_PROPERTY( NeedleMarginBelow, XInt32 )
  EW_PROPERTY( TrackAboveFrame, XInt32 )
  EW_PROPERTY( TrackBelowFrame, XInt32 )
  EW_PROPERTY( WidgetMinSize,   XPoint )
EW_END_OF_FIELDS( WidgetSetVerticalValueBarConfig )

/* Virtual Method Table (VMT) for the class : 'WidgetSet::VerticalValueBarConfig' */
EW_DEFINE_METHODS( WidgetSetVerticalValueBarConfig, WidgetSetWidgetConfig )
EW_END_OF_METHODS( WidgetSetVerticalValueBarConfig )

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetSwingDuration()' */
void WidgetSetVerticalValueBarConfig_OnSetSwingDuration( WidgetSetVerticalValueBarConfig _this, 
  XInt32 value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetNeedleMarginAbove()' */
void WidgetSetVerticalValueBarConfig_OnSetNeedleMarginAbove( WidgetSetVerticalValueBarConfig _this, 
  XInt32 value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetNeedleMarginBelow()' */
void WidgetSetVerticalValueBarConfig_OnSetNeedleMarginBelow( WidgetSetVerticalValueBarConfig _this, 
  XInt32 value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetTrackAboveFrame()' */
void WidgetSetVerticalValueBarConfig_OnSetTrackAboveFrame( WidgetSetVerticalValueBarConfig _this, 
  XInt32 value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetTrackAbove()' */
void WidgetSetVerticalValueBarConfig_OnSetTrackAbove( WidgetSetVerticalValueBarConfig _this, 
  ResourcesBitmap value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetTrackBelowFrame()' */
void WidgetSetVerticalValueBarConfig_OnSetTrackBelowFrame( WidgetSetVerticalValueBarConfig _this, 
  XInt32 value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetTrackBelow()' */
void WidgetSetVerticalValueBarConfig_OnSetTrackBelow( WidgetSetVerticalValueBarConfig _this, 
  ResourcesBitmap value );

/* 'C' function for method : 'WidgetSet::VerticalValueBarConfig.OnSetWidgetMinSize()' */
void WidgetSetVerticalValueBarConfig_OnSetWidgetMinSize( WidgetSetVerticalValueBarConfig _this, 
  XPoint value );

#ifdef __cplusplus
  }
#endif

#endif /* _WidgetSetVerticalValueBarConfig_H */

/* Embedded Wizard */

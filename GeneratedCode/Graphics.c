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
#include "_GraphicsCanvas.h"
#include "_ResourcesBitmap.h"
#include "_ResourcesFont.h"
#include "Graphics.h"
#include "Resources.h"
#include "Views.h"

/* Strings for the language 'Default'. */
EW_CONST_STRING_PRAGMA static const unsigned short _StringsDefault0[] =
{
  0xFFFF, 0xFFFF, 0xC557, 0x0043, 0x0061, 0x006E, 0x0020, 0x006E, 0x006F, 0x0074,
  0x0020, 0x0072, 0x0065, 0x0073, 0x0069, 0x007A, 0x0065, 0x0020, 0x0065, 0x0078,
  0x0070, 0x006C, 0x0069, 0x0063, 0x0069, 0x0074, 0x006C, 0x0079, 0x0020, 0x0061,
  0x0074, 0x0074, 0x0061, 0x0063, 0x0068, 0x0065, 0x0064, 0x0020, 0x0067, 0x0072,
  0x0061, 0x0070, 0x0068, 0x0069, 0x0063, 0x0073, 0x0020, 0x0065, 0x006E, 0x0067,
  0x0069, 0x006E, 0x0065, 0x0020, 0x0062, 0x0069, 0x0074, 0x006D, 0x0061, 0x0070,
  0x0073, 0x0000, 0xC557, 0x004E, 0x006F, 0x0020, 0x0067, 0x0072, 0x0061, 0x0070,
  0x0068, 0x0069, 0x0063, 0x0073, 0x0020, 0x0065, 0x006E, 0x0067, 0x0069, 0x006E,
  0x0065, 0x0020, 0x0062, 0x0069, 0x0074, 0x006D, 0x0061, 0x0070, 0x0020, 0x0061,
  0x0074, 0x0074, 0x0061, 0x0063, 0x0068, 0x0065, 0x0064, 0x0020, 0x0074, 0x006F,
  0x0020, 0x0074, 0x0068, 0x0069, 0x0073, 0x0020, 0x0063, 0x0061, 0x006E, 0x0076,
  0x0061, 0x0073, 0x0000, 0xC557, 0x0054, 0x0068, 0x0065, 0x0020, 0x0063, 0x0061,
  0x006E, 0x0076, 0x0061, 0x0073, 0x0020, 0x0069, 0x0073, 0x0020, 0x0061, 0x006C,
  0x0072, 0x0065, 0x0061, 0x0064, 0x0079, 0x0020, 0x0069, 0x006E, 0x0069, 0x0074,
  0x0069, 0x0061, 0x006C, 0x0069, 0x007A, 0x0065, 0x0064, 0x0020, 0x0077, 0x0069,
  0x0074, 0x0068, 0x0020, 0x0061, 0x0020, 0x0067, 0x0072, 0x0061, 0x0070, 0x0068,
  0x0069, 0x0063, 0x0073, 0x0020, 0x0065, 0x006E, 0x0067, 0x0069, 0x006E, 0x0065,
  0x0020, 0x0062, 0x0069, 0x0074, 0x006D, 0x0061, 0x0070, 0x0000, 0xC557, 0x0053,
  0x0063, 0x0061, 0x006C, 0x0065, 0x0042, 0x0069, 0x0074, 0x006D, 0x0061, 0x0070,
  0x0028, 0x0029, 0x0020, 0x006F, 0x0070, 0x0065, 0x0072, 0x0061, 0x0074, 0x0069,
  0x006F, 0x006E, 0x0020, 0x0068, 0x0061, 0x0073, 0x0020, 0x0062, 0x0065, 0x0065,
  0x006E, 0x0020, 0x006F, 0x006D, 0x0069, 0x0074, 0x0074, 0x0065, 0x0064, 0x0020,
  0x0064, 0x0075, 0x0065, 0x0020, 0x0074, 0x006F, 0x0020, 0x0074, 0x0068, 0x0065,
  0x0020, 0x0072, 0x0065, 0x0073, 0x0075, 0x006C, 0x0074, 0x0069, 0x006E, 0x0067,
  0x0020, 0x0061, 0x0072, 0x0065, 0x0061, 0x0020, 0x0062, 0x0065, 0x0069, 0x006E,
  0x0067, 0x0020, 0x0077, 0x0069, 0x0064, 0x0065, 0x0072, 0x0020, 0x006F, 0x0072,
  0x0020, 0x0068, 0x0069, 0x0067, 0x0068, 0x0065, 0x0072, 0x0020, 0x0074, 0x0068,
  0x0061, 0x006E, 0x0020, 0x0034, 0x0030, 0x0036, 0x0039, 0x0020, 0x0070, 0x0069,
  0x0078, 0x0065, 0x006C, 0x002E, 0x0000, 0xC557, 0x0044, 0x0072, 0x0061, 0x0077,
  0x0054, 0x0068, 0x0069, 0x0063, 0x006B, 0x004C, 0x0069, 0x006E, 0x0065, 0x0028,
  0x0029, 0x0020, 0x006F, 0x0070, 0x0065, 0x0072, 0x0061, 0x0074, 0x0069, 0x006F,
  0x006E, 0x0020, 0x0068, 0x0061, 0x0073, 0x0020, 0x0062, 0x0065, 0x0065, 0x006E,
  0x0020, 0x006F, 0x006D, 0x0069, 0x0074, 0x0074, 0x0065, 0x0064, 0x0020, 0x0064,
  0x0075, 0x0065, 0x0020, 0x0074, 0x006F, 0x0020, 0x0074, 0x0068, 0x0065, 0x0020,
  0x0072, 0x0065, 0x0073, 0x0075, 0x006C, 0x0074, 0x0069, 0x006E, 0x0067, 0x0020,
  0x0061, 0x0072, 0x0065, 0x0061, 0x0020, 0x0062, 0x0065, 0x0069, 0x006E, 0x0067,
  0x0020, 0x0077, 0x0069, 0x0064, 0x0065, 0x0072, 0x0020, 0x006F, 0x0072, 0x0020,
  0x0068, 0x0069, 0x0067, 0x0068, 0x0065, 0x0072, 0x0020, 0x0074, 0x0068, 0x0061,
  0x006E, 0x0020, 0x0034, 0x0030, 0x0036, 0x0039, 0x0020, 0x0070, 0x0069, 0x0078,
  0x0065, 0x006C, 0x002E, 0x0000
};

/* Constant values used in this 'C' module only. */
static const XPoint _Const0000 = { 0, 0 };
static const XStringRes _Const0001 = { _StringsDefault0, 0x0003 };
static const XColor _Const0002 = { 0x00, 0x00, 0x00, 0x00 };
static const XRect _Const0003 = {{ 0, 0 }, { 0, 0 }};
static const XStringRes _Const0004 = { _StringsDefault0, 0x003F };
static const XStringRes _Const0005 = { _StringsDefault0, 0x0072 };
static const XStringRes _Const0006 = { _StringsDefault0, 0x00B3 };
static const XStringRes _Const0007 = { _StringsDefault0, 0x011E };
static const XPoint _Const0008 = { 1, 1 };
static const XPoint _Const0009 = { 2, 2 };

/* Initializer for the class 'Graphics::Canvas' */
void GraphicsCanvas__Init( GraphicsCanvas _this, XObject aLink, XHandle aArg )
{
  /* At first initialize the super class ... */
  ResourcesBitmap__Init( &_this->_.Super, aLink, aArg );

  /* Allow the Immediate Garbage Collection to evalute the members of this class. */
  _this->_.XObject._.GCT = EW_CLASS_GCT( GraphicsCanvas );

  /* Setup the VMT pointer */
  _this->_.VMT = EW_CLASS( GraphicsCanvas );

  /* Call the user defined constructor */
  GraphicsCanvas_Init( _this, aArg );
}

/* Re-Initializer for the class 'Graphics::Canvas' */
void GraphicsCanvas__ReInit( GraphicsCanvas _this )
{
  /* At first re-initialize the super class ... */
  ResourcesBitmap__ReInit( &_this->_.Super );
}

/* Finalizer method for the class 'Graphics::Canvas' */
void GraphicsCanvas__Done( GraphicsCanvas _this )
{
  /* Call the user defined destructor of the class */
  GraphicsCanvas_Done( _this );

  /* Finalize this class */
  _this->_.Super._.VMT = EW_CLASS( ResourcesBitmap );

  /* Don't forget to deinitialize the super class ... */
  ResourcesBitmap__Done( &_this->_.Super );
}

/* 'C' function for method : 'Graphics::Canvas.Done()' */
void GraphicsCanvas_Done( GraphicsCanvas _this )
{
  if ( _this->attached )
    GraphicsCanvas_DetachBitmap( _this );
}

/* 'C' function for method : 'Graphics::Canvas.Init()' */
void GraphicsCanvas_Init( GraphicsCanvas _this, XHandle aArg )
{
  /* Dummy expressions to avoid the 'C' warning 'unused argument'. */
  EW_UNUSED_ARG( aArg );

  _this->Super1.Mutable = 1;
}

/* 'C' function for method : 'Graphics::Canvas.OnSetFrameSize()' */
void GraphicsCanvas_OnSetFrameSize( GraphicsCanvas _this, XPoint value )
{
  XHandle handle;

  if (( value.X <= 0 ) || ( value.Y <= 0 ))
    value = _Const0000;

  if ( !EwCompPoint( value, _this->Super1.FrameSize ))
    return;

  if ( _this->attached )
  {
    EwThrow( EwLoadString( &_Const0001 ));
    return;
  }

  _this->Super1.FrameSize = value;
  _this->Super1.Animated = (XBool)(((( _this->Super1.FrameSize.X > 0 ) && ( _this->Super1.FrameSize.Y 
  > 0 )) && ( _this->Super1.FrameDelay > 0 )) && ( _this->Super1.NoOfFrames > 1 ));

  if ( _this->Super1.bitmap == 0 )
    return;

  handle = _this->Super1.bitmap;
  EwFreeBitmap((XBitmap*)handle );
  _this->Super1.bitmap = 0;
}

/* 'C' function for method : 'Graphics::Canvas.Update()' */
void GraphicsCanvas_Update( GraphicsCanvas _this )
{
  if ((( _this->Super1.bitmap == 0 ) && ( _this->Super1.FrameSize.X > 0 )) && ( 
      _this->Super1.FrameSize.Y > 0 ))
  {
    XPoint frameSize = _this->Super1.FrameSize;
    XInt32 noOfFrames = _this->Super1.NoOfFrames;
    XInt32 frameDelay = _this->Super1.FrameDelay;
    XHandle handle = 0;
    {
      handle = (XHandle)EwCreateBitmap( EW_PIXEL_FORMAT_NATIVE, frameSize, 
                                        frameDelay, noOfFrames );
    }
    _this->Super1.bitmap = handle;

    if ( _this->Super1.bitmap == 0 )
    {
      _this->Super1.FrameSize = _Const0000;
      _this->Super1.FrameDelay = 0;
      _this->Super1.NoOfFrames = 1;
    }

    _this->InvalidArea = EwNewRect2Point( _Const0000, _this->Super1.FrameSize );

    if ( !!_this->Super1.bitmap )
    {
      XInt32 dstFrameNr = _this->DstFrameNr;

      for ( _this->DstFrameNr = _this->Super1.NoOfFrames - 1; _this->DstFrameNr 
           >= 0; _this->DstFrameNr-- )
        GraphicsCanvas_FillRectangle( _this, _this->InvalidArea, _this->InvalidArea, 
        _Const0002, _Const0002, _Const0002, _Const0002, 0 );

      _this->DstFrameNr = dstFrameNr;
    }
  }

  if ( !EwIsRectEmpty( _this->InvalidArea ))
    _this->InvalidArea = _Const0003;
}

/* The method DetachBitmap() exists for the integration purpose with the underlying 
   target system. You will never need to invoke this method directly from your GUI 
   application. The method is invoked after the screen update is finished and the 
   canvas object should be detached from the framebuffer. */
GraphicsCanvas GraphicsCanvas_DetachBitmap( GraphicsCanvas _this )
{
  if ( !_this->attached )
  {
    EwThrow( EwLoadString( &_Const0004 ));
    return 0;
  }

  _this->Super1.bitmap = 0;
  _this->attached = 0;
  _this->Super1.FrameSize = _Const0000;
  _this->Super1.FrameDelay = 0;
  _this->Super1.NoOfFrames = 1;
  _this->Super1.Animated = 0;
  return _this;
}

/* Wrapper function for the non virtual method : 'Graphics::Canvas.DetachBitmap()' */
GraphicsCanvas GraphicsCanvas__DetachBitmap( void* _this )
{
  return GraphicsCanvas_DetachBitmap((GraphicsCanvas)_this );
}

/* The method AttachBitmap() exists for the integration purpose with the underlying 
   target system. You will never need to invoke this method directly from your GUI 
   application. The method is invoked at the beginning of the screen update.
   The method connects the canvas object with the framebuffer. */
GraphicsCanvas GraphicsCanvas_AttachBitmap( GraphicsCanvas _this, XHandle aBitmap )
{
  XInt32 noOfFrames;
  XPoint frameSize;
  XInt32 frameDelay;

  if ( _this->Super1.bitmap != 0 )
  {
    EwThrow( EwLoadString( &_Const0005 ));
    return 0;
  }

  if ( aBitmap == 0 )
    return _this;

  _this->Super1.bitmap = aBitmap;
  _this->attached = 1;
  noOfFrames = 1;
  frameSize = _Const0000;
  frameDelay = 0;
  {
    XBitmap* bmp = (XBitmap*)aBitmap;

    noOfFrames = bmp->NoOfVirtFrames;
    frameSize  = bmp->FrameSize;
    frameDelay = bmp->FrameDelay;
  }
  _this->Super1.NoOfFrames = noOfFrames;
  _this->Super1.FrameSize = frameSize;
  _this->Super1.FrameDelay = frameDelay;
  _this->Super1.Animated = (XBool)(( _this->Super1.FrameDelay > 0 ) && ( _this->Super1.NoOfFrames 
  > 1 ));
  return _this;
}

/* Wrapper function for the non virtual method : 'Graphics::Canvas.AttachBitmap()' */
GraphicsCanvas GraphicsCanvas__AttachBitmap( void* _this, XHandle aBitmap )
{
  return GraphicsCanvas_AttachBitmap((GraphicsCanvas)_this, aBitmap );
}

/* The method DrawText() draws the text row passed in the parameter aString into 
   the canvas. The font to draw the text is passed in the parameter aFont. The parameter 
   aOffset determines within aString the sign to start the drawing operation. If 
   aOffset is zero, the operation starts with the first sign. The parameter aCount 
   determines the max. number of following sigs to draw. If aCount is -1, all signs 
   until the end of the string are drawn. 
   The area to draw the text is determined by the parameter aDstRect. The parameter 
   aOrientation controls the rotation of the text. The parameter aSrcPos determines 
   the base line position of the text relative to a corner of aDstRect, which by 
   taking in account the specified text orientation serves as the origin for the 
   draw operation. For example, if the parameter aOrientation is Views::Orientation.Rotated_270, 
   the text is drawn aSrcPos pixel relative to the bottom-right corner of aDstRect. 
   The parameter aMinWidth determines the min. width in pixel of the drawn text 
   row regardless of the specified rotation. If necessary the space signs within 
   the text will be stretched to fill this area. The parameters aColorTL, aColorTR, 
   aColorBL, aColorBR determine the colors at the corresponding corners of the aDstRect 
   area.
   The parameter aClip limits the drawing operation. Pixel lying outside this area 
   remain unchanged. The last aBlend parameter controls the mode how drawn pixel 
   are combined with the pixel already existing in the destination bitmap. If aBlend 
   is 'true', the drawn pixel are alpha-blended with the background, otherwise the 
   drawn pixel will overwrite the old content. */
void GraphicsCanvas_DrawText( GraphicsCanvas _this, XRect aClip, ResourcesFont aFont, 
  XString aString, XInt32 aOffset, XInt32 aCount, XRect aDstRect, XPoint aSrcPos, 
  XInt32 aMinWidth, XEnum aOrientation, XColor aColorTL, XColor aColorTR, XColor 
  aColorBR, XColor aColorBL, XBool aBlend )
{
  XInt32 orient;
  XInt32 dstFrameNo;
  XHandle dstBitmap;
  XHandle srcFont;
  XRect tempRect;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  if ( aOffset < 0 )
    aOffset = 0;

  if ((( aFont == 0 ) || ( aFont->font == 0 )) || (( aOffset > 0 ) && ( aOffset 
      >= EwGetStringLength( aString ))))
    return;

  orient = 0;

  if ( aOrientation == ViewsOrientationRotated_90 )
    orient = 90;
  else
    if ( aOrientation == ViewsOrientationRotated_180 )
      orient = 180;
    else
      if ( aOrientation == ViewsOrientationRotated_270 )
        orient = 270;

  dstFrameNo = _this->DstFrameNr;
  dstBitmap = _this->Super1.bitmap;
  srcFont = aFont->font;
  tempRect = aClip;
  {
    EwDrawText((XBitmap*)dstBitmap, (XFont*)srcFont, aString + aOffset, aCount,
                dstFrameNo, tempRect, aDstRect, aSrcPos, aMinWidth, orient, aColorTL, aColorTR,
                aColorBR, aColorBL, aBlend );
  }
}

/* The method ScaleBitmap() copies and scales an area of a aBitmap into the canvas. 
   The bitmap is specified in the parameter aBitmap and the desired area to copy 
   in aSrcRect. In case of a multi-frame bitmap the desired frame can be selected 
   in the parameter aFrameNr.
   The destination area in canvas is determined by the parameter aDstRect. The parameters 
   aColorTL, aColorTR, aColorBL, aColorBR determine the colors or opacities at the 
   corresponding corners of the aDstRect area.
   The parameter aClip limits the drawing operation. Pixel lying outside this area 
   remain unchanged. The aBlend parameter controls the mode how drawn pixel are 
   combined with the pixel already existing in the destination bitmap. If aBlend 
   is 'true', the drawn pixel are alpha-blended with the background, otherwise the 
   drawn pixel will overwrite the old content. The last parameter aFilter controls 
   the bi-linear filter. If aFilter is 'true', the source bitmap pixel will be bi-linear 
   filtered in order to get better output. */
void GraphicsCanvas_ScaleBitmap( GraphicsCanvas _this, XRect aClip, ResourcesBitmap 
  aBitmap, XInt32 aFrameNr, XRect aDstRect, XRect aSrcRect, XColor aColorTL, XColor 
  aColorTR, XColor aColorBR, XColor aColorBL, XBool aBlend, XBool aFilter )
{
  XFloat top;
  XFloat left;
  XFloat right;
  XFloat bottom;
  XHandle dstBitmap;
  XHandle srcBitmap;
  XInt32 dstFrameNo;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  if (((( aBitmap == 0 ) || ( aBitmap->bitmap == 0 )) || ( aFrameNr < 0 )) || ( 
      aFrameNr >= aBitmap->NoOfFrames ))
    return;

  top = (XFloat)aDstRect.Point1.Y;
  left = (XFloat)aDstRect.Point1.X;
  right = (XFloat)aDstRect.Point2.X;
  bottom = (XFloat)aDstRect.Point2.Y;

  if ((((( right - left ) > 4096.000000f ) || (( right - left ) < -4096.000000f )) 
      || (( bottom - top ) > 4096.000000f )) || (( bottom - top ) < -4096.000000f ))
  {
    EwTrace( "%s", EwLoadString( &_Const0006 ));
    return;
  }

  dstBitmap = _this->Super1.bitmap;
  srcBitmap = aBitmap->bitmap;
  dstFrameNo = _this->DstFrameNr;
  {
    EwWarpBitmap((XBitmap*)dstBitmap, (XBitmap*)srcBitmap, dstFrameNo, aFrameNr, aClip,
                  left,  top,    1.0f, right, top,    1.0f,
                  right, bottom, 1.0f, left,  bottom, 1.0f,
                  aSrcRect, aColorTL, aColorTR, aColorBR, aColorBL, aBlend, aFilter );
  }
}

/* The method CopyBitmap() copies an area of a aBitmap into the canvas. The bitmap 
   is specified in the parameter aBitmap. In case of a multi-frame bitmap the desired 
   frame can be selected in the parameter aFrameNr.
   The area to copy the bitmap is determined by the parameter aDstRect. The optional 
   aSrcPos parameter determines a displacement of the bitmap within this aDstRect 
   area. The parameters aColorTL, aColorTR, aColorBL, aColorBR determine the colors 
   or opacities at the corresponding corners of the aDstRect area.
   The parameter aClip limits the drawing operation. Pixel lying outside this area 
   remain unchanged. The last aBlend parameter controls the mode how drawn pixel 
   are combined with the pixel already existing in the destination bitmap. If aBlend 
   is 'true', the drawn pixel are alpha-blended with the background, otherwise the 
   drawn pixel will overwrite the old content. */
void GraphicsCanvas_CopyBitmap( GraphicsCanvas _this, XRect aClip, ResourcesBitmap 
  aBitmap, XInt32 aFrameNr, XRect aDstRect, XPoint aSrcPos, XColor aColorTL, XColor 
  aColorTR, XColor aColorBR, XColor aColorBL, XBool aBlend )
{
  XHandle dstBitmap;
  XHandle srcBitmap;
  XInt32 dstFrameNr;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  if (((( aBitmap == 0 ) || ( aBitmap->bitmap == 0 )) || ( aFrameNr < 0 )) || ( 
      aFrameNr >= aBitmap->NoOfFrames ))
    return;

  dstBitmap = _this->Super1.bitmap;
  srcBitmap = aBitmap->bitmap;
  dstFrameNr = _this->DstFrameNr;
  {
    EwCopyBitmap((XBitmap*)dstBitmap, (XBitmap*)srcBitmap, dstFrameNr, aFrameNr,
                  aClip, aDstRect, aSrcPos, aColorTL, aColorTR, aColorBR, aColorBL,
                  aBlend );
  }
}

/* The method FillRectangle() fills an area of canvas. The area is determined by 
   the parameter aDstRect. The parameters aColorTL, aColorTR, aColorBL, aColorBR 
   determine the colors at the corresponding corners of the aDstRect area.
   The parameter aClip limits the drawing operation. Pixel lying outside this area 
   remain unchanged. The last aBlend parameter controls the mode how drawn pixel 
   are combined with the pixel already existing in the destination bitmap. If aBlend 
   is 'true', the drawn pixel are alpha-blended with the background, otherwise the 
   drawn pixel will overwrite the old content. */
void GraphicsCanvas_FillRectangle( GraphicsCanvas _this, XRect aClip, XRect aDstRect, 
  XColor aColorTL, XColor aColorTR, XColor aColorBR, XColor aColorBL, XBool aBlend )
{
  XHandle dstBitmap;
  XInt32 dstFrameNo;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  dstBitmap = _this->Super1.bitmap;
  dstFrameNo = _this->DstFrameNr;
  {
    EwFillRectangle((XBitmap*)dstBitmap, dstFrameNo, aClip, aDstRect, aColorTL, aColorTR,
                     aColorBR, aColorBL, aBlend );
  }
}

/* 'C' function for method : 'Graphics::Canvas.DrawThickLine()' */
void GraphicsCanvas_DrawThickLine( GraphicsCanvas _this, XRect aClip, XPoint aDstPos1, 
  XPoint aDstPos2, XInt32 aWidth1, XInt32 aWidth2, XColor aColor1, XColor aColor2, 
  XBool aBlend )
{
  XInt32 w1;
  XInt32 w2;
  XPoint p1;
  XPoint p2;
  XFloat fw1;
  XFloat fw2;
  XFloat fp1X;
  XFloat fp1Y;
  XFloat fp2X;
  XFloat fp2Y;
  XFloat dirX;
  XFloat dirY;
  XFloat len;
  XFloat x1;
  XFloat y1;
  XFloat x2;
  XFloat y2;
  XFloat x3;
  XFloat y3;
  XFloat x4;
  XFloat y4;
  ResourcesBitmap opaqueBitmap;
  XRect srcRect;
  XHandle dstBitmap;
  XInt32 dstFrameNo;
  XHandle srcBitmap;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  w1 = aWidth1;
  w2 = aWidth2;
  p1 = aDstPos1;
  p2 = aDstPos2;

  if ( !EwCompPoint( p1, p2 ))
    return;

  if ( w1 <= 1 )
    w1 = 1;

  if ( w2 <= 1 )
    w2 = 1;

  fw1 = (XFloat)w1 / 2.000000f;
  fw2 = (XFloat)w2 / 2.000000f;
  fp1X = (XFloat)p1.X;
  fp1Y = (XFloat)p1.Y;
  fp2X = (XFloat)p2.X;
  fp2Y = (XFloat)p2.Y;
  dirX = fp2X - fp1X;
  dirY = fp2Y - fp1Y;
  len = EwMathSqrt(( dirX * dirX ) + ( dirY * dirY ));

  if ((( w1 > 4096 ) || ( w2 > 4096 )) || ( len > 4096.000000f ))
  {
    EwTrace( "%s", EwLoadString( &_Const0007 ));
    return;
  }

  dirX = dirX / len;
  dirY = dirY / len;
  x1 = fp1X + ( dirY * fw1 );
  y1 = fp1Y - ( dirX * fw1 );
  x2 = fp2X + ( dirY * fw2 );
  y2 = fp2Y - ( dirX * fw2 );
  x3 = fp2X - ( dirY * fw2 );
  y3 = fp2Y + ( dirX * fw2 );
  x4 = fp1X - ( dirY * fw1 );
  y4 = fp1Y + ( dirX * fw1 );
  opaqueBitmap = EwLoadResource( &ResourcesWhiteBitmapStripe, ResourcesBitmap );
  srcRect = EwNewRect2Point( _Const0008, EwMovePointNeg( opaqueBitmap->FrameSize, 
  _Const0009 ));
  dstBitmap = _this->Super1.bitmap;
  dstFrameNo = _this->DstFrameNr;
  srcBitmap = opaqueBitmap->bitmap;
  {
    EwWarpBitmap((XBitmap*)dstBitmap, (XBitmap*)srcBitmap, dstFrameNo, 0, aClip,
                  x1, y1, 1.0, x2, y2, 1.0, x3, y3, 1.0, x4, y4, 1.0, 
                  srcRect, aColor1, aColor2, aColor2, aColor1, aBlend, 0 );
  }
}

/* The method DrawLine() draws a line into the canvas. The line will be drawn from 
   the aDstPos1 point to the aDstPos2 point with the given colors aColor1 and aColor2.
   The parameter aClip limits the drawing operation. Pixel lying outside this area 
   remain unchanged. The last aBlend parameter controls the mode how drawn pixel 
   are combined with the pixel already existing in the destination bitmap. If aBlend 
   is 'true', the drawn pixel are alpha-blended with the background, otherwise the 
   drawn pixel will overwrite the old content. */
void GraphicsCanvas_DrawLine( GraphicsCanvas _this, XRect aClip, XPoint aDstPos1, 
  XPoint aDstPos2, XColor aColor1, XColor aColor2, XBool aBlend )
{
  XHandle dstBitmap;
  XInt32 dstFrameNo;

  if ( _this->Super1.bitmap == 0 )
    ResourcesBitmap__Update( _this );

  if ( _this->Super1.bitmap == 0 )
    return;

  dstBitmap = _this->Super1.bitmap;
  dstFrameNo = _this->DstFrameNr;
  {
    EwDrawLine((XBitmap*)dstBitmap, dstFrameNo, aClip, aDstPos1, aDstPos2, aColor1,
                aColor2, aBlend );
  }
}

/* Variants derived from the class : 'Graphics::Canvas' */
EW_DEFINE_CLASS_VARIANTS( GraphicsCanvas )
EW_END_OF_CLASS_VARIANTS( GraphicsCanvas )

/* Virtual Method Table (VMT) for the class : 'Graphics::Canvas' */
EW_DEFINE_CLASS( GraphicsCanvas, ResourcesBitmap, _.VMT, _.VMT, _.VMT, _.VMT, _.VMT, 
                 _.VMT, "Graphics::Canvas" )
  GraphicsCanvas_OnSetFrameSize,
  GraphicsCanvas_Update,
EW_END_OF_CLASS( GraphicsCanvas )

/* Embedded Wizard */
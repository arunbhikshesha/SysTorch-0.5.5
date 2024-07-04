/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This software and related documentation ("Software") are intellectual
* property owned by TARA Systems and are copyright of TARA Systems.
* Any modification, copying, reproduction or redistribution of the Software in
* whole or in part by any means not in accordance with the End-User License
* Agreement for Embedded Wizard is expressly prohibited. The removal of this
* preamble is expressly prohibited.
* 
********************************************************************************
*
* DESCRIPTION:
*   This module is a part of the Embedded Wizard Runtime Environment EWRTE. 
*   This environment consists of functions, type definitions and 'C' macros
*   used inside the automatic generated 'C' code.
*
*   The module 'ewcolor.c' contains a set of EWRTE functions purpose-built for 
*   operations with the Chora datatype 'color'. 
*
* SUBROUTINES:
*   EwNewColor
*   EwMinusColor
*   EwPlusColor
*   EwBlendColor
*   EwBlendColorInt
*   EwBlendColorUInt
*   EwSetColorRed
*   EwSetColorGreen
*   EwSetColorBlue 
*   EwSetColorAlpha
*   EwCompColor
*   EwIsColorNull
*   EwGetVariantOfColor
*
*******************************************************************************/

#include "ewrte.h"


/* EwLangId contains the ID of the currently selected language. LangId will be
   used to find out the correct variant of a color constant in the function 
   EwGetVariantOfColor() */
extern int EwLangId;


/*******************************************************************************
* FUNCTION:
*   EwNewColor
*
* DESCRIPTION:
*   The function EwNewColor() initializes a new color with the given values
*   returns the initialized color. 
*
*   EwNewColor() implements the Chora instant constructor: 
*   'color(aRed,aGreen,aBlue)' and 'color(aRed,aGreen,aBlue,aAlpha)'.
*
* ARGUMENTS:
*   aRed   - Red component of the color.
*   aGreen - Green component of the color.
*   aBlue  - Blue component of the color.
*   aAlpha - Alpha component of the color.
*
* RETURN VALUE:
*   Returns the initialized color.
*
*******************************************************************************/
XColor EwNewColor( XUInt8 aRed, XUInt8 aGreen, XUInt8 aBlue, XUInt8 aAlpha )
{
  XColor c;

  c.Red   = aRed;
  c.Green = aGreen;
  c.Blue  = aBlue;
  c.Alpha = aAlpha;

  return c;
}


/*******************************************************************************
* FUNCTION:
*   EwMinusColor
*
* DESCRIPTION:
*   The function EwMinusColor() subtracts two colors aColor1 - aColor2. The 
*   substraction will be done for each color component separately. In the case
*   the substraction results in a value < 0, EwMinusColor() adjusts the value
*   automatically to 0 (zero).
*
*   EwMinusColor() implements the Chora instant operator: 'color - color'
*
* ARGUMENTS:
*   aColor1 - The color to substract.
*   aColor2 - The color to be substracted.
*
* RETURN VALUE:
*   Returns a new color containing the substraction of aColor1 and aColor2.
*
*******************************************************************************/
XColor EwMinusColor( XColor aColor1, XColor aColor2 )
{
  int r = aColor1.Red   - aColor2.Red;
  int g = aColor1.Green - aColor2.Green;
  int b = aColor1.Blue  - aColor2.Blue;
  int a = aColor1.Alpha - aColor2.Alpha;

  /* Saturation */
  if ( r < 0 ) r = 0;
  if ( g < 0 ) g = 0;
  if ( b < 0 ) b = 0;
  if ( a < 0 ) a = 0;

  aColor1.Red   = (XUInt8)r;
  aColor1.Green = (XUInt8)g;
  aColor1.Blue  = (XUInt8)b;
  aColor1.Alpha = (XUInt8)a;

  return aColor1;
}


/*******************************************************************************
* FUNCTION:
*   EwPlusColor
*
* DESCRIPTION:
*   The function EwPlusColor() adds two colors aColor1 + aColor2. The addition
*   will be done for each color component separately. In the case the addition
*   results in a value > 255, EwPlusColor() adjusts the value automatically 
*   to 255.
*
*   EwPlusColor() implements the Chora instant operator: 'color + color'
*
* ARGUMENTS:
*   aColor1 - The first color.
*   aColor2 - The second color.
*
* RETURN VALUE:
*   Returns a new color containing the addition of aColor1 and aColor2.
*
*******************************************************************************/
XColor EwPlusColor( XColor aColor1, XColor aColor2 )
{
  int r = aColor1.Red   + aColor2.Red;
  int g = aColor1.Green + aColor2.Green;
  int b = aColor1.Blue  + aColor2.Blue;
  int a = aColor1.Alpha + aColor2.Alpha;

  /* Saturation */
  if ( r > 255 ) r = 255;
  if ( g > 255 ) g = 255;
  if ( b > 255 ) b = 255;
  if ( a > 255 ) a = 255;

  aColor1.Red   = (XUInt8)r;
  aColor1.Green = (XUInt8)g;
  aColor1.Blue  = (XUInt8)b;
  aColor1.Alpha = (XUInt8)a;

  return aColor1;
}


/*******************************************************************************
* FUNCTION:
*   EwBlendColor
*
* DESCRIPTION:
*   The function EwBlendColor() implements an alphablending algorithm to blend
*   one color aColor1 with the second color aColor2. The Alpha component of
*   aColor1 decides about the intensity of the alpha blending. If the Alpha
*   component of aColor1 is 0 (zero), the function returns an unchanged value
*   aColor2. If the Alpha component of aColor1 == 255, the function returns the
*   value aColor1. In all other cases, the function returns a mix-color of the
*   both colors.
*
*   EwBlendColor() implements the Chora instant operator: 'color * color'
*
* ARGUMENTS:
*   aColor1 - The first color.
*   aColor2 - The second color. 
*
* RETURN VALUE:
*   Returns a mix-color of the both colors.
*
*******************************************************************************/
XColor EwBlendColor( XColor aColor1, XColor aColor2 )
{
  int r  = aColor2.Red;
  int g  = aColor2.Green;
  int b  = aColor2.Blue;
  int a  = aColor2.Alpha;
  int na = 255 - a;

  r = (( r * a ) + ( aColor1.Red   * na )) / 255;
  g = (( g * a ) + ( aColor1.Green * na )) / 255;
  b = (( b * a ) + ( aColor1.Blue  * na )) / 255;
  a = 255 - (( na * ( 255 - aColor1.Alpha )) / 255 );

  /* Saturation */
  if ( r > 255 ) r = 255;
  if ( g > 255 ) g = 255;
  if ( b > 255 ) b = 255;
  if ( a > 255 ) a = 255;

  aColor2.Red   = (XUInt8)r;
  aColor2.Green = (XUInt8)g;
  aColor2.Blue  = (XUInt8)b;
  aColor2.Alpha = (XUInt8)a;

  return aColor2;
}


/*******************************************************************************
* FUNCTION:
*   EwBlendColorInt
*
* DESCRIPTION:
*   The function EwBlendColorInt() implements an alphablending algorithm to 
*   blend the color aColor with the value aAlpha. This function applies the
*   alpha blending algorithm on all 4 color components of the given color.
*
*   EwBlendColorInt() implements the Chora instant operator: 'color * int32'
*   and 'int32 * color'.
*
* ARGUMENTS:
*   aColor - The color to alphablend with aAlpha.
*   aAlpha - The blending factor in range from 0 .. 255.
*
* RETURN VALUE:
*   Returns a mix-color of the color aColor and the value aAlpha.
*
*******************************************************************************/
XColor EwBlendColorInt( XColor aColor, XInt32 aAlpha )
{
  /* Is aAlpha valid? */
  if ( aAlpha <   0 ) aAlpha = 0;
  if ( aAlpha > 255 ) aAlpha = 255;

  aColor.Red   = (XUInt8)(( aColor.Red   * aAlpha ) / 255 );
  aColor.Green = (XUInt8)(( aColor.Green * aAlpha ) / 255 );
  aColor.Blue  = (XUInt8)(( aColor.Blue  * aAlpha ) / 255 );
  aColor.Alpha = (XUInt8)(( aColor.Alpha * aAlpha ) / 255 );

  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwBlendColorUInt
*
* DESCRIPTION:
*   The function EwBlendColorInt() implements an alphablending algorithm to 
*   blend the color aColor with the value aAlpha. This function applies the
*   alpha blending algorithm on all 4 color components of the given color.
*
*   EwBlendColorUInt() implements the Chora instant operator: 'color * uint32'
*   and 'uint32 * color'.
*
* ARGUMENTS:
*   aColor - The color to alphablend with aAlpha.
*   aAlpha - The blending factor in range from 0 .. 255.
*
* RETURN VALUE:
*   Returns a mix-color of the color aColor and the value aAlpha.
*
*******************************************************************************/
XColor EwBlendColorUInt( XColor aColor, XUInt32 aAlpha )
{
  /* Is aAlpha valid? */
  if ( aAlpha > 255 ) aAlpha = 255;

  aColor.Red   = (XUInt8)(( aColor.Red   * aAlpha ) / 255 );
  aColor.Green = (XUInt8)(( aColor.Green * aAlpha ) / 255 );
  aColor.Blue  = (XUInt8)(( aColor.Blue  * aAlpha ) / 255 );
  aColor.Alpha = (XUInt8)(( aColor.Alpha * aAlpha ) / 255 );

  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwSetColorRed
*
* DESCRIPTION:
*   The function EwSetColorRed() changes the Red component of the given color 
*   to a new value aRed.
*
*   EwSetColorRed() implements the write access to the Chora instant property 
*   'color.red'.
*
* ARGUMENTS:
*   aColor - The color to change the Red component.
*   aRed   - The new value for the Red component.
*
* RETURN VALUE:
*   Returns a new color with the changed Red component.
*
*******************************************************************************/
XColor EwSetColorRed( XColor aColor, XUInt8 aRed )
{
  aColor.Red = aRed;
  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwSetColorGreen
*
* DESCRIPTION:
*   The function EwSetColorGreen() changes the Green component of the given 
*   color to a new value aGreen.
*
*   EwSetColorGreen() implements the write access to the Chora instant property 
*   'color.green'.
*
* ARGUMENTS:
*   aColor - The color to change the Green component.
*   aGreen - The new value for the Green component.
*
* RETURN VALUE:
*   Returns a new color with the changed Green component.
*
*******************************************************************************/
XColor EwSetColorGreen( XColor aColor, XUInt8 aGreen )
{
  aColor.Green = aGreen;
  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwSetColorBlue 
*
* DESCRIPTION:
*   The function EwSetColorBlue() changes the Blue component of the given color 
*   to a new value aBlue.
*
*   EwSetColorBlue() implements the write access to the Chora instant property 
*   'color.blue'.
*
* ARGUMENTS:
*   aColor - The color to change the Blue component.
*   aBlue  - The new value for the Blue component.
*
* RETURN VALUE:
*   Returns a new color with the changed Blue component.
*
*******************************************************************************/
XColor EwSetColorBlue( XColor aColor, XUInt8 aBlue )
{
  aColor.Blue = aBlue;
  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwSetColorAlpha
*
* DESCRIPTION:
*   The function EwSetColorAlpha() changes the Alpha component of the given 
*   color to a new value aAlpha.
*
*   EwSetColorAlpha() implements the write access to the Chora instant property 
*   'color.alpha'.
*
* ARGUMENTS:
*   aColor - The color to change the Alpha component.
*   aAlpha - The new value for the Alpha component.
*
* RETURN VALUE:
*   Returns a new color with the changed Alpha component.
*
*******************************************************************************/
XColor EwSetColorAlpha( XColor aColor, XUInt8 aAlpha )
{
  aColor.Alpha = aAlpha;
  return aColor;
}


/*******************************************************************************
* FUNCTION:
*   EwCompColor
*
* DESCRIPTION:
*   The function EwCompColor() compares the both given colors aColor1 and 
*   aColor2 and returns a value == 0 if the colors are identical. Otherwise
*   the function returns a value != 0.
*
*   EwCompColor() implements the Chora instant operators: 'color == color' and
*   'color != color'.
*
* ARGUMENTS:
*   aColor1 - The first color.
*   aColor2 - The second color.
*
* RETURN VALUE:
*   Returns zero if the both colors are equal.
*
*******************************************************************************/
int EwCompColor( XColor aColor1, XColor aColor2 )
{
  if ( aColor1.Red   != aColor2.Red   ) return 1;
  if ( aColor1.Green != aColor2.Green ) return 1;
  if ( aColor1.Blue  != aColor2.Blue  ) return 1;
  if ( aColor1.Alpha != aColor2.Alpha ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsColorNull
*
* DESCRIPTION:
*   The function EwIsColorNull() returns != 0 if the given color aColor does
*   contain the value #00000000. Otherwise the function returns 0.
*
* ARGUMENTS:
*   aColor - Color to verify.
*
* RETURN VALUE:
*   Returns != 0 if the color is #00000000.
*
*******************************************************************************/
XBool EwIsColorNull( XColor aColor )
{
  return !aColor.Alpha && !aColor.Blue && !aColor.Green && !aColor.Red;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfColor
*
* DESCRIPTION:
*   The function EwGetVariantOfColor() will be called to determinate a value
*   of a multilingual/multivariant constant depending on the currently selected
*   language and the styles set.
*
*   If the currently selected language could not be found in the multilingual
*   constant, the function returns the value corresponding to the default 
*   language (LangId == 0). In case of a multivariant constant, the function
*   evaluates the variants in order to find one, which fits the styles currently
*   active in the styles set.
*
* ARGUMENTS:
*   aVariants - A pointer to the constant containing multiple color values.
*
* RETURN VALUE:
*   Returns the determinated color value.
*
*******************************************************************************/
XColor EwGetVariantOfColor( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfColor )->Value;
}


/* pba, msy */

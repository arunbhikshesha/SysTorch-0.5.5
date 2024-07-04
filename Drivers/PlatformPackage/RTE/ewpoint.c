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
*   The module 'ewpoint.c' contains a set of EWRTE functions purpose-built for 
*   operations with the Chora datatype 'point'. 
*
* SUBROUTINES:
*   EwNewPoint
*   EwCompPoint
*   EwIsPointNull
*   EwMovePointNeg
*   EwMovePointPos
*   EwNegPoint
*   EwSetPointX
*   EwSetPointY
*   EwGetVariantOfPoint
*
*******************************************************************************/

#include "ewrte.h"


/* EwLangId contains the ID of the currently selected language. LangId will be
   used to find out the correct variant of a point constant in the function 
   EwGetVariantOfPoint() */
extern int EwLangId;


/*******************************************************************************
* FUNCTION:
*   EwNewPoint
*
* DESCRIPTION:
*   The function EwNewPoint() initializes a new point with the given aX and aY 
*   values and returns the initialized point.
*
*   EwNewPoint() implements the Chora instant constructor: 'point(aX,aY)'.
*
* ARGUMENTS:
*   aX - The X-coordinate of the point.
*   aY - The Y-coordinate of the point.
*
* RETURN VALUE:
*   Returns the initialized point.
*
*******************************************************************************/
XPoint EwNewPoint( XInt32 aX, XInt32 aY )
{
  XPoint p;

  p.X = aX;
  p.Y = aY;

  return p;
}


/*******************************************************************************
* FUNCTION:
*   EwCompPoint
*
* DESCRIPTION:
*   The function EwCompPoint() compares the both given points aPoint1 and 
*   aPoint2 and returns a value == 0 if the points are identical. Otherwise
*   the function returns a value != 0.
*
*   EwCompPoint() implements the Chora instant operators: 'point == point' and
*   'point != point'.
*
* ARGUMENTS:
*   aPoint1 - The first point.
*   aPoint2 - The second point.
*
* RETURN VALUE:
*   Returns zero if the both points are equal.
*
*******************************************************************************/
int EwCompPoint( XPoint aPoint1, XPoint aPoint2 )
{
  if ( aPoint1.X != aPoint2.X ) return 1;
  if ( aPoint1.Y != aPoint2.Y ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsPointNull
*
* DESCRIPTION:
*   The function EwIsPointNull() returns != 0 if the given point aPoint does
*   contain the value <0,0>. Otherwise the function returns 0.
*
* ARGUMENTS:
*   aPoint - Point to verify.
*
* RETURN VALUE:
*   Returns != 0 if the point is <0,0>.
*
*******************************************************************************/
XBool EwIsPointNull( XPoint aPoint )
{
  return !aPoint.X && !aPoint.Y;
}


/*******************************************************************************
* FUNCTION:
*   EwMovePointNeg
*
* DESCRIPTION:
*   The function EwMovePointNeg() adjusts the position of the point aPoint by 
*   the specified amount aOffset in the negative direction.
*
*   EwMovePointNeg() implements the Chora instant operator: 'point - point'.
*
* ARGUMENTS:
*   aPoint  - The point to be moved.
*   aOffset - The Offset that specifies the amount of vertical and horizontal 
*     space to move aPoint.
*
* RETURN VALUE:
*   Returns a new point with the changed position.
*
*******************************************************************************/
XPoint EwMovePointNeg( XPoint aPoint, XPoint aOffset )
{
  aPoint.X -= aOffset.X;
  aPoint.Y -= aOffset.Y;

  return aPoint;
}


/*******************************************************************************
* FUNCTION:
*   EwMovePointPos
*
* DESCRIPTION:
*   The function EwMovePointPos() adjusts the position of the point aPoint by 
*   the specified amount aOffset in the negative direction.
*
*   EwMovePointPos() implements the Chora instant operator: 'point + point'.
*
* ARGUMENTS:
*   aPoint  - The point to be moved.
*   aOffset - The offset that specifies the amount of vertical and horizontal 
*     space to move aPoint.
*
* RETURN VALUE:
*   Returns a new point with the changed position.
*
*******************************************************************************/
XPoint EwMovePointPos( XPoint aPoint, XPoint aOffset )
{
  aPoint.X += aOffset.X;
  aPoint.Y += aOffset.Y;

  return aPoint;
}


/*******************************************************************************
* FUNCTION:
*   EwNegPoint
*
* DESCRIPTION:
*   The function EwNegPoint() changes the sign of the point from '+' to '-',
*   or from '-' to '+'. The function negates the point position.
*
* ARGUMENTS:
*   aPoint  - The point to negate.
*
* RETURN VALUE:
*   Returns a new point with the changed position.
*
*******************************************************************************/
XPoint EwNegPoint( XPoint aPoint )
{
  aPoint.X = -aPoint.X;
  aPoint.Y = -aPoint.Y;

  return aPoint;
}


/*******************************************************************************
* FUNCTION:
*   EwSetPointX
*
* DESCRIPTION:
*   The function EwSetPointX() changes the X-coordinate of the given point to 
*   a new value aX.
*
*   EwSetPointX() implements the write access to the Chora instant property 
*   'point.x'.
*
* ARGUMENTS:
*   aPoint - The point to change the X-coordinate.
*   aX     - The new value for the X-coordinate.
*
* RETURN VALUE:
*   Returns a new point with the changed position.
*
*******************************************************************************/
XPoint EwSetPointX( XPoint aPoint, XInt32 aX )
{
  aPoint.X = aX;
  return aPoint;
}


/*******************************************************************************
* FUNCTION:
*   EwSetPointY
*
* DESCRIPTION:
*   The function EwSetPointY() changes the Y-coordinate of the given point to 
*   a new value aY.
*
*   EwSetPointY() implements the write access to the Chora instant property 
*   'point.y'.
*
* ARGUMENTS:
*   aPoint - The point to change the Y-coordinate.
*   aY     - The new value for the Y-coordinate.
*
* RETURN VALUE:
*   Returns a new point with the changed position.
*
*******************************************************************************/
XPoint EwSetPointY( XPoint aPoint, XInt32 aY )
{
  aPoint.Y = aY;
  return aPoint;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfPoint
*
* DESCRIPTION:
*   The function EwGetVariantOfPoint() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple point values.
*
* RETURN VALUE:
*   Returns the determinated point value.
*
*******************************************************************************/
XPoint EwGetVariantOfPoint( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfPoint )->Value;
}


/* pba, msy */

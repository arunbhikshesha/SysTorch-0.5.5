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
*   The module 'ewrect.c' contains a set of EWRTE functions purpose-built for 
*   operations with the Chora datatype 'rect'. 
*
* SUBROUTINES:
*   EwNewRect
*   EwNewRect2Point
*   EwCompRect
*   EwIsRectNull
*   EwIsPointInRect
*   EwIsRectEmpty
*   EwInflateRect
*   EwIntersectRect
*   EwUnionRect
*   EwIntersectRect2
*   EwMoveRectNeg
*   EwMoveRectPos
*   EwGetRectH
*   EwGetRectW
*   EwGetRectArea
*   EwGetRectSize
*   EwGetRectORect
*   EwGetRectCenter
*   EwSetRectSize
*   EwSetRectOrigin
*   EwSetRectX1
*   EwSetRectY1
*   EwSetRectX2
*   EwSetRectY2
*   EwSetRectX
*   EwSetRectY
*   EwSetRectW
*   EwSetRectH
*   EwSetRectPoint1
*   EwSetRectPoint2
*   EwGetVariantOfRect
*
*******************************************************************************/

#include "ewrte.h"


/* EwLangId contains the ID of the currently selected language. LangId will be
   used to find out the correct variant of a rect constant in the function 
   EwGetVariantOfRect() */
extern int EwLangId;


/*******************************************************************************
* FUNCTION:
*   EwNewRect
*
* DESCRIPTION:
*   The function EwNewRect() initializes a new rectangle with the given aX1, 
*   aY1, aX2, aY2 values and returns the initialized rect.
*
*   EwNewRect() implements the Chora instant constructor: 
*   'rect(aX1,aY1,aX2,aY2)'.
*
* ARGUMENTS:
*   aX1 - x-coordinate of the first (top-left) point of the rectangle.
*   aY1 - y-coordinate of the first (top-left) point of the rectangle.
*   aX2 - x-coordinate of the second (bottom-right) point of the rectangle.
*   aY2 - y-coordinate of the second (bottom-right) point of the rectangle.
*
* RETURN VALUE:
*   Returns the initialized rectangle.
*
*******************************************************************************/
XRect EwNewRect( XInt32 aX1, XInt32 aY1, XInt32 aX2, XInt32 aY2 )
{
  XRect r;

  r.Point1.X = aX1;
  r.Point1.Y = aY1;
  r.Point2.X = aX2;
  r.Point2.Y = aY2;

  return r;
}


/*******************************************************************************
* FUNCTION:
*   EwNewRect2Point
*
* DESCRIPTION:
*   The function EwNewRect2Point() initializes a new rectangle with the given 
*   points aPoint1 and aPoint2 and returns the initialized rect.
*
*   EwNewRect2Point() implements the Chora instant constructor: 
*   'rect(aPoint1,aPoint2)'.
*
* ARGUMENTS:
*   aPoint1 - Coordinates of the first (top-left) point of the rectangle.
*   aPoint2 - Coordinates of the second (bottom-right) point of the rectangle.
*
* RETURN VALUE:
*   Returns the initialized rectangle.
*
*******************************************************************************/
XRect EwNewRect2Point( XPoint aPoint1, XPoint aPoint2 )
{
  XRect r;

  r.Point1 = aPoint1;
  r.Point2 = aPoint2;

  return r;
}


/*******************************************************************************
* FUNCTION:
*   EwCompRect
*
* DESCRIPTION:
*   The function EwCompRect() compares the both given rectangles aRect1 and 
*   aRect2 and returns a value == 0 if the rectangles are identical. Otherwise 
*   the function returns a value != 0.
*
*   EwCompRect() implements the Chora instant operators: 'rect == rect' and
*   'rect != rect'.
*
* ARGUMENTS:
*   aRect1 - The first rectangle.
*   aRect2 - The second rectangle.
*
* RETURN VALUE:
*   Returns zero if the both rectangles are equal or both rectangles are empty.
*
*******************************************************************************/
int EwCompRect( XRect aRect1, XRect aRect2 )
{
  if ( aRect1.Point1.X != aRect2.Point1.X ) return 1;
  if ( aRect1.Point1.Y != aRect2.Point1.Y ) return 1;
  if ( aRect1.Point2.X != aRect2.Point2.X ) return 1;
  if ( aRect1.Point2.Y != aRect2.Point2.Y ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsRectNull
*
* DESCRIPTION:
*   The function EwIsRectNull() returns != 0 if the given rectangle aRect does
*   contain the value <0,0,0,0>. Otherwise the function returns 0.
*
* ARGUMENTS:
*   aRect - Rectangle to verify.
*
* RETURN VALUE:
*   Returns != 0 if the rectangle is <0,0,0,0>.
*
*******************************************************************************/
XBool EwIsRectNull( XRect aRect )
{
  return !aRect.Point1.X && !aRect.Point1.Y && !aRect.Point2.X && !aRect.Point2.Y;
}


/*******************************************************************************
* FUNCTION:
*   EwIsPointInRect
*
* DESCRIPTION:
*   The function EwIsPointInRect() determines whether the specified point lies 
*   within a rectangle. A point is within a rectangle if it lies on the left or
*   top side or is within all four sides. A point on the right or bottom side 
*   is outside a rectangle.
*
*   EwIsPointInRect() implements the Chora instant operator: 'rect == point'.
*
* ARGUMENTS:
*   aRect  - The rectangle to be tested.
*   aPoint - The coordinates of a point.
*
* RETURN VALUE:
*   Returns a value != 0 if the point aPoint lies within the rectangle aRect.
*   Otherwiese the function returns 0.
*
*******************************************************************************/
XBool EwIsPointInRect( XRect aRect, XPoint aPoint )
{
  if (( aPoint.X < aRect.Point1.X ) || ( aPoint.X >= aRect.Point2.X )) return 0;
  if (( aPoint.Y < aRect.Point1.Y ) || ( aPoint.Y >= aRect.Point2.Y )) return 0;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwIsRectEmpty
*
* DESCRIPTION:
*   The function EwIsRectEmpty() determines whether the specified rectangle is
*   empty. An empty rectangle has negative or zero width or height.
*
*   EwIsRectEmpty() implements the Chora instant property: 'rect.isempty'.
*
* ARGUMENTS:
*   aRect  - The rectangle to be tested.
*
* RETURN VALUE:
*   Returns a value != 0 if the rectangle is empty.
*
*******************************************************************************/
XBool EwIsRectEmpty( XRect aRect )
{
  if ( aRect.Point2.X <= aRect.Point1.X ) return 1;
  if ( aRect.Point2.Y <= aRect.Point1.Y ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwInflateRect
*
* DESCRIPTION:
*   The function EwInflateRect() inflates the rectangle aRect by moving its 
*   sides away from its center. To do this, EwInflateRect() subtracts units 
*   from the top-left point and adds units to the bottom-right point of aRect.
*
*   EwInflateRect() implements the Chora instant operator: 'rect * point'.
*
* ARGUMENTS:
*   aRect  - The rectangle to be inflated.
*   aDelta - The number of units to inflate aRect.
*
* RETURN VALUE:
*   Returns a new rectangle with changed position and size.
*
*******************************************************************************/
XRect EwInflateRect( XRect aRect, XPoint aDelta )
{
  aRect.Point1.X -= aDelta.X;
  aRect.Point1.Y -= aDelta.Y;
  aRect.Point2.X += aDelta.X;
  aRect.Point2.Y += aDelta.Y;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwIntersectRect
*
* DESCRIPTION:
*   The function EwIntersectRect() retrieves a rectangle that represents the 
*   intersecting area of two specified rectangles aRect1 and aRect2.
*
*   EwIntersectRect() implements the Chora instant operator: 'rect & rect'.
*
* ARGUMENTS:
*   aRect1 - The first rectangle to be intersect with aRect2.
*   aRect2 - The second rectangle to be intersect with aRect1.
*
* RETURN VALUE:
*   Returns a new rectangle that represents the intersecting area of aRect1 
*   and aRect2. If the intersecting area is empty, the function returns an  
*   empty rectangle.
*
*******************************************************************************/
XRect EwIntersectRect( XRect aRect1, XRect aRect2 )
{
  /* Empty rectangle? (rectangle with zero or negative width/height) */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  if ( aRect2.Point2.X <= aRect2.Point1.X ) 
    aRect2.Point1.X = aRect2.Point2.X = 0;

  if ( aRect2.Point2.Y <= aRect2.Point1.Y ) 
    aRect2.Point1.Y = aRect2.Point2.Y = 0;

  if ( aRect1.Point1.X < aRect2.Point1.X ) aRect1.Point1.X = aRect2.Point1.X;
  if ( aRect1.Point2.X > aRect2.Point2.X ) aRect1.Point2.X = aRect2.Point2.X;
  if ( aRect1.Point1.Y < aRect2.Point1.Y ) aRect1.Point1.Y = aRect2.Point1.Y;
  if ( aRect1.Point2.Y > aRect2.Point2.Y ) aRect1.Point2.Y = aRect2.Point2.Y;

  /* Is the result empty too? */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  return aRect1;
}


/*******************************************************************************
* FUNCTION:
*   EwUnionRect
*
* DESCRIPTION:
*   The function EwUnionRect() retrieves a rectangle that represents the 
*   smallest rectangle that contains two specified rectangular areas aRect1 and 
*   aRect2.
*
*   If one of the passed rectangles aRect1 or aRect2 is empty, no union is build
*   and the function returns the other rectangle immediately.
*
*   EwUnionRect() implements the Chora instant operator: 'rect | rect'.
*
* ARGUMENTS:
*   aRect1 - The first rectangle in the union.
*   aRect2 - The second rectangle in the union.
*
* RETURN VALUE:
*   Returns a new rectangle that represents the union of the two specified 
*   areas aRect1 and aRect2. If the union area is empty, the function returns  
*   an empty rectangle.
*
*******************************************************************************/
XRect EwUnionRect( XRect aRect1, XRect aRect2 )
{
  /* Empty rectangle? (rectangle with zero or negative width/height) */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  if ( aRect2.Point2.X <= aRect2.Point1.X ) 
    aRect2.Point1.X = aRect2.Point2.X = 0;

  if ( aRect2.Point2.Y <= aRect2.Point1.Y ) 
    aRect2.Point1.Y = aRect2.Point2.Y = 0;

  /* If the first rectangle is empty - do not build an union with them! */
  if (( aRect1.Point1.X == aRect1.Point2.X ) || 
      ( aRect1.Point1.Y == aRect1.Point2.Y ))
    return aRect2;

  /* If the second rectangle is empty - do not build an union with them! */
  if (( aRect2.Point1.X == aRect2.Point2.X ) || 
      ( aRect2.Point1.Y == aRect2.Point2.Y ))
    return aRect1;

  if ( aRect1.Point1.X > aRect2.Point1.X ) aRect1.Point1.X = aRect2.Point1.X;
  if ( aRect1.Point2.X < aRect2.Point2.X ) aRect1.Point2.X = aRect2.Point2.X;
  if ( aRect1.Point1.Y > aRect2.Point1.Y ) aRect1.Point1.Y = aRect2.Point1.Y;
  if ( aRect1.Point2.Y < aRect2.Point2.Y ) aRect1.Point2.Y = aRect2.Point2.Y;

  /* Is the result empty too? */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  return aRect1;
}


/*******************************************************************************
* FUNCTION:
*   EwIntersectRect2
*
* DESCRIPTION:
*   The function EwIntersectRect2() retrieves a rectangle that represents the 
*   intersecting area of two specified rectangles aRect1 and aRect2.
*
*   Unlike the function EwIntersectRect(), empty rectangles are ignored. If one
*   of the passed rectangles aRect1 or aRect2 is empty, no intersection is build
*   and the function returns the other rectangle immediately.
*
*   EwIntersectRect2() implements the Chora instant operator: 'rect && rect'.
*
* ARGUMENTS:
*   aRect1 - The first rectangle to be intersect with aRect2.
*   aRect2 - The second rectangle to be intersect with aRect1.
*
* RETURN VALUE:
*   Returns a new rectangle that represents the intersecting area of aRect1 
*   and aRect2. If the intersecting area is empty, the function returns an  
*   empty rectangle.
*
*******************************************************************************/
XRect EwIntersectRect2( XRect aRect1, XRect aRect2 )
{
  /* Empty rectangle? (rectangle with zero or negative width/height) */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  if ( aRect2.Point2.X <= aRect2.Point1.X ) 
    aRect2.Point1.X = aRect2.Point2.X = 0;

  if ( aRect2.Point2.Y <= aRect2.Point1.Y ) 
    aRect2.Point1.Y = aRect2.Point2.Y = 0;

  /* If the first rectangle is empty - do not intersect with them! */
  if (( aRect1.Point1.X == aRect1.Point2.X ) || 
      ( aRect1.Point1.Y == aRect1.Point2.Y ))
    return aRect2;

  /* If the second rectangle is empty - do not intersect with them! */
  if (( aRect2.Point1.X == aRect2.Point2.X ) || 
      ( aRect2.Point1.Y == aRect2.Point2.Y ))
    return aRect1;

  if ( aRect1.Point1.X < aRect2.Point1.X ) aRect1.Point1.X = aRect2.Point1.X;
  if ( aRect1.Point2.X > aRect2.Point2.X ) aRect1.Point2.X = aRect2.Point2.X;
  if ( aRect1.Point1.Y < aRect2.Point1.Y ) aRect1.Point1.Y = aRect2.Point1.Y;
  if ( aRect1.Point2.Y > aRect2.Point2.Y ) aRect1.Point2.Y = aRect2.Point2.Y;

  /* Is the result empty too? */
  if ( aRect1.Point2.X <= aRect1.Point1.X ) 
    aRect1.Point1.X = aRect1.Point2.X = 0;

  if ( aRect1.Point2.Y <= aRect1.Point1.Y ) 
    aRect1.Point1.Y = aRect1.Point2.Y = 0;

  return aRect1;
}


/*******************************************************************************
* FUNCTION:
*   EwMoveRectNeg
*
* DESCRIPTION:
*   The function EwMoveRectNeg() adjusts the position of the rectangle aRect by
*   the specified amount aOffset in the negative direction.
*
*   EwMoveRectNeg() implements the Chora instant operator: 'rect - point'.
*
* ARGUMENTS:
*   aRect   - The rectangle to be moved.
*   aOffset - The Offset that specifies the amount of vertical and horizontal 
*     space to move aRect.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed position.
*
*******************************************************************************/
XRect EwMoveRectNeg( XRect aRect, XPoint aOffset )
{
  aRect.Point1.X -= aOffset.X;
  aRect.Point2.X -= aOffset.X;
  aRect.Point1.Y -= aOffset.Y;
  aRect.Point2.Y -= aOffset.Y;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwMoveRectPos
*
* DESCRIPTION:
*   The function EwMoveRectPos() adjusts the position of the rectangle aRect by
*   the specified amount aOffset in the positive direction.
*
*   EwMoveRectPos() implements the Chora instant operator: 'rect + point'.
*
* ARGUMENTS:
*   aRect   - The rectangle to be moved.
*   aOffset - The Offset that specifies the amount of vertical and horizontal 
*     space to move aRect.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed position.
*
*******************************************************************************/
XRect EwMoveRectPos( XRect aRect, XPoint aOffset )
{
  aRect.Point1.X += aOffset.X;
  aRect.Point2.X += aOffset.X;
  aRect.Point1.Y += aOffset.Y;
  aRect.Point2.Y += aOffset.Y;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectH
*
* DESCRIPTION:
*   The function EwGetRectH() returns the height of the rectangle. The function
*   calculates the height of the rectangle by subtracting the coordinate of the
*   top border from the bottom border.
*
*   EwGetRectH() implements the read access to the Chora instant property: 
*   'rect.h'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the height.
*
* RETURN VALUE:
*   Returns the height of the rectangle. The value can be negative.
*
*******************************************************************************/
XInt32 EwGetRectH( XRect aRect )
{
  return aRect.Point2.Y - aRect.Point1.Y;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectW
*
* DESCRIPTION:
*   The function EwGetRectW() returns the width of the rectangle. The function
*   calculates the width of the rectangle by subtracting the coordinate of the
*   left border from the right border.
*
*   EwGetRectW() implements the read access to the Chora instant property: 
*   'rect.w'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the width.
*
* RETURN VALUE:
*   Returns the width of the rectangle. The value can be negative.
*
*******************************************************************************/
XInt32 EwGetRectW( XRect aRect )
{
  return aRect.Point2.X - aRect.Point1.X;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectArea
*
* DESCRIPTION:
*   The function EwGetRectArea() calculates the area of the rectangle. The 
*   multiplies the width with the height of the rectangle.
*
*   EwGetRectArea() implements the read access to the Chora instant property: 
*   'rect.area'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the area.
*
* RETURN VALUE:
*   Returns the area of the rectangle. The value can be negative.
*
*******************************************************************************/
XInt32 EwGetRectArea( XRect aRect )
{
  return ( aRect.Point2.X - aRect.Point1.X ) * 
         ( aRect.Point2.Y - aRect.Point1.Y );
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectSize
*
* DESCRIPTION:
*   The function EwGetRectSize() calculates the size of the rectangle.
*
*   EwGetRectSize() implements the read access to the Chora instant property: 
*   'rect.size'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the size.
*
* RETURN VALUE:
*   Returns the size of the rectangle.
*
*******************************************************************************/
XPoint EwGetRectSize( XRect aRect )
{
  aRect.Point2.X -= aRect.Point1.X;
  aRect.Point2.Y -= aRect.Point1.Y;

  return aRect.Point2;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectORect
*
* DESCRIPTION:
*   The function EwGetRectORect() calculates rectangle with the same size as
*   the given rectangle, but with the fixed origin point <0,0>.
*
*   EwGetRectORect() implements the read access to the Chora instant property: 
*   'rect.orect'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the zero fixed rectangle.
*
* RETURN VALUE:
*   Returns new rectangle.
*
*******************************************************************************/
XRect EwGetRectORect( XRect aRect )
{
  aRect.Point2.X -= aRect.Point1.X;
  aRect.Point2.Y -= aRect.Point1.Y;
  aRect.Point1.X  = 0;
  aRect.Point1.Y  = 0;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectCenter
*
* DESCRIPTION:
*   The function EwGetRectCenter() calculates the coordinates of the point in
*   the center of the given rectangle.
*
*   EwGetRectCenter() implements the read access to the Chora instant property: 
*   'rect.center'.
*
* ARGUMENTS:
*   aRect - The rectangle to get the center of them.
*
* RETURN VALUE:
*   Returns coordinates of the rectangle's center.
*
*******************************************************************************/
XPoint EwGetRectCenter( XRect aRect )
{
  XPoint center = aRect.Point1;

  center.X += ( aRect.Point2.X - aRect.Point1.X ) / 2;
  center.Y += ( aRect.Point2.Y - aRect.Point1.Y ) / 2;

  return center;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectSize
*
* DESCRIPTION:
*   The function EwSetRectSize() changes the size of the given rectangle to a 
*   new value aSize. The function changes the value of Point2.
*
*   EwSetRectSize() implements the write access to the Chora instant property 
*   'rect.size'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the size.
*   aSize - The new size of the rectangle.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed size.
*
*******************************************************************************/
XRect EwSetRectSize( XRect aRect, XPoint aSize )
{
  aRect.Point2.X = aRect.Point1.X + aSize.X;
  aRect.Point2.Y = aRect.Point1.Y + aSize.Y;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectOrigin
*
* DESCRIPTION:
*   The function EwSetRectOrigin() changes the origin of the given rectangle to
*   a new value aOrigin. The function changes the value of Point1 and Point2.
*
*   EwSetRectOrigin() implements the write access to the Chora instant property 
*   'rect.origin'.
*
* ARGUMENTS:
*   aRect   - The rectangle to change the origin.
*   aOrigin - The new origin of the rectangle.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed origin.
*
*******************************************************************************/
XRect EwSetRectOrigin( XRect aRect, XPoint aOrigin )
{
  aRect.Point2.X += aOrigin.X - aRect.Point1.X;
  aRect.Point2.Y += aOrigin.Y - aRect.Point1.Y;
  aRect.Point1.X  = aOrigin.X;
  aRect.Point1.Y  = aOrigin.Y;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectX1
*
* DESCRIPTION:
*   The function EwSetRectX1() changes the X-coordinate of the first (top-left)
*   point of the given rectangle to a new value aX1.
*
*   EwSetRectX1() implements the write access to the Chora instant property 
*   'rect.x1'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the X1-coordinate.
*   aX1   - The new value for the X1-coordinate.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed coordinate.
*
*******************************************************************************/
XRect EwSetRectX1( XRect aRect, XInt32 aX1 )
{
  aRect.Point1.X = aX1;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectY1
*
* DESCRIPTION:
*   The function EwSetRectY1() changes the Y-coordinate of the first (top-left)
*   point of the given rectangle to a new value aY1.
*
*   EwSetRectY1() implements the write access to the Chora instant property 
*   'rect.y1'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the Y1-coordinate.
*   aY1   - The new value for the Y1-coordinate.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed coordinate.
*
*******************************************************************************/
XRect EwSetRectY1( XRect aRect, XInt32 aY1 )
{
  aRect.Point1.Y = aY1;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectX2
*
* DESCRIPTION:
*   The function EwSetRectX2() changes the X-coordinate of the second (bottom-
*   right) point of the given rectangle to a new value aX2.
*
*   EwSetRectX2() implements the write access to the Chora instant property 
*   'rect.x2'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the X2-coordinate.
*   aX2   - The new value for the X2-coordinate.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed coordinate.
*
*******************************************************************************/
XRect EwSetRectX2( XRect aRect, XInt32 aX2 )
{
  aRect.Point2.X = aX2;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectY2
*
* DESCRIPTION:
*   The function EwSetRectY2() changes the Y-coordinate of the second (bottom-
*   right) point of the given rectangle to a new value aY2.
*
*   EwSetRectY2() implements the write access to the Chora instant property 
*   'rect.y2'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the Y2-coordinate.
*   aY2   - The new value for the Y2-coordinate.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed coordinate.
*
*******************************************************************************/
XRect EwSetRectY2( XRect aRect, XInt32 aY2 )
{
  aRect.Point2.Y = aY2;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectX
*
* DESCRIPTION:
*   The function EwSetRectX() changes the X origin of the given rectangle to 
*   the value aX.
*
*   EwSetRectX() implements the write access to the Chora instant property 
*   'rect.x'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the origin.
*   aX    - The new value for the rectangles origin X.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed origin.
*
*******************************************************************************/
XRect EwSetRectX( XRect aRect, XInt32 aX )
{
  aRect.Point2.X += aX - aRect.Point1.X;
  aRect.Point1.X  = aX;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectY
*
* DESCRIPTION:
*   The function EwSetRectY() changes the Y origin of the given rectangle to 
*   the value aY.
*
*   EwSetRectY() implements the write access to the Chora instant property 
*   'rect.y'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the origin.
*   aY    - The new value for the rectangles origin Y.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed origin.
*
*******************************************************************************/
XRect EwSetRectY( XRect aRect, XInt32 aY )
{
  aRect.Point2.Y += aY - aRect.Point1.Y;
  aRect.Point1.Y  = aY;

  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectW
*
* DESCRIPTION:
*   The function EwSetRectW() changes the width of the given rectangle to a new
*   value aW.
*
*   EwSetRectW() implements the write access to the Chora instant property 
*   'rect.w'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the width.
*   aW    - The new value for the rectangles width.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed width.
*
*******************************************************************************/
XRect EwSetRectW( XRect aRect, XInt32 aW )
{
  aRect.Point2.X = aRect.Point1.X + aW;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectH
*
* DESCRIPTION:
*   The function EwSetRectH() changes the height of the given rectangle to a 
*   new value aH.
*
*   EwSetRectH() implements the write access to the Chora instant property 
*   'rect.h'.
*
* ARGUMENTS:
*   aRect - The rectangle to change the height.
*   aH    - The new value for the rectangles height.
*
* RETURN VALUE:
*   Returns a new rectangle with the changed height.
*
*******************************************************************************/
XRect EwSetRectH( XRect aRect, XInt32 aH )
{
  aRect.Point2.Y = aRect.Point1.Y + aH;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectPoint1
*
* DESCRIPTION:
*   The function EwSetPoint1() changes the coordinates of the first (top-left)
*   point of the rectangle to a new value aPoint1.
*
*   EwSetRectPoint1() implements the write access to the Chora instant property 
*   'rect.point1'.
*
* ARGUMENTS:
*   aRect   - The rectangle to be changed.
*   aPoint1 - The new coordinates for the first (top-left) point.
*
* RETURN VALUE:
*   Returns a new rectangle with changed coordinates.
*
*******************************************************************************/
XRect EwSetRectPoint1( XRect aRect, XPoint aPoint1 )
{
  aRect.Point1 = aPoint1;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwSetRectPoint2
*
* DESCRIPTION:
*   The function EwSetPoint2() changes the coordinates of the second (bottom-
*   right) point of the rectangle to a new value aPoint2.
*
*   EwSetRectPoint2() implements the write access to the Chora instant property 
*   'rect.point2'.
*
* ARGUMENTS:
*   aRect   - The rectangle to be changed.
*   aPoint2 - The new coordinates for the second (bottom-right) point.
*
* RETURN VALUE:
*   Returns a new rectangle with changed coordinates.
*
*******************************************************************************/
XRect EwSetRectPoint2( XRect aRect, XPoint aPoint2 )
{
  aRect.Point2 = aPoint2;
  return aRect;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfRect
*
* DESCRIPTION:
*   The function EwGetVariantOfRect() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple rect values.
*
* RETURN VALUE:
*   Returns the determinated rect value.
*
*******************************************************************************/
XRect EwGetVariantOfRect( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfRect )->Value;
}


/* pba, msy */

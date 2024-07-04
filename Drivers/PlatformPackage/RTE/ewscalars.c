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
*   The module 'ewscalar.c' contains a set of EWRTE functions purpose-built for
*   operations with the Choras datatypes 'int8', 'int16' .. 'uint32', 'bool',
*   'float', 'enum' and 'set'.
*
* SUBROUTINES:
*   EwSetContains
*   EwGetVariantOfInt8
*   EwGetVariantOfInt16
*   EwGetVariantOfInt32
*   EwGetVariantOfInt64
*   EwGetVariantOfUInt8
*   EwGetVariantOfUInt16
*   EwGetVariantOfUInt32
*   EwGetVariantOfUInt64
*   EwGetVariantOfBool
*   EwGetVariantOfFloat
*   EwGetVariantOfEnum
*   EwGetVariantOfSet
*   EwGetInt32Abs
*   EwGetInt64Abs
*   EwGetFloatAbs
*   EwGetPointAbs
*   EwGetRectAbs
*   EwGetInt32UAbs
*   EwGetInt64UAbs
*   EwNewFloatNaN
*   EwNewFloatInfP
*   EwNewFloatInfN
*   EwIsFloatNaN
*   EwIsFloatInf
*   EwIsFloatInfP
*   EwIsFloatInfN
*   EwGetInt32Min
*   EwGetUInt32Min
*   EwGetInt64Min
*   EwGetUInt64Min
*   EwGetFloatMin
*   EwGetColorMin
*   EwGetPointMin
*   EwGetRectMin
*   EwGetInt32Max
*   EwGetUInt32Max
*   EwGetInt64Max
*   EwGetUInt64Max
*   EwGetFloatMax
*   EwGetColorMax
*   EwGetPointMax
*   EwGetRectMax
*   EwMathLength
*   EwMathLengthPoint
*   EwMathTrunc
*   EwMathFract
*
*******************************************************************************/

#include "ewrte.h"
#include "ewextrte.h"


/* EwLangId contains the ID of the currently selected language. LangId will be
   used to find out the correct variant of a constant in the following functions
   EwGetVariantOfXXX() */
extern int EwLangId;


/*******************************************************************************
* FUNCTION:
*   EwSetContains
*
* DESCRIPTION:
*   The function EwSetContains() implements the Chora instant method
*   'set.contains()'.
*
* ARGUMENTS:
*   aSet1 - The first set to verify its content.
*   aSet2 - The second set.
*
* RETURN VALUE:
*   The function returns != 0, if the second set aSet2 is contained in the
*   given aSet1.
*
*******************************************************************************/
XBool EwSetContains( XSet aSet1, XSet aSet2 )
{
  return aSet2 && (( aSet1 & aSet2 ) == aSet2 );
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfInt8
*
* DESCRIPTION:
*   The function EwGetVariantOfInt8() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple int8 values.
*
* RETURN VALUE:
*   Returns the determinated int8 value.
*
*******************************************************************************/
XInt8 EwGetVariantOfInt8( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfInt8 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfInt16
*
* DESCRIPTION:
*   The function EwGetVariantOfInt16() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple int16 values.
*
* RETURN VALUE:
*   Returns the determinated int16 value.
*
*******************************************************************************/
XInt16 EwGetVariantOfInt16( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfInt16 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfInt32
*
* DESCRIPTION:
*   The function EwGetVariantOfInt32() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple int32 values.
*
* RETURN VALUE:
*   Returns the determinated int32 value.
*
*******************************************************************************/
XInt32 EwGetVariantOfInt32( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfInt32 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfInt64
*
* DESCRIPTION:
*   The function EwGetVariantOfInt64() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple int64 values.
*
* RETURN VALUE:
*   Returns the determinated int64 value.
*
*******************************************************************************/
XInt64 EwGetVariantOfInt64( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfInt64 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfUInt8
*
* DESCRIPTION:
*   The function EwGetVariantOfUInt8() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple uint8 values.
*
* RETURN VALUE:
*   Returns the determinated uint8 value.
*
*******************************************************************************/
XUInt8 EwGetVariantOfUInt8( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfUInt8 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfUInt16
*
* DESCRIPTION:
*   The function EwGetVariantOfUInt16() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple uint16 values.
*
* RETURN VALUE:
*   Returns the determinated uint16 value.
*
*******************************************************************************/
XUInt16 EwGetVariantOfUInt16( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfUInt16 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfUInt32
*
* DESCRIPTION:
*   The function EwGetVariantOfUInt32() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple uint32 values.
*
* RETURN VALUE:
*   Returns the determinated uint32 value.
*
*******************************************************************************/
XUInt32 EwGetVariantOfUInt32( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfUInt32 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfUInt64
*
* DESCRIPTION:
*   The function EwGetVariantOfUInt64() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple uint64 values.
*
* RETURN VALUE:
*   Returns the determinated uint64 value.
*
*******************************************************************************/
XUInt64 EwGetVariantOfUInt64( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfUInt64 )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfBool
*
* DESCRIPTION:
*   The function EwGetVariantOfBool() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple bool values.
*
* RETURN VALUE:
*   Returns the determinated bool value.
*
*******************************************************************************/
XBool EwGetVariantOfBool( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfBool )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfFloat
*
* DESCRIPTION:
*   The function EwGetVariantOfFloat() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple float values.
*
* RETURN VALUE:
*   Returns the determinated float value.
*
*******************************************************************************/
XFloat EwGetVariantOfFloat( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfFloat )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfEnum
*
* DESCRIPTION:
*   The function EwGetVariantOfEnum() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple enum values.
*
* RETURN VALUE:
*   Returns the determinated enum value.
*
*******************************************************************************/
XEnum EwGetVariantOfEnum( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfEnum )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfSet
*
* DESCRIPTION:
*   The function EwGetVariantOfSet() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple set values.
*
* RETURN VALUE:
*   Returns the determinated set value.
*
*******************************************************************************/
XSet EwGetVariantOfSet( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfSet )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt32Abs
*
* DESCRIPTION:
*   The function EwGetInt32Abs() implements the Chora int8.abs, int16.abs,
*   int32.abs instant properties.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XInt32 EwGetInt32Abs( XInt32 aValue )
{
  return ( aValue < 0 )? -aValue : aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt64Abs
*
* DESCRIPTION:
*   The function EwGetInt64Abs() implements the Chora int8.abs, int16.abs,
*   int64.abs instant properties.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XInt64 EwGetInt64Abs( XInt64 aValue )
{
  return ( aValue < 0 )? -aValue : aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetFloatAbs
*
* DESCRIPTION:
*   The function EwGetFloatAbs() implements the Chora float.abs instant property.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XFloat EwGetFloatAbs( XFloat aValue )
{
  return ( aValue < 0.0f )? -aValue : aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetPointAbs
*
* DESCRIPTION:
*   The function EwGetPointAbs() implements the Chora point.abs instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XPoint EwGetPointAbs( XPoint aValue )
{
  if ( aValue.X < 0 ) aValue.X = -aValue.X;
  if ( aValue.Y < 0 ) aValue.Y = -aValue.Y;

  return aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectAbs
*
* DESCRIPTION:
*   The function EwGetRectAbs() implements the Chora rect.abs instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XRect EwGetRectAbs( XRect aValue )
{
  if ( aValue.Point1.X < 0 ) aValue.Point1.X = -aValue.Point1.X;
  if ( aValue.Point1.Y < 0 ) aValue.Point1.Y = -aValue.Point1.Y;
  if ( aValue.Point2.X < 0 ) aValue.Point2.X = -aValue.Point2.X;
  if ( aValue.Point2.Y < 0 ) aValue.Point2.Y = -aValue.Point2.Y;

  return aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt32UAbs
*
* DESCRIPTION:
*   The function EwGetInt32UAbs() implements the Chora int8.uabs, int16.uabs,
*   int32.uabs instant properties.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XUInt32 EwGetInt32UAbs( XInt32 aValue )
{
  return ( aValue < 0 )? (XUInt32)-aValue : (XUInt32)aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt64UAbs
*
* DESCRIPTION:
*   The function EwGetInt64UAbs() implements the Chora int8.uabs, int16.uabs,
*   int64.uabs instant properties.
*
* ARGUMENTS:
*   aValue - A value to calculate its absolute value.
*
* RETURN VALUE:
*   Returns the absolute value of aValue.
*
*******************************************************************************/
XUInt64 EwGetInt64UAbs( XInt64 aValue )
{
  return ( aValue < 0 )? (XUInt64)-aValue : (XUInt64)aValue;
}


/*******************************************************************************
* FUNCTION:
*   EwNewFloatNaN
*
* DESCRIPTION:
*   The function EwNewFloatNaN() return the value corresponding to float NAN.
*
*   EwNewFloatNaN() implements the Chora instant constructor:
*   'float_nan()'.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns the initialized float value.
*
*******************************************************************************/
XFloat EwNewFloatNaN( void )
{
  static const unsigned int valNAN = 0x7F800001;
  const void* ptr = &valNAN;
  return *((float*)ptr);
}


/*******************************************************************************
* FUNCTION:
*   EwNewFloatInfP
*
* DESCRIPTION:
*   The function EwNewFloatInfP() return the value corresponding to float +INF.
*
*   EwNewFloatInfP() implements the Chora instant constructor:
*   'float_infp()'.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns the initialized float value.
*
*******************************************************************************/
XFloat EwNewFloatInfP( void )
{
  static const unsigned int valINFP = 0x7F800000;
  const void* ptr = &valINFP;
  return *((float*)ptr);
}


/*******************************************************************************
* FUNCTION:
*   EwNewFloatInfN
*
* DESCRIPTION:
*   The function EwNewFloatInfN() return the value corresponding to float -INF.
*
*   EwNewFloatInfN() implements the Chora instant constructor:
*   'float_infn()'.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns the initialized float value.
*
*******************************************************************************/
XFloat EwNewFloatInfN( void )
{
  static const unsigned int valINFN = 0xFF800000;
  const void* ptr = &valINFN;
  return *((float*)ptr);
}


/*******************************************************************************
* FUNCTION:
*   EwIsFloatNaN
*
* DESCRIPTION:
*   The function EwIsFloatNaN() implements the Chora float.isnan instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to test whether it is 'not a number'.
*
* RETURN VALUE:
*   Returns != 0 if the given value is not a number.
*
*******************************************************************************/
XBool EwIsFloatNaN( XFloat aValue )
{
  void* ptr = &aValue;
  unsigned int l = *((unsigned int*)ptr );

  if (( l > 0x7F800000 ) && ( l <= 0x7FFFFFFF )) return 1;
  if ( l > 0xFF800000 ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsFloatInf
*
* DESCRIPTION:
*   The function EwIsFloatInf() implements the Chora float.isinf instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to test whether it is a positive/negative infinite value.
*
* RETURN VALUE:
*   Returns != 0 if the given value is +/- infinite.
*
*******************************************************************************/
XBool EwIsFloatInf( XFloat aValue )
{
  void* ptr = &aValue;
  unsigned int l = *((unsigned int*)ptr );

  if ( l == 0x7F800000 ) return 1;
  if ( l == 0xFF800000 ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsFloatInfP
*
* DESCRIPTION:
*   The function EwIsFloatInfP() implements the Chora float.isinfp instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to test whether it is a positive infinite value.
*
* RETURN VALUE:
*   Returns != 0 if the given value is + infinite.
*
*******************************************************************************/
XBool EwIsFloatInfP( XFloat aValue )
{
  void* ptr = &aValue;
  unsigned int l = *((unsigned int*)ptr );

  return l == 0x7F800000;
}


/*******************************************************************************
* FUNCTION:
*   EwIsFloatInfN
*
* DESCRIPTION:
*   The function EwIsFloatInfN() implements the Chora float.isinfn instant
*   property.
*
* ARGUMENTS:
*   aValue - A value to test whether it is a negative infinite value.
*
* RETURN VALUE:
*   Returns != 0 if the given value is - infinite.
*
*******************************************************************************/
XBool EwIsFloatInfN( XFloat aValue )
{
  void* ptr = &aValue;
  unsigned int l = *((unsigned int*)ptr );

  return l == 0xFF800000;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt32Min
*
* DESCRIPTION:
*   The function EwGetInt32Min() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XInt32 values.
*
*******************************************************************************/
XInt32 EwGetInt32Min( int aCount, ... )
{
  va_list marker;
  XInt32  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XInt32 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XInt32 tmp = va_arg( marker, XInt32 );

    if ( tmp < value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetUInt32Min
*
* DESCRIPTION:
*   The function EwGetUInt32Min() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XUInt32 values.
*
*******************************************************************************/
XUInt32 EwGetUInt32Min( int aCount, ... )
{
  va_list marker;
  XUInt32 value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XUInt32 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XUInt32 tmp = va_arg( marker, XUInt32 );

    if ( tmp < value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt64Min
*
* DESCRIPTION:
*   The function EwGetInt64Min() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XInt64 values.
*
*******************************************************************************/
XInt64 EwGetInt64Min( int aCount, ... )
{
  va_list marker;
  XInt64  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XInt64 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XInt64 tmp = va_arg( marker, XInt64 );

    if ( tmp < value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetUInt64Min
*
* DESCRIPTION:
*   The function EwGetUInt64Min() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XUInt64 values.
*
*******************************************************************************/
XUInt64 EwGetUInt64Min( int aCount, ... )
{
  va_list marker;
  XUInt64 value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XUInt64 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XUInt64 tmp = va_arg( marker, XUInt64 );

    if ( tmp < value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetFloatMin
*
* DESCRIPTION:
*   The function EwGetFloatMin() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XFloat values.
*
*******************************************************************************/
XFloat EwGetFloatMin( int aCount, ... )
{
  va_list marker;
  XFloat  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = (XFloat)va_arg( marker, double );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XFloat tmp = (XFloat)va_arg( marker, double );

    if ( tmp < value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetColorMin
*
* DESCRIPTION:
*   The function EwGetColorMin() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XColor values.
*
*******************************************************************************/
XColor EwGetColorMin( int aCount, ... )
{
  va_list marker;
  XColor  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XColor );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XColor tmp = va_arg( marker, XColor );

    if ( tmp.Red   < value.Red   ) value.Red   = tmp.Red;
    if ( tmp.Green < value.Green ) value.Green = tmp.Green;
    if ( tmp.Blue  < value.Blue  ) value.Blue  = tmp.Blue;
    if ( tmp.Alpha < value.Alpha ) value.Alpha = tmp.Alpha;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetPointMin
*
* DESCRIPTION:
*   The function EwGetPointMin() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XPoint values.
*
*******************************************************************************/
XPoint EwGetPointMin( int aCount, ... )
{
  va_list marker;
  XPoint  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XPoint );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XPoint tmp = va_arg( marker, XPoint );

    if ( tmp.X < value.X ) value.X = tmp.X;
    if ( tmp.Y < value.Y ) value.Y = tmp.Y;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectMin
*
* DESCRIPTION:
*   The function EwGetRectMin() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XRect values.
*
*******************************************************************************/
XRect EwGetRectMin( int aCount, ... )
{
  va_list marker;
  XRect   value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XRect );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XRect tmp = va_arg( marker, XRect );

    if ( tmp.Point1.X < value.Point1.X ) value.Point1.X = tmp.Point1.X;
    if ( tmp.Point1.Y < value.Point1.Y ) value.Point1.Y = tmp.Point1.Y;
    if ( tmp.Point2.X < value.Point2.X ) value.Point2.X = tmp.Point2.X;
    if ( tmp.Point2.Y < value.Point2.Y ) value.Point2.Y = tmp.Point2.Y;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt32Max
*
* DESCRIPTION:
*   The function EwGetInt32Max() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XInt32 values.
*
*******************************************************************************/
XInt32 EwGetInt32Max( int aCount, ... )
{
  va_list marker;
  XInt32  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XInt32 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XInt32 tmp = va_arg( marker, XInt32 );

    if ( tmp > value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetUInt32Max
*
* DESCRIPTION:
*   The function EwGetUInt32Max() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XUInt32 values.
*
*******************************************************************************/
XUInt32 EwGetUInt32Max( int aCount, ... )
{
  va_list marker;
  XUInt32 value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XUInt32 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XUInt32 tmp = va_arg( marker, XUInt32 );

    if ( tmp > value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetInt64Max
*
* DESCRIPTION:
*   The function EwGetInt64Max() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XInt64 values.
*
*******************************************************************************/
XInt64 EwGetInt64Max( int aCount, ... )
{
  va_list marker;
  XInt64  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XInt64 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XInt64 tmp = va_arg( marker, XInt64 );

    if ( tmp > value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetUInt64Max
*
* DESCRIPTION:
*   The function EwGetUInt64Max() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XUInt64 values.
*
*******************************************************************************/
XUInt64 EwGetUInt64Max( int aCount, ... )
{
  va_list marker;
  XUInt64 value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XUInt64 );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XUInt64 tmp = va_arg( marker, XUInt64 );

    if ( tmp > value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetFloatMax
*
* DESCRIPTION:
*   The function EwGetFloatMax() implements the Chora math_min() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the minimum.
*
* RETURN VALUE:
*   Returns the minimum of the given aCount XFloat values.
*
*******************************************************************************/
XFloat EwGetFloatMax( int aCount, ... )
{
  va_list marker;
  XFloat  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = (XFloat)va_arg( marker, double );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XFloat tmp = (XFloat)va_arg( marker, double );

    if ( tmp > value )
      value = tmp;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetColorMax
*
* DESCRIPTION:
*   The function EwGetColorMax() implements the Chora math_max() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the maximum.
*
* RETURN VALUE:
*   Returns the maximum of the given aCount XColor values.
*
*******************************************************************************/
XColor EwGetColorMax( int aCount, ... )
{
  va_list marker;
  XColor  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XColor );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XColor tmp = va_arg( marker, XColor );

    if ( tmp.Red   > value.Red   ) value.Red   = tmp.Red;
    if ( tmp.Green > value.Green ) value.Green = tmp.Green;
    if ( tmp.Blue  > value.Blue  ) value.Blue  = tmp.Blue;
    if ( tmp.Alpha > value.Alpha ) value.Alpha = tmp.Alpha;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetPointMax
*
* DESCRIPTION:
*   The function EwGetPointMax() implements the Chora math_max() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the maximum.
*
* RETURN VALUE:
*   Returns the maximum of the given aCount XPoint values.
*
*******************************************************************************/
XPoint EwGetPointMax( int aCount, ... )
{
  va_list marker;
  XPoint  value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XPoint );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XPoint tmp = va_arg( marker, XPoint );

    if ( tmp.X > value.X ) value.X = tmp.X;
    if ( tmp.Y > value.Y ) value.Y = tmp.Y;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetRectMax
*
* DESCRIPTION:
*   The function EwGetRectMax() implements the Chora math_max() function.
*
* ARGUMENTS:
*   aCount - Number of values to estimate the maximum.
*
* RETURN VALUE:
*   Returns the maximum of the given aCount XRect values.
*
*******************************************************************************/
XRect EwGetRectMax( int aCount, ... )
{
  va_list marker;
  XRect   value;
  int     i;

  /* Init arguments. */
  va_start( marker, aCount );
  value = va_arg( marker, XRect );

  /* Iterate over all available arguments */
  for ( i = 1; i < aCount; i++ )
  {
    XRect tmp = va_arg( marker, XRect );

    if ( tmp.Point1.X > value.Point1.X ) value.Point1.X = tmp.Point1.X;
    if ( tmp.Point1.Y > value.Point1.Y ) value.Point1.Y = tmp.Point1.Y;
    if ( tmp.Point2.X > value.Point2.X ) value.Point2.X = tmp.Point2.X;
    if ( tmp.Point2.Y > value.Point2.Y ) value.Point2.Y = tmp.Point2.Y;
  }

  /* Complete */
  va_end( marker );
  return value;
}


/*******************************************************************************
* FUNCTION:
*   EwMathLength
*
* DESCRIPTION:
*   The function EwMathLength() implements the Chora math_length() built-in
*   function intended to calculate the length of a given vector.
*
* ARGUMENTS:
*   aX, aY : The size of the vector in X and Y direction.
*
* RETURN VALUE:
*   Returns the length of the vector.
*
*******************************************************************************/
XFloat EwMathLength( XFloat aX, XFloat aY )
{
  return EwMathSqrt(( aX * aX ) + ( aY * aY ));
}


/*******************************************************************************
* FUNCTION:
*   EwMathLengthPoint
*
* DESCRIPTION:
*   The function EwMathLengthPoint() implements the Chora math_length() built-
*   in function intended to calculate the length of a given vector.
*
* ARGUMENTS:
*   aPoint : The size of the vector in X and Y direction.
*
* RETURN VALUE:
*   Returns the length of the vector.
*
*******************************************************************************/
XFloat EwMathLengthPoint( XPoint aPoint )
{
  XFloat x = (XFloat)aPoint.X;
  XFloat y = (XFloat)aPoint.Y;

  return EwMathSqrt(( x * x ) + ( y * y ));
}


/*******************************************************************************
* FUNCTION:
*   EwMathTrunc
*
* DESCRIPTION:
*   The function EwMathTrunc() implements the Chora math_trunc() built-in
*   function intended to calculate the integer part of a given number by
*   removing the fractional digits.
*
* ARGUMENTS:
*   aValue : The value to calculate the integer part.
*
* RETURN VALUE:
*   Returns the integer part of the number.
*
*******************************************************************************/
XFloat EwMathTrunc( XFloat aValue )
{
  if ( aValue < 0 ) return EwMathCeil ( aValue );
  else              return EwMathFloor( aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwMathFract
*
* DESCRIPTION:
*   The function EwMathFract() implements the Chora math_fract() built-in
*   function intended to calculate the fractional part of the given number.
*
* ARGUMENTS:
*   aValue : The value to calculate the fractional part.
*
* RETURN VALUE:
*   Returns the fractional part of the number.
*
*******************************************************************************/
XFloat EwMathFract( XFloat aValue )
{
  if ( aValue < 0 ) return aValue - EwMathCeil ( aValue );
  else              return aValue - EwMathFloor( aValue );
}


/* pba, msy */

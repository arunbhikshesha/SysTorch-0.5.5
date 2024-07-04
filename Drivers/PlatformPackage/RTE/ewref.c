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
*   The module 'ewref.c' contains a set of EWRTE functions purpose-built for 
*   operations with the Chora reference datatypes. References are very usefull
*   to build a 'pointer' to a property of an object. Similary to a 'C' pointer
*   it is possible to access the property by simple indirection the reference.
*
*   Because an access to a property causes a call to the properties own OnGet 
*   or OnSet method, it is necessary that a single reference has to store one
*   pointer to the OnGet method, one to the OnSet method and one pointer to
*   the object itself, where the property is contained in.
*
* SUBROUTINES:
*   EwNewRef
*   EwCompRef
*   EwIsRefNull
*   EwOnGetInt8
*   EwOnGetInt16
*   EwOnGetInt32
*   EwOnGetInt64
*   EwOnGetUInt8
*   EwOnGetUInt16
*   EwOnGetUInt32
*   EwOnGetUInt64
*   EwOnGetBool
*   EwOnGetFloat
*   EwOnGetChar
*   EwOnGetString
*   EwOnGetPoint
*   EwOnGetRect
*   EwOnGetColor
*   EwOnGetEnum
*   EwOnGetSet
*   EwOnGetObject
*   EwOnGetSlot
*   EwOnGetLangId
*   EwOnGetStylesSet
*   EwOnGetClass
*   EwOnGetHandle
*   EwOnSetInt8
*   EwOnSetInt16
*   EwOnSetInt32
*   EwOnSetInt64
*   EwOnSetUInt8
*   EwOnSetUInt16
*   EwOnSetUInt32
*   EwOnSetUInt64
*   EwOnSetBool
*   EwOnSetFloat
*   EwOnSetChar
*   EwOnSetString
*   EwOnSetPoint
*   EwOnSetRect
*   EwOnSetColor
*   EwOnSetEnum
*   EwOnSetSet
*   EwOnSetObject
*   EwOnSetSlot
*   EwOnSetLangId
*   EwOnSetStylesSet
*   EwOnSetClass
*   EwOnSetHandle
*
*******************************************************************************/

#include "ewrte.h"


/* Undo the macro overloading of following functions */
#undef EwMarkRefs
#undef EwNewRef

/* EwNullRef stores a constant 'null' reference, so it is possible to use it
   immediatelly in assignments or comparations */
const XRef EwNullRef = { 0, 0, 0 };

/* Prototypes for OnGetXXX() methods - will be used to call an OnGetXXX() 
   method (property read access) */
typedef XInt8     (*XOnGetInt8     )( void* aThis );
typedef XInt16    (*XOnGetInt16    )( void* aThis );
typedef XInt32    (*XOnGetInt32    )( void* aThis );
typedef XInt64    (*XOnGetInt64    )( void* aThis );
typedef XUInt8    (*XOnGetUInt8    )( void* aThis );
typedef XUInt16   (*XOnGetUInt16   )( void* aThis );
typedef XUInt32   (*XOnGetUInt32   )( void* aThis );
typedef XUInt64   (*XOnGetUInt64   )( void* aThis );
typedef XBool     (*XOnGetBool     )( void* aThis );
typedef XFloat    (*XOnGetFloat    )( void* aThis );
typedef XChar     (*XOnGetChar     )( void* aThis );
typedef XString   (*XOnGetString   )( void* aThis );
typedef XPoint    (*XOnGetPoint    )( void* aThis );
typedef XRect     (*XOnGetRect     )( void* aThis );
typedef XColor    (*XOnGetColor    )( void* aThis );
typedef XEnum     (*XOnGetEnum     )( void* aThis );
typedef XSet      (*XOnGetSet      )( void* aThis );
typedef XObject   (*XOnGetObject   )( void* aThis );
typedef XSlot     (*XOnGetSlot     )( void* aThis );
typedef XLangId   (*XOnGetLangId   )( void* aThis );
typedef XStylesSet(*XOnGetStylesSet)( void* aThis );
typedef XClass    (*XOnGetClass    )( void* aThis );
typedef XHandle   (*XOnGetHandle   )( void* aThis );

/* Prototypes for OnSetXXX() methods - will be used to call an OnSetXXX() 
   method (property write access) */
typedef void (*XOnSetInt8     )( void* aThis, XInt8      aValue );
typedef void (*XOnSetInt16    )( void* aThis, XInt16     aValue );
typedef void (*XOnSetInt32    )( void* aThis, XInt32     aValue );
typedef void (*XOnSetInt64    )( void* aThis, XInt64     aValue );
typedef void (*XOnSetUInt8    )( void* aThis, XUInt8     aValue );
typedef void (*XOnSetUInt16   )( void* aThis, XUInt16    aValue );
typedef void (*XOnSetUInt32   )( void* aThis, XUInt32    aValue );
typedef void (*XOnSetUInt64   )( void* aThis, XUInt64    aValue );
typedef void (*XOnSetBool     )( void* aThis, XBool      aValue );
typedef void (*XOnSetFloat    )( void* aThis, XFloat     aValue );
typedef void (*XOnSetChar     )( void* aThis, XChar      aValue );
typedef void (*XOnSetString   )( void* aThis, XString    aValue );
typedef void (*XOnSetPoint    )( void* aThis, XPoint     aValue );
typedef void (*XOnSetRect     )( void* aThis, XRect      aValue );
typedef void (*XOnSetColor    )( void* aThis, XColor     aValue );
typedef void (*XOnSetEnum     )( void* aThis, XEnum      aValue );
typedef void (*XOnSetSet      )( void* aThis, XSet       aValue );
typedef void (*XOnSetObject   )( void* aThis, XObject    aValue );
typedef void (*XOnSetSlot     )( void* aThis, XSlot      aValue );
typedef void (*XOnSetLangId   )( void* aThis, XLangId    aValue );
typedef void (*XOnSetStylesSet)( void* aThis, XStylesSet aValue );
typedef void (*XOnSetClass    )( void* aThis, XClass     aValue );
typedef void (*XOnSetHandle   )( void* aThis, XHandle    aValue );


/*******************************************************************************
* FUNCTION:
*   EwNewRef
*
* DESCRIPTION:
*   The function EwNewRef() initializes a new XRef structure with the given
*   arguments aObject, aOnGetProc, aOnSetProc and returns the structure.
*
* ARGUMENTS:
*   aObject    - Pointer to an object which contains the referenced property.
*   aOnGetProc - Pointer to the properties own OnGet() method.
*   aOnSetProc - Pointer to the properties own OnSet() method.
*
* RETURN VALUE:
*   Returns an initialized XRef structure, describing a reference to a 
*   property.
*
*******************************************************************************/
XRef EwNewRef( void* aObject, XRefGetSetProc aOnGetProc, XRefGetSetProc aOnSetProc )
{
  XRef r;

  r.Object = aObject;
  r.OnGet  = aOnGetProc;
  r.OnSet  = aOnSetProc;

  return r;
}


/*******************************************************************************
* FUNCTION:
*   EwCompRef
*
* DESCRIPTION:
*   The function EwCompRef() compares the given references aRef1 and aRef2 
*   and returns a value == 0 if the references are identical. Otherwise the 
*   function returns a value != 0.
*
*   EwCompRef() implements the Chora instant operators: 'ref == ref' and
*   'ref != ref'.
*
* ARGUMENTS:
*   aRef1 - The first reference.
*   aRef2 - The second reference.
*
* RETURN VALUE:
*   Returns zero if both references are equal.
*
*******************************************************************************/
int EwCompRef( XRef aRef1, XRef aRef2 )
{
  if ( aRef1.Object != aRef2.Object ) return 1;
  if ( aRef1.OnGet  != aRef2.OnGet  ) return 1;
  if ( aRef1.OnSet  != aRef2.OnSet  ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsRefNull
*
* DESCRIPTION:
*   The function EwIsRefNull() returns != 0 if the given reference aRef does 
*   not refer any valid property (it is NULL). Otherwise the function returns
*   0.
*
* ARGUMENTS:
*   aRef - Reference to verify.
*
* RETURN VALUE:
*   Returns != 0 if the reference is NULL.
*
*******************************************************************************/
XBool EwIsRefNull( XRef aRef )
{
  return !aRef.Object && !aRef.OnGet && !aRef.OnSet;
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetInt8
*
* DESCRIPTION:
*   The function EwOnGetInt8() will be called in order to read an int8 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an int8 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XInt8 EwOnGetInt8( XRef aRef )
{
  return ((XOnGetInt8)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetInt16
*
* DESCRIPTION:
*   The function EwOnGetInt16() will be called in order to read an int16
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an int16 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XInt16 EwOnGetInt16( XRef aRef )
{
  return ((XOnGetInt16)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetInt32
*
* DESCRIPTION:
*   The function EwOnGetInt32() will be called in order to read an int32 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an int32 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XInt32 EwOnGetInt32( XRef aRef )
{
  return ((XOnGetInt32)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetInt64
*
* DESCRIPTION:
*   The function EwOnGetInt64() will be called in order to read an int64 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an int64 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XInt64 EwOnGetInt64( XRef aRef )
{
  return ((XOnGetInt64)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetUInt8
*
* DESCRIPTION:
*   The function EwOnGetUInt8() will be called in order to read an uint8 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an uint8 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XUInt8 EwOnGetUInt8( XRef aRef )
{
  return ((XOnGetUInt8)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetUInt16
*
* DESCRIPTION:
*   The function EwOnGetUInt16() will be called in order to read an uint16 
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an uint16 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XUInt16 EwOnGetUInt16( XRef aRef )
{
  return ((XOnGetUInt16)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetUInt32
*
* DESCRIPTION:
*   The function EwOnGetUInt32() will be called in order to read an uint32 
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an uint32 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XUInt32 EwOnGetUInt32( XRef aRef )
{
  return ((XOnGetUInt32)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetUInt64
*
* DESCRIPTION:
*   The function EwOnGetUInt64() will be called in order to read an uint64 
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an uint64 property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XUInt64 EwOnGetUInt64( XRef aRef )
{
  return ((XOnGetUInt64)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetBool
*
* DESCRIPTION:
*   The function EwOnGetBool() will be called in order to read a bool property 
*   referenced by the aRef argument. The access to the property will be done 
*   by calling the properties own OnGet() method. The value returned from the 
*   OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a bool property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XBool EwOnGetBool( XRef aRef )
{
  return ((XOnGetBool)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetFloat
*
* DESCRIPTION:
*   The function EwOnGetFloat() will be called in order to read a float 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a float property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XFloat EwOnGetFloat( XRef aRef )
{
  return ((XOnGetFloat)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetChar
*
* DESCRIPTION:
*   The function EwOnGetChar() will be called in order to read a char 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a char property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XChar EwOnGetChar( XRef aRef )
{
  return ((XOnGetChar)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetString
*
* DESCRIPTION:
*   The function EwOnGetString() will be called in order to read a string 
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a string property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XString EwOnGetString( XRef aRef )
{
  return ((XOnGetString)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetPoint
*
* DESCRIPTION:
*   The function EwOnGetPoint() will be called in order to read a point 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a point property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XPoint EwOnGetPoint( XRef aRef )
{
  return ((XOnGetPoint)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetRect
*
* DESCRIPTION:
*   The function EwOnGetRect() will be called in order to read a rect 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a rect property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XRect EwOnGetRect( XRef aRef )
{
  return ((XOnGetRect)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetColor
*
* DESCRIPTION:
*   The function EwOnGetColor() will be called in order to read a color 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a color property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XColor EwOnGetColor( XRef aRef )
{
  return ((XOnGetColor)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetEnum
*
* DESCRIPTION:
*   The function EwOnGetEnum() will be called in order to read an enum 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to an enum property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XEnum EwOnGetEnum( XRef aRef )
{
  return ((XOnGetEnum)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetSet
*
* DESCRIPTION:
*   The function EwOnGetSet() will be called in order to read a set 
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a set property.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XSet EwOnGetSet( XRef aRef )
{
  return ((XOnGetSet)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetObject
*
* DESCRIPTION:
*   The function EwOnGetObject() will be called in order to read a object 
*   property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnGet() method. The value 
*   returned from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing a pointer to an object.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XObject EwOnGetObject( XRef aRef )
{
  return ((XOnGetObject)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetSlot
*
* DESCRIPTION:
*   The function EwOnGetSlot() will be called in order to read a slot property 
*   referenced by the aRef argument. The access to the property will be done by
*   calling the properties own OnGet() method. The value returned from the 
*   OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing the slot.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XSlot EwOnGetSlot( XRef aRef )
{
  return ((XOnGetSlot)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetLangId
*
* DESCRIPTION:
*   The function EwOnGetLangId() will be called in order to read a language 
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing the language.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XLangId EwOnGetLangId( XRef aRef )
{
  return ((XOnGetLangId)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetStylesSet
*
* DESCRIPTION:
*   The function EwOnGetStylesSet() will be called in order to read a styles
*   property referenced by the aRef argument. The access to the property will 
*   be done by calling the properties own OnGet() method. The value returned 
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing the styles set.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XStylesSet EwOnGetStylesSet( XRef aRef )
{
  return ((XOnGetStylesSet)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetClass
*
* DESCRIPTION:
*   The function EwOnGetClass() will be called in order to read a class property
*   referenced by the aRef argument. The access to the property will be done by
*   calling the properties own OnGet() method. The value returned from the 
*   OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing the class.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XClass EwOnGetClass( XRef aRef )
{
  return ((XOnGetClass)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnGetHandle
*
* DESCRIPTION:
*   The function EwOnGetHandle() will be called in order to read a handle
*   property referenced by the aRef argument. The access to the property will
*   be done by calling the properties own OnGet() method. The value returned
*   from the OnGet() method will be passed back to the caller.
*
* ARGUMENTS:
*   aRef - Reference to a property containing the handle.
*
* RETURN VALUE:
*   Returns the value of the property referenced by aRef.
*
*******************************************************************************/
XHandle EwOnGetHandle( XRef aRef )
{
  return ((XOnGetHandle)aRef.OnGet)( aRef.Object );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetInt8
*
* DESCRIPTION:
*   The function EwOnSetInt8() will be called in order to assign a value to an
*   int8 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an int8 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetInt8( XRef aRef, XInt8 aValue )
{
  ((XOnSetInt8)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetInt16
*
* DESCRIPTION:
*   The function EwOnSetInt16() will be called in order to assign a value to an
*   int16 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an int16 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetInt16( XRef aRef, XInt16 aValue )
{
  ((XOnSetInt16)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetInt32
*
* DESCRIPTION:
*   The function EwOnSetInt32() will be called in order to assign a value to an
*   int32 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an int32 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetInt32( XRef aRef, XInt32 aValue )
{
  ((XOnSetInt32)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetInt64
*
* DESCRIPTION:
*   The function EwOnSetInt64() will be called in order to assign a value to an
*   int64 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an int64 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetInt64( XRef aRef, XInt64 aValue )
{
  ((XOnSetInt64)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetUInt8
*
* DESCRIPTION:
*   The function EwOnSetUInt8() will be called in order to assign a value to an
*   uint8 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an uint8 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetUInt8( XRef aRef, XUInt8 aValue )
{
  ((XOnSetUInt8)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetUInt16
*
* DESCRIPTION:
*   The function EwOnSetUInt16() will be called in order to assign a value to an
*   uint16 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an uint16 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetUInt16( XRef aRef, XUInt16 aValue )
{
  ((XOnSetUInt16)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetUInt32
*
* DESCRIPTION:
*   The function EwOnSetUInt32() will be called in order to assign a value to an
*   uint32 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an uint32 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetUInt32( XRef aRef, XUInt32 aValue )
{
  ((XOnSetUInt32)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetUInt64
*
* DESCRIPTION:
*   The function EwOnSetUInt64() will be called in order to assign a value to an
*   uint64 property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an uint64 property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetUInt64( XRef aRef, XUInt64 aValue )
{
  ((XOnSetUInt64)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetBool
*
* DESCRIPTION:
*   The function EwOnSetBool() will be called in order to assign a value to a
*   bool property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a bool property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetBool( XRef aRef, XBool aValue )
{
  ((XOnSetBool)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetFloat
*
* DESCRIPTION:
*   The function EwOnSetFloat() will be called in order to assign a value to a
*   float property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a float property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetFloat( XRef aRef, XFloat aValue )
{
  ((XOnSetFloat)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetChar
*
* DESCRIPTION:
*   The function EwOnSetChar() will be called in order to assign a value to a
*   char property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a char property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetChar( XRef aRef, XChar aValue )
{
  ((XOnSetChar)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetString
*
* DESCRIPTION:
*   The function EwOnSetString() will be called in order to assign a value to 
*   a string property referenced by the aRef argument. The access to the 
*   property will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a string property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetString( XRef aRef, XString aValue )
{
  ((XOnSetString)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetPoint
*
* DESCRIPTION:
*   The function EwOnSetPoint() will be called in order to assign a value to a
*   point property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a point property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetPoint( XRef aRef, XPoint aValue )
{
  XPoint tmp = aValue; /* to avoid compiler bug in GCC for X86 */
  ((XOnSetPoint)aRef.OnSet)( aRef.Object, tmp );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetRect
*
* DESCRIPTION:
*   The function EwOnSetRect() will be called in order to assign a value to a
*   rect property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a rect property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetRect( XRef aRef, XRect aValue )
{
  /* The 'tmp' variable is a workaround for a GCC ARM compiler bug. This bug
     cause the Y1 value of the struct to be overriden by its Y1 sibling. */
  XRect tmp = aValue;
  ((XOnSetRect)aRef.OnSet)( aRef.Object, tmp );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetColor
*
* DESCRIPTION:
*   The function EwOnSetColor() will be called in order to assign a value to a
*   color property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a color property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetColor( XRef aRef, XColor aValue )
{
  ((XOnSetColor)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetEnum
*
* DESCRIPTION:
*   The function EwOnSetEnum() will be called in order to assign a value to an
*   enum property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to an enum property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetEnum( XRef aRef, XEnum aValue )
{
  ((XOnSetEnum)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetSet
*
* DESCRIPTION:
*   The function EwOnSetSet() will be called in order to assign a value to a
*   set property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a set property.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetSet( XRef aRef, XSet aValue )
{
  ((XOnSetSet)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetObject
*
* DESCRIPTION:
*   The function EwOnSetObject() will be called in order to assign a value to 
*   an object property referenced by the aRef argument. The access to the 
*   property will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing a pointer to an object.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetObject( XRef aRef, XObject aValue )
{
  ((XOnSetObject)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetSlot
*
* DESCRIPTION:
*   The function EwOnSetSlot() will be called in order to assign a value to a 
*   slot property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing the slot.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetSlot( XRef aRef, XSlot aValue )
{
  XSlot tmp = aValue; /* to avoid compiler bug in GCC for X86 */
  ((XOnSetSlot)aRef.OnSet)( aRef.Object, tmp );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetLangId
*
* DESCRIPTION:
*   The function EwOnSetLangId() will be called in order to assign a value to a 
*   language property referenced by the aRef argument. The access to the 
*   property will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing the language.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetLangId( XRef aRef, XLangId aValue )
{
  ((XOnSetLangId)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetStylesSet
*
* DESCRIPTION:
*   The function EwOnSetStylesSet() will be called in order to assign a value 
*   to a styles property referenced by the aRef argument. The access to the 
*   property will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing the styles set.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetStylesSet( XRef aRef, XStylesSet aValue )
{
  ((XOnSetStylesSet)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetClass
*
* DESCRIPTION:
*   The function EwOnSetClass() will be called in order to assign a value to a 
*   class property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing the class.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetClass( XRef aRef, XClass aValue )
{
  ((XOnSetClass)aRef.OnSet)( aRef.Object, aValue );
}


/*******************************************************************************
* FUNCTION:
*   EwOnSetHandle
*
* DESCRIPTION:
*   The function EwOnSetHandle() will be called in order to assign a value to a 
*   handle property referenced by the aRef argument. The access to the property 
*   will be done by calling the properties own OnSet() method.
*
* ARGUMENTS:
*   aRef   - Reference to a property containing the handle.
*   aValue - The value to be assigned to the property.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwOnSetHandle( XRef aRef, XHandle aValue )
{
  ((XOnSetHandle)aRef.OnSet)( aRef.Object, aValue );
}


/* pba, msy */

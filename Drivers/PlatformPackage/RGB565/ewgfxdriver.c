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
*   The module 'ewgfxdriver' implements the platform independent layer to the
*   software pixel driver. Its functionality provides a kind of reserve in case
*   the underlying graphics subsystem can not handle the necessary operations.
*   For example, if there is no support for index8 surfaces, 'ewgfxdriver' will
*   recompense it by a software emulated index8 surface including all necessary
*   drawing operations.
*
*   This header file provides a generic and platform independent interface to
*   the software pixel driver functionality. The interface consists of two
*   categories of functions:
*
*   1. 'per row' worker functions. These functions build the low-level software
*      pixel driver. They are intended to perform drawing operations optimized
*      for the particular pixel format and the drawing mode. The declarations,
*      any way, are platform independent. 
*
*   2. 'per area' drawing functions. These functions provide the top-level view
*      to the software pixel driver. They are intended to drive the 'per row'
*      worker functions for the affected fill/copy/warp operation. When called,
*      the top-level function will receive a pointer to one of the low-level
*      worker functions. The passed worker function can be used thereupon to
*      perform row-wise the operation without any dependency to the underlying
*      pixel format.
*
* SUBROUTINES:
*   EwCreateIndex8Surface
*   EwCreateConstIndex8Surface
*   EwDestroyIndex8Surface
*   EwGetIndex8SurfaceMemory
*   EwGetIndex8SurfaceMemSize
*   EwCreateAlpha8Surface
*   EwCreateConstAlpha8Surface
*   EwDestroyAlpha8Surface
*   EwGetAlpha8SurfaceMemory
*   EwGetAlpha8SurfaceMemSize
*   EwInitColorGradient
*   EwInitOpacityGradient
*   EwGetColorFromGradient
*   EwGetOpacityFromGradient
*   EwRasterAlpha8Polygon
*   EwEmulateLine
*   EwEmulateFill
*   EwEmulateCopy
*   EwEmulateWarp
*   EwEmulateFillPolygon
*   EwAllocVideo
*   EwFreeVideo
*
*******************************************************************************/

#include "ewrte.h"
#include "ewgfxdriver.h"


/* This is an internal representation of a software only index8 surface. */
typedef struct
{
  int               Width;
  int               Height;
  unsigned char*    Pixel;
  unsigned int*     Clut;
  int               PitchY;
  int               IsConst;
} XIndex8Surface;


/* This is an internal representation of a software only alpha8 surface. */
typedef struct
{
  int               Width;
  int               Height;
  unsigned char*    Pixel;
  int               PitchY;
  int               IsConst;
} XAlpha8Surface;


/* Memory usage profiler */
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwMemoryPeak;


/* Helper macros for minimum and maximum calculation */
#ifdef MIN
  #undef MIN
#endif
#define MIN( a, b ) (((a) < (b))? (a) : (b))

#ifdef MAX
  #undef MAX
#endif
#define MAX( a, b ) (((a) > (b))? (a) : (b))

#ifdef ABS
  #undef ABS
#endif
#define ABS( a ) (((a) < 0 )? (-(a)) : (a))


/* Helper function for minimum and maximum calculation */
static int MIN4( int a, int b, int c, int d );
static int MAX4( int a, int b, int c, int d );


/* If the memory of constant surfaces is not accessible by the CPU, use a
   'Reader' to load and map the pixel pointers.  */
#ifdef EW_USE_READER_FOR_CONST_SURFACES 
  extern XFlashAreaReaderProc EwFlashAreaReaderProc;
#endif


/*******************************************************************************
* FUNCTION:
*   EwCreateIndex8Surface
*
* DESCRIPTION:
*   The function EwCreateIndex8Surface() has the job to create and return a new
*   index8 surface with the given size. If no surface could be created due to
*   memory deficit, 0 is returned.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function creates a pure software surface
*   within the CPU address space only.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface to create.
*
* RETURN VALUE:
*   If successful, returns a handle to the new surface. Otherwise 0 is returned.
*
*******************************************************************************/
unsigned long EwCreateIndex8Surface( int aWidth, int aHeight )
{
  int             size1   = aWidth * aHeight;
  int             size2   = 256 * sizeof( unsigned int );
  XIndex8Surface* surface = EwAllocVideo( sizeof( XIndex8Surface ) + size1 + size2 );

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Clut    = (unsigned int*)( surface + 1 );
    surface->Pixel   = (unsigned char*)( surface->Clut + 256 );
    surface->PitchY  = aWidth;
    surface->IsConst = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size1 + size2 + sizeof( XIndex8Surface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/*******************************************************************************
* FUNCTION:
*   EwCreateConstIndex8Surface
*
* DESCRIPTION:
*   The function EwCreateConstIndex8Surface() has the job to create and return
*   a new index8 surface with the given size and pixel data content. Important
*   here is the fact that the function doesn't reserve any memory to store the
*   pixel information. Instead it, the surface associates the pixel data passed
*   in the parameter aMemory.
*
*   This function is thus intended to create surfaces from the pixel data stored
*   directly within a ROM area. Accordingly it is essential that the ROM content
*   does exactly correspond to the index8 surface pixel format. It includes the
*   order of CLUT color channels, premultiplication with alpha value, etc..
*
*   If no surface could be created due to memory deficit, 0 is returned.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality.
*
*   Please note, the surfaces created with this function are signed internally
*   as constant. Trying to obtrain write-access to such surface will result in
*   a runtime error.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface to create.
*   aMemory - Structure to pass the ROM address information.
*
* RETURN VALUE:
*   If successful, returns a handle to the new surface. Otherwise 0 is returned.
*
*******************************************************************************/
unsigned long EwCreateConstIndex8Surface( int aWidth, int aHeight,
  XSurfaceMemory* aMemory )
{
  XIndex8Surface* surface;

  /* Verify superficially the passed pixel memory */
  if ( !aMemory || !aMemory->Pixel1 || !aMemory->Clut ||
     ( aMemory->Pitch1X != 1 ) || ( aMemory->Pitch1Y < aWidth ))
    return 0;

  /* Reserve memory for the descriptor of the surface */
  surface = EwAllocVideo( sizeof( XIndex8Surface ));

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Clut    = aMemory->Clut;
    surface->Pixel   = aMemory->Pixel1;
    surface->PitchY  = aMemory->Pitch1Y;
    surface->IsConst = 1;

    /* Track the RAM usage */
    EwResourcesMemory += sizeof( XIndex8Surface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/*******************************************************************************
* FUNCTION:
*   EwDestroyIndex8Surface
*
* DESCRIPTION:
*   The function EwDestroyIndex8Surface() has the job to release resources of
*   a previously created index8 surface.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function destroys pure software surfaces
*   previously created by the function EwCreateIndex8Surface() only.
*
* ARGUMENTS:
*   aSurface - Handle to the surface to destroy.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDestroyIndex8Surface( unsigned long aSurface )
{
  XIndex8Surface* surface = (XIndex8Surface*)aSurface;

  /* Track the RAM usage */
  if ( surface && !surface->IsConst )
  {
    EwResourcesMemory -= 256 * sizeof( unsigned int );
    EwResourcesMemory -= surface->Width * surface->Height;
  }

  /* Track the RAM usage */
  if ( surface )
    EwResourcesMemory -= sizeof( XIndex8Surface );

  EwFreeVideo( surface );
}


/*******************************************************************************
* FUNCTION:
*   EwGetIndex8SurfaceMemory
*
* DESCRIPTION:
*   The function EwGetIndex8SurfaceMemory() has the job to obtain the pointer to
*   the index8 memory plane and to the color table of the given surface.
*
*   The returned pointer is calculated for the given pixel position aX, aY and
*   in case of the color table, to the entry at the position aIndex.
*
*   Please note, if the affected surface has been created with constant pixel
*   information by using EwCreateConstIndex8Surface(), you can't obtain the
*   write access to such surface. In such case the function will fail with the
*   return value 0.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function can handle with the pure software
*   surfaces previously created by the function EwCreateIndex8Surface() and
*   EwCreateConstIndex8Surface() only.
*
* ARGUMENTS:
*   aSurface     - Handle to the surface to obtain the addresses.
*   aX, aY       - Position within the surface to calculate the pixel address.
*   aIndex       - Index within the color table to calculate the CLUT address.
*   aWriteAccess - If != 0 the caller has the intention to modify the returned
*     memory content.
*   aMemory      - Structure to receive the desired address information.
*
* RETURN VALUE:
*   If successful, the function fills the given aMemory structure with the
*   address information and returns != 0. Otherwise 0 is returned.
*
*******************************************************************************/
int EwGetIndex8SurfaceMemory( unsigned long aSurface, int aX, int aY,
  int aIndex, int aWriteAccess, XSurfaceMemory* aMemory )
{
  XIndex8Surface* surface = (XIndex8Surface*)aSurface;

  /* Can't modify constant surface pixel data */
  if ( aWriteAccess && surface->IsConst )
    return 0;

  aMemory->Pixel1  = surface->Pixel + ( aY * surface->PitchY ) + aX;
  aMemory->Pitch1Y = surface->PitchY;
  aMemory->Pitch1X = 1;
  aMemory->Pixel2  = 0;
  aMemory->Pitch2Y = 0;
  aMemory->Pitch2X = 0;
  aMemory->Clut    = surface->Clut + aIndex;

  /* If the memory of constant surfaces is not accessible by the CPU, use a
     'Reader' to load and map the pixel pointers.  */
  #ifdef EW_USE_READER_FOR_CONST_SURFACES 
    aMemory->Reader = ( surface->IsConst )? EwFlashAreaReaderProc : 0;
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwGetIndex8SurfaceMemSize
*
* DESCRIPTION:
*   The function EwGetIndex8SurfaceMemSize() has the job to determine the number
*   of memory bytes occupied by an index8 surface with the given size.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface in pixel.
*
* RETURN VALUE:
*   If successful, the function return the surface size in bytes.
*
*******************************************************************************/
int EwGetIndex8SurfaceMemSize( int aWidth, int aHeight )
{
  return ( aWidth * aHeight ) + ( 256 * sizeof( unsigned int )) + 
           sizeof( XIndex8Surface );
}


/*******************************************************************************
* FUNCTION:
*   EwCreateAlpha8Surface
*
* DESCRIPTION:
*   The function EwCreateAlpha8Surface() has the job to create and return a new
*   alpha8 surface with the given size. If no surface could be created due to
*   memory deficit, 0 is returned.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function creates a pure software surface
*   within the CPU address space only.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface to create.
*
* RETURN VALUE:
*   If successful, returns a handle to the new surface. Otherwise 0 is returned.
*
*******************************************************************************/
unsigned long EwCreateAlpha8Surface( int aWidth, int aHeight )
{
  int             size    = aWidth * aHeight;
  XAlpha8Surface* surface = EwAllocVideo( sizeof( XAlpha8Surface ) + size );

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Pixel   = (unsigned char*)( surface + 1 );
    surface->PitchY  = aWidth;
    surface->IsConst = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size + sizeof( XAlpha8Surface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/*******************************************************************************
* FUNCTION:
*   EwCreateConstAlpha8Surface
*
* DESCRIPTION:
*   The function EwCreateConstAlpha8Surface() has the job to create and return
*   a new alpha8 surface with the given size and pixel data content. Important
*   here is the fact that the function doesn't reserve any memory to store the
*   pixel information. Instead it, the surface associates the pixel data passed
*   in the parameter aMemory.
*
*   This function is thus intended to create surfaces from the pixel data stored
*   directly within a ROM area. Accordingly it is essential that the ROM content
*   does exactly correspond to the alpha8 surface pixel format.
*
*   If no surface could be created due to memory deficit, 0 is returned.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality.
*
*   Please note, the surfaces created with this function are signed internally
*   as constant. Trying to obtrain write-access to such surface will result in
*   a runtime error.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface to create.
*   aMemory - Structure to pass the ROM address information.
*
* RETURN VALUE:
*   If successful, returns a handle to the new surface. Otherwise 0 is returned.
*
*******************************************************************************/
unsigned long EwCreateConstAlpha8Surface( int aWidth, int aHeight,
  XSurfaceMemory* aMemory )
{
  XAlpha8Surface* surface;

  /* Verify superficially the passed pixel memory */
  if ( !aMemory || !aMemory->Pixel1 || ( aMemory->Pitch1X != 1 ) ||
     ( aMemory->Pitch1Y < aWidth ))
    return 0;

  /* Reserve memory for the descriptor of the surface */
  surface = EwAllocVideo( sizeof( XAlpha8Surface ));

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Pixel   = aMemory->Pixel1;
    surface->PitchY  = aMemory->Pitch1Y;
    surface->IsConst = 1;

    /* Track the RAM usage */
    EwResourcesMemory += sizeof( XAlpha8Surface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/*******************************************************************************
* FUNCTION:
*   EwDestroyAlpha8Surface
*
* DESCRIPTION:
*   The function EwDestroyAlphaSurface() has the job to release resources of
*   a previously created alpha8 surface.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function destroys pure software surfaces
*   previously created by the function EwCreateAlpha8Surface() only.
*
* ARGUMENTS:
*   aSurface - Handle to the surface to destroy.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDestroyAlpha8Surface( unsigned long aSurface )
{
  XAlpha8Surface* surface = (XAlpha8Surface*)aSurface;

  /* Track the RAM usage */
  if ( aSurface )
    EwResourcesMemory -= sizeof( XAlpha8Surface ) + ( surface->IsConst? 0 :
                               ( surface->Width * surface->Height ));

  EwFreeVideo( surface );
}


/*******************************************************************************
* FUNCTION:
*   EwGetAlpha8SurfaceMemory
*
* DESCRIPTION:
*   The function EwGetAlpha8SurfaceMemory() has the job to obtain pointers to
*   the memory planes of the given surface.
*
*   The returned pointer is calculated for the given pixel position aX, aY.
*
*   Please note, if the affected surface has been created with constant pixel
*   information by using EwCreateConstAlpha8Surface(), you can't obtain the
*   write access to such surface. In such case the function will fail with the
*   return value 0.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function can handle with the pure software
*   surfaces previously created by the function EwCreateIAlpha8Surface() and
*   EwCreateConstAlpha8Surface() only.
*
* ARGUMENTS:
*   aSurface     - Handle to the surface to obtain the addresses.
*   aX, aY       - Position within the surface to calculate the pixel address.
*   aIndex       - Index within the color table to calculate the CLUT address.
*   aWriteAccess - If != 0 the caller has the intention to modify the returned
*     memory content.
*   aMemory      - Structure to receive the desired address information.
*
* RETURN VALUE:
*   If successful, the function fills the given aMemory structure with the
*   address information and returns != 0. Otherwise 0 is returned.
*
*******************************************************************************/
int EwGetAlpha8SurfaceMemory( unsigned long aSurface, int aX, int aY, 
  int aIndex, int aWriteAccess, XSurfaceMemory* aMemory )
{
  XAlpha8Surface* surface = (XAlpha8Surface*)aSurface;

  EW_UNUSED_ARG( aIndex );

  /* Can't modify constant surface pixel data */
  if ( aWriteAccess && surface->IsConst )
    return 0;

  aMemory->Pixel1  = surface->Pixel + ( aY * surface->PitchY ) + aX;
  aMemory->Pitch1Y = surface->PitchY;
  aMemory->Pitch1X = 1;
  aMemory->Pixel2  = 0;
  aMemory->Pitch2Y = 0;
  aMemory->Pitch2X = 0;
  aMemory->Clut    = 0;

  /* If the memory of constant surfaces is not accessible by the CPU, use a
     'Reader' to load and map the pixel pointers.  */
  #ifdef EW_USE_READER_FOR_CONST_SURFACES 
    aMemory->Reader = ( surface->IsConst )? EwFlashAreaReaderProc : 0;
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwGetAlpha8SurfaceMemSize
*
* DESCRIPTION:
*   The function EwGetAlpha8SurfaceMemSize() has the job to determine the number
*   of memory bytes occupied by an alpha8 surface with the given size.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface in pixel.
*
* RETURN VALUE:
*   If successful, the function return the surface size in bytes.
*
*******************************************************************************/
int EwGetAlpha8SurfaceMemSize( int aWidth, int aHeight )
{
  return aWidth * aHeight + sizeof( XAlpha8Surface );
}


/*******************************************************************************
* FUNCTION:
*   EwInitColorGradient
*
* DESCRIPTION:
*   The function EwInitColorGradient() has the job to initialize a rectangular,
*   linear gradient from 4 color values. The color values do correspond to the
*   four corners of the rectangular gradient: top-left, top-right, bottom-right
*   and bottom-left.
*
* ARGUMENTS:
*   aWidth,
*   aHeight   - Size of the gradient area in pixel.
*   aColors   - Array of 4 color values in the universal RGBA8888 color format.
*   aGradient - Destination gradient structure to initialize.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwInitColorGradient( int aWidth, int aHeight, unsigned int* aColors,
  XGradient* aGradient )
{
  unsigned int mask         = 0xFF;
  unsigned int c1           = aColors[0];
  unsigned int c2           = aColors[1];
  unsigned int c3           = aColors[3];
  unsigned int c4           = aColors[2];
  int          invWidth     = 1 << 20;
  int          invHeight    = 1 << 20;
  int          isVertical   = ( c1 != c3 ) || ( c2 != c4 );
  int          isHorizontal = ( c1 != c2 ) || ( c3 != c4 );

  /* For the slope calculation - get the 1/width and 1/height reciprocal 
     values */
  if ( aWidth  && isHorizontal ) invWidth  /= aWidth;
  if ( aHeight && isVertical   ) invHeight /= aHeight;

  aGradient->IsHorizontal = isHorizontal;
  aGradient->IsVertical   = isVertical;
  aGradient->InvWidth     = invWidth;
  aGradient->InvHeight    = invHeight;
  aGradient->Width        = aWidth;
  aGradient->Height       = aHeight;

  /* Calculate the components for the red channel */
  aGradient->R0 = ( c1 & mask ) << 20;
  aGradient->R1 = ( c3 & mask ) - ( c1 & mask );
  aGradient->R2 = ( c2 & mask ) << 20;
  aGradient->R3 = ( c4 & mask ) - ( c2 & mask );

  /* Calculate the components for the green channel */
  mask <<= 8;
  aGradient->G0 =  ( c1 & mask ) << 12;
  aGradient->G1 = (( c3 & mask ) >> 8 ) - (( c1 & mask ) >> 8 );
  aGradient->G2 =  ( c2 & mask ) << 12;
  aGradient->G3 = (( c4 & mask ) >> 8 ) - (( c2 & mask ) >> 8 );

  /* Calculate the components for the blue channel */
  mask <<= 8;
  aGradient->B0 =  ( c1 & mask ) << 4;
  aGradient->B1 = (( c3 & mask ) >> 16 ) - (( c1 & mask ) >> 16 );
  aGradient->B2 =  ( c2 & mask ) << 4;
  aGradient->B3 = (( c4 & mask ) >> 16 ) - (( c2 & mask ) >> 16 );

  /* Calculate the components for the alpha channel */
  mask <<= 8;
  aGradient->A0 =  ( c1 & mask ) >> 4;
  aGradient->A1 = (( c3 & mask ) >> 24 ) - (( c1 & mask ) >> 24 );
  aGradient->A2 =  ( c2 & mask ) >> 4;
  aGradient->A3 = (( c4 & mask ) >> 24 ) - (( c2 & mask ) >> 24 );

  /* Avoid slow multiplications if no vertical trend in the gradient */
  if ( isVertical )
  {
    aGradient->R1 *= invHeight; aGradient->R3 *= invHeight;
    aGradient->G1 *= invHeight; aGradient->G3 *= invHeight;
    aGradient->B1 *= invHeight; aGradient->B3 *= invHeight;
    aGradient->A1 *= invHeight; aGradient->A3 *= invHeight;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwInitOpacityGradient
*
* DESCRIPTION:
*   The function EwInitOpacityGradient() has the job to initialize a new linear,
*   rectangular gradient from 4 color values. The color values do correspond to
*   the four corners of the gradient area: top-left, top-right, bottom-right and
*   bottom-left.
*
* ARGUMENTS:
*   aWidth,
*   aHeight   - Size of the gradient area in pixel.
*   aColors   - Array of 4 color values in the universal RGBA8888 color format.
*   aGradient - Destination gradient structure to initialize.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwInitOpacityGradient( int aWidth, int aHeight, unsigned int* aColors,
  XGradient* aGradient )
{
  unsigned int mask         = 0xFF000000;
  unsigned int c1           = aColors[0];
  unsigned int c2           = aColors[1];
  unsigned int c3           = aColors[3];
  unsigned int c4           = aColors[2];
  int          invWidth     = 1 << 20;
  int          invHeight    = 1 << 20;
  int          isVertical   = ( c1 != c3 ) || ( c2 != c4 );
  int          isHorizontal = ( c1 != c2 ) || ( c3 != c4 );

  /* For the slope calculation - get the 1/width and 1/height reciprocal 
     values */
  if ( aWidth  && isHorizontal ) invWidth  /= aWidth;
  if ( aHeight && isVertical   ) invHeight /= aHeight;

  aGradient->IsHorizontal = isHorizontal;
  aGradient->IsVertical   = isVertical;
  aGradient->InvWidth     = invWidth;
  aGradient->InvHeight    = invHeight;
  aGradient->Width        = aWidth;
  aGradient->Height       = aHeight;

  /* Clear the RGB channels */
  aGradient->R0 = 0; aGradient->R1 = 0; aGradient->R2 = 0; aGradient->R3 = 0;
  aGradient->G0 = 0; aGradient->G1 = 0; aGradient->G2 = 0; aGradient->G3 = 0;
  aGradient->B0 = 0; aGradient->B1 = 0; aGradient->B2 = 0; aGradient->B3 = 0;

  /* Calculate the components for the alpha channel */
  aGradient->A0 =  ( c1 & mask ) >> 4;
  aGradient->A1 = (( c3 & mask ) >> 24 ) - (( c1 & mask ) >> 24 );
  aGradient->A2 =  ( c2 & mask ) >> 4;
  aGradient->A3 = (( c4 & mask ) >> 24 ) - (( c2 & mask ) >> 24 );

  /* Avoid slow multiplications if no vertical trend in the gradient */
  if ( isVertical )
  {
    aGradient->A1 *= invHeight;
    aGradient->A3 *= invHeight;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwGetColorFromGradient
*
* DESCRIPTION:
*   The function EwGetColorFromGradient() has the job to interpolate the color
*   value for the given position within a color gradient.
*
* ARGUMENTS:
*   aGradient  - Color gradient to get the color.
*   aX, aY     - Position relative to the upper-left corner of the gradient to 
*     get the color value.    
*
* RETURN VALUE:
*   Interpolated color value in the universal RGBA8888 color format.
*
*******************************************************************************/
unsigned int EwGetColorFromGradient( XGradient* aGradient, int aX, int aY )
{
  int mask1 = 0x0FF00000;
  int rl    = aGradient->R0;
  int gl    = aGradient->G0;
  int bl    = aGradient->B0;
  int al    = aGradient->A0;
  int rr    = aGradient->R2;
  int gr    = aGradient->G2;
  int br    = aGradient->B2;
  int ar    = aGradient->A2;

  /* Limit the coordinates to the gradient area */
  if ( aX < 0 ) aX = 0;
  if ( aY < 0 ) aY = 0;
  if ( aX > aGradient->Width  ) aX = aGradient->Width;
  if ( aY > aGradient->Height ) aY = aGradient->Height;

  /* Interpolate the gradient values vertically */
  if ( aGradient->IsVertical && aY )
  {
    rl += aY * aGradient->R1;
    gl += aY * aGradient->G1;
    bl += aY * aGradient->B1;
    al += aY * aGradient->A1;
  }

  /* Avoid unnecessary calculations for vertical only gradients */
  if ( aGradient->IsVertical && aGradient->IsHorizontal && aY )
  {
    rr += aY * aGradient->R3;
    gr += aY * aGradient->G3;
    br += aY * aGradient->B3;
    ar += aY * aGradient->A3;
  }

  /* Interpolate the gradient values horizontally */
  if ( aGradient->IsHorizontal && aX )
  {
    aX *= aGradient->InvWidth;
    rl  = ( rl & mask1 ) + (( rr >> 20 ) - ( rl >> 20 )) * aX;
    gl  = ( gl & mask1 ) + (( gr >> 20 ) - ( gl >> 20 )) * aX;
    bl  = ( bl & mask1 ) + (( br >> 20 ) - ( bl >> 20 )) * aX;
    al  = ( al & mask1 ) + (( ar >> 20 ) - ( al >> 20 )) * aX;
  }

  /* Return the universal RGBA8888 color format */
  return  (  rl           >> 20 ) |
          (( gl & mask1 ) >> 12 ) |
          (( bl & mask1 ) >>  4 ) |
          (( al & mask1 ) <<  4 );
}


/*******************************************************************************
* FUNCTION:
*   EwGetOpacityFromGradient
*
* DESCRIPTION:
*   The function EwGetOpacityFromGradient() has the job to interpolate the 
*   opacity value for the given position within a color gradient.
*
* ARGUMENTS:
*   aGradient  - Color gradient to get the value.
*   aX, aY     - Position relative to the upper-left corner of the gradient to 
*     get the opacity value.    
*
* RETURN VALUE:
*   Interpolated color value in the universal RGBA8888 color format. Only the
*   alpha channel is valid.
*
*******************************************************************************/
unsigned int EwGetOpacityFromGradient( XGradient* aGradient, int aX, int aY )
{
  int al = aGradient->A0;
  int ar = aGradient->A2;

  /* Limit the coordinates to the gradient area */
  if ( aX < 0 ) aX = 0;
  if ( aY < 0 ) aY = 0;
  if ( aX > aGradient->Width  ) aX = aGradient->Width;
  if ( aY > aGradient->Height ) aY = aGradient->Height;

  /* Interpolate the gradient values vertically */
  if ( aGradient->IsVertical && aY )
    al += aY * aGradient->A1;

  /* Avoid unnecessary calculations for vertical only gradients */
  if ( aGradient->IsVertical && aGradient->IsHorizontal && aY )
    ar += aY * aGradient->A3;

  /* Interpolate the gradient values horizontally */
  if ( aGradient->IsHorizontal && aX )
    al = ( al & 0x0FF00000 ) + (( ar >> 20 ) - ( al >> 20 )) * 
           aGradient->InvWidth * aX;

  /* Return the universal RGBA8888 color format */
  return ( al << 4 ) & 0xFF000000;
}


#ifndef EW_DONT_USE_PATH_FUNCTIONS

/* This internal worker function implements the polygon raster operation for the
   case of polygon rasterized with antialiasing and non-zero-winding rule. */
static int RasterPolygonRow_AA_NZW( unsigned char* aPixel, 
  unsigned char* aEdgeBuffer, int aWidth, int aX, int aY, int* aPaths, 
  int* aFirst )
{
  static const union {unsigned char Bytes[2]; short Word; } byteOrder = {{ 255, 0 }};
  int yt    = ( aY & ~15 ) + 2;
  int yb    = yt + 12;
  int first = aWidth << 4;
  int last  = -1;
  int act   = 0;
  int i;

  /* Step 1: Evaluate all sub-paths from the array aPaths and record all
     intersections found between the sub-path edges and the actual row */
  while ( *aPaths > 0 )
  {
    /* How many edges do belong to the actual sub-path? */
    int* stop = aPaths + ( *aPaths * 2 ) + 2;
    int  y1   = aPaths[2];
    int  y2;

    /* Process all edges in the sub-path */
    for ( aPaths += 4; aPaths <= stop; aPaths += 2, y1 = y2 )
    {
      int x1, x2;
      int x, y;

      y2 = *aPaths;

      /* Skip over edges, which surely lie above the row */
      if (( y1 < yt ) && ( y2 < yt ))
      {
        while (( y2 < yt ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Skip over edges, which surely lie below the row */
      else if ( y1 > yb )
      {
        while (( y2 > yb ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Get the x coordinates of the affected edge */
      x1 = aPaths[-3] - aX;
      x2 = aPaths[-1] - aX;

      /* Evaluate all possible intersections. In case of antialiasing, there are
         4 sib-pixel rows to take in account */
      for ( y = yt; y <= yb; y += 4 )
      {
        /* The actual sub-pixel row is not affected by the intersection */
        if ((( y < y1 ) || ( y >= y2 )) && (( y < y2 ) || ( y >= y1 )))
          continue;

        /* Calculate the x position for the intersection */
        x = x1 + (( x2 - x1 ) * ( y - y1 )) / ( y2 - y1 );

        /* The intersection lies on the left of visible row. Just count it */
        if ( x < 0 )
        {
          act += ( y2 > y1 )? 1 : -1;
          continue;
        }

        /* Count the intersection in the edge buffer. If the intersection lies 
           on the right of the row - ignore it */
        if (( x >> 4 ) < aWidth )
        {
          /* Count the intersection in the edge buffer */
          ((unsigned short*)aEdgeBuffer)[ x >> 2 ] += ( y2 > y1 )? 1 : -1;

          /* Track the range of edge buffer array elements modified by the loop */
          if ( x < first ) first = x;
          if ( x > last  ) last  = x;
        }
      }
    }

    /* Adjust the pointer to refer to the begin of the next sub-path */
    aPaths--;
  }

  /* From the position of the first/last intersection determine the index of the
     first and last affected pixel */
  first >>= 4;
  last  >>= 4;

  /* Step 2: If there was an intersection on the left of the visible row, start 
     filling the row until the first intersection in the row is reached */
  if ( act && first )
  {
    int           a = ( act < 0 )? -act : act;
    unsigned char v = ( a > 3 )? 255 : (unsigned char)( 64 * a );

    /* Fill the row */
    EwFill( aPixel, v, first );
  }

  /* Step 3: Evaluate the recorded edge intersections and fill the row pixel with
     corresponding values */
  for ( i = first, first = act? 0 : first; i <= last; i++ )
  {
    int          ii  = i;
    unsigned int ev1 = ((unsigned int*)aEdgeBuffer)[ i * 2 + 0 ];
    unsigned int ev2 = ((unsigned int*)aEdgeBuffer)[ i * 2 + 1 ];
    int          v;
    int          a;

    /* Scan for a pixel containing an intersection */
    while ( !ev1 && !ev2 && ( i < last ))
    {
      ev1 = ((unsigned int*)aEdgeBuffer)[ ++i * 2 + 0 ];
      ev2 = ((unsigned int*)aEdgeBuffer)[   i * 2 + 1 ];
    }

    /* A sequence of pixel without any intersection and the pixel should
       appear filled? */
    if ( act && ( i > ii ))
    {
      a = ( act < 0 )? -act : act;
      v = ( a > 3 )? 255 : 64 * a;

      /* Fill the row */
      EwFill( aPixel + ii, (unsigned char)v, i - ii );
    }

    /* The end of the row reached */
    if ( !ev1 && !ev2 )
      continue;

    /* Running on a big-endian host? Revert the order of bytes in the 'evX'
       variables */
    if ( byteOrder.Word < 0 )
    {
      ev1 = ( ev1 >> 16 ) | (( ev1 & 0xFFFF ) << 16 );
      ev2 = ( ev2 >> 16 ) | (( ev2 & 0xFFFF ) << 16 );
    }

    /* Evaluate the intersections in the actual pixel */
    a = (( act += (signed short)ev1 ) < 0 )? -act : act; v  = ( a > 3 )? 4 : a; ev1 >>= 16;
    a = (( act += (signed short)ev1 ) < 0 )? -act : act; v += ( a > 3 )? 4 : a;
    a = (( act += (signed short)ev2 ) < 0 )? -act : act; v += ( a > 3 )? 4 : a; ev2 >>= 16;
    a = (( act += (signed short)ev2 ) < 0 )? -act : act; v += ( a > 3 )? 4 : a;

    if ( v )
      aPixel[i] = (unsigned char)(( v << 4 ) - 1 );
    
    /* Reset the intersection info in the EdgeBuffer */
    ((unsigned int*)aEdgeBuffer)[ i * 2 + 0 ] = 0;
    ((unsigned int*)aEdgeBuffer)[ i * 2 + 1 ] = 0;
  }

  /* All intersections lie outside the visible area. Handle the special case of 
     a an inner area of the polygon being redrawn only */
  if ( act && ( first > last ))
    i = 0;

  /* Step 4: The row ends with an opened intersection causing the pixel till the
     end of the row to be filled */
  if ( act && ( i < aWidth ))
  {
    int           a = ( act < 0 )? -act : act;
    unsigned char v = ( a > 3 )? 255 : (unsigned char)( 64 * a );

    /* Fill the row */
    EwFill( aPixel + i, v, aWidth - i );

    /* The pixel row is affected ending with the last pixel */
    last = aWidth - 1;
  }

  /* No pixel drawn */
  if ( first > last )
    return 0;

  if ( aFirst )
   *aFirst = first;

  /* At least one pixel affected within the row? */
  return last - first + 1;
}


/* This internal worker function implements the polygon raster operation for the
   case of polygon rasterized with non-zero-winding rule and without 
   antialiasing. */
static int RasterPolygonRow_NZW( unsigned char* aPixel, 
  unsigned char* aEdgeBuffer, int aWidth, int aX, int aY, int* aPaths, 
  int* aFirst )
{
  static const union { unsigned char Bytes[2]; short Word; } byteOrder = {{ 255, 0 }};
  int y      = ( aY & ~15 ) + 8;
  int first  = aWidth << 4;
  int last   = -1;
  int act    = 0;
  int i;

  /* Step 1: Evaluate all sub-paths from the array aPaths and record all
     intersections found between the sub-path edges and the actual row */
  while ( *aPaths > 0 )
  {
    /* How many edges do belong to the actual sub-path? */
    int* stop = aPaths + ( *aPaths * 2 ) + 2;
    int  y1   = aPaths[2];
    int  y2;

    /* Process all edges in the sub-path */
    for ( aPaths += 4; aPaths <= stop; aPaths += 2, y1 = y2 )
    {
      int x1, x2;
      int x;

      y2 = *aPaths;

      /* Skip over edges, which surely lie above the row */
      if (( y1 < y ) && ( y2 < y ))
      {
        while (( y2 < y ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Skip over edges, which surely lie below the row */
      else if (( y1 > y ) && ( y2 > y ))
      {
        while (( y2 > y ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* The actual sub-pixel row is not affected by the intersection */
      if ((( y <= y1 ) || ( y > y2 )) && (( y <= y2 ) || ( y > y1 )))
        continue;

      /* Get the x coordinates of the affected edge */
      x1 = aPaths[-3] - aX;
      x2 = aPaths[-1] - aX;

      /* Calculate the x position for the intersection */
      x = x1 + (( x2 - x1 ) * ( y - y1 )) / ( y2 - y1 );

      /* The intersection lies on the left of visible row. Just count it */
      if ( x < 0 )
      {
        act += ( y2 > y1 )? 1 : -1;
        continue;
      }

      /* Count the intersection in the edge buffer. If the intersection lies 
         on the right of the row - ignore it */
      if (( x >> 4 ) < aWidth )
      {
        /* Count the intersection in the edge buffer */
        ((unsigned short*)aEdgeBuffer)[ x >> 2 ] += ( y2 > y1 )? 1 : -1;

        /* Track the range of edge buffer array elements modified by the loop */
        if ( x < first ) first = x;
        if ( x > last  ) last  = x;
      }
    }

    /* Adjust the pointer to refer to the begin of the next sub-path */
    aPaths--;
  }

  /* From the position of the first/last intersection determine the index of the
     first and last affected pixel */
  first >>= 4;
  last  >>= 4;

  /* Step 2: If there was an intersection on the left of the visible row, start 
     filling the row until the first intersection in the row is reached */
  if ( act && first )
    EwFill( aPixel, 255, first );

  /* Step 3: Evaluate the recorded edge intersections and fill the row pixel with
     corresponding values */
  for ( i = first, first = act? 0 : first; i <= last; i++ )
  {
    int          ii  = i;
    unsigned int ev1 = ((unsigned int*)aEdgeBuffer)[ i * 2 + 0 ];
    unsigned int ev2 = ((unsigned int*)aEdgeBuffer)[ i * 2 + 1 ];
    int          v;

    /* Scan for a pixel containing an intersection */
    while ( !ev1 && !ev2 && ( i < last ))
    {
      ev1 = ((unsigned int*)aEdgeBuffer)[ ++i * 2 + 0 ];
      ev2 = ((unsigned int*)aEdgeBuffer)[   i * 2 + 1 ];
    }

    /* A sequence of pixel without any intersection and the pixel should
       appear filled? */
    if ( act && ( i > ii ))
      EwFill( aPixel + ii, 255, i - ii );

    /* The end of the row reached */
    if ( !ev1 && !ev2 )
      continue;

    /* Running on a big-endian host? Revert the order of bytes in the 'evX'
       variables */
    if ( byteOrder.Word < 0 )
    {
      ev1 = ( ev1 >> 16 ) | (( ev1 & 0xFFFF ) << 16 );
      ev2 = ( ev2 >> 16 ) | (( ev2 & 0xFFFF ) << 16 );
    }

    /* Evaluate the intersections in the actual pixel */
    act += (signed short)ev1; v  = act? 1 : 0; ev1 >>= 16;
    act += (signed short)ev1; v += act? 1 : 0;
    act += (signed short)ev2; v += act? 1 : 0; ev2 >>= 16;
    act += (signed short)ev2; v += act? 1 : 0;

    /* Evaluate the intersections in the actual pixel */
    if ( v > 2 )
      aPixel[i] = 255;
    
    /* Reset the intersection info in the EdgeBuffer */
    ((unsigned int*)aEdgeBuffer)[ i * 2 + 0 ] = 0;
    ((unsigned int*)aEdgeBuffer)[ i * 2 + 1 ] = 0;
  }

  /* All intersections lie outside the visible area. Handle the special case of 
     a an inner area of the polygon being redrawn only */
  if ( act && ( first > last ))
    i = 0;

  /* Step 4: The row ends with an opened intersection causing the pixel till the
     end of the row to be filled */
  if ( act && ( i < aWidth ))
  {
    /* Fill the row */
    EwFill( aPixel + i, 255, aWidth - i );

    /* The pixel row is affected ending with the last pixel */
    last = aWidth - 1;
  }

  /* No pixel drawn */
  if ( first > last )
    return 0;

  if ( aFirst )
   *aFirst = first;

  /* At least one pixel affected within the row? */
  return last - first + 1;
}


/* This internal worker function implements the polygon raster operation for the
   case of polygon rasterized with antialiasing and even-odd fill rule. */
static int RasterPolygonRow_AA_EO( unsigned char* aPixel, 
  unsigned char* aEdgeBuffer, int aWidth, int aX, int aY, int* aPaths,
  int* aFirst )
{
  const static signed char bits[16] = { 0, 1, 1, 2, 1, 2, 2, 3, 1, 2, 2, 3, 2, 3, 3, 4 };
  static const union { unsigned char Bytes[2]; short Word; } byteOrder = {{ 255, 0 }};
  int yt     = ( aY & ~15 ) + 2;
  int yb     = yt + 12;
  int first  = aWidth << 4;
  int last   = -1;
  int act    = 0;
  int i;

  /* Step 1: Evaluate all sub-paths from the array aPaths and record all
     intersections found between the sub-path edges and the actual row */
  while ( *aPaths > 0 )
  {
    /* How many edges do belong to the actual sub-path? */
    int* stop = aPaths + ( *aPaths * 2 ) + 2;
    int  y1   = aPaths[2];
    int  y2;

    /* Process all edges in the sub-path */
    for ( aPaths += 4; aPaths <= stop; aPaths += 2, y1 = y2 )
    {
      int x1, x2;
      int x, y;

      y2 = *aPaths;

      /* Skip over edges, which surely lie above the row */
      if (( y1 < yt ) && ( y2 < yt ))
      {
        while (( y2 < yt ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Skip over edges, which surely lie below the row */
      else if ( y1 > yb )
      {
        while (( y2 > yb ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Get the x coordinates of the affected edge */
      x1 = aPaths[-3] - aX;
      x2 = aPaths[-1] - aX;

      /* Evaluate all possible intersections. In case of antialiasing, there are
         4 sib-pixel rows to take in account */
      for ( y = yt; y <= yb; y += 4 )
      {
        /* The actual sub-pixel row is not affected by the intersection */
        if ((( y < y1 ) || ( y >= y2 )) && (( y < y2 ) || ( y >= y1 )))
          continue;

        /* Calculate the x position for the intersection */
        x = x1 + (( x2 - x1 ) * ( y - y1 )) / ( y2 - y1 );

        /* The intersection lies on the left of visible row. Just toggle it */
        if ( x < 0 )
        {
          act ^= 1 << (( y >> 2 ) & 3 );
          continue;
        }

        /* Toggle the intersection in the edge buffer. If the intersection lies 
           on the right of the row - ignore it */
        if (( x >> 4 ) < aWidth )
        {
          /* Toggle the intersection in the edge buffer */
          aEdgeBuffer[ x >> 2 ] ^= 1 << (( y >> 2 ) & 3 );

          /* Track the range of edge buffer array elements modified by the loop */
          if ( x < first ) first = x;
          if ( x > last  ) last  = x;
        }
      }
    }

    /* Adjust the pointer to refer to the begin of the next sub-path */
    aPaths--;
  }

  /* From the position of the first/last intersection determine the index of the
     first and last affected pixel */
  first >>= 4;
  last  >>= 4;

  /* Step 2: If there was an intersection on the left of the visible row, start 
     filling the row until the first intersection in the row is reached */
  if ( act && first )
    EwFill( aPixel, ( bits[act] << 6 ) - 1, first );

  /* Step 3: Evaluate the recorded edge intersections and fill the row pixel with
     corresponding values */
  for ( i = first, first = act? 0 : first; i <= last; i++ )
  {
    int          ii = i;
    unsigned int ev = ((unsigned int*)aEdgeBuffer)[ i ];
    int          v;

    /* Scan for a pixel containing an intersection */
    while ( !ev && ( i < last ))
      ev = ((unsigned int*)aEdgeBuffer)[ ++i ];

    /* A sequence of pixel without any intersection and the pixel should
       appear filled? */
    if ( act && ( i > ii ))
      EwFill( aPixel + ii, ( bits[act] << 6 ) - 1, i - ii );

    /* The end of the row reached */
    if ( !ev )
      continue;

    /* Running on a big-endian host? Revert the order of bytes in the 'ev'
       variable */
    if ( byteOrder.Word < 0 )
      ev = ( ev >> 24 ) | (( ev >> 8 ) & 0xFF00 ) | (( ev & 0xFF00 ) << 8 ) |
           ( ev << 24 );

    /* Evaluate the intersections in the actual pixel */
    act ^= (signed char)ev; v  = bits[ act ]; ev >>= 8;
    act ^= (signed char)ev; v += bits[ act ]; ev >>= 8;
    act ^= (signed char)ev; v += bits[ act ]; ev >>= 8;
    act ^= (signed char)ev; v += bits[ act ];

    if ( v )
      aPixel[i] = (unsigned char)(( v << 4 ) - 1 );
    
    /* Reset the intersection info in the EdgeBuffer */
    ((unsigned int*)aEdgeBuffer)[ i ] = 0;
  }

  /* All intersections lie outside the visible area. Handle the special case of 
     a an inner area of the polygon being redrawn only */
  if ( act && ( first > last ))
    i = 0;

  /* Step 4: The row ends with an opened intersection causing the pixel till the
     end of the row to be filled */
  if ( act && ( i < aWidth ))
  {
    /* Fill the row */
    EwFill( aPixel + i, ( bits[act] << 6 ) - 1, aWidth - i );

    /* The pixel row is affected ending with the last pixel */
    last = aWidth - 1;
  }

  /* No pixel drawn */
  if ( first > last )
    return 0;

  if ( aFirst )
   *aFirst = first;

  /* At least one pixel affected within the row? */
  return last - first + 1;
}


/* This internal worker function implements the polygon raster operation for the
   case of polygon rasterized with even-odd fill rule and without antialiasing. */
static int RasterPolygonRow_EO( unsigned char* aPixel, 
  unsigned char* aEdgeBuffer, int aWidth, int aX, int aY, int* aPaths,
  int* aFirst )
{
  int y      = ( aY & ~15 ) + 8;
  int first  = aWidth << 4;
  int last   = -1;
  int act    = 0;
  int i;

  /* Step 1: Evaluate all sub-paths from the array aPaths and record all
     intersections found between the sub-path edges and the actual row */
  while ( *aPaths > 0 )
  {
    /* How many edges do belong to the actual sub-path? */
    int* stop = aPaths + ( *aPaths * 2 ) + 2;
    int  y1   = aPaths[2];
    int  y2;

    /* Process all edges in the sub-path */
    for ( aPaths += 4; aPaths <= stop; aPaths += 2, y1 = y2 )
    {
      int x1, x2;
      int x;

      y2 = *aPaths;

      /* Skip over edges, which surely lie above the row */
      if (( y1 < y ) && ( y2 < y ))
      {
        while (( y2 < y ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* Skip over edges, which surely lie below the row */
      else if (( y1 > y ) && ( y2 > y ))
      {
        while (( y2 > y ) && ( aPaths < stop ))
          y2 = *( aPaths += 2 );

        y1 = aPaths[-2];
      }

      /* The actual sub-pixel row is not affected by the intersection */
      if ((( y <= y1 ) || ( y > y2 )) && (( y <= y2 ) || ( y > y1 )))
        continue;

      /* Get the x coordinates of the affected edge */
      x1 = aPaths[-3] - aX;
      x2 = aPaths[-1] - aX;

      /* Calculate the x position for the intersection */
      x = x1 + (( x2 - x1 ) * ( y - y1 )) / ( y2 - y1 );

      /* The intersection lies on the left of visible row. Just toggle it */
      if ( x < 0 )
      {
        act ^= 1;
        continue;
      }

      /* Count the intersection in the edge buffer. If the intersection lies 
         on the right of the row - ignore it */
      if (( x >> 4 ) < aWidth )
      {
        /* Toggle the intersection in the edge buffer */
        aEdgeBuffer[ x >> 4 ] ^= 1 << (( x >> 2 ) & 3 );

        /* Track the range of edge buffer array elements modified by the loop */
        if ( x < first ) first = x;
        if ( x > last  ) last  = x;
      }
    }

    /* Adjust the pointer to refer to the begin of the next sub-path */
    aPaths--;
  }

  /* From the position of the first/last intersection determine the index of the
     first and last affected pixel */
  first >>= 4;
  last  >>= 4;

  /* Step 2: If there was an intersection on the left of the visible row, start 
     filling the row until the first intersection in the row is reached */
  if ( act && first )
    EwFill( aPixel, 255, first );

  /* Step 3: Evaluate the recorded edge intersections and fill the row pixel with
     corresponding values */
  for ( i = first, first = act? 0 : first; i <= last; i++ )
  {
    int           ii = i;
    unsigned char ev = aEdgeBuffer[ i ];
    int           v;

    /* Scan for a pixel containing an intersection */
    while ( !ev && ( i < last ))
      ev = aEdgeBuffer[ ++i ];

    /* A sequence of pixel without any intersection and the pixel should
       appear filled? */
    if ( act && ( i > ii ))
      EwFill( aPixel + ii, 255, i - ii );

    /* The end of the row reached */
    if ( !ev )
      continue;

    /* Evaluate the intersections in the actual pixel */
    act ^= ev & 1; v  = act? 1 : 0; ev >>= 1;
    act ^= ev & 1; v += act? 1 : 0; ev >>= 1;
    act ^= ev & 1; v += act? 1 : 0; ev >>= 1;
    act ^= ev & 1; v += act? 1 : 0;

    /* Evaluate the intersections in the actual pixel */
    if ( v > 2 )
      aPixel[i] = 255;
    
    /* Reset the intersection info in the EdgeBuffer */
    aEdgeBuffer[ i ] = 0;
  }

  /* All intersections lie outside the visible area. Handle the special case of 
     a an inner area of the polygon being redrawn only */
  if ( act && ( first > last ))
    i = 0;

  /* Step 4: The row ends with an opened intersection causing the pixel till the
     end of the row to be filled */
  if ( act && ( i < aWidth ))
  {
    /* Fill the row */
    EwFill( aPixel + i, 255, aWidth - i );

    /* The pixel row is affected ending with the last pixel */
    last = aWidth - 1;
  }

  /* No pixel drawn */
  if ( first > last )
    return 0;

  if ( aFirst )
   *aFirst = first;

  /* At least one pixel affected within the row? */
  return last - first + 1;
}

#endif /* EW_DONT_USE_PATH_FUNCTIONS */


/*******************************************************************************
* FUNCTION:
*   EwRasterAlpha8Polygon
*
* DESCRIPTION:
*  The function EwRasterAlpha8Polygon() implements an algorithm to estimate the
*  content of an ALPHA8 bitmap from polygon data determined by edges in the
*  array aPaths. aPaths stores the edges as a serie of X,Y pairs starting always
*  with a value specifying the number of existing edges. With this approach one
*  path can consist of several sub-paths. The end of the list is determined by a
*  sub-path with 0 edges.
*
*  +-------+------+------+------+------+------+------+-------+     +-----+
*  | edges |  X0  |  Y0  |  X1  |  Y1  |  X2  |  Y2  | edges | ... |  0  |
*  +-------+------+------+------+------+------+------+-------+     +-----+
*
*  The function evaluates the path data for intersections between the edges and
*  the pixel within the destination area aDstX, aDstY, aWidth and aHeight. Then
*  the affected pixel are set or cleared according to whether they lie inside
*  or outside the polygon.
*
* ARGUMENTS:
*   aDst            - Pointer to the first pixel of the destination ALPHA8
*     surface. The caller is responsable for clearing the surface before.
*   aPaths          - An array containing the path data. The array starts with
*     the number of edges a path is composed of. Then follow the coordinates of
*     all path corners as X,Y pairs. After the last coordinate pair next path
*     can follow starting again with the number of edges. The end of the path
*     data is signed with 0. The X,Y coordinates are stored as signed integer
*     with 4-bit fixpoint precision. The coordinates are valid relative to the
*     top-left corner of the destination area aDstX, aDstY.
*   aDstX,
*   aDstY           - Origin of the area to fill (relative to the top-left 
*     corner of the destination surface).
*   aWidth,
*   aHeight         - Size of the area to fill.
*   aX, aY          - Path coordinate at the top-left corner of aDstX, aDstY
*     area. This value is expressed in in 4-bit fixpoint precision.
*   aAntialiased    - If != 0, the function applies antialiasing to the pixel.
*     The antialiasing is based on supersampling with 4 samples in X and Y
*     direction.
*   aNonZeroWinding - Controls the fill rule to be used by the algorithm. If
*    this parameter is == 0, the even-odd fill rule is used. If this parameter
*    is != 0, the non-zero winding rule is used.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwRasterAlpha8Polygon( XSurfaceMemory* aDst, int* aPaths, int aDstX,
  int aDstY, int aWidth, int aHeight, int aX, int aY, int aAntialiased, 
  int aNonZeroWinding )
{
  #ifndef EW_DONT_USE_PATH_FUNCTIONS
    int            bufSize = aWidth * ( aNonZeroWinding? 8 : 
                                        aAntialiased? 4 : 1 );
    unsigned char* pixel   = (unsigned char*)aDst->Pixel1 + aDstY * aDst->Pitch1Y +
                             aDstX;
    unsigned char* buf;

    do
      buf = EwAlloc( bufSize );
    while ( !buf && EwImmediateReclaimMemory( 15 ));

    /* We were not able to reserve memory for temporary edge buffers. Print just
       a warning and continue. The application should not enter panic in this case. */
    if ( !buf )
    {
      EwError( 15 );
      return;
    }

    /* Clear the temporary edge buffer */
    EwZero( buf, bufSize );

    /* Repeat until all rows have been processed */
    for ( ; aHeight > 0; aHeight--, aY += 16, pixel += aDst->Pitch1Y )
    {
      /* Perform the raster operation. Select the appropriate function */
      if ( aNonZeroWinding && aAntialiased )
        RasterPolygonRow_AA_NZW( pixel, buf, aWidth, aX, aY, aPaths, 0 );
      else if ( aNonZeroWinding )
        RasterPolygonRow_NZW( pixel, buf, aWidth, aX, aY, aPaths, 0 );
      else if ( aAntialiased )
        RasterPolygonRow_AA_EO( pixel, buf, aWidth, aX, aY, aPaths, 0 );
      else
        RasterPolygonRow_EO( pixel, buf, aWidth, aX, aY, aPaths, 0 );
    }

    /* Track the temporary used memory */
    EwResourcesMemory += sizeof( bufSize );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    EwResourcesMemory -= sizeof( bufSize );

    /* Release the temporary used memory */
    EwFree( buf );
  #else
    EW_UNUSED_ARG( aDst );
    EW_UNUSED_ARG( aPaths );
    EW_UNUSED_ARG( aDstX );
    EW_UNUSED_ARG( aDstY );
    EW_UNUSED_ARG( aWidth );
    EW_UNUSED_ARG( aHeight );
    EW_UNUSED_ARG( aX );
    EW_UNUSED_ARG( aY );
    EW_UNUSED_ARG( aAntialiased );
    EW_UNUSED_ARG( aNonZeroWinding );
  #endif /* EW_DONT_USE_PATH_FUNCTIONS */
}


/*******************************************************************************
* FUNCTION:
*   EwEmulateLine
*
* DESCRIPTION:
*   The function EwEmulateLine() drives a draw line operation by using solid or
*   gradient color values.
*
*   The function provides the top-level interface to the software pixel driver.
*   The real drawing operation will be driven by the passed worker function.
*
* ARGUMENTS:
*   aDst        - Pointer to the first pixel of the destination surface.
*   aDstX1,
*   aDstY1      - Start position to draw the line (relative to the top-left
*     corner of the destination surface).
*   aDstX2,
*   aDstY2      - End position to draw the line (relative to the top-left
*      corner of the destination surface). The pixel at the end position does
*      not belong to the line - it is invisible. In this manner polylines can
*      be drawn.
*   aClipX1,
*   aClipY1,
*   aClipX2,
*   aClipY2     - Optional clipping area relative to the top-left corner of the
*     destination surface. No pixel outside this area may be drawn.
*   aGradient   - Color information to modulate the drawn pixel.
*   aWorker     - Low-level worker function to perform the operation.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwEmulateLine( XSurfaceMemory* aDst, int aDstX1, int aDstY1, int aDstX2,
  int aDstY2, int aClipX1, int aClipY1, int aClipX2, int aClipY2,
  XGradient* aGradient, XLineWorker aWorker )
{
  int       w      = ABS( aDstX2 - aDstX1 );
  int       h      = ABS( aDstY2 - aDstY1 );
  int       x      = ( aDstX1 << 16 ) + 0x08000;
  int       y      = ( aDstY1 << 16 ) + 0x08000;
  int       stepX  = 0x10000;
  int       stepY  = 0x10000;
  int       steps  = MAX( w, h );
  XGradient grad;

  /* From the passed gradient derive a new one, which is intended for treatment
     of single pixel */
  grad.R0 = aGradient->R0;
  grad.G0 = aGradient->G0;
  grad.B0 = aGradient->B0;
  grad.A0 = aGradient->A0;

  /* Calculate the feed rate and the direction */
  if ( w > h ) stepY = ( h << 16 ) / w;
  if ( h > w ) stepX = ( w << 16 ) / h;
  if ( aDstX1 > aDstX2 ) stepX = -stepX; 
  if ( aDstY1 > aDstY2 ) stepY = -stepY;

  /* Repeat until the entire line has been drawn */
  for ( ; steps > 0; steps-- )
  {
    int xx = x >> 16;
    int yy = y >> 16;

    /* Draw pixel inside the clipping area only */
    if (( xx >= aClipX1 ) && ( xx < aClipX2 ) &&
        ( yy >= aClipY1 ) && ( yy < aClipY2 ))
      aWorker( aDst, xx, yy, &grad );

    /* For the next pixel */
    x += stepX;
    y += stepY;

    /* If there is a trend in the gradient - adapt the color values for the next
       following pixel */
    if ( aGradient->IsVertical )
    {
      grad.R0 += aGradient->R1;
      grad.G0 += aGradient->G1;
      grad.B0 += aGradient->B1;
      grad.A0 += aGradient->A1;
    }
  }
}


/*******************************************************************************
* FUNCTION:
*   EwEmulateFill
*
* DESCRIPTION:
*   The function EwEmulateFill() drives a fill operation for a rectangular area
*   by using solid or gradient color values.
*
*   The function provides the top-level interface to the software pixel driver.
*   The real drawing operation will be driven by the passed worker function.
*
* ARGUMENTS:
*   aDst      - Pointer to the first pixel of the destination surface.
*   aDstX,
*   aDstY     - Origin of the area to fill (relative to the top-left corner of
*     the destination surface).
*   aWidth,
*   aHeight   - Size of the area to fill.
*   aGradient - Color information to modulate the filled pixel.
*   aGrdX,
*   aGrdY     - Origin of the affected area in the gradient coordinate space.
*   aWorker   - Low-level worker function to perform the operation row-wise.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwEmulateFill( XSurfaceMemory* aDst, int aDstX, int aDstY, int aWidth,
  int aHeight, XGradient* aGradient, int aGrdX, int aGrdY, XFillWorker aWorker )
{
  int       mask1     = 0x0FF00000;
  XGradient grad;
  int       rl, gl, bl, al;
  int       rr, gr, br, ar;
  int       isHorz    = aGradient->IsHorizontal;
  void*     savedDst1 = aDst->Pixel1;
  void*     savedDst2 = aDst->Pixel2;

  /* Relative destination position to start the transfer */
  aDst->Pixel1 = (char*)aDst->Pixel1 + aDstY * aDst->Pitch1Y + 
                                       aDstX * aDst->Pitch1X;
  aDst->Pixel2 = (char*)aDst->Pixel2 + aDstY * aDst->Pitch2Y +
                                       aDstX * aDst->Pitch2X;

  /* Start the gradient with the top-left and top-right colors */
  rl = aGradient->R0; rr = aGradient->R2;
  gl = aGradient->G0; gr = aGradient->G2;
  bl = aGradient->B0; br = aGradient->B2;
  al = aGradient->A0; ar = aGradient->A2;

  /* Adjust the gradient start values, if there is an offset between the origin
     of the gradient and the origin of the area affected by the operations */
  if ( aGradient->IsVertical && aGrdY )
  {
    rl += aGradient->R1 * aGrdY; rr += aGradient->R3 * aGrdY;
    gl += aGradient->G1 * aGrdY; gr += aGradient->G3 * aGrdY;
    bl += aGradient->B1 * aGrdY; br += aGradient->B3 * aGrdY;
    al += aGradient->A1 * aGrdY; ar += aGradient->A3 * aGrdY;
  }

  /* Start color for the first row */
  grad.R0 = rl & mask1; grad.G0 = gl & mask1;
  grad.B0 = bl & mask1; grad.A0 = al & mask1;

  /* Repeat until all rows have been processed */
  while ( aHeight-- > 0 )
  {
    /* If there is a horizontal trend in the gradient - recalculate the color
       slope values for the next following pixel row */
    if ( isHorz )
    {
      grad.R1 = (( rr >> 20 ) - ( rl >> 20 )) * aGradient->InvWidth;
      grad.G1 = (( gr >> 20 ) - ( gl >> 20 )) * aGradient->InvWidth;
      grad.B1 = (( br >> 20 ) - ( bl >> 20 )) * aGradient->InvWidth;
      grad.A1 = (( ar >> 20 ) - ( al >> 20 )) * aGradient->InvWidth;

      /* Adjust the gradient start values, if there is an offset between the
         origin of the gradient and the origin of the area affected by the
         operations */
      if ( aGrdX )
      {
        grad.R0 += grad.R1 * aGrdX; grad.G0 += grad.G1 * aGrdX;
        grad.B0 += grad.B1 * aGrdX; grad.A0 += grad.A1 * aGrdX;
      }

      /* If there is no additional vertical trend in the gradient (horizontal
         gradient only) - omit this recalculation step for the next time */
      isHorz = aGradient->IsVertical;
    }
    
    /* Process the row */
    aWorker( aDst, aWidth, &grad );

    /* Continue with the next row */
    aDst->Pixel1 = (char*)aDst->Pixel1 + aDst->Pitch1Y;
    aDst->Pixel2 = (char*)aDst->Pixel2 + aDst->Pitch2Y; 

    /* If there is a vertical trend in the gradient - adapt the color values
       for the next following pixel row */
    if ( aGradient->IsVertical )
    {
      grad.R0 = ( rl += aGradient->R1 ) & mask1; rr += aGradient->R3;
      grad.G0 = ( gl += aGradient->G1 ) & mask1; gr += aGradient->G3;
      grad.B0 = ( bl += aGradient->B1 ) & mask1; br += aGradient->B3;
      grad.A0 = ( al += aGradient->A1 ) & mask1; ar += aGradient->A3;
    }
  }

  /* Restore the pixel pointers - may be the caller will reuse the surface
     memory structure */
  aDst->Pixel1 = savedDst1;
  aDst->Pixel2 = savedDst2;
}


/*******************************************************************************
* FUNCTION:
*   EwEmulateCopy
*
* DESCRIPTION:
*   The function EwEmulateCopy() drives a copy operation for a rectangular area
*   from a native, index8 or alpha8 surface to a screen or native surface. The
*   function modulates the copied pixel by solid or gradient opacity values.
*
*   The function provides the top-level interface to the software pixel driver.
*   The real drawing operation will be driven by the passed worker function.
*
* ARGUMENTS:
*   aDst      - Pointer to the first pixel of the destination surface.
*   aSrc      - Pointer to the first pixel of the source surface.
*   aDstX,
*   aDstY     - Origin of the area to fill (relative to the top-left corner of
*     the destination surface).
*   aWidth,
*   aHeight   - Size of the area to fill.
*   aSrcX,
*   aSrcY     - Origin of the source area to copy (relative to the top-left
*     corner of the source surface).
*   aGradient - Color information to modulate the copied pixel.
*   aGrdX,
*   aGrdY     - Origin of the affected area in the gradient coordinate space.
*   aWorker   - Low-level worker function to perform the operation row-wise.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwEmulateCopy( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aDstX,
  int aDstY, int aWidth, int aHeight, int aSrcX, int aSrcY, XGradient* aGradient,
  int aGrdX, int aGrdY, XCopyWorker aWorker )
{
  int       mask1     = 0x0FF00000;
  XGradient grad;
  int       rl, gl, bl, al;
  int       rr, gr, br, ar;
  int       isHorz    = aGradient->IsHorizontal;
  void*     savedDst1 = aDst->Pixel1;
  void*     savedDst2 = aDst->Pixel2;
  void*     savedSrc1 = aSrc->Pixel1;
  void*     savedSrc2 = aSrc->Pixel2;

  /* Relative destination position to start the transfer */
  aDst->Pixel1 = (char*)aDst->Pixel1 + aDstY * aDst->Pitch1Y + 
                                       aDstX * aDst->Pitch1X;
  aDst->Pixel2 = (char*)aDst->Pixel2 + aDstY * aDst->Pitch2Y +
                                       aDstX * aDst->Pitch2X;

  /* Relative source position to start the transfer */
  aSrc->Pixel1 = (char*)aSrc->Pixel1 + aSrcY * aSrc->Pitch1Y + 
                                       aSrcX * aSrc->Pitch1X;
  aSrc->Pixel2 = (char*)aSrc->Pixel2 + aSrcY * aSrc->Pitch2Y +
                                       aSrcX * aSrc->Pitch2X;

  /* Start the gradient with the top-left and top-right colors */
  rl = aGradient->R0; rr = aGradient->R2;
  gl = aGradient->G0; gr = aGradient->G2;
  bl = aGradient->B0; br = aGradient->B2;
  al = aGradient->A0; ar = aGradient->A2;

  /* Adjust the gradient start values, if there is an offset between the origin
     of the gradient and the origin of the area affected by the operations */
  if ( aGradient->IsVertical && aGrdY )
  {
    rl += aGradient->R1 * aGrdY; rr += aGradient->R3 * aGrdY;
    gl += aGradient->G1 * aGrdY; gr += aGradient->G3 * aGrdY;
    bl += aGradient->B1 * aGrdY; br += aGradient->B3 * aGrdY;
    al += aGradient->A1 * aGrdY; ar += aGradient->A3 * aGrdY;
  }

  /* Start color for the first row */
  grad.R0 = rl & mask1; grad.G0 = gl & mask1;
  grad.B0 = bl & mask1; grad.A0 = al & mask1;

  /* Repeat until all rows have been processed */
  while ( aHeight-- > 0 )
  {
    /* If there is a horizontal trend in the gradient - recalculate the color
       slope values for the next following pixel row */
    if ( isHorz )
    {
      grad.R1 = (( rr >> 20 ) - ( rl >> 20 )) * aGradient->InvWidth;
      grad.G1 = (( gr >> 20 ) - ( gl >> 20 )) * aGradient->InvWidth;
      grad.B1 = (( br >> 20 ) - ( bl >> 20 )) * aGradient->InvWidth;
      grad.A1 = (( ar >> 20 ) - ( al >> 20 )) * aGradient->InvWidth;

      /* Adjust the gradient start values, if there is an offset between the
         origin of the gradient and the origin of the area affected by the
         operations */
      if ( aGrdX )
      {
        grad.R0 += grad.R1 * aGrdX; grad.G0 += grad.G1 * aGrdX;
        grad.B0 += grad.B1 * aGrdX; grad.A0 += grad.A1 * aGrdX;
      }

      /* If there is no additional vertical trend in the gradient (horizontal
         gradient only) - omit this recalculation step for the next time */
      isHorz = aGradient->IsVertical;
    }
    
    /* Process the row */
    aWorker( aDst, aSrc, aWidth, &grad );

    /* Continue with the next row */
    aDst->Pixel1 = (char*)aDst->Pixel1 + aDst->Pitch1Y;
    aDst->Pixel2 = (char*)aDst->Pixel2 + aDst->Pitch2Y; 
    aSrc->Pixel1 = (char*)aSrc->Pixel1 + aSrc->Pitch1Y;
    aSrc->Pixel2 = (char*)aSrc->Pixel2 + aSrc->Pitch2Y; 

    /* If there is a vertical trend in the gradient - adapt the color values
       for the next following pixel row */
    if ( aGradient->IsVertical )
    {
      grad.R0 = ( rl += aGradient->R1 ) & mask1; rr += aGradient->R3;
      grad.G0 = ( gl += aGradient->G1 ) & mask1; gr += aGradient->G3;
      grad.B0 = ( bl += aGradient->B1 ) & mask1; br += aGradient->B3;
      grad.A0 = ( al += aGradient->A1 ) & mask1; ar += aGradient->A3;
    }
  }

  /* Restore the pixel pointers - may be the caller will reuse the surface
     memory structure */
  aDst->Pixel1 = savedDst1;
  aDst->Pixel2 = savedDst2;
  aSrc->Pixel1 = savedSrc1;
  aSrc->Pixel2 = savedSrc2;
}


/* The following function is used to calculate with 64-bit precision the reciproc
   of the value aRange (1 / aRange). The results are stored in the both 32-bit
   unsigned integer variables aInvHiVar and aInvLoVar. */
static void GetInverseRange( int aRange, unsigned int* aInvHiVar, 
  unsigned int* aInvLoVar )
{
  if ( aRange != 0 )
  {
    int          sign  = aRange < 0;
    unsigned int range = (unsigned int)( sign? -aRange : aRange );
    unsigned int QL    = 0;
    unsigned int QH    = 0;
    unsigned int R     = 1;
    int          i     = 61;

    do
    {
      R <<= 1;

      if ( R >= range )
      {
        R  -= range;
        if ( i > 31 ) QH |= 1 << ( i - 32 );
        else          QL |= 1 << i;
      }
    }
    while ( --i >= 0 );

    if ( sign )
    {
      QL = ~QL + 1;
      QH = QL? ~QH : ~QH + 1;
    }

    *aInvHiVar = QH;
    *aInvLoVar = QL;
  }
  else
  {
    *aInvHiVar = 0;
    *aInvLoVar = 0;
  }
}


/* The following function is used to calculate a interpolation factor which does
   correspond to the given portion aPortion of the range defined in the both
   parameterts aInvRangeHi and aInvRangeLo. The resulting factor is returned as
   as [s].31 fixed point number. */ 
static int GetInterpolationFactor( unsigned int aInvRangeHi, 
  unsigned int aInvRangeLo, int aPortion )
{
  int          signF  = aPortion < 0;
  int          signI  = ( aInvRangeHi >> 31 ) != 0;
  unsigned int port   = (unsigned int)( signF? -aPortion : aPortion );
  unsigned int invLo  = signI? ~aInvRangeLo + 1 : aInvRangeLo;
  unsigned int invHi  = signI? ~aInvRangeHi + ( invLo? 0 : 1 ) : aInvRangeHi;
  unsigned int fl     = port & 0xFFFF;
  unsigned int fh     = port >> 16;
  unsigned int irhl   = invHi & 0xFFFF;
  unsigned int irhh   = invHi >> 16;
  unsigned int irll   = invLo & 0xFFFF;
  unsigned int irlh   = invLo >> 16;
  unsigned int p0     = fl * irll;
  unsigned int p1     = fh * irll;
  unsigned int p2     = fl * irlh;
  unsigned int p3     = fh * irlh;
  unsigned int p4     = fl * irhl;
  unsigned int p5     = fh * irhl;
  unsigned int p6     = fl * irhh;
  unsigned int hl     = p0 + ( p1 << 16 ) + ( p2 << 16 );
  unsigned int hr     = ( p1 >> 16 ) + ( p2 >> 16 ) + p3 + p4 + ( p5 << 16 ) +
                         ( p6 << 16 ) +
                        ((( p0 >> 16 ) + ( p2 & 0xFFFF ) +
                         ( p1 & 0xFFFF )) >> 16 );

  if ( signF != signI )
    return -(int)((( hl >> 31 ) | ( hr << 1 )) & 0x7FFFFFFF );
  else
    return (int)((( hl >> 31 ) | ( hr << 1 )) & 0x7FFFFFFF );
}


/* The following function is used to calculate a value within a given 32-bit 
   signed range aRange depending on the 32-bit signed factor aFactor as
   [s].31 bit fixed point value. The calculation is performed with 64-bit
   arithmetic. */
static int InterpolateInRange( int aRange, int aFactor )
{
  int          signR = aRange < 0;
  int          signF = aFactor < 0;
  unsigned int range = (unsigned int)( signR? -aRange  : aRange  );
  unsigned int port  = (unsigned int)( signF? -aFactor : aFactor );
  unsigned int fl    = port & 0xFFFF;
  unsigned int rl    = range & 0xFFFF;
  unsigned int fh    = port >> 16;
  unsigned int rh    = range >> 16;
  unsigned int p0    = fl * rl;
  unsigned int p1    = fh * rl;
  unsigned int p2    = fl * rh;
  unsigned int p3    = fh * rh;
  unsigned int hr    = p3 + ( p2 >> 16 ) + ( p1 >> 16 ) +
                        ((( p0 >> 16 ) + ( p2 & 0xFFFF ) +
                          ( p1 & 0xFFFF )) >> 16 );

  if ( signR != signF )
    return -(int)( hr << 1 );
  else
    return (int)( hr << 1 );
}


/* The following macro helps by the interpolation of x, w, s and t coordinates
   at the intersection with the quads edge number aEdge relative to the corner
   aCorner */
#define IntersectEdge( aCorner, aEdge )                                        \
{                                                                              \
  int m = y - aDstY##aCorner;                                                  \
                                                                               \
  /* Use fast interpolation algorithm with small coordinates (<4000 pixel) */  \
  if ( !isLarge )                                                              \
  {                                                                            \
    int d2 = ey##aEdge >> 1;                                                   \
                                                                               \
    /* Interpolation the x, w, s, t coordinates */                             \
    int x = aDstX##aCorner + (( ex##aEdge * m ) + d2 ) / ey##aEdge;            \
    int s = dstS##aCorner  + (( es##aEdge * m ) + d2 ) / ey##aEdge;            \
    int t = dstT##aCorner  + (( et##aEdge * m ) + d2 ) / ey##aEdge;            \
    int w = aDstW##aCorner + (( ew##aEdge * m ) + d2 ) / ey##aEdge;            \
                                                                               \
    /* Does the intersection affect the start or the end of a scanline? */     \
    if ( x < x1 ) { x1 = x; w1 = w; s1 = s; t1 = t; }                          \
    if ( x > x2 ) { x2 = x; w2 = w; s2 = s; t2 = t; }                          \
  }                                                                            \
                                                                               \
  /* Large coordinates (> 4000 pixel) expect 64-bit multiplication and         \
     division. Since such opoerations are note available on typical embedded   \
     systems, we use our own implementation. */                                \
  else                                                                         \
  {                                                                            \
    /* From the value 'm' calculate a 0.32 fixed point factor to weight the    \
       values to interpolate and interpolate the x, w, s, t coordinates */     \
    int factor = GetInterpolationFactor( iey##aEdge##h, iey##aEdge##l, m );    \
    int x      = InterpolateInRange( ex##aEdge, factor ) + aDstX##aCorner;     \
    int s      = InterpolateInRange( es##aEdge, factor ) + dstS##aCorner;      \
    int t      = InterpolateInRange( et##aEdge, factor ) + dstT##aCorner;      \
    int w      = InterpolateInRange( ew##aEdge, factor ) + aDstW##aCorner;     \
                                                                               \
    /* Does the intersection affect the start or the end of a scanline? */     \
    if ( x < x1 ) { x1 = x; w1 = w; s1 = s; t1 = t; }                          \
    if ( x > x2 ) { x2 = x; w2 = w; s2 = s; t2 = t; }                          \
  }                                                                            \
}


/*******************************************************************************
* FUNCTION:
*   EwEmulateWarp
*
* DESCRIPTION:
*   The function EwEmulateWarp() drives a warp operation for a rectangular area
*   of a native, index8 or alpha8 surface to a polygon within a screen or native
*   surface. The function modulates the copied pixel by solid or gradient 
*   opacity values.
*
*   The function provides the top-level interface to the software pixel driver.
*   The real drawing operation will be driven by the passed worker function.
*
* ARGUMENTS:
*   aDst        - Pointer to the first pixel of the destination surface.
*   aSrc        - Pointer to the first pixel of the source surface.
*   DstX1, 
*   DstY1,
*   DstW1,
*   ...
*   DstX4,
*   DstY4,
*   DstW4       - Coordinates of the polygon to fill with the source bitmap
*     (Relative to the top-left corner of the destination bitmap). The 'X', 'Y'
*     coefficients are specified in 29.3 fixed point format due to the
*     sub-pixel precision. The 'W' coefficients in 16.16.
*   aSrcX,
*   aSrcY       - Origin of the source area to copy (relative to the top-left
*     corner of the source surface).
*   aSrcWidth,
*   aSrcHeight  - Size of the source area to copy and warp.
*   aClipX1,
*   aClipY1,
*   aClipX2,
*   aClipY2     - Optional clipping area relative to the top-left corner of the
*     destination surface. No pixel outside this area may be drawn.
*   aGradient   - Color information to modulate the copied pixel.
*   aWorker     - Low-level worker function to perform the operation row-wise.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwEmulateWarp( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aDstX1,
  int aDstY1, int aDstW1, int aDstX2, int aDstY2, int aDstW2, int aDstX3,
  int aDstY3, int aDstW3, int aDstX4, int aDstY4, int aDstW4, int aSrcX,
  int aSrcY, int aSrcWidth, int aSrcHeight, int aClipX1, int aClipY1, 
  int aClipX2, int aClipY2, XGradient* aGradient, XWarpWorker aWorker )
{
  XSurfaceMemory dst = *aDst;
  XSurfaceMemory src = *aSrc;
  int            dstS1 = 0;
  int            dstT1 = 0;
  int            dstS2 = aDstW2; /* s, t coordinates premultipied by w */
  int            dstT2 = 0;
  int            dstS3 = aDstW3;
  int            dstT3 = aDstW3;
  int            dstS4 = 0;
  int            dstT4 = aDstW4;
  int            ex1, ex2, ex3, ex4;
  int            ey1, ey2, ey3, ey4;
  int            ew1, ew2, ew3, ew4;
  int            es1, es2, es3, es4;
  int            et1, et2, et3, et4;
  int            y1,  y2, y;
  unsigned int   iey1h   = 0, iey1l = 0, iey2h = 0, iey2l = 0, iey3h = 0,
                 iey3l   = 0, iey4h = 0, iey4l = 0;
  int            affine  = ( aDstW1 == aDstW2 ) && ( aDstW1 == aDstW3 ) &&
                           ( aDstW1 == aDstW4 );
  int            isLarge = ( aDstX1 >  32000 ) || ( aDstX2 >  32000 ) ||
                           ( aDstX3 >  32000 ) || ( aDstX4 >  32000 ) ||
                           ( aDstY1 >  32000 ) || ( aDstY2 >  32000 ) ||
                           ( aDstY3 >  32000 ) || ( aDstY4 >  32000 ) ||
                           ( aDstX1 < -32000 ) || ( aDstX2 < -32000 ) ||
                           ( aDstX3 < -32000 ) || ( aDstX4 < -32000 ) ||
                           ( aDstY1 < -32000 ) || ( aDstY2 < -32000 ) ||
                           ( aDstY3 < -32000 ) || ( aDstY4 < -32000 );

  /* Relative source surface position to start the transfer */
  src.Pixel1 = (char*)src.Pixel1 + aSrcY * src.Pitch1Y + aSrcX * src.Pitch1X;
  src.Pixel2 = (char*)src.Pixel2 + aSrcY * src.Pitch2Y + aSrcX * src.Pitch2X;

  /* Respect the sub pixel precision */
  aClipX1 <<= 3; aClipY1 <<= 3;
  aClipX2 <<= 3; aClipY2 <<= 3;

  /* Determine the first and the last row of the destination bitmap, which 
     are affected by the transfer operation. Respect the upper and the bottom
     borders of the clipping area */
  y1 = ( MIN4( aDstY1, aDstY2, aDstY3, aDstY4 ) + 3 ) & ~7;
  y2 = ( MAX4( aDstY1, aDstY2, aDstY3, aDstY4 ) + 3 ) & ~7;
  y1 =   MAX ( y1, aClipY1 );
  y2 =   MIN ( y2, aClipY2 );

  /* Precalculate some slope constants for the interpolation of x, w, s and t
     coordinates */
  ey1 = aDstY2 - aDstY1; ex1 = aDstX2 - aDstX1; ew1 = aDstW2 - aDstW1;
  ey2 = aDstY3 - aDstY2; ex2 = aDstX3 - aDstX2; ew2 = aDstW3 - aDstW2;
  ey3 = aDstY3 - aDstY4; ex3 = aDstX3 - aDstX4; ew3 = aDstW3 - aDstW4;
  ey4 = aDstY4 - aDstY1; ex4 = aDstX4 - aDstX1; ew4 = aDstW4 - aDstW1;
  es1 = dstS2  - dstS1;  et1 = dstT2  - dstT1;
  es2 = dstS3  - dstS2;  et2 = dstT3  - dstT2;
  es3 = dstS3  - dstS4;  et3 = dstT3  - dstT4;
  es4 = dstS4  - dstS1;  et4 = dstT4  - dstT1;

  /* The entire routine is optimized to calculate with 32-bit integer values
     containing fixed-point numbers with 3-bit for the sub-pixel. Due to used
     multiplications large coordinates may cause overflows. Therefore use 
     aternative algorithm when the coordinates are too large (> 4000 pixel).
     Here particularly, calculate a 64-bit inverse of the given edge. */
  if ( isLarge )
  {
    GetInverseRange( ey1, &iey1h, &iey1l );
    GetInverseRange( ey2, &iey2h, &iey2l );
    GetInverseRange( ey3, &iey3h, &iey3l );
    GetInverseRange( ey4, &iey4h, &iey4l );
  }

  /* Perform the transformation for the affected destination rows only. Note
     the increased precision -> a single pixel within the destination row is
     divided in 8 sub-pixel vertically and 8 sub-pixel horizontally */
  for ( y = y1 + 4; y < y2; y += 8 )
  {
    /* The storage for the left/right intersection with the polygon edges */
    int x1 = 0x7FFFFFFF;
    int x2 = 0x80000000;
    int w1 = 0;
    int w2 = 0;
    int s1 = 0;
    int s2 = 0;
    int t1 = 0;
    int t2 = 0;
    int xx1, xx2;
    int c,   c2;

    /* Calculate the intersection between the current row and the first edge
       of the polygon */
    if ( ey1 && ((( y >= aDstY1 ) && ( y <= aDstY2 )) ||
                 (( y >= aDstY2 ) && ( y <= aDstY1 ))))
      IntersectEdge( 1, 1 );

    /* Calculate the intersection between the current row and the second edge
       of the polygon */
    if ( ey2 && ((( y >= aDstY2 ) && ( y <= aDstY3 )) ||
                 (( y >= aDstY3 ) && ( y <= aDstY2 ))))
      IntersectEdge( 2, 2 );

    /* Calculate the intersection between the current row and the third edge
       of the polygon */
    if ( ey3 && ((( y >= aDstY3 ) && ( y <= aDstY4 )) ||
                 (( y >= aDstY4 ) && ( y <= aDstY3 ))))
      IntersectEdge( 4, 3 );

    /* Calculate the intersection between the current row and the fourth edge
       of the polygon */
    if ( ey4 && ((( y >= aDstY4 ) && ( y <= aDstY1 )) ||
                 (( y >= aDstY1 ) && ( y <= aDstY4 ))))
      IntersectEdge( 1, 4 );

    /* Round up/down to the start/end pixel and respect the left and the right
       clipping borders */
    xx1 = ( x1 + 3 ) & ~7;
    xx2 = ( x2 + 3 ) & ~7;
    xx1 = MAX( xx1, aClipX1 );
    xx2 = MIN( xx2, aClipX2 );

    /* Nothing to draw within the row */
    if ( xx2 <= xx1 )
      continue;

    /* Determine the address and the length of the row to fill within the
       destination surface */
    {
      int dy  = y   >> 3;
      int dx1 = xx1 >> 3;
      int dx2 = xx2 >> 3;

      /* Number of pixel to fill the row */
      c  = dx2 - dx1;
      c2 = c >> 1;

      /* Calculate the address of the first pixel of the row */
      dst.Pixel1 = (char*)aDst->Pixel1 + dy * dst.Pitch1Y + dx1 * dst.Pitch1X;
      dst.Pixel2 = (char*)aDst->Pixel2 + dy * dst.Pitch2Y + dx1 * dst.Pitch2X;
    }

    /* Calculate the s, t, w values for the first and last pixel to draw. These
       s, t, w values control the mapping of pixel from the source surface (the
       texture) to the destination surface */
    {
      int d  = x2  - x1;
      int m1 = xx1 - x1 + 4;
      int m2 = xx2 - x2 + 4;
      int ds = s2  - s1;
      int dt = t2  - t1;
      int dw = w2  - w1;

      /* The first and last pixel of the row need interpolated s, t and w 
         coordinates. Please note, the entire routine is optimized to calculate
         with 32-bit integer values containing fixed-point numbers with 3-bit
         for the sub-pixel. Due to used multiplications large coordinates may
         cause overflows. Therefore use alternative algorithm when the 
         coordinates are too large (> 4000 pixel). */
      if ( !isLarge )
      {
        int d2 = d >> 1;

        /* Avoid evtl. div by zero */
        if ( d )
        {
          s1 += (( m1 * ds ) + d2 ) / d; s2 += (( m2 * ds ) + d2 ) / d;
          t1 += (( m1 * dt ) + d2 ) / d; t2 += (( m2 * dt ) + d2 ) / d;
          w1 += (( m1 * dw ) + d2 ) / d; w2 += (( m2 * dw ) + d2 ) / d;
        }
      }

      /* Large coordinates (> 4000 pixel) expect 64-bit multiplication and
         division. Since such opoerations are note available on typical embedded
         systems, we use our own implementation. */
      else
      {
        unsigned int idh, idl;
        int          factor1, factor2;

        GetInverseRange( d, &idh, &idl );
        factor1 = GetInterpolationFactor( idh, idl, m1 );
        factor2 = GetInterpolationFactor( idh, idl, m2 );
        s1 += InterpolateInRange( ds, factor1 );
        s2 += InterpolateInRange( ds, factor2 );
        t1 += InterpolateInRange( dt, factor1 );
        t2 += InterpolateInRange( dt, factor2 );
        w1 += InterpolateInRange( dw, factor1 );
        w2 += InterpolateInRange( dw, factor2 );
      }

      /* Calculate the increment step for the s, t, w coefficients. When drawing
         the row, the s, t, w values will be incremented for each pixel. Also
         promote the values to 16.12 precision space in order to reduce rounding
         errors */
      s2 = ((( s2 - s1 ) << 12 ) + c2 ) / c; s1 <<= 12;
      t2 = ((( t2 - t1 ) << 12 ) + c2 ) / c; t1 <<= 12;
      w2 = ((( w2 - w1 ) << 12 ) + c2 ) / c; w1 <<= 12;

      /* Avoid rounding errors when performing division operations later */
      s1 += ( 1 << 12 ) + ( 1 << 11 );
      t1 += ( 1 << 12 ) + ( 1 << 11 );
      w1 += ( 1 << 12 ) + ( 1 << 11 );
    }

    /* No perspective distortion? Then run the pure affine transformation only */
    if ( affine )
      aWorker( &dst, &src, c, s1, t1, s2, t2, aSrcWidth, aSrcHeight, aGradient );

    /* Map the source to destination coordinates in perspective manner. This
       expects slow division operations */
    else
    {
      int w12 = w1 >> 13;
      int ss2 = (( s1 + w12 ) / ( w1 >> 12 )) << 16;
      int tt2 = (( t1 + w12 ) / ( w1 >> 12 )) << 16;

      /* The row is drawn as short pixel segments. The start and end cordinates
         of each segment are calculated perspective correctly. The pixel of the
         segment are interpolated lineary - this reduces the impact of division
         operations */
      for ( ;; )
      {
        int cc   = MIN( c, 16 );
        int ccc  = (( cc == c ) && ( cc > 1 ))? cc - 1 : cc;
        int ccc2 = ccc >> 1;
        int ss1  = ss2;
        int tt1  = tt2;
        int sss, ttt;

        /* s, t, w coefficients for the next following segment */
        s1 += s2 * ccc;
        t1 += t2 * ccc;
        w1 += w2 * ccc;

        /* Calculate the perspective correct source coordinates for the end
           of the row */
        w12 = w1 >> 13;
        ss2 = (( s1 + w12 ) / ( w1 >> 12 )) << 16;
        tt2 = (( t1 + w12 ) / ( w1 >> 12 )) << 16;

        /* For the linear interpolation within the segment calculate the step */
        sss = ( ss2 - ss1 ) + ccc2; sss = ( ccc == 16 )? sss >> 4 : sss / ccc;
        ttt = ( tt2 - tt1 ) + ccc2; ttt = ( ccc == 16 )? ttt >> 4 : ttt / ccc;

        /* Avoid rounding errors when performing division operations later */
        ss1 += ( 1 << 12 ) + ( 1 << 11 );
        tt1 += ( 1 << 12 ) + ( 1 << 11 );

        aWorker( &dst, &src, cc, ss1, tt1, sss, ttt, aSrcWidth, aSrcHeight,
          aGradient );

        /* The row is complete */
        if (( c -= cc ) == 0 )
          break;

        /* Otherwise continue with the next segment of the row */
        dst.Pixel1 = (char*)dst.Pixel1 + dst.Pitch1X * cc;
        dst.Pixel2 = (char*)dst.Pixel2 + dst.Pitch2X * cc;
      }
    }
  }
}


/*******************************************************************************
* FUNCTION:
*   EwEmulateFillPolygon
*
* DESCRIPTION:
*  The function EwEmulateFillPolygon() drives a fill polygon operation with 
*  polygon data provided in the array aPaths. aPaths stores polygon edges as a
*  serie of X,Y coordinate pairs starting always with a value specifying the
*  number of edges the path is composed of. With this approach one path can
*  consist of several sub-paths. The end of the list is determined by a sub-
*  path with 0 edges:
*
*  +-------+------+------+------+------+------+------+-------+     +-----+
*  | edges |  X0  |  Y0  |  X1  |  Y1  |  X2  |  Y2  | edges | ... |  0  |
*  +-------+------+------+------+------+------+------+-------+     +-----+
*
*  The function evaluates the path data for intersections between the edges and
*  the pixel within the destination area aDstX, aDstY, aWidth and aHeight. The
*  function modulates the pixel by solid or gradient color values.
*
*  The function provides the top-level interface to the software pixel driver.
*  The real drawing operation will be driven by the passed worker function.
*
* ARGUMENTS:
*   aDst            - Pointer to the first pixel of the destination surface.
*   aPaths          - An array containing the path data. The array starts with
*     the number of edges a path is composed of. Then follow the coordinates of
*     all path corners as X,Y pairs. After the last coordinate pair next path
*     can follow starting again with the number of edges. The end of the path
*     data is signed with 0. The X,Y coordinates are stored as signed integer
*     with 4-bit fixpoint precision. The coordinates are valid relative to the
*     top-left corner of the destination bitmap.
*   aDstX,
*   aDstY           - Origin of the area to fill (relative to the top-left 
*     corner of the destination surface).
*   aWidth,
*   aHeight         - Size of the area to fill.
*   aAntialiased    - If != 0, the function applies antialiasing to the pixel.
*     The antialiasing is based on supersampling with 4 samples in X and Y
*     direction.
*   aNonZeroWinding - Controls the fill rule to be used by the algorithm. If
*    this parameter is == 0, the even-odd fill rule is used. If this parameter
*    is != 0, the non-zero winding rule is used.
*   aGradient       - Color information to modulate the copied pixel.
*   aGrdX,
*   aGrdY           - Origin of the affected area in the gradient coordinate 
*     space.
*   aWorker         - Low-level worker function to perform the final copy 
*     operation of the rasterized pixel row-wise.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwEmulateFillPolygon( XSurfaceMemory* aDst, int* aPaths, int aDstX, 
  int aDstY, int aWidth, int aHeight, int aAntialiased, int aNonZeroWinding,
  XGradient* aGradient, int aGrdX, int aGrdY, XCopyWorker aWorker )
{
  #ifndef EW_DONT_USE_PATH_FUNCTIONS
    int            bufSize   = aWidth * ( aNonZeroWinding? 9 : 
                                          aAntialiased? 5 : 2 );
    XSurfaceMemory src       = { 0 };
    int            mask1     = 0x0FF00000;
    XGradient      grad;
    int            rl, gl, bl, al;
    int            rr, gr, br, ar;
    int            isHorz    = aGradient->IsHorizontal;
    void*          savedDst1 = aDst->Pixel1;
    void*          savedDst2 = aDst->Pixel2;
    int            x         = aDstX << 4;
    unsigned char* buf;
    unsigned char* pixel;
    unsigned char* edgeBuf;

    do
      buf = EwAlloc( bufSize );
    while ( !buf && EwImmediateReclaimMemory( 16 ));

    /* We were not able to reserve memory for temporary pixel and edge buffers.
       Print just a warning and continue. The application should not enter 
       panic in this case. */
    if ( !buf )
    {
      EwError( 16 );
      return;
    }

    pixel   = buf + bufSize - aWidth;
    edgeBuf = buf;

    /* Clear the temporary buffers */
    EwZero( buf, bufSize );

    /* Start the gradient with the top-left and top-right colors */
    rl = aGradient->R0; rr = aGradient->R2;
    gl = aGradient->G0; gr = aGradient->G2;
    bl = aGradient->B0; br = aGradient->B2;
    al = aGradient->A0; ar = aGradient->A2;

    /* Adjust the gradient start values, if there is an offset between the origin
       of the gradient and the origin of the area affected by the operations */
    if ( aGradient->IsVertical && aGrdY )
    {
      rl += aGradient->R1 * aGrdY; rr += aGradient->R3 * aGrdY;
      gl += aGradient->G1 * aGrdY; gr += aGradient->G3 * aGrdY;
      bl += aGradient->B1 * aGrdY; br += aGradient->B3 * aGrdY;
      al += aGradient->A1 * aGrdY; ar += aGradient->A3 * aGrdY;
    }

    /* Start color for the first row */
    grad.R0 = rl & mask1; grad.G0 = gl & mask1;
    grad.B0 = bl & mask1; grad.A0 = al & mask1;
    grad.R1 = 0;          grad.G1 = 0;
    grad.B1 = 0;          grad.A1 = 0;

    /* Repeat until all rows have been processed */
    for ( ; aHeight > 0; aHeight--, aDstY++ )
    {
      int y     = aDstY << 4;
      int first = 0;
      int count;

      /* Raster the pixel row */
      if ( aNonZeroWinding && aAntialiased )
        count = RasterPolygonRow_AA_NZW( pixel, edgeBuf, aWidth, x, y, aPaths,
                                         &first );
      else if ( aNonZeroWinding )
        count = RasterPolygonRow_NZW( pixel, edgeBuf, aWidth, x, y, aPaths,
                                      &first );
      else if ( aAntialiased )
        count = RasterPolygonRow_AA_EO( pixel, edgeBuf, aWidth, x, y, aPaths,
                                        &first );
      else
        count = RasterPolygonRow_EO( pixel, edgeBuf, aWidth, x, y, aPaths,
                                     &first );

      /* If there is a horizontal trend in the gradient - recalculate the color
         slope values for the next following pixel row */
      if ( isHorz && count )
      {
        grad.R1 = (( rr >> 20 ) - ( rl >> 20 )) * aGradient->InvWidth;
        grad.G1 = (( gr >> 20 ) - ( gl >> 20 )) * aGradient->InvWidth;
        grad.B1 = (( br >> 20 ) - ( bl >> 20 )) * aGradient->InvWidth;
        grad.A1 = (( ar >> 20 ) - ( al >> 20 )) * aGradient->InvWidth;

        /* Adjust the gradient start values, if there is an offset between the
           origin of the gradient and the origin of the area affected by the
           operations */
        if ( aGrdX )
        {
          grad.R0 += grad.R1 * aGrdX; grad.G0 += grad.G1 * aGrdX;
          grad.B0 += grad.B1 * aGrdX; grad.A0 += grad.A1 * aGrdX;
        }

        /* If there is no additional vertical trend in the gradient (horizontal
           gradient only) - omit this recalculation step for the next time */
        isHorz = aGradient->IsVertical;
      }

      /* Any pixel rasterized for the row? */
      if ( count )
      {
        int tmpR0 = grad.R0;
        int tmpG0 = grad.G0; 
        int tmpB0 = grad.B0;
        int tmpA0 = grad.A0;

        /* Rasterizing the polygon with horizontal gradient requires the gradient
           colors to be adjusted to the first affected pixel if it is not the left
           edge of the destination area */
        if ( aGradient->IsHorizontal && first )
        {
          grad.R0 += grad.R1 * first; grad.G0 += grad.G1 * first;
          grad.B0 += grad.B1 * first; grad.A0 += grad.A1 * first;
        }

        /* The first affected pixel in the rasterized buffer to copy to the
           destination bitmap */
        src.Pixel1 = pixel + first;

        /* Relative destination position to start the transfer */
        aDst->Pixel1 = (char*)savedDst1 + aDstY * aDst->Pitch1Y + 
                                        ( aDstX + first ) * aDst->Pitch1X;
        aDst->Pixel2 = (char*)savedDst2 + aDstY * aDst->Pitch2Y +
                                        ( aDstX + first ) * aDst->Pitch2X;

        /* Copy the rasterized pixel */
        aWorker( aDst, &src, count, &grad );

        /* Restore the gradient state */
        grad.R0 = tmpR0; grad.G0 = tmpG0;
        grad.B0 = tmpB0; grad.A0 = tmpA0;

        /* For the next run clear the just copied pixel */
        if ( aHeight > 1 )
          EwZero( pixel + first, count );
      }

      /* If there is a vertical trend in the gradient - adapt the color values
         for the next following pixel row */
      if ( aGradient->IsVertical )
      {
        grad.R0 = ( rl += aGradient->R1 ) & mask1; rr += aGradient->R3;
        grad.G0 = ( gl += aGradient->G1 ) & mask1; gr += aGradient->G3;
        grad.B0 = ( bl += aGradient->B1 ) & mask1; br += aGradient->B3;
        grad.A0 = ( al += aGradient->A1 ) & mask1; ar += aGradient->A3;
      }
    }

    /* Restore the pixel pointers - may be the caller will reuse the surface
       memory structure */
    aDst->Pixel1 = savedDst1;
    aDst->Pixel2 = savedDst2;

    /* Track the temporary used memory */
    EwResourcesMemory += sizeof( bufSize );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    EwResourcesMemory -= sizeof( bufSize );

    /* Release the temporary used memory */
    EwFree( buf );
  #else
    EW_UNUSED_ARG( aDst );
    EW_UNUSED_ARG( aPaths );
    EW_UNUSED_ARG( aDstX );
    EW_UNUSED_ARG( aDstY );
    EW_UNUSED_ARG( aWidth );
    EW_UNUSED_ARG( aHeight );
    EW_UNUSED_ARG( aAntialiased );
    EW_UNUSED_ARG( aNonZeroWinding );
    EW_UNUSED_ARG( aGradient );
    EW_UNUSED_ARG( aGrdX );
    EW_UNUSED_ARG( aGrdY );
    EW_UNUSED_ARG( aWorker );
  #endif /* EW_DONT_USE_PATH_FUNCTIONS */
}


/*******************************************************************************
* FUNCTION:
*   EwAllocVideo
*
* DESCRIPTION:
*   The function EwAllocVideo() has the job to reserve a block of memory which
*   is exclusively intended for pure software surfaces.
*
*   The large size and the persistent character of surfaces may expect special
*   heap management and algorithms. The intension of this function is to handle
*   surface memory allocations in a separate way of any other allocations.
*
* ARGUMENTS:
*   aSize - Desired size of the new memory block in bytes.
*
* RETURN VALUE:
*   If successful, returns the pointer to the reserved memory block, otherwise
*   null is returned.
*
*******************************************************************************/
void* EwAllocVideo( int aSize )
{
  void* tmp;

  do
    #ifdef EwGfxAllocVideo
      tmp = EwGfxAllocVideo( aSize );
    #else
      tmp = EwAlloc( aSize );
    #endif
  while ( !tmp && aSize && EwImmediateReclaimMemory( 41 ));

  if ( !tmp && aSize )
    EwError( 41 );

  return tmp;
}


/*******************************************************************************
* FUNCTION:
*   EwFreeVideo
*
* DESCRIPTION:
*   The function EwFreeVideo() is the counterpart of EwAllocVideo() function.
*   Its job is to release the given memory block.
*
*   The large size and the persistent character of surfaces may expect special
*   heap management and algorithms. The intension of this function is to handle
*   surface memory allocations in a separate way of any other allocations.
*
* ARGUMENTS:
*   aMemory - Address of the memory block to release.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeVideo( void* aMemory )
{
  #ifdef EwGfxFreeVideo
    EwGfxFreeVideo( aMemory );
  #else
    EwFree( aMemory );
  #endif
}


/* Helper function for minimum value determination */
static int MIN4( int a, int b, int c, int d )
{
  if ( b < a ) a = b; 
  if ( d < c ) c = d; 

  return ( a < c )? a : c;
}


/* Helper function for maximum value determination */
static int MAX4( int a, int b, int c, int d )
{
  if ( b > a ) a = b; 
  if ( d > c ) c = d; 

  return ( a > c )? a : c;
}


/* pba */

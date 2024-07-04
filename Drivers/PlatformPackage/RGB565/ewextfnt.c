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
*   This file contains the implementation of a font resource provider intended
*   to load pre-rasterized and compressed font resources directly from the code
*   memory as it will be generated by Embedded Wizard resource converters.
*
*******************************************************************************/

#include "ewrte.h"
#include "ewextfnt.h"
#include "ewgfxdriver.h"
#include "ewextgfx.h"
#include "ewgfxdefs.h"


/* The following helper function searches the glyphs array aGlyphs for the 
   character code aCharCode. */
static const XFntGlyphRes* FindGlyph( unsigned short aCharCode, 
  const XFntGlyphRes* aGlyphs, int aCount );


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress1( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount, unsigned char* aDestLimit );


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress2( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount, unsigned char* aDestLimit );


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress4( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount );


/*  The following helper function combines the pixel of upper rows with the 
    pixel of the lower rows. */
static void XOrRows( unsigned char* aData, int aCount, int aWidth );


/*  The following helper function copies the glyph pixel rows from the source
    to the destination memory. */
static void CopyRows( XSurfaceMemory* aDst, unsigned char* aSrc, int aWidth,
  int aHeight );


/* Following variables describe Flash memory which is not directly accessible
   by the CPU. In order to read the Flash memory, a user own reader procedure
   is used. Please see the function EwRegisterFlashAreaReader(). */
extern XFlashAreaReaderProc EwFlashAreaReaderProc;
extern void*                EwFlashAreaStartAddress;
extern void*                EwFlashAreaEndAddress;
extern int                  EwFlashAreaBlockSize;


/* These global variables store the number of bytes occupied by Chora objects */
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwMemoryPeak;


/*******************************************************************************
* FUNCTION:
*   EwFntInit
*
* DESCRIPTION:
*   The function EwFntInit() is called during the initialization of the Graphics
*   Engine. Its intention is to give the external loader a chance to initialize
*   its private variables and to start the TrueType font engine functionality
*   (if any).
*
* ARGUMENTS:
*   aArgs - User argument passed in the EwInitGraphicsEngine().
*
* RETURN VALUE:
*   If successful, returns != 0.
*
*******************************************************************************/
int EwFntInit( void* aArgs )
{
  EW_UNUSED_ARG( aArgs );
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwFntDone
*
* DESCRIPTION:
*   The function EwFntDone() will be called during the shutdown of the Graphics
*   Engine. It gives the loader a chance to deinitialize itself just before the
*   Graphics Engine terminates.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFntDone( void )
{
}


/*******************************************************************************
* FUNCTION:
*   EwFntGetResource
*
* DESCRIPTION:
*   The function EwFntGetResource() has the job to find or create a new resource
*   descriptor matching the attributes specified in the function parameters. The
*   returned resource can thereupon be passed to EwLoadFont() function in order
*   to use the font.
*
*   Please note: this function allows the caller to flexibly create fonts on
*   target systems supporting a TrueType font engine. In the case, you are using
*   the version without TrueType font engine integration, all fonts are already
*   stored as prerasterized glyphs. Dynamic creation of a font with different
*   attributes is in such case not possible and the function will return null.
*
* ARGUMENTS:
*   aName        - A unique name of the TrueType font stored as zero terminated
*     ASCII string (e.g. "Arial"). The function limits the evaluation to the
*     first 31 characters from the string.
*   aHeight      - The desired height of the font in pixel.
*   aBold        - This parameter determines, whether a bold or a normal style
*     of the font should be used at the runtime. A bold font will be used if
*     this parameter contains a value != 0.
*   aItalic      - This parameter determines, whether an italic or a normal
*     style of the font should be used at the runtime. An italic font is used
*     if this parameter contains a value != 0.
*   aAspectRatio - The AspectRatio parameter defines the desired aspect ratio
*     of the font in the range from 0.25 to 4.0. The default value is 1.0 - in
*     this case the aspect ratio of the font corresponds to the origin design
*     of the font.
*   aNoOfColors  - Desired quality of the font glyphs to raster. Only the 
*     values 2, 4 or 16 are valid.
*   aKerning     - Determines whether kerning data should be used for this
*     font. In such case the value has to be != 0.
*   aRowDistance - The desired distance between two consecutive text rows. If
*     the value is == 0, the distance is calculated on the base of the font
*     metrics.
*   aPopularMode - If != 0, calculate the font size similarly to how other
*     application it does. If == 0, the mode compatible to older Embedded
*     Wizard version is used.
*
* RETURN VALUE:
*   Returns a pointer to a data structure representing the font resource. If
*   the target system does not support the dynamic font creation, the function
*   will return null.
*
*******************************************************************************/
const struct XFntRes* EwFntGetResource( const char* aName, int aHeight,
  int aBold, int aItalic, float aAspectRatio, int aNoOfColors, int aKerning,
  int aRowDistance, int aPopularMode )
{
  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwFntOpen
*
* DESCRIPTION:
*   The function EwFntOpen() will be called by the Graphics Engine in order to
*   initiate the access to the given font resource. Depending on the manner,
*   the resources are handled in the particular platform system, the function
*   can open a TrueType file or it can access font information available in the
*   code memory.
*
*   The function should return a handle to the opened resource. By using this
*   handle, the font content and its metrics can be requested by the Graphics
*   Engine without taking in account any platform specific aspects.
*
* ARGUMENTS:
*   aResource - Descriptor of the font resource to open. The content of this
*     descriptor depends on the particular platform system.
*
* RETURN VALUE:
*   If sucessful, the function returns the handle to the opened font resource.
*   If failed, the function returns null.
*
*******************************************************************************/
unsigned long EwFntOpen( const struct XFntRes* aResource )
{
  /* Verify, whether the binary resource does fit to this loader */
  if ( aResource && ( aResource->MagicNo != EW_MAGIC_NO_FONT ))
  {
    EwErrorPD( 207, aResource, 0 );
    EwPanic();
    return 0;
  }

  /* This interface works with the original data structure as it was stored in
     the code memory */
  return (unsigned long)aResource;
}


/*******************************************************************************
* FUNCTION:
*   EwFntClose
*
* DESCRIPTION:
*   The function EwFntClose() will be called by the Graphics Engine to finish
*   the processing of a font resource. Depending on the manner, the resources
*   are handled in the particular platform system, the function can close the
*   previously opened file and free any temporarily reserved resources.
*
* ARGUMENTS:
*   aHandle - Handle to the font resource to terminate the access.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFntClose( unsigned long aHandle )
{
  EW_UNUSED_ARG( aHandle );
}


/*******************************************************************************
* FUNCTION:
*   EwFntGetResourceName
*
* DESCRIPTION:
*   The function EwFntGetResourceName() will be called by the Graphics Engine in
*   order to query the name of the corresponding font resource member. This name
*   is used to display error messages and warnings.
*
*   The function should return a pointer to a zero terminated char string.
*
* ARGUMENTS:
*   aResource - Descriptor of the font resource to query its name.
*
* RETURN VALUE:
*   If sucessful, the function returns the pointer to the char string.
*
*******************************************************************************/
const char* EwFntGetResourceName( const struct XFntRes* aResource )
{
  if ( aResource )
    return aResource->Name;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwFntGetMetrics
*
* DESCRIPTION:
*   The function EwFntGetMetrics() will be called by the Graphics Engine to get
*   the general attributes of the font resource. By using of these attributes,
*   Graphics Engine can create and initialize new font instances.
*
* ARGUMENTS:
*   aHandle   - Handle to the font resource to determine its attributes.
*   aAscent,
*   aDescent,
*   aLeading  - Pointers to variables, where the font metrics will be returned.
*   aDefChar  - Pointers to variable, where the default character code will be
*     returned.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwFntGetMetrics( unsigned long aHandle, int* aAscent, int* aDescent,
  int* aLeading, XChar* aDefChar )
{
  const XFntRes* res = (const XFntRes*)aHandle;

  *aAscent  = res->Ascent;
  *aDescent = res->Descent;
  *aLeading = res->Leading;
  *aDefChar = res->DefChar;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwFntGetGlyphMetrics
*
* DESCRIPTION:
*   The function EwFntGetGlyphMetrics() will be called by the Graphics Engine in
*   order to obtain the metrics of a single glyph from the given font.
*
* ARGUMENTS:
*   aHandle   - Handle to the font resource to determine its attributes.
*   aCharCode - Character code of the glyph to be inspected.
*   aOriginX,
*   aOriginY  - Pointers to variables, where the origin offset in pixel of the
*     desired glyph will be returned.
*   aWidth,
*   aHeight   - Pointers to variables, where the size in pixel of the glyph will
*     be returned.
*   aAdvance  - Pointer to a variable, where the horizontal advance in pixel for
*     the glyph will be returned.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwFntGetGlyphMetrics( unsigned long aHandle, unsigned short aCharCode,
  int* aOriginX, int* aOriginY, int* aWidth, int* aHeight, int* aAdvance )
{
  const XFntRes*      res   = (const XFntRes*)aHandle;
  const XFntGlyphRes* glyph = FindGlyph( aCharCode, res->Glyphs, 
                                         res->NoOfGlyphs );

  /* if there is no glyph resource for the desired character */
  if ( !glyph )
    return 0;

  /* The memory containing the glyph metrics is not directly accessible. Use a
     reader to access this data. * */
  if ( EwFlashAreaReaderProc &&
     ((void*)glyph >= EwFlashAreaStartAddress ) &&
     ((void*)glyph <= EwFlashAreaEndAddress   ))
  {
    *aOriginX = *(const short*)EwFlashAreaReaderProc( &glyph->OriginX );
    *aOriginY = *(const short*)EwFlashAreaReaderProc( &glyph->OriginY );
    *aWidth   = *(const short*)EwFlashAreaReaderProc( &glyph->Width   );
    *aHeight  = *(const short*)EwFlashAreaReaderProc( &glyph->Height  );
    *aAdvance = *(const short*)EwFlashAreaReaderProc( &glyph->Advance );
  }
  else
  {
    *aOriginX = glyph->OriginX;
    *aOriginY = glyph->OriginY;
    *aWidth   = glyph->Width;
    *aHeight  = glyph->Height;
    *aAdvance = glyph->Advance;
  }

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwFntGetKerning
*
* DESCRIPTION:
*   The function EwFntGetKerning() will be called by the Graphics Engine in
*   order to obtain the kerning value for the pair of two glyphs from the given
*   font.
*
* ARGUMENTS:
*   aHandle    - Handle to the font resource to determine the kerning.
*   aCharCode1 - Character code of the first glyph.
*   aCharCode2 - Character code of the second glyph.
*
* RETURN VALUE:
*   The function returns the kerning value for the given pair of glyphs or 0
*   if the glyphs are not affected by kerning or at least one of the glyphs is
*   not existing in the font.
*
*******************************************************************************/
int EwFntGetKerning( unsigned long aHandle, unsigned short aCharCode1,
  unsigned short aCharCode2 )
{
  const XFntRes*      res       = (const XFntRes*)aHandle;
  const unsigned int* codes     = res->KerningCodes;
  unsigned int        key       = aCharCode1 | ( aCharCode2 << 16 );
  int                 useReader = ( EwFlashAreaReaderProc &&
                                  ((void*)codes >= EwFlashAreaStartAddress ) &&
                                  ((void*)codes <= EwFlashAreaEndAddress   ));
  int                 inx       = 0;
  int                 min       = 0;
  int                 comp      = -1;
  int                 max       = 0;

  /* No kerning with a zero character */
  if ( !aCharCode1 || !aCharCode2 )
    return 0;

  if ( codes &&  useReader ) max = *(const int*)EwFlashAreaReaderProc( codes );
  if ( codes && !useReader ) max = *(const int*)codes;

  /* No kerning information available in this font */
  if ( max <= 0 )
    return 0;

  /* The first array value stores the number entries. Skip over. Adjust the
     'max' to refer to the last entry in the array */
  codes++;
  max -= 2;

  /* Repeat until the entry affecting the both characters is found, or the end 
     of the array is reached */
  while( max >= min )
  {
    inx  = ( max + min ) / 2;

    /* If the memory containing the kerning data is not directly accessible,
       use a reader for this purpose */
    if ( useReader )
    {
      comp = key - *(const unsigned int*)EwFlashAreaReaderProc( codes + inx ); 

      if ( !comp )
        return *(const unsigned char*)EwFlashAreaReaderProc( res->KerningValues +
                                                             inx ) - 128;
    }
    else
    {
      comp = key - codes[ inx ]; 

      if ( !comp )
        return res->KerningValues[ inx ] - 128;
    }

    /* Continue the search operation */
    if (  comp < 0 ) max = inx - 1;
    else             min = inx + 1;
  }

  /* No entry fount */
  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwFntIsGlyphAvailable
*
* DESCRIPTION:
*   The function EwFntIsGlyphAvailable() will be called by the Graphics Engine
*   in order to verify whether the given font contains a glyph for the character
*   aCharCode.
*
* ARGUMENTS:
*   aHandle   - Handle to the font resource to determine the glyph existence.
*   aCharCode - Character code of the glyph.
*
* RETURN VALUE:
*   The function returns != 0 if the glyph exists. Otherwise 0 is returned.
*
*******************************************************************************/
int EwFntIsGlyphAvailable( unsigned long aHandle, unsigned short aCharCode )
{
  const XFntRes*      res   = (const XFntRes*)aHandle;
  const XFntGlyphRes* glyph = FindGlyph( aCharCode, res->Glyphs, 
                                         res->NoOfGlyphs );

  /* if there is no glyph resource for the desired character */
  return glyph != 0;
}


/*******************************************************************************
* FUNCTION:
*   EwFntLoadGlyph
*
* DESCRIPTION:
*   The function EwFntLoadGlyph() will be called by the Graphics Engine in order
*   to load the glyph pixel data from the openend font resource aHandle into the
*   given memory area aMemory. Depending on the manner, a resource is handled in
*   the particular platform, the pixel data may be loaded from a TrueType file
*   or they can be decompressed from the code memory.
*
* ARGUMENTS:
*   aHandle   - Handle to the font resource to load its glyph pixel data.
*   aCharCode - Character code of the glyph to be loaded.
*   aMemory   - Memory descriptor containing pointers and pitch information of
*     the destination to write the glyph pixel data.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwFntLoadGlyph( unsigned long aHandle, unsigned short aCharCode,
  XSurfaceMemory* aMemory )
{
  const XFntRes*      res   = (const XFntRes*)aHandle;
  const XFntGlyphRes* glyph = FindGlyph( aCharCode, res->Glyphs, 
                                         res->NoOfGlyphs );
  int                 glyphWidth;
  int                 glyphHeight;
  int                 glyphPixel;
  int                 glyph1Pixel;

  /* if there is no glyph resource for the desired character */
  if ( !glyph )
    return 0;

  /* The metric information is stored in a not directly accessible memory area.
     Use the reader to copy this information. */
  if ( EwFlashAreaReaderProc &&
     ((void*)glyph >= EwFlashAreaStartAddress ) &&
     ((void*)glyph <= EwFlashAreaEndAddress   ))
  {
    glyphWidth  = *(const short*)       EwFlashAreaReaderProc( &glyph->Width   );
    glyphHeight = *(const short*)       EwFlashAreaReaderProc( &glyph->Height  );
    glyphPixel  = *(const unsigned int*)EwFlashAreaReaderProc( &glyph->Pixel   );
    glyph1Pixel = *(const unsigned int*)EwFlashAreaReaderProc( &glyph[1].Pixel );
  }
  else
  {
    glyphWidth  = glyph->Width;
    glyphHeight = glyph->Height;
    glyphPixel  = glyph->Pixel;
    glyph1Pixel = glyph[1].Pixel;
  }

  /* No decompression necessary because there is no pixel data for the glyph */
  if ( !glyphWidth || !glyphHeight )
    return 1;

  /* Now decompress the pixel data. Note the temporary memory area - it is
     needed because the decompression can work with continuous memory only. */
  {
    int            size = glyphWidth  * glyphHeight;
    int            bits = glyph1Pixel - glyphPixel;
    unsigned char* pixel;

    do
      pixel = EwAlloc( size );
    while ( !pixel && EwImmediateReclaimMemory( 35 ));

    /* No temporary memory for the decompressed pixel data */
    if ( !pixel )
    {
      EwError( 35 );
      return 0;
    }

    /* Track the temporary used memory */
    EwResourcesMemory += size;

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    /* Decompress the pixel information of the glyph ... */
    switch ( res->NoOfColors )
    {
      case 2 :
        Decompress1( res->Pixel, pixel, glyphPixel, bits, pixel + size );
      break;
      case 4 :
        Decompress2( res->Pixel, pixel, glyphPixel, bits, pixel + size );
      break;
      case 16 :
        Decompress4( res->Pixel, pixel, glyphPixel, bits );
      break;
    }

    /* ... and XOR the rows */
    XOrRows( pixel, size, glyphWidth );

    /* At fin, transfer the decompressed compact pixel information to the 
       destination */
    CopyRows( aMemory, pixel, glyphWidth, glyphHeight );

    /* Dont't forget to release the temporary memory */
    EwFree( pixel );
    EwResourcesMemory -= size;
  }

  return 1;
}


/* A table used as a node tree for the decompression. */
static const unsigned char Tree[ 15 ][ 2 ] =
{
  { 0x10, 0x01 },
  { 0x02, 0x05 },
  { 0x03, 0x04 },
  { 0x11, 0x12 },
  { 0x14, 0x18 },
  { 0x06, 0x09 },
  { 0x07, 0x08 },
  { 0x13, 0x16 },
  { 0x17, 0x1C },
  { 0x0A, 0x0B },
  { 0x1E, 0x1F },
  { 0x0C, 0x0D },
  { 0x15, 0x19 },
  { 0x1A, 0x0E },
  { 0x1B, 0x1D }
};


/* The following helper function searches the glyphs array aGlyphs for the 
   character code aCharCode. */
static const XFntGlyphRes* FindGlyph( unsigned short aCharCode, 
  const XFntGlyphRes* aGlyphs, int aCount )
{
  int index     = 0;
  int min       = 0;
  int max       = aCount - 1;
  int comp      = -1;
  int useReader = ( EwFlashAreaReaderProc &&
                  ((void*)aGlyphs >= EwFlashAreaStartAddress ) &&
                  ((void*)aGlyphs <= EwFlashAreaEndAddress   ));
  
  /* repeat until the desired glyph resource is found, or the end of the 
     array is reached */
  while( max >= min )
  {
    index = ( max + min ) / 2;

    /* If the memory containing the metric data is not directly accessible,
       use a reader for this purpose */
    if ( useReader )
      comp  = aCharCode - *(const unsigned short*)EwFlashAreaReaderProc( 
                                                  &aGlyphs[ index ].CharCode );
    else
      comp  = aCharCode - aGlyphs[ index ].CharCode;

    /* found? */
    if ( !comp )
      return &aGlyphs[ index ];

    /* otherwise confine the search to the upper half of the array */
    if ( comp > 0 )
      min = index + 1 ;

    /* or to the lower half */
    else if ( comp < 0 )
      max = index - 1;
  }

  /* not found */
  return 0;
}


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress1( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount, unsigned char* aDestLimit )
{
  unsigned int        data;
  int                 inx       = 0;
  const unsigned int* src       = aSrc;
  int                 useReader = EwFlashAreaReaderProc && 
                                   ((void*)aSrc >= EwFlashAreaStartAddress ) && 
                                   ((void*)aSrc <= EwFlashAreaEndAddress );

  /* Determinate the address of the data word where the compressed bit-
     stream begins and calculate the offset to the first data bit within
     the word. */
  aSrc    += aOffset / 32;
  src     += aOffset / 32;
  aOffset %= 32;

  /* The data is stored in not directly accessible memory area - load the block 
     containing the data first */
  if ( useReader )
    src = EwFlashAreaReaderProc( aSrc );

  /* Get the content of this data word and shift it, so the first compressed
     bit lies at the bit position 0. */
  data = *src++ >> aOffset;
  aSrc++;

  /* Repeat until all compressed bits are evaluated. */
  while ( aCount-- )
  {
    /* Determinate the next node in the tree. */
    if ( data & 1 ) inx = Tree[ inx ][ 1 ];
    else            inx = Tree[ inx ][ 0 ];

    /* A leaf node is reached? Store the uncompressed bits and start the
       next compressed sequence. */
    if ( inx > 15 )
    {
      inx -= 16;

      *aDest++ = (unsigned char)(( inx & 0x01 )? 0xFF : 0x00 );

      /* Is there a space to take one more? */
      if ( aDest != aDestLimit )
        *aDest++ = (unsigned char)(( inx & 0x02 )? 0xFF : 0x00 );

      /* Is there a space to take one more? */
      if ( aDest != aDestLimit )
        *aDest++ = (unsigned char)(( inx & 0x04 )? 0xFF : 0x00 );

      /* Is there a space to take one more? */
      if ( aDest != aDestLimit )
        *aDest++ = (unsigned char)(( inx & 0x08 )? 0xFF : 0x00 );

      inx = 0;
    }
      
    /* Continue with the next bit - if all bits are processed, get the
       content of the next word. */
    if (( aOffset = ( aOffset + 1 )) & 0x1F )
      data >>= 1;
    else
    {
      /* The data from the previously loaded block is processed. Get the next 
         block. */
      if ( useReader && !((unsigned long)aSrc & ( EwFlashAreaBlockSize - 1 )))
        src = EwFlashAreaReaderProc( aSrc );

      data = *src++;
      aSrc++;
    }
  }
}


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress2( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount, unsigned char* aDestLimit )
{
  static const unsigned char trans[] = { 0x00, 0x55, 0xAA, 0xFF };

  unsigned int        data;
  int                 inx       = 0;
  const unsigned int* src       = aSrc;
  int                 useReader = EwFlashAreaReaderProc && 
                                   ((void*)aSrc >= EwFlashAreaStartAddress ) && 
                                   ((void*)aSrc <= EwFlashAreaEndAddress );

  /* Determinate the address of the data word where the compressed bit-
     stream begins and calculate the offset to the first data bit within
     the byte. */
  aSrc    += aOffset / 32;
  src     += aOffset / 32;
  aOffset %= 32;

  /* The data is stored in not directly accessible memory area - load the block 
     containing the data first */
  if ( useReader )
    src = EwFlashAreaReaderProc( aSrc );

  /* Get the content of this data word and shift it, so the first compressed
     bit lies at the bit position 0. */
  data = *src++ >> aOffset;
  aSrc++;

  /* Repeat until all compressed bits are evaluated. */
  while ( aCount-- )
  {
    /* Determinate the next node in the tree. */
    if ( data & 1 ) inx = Tree[ inx ][ 1 ];
    else            inx = Tree[ inx ][ 0 ];

    /* A leaf node is reached? Store the uncompressed bits and start the
       next compressed sequence. */
    if ( inx > 15 )
    {
      inx -= 16;

      *aDest++ = trans[ inx & 0x03 ];

      /* Is there a space to take one more? */
      if ( aDest != aDestLimit )
        *aDest++ = trans[ inx >> 2 ];

      inx = 0;
    }
      
    /* Continue with the next bit - if all bits are processed, get the
       content of the next word. */
    if (( aOffset = ( aOffset + 1 )) & 0x1F )
      data >>= 1;
    else
    {
      /* The data from the previously loaded block is processed. Get the next 
         block. */
      if ( useReader && !((unsigned long)aSrc & ( EwFlashAreaBlockSize - 1 )))
        src = EwFlashAreaReaderProc( aSrc );

      data = *src++;
      aSrc++;
    }
  }
}


/* The following function decompresses aCount bits from the bit stream aSrc
   starting at the position aOffset and stores the decompressed data in aDest */
static void Decompress4( const unsigned int* aSrc, unsigned char* aDest,
  int aOffset, int aCount )
{
  unsigned int        data;
  int                 inx       = 0;
  const unsigned int* src       = aSrc;
  int                 useReader = EwFlashAreaReaderProc && 
                                   ((void*)aSrc >= EwFlashAreaStartAddress ) && 
                                   ((void*)aSrc <= EwFlashAreaEndAddress );

  /* Determinate the address of the data word where the compressed bit-
     stream begins and calculate the offset to the first data bit within
     the word. */
  aSrc    += aOffset / 32;
  src     += aOffset / 32;
  aOffset %= 32;

  /* The data is stored in not directly accessible memory area - load the block 
     containing the data first */
  if ( useReader )
    src = EwFlashAreaReaderProc( aSrc );

  /* Get the content of this data word and shift it, so the first compressed
     bit lies at the bit position 0. */
  data = *src++ >> aOffset;
  aSrc++;

  /* Repeat until all compressed bits are evaluated. */
  while ( aCount-- )
  {
    /* Determinate the next node in the tree. */
    if ( data & 1 ) inx = Tree[ inx ][ 1 ];
    else            inx = Tree[ inx ][ 0 ];

    /* A leaf node is reached? Store the uncompressed bits and start the
       next compressed sequence. */
    if ( inx > 15 )
    {
      inx -= 16;

      *aDest++ = (unsigned char)( inx | ( inx << 4 ));

      inx = 0;
    }
      
    /* Continue with the next bit - if all bits are processed, get the
       content of the next word. */
    if (( aOffset = ( aOffset + 1 )) & 0x1F )
      data >>= 1;
    else
    {
      /* The data from the previously loaded block is processed. Get the next 
         block. */
      if ( useReader && !((unsigned long)aSrc & ( EwFlashAreaBlockSize - 1 )))
        src = EwFlashAreaReaderProc( aSrc );

      data = *src++;
      aSrc++;
    }
  }
}


/*  The following helper function combines the pixel of upper rows with the 
    pixel of the lower rows. */
static void XOrRows( unsigned char* aData, int aCount, int aWidth )
{
  unsigned char* dest = aData + aWidth;

  /* start with the second row. */
  aCount -= aWidth;

  /* repeat until all pixel are evaluated */
  while ( aCount-- )
    *dest++ ^= *aData++;
}


/*  The following helper function copies the glyph pixel rows from the source
    to the destination memory. */
static void CopyRows( XSurfaceMemory* aDst, unsigned char* aSrc, int aWidth,
  int aHeight )
{
  unsigned char* dst = aDst->Pixel1;

  /* Copy the pixel data - row by row */
  #if EW_SURFACE_ROTATION == 0
    while ( aHeight-- > 0 )
    {
      int width = aWidth;

      while ( width-- > 0 )
        *dst++ = *aSrc++;

      dst += aDst->Pitch1Y - aWidth;
    }
  #endif

  #if EW_SURFACE_ROTATION == 90
    int width = aWidth;
    aSrc += ( aHeight - 1 ) * aWidth;

    while ( width-- > 0 )
    {
      int height = aHeight;

      while ( height-- > 0 )
      {
        *dst++ = *aSrc;
        aSrc  -= aWidth;
      }

      dst  += aDst->Pitch1Y - aHeight;
      aSrc += ( aHeight * aWidth ) + 1;
    }
  #endif

  #if EW_SURFACE_ROTATION == 180
    aSrc += (( aHeight - 1 ) * aWidth ) + ( aWidth - 1 );

    while ( aHeight-- > 0 )
    {
      int width = aWidth;

      while ( width-- > 0 )
        *dst++ = *aSrc--;

      dst += aDst->Pitch1Y - aWidth;
    }
  #endif

  #if EW_SURFACE_ROTATION == 270
    int width = aWidth;
    aSrc += aWidth - 1;

    while ( width-- > 0 )
    {
      int height = aHeight;

      while ( height-- > 0 )
      {
        *dst++ = *aSrc;
        aSrc  += aWidth;
      }

      dst  += aDst->Pitch1Y - aHeight;
      aSrc -= ( aHeight * aWidth ) + 1;
    }
  #endif
}


/* msy, pba */
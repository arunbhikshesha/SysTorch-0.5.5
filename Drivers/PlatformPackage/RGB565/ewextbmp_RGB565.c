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
*   This file contains the implementation of a bitmap resource provider intended
*   to load RGBA8888 compressed bitmap resources directly from the code memory
*   as they will be generated by Embedded Wizard resource converters.
*
*   For more details about this color format see: 'ewextpxl_RGB565.h'.
*
*******************************************************************************/

#include "ewrte.h"
#include "ewextbmp.h"
#include "ewgfxdriver.h"
#include "ewextgfx.h"
#include "ewgfxdefs.h"
#include "ewextpxl_RGB565.h"


/* Suppress MSVC++ compiler warnings caused by supposed negative bit-shift
   operations. It is o.k. */
#ifdef _MSC_VER 
  #pragma warning( disable : 4293 )
#endif


/* Does the RGBA channel order differ from the default RGBA8888 order? */
#if ( EW_COLOR_CHANNEL_BIT_OFFSET_RED   != 0  ) ||                             \
    ( EW_COLOR_CHANNEL_BIT_OFFSET_GREEN != 8  ) ||                             \
    ( EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  != 16 ) ||                             \
    ( EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA != 24 )
  #define EW_REORDER_COLOR_CHANNELS
#endif

/* Does the RGB channel order in the RGB565 color format differ from the 
   default RGB565 order? */
#if ( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_RED   != 0  ) ||                      \
    ( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_GREEN != 5  ) ||                      \
    ( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_BLUE  != 11 )
  #define EW_REORDER_RGB565_COLOR_CHANNELS
#endif

/* A constant value identifying uniquely the order of color channels */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define EW_COLOR_CHANNELS_MAGIC_COOKIE                                       \
    (( EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA << 18 ) |                             \
     ( EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  << 12 ) |                             \
     ( EW_COLOR_CHANNEL_BIT_OFFSET_GREEN << 6  ) |                             \
       EW_COLOR_CHANNEL_BIT_OFFSET_RED           | 0x80000000 )
#else
  #define EW_COLOR_CHANNELS_MAGIC_COOKIE                                       \
    (( EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA << 18 ) |                             \
     ( EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  << 12 ) |                             \
     ( EW_COLOR_CHANNEL_BIT_OFFSET_GREEN << 6  ) |                             \
       EW_COLOR_CHANNEL_BIT_OFFSET_RED )
#endif

/* A constant value identifying uniquely the order of RGB565 color channels */
#define EW_RGB565_COLOR_CHANNELS_MAGIC_COOKIE                                  \
   (( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_BLUE  << 8 ) |                        \
    ( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_GREEN << 4 ) |                        \
      EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_RED )


/* If the memory of constant surfaces is not accessible by the CPU, use a
   'Reader' to load and map the pixel pointers.  */
extern XFlashAreaReaderProc EwFlashAreaReaderProc;
extern void*                EwFlashAreaStartAddress;
extern void*                EwFlashAreaEndAddress;


/*******************************************************************************
* FUNCTION:
*   EwBmpInit
*
* DESCRIPTION:
*   The function EwBmpInit() is called during the initialization of the Graphics
*   Engine. Its intention is to give the external loader a chance to initialize
*   its private variables and to start the bitmap decoder functionality (if any)
*
* ARGUMENTS:
*   aArgs - User argument passed in the EwInitGraphicsEngine().
*
* RETURN VALUE:
*   If successful, returns != 0.
*
*******************************************************************************/
int EwBmpInit( void* aArgs )
{
  EW_UNUSED_ARG( aArgs );

  /* Plausibility test - color formats of the pixel driver and bitmap loader
     always have to match! This error can occur if wrong ewextpxl_XXX.c and
     ewextbmp_XXX.c files are linked together. For example, ewextpxl_RGBA8888.c
     can not work with ewextbmp_YUVA8888.c */
  if ( EwPixelDriverVariant != EW_DRIVER_VARIANT_RGB565_RGBA8888 )
  {
    EwError( 232 );
    EwPanic();
    return 0;
  }

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpDone
*
* DESCRIPTION:
*   The function EwBmpDone() will be called during the shutdown of the Graphics
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
void EwBmpDone( void )
{
}


/*******************************************************************************
* FUNCTION:
*   EwBmpOpen
*
* DESCRIPTION:
*   The function EwBmpOpen() will be called by the Graphics Engine in order to
*   initiate the access to the given bitmap resource. Depending on the manner,
*   the resources are handled in the particular platform system, the function
*   can open an image file or it can access bitmap information available in the
*   code memory.
*
*   The function should return a handle to the opened resource. By using this
*   handle, the bitmap content and its metrics can be requested by the Graphics
*   Engine without taking in account any platform specific aspects.
*
* ARGUMENTS:
*   aResource - Descriptor of the bitmap resource to open. The content of this
*     descriptor depends on the particular platform system.
*
* RETURN VALUE:
*   If sucessful, the function returns the handle to the opened bitmap resource.
*   If failed, the function returns null.
*
*******************************************************************************/
unsigned long EwBmpOpen( const struct XBmpRes* aResource )
{
  /* Verify, whether the binary resource does fit to this loader */
  if (( aResource->MagicNo != EW_MAGIC_NO_BITMAP      ) &&
      ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R90  ) &&
      ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R180 ) &&
      ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R270 ))
  {
    EwErrorS( 233, aResource->Name );
    EwPanic();
    return 0;
  }

  /* Verify whether the format of the bitmap resource does match with this
     loader. This error can occur when trying to link with code generated for
     other target system or color format. */
  if (( aResource->Format != EW_DRIVER_VARIANT_RGBA8888 ) &&
      ( aResource->Format != EW_DRIVER_VARIANT_ALPHA8   ) &&
      ( aResource->Format != EW_DRIVER_VARIANT_INDEX8   ) &&
      ( aResource->Format != EW_DRIVER_VARIANT_RGB565   ))
  {
    EwErrorS( 234, aResource->Name );
    EwPanic();
    return 0;
  }

  /* Verify whether the orientation of the bitmap does correspond to the used
     in the target display configuration. This error can occur when trying to
     link code generated for other orientation (e.g. 90 degree rotated) with
     target configured to e.g. 270 degree rotated display. The orientations 
     have to match. */
  #if EW_SURFACE_ROTATION == 90
    if ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R90 )
  #elif EW_SURFACE_ROTATION == 180
    if ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R180 )
  #elif EW_SURFACE_ROTATION == 270
    if ( aResource->MagicNo != EW_MAGIC_NO_BITMAP_R270 )
  #else
    if ( aResource->MagicNo != EW_MAGIC_NO_BITMAP )
  #endif
  {
    EwErrorS( 235, aResource->Name );
    EwPanic();
    return 0;
  }

  /* Bitmap resources generated for 'direct access' need to be validated to
     ensure that they do store the pixel information in the color format of
     this Graphics Engine. */
  if ( !aResource->Compressed && 
     ( aResource->Format == EW_DRIVER_VARIANT_RGBA8888 ))
  {
    const unsigned int* pixel = (const unsigned int*)aResource->Pixel1;

    /* If the memory of constant surfaces is not accessible by the CPU, use a
       'Reader' to load and map the pixel pointers.  */
    #ifdef EW_USE_READER_FOR_CONST_SURFACES
      pixel = (const unsigned int*)EwFlashAreaReaderProc( pixel );
    #endif

    if ( pixel[0] != EW_COLOR_CHANNELS_MAGIC_COOKIE )
    {
      EwErrorS( 236, aResource->Name );
      EwPanic();
      return 0;
    }
  }

  /* Bitmap resources generated for 'direct access' need to be validated to
     ensure that they do store the pixel information in the color format of
     this Graphics Engine. */
  if ( !aResource->Compressed && 
     ( aResource->Format == EW_DRIVER_VARIANT_INDEX8 ))
  {
    const unsigned int* clut = (const unsigned int*)aResource->Clut;

    /* If the memory of constant surfaces is not accessible by the CPU, use a
       'Reader' to load and map the clut pointer.  */
    #ifdef EW_USE_READER_FOR_CONST_SURFACES
      clut = (const unsigned int*)EwFlashAreaReaderProc( clut );
    #endif

    if ( clut[0] != EW_COLOR_CHANNELS_MAGIC_COOKIE )
    {
      EwErrorS( 237, aResource->Name );
      EwPanic();
      return 0;
    }
  }

  /* Bitmap resources generated for 'direct access' need to be validated to
     ensure that they do store the pixel information in the color format of
     this Graphics Engine. */
  if ( !aResource->Compressed && 
     ( aResource->Format == EW_DRIVER_VARIANT_RGB565 ))
  {
    const unsigned short* pixel = (const unsigned short*)aResource->Pixel1;

    /* If the memory of constant surfaces is not accessible by the CPU, use a
       'Reader' to load and map the pixel pointers.  */
    #ifdef EW_USE_READER_FOR_CONST_SURFACES
      pixel = (const unsigned short*)EwFlashAreaReaderProc( pixel );
    #endif

    if ( pixel[0] != EW_RGB565_COLOR_CHANNELS_MAGIC_COOKIE )
    {
      EwErrorS( 238, aResource->Name );
      EwPanic();
      return 0;
    }
  }

  /* This interface works with the original data structures as it was stored in
     the code memory */
  return (unsigned long)aResource;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpClose
*
* DESCRIPTION:
*   The function EwBmpClose() will be called by the Graphics Engine to finish
*   the processing of a bitmap resource. Depending on the manner, the resources
*   are handled in the particular platform system, the function can close the
*   previously opened file and free any temporarily reserved resources.
*
* ARGUMENTS:
*   aHandle - Handle to the bitmap resource to terminate the access.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBmpClose( unsigned long aHandle )
{
  EW_UNUSED_ARG( aHandle );
}


/*******************************************************************************
* FUNCTION:
*   EwBmpGetMetrics
*
* DESCRIPTION:
*   The function EwBmpGetMetrics() will be called by the Graphics Engine to get
*   the general attributes of the bitmap resource. By using of these attributes,
*   Graphics Engine can create and initialize video surfaces.
*
* ARGUMENTS:
*   aHandle         - Handle to the bitmap resource to determine its attributes.
*   aFormat         - Pointer to a variable, where the pixel format of the bitmap
*     resource will returned (See EW_PIXEL_FORMAT_XXX).
*   aNoOfFrames     - Pointer to a variable, where the number of frames included
*     within this bitmap resource will returned.
*   aNoOfVirtFrames - Pointer to a variable, where the number of all frames incl.
*     duplicates will be returned. This value can be greater than aNoOfFrames if
*     some of the frames do repeat in the bitmap.
*   aFrameWidth,
*   aFrameHeight    - Pointers to variables, where the size in pixel of a single
*     frame will be returned.
*   aFrameDelay     - Pointer to a variable, where the delay in milliseconds for
*     animated bitmap resources will be returned.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwBmpGetMetrics( unsigned long aHandle, int* aFormat, int* aNoOfFrames,
  int* aNoOfVirtFrames, int* aFrameWidth, int* aFrameHeight, int* aFrameDelay )
{
  const XBmpRes*        res     = (const XBmpRes*)aHandle;
  const unsigned short* mapping = res->Mapping;
  int                   count   = 0;

  /* What kind of bitmap format is stored in the resource? */
  if ( res->Format == EW_DRIVER_VARIANT_RGBA8888 )
    *aFormat = EW_PIXEL_FORMAT_NATIVE;
  if ( res->Format == EW_DRIVER_VARIANT_RGB565 )
    *aFormat = EW_PIXEL_FORMAT_RGB565;
  if ( res->Format == EW_DRIVER_VARIANT_ALPHA8 )
    *aFormat = EW_PIXEL_FORMAT_ALPHA8;
  if ( res->Format == EW_DRIVER_VARIANT_INDEX8 )
    *aFormat = EW_PIXEL_FORMAT_INDEX8;

  /* Count the frame numbers in the mapping table */
  for ( ; *mapping != 0xFFFF; mapping++, count++ )
    ;

  *aNoOfVirtFrames = count? count : res->NoOfFrames;
  *aNoOfFrames     = res->NoOfFrames;
  *aFrameWidth     = res->FrameWidth;
  *aFrameHeight    = res->FrameHeight;
  *aFrameDelay     = res->FrameDelay;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpGetFrameMetrics
*
* DESCRIPTION:
*   The function EwBmpGetFrameMetrics() will be called by the Graphics Engine in
*   order to obtain attributes for a single bitmap resource frame.
*
* ARGUMENTS:
*   aHandle      - Handle to the bitmap resource to determine its attributes.
*   aFrameNo     - Number of the desired frame to get its attributes.
*   aOpqX, aOpqY,
*   aOpqWidth,
*   aOpqHeight   - Pointers to variables, where an optional full-opaque area of
*     the frame will be returned. By using this area, drawing operation can be
*     optimized. The returned origin lies relative to the top-left corner of the
*     frame.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwBmpGetFrameMetrics( unsigned long aHandle, int aFrameNo, int* aOpqX,
  int* aOpqY, int* aOpqWidth, int* aOpqHeight )
{
  const XBmpRes*      res   = (const XBmpRes*)aHandle;
  const XBmpFrameRes* frame = res->Frames + aFrameNo;

  /* Invalid frame number */
  if (( aFrameNo < 0 ) || ( aFrameNo >= res->NoOfFrames ))
    return 0;

  *aOpqX      = frame->OpqX;
  *aOpqY      = frame->OpqY;
  *aOpqWidth  = frame->OpqWidth;
  *aOpqHeight = frame->OpqHeight;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpGetFrameMemory
*
* DESCRIPTION:
*   The function EwBmpGetFrameMemory() will be called by the Graphics Engine in
*   order to verify whether it is possible to directly access the frame pixel
*   data of the bitmap resource represented by aHandle and in the positive case
*   to obtain the affected memory address pointers.
*
*   This function belongs to the interface separating the Graphics Engine from
*   the platform specific bitmap resource loader functionality. In the case the
*   target system stores the bitmap resources already in a usable format, the
*   function allows the Graphics Engine to use it directly without to need any
*   additional surface memory to be allocated and pixel contents to be copied.
*
*   This approach will not work when the bitmap resources are stored compressed
*   or loaded dynamically from e.g. file system. To work it is essential, that
*   the delivered pixel memory address is persistently valid (e.g. resides in
*   ROM code) and the pixel in the memory are stored in the format used by the
*   surfaces in the target system.
*
*   The function should return 0 if the direct access is not supported.
*
* ARGUMENTS:
*   aHandle  - Handle to the bitmap resource to obtain direct pixel memory
*     access.
*   aFrameNo - Number of the desired frame to get its pixel data.
*   aMemory  - Structure to receive the desired address information.
*
* RETURN VALUE:
*   If successful, the function fills the given aMemory structure with the
*   address information and returns != 0. Otherwise 0 is returned.
*
*******************************************************************************/
int EwBmpGetFrameMemory( unsigned long aHandle, int aFrameNo, 
  XSurfaceMemory* aMemory )
{
  const XBmpRes*       res    = (const XBmpRes*)aHandle;
  const XBmpFrameRes*  frame = res->Frames + aFrameNo;
  const unsigned char* pixel = (const unsigned char*)res->Pixel1;
  int                  width = res->FrameWidth;

  /* Invalid frame number */
  if (( aFrameNo < 0 ) || ( aFrameNo >= res->NoOfFrames ))
    return 0;

  /* The bitmap resource doesn't provide any 'direct access' content. */
  if ( res->Compressed )
    return 0;

  /* The surface memory is rotated by 90 degrees? Exchange X/Y coordinates */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    width = res->FrameHeight;
  #endif

  /* Just returns the pointer to the pixel data of an Alpha8 bitmap. Note in
     EmWi all frames have a 1px border transparent for antialiasing when 
     images are scaled or 3D projected. The returned pointer has to refer to
     the top-left corner of such border. The additional border also affects 
     the pixel memory pitch. */
  if ( res->Format == EW_DRIVER_VARIANT_ALPHA8 )
  {
    aMemory->Pixel1  = (void*)( pixel + frame->Pixel1 );
    aMemory->Pitch1Y = width + 2; /* always + 1px border left/right */
    aMemory->Pitch1X = 1;
    aMemory->Pixel2  = 0;
    aMemory->Pitch2Y = 0;
    aMemory->Pitch2X = 0;
    aMemory->Clut    = 0;
  }

  /* ... the same for the RGB565 format. */
  else if ( res->Format == EW_DRIVER_VARIANT_RGB565 )
  {
    aMemory->Pixel1  = (void*)( pixel + 2 + frame->Pixel1 );
    aMemory->Pitch1Y = ( width + 2 ) * sizeof( short );
    aMemory->Pitch1X = sizeof( short );
    aMemory->Pixel2  = 0;
    aMemory->Pitch2Y = 0;
    aMemory->Pitch2X = 0;
    aMemory->Clut    = 0;
  }

  /* ... the same for the Index8 format. */
  else if ( res->Format == EW_DRIVER_VARIANT_INDEX8 )
  {
    aMemory->Pixel1  = (void*)( pixel + frame->Pixel1 );
    aMemory->Pitch1Y = width + 2; /* always + 1px border left/right */
    aMemory->Pitch1X = 1;
    aMemory->Pixel2  = 0;
    aMemory->Pitch2Y = 0;
    aMemory->Pitch2X = 0;
    aMemory->Clut    = (unsigned int*)res->Clut + 1;
  }

  /* ... the same for the native RGBA8888 format. */
  else
  {
    aMemory->Pixel1  = (void*)( pixel + 4 + frame->Pixel1 );
    aMemory->Pitch1Y = ( width + 2 ) * sizeof( int );
    aMemory->Pitch1X = sizeof( int );
    aMemory->Pixel2  = 0;
    aMemory->Pitch2Y = 0;
    aMemory->Pitch2X = 0;
    aMemory->Clut    = 0;
  }

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpLoadFrame
*
* DESCRIPTION:
*   The function EwBmpLoadFrame() will be called by the Graphics Engine in order
*   to load the frame pixel data from the openend bitmap resource aHandle into
*   the given memory area aMemory. Depending on the manner, the resources are
*   handled in the particular platform, the pixel data may be loaded from a file
*   or they can be decompressed from the code memory.
*
* ARGUMENTS:
*   aHandle  - Handle to the bitmap resource to load the pixel data.
*   aFrameNo - Number of the desired frame to get its pixel data.
*   aMemory  - Memory descriptor containing pointers and pitch information of
*     the destination to write the pixel data.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwBmpLoadFrame( unsigned long aHandle, int aFrameNo,
  XSurfaceMemory* aMemory )
{
  const XBmpRes*       res    = (const XBmpRes*)aHandle;
  const XBmpFrameRes*  frame  = res->Frames + aFrameNo;
  const unsigned char* pixel  = (const unsigned char*)res->Pixel1;
  int                  width  = res->FrameWidth;
  int                  height = res->FrameHeight;

  /* Invalid frame number */
  if (( aFrameNo < 0 ) || ( aFrameNo >= res->NoOfFrames ))
    return 0;

  /* The bitmap resource doesn't provide any 'compressed' content. */
  if ( !res->Compressed )
    return 0;

  /* The surface memory is rotated by 90 degrees? Exchange X/Y coordinates */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    width  = res->FrameHeight;
    height = res->FrameWidth;
  #endif

  /* Just decompress the pixel data. Note the generic Alpha8 and Index8 
     bitmap formats! */
  if (( res->Format == EW_DRIVER_VARIANT_ALPHA8 ) || 
      ( res->Format == EW_DRIVER_VARIANT_INDEX8 ))
    EwDecompress((const unsigned int*)( pixel + frame->Pixel1 ),
      (unsigned char*)aMemory->Pixel1, width, aMemory->Pitch1Y );

  /* Decompress RGB565 format */
  else if ( res->Format == EW_DRIVER_VARIANT_RGB565 )
  {
    unsigned short* dest = (unsigned short*)aMemory->Pixel1;
    int             ofs  = aMemory->Pitch1Y >> 1;
    int             y;

    EwDecompress((const unsigned int*)( pixel + frame->Pixel1 ),
      (unsigned char*)aMemory->Pixel1, width * 2, aMemory->Pitch1Y );

    /* Adapt the byte order to the target CPU endian architecture */
    for ( y = 0; y < height; y++, dest += ofs )
      EwAdaptByteOrder2( dest, width );
  }

  /* Decompress RGBA8888 */
  else
  {
    unsigned int* dest = (unsigned int*)aMemory->Pixel1;
    int           ofs  = aMemory->Pitch1Y >> 2;
    int           y;

    EwDecompress((const unsigned int*)( pixel + frame->Pixel1 ),
      (unsigned char*)aMemory->Pixel1, width * 4, aMemory->Pitch1Y );

    /* Adapt the byte order to the target CPU endian architecture */
    for ( y = 0; y < height; y++, dest += ofs )
      EwAdaptByteOrder4( dest, width );
  }

  /* Post-process the decompressed RGB565 pixel information. If necessary 
     adapt the order of the channels */
  #if defined EW_REORDER_RGB565_COLOR_CHANNELS
    if ( res->Format == EW_DRIVER_VARIANT_RGB565 )
    {
      unsigned short* dest = (unsigned short*)aMemory->Pixel1;
      int             ofs  = ( aMemory->Pitch1Y >> 1 ) - width;
      int             x, y;

      /* Evaluate all decompressed bitmap pixel */
      for ( y = 0; y < height; y++, dest += ofs )
        for ( x = 0; x < width; x++, dest++ )
        {
          unsigned int c  = *dest;
          unsigned int c0 = c & 0x001F;
          unsigned int c1 = c & 0x07E0;
          unsigned int c2 = c & 0xF800;

          /* Change the color channel order */
          c0 = EW_SHIFT( c0, 0  - EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_RED   );
          c1 = EW_SHIFT( c1, 5  - EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_GREEN );
          c2 = EW_SHIFT( c2, 11 - EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_BLUE  );

          /* Compose the resulting pixel from the four channels */
          *dest = (unsigned short)( c0 | c1 | c2 );
        }
    }
  #endif

  /* Post-process the decompressed RGBA8888 pixel information. If necessary 
     pre-multiply the red, green and blue channels by the alpha-value and 
     adapt the order of the channels */
  #if defined EW_PREMULTIPLY_COLOR_CHANNELS || defined EW_REORDER_COLOR_CHANNELS
    if ( res->Format == EW_DRIVER_VARIANT_RGBA8888 )
    {
      unsigned int* dest = (unsigned int*)aMemory->Pixel1;
      int           ofs  = ( aMemory->Pitch1Y >> 2 ) - width;
      int           x, y;

      /* Evaluate all decompressed bitmap pixel */
      for ( y = 0; y < height; y++, dest += ofs )
        for ( x = 0; x < width; x++, dest++ )
        {
          unsigned int c  = *dest;
          unsigned int c0 = c & 0x000000FF;
          unsigned int c1 = c & 0x0000FF00;
          unsigned int c2 = c & 0x00FF0000;
          unsigned int ca = c & 0xFF000000;

          /* Pre-multiplication of red, gree, blue channels */
          #ifdef EW_PREMULTIPLY_COLOR_CHANNELS
            unsigned int a = ( ca >> 24 ) + 1;
            c0 = (( c0 * a ) >> 8 ) & 0x000000FF;
            c1 = (( c1 * a ) >> 8 ) & 0x0000FF00;
            c2 = (( c2 * a ) >> 8 ) & 0x00FF0000;
          #endif

          /* Change the color channel order */
          #ifdef EW_REORDER_COLOR_CHANNELS
            c0 = EW_SHIFT( c0, 0  - EW_COLOR_CHANNEL_BIT_OFFSET_RED   );
            c1 = EW_SHIFT( c1, 8  - EW_COLOR_CHANNEL_BIT_OFFSET_GREEN );
            c2 = EW_SHIFT( c2, 16 - EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  );
            ca = EW_SHIFT( ca, 24 - EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );
          #endif

          /* Compose the resulting pixel from the four channels */
          *dest = c0 | c1 | c2 | ca;
        }
    }
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpLoadClut
*
* DESCRIPTION:
*   The function EwBmpLoadClut() will be called by the Graphics Engine in order
*   to load the Clut data from the openend bitmap resource aHandle into the 
*   given memory area aMemory.
*
* ARGUMENTS:
*   aHandle  - Handle to the bitmap resource to load the CLUT data.
*   aMemory  - Memory descriptor containing pointers to the CLUT where to copy
*     the CLUT data.
*
* RETURN VALUE:
*   If sucessful, the function returns != 0.
*
*******************************************************************************/
int EwBmpLoadClut( unsigned long aHandle, XSurfaceMemory* aMemory )
{
  const XBmpRes*      res  = (const XBmpRes*)aHandle;
  const unsigned int* clut = (const unsigned int*)res->Clut;

  /* The bitmap resource doesn't provide any 'compressed' content. */
  if ( !res->Compressed || ( res->Format != EW_DRIVER_VARIANT_INDEX8 ))
    return 0;

  /* Before copying the CLUT entries zero the memory */
  EwZero( aMemory->Clut, 256 * sizeof( int ));

  /* Can the CPU access the CLUT resource data directly? Then simply copy the
     clut entries at once */
  if ( !EwFlashAreaReaderProc || ((const void*)clut < EwFlashAreaStartAddress ) ||
     ((const void*)clut > EwFlashAreaEndAddress ))
    EwCopy( aMemory->Clut, clut + 1, clut[0] * sizeof( int ));

  /* ... otherwise map the pointer to the clut entry before copying it */
  else
  {
    int count = *(const int*)EwFlashAreaReaderProc( clut );
    int i;

    /* The entries are copied individually */
    for ( i = 0, clut++; i < count; i++ )
      aMemory->Clut[i] = *(const unsigned int*)EwFlashAreaReaderProc( clut + i );
  }

  /* Post-process the decompressed CLUT information. If necessary pre-multiply
     the red, green and blue channels by the alpha-value and adapt the order of
     the channels */
  #if defined EW_PREMULTIPLY_COLOR_CHANNELS || defined EW_REORDER_COLOR_CHANNELS
  {
    unsigned int* dest = (unsigned int*)aMemory->Clut;
    int           i;

    /* Evaluate all CLUT entries */
    for ( i = 0; i < 256; i++, dest++ )
    {
      unsigned int c  = *dest;
      unsigned int c0 = c & 0x000000FF;
      unsigned int c1 = c & 0x0000FF00;
      unsigned int c2 = c & 0x00FF0000;
      unsigned int ca = c & 0xFF000000;

      /* Pre-multiplication of red, gree, blue channels */
      #ifdef EW_PREMULTIPLY_COLOR_CHANNELS
        unsigned int a = ( ca >> 24 ) + 1;
        c0 = (( c0 * a ) >> 8 ) & 0x000000FF;
        c1 = (( c1 * a ) >> 8 ) & 0x0000FF00;
        c2 = (( c2 * a ) >> 8 ) & 0x00FF0000;
      #endif

      /* Change the color channel order */
      #ifdef EW_REORDER_COLOR_CHANNELS
        c0 = EW_SHIFT( c0, 0  - EW_COLOR_CHANNEL_BIT_OFFSET_RED   );
        c1 = EW_SHIFT( c1, 8  - EW_COLOR_CHANNEL_BIT_OFFSET_GREEN );
        c2 = EW_SHIFT( c2, 16 - EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  );
        ca = EW_SHIFT( ca, 24 - EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );
      #endif

      /* Compose the resulting CLUT entry from the four channels */
      *dest = c0 | c1 | c2 | ca;
    }
  }
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwBmpLoadMappingTable
*
* DESCRIPTION:
*   The function EwBmpLoadMappingTable() will be called by the Graphics Engine
*   in order to load a table used to map between virtual and real frame numbers.
*   If a bitmap contains repetitions of a frame, the corresponding frames are
*   stored only once (as real frame). The numbers of original frames need to be
*   mapped to the numbers of the real (existing) frames.
*
* ARGUMENTS:
*   aHandle  - Handle to the bitmap resource to load the table.
*   aMapping - Pointer to memory where to copy the table. The memory area has
*     to be large enough.
*
* RETURN VALUE:
*   If sucessful, the function returns the number of virtual frames (the number
*   of copied entries).
*
*******************************************************************************/
int EwBmpLoadMappingTable( unsigned long aHandle, unsigned short* aMapping )
{
  const XBmpRes*        res     = (const XBmpRes*)aHandle;
  const unsigned short* mapping = res->Mapping;

  while ( *mapping != 0xFFFF )
    *aMapping++ = *mapping++;

  return (int)( mapping - res->Mapping );
}


/* pba */

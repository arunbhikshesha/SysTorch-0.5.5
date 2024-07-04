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
*   This module implements the low level software driver for target platforms
*   based on the RGB565 screen and RGBA8888 native pixel format whereby the
*   screen pixel format does correspond exclusively to the format of the frame
*   buffer.
*
*   This implementation includes following special options:
*
*   > Pre-multiplied or non pre-multiplied pixel RGB color channels. Depending
*     on the graphics hardware, it may be expected that RGB color channels are
*     stored and calculated as already alpha pre-multiplied values.
*
*     This option is controlled by the definition EW_PREMULTIPLY_COLOR_CHANNELS.
*     If this macro is defined, the pixel driver handles the RGB color channels
*     as already pre-multiplied by the corresponding pixel alpha value.
*
*     To select the pre-multiplied color format take following line into your
*     'ewextgfx.h' file:
*
*       #define EW_PREMULTIPLY_COLOR_CHANNELS 1
*
*     If your graphics hardware doesn't work with pre-multiplied color channels
*     set the following line into your 'ewextgfx.h' file:
*
*       #define EW_PREMULTIPLY_COLOR_CHANNELS 0
*
*     Please note, this option affects the native pixel format RGBA8888 only.
*     The screen pixel format does not store any alpha information.
*
*   > Custom specific color channel order of the native pixel format. Depending
*     on the graphics hardware, it may be necessary to adapt the order in which
*     the color channels are stored in the video memory of a native surface.
*
*     The format name 'RGBA8888' refers to the generic 32 bit color format
*     where the color and alpha channels are stored within a 32 bit value
*     in the following manner:
*
*              31           24           16             8            0
*               +------------+---------------------------------------+
*               |   alpha    |    blue    |    green    |     red    |
*               +------------+---------------------------------------+
*
*
*     To control the order of channels, specify the bit start position of each
*     channel. For this purpose the macros EW_COLOR_CHANNEL_BIT_OFFSET_XXX are
*     available (XXX stands for RED, GREEN, BLUE and ALPHA). For example to
*     adapt this pixel driver to graphics hardware using BGRA8888 color format,
*     add following lines to your 'ewextgfx.h' file:
*
*       #define EW_COLOR_CHANNEL_BIT_OFFSET_RED    16
*       #define EW_COLOR_CHANNEL_BIT_OFFSET_GREEN   8
*       #define EW_COLOR_CHANNEL_BIT_OFFSET_BLUE    0
*       #define EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA  24
*
*     Please note, these options affect the native pixel format RGBA8888 only.
*
*   > Custom specific color channel order of the screen pixel format. Depending
*     on the graphics hardware, it may be necessary to adapt the order in which
*     the color channels are stored in the video memory of a screen surface.
*
*     The format name 'RGB565' refers to the generic 16 bit color format where
*     the color channels are stored within a 16 bit value in the following
*     manner:
*
*              15           11             5            0
*               +---------------------------------------+
*               |    blue    |    green    |     red    |
*               +---------------------------------------+
*
*
*     Note, the green information always occupies the middle 6 bit wide channel.
*     This can not be changed.
*
*     To control the order of channels, specify the bit start position of each
*     channel. For this purpose the macros EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_XXX
*     are available (XXX stands for RED and BLUE - green channel position is
*     fixed as mentioned above). For example to adapt this pixel driver to
*     graphics hardware using BGR565 color format, add following lines to your
*     'ewextgfx.h' file:
*
*       #define EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_RED   11
*       #define EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_BLUE  0
*
*     Please note, these options affect the screen pixel format RGB565 only.
*
*   > The RGB565 bitmap source format configuration does correspond to the of
*     the SCREEN format as described above.
*
*   This driver is intended for running drawing operations needed by Embedded
*   Wizard. For optimization purpose all respective drawing modes are realized
*   as separate drawing functions and they are limited to a single pixel row.
*
*******************************************************************************/

#include "ewrte.h"
#include "ewgfxdriver.h"
#include "ewextgfx.h"
#include "ewgfxdefs.h"
#include "ewextpxl_RGB565.h"


/* Memory usage profiler */
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwMemoryPeak;


/* Suppress MSVC++ compiler warnings caused by supposed negative bit-shift
   operations. It is o.k. */
#ifdef _MSC_VER 
  #pragma warning( disable : 4293 )
#endif


/* This global variable stores the target basic color format supported by this
   pixel driver. It is used at the runtime to verify, whether correct pixel
   driver are linked together with the application. */
const int EwPixelDriverVariant = EW_DRIVER_VARIANT_RGB565_RGBA8888;


/* How to extract the alpha value - depending on the color channel order and the
   operands where alpha is stored */
#if EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA == 24
  #define CHANNEL_ALPHA( aColor )                                              \
    (( aColor ) >> 24 )
#else
  #define CHANNEL_ALPHA( aColor )                                              \
    ((( aColor ) >> EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA ) & 0xFF )
#endif


/* Convert the RED channel from the native to the scren pixel format */
#define NATIVE_2_SCREEN_RED( aSrc )                                            \
  ((( aSrc >> ( EW_COLOR_CHANNEL_BIT_OFFSET_RED + 3 )) & mask1F ) <<           \
     EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_RED )


/* Convert the GREEN channel from the native to the scren pixel format */
#define NATIVE_2_SCREEN_GREEN( aSrc )                                          \
  ((( aSrc >> ( EW_COLOR_CHANNEL_BIT_OFFSET_GREEN + 2 )) & mask3F ) <<         \
     EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_GREEN )


/* Convert the BLUE channel from the native to the scren pixel format */
#define NATIVE_2_SCREEN_BLUE( aSrc )                                           \
  ((( aSrc >> ( EW_COLOR_CHANNEL_BIT_OFFSET_BLUE + 3 )) & mask1F ) <<          \
     EW_SCREEN_COLOR_CHANNEL_BIT_OFFSET_BLUE )


/* The following macros extract from the given RGB565 value the desired red, 
   green or blue component and return it as an 8-bit value with the lower 2
   or 3 bits being cleared 0. */
#define GET_RGB565_RED( aSrc )                                                 \
  ( EW_SHIFT( aSrc, EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_RED - 3 ) & 0xF8 )

#define GET_RGB565_GREEN( aSrc )                                               \
  ((( aSrc ) >> ( EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_GREEN - 2 )) & 0xFC )

#define GET_RGB565_BLUE( aSrc )                                                \
  ( EW_SHIFT( aSrc, EW_RGB565_COLOR_CHANNEL_BIT_OFFSET_BLUE - 3 ) & 0xF8 )


/* If the memory of constant surfaces is not accessible by the CPU, use a
   'Reader' to load and map the pixel pointers.  */
#ifdef EW_USE_READER_FOR_CONST_SURFACES 
  extern XFlashAreaReaderProc EwFlashAreaReaderProc;

  #define MAP_UINT8( aPtr )                                                    \
   (( aSrc->Reader )? (unsigned char*)aSrc->Reader( aPtr ) : ( aPtr ))
  #define MAP_UINT16( aPtr )                                                   \
   (( aSrc->Reader )? (unsigned short*)aSrc->Reader( aPtr ) : ( aPtr ))
  #define MAP_UINT32( aPtr )                                                   \
   (( aSrc->Reader )? (unsigned int*)aSrc->Reader( aPtr ) : ( aPtr ))
#else
  #define MAP_UINT8( aPtr )  (aPtr)
  #define MAP_UINT16( aPtr ) (aPtr)
  #define MAP_UINT32( aPtr ) (aPtr)
#endif


/* The following macro converts the given RGB565 color value in the RGBA8888
   native format by taking in account all color channel orders. The Alpha
   component in the resulting value is 0xFF (opaque). */
#define RGB565_2_NATIVE_OPAQUE( aDst, aSrc )                                   \
  {                                                                            \
    unsigned int _srcR = GET_RGB565_RED( aSrc );                               \
    unsigned int _srcG = GET_RGB565_GREEN( aSrc );                             \
    unsigned int _srcB = GET_RGB565_BLUE( aSrc );                              \
                                                                               \
    /* Compose the channels for the RGBA8888 format */                         \
    aDst = (( _srcR | ( _srcR >> 5 )) << EW_COLOR_CHANNEL_BIT_OFFSET_RED   ) | \
           (( _srcG | ( _srcG >> 6 )) << EW_COLOR_CHANNEL_BIT_OFFSET_GREEN ) | \
           (( _srcB | ( _srcB >> 5 )) << EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  ) | \
           ((unsigned int)0xFF        << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );  \
  }


/* The following macro converts the given RGB565 color value in the RGBA8888
   native format by taking in account all color channel orders. The Alpha
   component in the resulting value is 0x00 (transparent). */
#define RGB565_2_NATIVE_TRANSPARENT( aDst, aSrc )                              \
  {                                                                            \
    unsigned int _srcR = GET_RGB565_RED( aSrc );                               \
    unsigned int _srcG = GET_RGB565_GREEN( aSrc );                             \
    unsigned int _srcB = GET_RGB565_BLUE( aSrc );                              \
                                                                               \
    /* Compose the channels for the RGBA8888 format */                         \
    aDst = (( _srcR | ( _srcR >> 5 )) << EW_COLOR_CHANNEL_BIT_OFFSET_RED   ) | \
           (( _srcG | ( _srcG >> 6 )) << EW_COLOR_CHANNEL_BIT_OFFSET_GREEN ) | \
           (( _srcB | ( _srcB >> 5 )) << EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  );  \
  }


/* The following constants are used to replace slow divisions by mult-ops
   with the reciprocal values. */
#ifndef EW_PREMULTIPLY_COLOR_CHANNELS
  const static unsigned short InvAlpha[] =
  {
    0x0000, 0xFF00, 0x7F80, 0x5500, 0x3FC0, 0x3300, 0x2A80, 0x246E, 0x1FE0,
    0x1C56, 0x1980, 0x172F, 0x1540, 0x139E, 0x1237, 0x1100, 0x0FF0, 0x0F00,
    0x0E2B, 0x0D6C, 0x0CC0, 0x0C25, 0x0B98, 0x0B17, 0x0AA0, 0x0A34, 0x09CF,
    0x0972, 0x091C, 0x08CC, 0x0880, 0x083A, 0x07F8, 0x07BB, 0x0780, 0x074A,
    0x0716, 0x06E5, 0x06B6, 0x068A, 0x0660, 0x0639, 0x0613, 0x05EF, 0x05CC,
    0x05AB, 0x058C, 0x056D, 0x0550, 0x0535, 0x051A, 0x0500, 0x04E8, 0x04D0,
    0x04B9, 0x04A3, 0x048E, 0x047A, 0x0466, 0x0453, 0x0440, 0x042F, 0x041D,
    0x040D, 0x03FC, 0x03ED, 0x03DE, 0x03CF, 0x03C0, 0x03B3, 0x03A5, 0x0398,
    0x038B, 0x037F, 0x0373, 0x0367, 0x035B, 0x0350, 0x0345, 0x033B, 0x0330,
    0x0326, 0x031D, 0x0313, 0x030A, 0x0300, 0x02F8, 0x02EF, 0x02E6, 0x02DE,
    0x02D6, 0x02CE, 0x02C6, 0x02BE, 0x02B7, 0x02B0, 0x02A8, 0x02A1, 0x029B,
    0x0294, 0x028D, 0x0287, 0x0280, 0x027A, 0x0274, 0x026E, 0x0268, 0x0263,
    0x025D, 0x0257, 0x0252, 0x024D, 0x0247, 0x0242, 0x023D, 0x0238, 0x0233,
    0x022E, 0x022A, 0x0225, 0x0220, 0x021C, 0x0218, 0x0213, 0x020F, 0x020B,
    0x0207, 0x0203, 0x01FE, 0x01FB, 0x01F7, 0x01F3, 0x01EF, 0x01EB, 0x01E8,
    0x01E4, 0x01E0, 0x01DD, 0x01DA, 0x01D6, 0x01D3, 0x01CF, 0x01CC, 0x01C9,
    0x01C6, 0x01C3, 0x01C0, 0x01BD, 0x01BA, 0x01B7, 0x01B4, 0x01B1, 0x01AE,
    0x01AB, 0x01A8, 0x01A6, 0x01A3, 0x01A0, 0x019E, 0x019B, 0x0198, 0x0196,
    0x0193, 0x0191, 0x018F, 0x018C, 0x018A, 0x0187, 0x0185, 0x0183, 0x0180,
    0x017E, 0x017C, 0x017A, 0x0178, 0x0176, 0x0173, 0x0171, 0x016F, 0x016D,
    0x016B, 0x0169, 0x0167, 0x0165, 0x0163, 0x0161, 0x015F, 0x015E, 0x015C,
    0x015A, 0x0158, 0x0156, 0x0154, 0x0153, 0x0151, 0x014F, 0x014E, 0x014C,
    0x014A, 0x0149, 0x0147, 0x0145, 0x0144, 0x0142, 0x0140, 0x013F, 0x013D,
    0x013C, 0x013A, 0x0139, 0x0137, 0x0136, 0x0134, 0x0133, 0x0132, 0x0130,
    0x012F, 0x012D, 0x012C, 0x012B, 0x0129, 0x0128, 0x0127, 0x0125, 0x0124,
    0x0123, 0x0121, 0x0120, 0x011F, 0x011E, 0x011C, 0x011B, 0x011A, 0x0119,
    0x0117, 0x0116, 0x0115, 0x0114, 0x0113, 0x0112, 0x0110, 0x010F, 0x010E,
    0x010D, 0x010C, 0x010B, 0x010A, 0x0109, 0x0108, 0x0107, 0x0106, 0x0105,
    0x0104, 0x0103, 0x0102, 0x0100
  };
#endif


/* The following macro provides a code fragment for the copy of a native color 
   value into a screen color value. The expressions aSrc result in the native
   color value. aDst refers to destination variable, where the operation result
   should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_COPY_NATIVE( aDst, aSrc )                                     \
    {                                                                          \
      unsigned int _src = aSrc;                                                \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _src )                                                              \
        _src = NATIVE_2_SCREEN_RED  ( _src ) | NATIVE_2_SCREEN_BLUE ( _src ) | \
               NATIVE_2_SCREEN_GREEN( _src );                                  \
                                                                               \
      /* Store the results */                                                  \
      aDst = (unsigned short)( _src );                                         \
    }
#else
  #define SCREEN_COPY_NATIVE( aDst, aSrc )                                     \
    {                                                                          \
      unsigned int _src  = aSrc;                                               \
      unsigned int _srcA = CHANNEL_ALPHA( _src ) >> 2;                         \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( _src ) |                 \
                                NATIVE_2_SCREEN_BLUE ( _src );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( _src );                  \
                                                                               \
        /* Modulate the source with opacity and the alpha of the original */   \
        _srcC13 *= ++_srcA;                                                    \
        _srcC24 *=   _srcA;                                                    \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)((( _srcC13 >> 6 ) & mask2L ) |                 \
                                (( _srcC24 >> 6 ) & mask2H ));                 \
      }                                                                        \
                                                                               \
      /* RGB565 has no alpha channel - transparent is black */                 \
      else                                                                     \
        aDst = 0;                                                              \
    }
#endif


/* The following macro provides a code fragment for the modulation of a native 
   color value by an opacity value. The expressions aSrc and aOpacity result in
   the native color value and the opacity value to modulate it. aDst refers to
   destination variable, where the operation result should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define COPY_NATIVE_OPACITY( aDst, aSrc, aOpacity )                          \
    {                                                                          \
      /* Anything to do? */                                                    \
      if ( aSrc )                                                              \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 =   aSrc        & mask2;                          \
        unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                          \
                                                                               \
        /* Modulate the source with opacity */                                 \
        _srcC13 *= aOpacity;                                                   \
        _srcC24 *= aOpacity;                                                   \
                                                                               \
        /* Compose the results */                                              \
        aSrc = (( _srcC13 >> 8 ) & mask2 ) | ( _srcC24 & ( mask2 << 8 ));      \
      }                                                                        \
                                                                               \
      aDst = aSrc;                                                             \
    }
#else
  #define COPY_NATIVE_OPACITY( aDst, aSrc, aOpacity )                          \
    {                                                                          \
      unsigned int _srcA = ( CHANNEL_ALPHA( aSrc ) * aOpacity ) >> 8;          \
                                                                               \
      aSrc &= ~( 0xFFu << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );                 \
      aDst  =    aSrc | ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );        \
    }
#endif


/* The following macro provides a code fragment for the modulation of a native 
   color value by an opacity value. The expressions aSrc and aOpacity result in
   the native color value and the opacity value to modulate it. aDst refers to
   destination variable, where the operation result should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_COPY_NATIVE_OPACITY( aDst, aSrc, aOpacity )                   \
    {                                                                          \
      unsigned int _src = aSrc;                                                \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _src )                                                              \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( _src ) |                 \
                                NATIVE_2_SCREEN_BLUE ( _src );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( _src );                  \
                                                                               \
        /* Modulate the source with opacity */                                 \
        _srcC13 *= aOpacity;                                                   \
        _srcC24 *= aOpacity;                                                   \
                                                                               \
        /* Compose the results */                                              \
        _src = (( _srcC13 >> 6 ) & mask2L ) | (( _srcC24 >> 6 ) & mask2H );    \
      }                                                                        \
                                                                               \
      /* Store the results */                                                  \
      aDst = (unsigned short)( _src );                                         \
    }
#else
  #define SCREEN_COPY_NATIVE_OPACITY( aDst, aSrc, aOpacity )                   \
    {                                                                          \
      unsigned int _srcA = (( CHANNEL_ALPHA( aSrc ) + 1 ) * aOpacity ) >> 8;   \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aSrc ) |                 \
                                NATIVE_2_SCREEN_BLUE ( aSrc );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aSrc );                  \
                                                                               \
        /* Modulate the source with opacity and the alpha of the original */   \
        _srcC13 *= _srcA;                                                      \
        _srcC24 *= _srcA;                                                      \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)((( _srcC13 >> 6 ) & mask2L ) |                 \
                                (( _srcC24 >> 6 ) & mask2H ));                 \
      }                                                                        \
                                                                               \
      /* RGB565 has no alpha channel - transparent is black */                 \
      else                                                                     \
        aDst = 0;                                                              \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   between two native color values. The color values are passed in the aDst and
   aSrc expressions. The results of the alpha-blending are left in aDst. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define BLEND_NATIVE( aDst, aSrc )                                           \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc );                              \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        unsigned int _dst;                                                     \
                                                                               \
        /* Avoid the alpha-blending calculation if full opaque pixel is drawn  \
           or the background is still transparent - replace the pixel */       \
        if (( _srcA == 255 ) || (( _dst = aDst ) == 0 ))                       \
          aDst = aSrc;                                                         \
                                                                               \
        /* Combine the new color value with the pixel in the background */     \
        else                                                                   \
        {                                                                      \
          unsigned int _srcC13 =   aSrc        & mask2;                        \
          unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                        \
          unsigned int _dstC13 =   _dst        & mask2;                        \
          unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                        \
                                                                               \
          /* Perform alpha-blending */                                         \
          _srcA    = 0x100 - _srcA;                                            \
          _srcC13 += ( _dstC13 * _srcA ) >> 8;                                 \
          _srcC24 += ( _dstC24 * _srcA ) >> 8;                                 \
                                                                               \
          /* Compose the results */                                            \
          aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 );            \
        }                                                                      \
      }                                                                        \
    }
#else
  #define BLEND_NATIVE( aDst, aSrc )                                           \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc );                              \
      unsigned int _dstA = CHANNEL_ALPHA( aDst );                              \
                                                                               \
      /* Avoid the alpha-blending calculation if full opaque pixel is drawn -  \
         simply replace the pixel. The blending can also be avoided when the   \
         destination bitmap is fully transparent. Just transfer the source     \
         color information. */                                                 \
      if (( _srcA == 255 ) || ( !_dstA && _srcA ))                             \
        aDst = aSrc;                                                           \
                                                                               \
      /* Combine the new color value with the pixel in the background */       \
      else if ( _srcA )                                                        \
      {                                                                        \
        /* Extract the alpha-channel */                                        \
        unsigned int _src    =    aSrc ^                                       \
                                ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );\
        unsigned int _dst    =    aDst ^                                       \
                                ( _dstA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );\
        unsigned int _srcC13 =   _src        & mask2;                          \
        unsigned int _srcC24 = ( _src >> 8 ) & mask2;                          \
        unsigned int _dstC13 =   _dst        & mask2;                          \
        unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                          \
                                                                               \
        /* Perform the alpha-blending on all 4 channels simultanously */       \
        _dstA   = (( 0x100 - ++_srcA ) * ( _dstA + 1 )) >> 8;                  \
        _dstC13 = (( _srcC13 * _srcA ) + ( _dstC13 * _dstA )) >> 8;            \
        _dstC24 = (( _srcC24 * _srcA ) + ( _dstC24 * _dstA )) >> 8;            \
        _dstA  += _srcA;                                                       \
                                                                               \
        /* Divide the color channels by the alpha value */                     \
        if ( --_dstA < 255 )                                                   \
        {                                                                      \
          _srcA   = InvAlpha[ _dstA ];                                         \
          _dstC13 = (( _dstC13 & mask2 ) * _srcA ) >> 8;                       \
          _dstC24 = (( _dstC24 & mask2 ) * _srcA ) >> 8;                       \
        }                                                                      \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _dstC13 & mask2 ) | (( _dstC24 & mask2 ) << 8 ) |             \
               ( _dstA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );                 \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   of a native color value on a screen color value. The color values are passed
   in the aDst and aSrc expressions. The results of the alpha-blending are left
   in aDst. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_BLEND_NATIVE( aDst, aSrc )                                    \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc ) >> 2;                         \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aSrc ) |                 \
                                NATIVE_2_SCREEN_BLUE ( aSrc );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aSrc );                  \
                                                                               \
        /* Combine the new color value with the pixel in the background */     \
        if ( _srcA < 63 )                                                      \
        {                                                                      \
          unsigned int _dst    = aDst;                                         \
          unsigned int _dstC13 = _dst & mask2L;                                \
          unsigned int _dstC24 = _dst & mask2H;                                \
                                                                               \
          /* Perform alpha-blending */                                         \
          _srcA    = 64 - _srcA;                                               \
          _srcC13 += (( _dstC13 * _srcA ) >> 6 ) & mask2L;                     \
          _srcC24 += (( _dstC24 * _srcA ) >> 6 ) & mask2H;                     \
        }                                                                      \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#else
  #define SCREEN_BLEND_NATIVE( aDst, aSrc )                                    \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc ) >> 2;                         \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aSrc ) |                 \
                                NATIVE_2_SCREEN_BLUE ( aSrc );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aSrc );                  \
                                                                               \
        /* Combine the new color value with the pixel in the background */     \
        if ( _srcA < 63 )                                                      \
        {                                                                      \
          unsigned int _dst    = aDst;                                         \
          unsigned int _dstC13 = _dst & mask2L;                                \
          unsigned int _dstC24 = _dst & mask2H;                                \
                                                                               \
          /* Perform the alpha-blending on all channels simultanously */       \
          unsigned int _dstA  = ( 64 - ++_srcA );                              \
                                                                               \
          _srcC13 = (( _srcC13 * _srcA ) + ( _dstC13 * _dstA )) >> 6;          \
          _srcC24 = (( _srcC24 * _srcA ) + ( _dstC24 * _dstA )) >> 6;          \
                                                                               \
          _srcC13 &= mask2L;                                                   \
          _srcC24 &= mask2H;                                                   \
        }                                                                      \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   between two native color values with an additional opacity factor. The color
   values and the opacity are passed in the aDst, aSrc and aOpacity expressions.
   The results of the alpha-blending are left in aDst. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define BLEND_NATIVE_OPACITY( aDst, aSrc, aOpacity )                         \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc );                              \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        unsigned int _dst    = aDst;                                           \
        unsigned int _srcC13 =   aSrc        & mask2;                          \
        unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                          \
        unsigned int _dstC13 =   _dst        & mask2;                          \
        unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                          \
                                                                               \
        /* Calculate the result of blending the source with destination */     \
        if ( _dst && ( _srcA < 255 ))                                          \
        {                                                                      \
          /* Perform alpha-blending */                                         \
          _srcA    = 0x100 - _srcA;                                            \
          _srcC13 += (( _dstC13 * _srcA ) >> 8 ) & mask2;                      \
          _srcC24 += (( _dstC24 * _srcA ) >> 8 ) & mask2;                      \
        }                                                                      \
                                                                               \
        /* Interpolate between destination and the alpha-blending result */    \
        _srcA   = 0x100 - aOpacity;                                            \
        _srcC13 = (( _srcC13 * aOpacity ) + ( _dstC13 * _srcA )) >> 8;         \
        _srcC24 = (( _srcC24 * aOpacity ) + ( _dstC24 * _srcA )) >> 8;         \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 );              \
      }                                                                        \
    }
#else
  #define BLEND_NATIVE_OPACITY( aDst, aSrc, aOpacity )                         \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc );                              \
                                                                               \
      /* Avoid blending if the source pixel is transparent */                  \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Extract the alpha-channel */                                        \
        unsigned int _dstA   = CHANNEL_ALPHA( aDst );                          \
        unsigned int _src    =    aSrc ^                                       \
                                ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );\
        unsigned int _srcC13 =   _src        & mask2;                          \
        unsigned int _srcC24 = ( _src >> 8 ) & mask2;                          \
                                                                               \
        _srcA    = ( aOpacity * ( _srcA + 1 )) >> 8;                           \
        _srcC13 *= _srcA;                                                      \
        _srcC24 *= _srcA;                                                      \
                                                                               \
        /* Avoid blending if the pixel in the background is transparent */     \
        if ( _dstA > 0 )                                                       \
        {                                                                      \
          /* Extract the alpha-channel */                                      \
          unsigned int _dst   =  aDst ^                                        \
                               ( _dstA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA ); \
          unsigned int _dstC13 =   _dst        & mask2;                        \
          unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                        \
                                                                               \
          _dstA    = (( 0x100 - _srcA ) * ( _dstA + 1 )) >> 8;                 \
          _srcC13 += _dstC13 * _dstA;                                          \
          _srcC24 += _dstC24 * _dstA;                                          \
          _srcA   += _dstA;                                                    \
        }                                                                      \
                                                                               \
        _srcC13 >>= 8;                                                         \
        _srcC24 >>= 8;                                                         \
                                                                               \
        /* Divide the color channels by the alpha value */                     \
        if ( _srcA )                                                           \
          if ( --_srcA < 255 )                                                 \
          {                                                                    \
            _dstA   = InvAlpha[ _srcA ];                                       \
            _srcC13 = (( _srcC13 & mask2 ) * _dstA ) >> 8;                     \
            _srcC24 = (( _srcC24 & mask2 ) * _dstA ) >> 8;                     \
          }                                                                    \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 ) |             \
               ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );                 \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   of a native color value on a screen color value with an additional opacity 
   factor. The color values and the opacity are passed in the aDst, aSrc and 
   aOpacity expressions. The results of the alpha-blending are left in aDst. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_BLEND_NATIVE_OPACITY( aDst, aSrc, aOpacity )                  \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc ) >> 2;                         \
                                                                               \
      /* Anything to do? */                                                    \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aSrc ) |                 \
                                NATIVE_2_SCREEN_BLUE ( aSrc );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aSrc );                  \
        unsigned int _dst    = aDst;                                           \
        unsigned int _dstC13 = _dst & mask2L;                                  \
        unsigned int _dstC24 = _dst & mask2H;                                  \
                                                                               \
        /* Calculate the result of blending the source with destination */     \
        if ( _srcA < 63 )                                                      \
        {                                                                      \
          /* Perform alpha-blending */                                         \
          _srcA    = 64 - _srcA;                                               \
          _srcC13 += (( _dstC13 * _srcA ) >> 6 ) & mask2L;                     \
          _srcC24 += (( _dstC24 * _srcA ) >> 6 ) & mask2H;                     \
        }                                                                      \
                                                                               \
        /* Interpolate between destination and the alpha-blending result */    \
        _srcA    = 64 - aOpacity;                                              \
        _srcC13  = (( _srcC13 * aOpacity ) + ( _dstC13 * _srcA )) >> 6;        \
        _srcC24  = (( _srcC24 * aOpacity ) + ( _dstC24 * _srcA )) >> 6;        \
        _srcC13 &= mask2L;                                                     \
        _srcC24 &= mask2H;                                                     \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#else
  #define SCREEN_BLEND_NATIVE_OPACITY( aDst, aSrc, aOpacity )                  \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aSrc ) >> 2;                         \
                                                                               \
      /* Avoid blending if the source pixel is transparent */                  \
      if ( _srcA )                                                             \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aSrc ) |                 \
                                NATIVE_2_SCREEN_BLUE ( aSrc );                 \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aSrc );                  \
        unsigned int _dst    = aDst;                                           \
        unsigned int _dstC13 = _dst & mask2L;                                  \
        unsigned int _dstC24 = _dst & mask2H;                                  \
                                                                               \
        _srcA    = ( aOpacity * ( _srcA + 1 )) >> 6;                           \
        _srcC13 *= _srcA;                                                      \
        _srcC24 *= _srcA;                                                      \
                                                                               \
        _srcA    = 64 - _srcA ;                                                \
        _srcC13 += _dstC13 * _srcA;                                            \
        _srcC24 += _dstC24 * _srcA;                                            \
                                                                               \
        _srcC13 = ( _srcC13 >> 6 ) & mask2L;                                   \
        _srcC24 = ( _srcC24 >> 6 ) & mask2H;                                   \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for the modulation of a native 
   color value by an alpha8 value. The expressions aColor and aSrc result in
   the native color value and the opacity value to modulate it. aDst refers to
   destination variable, where the operation result should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define COPY_ALPHA8( aDst, aSrc, aColor )                                    \
    {                                                                          \
      /* Results in a transparent pixel */                                     \
      if ( !aSrc )                                                             \
        aDst = 0;                                                              \
                                                                               \
      /* ... or a full opaque color */                                         \
      else if ( aSrc == 255 )                                                  \
        aDst = aColor;                                                         \
                                                                               \
      /* The color should be modulated with opacity */                         \
      else                                                                     \
      {                                                                        \
        unsigned int _srcC13 =   aColor        & mask2;                        \
        unsigned int _srcC24 = ( aColor >> 8 ) & mask2;                        \
                                                                               \
        /* Modulate the source with opacity */                                 \
        _srcC13 *= aSrc + 1;                                                   \
        _srcC24 *= aSrc + 1;                                                   \
                                                                               \
        /* Compose the results */                                              \
        aDst = (( _srcC13 >> 8 ) & mask2 ) | ( _srcC24 & ( mask2 << 8 ));      \
      }                                                                        \
    }
#else
  #define COPY_ALPHA8( aDst, aSrc, aColor )                                    \
    {                                                                          \
      /* Results in a transparent pixel */                                     \
      if ( !aSrc )                                                             \
        aDst = 0;                                                              \
                                                                               \
      /* Full opaque color */                                                  \
      else if ( aSrc == 255 )                                                  \
        aDst = aColor;                                                         \
                                                                               \
      /* Modulate the color with the value of the source pixel */              \
      else                                                                     \
      {                                                                        \
        unsigned int _dst = aColor;                                            \
                                                                               \
        aSrc = (( aSrc + 1 ) * CHANNEL_ALPHA( _dst )) >> 8;                    \
                                                                               \
        _dst &= ~( 0xFFu << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );               \
        aDst  = _dst | ( aSrc << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );          \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for the modulation of a native 
   color value by an alpha8 value into a screen color value The expressions 
   aColor and aSrc result in the native color value and the opacity value to 
   modulate it. aDst refers to destination variable, where the operation result
   should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_COPY_ALPHA8( aDst, aSrc, aColor )                             \
    {                                                                          \
      unsigned int _opc = aSrc >> 2;                                           \
                                                                               \
      /* Results in a transparent pixel. THe RGB has no alpha channel.         \
         Transparent will appear as black. */                                  \
      if ( !_opc )                                                             \
        aDst = 0;                                                              \
                                                                               \
      /* Modulate the color with the value of the source pixel */              \
      else                                                                     \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aColor ) |               \
                                NATIVE_2_SCREEN_BLUE ( aColor );               \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aColor );                \
                                                                               \
        /* Modulate the source with opacity */                                 \
        if ( _opc < 63 )                                                       \
        {                                                                      \
          _srcC13 = (( _srcC13 * ++_opc ) >> 6 ) & mask2L;                     \
          _srcC24 = (( _srcC24 *   _opc ) >> 6 ) & mask2H;                     \
        }                                                                      \
                                                                               \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#else
  #define SCREEN_COPY_ALPHA8( aDst, aSrc, aColor )                             \
    {                                                                          \
      unsigned int _opc = aSrc >> 2;                                           \
                                                                               \
      /* Results in a transparent pixel. THe RGB has no alpha channel.         \
         Transparent will appear as black. */                                  \
      if ( !_opc )                                                             \
        aDst = 0;                                                              \
                                                                               \
      /* Modulate the color with the value of the source pixel */              \
      else                                                                     \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aColor ) |               \
                                NATIVE_2_SCREEN_BLUE ( aColor );               \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aColor );                \
        unsigned int _srcA   = CHANNEL_ALPHA( aColor ) >> 2;                   \
                                                                               \
        /* Premultiply and modulate the source color channels */               \
        if (( _opc < 63 ) || ( _srcA != _opc ))                                \
        {                                                                      \
          _srcA   = ( ++_opc * ( _srcA + 1 )) >> 6;                            \
          _srcC13 = (( _srcC13 * _srcA ) >> 6 ) & mask2L;                      \
          _srcC24 = (( _srcC24 * _srcA ) >> 6 ) & mask2H;                      \
        }                                                                      \
                                                                               \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   of a native color value modulated by an alpha8 value. The expressions aColor
   and aSrc result in the native color value and the opacity value to modulate
   it. aDst refers to destination variable, where the operation result should
   be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define BLEND_ALPHA8( aDst, aSrc, aColor )                                   \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aColor );                            \
                                                                               \
      /* Can the blending calculation be avoided? */                           \
      if (( aSrc == 255 ) && ( _srcA == aSrc ))                                \
        aDst = aColor;                                                         \
                                                                               \
      /* Anything to do? */                                                    \
      else if ( aSrc && _srcA )                                                \
      {                                                                        \
        unsigned int _dst    = aDst;                                           \
        unsigned int _srcC13 =   aColor        & mask2;                        \
        unsigned int _srcC24 = ( aColor >> 8 ) & mask2;                        \
        unsigned int _dstC13 =   _dst          & mask2;                        \
        unsigned int _dstC24 = ( _dst   >> 8 ) & mask2;                        \
                                                                               \
        /* Calculate the result of blending the source with destination */     \
        if ( _dst && ( _srcA < 255 ))                                          \
        {                                                                      \
          /* Perform alpha-blending */                                         \
          _srcA    = 0x100 - _srcA;                                            \
          _srcC13 += (( _dstC13 * _srcA ) >> 8 ) & mask2;                      \
          _srcC24 += (( _dstC24 * _srcA ) >> 8 ) & mask2;                      \
        }                                                                      \
                                                                               \
        /* Interpolate between destination and the alpha-blending result */    \
        _srcA   = 0x100 - ++aSrc;                                              \
        _srcC13 = (( _srcC13 * aSrc ) + ( _dstC13 * _srcA )) >> 8;             \
        _srcC24 = (( _srcC24 * aSrc ) + ( _dstC24 * _srcA )) >> 8;             \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 );              \
      }                                                                        \
    }
#else
  #define BLEND_ALPHA8( aDst, aSrc, aColor )                                   \
    {                                                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aColor );                            \
                                                                               \
      /* Can the blending calculation be avoided? */                           \
      if (( aSrc == 255 ) && ( _srcA == aSrc ))                                \
        aDst = aColor;                                                         \
                                                                               \
      /* Anything to do? */                                                    \
      else if ( aSrc && _srcA )                                                \
      {                                                                        \
        /* Extract the alpha-channel */                                        \
        unsigned int _dstA   = CHANNEL_ALPHA( aDst );                          \
        unsigned int _src    =  aColor ^                                       \
                               ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA ); \
        unsigned int _srcC13 =   _src        & mask2;                          \
        unsigned int _srcC24 = ( _src >> 8 ) & mask2;                          \
                                                                               \
        _srcA    = (( aSrc + 1 ) * ( _srcA + 1 )) >> 8;                        \
        _srcC13 *= _srcA;                                                      \
        _srcC24 *= _srcA;                                                      \
                                                                               \
        /* Avoid blending if the pixel in the background is transparent */     \
        if ( _dstA > 0 )                                                       \
        {                                                                      \
          /* Extract the alpha-channel */                                      \
          unsigned int _dst    = aDst ^                                        \
                               ( _dstA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA ); \
          unsigned int _dstC13 =   _dst          & mask2;                      \
          unsigned int _dstC24 = ( _dst   >> 8 ) & mask2;                      \
                                                                               \
          _dstA    = (( 0x100 - _srcA ) * ( _dstA + 1 )) >> 8;                 \
          _srcC13 += _dstC13 * _dstA;                                          \
          _srcC24 += _dstC24 * _dstA;                                          \
          _srcA   += _dstA;                                                    \
        }                                                                      \
                                                                               \
        _srcC13 >>= 8;                                                         \
        _srcC24 >>= 8;                                                         \
                                                                               \
        /* Divide the color channels by the alpha value */                     \
        if ( _srcA )                                                           \
          if ( --_srcA < 255 )                                                 \
          {                                                                    \
            _dstA   = InvAlpha[ _srcA ];                                       \
            _srcC13 = (( _srcC13 & mask2 ) * _dstA ) >> 8;                     \
            _srcC24 = (( _srcC24 & mask2 ) * _dstA ) >> 8;                     \
          }                                                                    \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 ) |             \
               ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );                 \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   of a native color value modulated by an alpha8 value with a screen color
   value. The expressions aColor and aSrc result in the native color value and
   the opacity value to modulate it. aDst refers to destination variable, where
   the operation result should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define SCREEN_BLEND_ALPHA8( aDst, aSrc, aColor )                            \
    {                                                                          \
      unsigned int _opc  = aSrc >> 2;                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aColor ) >> 2;                       \
                                                                               \
      /* Anything to blend */                                                  \
      if ( _opc && _srcA )                                                     \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aColor ) |               \
                                NATIVE_2_SCREEN_BLUE ( aColor );               \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aColor );                \
                                                                               \
        /* Is blending necessary? */                                           \
        if (( _opc < 63 ) || ( _opc != _srcA ))                                \
        {                                                                      \
          unsigned int _dst    = aDst;                                         \
          unsigned int _dstC13 = _dst & mask2L;                                \
          unsigned int _dstC24 = _dst & mask2H;                                \
                                                                               \
          /* Calculate the result of blending the source with destination */   \
          if ( _srcA < 63 )                                                    \
          {                                                                    \
            /* Perform alpha-blending */                                       \
            _srcA    = 64 - _srcA;                                             \
            _srcC13 += (( _dstC13 * _srcA ) >> 6 ) & mask2L;                   \
            _srcC24 += (( _dstC24 * _srcA ) >> 6 ) & mask2H;                   \
          }                                                                    \
                                                                               \
          /* Interpolate between destination and the alpha-blending result */  \
          _srcA   = 64 - ++_opc;                                               \
          _srcC13 = ((( _srcC13 * _opc ) + ( _dstC13 * _srcA )) >> 6 );        \
          _srcC24 = ((( _srcC24 * _opc ) + ( _dstC24 * _srcA )) >> 6 );        \
                                                                               \
          _srcC13 &= mask2L;                                                   \
          _srcC24 &= mask2H;                                                   \
        }                                                                      \
                                                                               \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#else
  #define SCREEN_BLEND_ALPHA8( aDst, aSrc, aColor )                            \
    {                                                                          \
      unsigned int _opc  = aSrc >> 2;                                          \
      unsigned int _srcA = CHANNEL_ALPHA( aColor ) >> 2;                       \
                                                                               \
      /* Anything to blend */                                                  \
      if ( _opc && _srcA )                                                     \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 = NATIVE_2_SCREEN_RED  ( aColor ) |               \
                                NATIVE_2_SCREEN_BLUE ( aColor );               \
        unsigned int _srcC24 = NATIVE_2_SCREEN_GREEN( aColor );                \
                                                                               \
        /* Is blending necessary? */                                           \
        if (( _opc < 63 ) || ( _opc != _srcA ))                                \
        {                                                                      \
          unsigned int _dst    = aDst;                                         \
          unsigned int _dstC13 = _dst & mask2L;                                \
          unsigned int _dstC24 = _dst & mask2H;                                \
                                                                               \
          _srcA    = ( ++_opc * ( _srcA + 1 )) >> 6;                           \
          _srcC13 *= _srcA;                                                    \
          _srcC24 *= _srcA;                                                    \
                                                                               \
          _srcA    = 64 - _srcA;                                               \
          _srcC13 += _dstC13 * _srcA;                                          \
          _srcC24 += _dstC24 * _srcA;                                          \
                                                                               \
          _srcC13 = ( _srcC13 >> 6 ) & mask2L;                                 \
          _srcC24 = ( _srcC24 >> 6 ) & mask2H;                                 \
        }                                                                      \
                                                                               \
        /* Compose the results */                                              \
        aDst = (unsigned short)( _srcC13 | _srcC24 );                          \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for the modulation of an RGB565 
   color value by an opacity value. The expressions aSrc and aOpacity result in
   the RGB565 color value and the opacity value to modulate it. aDst refers to
   destination variable, where the Native operation result should be left. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define COPY_RGB565_OPACITY( aDst, aSrc, aOpacity )                          \
    {                                                                          \
      /* Convert RGB565 in the Native format */                                \
      RGB565_2_NATIVE_OPAQUE( aSrc, aSrc );                                    \
                                                                               \
      /* Modulate the resulting color value */                                 \
      {                                                                        \
        /* Bring all color channels into 2 x 32-bit register and ... */        \
        unsigned int _srcC13 =   aSrc        & mask2;                          \
        unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                          \
                                                                               \
        /* Modulate the source with opacity */                                 \
        _srcC13 *= aOpacity;                                                   \
        _srcC24 *= aOpacity;                                                   \
                                                                               \
        /* Compose the results */                                              \
        aSrc = (( _srcC13 >> 8 ) & mask2 ) | ( _srcC24 & ( mask2 << 8 ));      \
      }                                                                        \
                                                                               \
      aDst = aSrc;                                                             \
    }
#else
  #define COPY_RGB565_OPACITY( aDst, aSrc, aOpacity )                          \
    {                                                                          \
      /* Convert RGB565 in the Native format */                                \
      if ( aSrc )                                                              \
        RGB565_2_NATIVE_TRANSPARENT( aSrc, aSrc );                             \
                                                                               \
      aDst = aSrc | ( aOpacity << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );         \
    }
#endif


/* The following macro provides a code fragment for alpha-blending calculation
   between an RGB565 and native color values with an additional opacity factor.
   The color values and the opacity are passed in the aDst, aSrc and aOpacity
   expressions. The results of the alpha-blending are left in aDst. */
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  #define BLEND_RGB565_OPACITY( aDst, aSrc, aOpacity )                         \
    {                                                                          \
      /* Convert RGB565 in the Native format */                                \
      RGB565_2_NATIVE_OPAQUE( aSrc, aSrc );                                    \
                                                                               \
      /* Perform the alpha-blending */                                         \
      {                                                                        \
        unsigned int _dst    = aDst;                                           \
        unsigned int _srcC13 =   aSrc        & mask2;                          \
        unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                          \
        unsigned int _dstC13 =   _dst        & mask2;                          \
        unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                          \
                                                                               \
        /* Interpolate between destination and the alpha-blending result */    \
        unsigned int _srcA   = 0x100 - aOpacity;                               \
        _srcC13 = (( _srcC13 * aOpacity ) + ( _dstC13 * _srcA )) >> 8;         \
        _srcC24 = (( _srcC24 * aOpacity ) + ( _dstC24 * _srcA )) >> 8;         \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 );              \
      }                                                                        \
    }
#else
  #define BLEND_RGB565_OPACITY( aDst, aSrc, aOpacity )                         \
    {                                                                          \
      /* Convert RGB565 in the Native format */                                \
      RGB565_2_NATIVE_TRANSPARENT( aSrc, aSrc );                               \
                                                                               \
      /* Perform the alpha-blending */                                         \
      {                                                                        \
        /* Extract the alpha-channel */                                        \
        unsigned int _srcA   = aOpacity;                                       \
        unsigned int _dstA   = CHANNEL_ALPHA( aDst );                          \
        unsigned int _srcC13 =   aSrc        & mask2;                          \
        unsigned int _srcC24 = ( aSrc >> 8 ) & mask2;                          \
                                                                               \
        _srcC13 *= _srcA;                                                      \
        _srcC24 *= _srcA;                                                      \
                                                                               \
        /* Avoid blending if the pixel in the background is transparent */     \
        if ( _dstA > 0 )                                                       \
        {                                                                      \
          /* Extract the alpha-channel */                                      \
          unsigned int _dst   =  aDst ^                                        \
                               ( _dstA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA ); \
          unsigned int _dstC13 =   _dst        & mask2;                        \
          unsigned int _dstC24 = ( _dst >> 8 ) & mask2;                        \
                                                                               \
          _dstA    = (( 0x100 - _srcA ) * ( _dstA + 1 )) >> 8;                 \
          _srcC13 += _dstC13 * _dstA;                                          \
          _srcC24 += _dstC24 * _dstA;                                          \
          _srcA   += _dstA;                                                    \
        }                                                                      \
                                                                               \
        _srcC13 >>= 8;                                                         \
        _srcC24 >>= 8;                                                         \
                                                                               \
        /* Divide the color channels by the alpha value */                     \
        if ( _srcA )                                                           \
          if ( --_srcA < 255 )                                                 \
          {                                                                    \
            _dstA   = InvAlpha[ _srcA ];                                       \
            _srcC13 = (( _srcC13 & mask2 ) * _dstA ) >> 8;                     \
            _srcC24 = (( _srcC24 & mask2 ) * _dstA ) >> 8;                     \
          }                                                                    \
                                                                               \
        /* Compose the results */                                              \
        aDst = ( _srcC13 & mask2 ) | (( _srcC24 & mask2 ) << 8 ) |             \
               ( _srcA << EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );                 \
      }                                                                        \
    }
#endif


/* The following macro provides a code fragment for the modulation of an RGB565 
   color value by an opacity value. The expressions aSrc and aOpacity result in
   the RGB565 color value and the opacity value to modulate it. aDst refers to
   destination variable, where the operation result should be left. */
#define SCREEN_COPY_RGB565_OPACITY( aDst, aSrc, aOpacity )                     \
  {                                                                            \
    unsigned int _src = aSrc;                                                  \
                                                                               \
    /* Anything to do? */                                                      \
    if ( _src )                                                                \
    {                                                                          \
      /* Bring all color channels into 2 x 32-bit register and ... */          \
      unsigned int _srcC13 = aSrc & mask2L;                                    \
      unsigned int _srcC24 = aSrc & mask2H;                                    \
                                                                               \
      /* Modulate the source with opacity */                                   \
      _srcC13 *= aOpacity;                                                     \
      _srcC24 *= aOpacity;                                                     \
                                                                               \
      /* Compose the results */                                                \
      _src = (( _srcC13 >> 6 ) & mask2L ) | (( _srcC24 >> 6 ) & mask2H );      \
    }                                                                          \
                                                                               \
    /* Store the results */                                                    \
    aDst = (unsigned short)( _src );                                           \
  }


/* The following macro provides a code fragment for alpha-blending calculation
   between two native color values with an additional opacity factor. The color
   values and the opacity are passed in the aDst, aSrc and aOpacity expressions.
   The results of the alpha-blending are left in aDst. */
#define SCREEN_BLEND_RGB565_OPACITY( aDst, aSrc, aOpacity )                    \
  {                                                                            \
    unsigned int _dst    = aDst;                                               \
    unsigned int _srcC13 = aSrc & mask2L;                                      \
    unsigned int _srcC24 = aSrc & mask2H;                                      \
    unsigned int _dstC13 = _dst & mask2L;                                      \
    unsigned int _dstC24 = _dst & mask2H;                                      \
    unsigned int _srcA   = 64 - aOpacity;                                      \
                                                                               \
    /* Interpolate between destination and the alpha-blending result */        \
    _srcC13  = (( _srcC13 * aOpacity ) + ( _dstC13 * _srcA )) >> 6;            \
    _srcC24  = (( _srcC24 * aOpacity ) + ( _dstC24 * _srcA )) >> 6;            \
    _srcC13 &= mask2L;                                                         \
    _srcC24 &= mask2H;                                                         \
                                                                               \
    /* Compose the results */                                                  \
    aDst = (unsigned short)( _srcC13 | _srcC24 );                              \
  }


/* The following macro provides a code fragment for the bi-linear smooth filter.
   The filter calculates the average value from up to 4 neighboring pixel by
   taking in account the area fraction of each affected pixel. The pixel memory
   to pick the color values is determined by the expression aSrcPtr and the
   distance between two pixel rows within this memory is got by aSrcPitch. The
   pixel area to calculate the average is determined in the expressions aX, aY
   in .4 sub-pixel precision. The results are left in aDst. */
#define BILINEAR_NATIVE( aDst, aSrcPtr, aSrcPitch, aX, aY )                    \
  {                                                                            \
    unsigned int* _srcPtr = aSrcPtr + ( aY >> 4 ) * aSrcPitch + ( aX >> 4 );   \
    unsigned int  _src1;                                                       \
    unsigned int  _src2;                                                       \
    unsigned int  _src3;                                                       \
    unsigned int  _src4;                                                       \
                                                                               \
    /* Calculate the area fractions of the 4 pixel to apply the bi-linear      \
       algorithm */                                                            \
    int _sx = aX & 0xF;                                                        \
    int _sy = aY & 0xF;                                                        \
    int _f2 = _sx * ( 16 - _sy );                                              \
    int _f3 = _sy * ( 16 - _sx );                                              \
    int _f4 = _sx * _sy;                                                       \
    int _f1 = 256 - _f2 - _f3 - _f4;                                           \
                                                                               \
    /* Read the content of the 4 source pixel */                               \
    _src1    = *MAP_UINT32( _srcPtr++ );                                       \
    _src2    = *MAP_UINT32( _srcPtr   );                                       \
    _srcPtr += aSrcPitch;                                                      \
    _src4    = *MAP_UINT32( _srcPtr-- );                                       \
    _src3    = *MAP_UINT32( _srcPtr   );                                       \
                                                                               \
    /* Bring together the fractions of the 4 source pixel */                   \
    aDst =                                                                     \
    (                                                                          \
      (                                                                        \
        (                                                                      \
          (( _src1 & mask2 ) * _f1 ) +                                         \
          (( _src2 & mask2 ) * _f2 ) +                                         \
          (( _src3 & mask2 ) * _f3 ) +                                         \
          (( _src4 & mask2 ) * _f4 )                                           \
        ) >> 8                                                                 \
      ) & mask2                                                                \
    ) |                                                                        \
    (                                                                          \
      (                                                                        \
        ((( _src1 & mask3 ) >> 8 ) * _f1 ) +                                   \
        ((( _src2 & mask3 ) >> 8 ) * _f2 ) +                                   \
        ((( _src3 & mask3 ) >> 8 ) * _f3 ) +                                   \
        ((( _src4 & mask3 ) >> 8 ) * _f4 )                                     \
      ) & mask3                                                                \
    );                                                                         \
  }


/* The following macro provides a code fragment for the bi-linear smooth filter.
   The filter calculates the average value from up to 4 neighboring pixel by
   taking in account the area fraction of each affected pixel. The pixel memory
   to pick the color values is determined by the expression aSrcPtr and the
   distance between two pixel rows within this memory is got by aSrcPitch. The
   pixel area to calculate the average is determined in the expressions aX, aY
   in .4 sub-pixel precision. The results are left in aDst. */
#define BILINEAR_INDEX8( aDst, aSrcPtr, aClut, aSrcPitch, aX, aY )             \
  {                                                                            \
    unsigned char* _srcPtr = aSrcPtr + ( aY >> 4 ) * aSrcPitch + ( aX >> 4 );  \
    unsigned int   _src1;                                                      \
    unsigned int   _src2;                                                      \
    unsigned int   _src3;                                                      \
    unsigned int   _src4;                                                      \
                                                                               \
    /* Calculate the area fractions of the 4 pixel to apply the bi-linear      \
       algorithm */                                                            \
    int _sx = aX & 0xF;                                                        \
    int _sy = aY & 0xF;                                                        \
    int _f2 = _sx * ( 16 - _sy );                                              \
    int _f3 = _sy * ( 16 - _sx );                                              \
    int _f4 = _sx * _sy;                                                       \
    int _f1 = 256 - _f2 - _f3 - _f4;                                           \
                                                                               \
    /* Read the content of the 4 source pixel */                               \
    _src1    = *MAP_UINT32( aClut + *MAP_UINT8( _srcPtr++ ));                  \
    _src2    = *MAP_UINT32( aClut + *MAP_UINT8( _srcPtr   ));                  \
    _srcPtr += aSrcPitch;                                                      \
    _src4    = *MAP_UINT32( aClut + *MAP_UINT8( _srcPtr-- ));                  \
    _src3    = *MAP_UINT32( aClut + *MAP_UINT8( _srcPtr   ));                  \
                                                                               \
    /* Bring together the fractions of the 4 source pixel */                   \
    aDst =                                                                     \
    (                                                                          \
      (                                                                        \
        (                                                                      \
          (( _src1 & mask2 ) * _f1 ) +                                         \
          (( _src2 & mask2 ) * _f2 ) +                                         \
          (( _src3 & mask2 ) * _f3 ) +                                         \
          (( _src4 & mask2 ) * _f4 )                                           \
        ) >> 8                                                                 \
      ) & mask2                                                                \
    ) |                                                                        \
    (                                                                          \
      (                                                                        \
        ((( _src1 & mask3 ) >> 8 ) * _f1 ) +                                   \
        ((( _src2 & mask3 ) >> 8 ) * _f2 ) +                                   \
        ((( _src3 & mask3 ) >> 8 ) * _f3 ) +                                   \
        ((( _src4 & mask3 ) >> 8 ) * _f4 )                                     \
      ) & mask3                                                                \
    );                                                                         \
  }


/* The following macro provides a code fragment for the bi-linear smooth filter.
   The filter calculates the average value from up to 4 neighboring pixel by
   taking in account the area fraction of each affected pixel. The pixel memory
   to pick the alpha8 values is determined by the expression aSrcPtr and the
   distance between two pixel rows within this memory is got by aSrcPitch. The
   pixel area to calculate the average is determined in the expressions aX, aY
   in .4 sub-pixel precision. The results are left in aDst. */
#define BILINEAR_ALPHA8( aDst, aSrcPtr, aSrcPitch, aX, aY )                    \
  {                                                                            \
    unsigned char* _srcPtr = aSrcPtr + ( aY >> 4 ) * aSrcPitch + ( aX >> 4 );  \
    unsigned int  _src1;                                                       \
    unsigned int  _src2;                                                       \
    unsigned int  _src3;                                                       \
    unsigned int  _src4;                                                       \
                                                                               \
    /* Calculate the area fractions of the 4 pixel to apply the bi-linear      \
       algorithm */                                                            \
    int _sx = aX & 0xF;                                                        \
    int _sy = aY & 0xF;                                                        \
    int _f2 = _sx * ( 16 - _sy );                                              \
    int _f3 = _sy * ( 16 - _sx );                                              \
    int _f4 = _sx * _sy;                                                       \
    int _f1 = 256 - _f2 - _f3 - _f4;                                           \
                                                                               \
    /* Read the content of the 4 source pixel */                               \
    _src1    = *MAP_UINT8( _srcPtr++ );                                        \
    _src2    = *MAP_UINT8( _srcPtr   );                                        \
    _srcPtr += aSrcPitch;                                                      \
    _src4    = *MAP_UINT8( _srcPtr-- );                                        \
    _src3    = *MAP_UINT8( _srcPtr   );                                        \
                                                                               \
    /* Bring together the fractions of the 4 source pixel */                   \
    aDst =                                                                     \
    (                                                                          \
      (                                                                        \
        ( _src1 * _f1 ) +                                                      \
        ( _src2 * _f2 ) +                                                      \
        ( _src3 * _f3 ) +                                                      \
        ( _src4 * _f4 )                                                        \
      ) >> 8                                                                   \
    );                                                                         \
  }


/* The following macro provides a code fragment for the bi-linear smooth filter.
   The filter calculates the average value from up to 4 neighboring pixel by
   taking in account the area fraction of each affected pixel. The pixel memory
   to pick the color values is determined by the expression aSrcPtr and the
   distance between two pixel rows within this memory is got by aSrcPitch. The
   pixel area to calculate the average is determined in the expressions aX, aY
   in .4 sub-pixel precision. The results are left in aDst. */
#define BILINEAR_RGB565( aDst, aSrcPtr, aSrcPitch, aX, aY )                    \
  {                                                                            \
    unsigned short* _srcPtr = aSrcPtr + ( aY >> 4 ) * aSrcPitch + ( aX >> 4 ); \
    unsigned int    _src1;                                                     \
    unsigned int    _src2;                                                     \
    unsigned int    _src3;                                                     \
    unsigned int    _src4;                                                     \
                                                                               \
    /* Calculate the area fractions of the 4 pixel to apply the bi-linear      \
       algorithm */                                                            \
    int _sx = ( aX >> 1 ) & 0x7;                                               \
    int _sy = ( aY >> 1 ) & 0x7;                                               \
    int _f2 = _sx * ( 8 - _sy );                                               \
    int _f3 = _sy * ( 8 - _sx );                                               \
    int _f4 = _sx * _sy;                                                       \
    int _f1 = 64 - _f2 - _f3 - _f4;                                            \
                                                                               \
    /* Read the content of the 4 source pixel (Note the both bitmap planes) */ \
    _src1    = *MAP_UINT16( _srcPtr++ );                                       \
    _src2    = *MAP_UINT16( _srcPtr   );                                       \
    _srcPtr += aSrcPitch;                                                      \
    _src4    = *MAP_UINT16( _srcPtr-- );                                       \
    _src3    = *MAP_UINT16( _srcPtr   );                                       \
                                                                               \
    /* Bring together the fractions of the 4 source pixel */                   \
    aDst =                                                                     \
    (                                                                          \
      (                                                                        \
        (                                                                      \
          (                                                                    \
            (( _src1 & mask2L ) * _f1 ) +                                      \
            (( _src2 & mask2L ) * _f2 ) +                                      \
            (( _src3 & mask2L ) * _f3 ) +                                      \
            (( _src4 & mask2L ) * _f4 )                                        \
          ) >> 6                                                               \
        ) & mask2L                                                             \
      ) |                                                                      \
      (                                                                        \
        (                                                                      \
          (                                                                    \
            (( _src1 & mask2H ) * _f1 ) +                                      \
            (( _src2 & mask2H ) * _f2 ) +                                      \
            (( _src3 & mask2H ) * _f3 ) +                                      \
            (( _src4 & mask2H ) * _f4 )                                        \
          ) >> 6                                                               \
        ) & mask2H                                                             \
      )                                                                        \
    );                                                                         \
  }


/* The following macro provides a code fragment for the gradient RGBA to native
   pixel format conversion. The expressions aRed, aGreen aBlue, aAlpha result 
   in RGBA color channel values in the gradient high precision (20 bit). */
#define GRADIENT_TO_COLOR( aRed, aGreen, aBlue, aAlpha )                       \
  (                                                                            \
    EW_SHIFT( aRed   & mask1, 20 - EW_COLOR_CHANNEL_BIT_OFFSET_RED   ) |       \
    EW_SHIFT( aGreen & mask1, 20 - EW_COLOR_CHANNEL_BIT_OFFSET_GREEN ) |       \
    EW_SHIFT( aBlue  & mask1, 20 - EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  ) |       \
    EW_SHIFT( aAlpha & mask1, 20 - EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA )         \
  )


/* The following macro provides a code fragment for the determination of opacity
   values from a gradient depending on the position within this gradient area.
   The gradient and the position are determined by the expressions aGradient,
   aX and aY. The result of the calculation is left in the variable aOpacity. */
#define PICK_FROM_OPACITY_GRADIENT( aOpacity, aGradient, aX, aY )              \
  {                                                                            \
    int _al = aGradient->A0;                                                   \
    int _ar = aGradient->A2;                                                   \
    int _x  = aX;                                                              \
    int _y  = aY;                                                              \
                                                                               \
    /* Interpolate the gradient values vertically */                           \
    if ( aGradient->IsVertical && _y )                                         \
      _al += _y * aGradient->A1;                                               \
                                                                               \
    /* Avoid unnecessary calculations for vertical only gradients */           \
    if ( aGradient->IsVertical && aGradient->IsHorizontal && _y )              \
      _ar += _y * aGradient->A3;                                               \
                                                                               \
    /* Interpolate the gradient values horizontally */                         \
    if ( aGradient->IsHorizontal && _x )                                       \
      _al = ( _al & mask1 ) + (( _ar >> 20 ) - ( _al >> 20 )) *                \
              aGradient->InvWidth * _x;                                        \
                                                                               \
    aOpacity = ( _al & mask1 ) >> 20;                                          \
  }


/* The following macro provides a code fragment for the determination of color
   values from a gradient depending on the position within this gradient area.
   The gradient and the position are determined by the expressions aGradient,
   aX and aY. The result of the calculation is left in the variable aColor. */
#define PICK_FROM_COLOR_GRADIENT( aColor, aGradient, aX, aY )                  \
  {                                                                            \
    int _rl = aGradient->R0;                                                   \
    int _gl = aGradient->G0;                                                   \
    int _bl = aGradient->B0;                                                   \
    int _al = aGradient->A0;                                                   \
    int _rr = aGradient->R2;                                                   \
    int _gr = aGradient->G2;                                                   \
    int _br = aGradient->B2;                                                   \
    int _ar = aGradient->A2;                                                   \
    int _x  = aX;                                                              \
    int _y  = aY;                                                              \
                                                                               \
    /* Interpolate the gradient values vertically */                           \
    if ( aGradient->IsVertical && _y )                                         \
    {                                                                          \
      _rl += _y * aGradient->R1;                                               \
      _gl += _y * aGradient->G1;                                               \
      _bl += _y * aGradient->B1;                                               \
      _al += _y * aGradient->A1;                                               \
    }                                                                          \
                                                                               \
    /* Avoid unnecessary calculations for vertical only gradients */           \
    if ( aGradient->IsVertical && aGradient->IsHorizontal && _y )              \
    {                                                                          \
      _rr += _y * aGradient->R3;                                               \
      _gr += _y * aGradient->G3;                                               \
      _br += _y * aGradient->B3;                                               \
      _ar += _y * aGradient->A3;                                               \
    }                                                                          \
                                                                               \
    /* Interpolate the gradient values horizontally */                         \
    if ( aGradient->IsHorizontal && _x )                                       \
    {                                                                          \
      _x  *= aGradient->InvWidth;                                              \
      _rl  = ( _rl & mask1 ) + (( _rr >> 20 ) - ( _rl >> 20 )) * _x;           \
      _gl  = ( _gl & mask1 ) + (( _gr >> 20 ) - ( _gl >> 20 )) * _x;           \
      _bl  = ( _bl & mask1 ) + (( _br >> 20 ) - ( _bl >> 20 )) * _x;           \
      _al  = ( _al & mask1 ) + (( _ar >> 20 ) - ( _al >> 20 )) * _x;           \
    }                                                                          \
                                                                               \
    /* Compose the four channels to a single native color */                   \
    aColor = GRADIENT_TO_COLOR( _rl, _gl, _bl, _al );                          \
  }


/* This is an internal representation of a native software only RGBA8888
   surface. */
typedef struct
{
  int               Width;
  int               Height;
  unsigned char*    Pixel;
  int               PitchY;
  int               IsConst;
  int               IsFramebuffer;
} XNativeSurface;


/* The following low-level driver function has the job to create and return a
   new native surface with the given size. If no surface could be created due
   to memory deficit, 0 is returned. */
unsigned long EwCreateNativeSurface( int aWidth, int aHeight )
{
  int             size    = aWidth * aHeight * sizeof( int );
  XNativeSurface* surface = EwAllocVideo( sizeof( XNativeSurface ) + size );

  if ( surface )
  {
    surface->Width         = aWidth;
    surface->Height        = aHeight;
    surface->Pixel         = (unsigned char*)( surface + 1 );
    surface->PitchY        = aWidth * sizeof( int );
    surface->IsConst       = 0;
    surface->IsFramebuffer = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size + sizeof( XNativeSurface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/* The following low-level driver function has the job to create and return a
   new native surface with the given size and pixel data content. If no surface
   could be created due to memory deficit, 0 is returned. */
unsigned long EwCreateConstNativeSurface( int aWidth, int aHeight,
  XSurfaceMemory* aMemory )
{
  XNativeSurface* surface = 0;

  /* Verify superficially the passed pixel memory */
  if ( !aMemory || !aMemory->Pixel1 || ( aMemory->Pitch1X != sizeof( int )) ||
     ( aMemory->Pitch1Y < (int)( aWidth * sizeof( int ))))
    return 0;

  /* Reserve memory for the descriptor of the surface */
  surface = EwAllocVideo( sizeof( XNativeSurface ));

  if ( surface )
  {
    surface->Width         = aWidth;
    surface->Height        = aHeight;
    surface->Pixel         = aMemory->Pixel1;
    surface->PitchY        = aMemory->Pitch1Y;
    surface->IsConst       = 1;
    surface->IsFramebuffer = 0;

    /* Track the RAM usage */
    EwResourcesMemory += sizeof( XNativeSurface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/* The following low-level driver function has the job to release resources of
   the given native surface. */
void EwDestroyNativeSurface( unsigned long aSurface )
{
  XNativeSurface* surface = (XNativeSurface*)aSurface;

  if ( surface && !surface->IsFramebuffer )
    EwResourcesMemory -= sizeof( XNativeSurface ) + ( surface->IsConst? 0 :
                               ( surface->Width * surface->Height * sizeof ( int )));

  EwFreeVideo( surface );
}


/* The following low-level driver function has the job to obtain a pointer to
   the surface color plane and if existing to the alpha plane. If the surface
   is using a color table, the function should also return the pointer to the
   table.

   The returned pointers are calculate for the given pixel position aX, aY
   and in case of the color table, to the entry at the position aIndex.

   If successful, the function returns != 0. */
int EwGetNativeSurfaceMemory( unsigned long aSurface, int aX, int aY, 
  int aIndex, int aWriteAccess, XSurfaceMemory* aMemory )
{
  XNativeSurface* surface = (XNativeSurface*)aSurface;

  EW_UNUSED_ARG( aIndex );

  /* Can't modify constant surface pixel data */
  if ( aWriteAccess && surface->IsConst )
    return 0;

  aMemory->Pixel1  = surface->Pixel + ( aY * surface->PitchY ) + 
                                        aX * sizeof( int );
  aMemory->Pitch1Y = surface->PitchY;
  aMemory->Pitch1X = sizeof( int );
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


/* The function EwGetNativeSurfaceMemSize() has the job to determine the number
   of memory bytes occupied by a native surface with the given size. */
int EwGetNativeSurfaceMemSize( int aWidth, int aHeight )
{
  return aWidth * aHeight * sizeof( unsigned int ) + sizeof( XNativeSurface );
}


/* This is an internal representation of a screen software only RGB565
   surface. */
typedef struct
{
  int               Width;
  int               Height;
  unsigned char*    Pixel;
  int               PitchY;
  int               IsFramebuffer;
} XScreenSurface;


/* The following low-level driver function has the job to create and return a
   new screen surface with the given size. If no surface could be created due
   to memory deficit, 0 is returned. */
unsigned long EwCreateScreenSurface( int aWidth, int aHeight )
{
  int             size    = aWidth * aHeight * sizeof( short );
  XScreenSurface* surface = EwAllocVideo( sizeof( XScreenSurface ) + size );

  if ( surface )
  {
    surface->Width         = aWidth;
    surface->Height        = aHeight;
    surface->Pixel         = (unsigned char*)( surface + 1 );
    surface->PitchY        = aWidth * sizeof( short );
    surface->IsFramebuffer = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size + sizeof( XScreenSurface );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  return (unsigned long)surface;  
}


/* The following low-level driver function has the job to release resources of
   the given screen surface. */
void EwDestroyScreenSurface( unsigned long aSurface )
{
  XScreenSurface* surface = (XScreenSurface*)aSurface;

  if ( surface && !surface->IsFramebuffer )
    EwResourcesMemory -= sizeof( XScreenSurface ) + 
                               ( surface->Width * surface->Height * sizeof ( short ));

  EwFreeVideo( surface );
}


/* The following low-level driver function has the job to obtain a pointer to
   the surface color plane and if existing to the alpha plane. If the surface
   is using a color table, the function should also return the pointer to the
   table.

   The returned pointers are calculate for the given pixel position aX, aY
   and in case of the color table, to the entry at the position aIndex.

   If successful, the function returns != 0. */
int EwGetScreenSurfaceMemory( unsigned long aSurface, int aX, int aY, 
  int aIndex, int aWriteAccess, XSurfaceMemory* aMemory )
{
  XScreenSurface* surface = (XScreenSurface*)aSurface;

  EW_UNUSED_ARG( aIndex );
  EW_UNUSED_ARG( aWriteAccess );

  aMemory->Pixel1  = surface->Pixel + ( aY * surface->PitchY ) + 
                                        aX * sizeof( short );
  aMemory->Pitch1Y = surface->PitchY;
  aMemory->Pitch1X = sizeof( short );
  aMemory->Pixel2  = 0;
  aMemory->Pitch2Y = 0;
  aMemory->Pitch2X = 0;
  aMemory->Clut    = 0;

  return 1;
}


/* The function EwGetScreenSurfaceMemSize() has the job to determine the number
   of memory bytes occupied by a screen surface with the given size. */
int EwGetScreenSurfaceMemSize( int aWidth, int aHeight )
{
  return aWidth * aHeight * sizeof( unsigned short );
}


/* This is an internal representation of a software only RGB565 surface. */
typedef struct
{
  int               Width;
  int               Height;
  unsigned char*    Pixel;
  int               PitchY;
  int               IsConst;
} XRGB565Surface;


/*******************************************************************************
* FUNCTION:
*   EwCreateConstRGB565Surface
*
* DESCRIPTION:
*   The function EwCreateConstRGB565Surface() has the job to create and return
*   a new rgb565 surface with the given size and pixel data content. Important
*   here is the fact that the function doesn't reserve any memory to store the
*   pixel information. Instead it, the surface associates the pixel data passed
*   in the parameter aMemory.
*
*   This function is thus intended to create surfaces from the pixel data stored
*   directly within a ROM area. Accordingly it is essential that the ROM content
*   does exactly correspond to the rgb565 surface pixel format. It includes the
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
unsigned long EwCreateConstRGB565Surface( int aWidth, int aHeight,
  XSurfaceMemory* aMemory )
{
  XRGB565Surface* surface = 0;

  /* Verify superficially the passed pixel memory */
  if ( !aMemory || !aMemory->Pixel1 || ( aMemory->Pitch1X != sizeof( short )) ||
     ( aMemory->Pitch1Y < (int)( aWidth * sizeof( short ))))
    return 0;

  /* Reserve memory for the descriptor of the surface */
  surface = EwAllocVideo( sizeof( XRGB565Surface ));

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Pixel   = aMemory->Pixel1;
    surface->PitchY  = aMemory->Pitch1Y;
    surface->IsConst = 1;

    /* Track the RAM usage */
    EwResourcesMemory += sizeof( XRGB565Surface );

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
*   EwCreateRGB565Surface
*
* DESCRIPTION:
*   The function EwCreateRGB565Surface() has the job to create and return a new
*   rgb565 surface with the given size. If no surface could be created due to
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
unsigned long EwCreateRGB565Surface( int aWidth, int aHeight )
{
  int             size    = aWidth * aHeight * sizeof( short );
  XRGB565Surface* surface = EwAllocVideo( sizeof( XRGB565Surface ) + size );

  if ( surface )
  {
    surface->Width   = aWidth;
    surface->Height  = aHeight;
    surface->Pixel   = (unsigned char*)( surface + 1 );
    surface->PitchY  = aWidth * sizeof( short );
    surface->IsConst = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size + sizeof( XRGB565Surface );

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
*   EwDestroyRGB565Surface
*
* DESCRIPTION:
*   The function EwDestroyRGB565Surface() has the job to release resources of
*   a previously created rgb565 surface.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function destroys pure software surfaces
*   previously created by the function EwCreateRGB565Surface() only.
*
* ARGUMENTS:
*   aSurface - Handle to the surface to destroy.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDestroyRGB565Surface( unsigned long aSurface )
{
  XRGB565Surface* surface = (XRGB565Surface*)aSurface;

  if ( surface )
    EwResourcesMemory -= sizeof( XRGB565Surface ) + ( surface->IsConst? 0 :
                               ( surface->Width * surface->Height * sizeof( short )));

  EwFreeVideo( surface );
}


/*******************************************************************************
* FUNCTION:
*   EwGetRGB565SurfaceMemory
*
* DESCRIPTION:
*   The function EwGetRGB565SurfaceMemory() has the job to obtain the pointer to
*   the rgb565 memory plane and to the color table of the given surface.
*
*   The returned pointer is calculated for the given pixel position aX, aY and
*   in case of the color table, to the entry at the position aIndex.
*
*   Please note, if the affected surface has been created with constant pixel
*   information by using EwCreateConstRGB565Surface(), you can't obtain the
*   write access to such surface. In such case the function will fail with the
*   return value 0.
*
*   The function is used, if the underlying graphics subsystem doesn't provide
*   any adequate functionality. The function can handle with the pure software
*   surfaces previously created by the function EwCreateRGB565Surface() and
*   EwCreateConstRGB565Surface() only.
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
int EwGetRGB565SurfaceMemory( unsigned long aSurface, int aX, int aY, int aIndex,
  int aWriteAccess, XSurfaceMemory* aMemory )
{
  XRGB565Surface* surface = (XRGB565Surface*)aSurface;

  EW_UNUSED_ARG( aIndex );

  /* Can't modify constant surface pixel data */
  if ( aWriteAccess && surface->IsConst )
    return 0;

  aMemory->Pixel1  = surface->Pixel + ( aY * surface->PitchY ) + 
                                        aX * sizeof( short );
  aMemory->Pitch1Y = surface->PitchY;
  aMemory->Pitch1X = sizeof( short );
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
*   EwGetRGB565SurfaceMemSize
*
* DESCRIPTION:
*   The function EwGetRGB565SurfaceMemSize() has the job to determine the number
*   of memory bytes occupied by an rgb565 surface with the given size.
*
* ARGUMENTS:
*   aWidth,
*   aHeight - Size of the surface in pixel.
*
* RETURN VALUE:
*   If successful, the function return the surface size in bytes.
*
*******************************************************************************/
int EwGetRGB565SurfaceMemSize( int aWidth, int aHeight )
{
  return aWidth * aHeight * sizeof( unsigned short ) + sizeof( XRGB565Surface );
}


/* The following EwPackColor() function has the job to convert the given RGBA
   color channels into a generic packed 32 bit format as it is used internally
   by the Graphics Engine. */
unsigned int EwPackColor( int aRed, int aGreen, int aBlue, int aAlpha )
{
  #ifdef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int a = aAlpha + 1;

    aRed   = (( aRed   * a ) & 0x0000FF00 ) >> 8;
    aGreen = (( aGreen * a ) & 0x0000FF00 ) >> 8;
    aBlue  = (( aBlue  * a ) & 0x0000FF00 ) >> 8;
  #endif

  return aRed | ( aGreen << 8  ) | ( aBlue  << 16 ) | ( aAlpha << 24 );
}


/* The following EwPackClutEntry() function has the job to convert the given
   RGBA color channels into a 32 bit wide clut entry as it is used by Index8
   surfaces internally. */
unsigned int EwPackClutEntry( int aRed, int aGreen, int aBlue, int aAlpha )
{
  #ifdef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int a = aAlpha + 1;

    aRed   = (( aRed   * a ) & 0x0000FF00 ) >> 8;
    aGreen = (( aGreen * a ) & 0x0000FF00 ) >> 8;
    aBlue  = (( aBlue  * a ) & 0x0000FF00 ) >> 8;
  #endif

  /* Compose the resulting clut entry - note, the value corresponds exactly
     to the internal pixel representation. In this manner the clut entry can
     be involved in all color calculations - witout any per pixel conversion! */
  return  EW_SHIFT( aRed,   -EW_COLOR_CHANNEL_BIT_OFFSET_RED   ) |
          EW_SHIFT( aGreen, -EW_COLOR_CHANNEL_BIT_OFFSET_GREEN ) |
          EW_SHIFT( aBlue,  -EW_COLOR_CHANNEL_BIT_OFFSET_BLUE  ) |
          EW_SHIFT( aAlpha, -EW_COLOR_CHANNEL_BIT_OFFSET_ALPHA );
}


/* Draw a single pixel with solid color. The new pixel will overwrite the
   existing pixel within the destination. */
void EwSetPixelSolid( XSurfaceMemory* aDst, int aX, int aY,
  XGradient* aGradient )
{
  unsigned int mask1 = 0x0FF00000;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Calculate the address of the pixel */
  unsigned int* dstPtr = (unsigned int*)((char*)aDst->Pixel1 + 
    ( aY * aDst->Pitch1Y ) + ( aX * sizeof( unsigned int )));

  *dstPtr = src;
}


/* Draw a single pixel with solid color. The new pixel will be alpha blended
   with the existing pixel of the destination. */
void EwSetPixelSolidBlend( XSurfaceMemory* aDst, int aX, int aY,
  XGradient* aGradient )
{
  unsigned int mask1 = 0x0FF00000;
  unsigned int mask2 = 0x00FF00FF;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Calculate the address of the pixel */
  unsigned int* dstPtr = (unsigned int*)((char*)aDst->Pixel1 + 
    ( aY * aDst->Pitch1Y ) + ( aX * sizeof( unsigned int )));

  /* Combine the source pixel with the destination pixel by alpha blending */
  BLEND_NATIVE( *dstPtr, src );
}


/* Fill a horizontal pixel row with a solid color. The new pixel will overwrite
   the existing pixel within the destination. */
void EwFillRowSolid( XSurfaceMemory* aDst, int aWidth, XGradient* aGradient )
{
  unsigned int  mask1  = 0x0FF00000;
  unsigned int* dstPtr = aDst->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
    *dstPtr++ = src;
}


/* Fill a horizontal pixel row with a solid color. The new pixel will be alpha
   blended with the existing pixel of the destination. */
void EwFillRowSolidBlend( XSurfaceMemory* aDst, int aWidth, 
  XGradient* aGradient )
{
  unsigned int  mask1  = 0x0FF00000;
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the gradient color with the destination pixel by alpha 
       blending */
    BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with a color gradient. The new pixel will
   overwrite the existing pixel within the destination. */
void EwFillRowGradient( XSurfaceMemory* aDst, int aWidth, XGradient* aGradient )
{
  unsigned int  mask1  = 0x0FF00000;
  unsigned int* dstPtr = aDst->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the 4 channels to a single color value */
    unsigned int src = GRADIENT_TO_COLOR( r, g, b, a );

    *dstPtr++ = src;

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
  }
}


/* Fill a horizontal pixel row with a color gradient. The new pixel will be
   alpha blended with the existing pixel of the destination. */
void EwFillRowGradientBlend( XSurfaceMemory* aDst, int aWidth,
  XGradient* aGradient )
{
  unsigned int  mask1  = 0x0FF00000;
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the 4 channels to a single color value */
    unsigned int src = GRADIENT_TO_COLOR( r, g, b, a );

    /* Combine the gradient color with the destination pixel by alpha 
       blending */
    BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyNativeRow( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
    *dstPtr++ = *MAP_UINT32( srcPtr++ );
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyNativeRowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyNativeRowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int  mask2  = 0x00FF00FF;
#endif  
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  unsigned int  grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyNativeRowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  unsigned int  grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src  = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwCopyNativeRowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int  mask2  = 0x00FF00FF;
#endif
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  int           a      = aGradient->A0;
  int           as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 20 ) + 1;
    unsigned int src  = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyNativeRowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  int           a      = aGradient->A0;
  int           as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src  = *MAP_UINT32( srcPtr++ );
    unsigned int grdA = ( a >> 20 ) + 1;

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
    a += as;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyIndex8Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
    *dstPtr++ = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyIndex8RowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyIndex8RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int  mask2  = 0x00FF00FF;
#endif
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );
    
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyIndex8RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwCopyIndex8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int  mask2  = 0x00FF00FF;
#endif
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            a      = aGradient->A0;
  int            as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 20 ) + 1;
    unsigned int src  = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    dstPtr++;
    a += as;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyIndex8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            a      = aGradient->A0;
  int            as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src  = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));
    unsigned int grdA = ( a >> 20 ) + 1;

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
    a += as;
  }
}


/* Fill a horizontal pixel row with modulated solid color. The new pixel will
   overwrite the existing pixel within the destination. */
void EwCopyAlpha8RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2  = 0x00FF00FF;
#endif  
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );

    /* Modulate the gradient color with the alpha8 value and store the result
       in the destination */
    COPY_ALPHA8( *dstPtr, srcA, grd );

    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated solid color. The new pixel will
   be alpha blended with the existing pixel of the destination. */
void EwCopyAlpha8RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                        aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );

    /* Combine the gradient color with the destination pixel by alpha
       blending */
    if ( srcA )
      BLEND_ALPHA8( *dstPtr, srcA, grd );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated gradient color. The new pixel will
   overwrite the existing pixel within the destination. */
void EwCopyAlpha8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2  = 0x00FF00FF;
#endif
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );
    unsigned int grd  = 0;

    /* Calculate the gradient value for visible pixel only */
    if ( srcA > 0 )
      grd = GRADIENT_TO_COLOR( r, g, b, a );

    /* Modulate the gradient color with the alpha8 value and store the result
       in the destination */
    COPY_ALPHA8( *dstPtr, srcA, grd );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated gradient color. The new pixel will
   be alpha blended with the existing pixel of the destination. */
void EwCopyAlpha8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );
    unsigned int grd  = 0;

    /* Calculate the gradient value for visible pixel only */
    if ( srcA > 0 )
    {
      grd = GRADIENT_TO_COLOR( r, g, b, a );

      /* Combine the gradient color with the destination pixel by alpha 
         blending */
      BLEND_ALPHA8( *dstPtr, srcA, grd );
    }

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyRGB565Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT16( srcPtr++ );

    /* Convert the RGB565 pixel in the native format */
    RGB565_2_NATIVE_OPAQUE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwCopyRGB565RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int  mask2  = 0x00FF00FF;
#endif  
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyRGB565RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 20 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src  = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwCopyRGB565RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int    mask2  = 0x00FF00FF;
#endif
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 20 ) + 1;
    unsigned int src  = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwCopyRGB565RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src  = *MAP_UINT16( srcPtr++ );
    unsigned int grdA = ( a >> 20 ) + 1;

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
    a += as;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwWarpNativeRow( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  int           pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Map the calculated source pixel to the current destination pixel */
    *dstPtr++ = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwWarpNativeRowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int  mask2   = 0x00FF00FF;
  unsigned int  mask3   = 0xFF00FF00;
  unsigned int* dstPtr  = aDst->Pixel1;
  unsigned int* srcPtr  = aSrc->Pixel1;
  int           pitch   = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( *dstPtr, srcPtr, pitch, x, y );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwWarpNativeRowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int  mask2  = 0x00FF00FF;
  unsigned int* dstPtr = aDst->Pixel1;
  unsigned int* srcPtr = aSrc->Pixel1;
  int           pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will be alpha-blended with the existing pixel in the
   destination memory. */
void EwWarpNativeRowFilterBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned int*  srcPtr  = aSrc->Pixel1;
  int            pitch   = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( src, srcPtr, pitch, x, y );

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwWarpNativeRowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2  = 0x00FF00FF;
#endif  
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned int*  srcPtr = aSrc->Pixel1;
  int            pitch  = aSrc->Pitch1Y >> 2;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better
   output quality interpolate source pixel values by the bi-linear filter.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwWarpNativeRowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned int*  srcPtr  = aSrc->Pixel1;
  int            pitch   = aSrc->Pitch1Y >> 2;
  unsigned int   grdA    = ( aGradient->A0 >> 20 ) + 1;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwWarpNativeRowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned int*  srcPtr = aSrc->Pixel1;
  int            pitch  = aSrc->Pitch1Y >> 2;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwWarpNativeRowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned int*  srcPtr  = aSrc->Pixel1;
  int            pitch   = aSrc->Pitch1Y >> 2;
  unsigned int   grdA    = ( aGradient->A0 >> 20 ) + 1;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwWarpIndex8Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Map the calculated source pixel to the current destination pixel */
    *dstPtr++ = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwWarpIndex8RowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  unsigned int*  clut    = aSrc->Clut;
  int            pitch   = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( *dstPtr, srcPtr, clut, pitch, x, y );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will be alpha-blended with the existing pixel in the 
   destination memory. */
void EwWarpIndex8RowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will be alpha-blended with the existing pixel in the
   destination memory. */
void EwWarpIndex8RowFilterBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  unsigned int*  clut    = aSrc->Clut;
  int            pitch   = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( src, srcPtr, clut, pitch, x, y );

    /* Combine the source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE( *dstPtr, src )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwWarpIndex8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2  = 0x00FF00FF;
#endif
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            pitch  = aSrc->Pitch1Y;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will overwrite the existing pixel in the destination memory. */
void EwWarpIndex8RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  unsigned int*  clut    = aSrc->Clut;
  int            pitch   = aSrc->Pitch1Y;
  unsigned int   grdA    = ( aGradient->A0 >> 20 ) + 1;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( src, srcPtr, clut, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwWarpIndex8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  unsigned int*  clut   = aSrc->Clut;
  int            pitch  = aSrc->Pitch1Y;
  unsigned int   grdA   = ( aGradient->A0 >> 20 ) + 1;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwWarpIndex8RowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int   mask3   = 0xFF00FF00;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  unsigned int*  clut    = aSrc->Clut;
  int            pitch   = aSrc->Pitch1Y;
  unsigned int   grdA    = ( aGradient->A0 >> 20 ) + 1;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( src, srcPtr, clut, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_NATIVE_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwWarpAlpha8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2  = 0x00FF00FF;
#endif
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  int            pitch  = aSrc->Pitch1Y;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int   grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    unsigned int src = *MAP_UINT8( srcPtr + ( y * pitch ) + x );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x, y );

    /* Modulate the got color value with the alpha8 value and store the result
       in the destination */
    COPY_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will overwrite the existing pixel in the destination memory. */
void EwWarpAlpha8RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int   mask2   = 0x00FF00FF;
#endif
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  int            pitch   = aSrc->Pitch1Y;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int   grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x >> 4, y >> 4 );

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick alpha8 values of 4 pixel and average the resulting color */
    BILINEAR_ALPHA8( src, srcPtr, pitch, x, y );

    /* Modulate the got color value with the alpha8 value and store the result
       in the destination */
    COPY_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwWarpAlpha8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask2  = 0x00FF00FF;
  unsigned int*  dstPtr = aDst->Pixel1;
  unsigned char* srcPtr = aSrc->Pixel1;
  int            pitch  = aSrc->Pitch1Y;
  int            solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int   grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT8( srcPtr + ( y * pitch ) + x );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x, y );

    /* Modulate the got color value with the alpha8 value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_ALPHA8( *dstPtr, src, grd )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwWarpAlpha8RowFilterGradientBlend( XSurfaceMemory* aDst, 
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int   mask1   = 0x0FF00000;
  unsigned int   mask2   = 0x00FF00FF;
  unsigned int*  dstPtr  = aDst->Pixel1;
  unsigned char* srcPtr  = aSrc->Pixel1;
  int            pitch   = aSrc->Pitch1Y;
  int            solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int   grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x >> 4, y >> 4 );

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick alpha8 values of 4 pixel and average the resulting color */
    BILINEAR_ALPHA8( src, srcPtr, pitch, x, y );

    /* Modulate the got color value with the alpha8 value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_ALPHA8( *dstPtr, src, grd )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwWarpRGB565Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Map the calculated source pixel to the current destination pixel */
    unsigned int src = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Convert the RGB565 pixel in the native format */
    RGB565_2_NATIVE_OPAQUE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
    aS += aSS;
    aT += aTS;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwWarpRGB565RowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask2L  = 0xF81F;
  unsigned int    mask2H  = 0x07E0;
  unsigned int*   dstPtr  = aDst->Pixel1;
  unsigned short* srcPtr  = aSrc->Pixel1;
  int             pitch   = aSrc->Pitch1Y >> 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( src, srcPtr, pitch, x, y );

    /* Convert the RGB565 pixel in the native format */
    RGB565_2_NATIVE_OPAQUE( *dstPtr, src );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwWarpRGB565RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int    mask2  = 0x00FF00FF;
#endif  
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 20 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    unsigned int src = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better
   output quality interpolate source pixel values by the bi-linear filter.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwWarpRGB565RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1   = 0x0FF00000;
#ifdef EW_PREMULTIPLY_COLOR_CHANNELS
  unsigned int    mask2   = 0x00FF00FF;
#endif
  unsigned int    mask2L  = 0xF81F;
  unsigned int    mask2H  = 0x07E0;
  unsigned int*   dstPtr  = aDst->Pixel1;
  unsigned short* srcPtr  = aSrc->Pixel1;
  int             pitch   = aSrc->Pitch1Y >> 1;
  unsigned int    grdA    = ( aGradient->A0 >> 20 ) + 1;
  int             solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations 
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwWarpRGB565RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int*   dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 20 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Pick the calculated source pixel */
    unsigned int src = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_RGB565_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwWarpRGB565RowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1   = 0x0FF00000;
  unsigned int    mask2   = 0x00FF00FF;
  unsigned int    mask2L  = 0xF81F;
  unsigned int    mask2H  = 0x07E0;
  unsigned int*   dstPtr  = aDst->Pixel1;
  unsigned short* srcPtr  = aSrc->Pixel1;
  int             pitch   = aSrc->Pitch1Y >> 1;
  unsigned int    grdA    = ( aGradient->A0 >> 20 ) + 1;
  int             solid   = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..255 to 0..256 to allow shift operations
         instead of divisions */
      grdA++;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    BLEND_RGB565_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Draw a single pixel with solid color. The new pixel will overwrite the
   existing pixel within the destination. */
void EwScreenSetPixelSolid( XSurfaceMemory* aDst, int aX, int aY,
  XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask1F = 0x0000001F;
  unsigned int   mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int mask2L = 0x0000F81F;
    unsigned int mask2H = 0x000007E0;
  #endif

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Calculate the address of the pixel */
  unsigned short* dstPtr = (unsigned short*)((char*)aDst->Pixel1 + 
    ( aY * aDst->Pitch1Y ) + ( aX * sizeof( unsigned short )));

  /* Save the intermediate RGBA565 value in separate color and alpha planes */
  SCREEN_COPY_NATIVE( *dstPtr, src );
}


/* Draw a single pixel with solid color. The new pixel will be alpha blended
   with the existing pixel of the destination. */
void EwScreenSetPixelSolidBlend( XSurfaceMemory* aDst, int aX, int aY,
  XGradient* aGradient )
{
  unsigned int mask1  = 0x0FF00000;
  unsigned int mask1F = 0x0000001F;
  unsigned int mask3F = 0x0000003F;
  unsigned int mask2L = 0x0000F81F;
  unsigned int mask2H = 0x000007E0;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Calculate the address of the pixel */
  unsigned short* dstPtr = (unsigned short*)((char*)aDst->Pixel1 + 
    ( aY * aDst->Pitch1Y ) + ( aX * sizeof( unsigned short )));

  /* Combine the source pixel with the destination pixel by alpha blending */
  SCREEN_BLEND_NATIVE( *dstPtr, src );
}


/* Fill a horizontal pixel row with a solid color. The new pixel will overwrite
   the existing pixel within the destination. */
void EwScreenFillRowSolid( XSurfaceMemory* aDst, int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask1F = 0x0000001F;
  unsigned int   mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int mask2L = 0x0000F81F;
    unsigned int mask2H = 0x000007E0;
  #endif

  /* Combine the 4 channels to a single color value */
  unsigned int   src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                           aGradient->B0, aGradient->A0 );
  unsigned int*  dstPtr = aDst->Pixel1;

  /* Convert RGBA8888 to RGB565 */
  SCREEN_COPY_NATIVE( src, src );

  /* The first pixel lies on an unaligned memory address? */
  if (((unsigned long)dstPtr & 3 ) && ( aWidth > 0 ))
  {
    *(unsigned short*)dstPtr = (unsigned short)src;
    dstPtr = (unsigned int*)((unsigned short*)dstPtr + 1 );
    aWidth--;
  }

  /* The last pixel starts in a new dword */
  if (( aWidth > 0 ) && ( aWidth & 0x1 ))
    *((unsigned short*)dstPtr + --aWidth ) = (unsigned short)src;

  /* The remaining are entire dwords which ... */
  aWidth >>= 1;
  src = src | ( src << 16 );

  /* ... can be copied dword-wise very fast */
  while ( aWidth-- > 0 )
    *dstPtr++ = src;
}


/* Fill a horizontal pixel row with a solid color. The new pixel will be alpha
   blended with the existing pixel of the destination. */
void EwScreenFillRowSolidBlend( XSurfaceMemory* aDst, int aWidth, 
  XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int src = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                         aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the gradient color with the destination pixel by alpha 
       blending */
    SCREEN_BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with a color gradient. The new pixel will
   overwrite the existing pixel within the destination. */
void EwScreenFillRowGradient( XSurfaceMemory* aDst, int aWidth, XGradient* aGradient )
{
  unsigned int   mask1  = 0x0FF00000;
  unsigned int   mask1F = 0x0000001F;
  unsigned int   mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int mask2L = 0x0000F81F;
    unsigned int mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the 4 channels to a single color value */
    unsigned int src = GRADIENT_TO_COLOR( r, g, b, a );

    /* Save the intermediate RGBA565 value in separate color  */
    SCREEN_COPY_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with a color gradient. The new pixel will be
   alpha blended with the existing pixel of the destination. */
void EwScreenFillRowGradientBlend( XSurfaceMemory* aDst, int aWidth,
  XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* Combine the 4 channels to a single color value */
    unsigned int src = GRADIENT_TO_COLOR( r, g, b, a );

    /* Combine the gradient color with the destination pixel by alpha 
       blending */
    SCREEN_BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyNativeRow( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    SCREEN_COPY_NATIVE( *dstPtr, src );
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyNativeRowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    /* Combine the source pixel with the destination pixel by alpha blending */
    SCREEN_BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyNativeRowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyNativeRowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyNativeRowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 22 ) + 1;
    unsigned int src  = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyNativeRowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 22 ) + 1;
    unsigned int src  = *MAP_UINT32( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyIndex8Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    SCREEN_COPY_NATIVE( *dstPtr, src );
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyIndex8RowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    SCREEN_BLEND_NATIVE( *dstPtr, src );
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyIndex8RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyIndex8RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyIndex8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 22 ) + 1;
    unsigned int src  = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyIndex8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 22 ) + 1;
    unsigned int src  = *MAP_UINT32( clut + *MAP_UINT8( srcPtr++ ));

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated solid color. The new pixel will
   overwrite the existing pixel within the destination. */
void EwScreenCopyAlpha8RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                        aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT8( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated solid color. The new pixel will
   be alpha blended with the existing pixel of the destination. */
void EwScreenCopyAlpha8RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;

  /* Combine the 4 channels to a single color value */
  unsigned int grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0,
                                        aGradient->B0, aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT8( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated gradient color. The new pixel will
   overwrite the existing pixel within the destination. */
void EwScreenCopyAlpha8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );
    unsigned int grd  = 0;

    /* Calculate the gradient value for visible pixel only */
    if ( srcA > 0 )
      grd = GRADIENT_TO_COLOR( r, g, b, a );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_ALPHA8( *dstPtr, srcA, grd );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with modulated gradient color. The new pixel will
   be alpha blended with the existing pixel of the destination. */
void EwScreenCopyAlpha8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;

  /* Load the color gradient parameters */
  int r  = aGradient->R0;
  int g  = aGradient->G0;
  int b  = aGradient->B0;
  int a  = aGradient->A0;
  int rs = aGradient->R1;
  int gs = aGradient->G1;
  int bs = aGradient->B1;
  int as = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int srcA = *MAP_UINT8( srcPtr++ );
    unsigned int grd  = 0;

    /* Calculate the gradient value for visible pixel only */
    if ( srcA > 0 )
      grd = GRADIENT_TO_COLOR( r, g, b, a );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_ALPHA8( *dstPtr, srcA, grd );

    /* Continue with the next pixel */
    r += rs;
    g += gs;
    b += bs;
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row without any additional opacity values. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyRGB565Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  XGradient* aGradient )
{
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
    *dstPtr++ = *MAP_UINT16( srcPtr++ );
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyRGB565RowSolid( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional solid opacity value. The new
   pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyRGB565RowSolidBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    SCREEN_BLEND_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The 
   new pixel will overwrite the existing pixel within the destination. */
void EwScreenCopyRGB565RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int grdA = ( a >> 22 ) + 1;
    unsigned int src  = *MAP_UINT16( srcPtr++ );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Copy a horizontal pixel row with an additional gradient opacity value. The
   new pixel will be alpha blended with the existing pixel of the destination. */
void EwScreenCopyRGB565RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, XGradient* aGradient )
{
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             a      = aGradient->A0;
  int             as     = aGradient->A1;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int  src  = *MAP_UINT16( srcPtr++ );
    unsigned int  grdA = ( a >> 22 ) + 1;

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    SCREEN_BLEND_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    a += as;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpNativeRow( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    SCREEN_COPY_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwScreenWarpNativeRowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int dst;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( dst, srcPtr, pitch, x, y );
    SCREEN_COPY_NATIVE( *dstPtr, dst );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwScreenWarpNativeRowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    SCREEN_BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will be alpha-blended with the existing pixel in the
   destination memory. */
void EwScreenWarpNativeRowFilterBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int dst;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( dst, srcPtr, pitch, x, y );
    SCREEN_BLEND_NATIVE( *dstPtr, dst );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwScreenWarpNativeRowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better
   output quality interpolate source pixel values by the bi-linear filter.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpNativeRowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwScreenWarpNativeRowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwScreenWarpNativeRowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned int*   srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 2;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_NATIVE( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpIndex8Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    SCREEN_COPY_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwScreenWarpIndex8RowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;

  #ifndef EW_PREMULTIPLY_COLOR_CHANNELS
    unsigned int  mask2L = 0x0000F81F;
    unsigned int  mask2H = 0x000007E0;
  #endif

  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int dst;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( dst, srcPtr, clut, pitch, x, y );
    SCREEN_COPY_NATIVE( *dstPtr, dst );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will be alpha-blended with the existing pixel in the 
   destination memory. */
void EwScreenWarpIndex8RowBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    SCREEN_BLEND_NATIVE( *dstPtr, src );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will be alpha-blended with the existing pixel in the
   destination memory. */
void EwScreenWarpIndex8RowFilterBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;

  EW_UNUSED_ARG( aGradient );

  while ( aWidth-- > 0 )
  {
    unsigned int dst;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( dst, srcPtr, clut, pitch, x, y );
    SCREEN_BLEND_NATIVE( *dstPtr, dst );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwScreenWarpIndex8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpIndex8RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( src, srcPtr, clut, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwScreenWarpIndex8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT32( clut + *MAP_UINT8( srcPtr + ( y * pitch ) + x ));

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwScreenWarpIndex8RowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2  = 0x00FF00FF;
  unsigned int    mask3  = 0xFF00FF00;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  unsigned int*   clut   = aSrc->Clut;
  int             pitch  = aSrc->Pitch1Y;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_INDEX8( src, srcPtr, clut, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_NATIVE_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwScreenWarpAlpha8RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int    grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT8( srcPtr + ( y * pitch ) + x );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpAlpha8RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int    grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x >> 4, y >> 4 );

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick alpha8 values of 4 pixel and average the resulting color */
    BILINEAR_ALPHA8( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwScreenWarpAlpha8RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int    grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT8( srcPtr + ( y * pitch ) + x );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwScreenWarpAlpha8RowFilterGradientBlend( XSurfaceMemory* aDst, 
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask1F = 0x0000001F;
  unsigned int    mask3F = 0x0000003F;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x000007E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned char*  srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;
  unsigned int    grd;

  /* Combine the 4 channels to a single color value */
  grd = GRADIENT_TO_COLOR( aGradient->R0, aGradient->G0, aGradient->B0,
                           aGradient->A0 );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the color value corresponding to the pixel coordinates */
    if ( !solid )
      PICK_FROM_COLOR_GRADIENT( grd, aGradient, x >> 4, y >> 4 );

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick alpha8 values of 4 pixel and average the resulting color */
    BILINEAR_ALPHA8( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_ALPHA8( *dstPtr, src, grd );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpRGB565Row( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, int aWidth,
  int aS, int aT, int aSS, int aTS, int aSrcWidth, int aSrcHeight,
  XGradient* aGradient )
{
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int y = (( aT >> 12 ) * aSrcHeight ) >> 16;

    /* Map the calculated source pixel to the current destination pixel */
    *dstPtr++ = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface.
   For better output quality interpolate source pixel values by the bi-linear
   filter. The new pixel will overwrite the existing pixel in the destination
   memory. */
void EwScreenWarpRGB565RowFilter( XSurfaceMemory* aDst, XSurfaceMemory* aSrc, 
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;

  EW_UNUSED_ARG( aGradient );

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision and move to
       the top-left origin of a pixel */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 ) - 8;
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 ) - 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( *dstPtr, srcPtr, pitch, x, y );

    /* Continue with the next pixel */
    dstPtr++;
    aS += aSS;
    aT += aTS;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will overwrite the existing pixel in the destination memory. */
void EwScreenWarpRGB565RowGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better
   output quality interpolate source pixel values by the bi-linear filter.
   The new pixel will overwrite the existing pixel in the destination memory. */
void EwScreenWarpRGB565RowFilterGradient( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_COPY_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. The new pixel
   will be alpha-blended with the existing pixel in the destination memory. */
void EwScreenWarpRGB565RowGradientBlend( XSurfaceMemory* aDst, XSurfaceMemory* aSrc,
  int aWidth, int aS, int aT, int aSS, int aTS, int aSrcWidth,
  int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2L = 0xF81F;
  unsigned int    mask2H = 0x07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int          x   = (( aS >> 12 ) * aSrcWidth  ) >> 16;
    int          y   = (( aT >> 12 ) * aSrcHeight ) >> 16;
    unsigned int src = *MAP_UINT16( srcPtr + ( y * pitch ) + x );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x, y );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Modulate the got source pixel with the opacity value and combine the
       source pixel with the destination pixel by alpha blending */
    SCREEN_BLEND_RGB565_OPACITY( *dstPtr, src, grdA )

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Fill a horizontal pixel row with transformed content of the source surface
   and modulate the pixel with opacity values from a gradient. For better output
   quality interpolate source pixel values by the bi-linear filter. The new
   pixel will be alpha-blended with the existing pixel in the destination
   memory. */
void EwScreenWarpRGB565RowFilterGradientBlend( XSurfaceMemory* aDst,
  XSurfaceMemory* aSrc, int aWidth, int aS, int aT, int aSS, int aTS,
  int aSrcWidth, int aSrcHeight, XGradient* aGradient )
{
  unsigned int    mask1  = 0x0FF00000;
  unsigned int    mask2L = 0x0000F81F;
  unsigned int    mask2H = 0x00FC07E0;
  unsigned short* dstPtr = aDst->Pixel1;
  unsigned short* srcPtr = aSrc->Pixel1;
  int             pitch  = aSrc->Pitch1Y >> 1;
  unsigned int    grdA   = ( aGradient->A0 >> 22 ) + 1;
  int             solid  = !aGradient->IsVertical && !aGradient->IsHorizontal;

  /* Repeat until the entire row is drawn */
  while ( aWidth-- > 0 )
  {
    unsigned int src;

    /* From the interpolated s and t coefficients get the resulting source
       surface coordinates. Preserve the calculation precision */
    int x = ((( aS >> 12 ) * aSrcWidth  ) >> 12 );
    int y = ((( aT >> 12 ) * aSrcHeight ) >> 12 );

    /* Calculate the opacity value corresponding to the pixel coordinates */
    if ( !solid )
    {
      PICK_FROM_OPACITY_GRADIENT( grdA, aGradient, x >> 4, y >> 4 );

      /* Stretch the value from range 0..63 to 0..64 to allow shift operations 
         instead of divisions */
      grdA = ( grdA >> 2 ) + 1;
    }

    /* Move to the top-left origin of a pixel */
    x -= 8;
    y -= 8;

    /* Pick color values of 4 pixel and average the resulting color */
    BILINEAR_RGB565( src, srcPtr, pitch, x, y );

    /* Modulate the got source pixel with the opacity value and store the result
       in the destination */
    SCREEN_BLEND_RGB565_OPACITY( *dstPtr, src, grdA );

    /* Continue with the next pixel */
    aS += aSS;
    aT += aTS;
    dstPtr++;
  }
}


/* Following 'solid' warp operations are not implemented as individual functions. Instead
   they are redirected to the corresponding 'gradient' version */
EW_REDIRECT_WARP_FUNC( EwWarpNativeRowSolid,            EwWarpNativeRowGradient            )
EW_REDIRECT_WARP_FUNC( EwWarpNativeRowSolidBlend,       EwWarpNativeRowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwWarpNativeRowFilterSolid,      EwWarpNativeRowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwWarpNativeRowFilterSolidBlend, EwWarpNativeRowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwWarpIndex8RowSolid,            EwWarpIndex8RowGradient            )
EW_REDIRECT_WARP_FUNC( EwWarpIndex8RowSolidBlend,       EwWarpIndex8RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwWarpIndex8RowFilterSolid,      EwWarpIndex8RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwWarpIndex8RowFilterSolidBlend, EwWarpIndex8RowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwWarpAlpha8RowSolid,            EwWarpAlpha8RowGradient            )
EW_REDIRECT_WARP_FUNC( EwWarpAlpha8RowSolidBlend,       EwWarpAlpha8RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwWarpAlpha8RowFilterSolid,      EwWarpAlpha8RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwWarpAlpha8RowFilterSolidBlend, EwWarpAlpha8RowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwWarpRGB565RowSolid,            EwWarpRGB565RowGradient            )
EW_REDIRECT_WARP_FUNC( EwWarpRGB565RowSolidBlend,       EwWarpRGB565RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwWarpRGB565RowFilterSolid,      EwWarpRGB565RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwWarpRGB565RowFilterSolidBlend, EwWarpRGB565RowFilterGradientBlend )

EW_REDIRECT_WARP_FUNC( EwScreenWarpNativeRowSolid,            EwScreenWarpNativeRowGradient            )
EW_REDIRECT_WARP_FUNC( EwScreenWarpNativeRowSolidBlend,       EwScreenWarpNativeRowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwScreenWarpNativeRowFilterSolid,      EwScreenWarpNativeRowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwScreenWarpNativeRowFilterSolidBlend, EwScreenWarpNativeRowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwScreenWarpIndex8RowSolid,            EwScreenWarpIndex8RowGradient            )
EW_REDIRECT_WARP_FUNC( EwScreenWarpIndex8RowSolidBlend,       EwScreenWarpIndex8RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwScreenWarpIndex8RowFilterSolid,      EwScreenWarpIndex8RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwScreenWarpIndex8RowFilterSolidBlend, EwScreenWarpIndex8RowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwScreenWarpAlpha8RowSolid,            EwScreenWarpAlpha8RowGradient            )
EW_REDIRECT_WARP_FUNC( EwScreenWarpAlpha8RowSolidBlend,       EwScreenWarpAlpha8RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwScreenWarpAlpha8RowFilterSolid,      EwScreenWarpAlpha8RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwScreenWarpAlpha8RowFilterSolidBlend, EwScreenWarpAlpha8RowFilterGradientBlend )
EW_REDIRECT_WARP_FUNC( EwScreenWarpRGB565RowSolid,            EwScreenWarpRGB565RowGradient            )
EW_REDIRECT_WARP_FUNC( EwScreenWarpRGB565RowSolidBlend,       EwScreenWarpRGB565RowGradientBlend       )
EW_REDIRECT_WARP_FUNC( EwScreenWarpRGB565RowFilterSolid,      EwScreenWarpRGB565RowFilterGradient      )
EW_REDIRECT_WARP_FUNC( EwScreenWarpRGB565RowFilterSolidBlend, EwScreenWarpRGB565RowFilterGradientBlend )


/* pba */

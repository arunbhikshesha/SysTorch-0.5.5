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
*   This module implements the attributed text processing top level API of the
*   Embedded Wizard Graphics Engine EWGFX. Unlike simple text output, attributed
*   text may appear with multiple fonts, colors and images - even like HTML.
*
*   The appearance of the text is determinated by the text attributes. These
*   attributes describe what to do with the text, which font/color should be
*   used or what images should be displayed together with the text.
*
*   The attributes are always enclosed in '{ .... }' braces. For example, the
*   attribute for color selection looks like this:
*
*     "The text with two {clr1} different {clr0} colors."
*
*   The attributed strings provide a wide range of attributes for paragraphs,
*   text column layout, vertical and hoprizontal alignment, background and
*   borders colors, embedded images, font selection, hyper-links and more.
*
*   The attributed strings also provides an automatic line wrap, so multiline
*   text output is possible.
*
*   The automatic line wrap is performed primarily between words. Additionally
*   the 'blind wrap' and 'silent wrap' are possible. These are controlled by
*   the special characters '^' (for the blind wrap) and '~' (for silent wrap).
*
*   The both special characters '^' and '~' are usually ignored and they are not
*   displayed until the line wrap took place at its position. Then the '~' sign
*   is converted into the hyphen '-'. The blind wrap in contrast breaks a line
*   only and remains invisible. These both special characters provide a flexibel
*   way to output flow text. The silent wrap is also possible at the soft-hyphen
*   character \x00AD unicode.
*
*   Beside the automatic text wrap, an explicit linefeed is possible when the
*   '\n' sign has been found in the string.
*
*   In order to be able to output the special characters as regular signs, the
*   character '%' can be applied in front of the affected sign to convert it
*   to a regular sign.
*
*   This module also provides functionality for so called flow text, which is
*   a kind of multiline text without any attributes -> the entire text uses
*   one font and one color only. Only the special control characters '^', '~'
*   \x00AD, '\n' and '%' are supported by the flow text. The flow text can be
*   seen as the minor and slim version of the attributed string.
*
* SUBROUTINES:
*   EwParseFlowString
*   EwGetFlowTextAdvance
*   EwCreateAttrSet
*   EwFreeAttrSet
*   EwGetNoOfAttrFonts
*   EwGetNoOfAttrBitmaps
*   EwGetNoOfAttrColors
*   EwGetAttrFont
*   EwGetAttrBitmap
*   EwGetAttrColor
*   EwSetAttrFont
*   EwSetAttrBitmap
*   EwSetAttrColor
*   EwParseAttrString
*   EwFreeAttrString
*   EwDrawAttrText
*   EwGetAttrTextSize
*   EwIsAttrTextRTL
*   EwGetNoOfAttrLinks
*   EwGetAttrLinkName
*   EwGetNoOfAttrLinkRegions
*   EwGetAttrLinkRect
*   EwGetAttrLinkBaseline
*
*******************************************************************************/

#include "ewrte.h"
#include "ewgfxdriver.h"
#include "ewgfxcore.h"
#include "ewgfxtasks.h"
#include "ewgfxres.h"
#include "ewgfx.h"


/* Helper macros for storing and accessing of signed numbers in the parsed
   attributed string */
#define WORDNUM( aChar ) ((signed short) (aChar))
#define NUMWORD( aNum  ) ((unsigned short)(aNum))
#define BUFSIZE( aDest ) ((int)((aDest)->Ptr - (aDest)->Buffer ))

/* This helper macro returns the maximal one of the both arguments */
#ifdef MAX
  #undef MAX
#endif
#define MAX( a, b ) (((a) > (b))? (a) : (b))


/* This helper macro returns the minimum one of the both arguments */
#ifdef MIN
  #undef MIN
#endif
#define MIN( a, b ) (((a) < (b))? (a) : (b))


/* Following are the possible types of entities within an attributed text
   paragraph. */
#define TYPE_GLYPH           1
#define TYPE_GLYPH_UL        2
#define TYPE_IMAGE_MIDDLE    3
#define TYPE_IMAGE_BOTTOM    4
#define TYPE_IMAGE_TOP       5

/* Helper macros for accessing the attribute information associated to entities
   within an attributed text paragraph. Every entity manages its attributes as a
   32-bit DWORD value. */
#define GET_ATTR_TYPE( aA )   ((aA)  & 0x0000000F)
#define GET_ATTR_COLOR( aA )  (((aA) & 0x00000FF0) >> 4  )
#define GET_ATTR_FONT( aA )   (((aA) & 0x000FF000) >> 12 )
#define GET_ATTR_BITMAP( aA ) (((aA) & 0x000FF000) >> 12 )
#define GET_ATTR_FRAME( aA )  (((aA) & 0xFFF00000) >> 20 )

/* Helper macros to create attribute information for entities within the
   attributed text paragraph. Every entity manages its attributes as a
   32-bit DWORD value. */
#define ATTR_GLYPH( aColor, aFont, aUnderlined )                              \
  (( aUnderlined? TYPE_GLYPH_UL : TYPE_GLYPH ) |                              \
  ((aColor) << 4 ) | ((aFont) << 12 ))

#define ATTR_IMAGE( aColor, aBitmap, aFrame, aAlignment )                     \
  ((( aAlignment == 't' )? TYPE_IMAGE_TOP :                                   \
    ( aAlignment == 'b' )? TYPE_IMAGE_BOTTOM : TYPE_IMAGE_MIDDLE ) |          \
    ((aColor) << 4 ) | ((aBitmap) << 12 ) | ((aFrame) << 20 ))


/* XBuffer stores the parsed, preprocessed attributed string. This buffer can
   grow in order to take more attributes. It's used internaly while the strings
   are parsed */
typedef struct
{
  unsigned short*   Buffer;
  unsigned short*   Ptr;
  int               Size;
} XBuffer;


/* Memory usage profiler */
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwMemoryPeak;


/* This function creates a new instance of the internal buffer. It is used to 
   store preprocessed attributed strings temporary */
static XBuffer* AllocBuffer( void );

/* This function frees the buffer and it's entire content */
static void FreeBuffer( XBuffer* aBuffer );

/* This function parses the body block in aString and stores the preprocessed
   attributes in aDest buffer. The function determinates the height occupied by
   the body and returns it in aHeight argument. */
static XChar* Body( XAttrSet* aAttrSet, XChar* aString, int aWidth,
  int aEnableBidiText, XBuffer* aDest, int* aHeight, int* aLastLeading,
  int* aIsRTL, int* aLinkCount, int* aLinkNamesArea );

/* This function executes all fill rectangle drawing statements from aPtr memory
   area and draws the background of the attributed string into the aDst bitmap at
   the position aOffset. */
static void DrawAttrTextBackground( XBitmap* aDst, int aDstFrameNo, 
  XRect aClipRect, XPoint aDstPos, int aOfsX, int aOfsY, XAttrSet* aAttrSet,
  unsigned short* aPtr, unsigned short* aEnd, XBool aBlend,
  XGradient* aOpacity );

/* This function executes all text/image drawing statements from aPtr memory
   area and draws the content of the attributed string into the aDst bitmap at
   the position aOffset. */
static void DrawAttrTextContent( XBitmap* aDst, int aDstFrameNo, XRect aClipRect,
  XPoint aDstPos, int aOfsX, int aOfsY, XAttrSet* aAttrSet, unsigned short* aPtr,
  unsigned short* aEnd, XBool aBlend, XGradient* aOpacity );

/* This function counts the line feed signs in the given string. */
static int CountLineFeeds( XChar* aString );

/* The following function parses the string aString for {lnk:... attributes
   and stores for every found valid attribute an entry in aLinks array and the
   link name in aNames array. */
static void CollectLinks( XChar* aString, XAttrLink* aLinks, XChar* aNames );

/* This function searches recursively the memory area aPtr for link region
   statements and if found updates the corresponding link entries in aLinks. */
static void CollectLinkRegions( XAttrLink* aLinks, int aOfsX, int aOfsY,
  unsigned short* aPtr, unsigned short* aEnd );

/* This function searches in the given area of drawing statements for the link
   region statement with the number aRegionNo and aLinkNo */
static unsigned short* SkipToRegion( int aLinkNo, int aRegionNo, 
  unsigned short* aStartPtr, unsigned short* aEndPtr );


/* Performance counter for performance investigation and debugging only */
EW_PERF_COUNTER( EwParseFlowString,        Graphics_Engine_API )
EW_PERF_COUNTER( EwGetFlowTextAdvance,     Graphics_Engine_API )
EW_PERF_COUNTER( EwCreateAttrSet,          Graphics_Engine_API )
EW_PERF_COUNTER( EwFreeAttrSet,            Graphics_Engine_API )
EW_PERF_COUNTER( EwGetNoOfAttrFonts,       Graphics_Engine_API )
EW_PERF_COUNTER( EwGetNoOfAttrBitmaps,     Graphics_Engine_API )
EW_PERF_COUNTER( EwGetNoOfAttrColors,      Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrFont,            Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrBitmap,          Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrColor,           Graphics_Engine_API )
EW_PERF_COUNTER( EwSetAttrFont,            Graphics_Engine_API )
EW_PERF_COUNTER( EwSetAttrBitmap,          Graphics_Engine_API )
EW_PERF_COUNTER( EwSetAttrColor,           Graphics_Engine_API )
EW_PERF_COUNTER( EwParseAttrString,        Graphics_Engine_API )
EW_PERF_COUNTER( EwFreeAttrString,         Graphics_Engine_API )
EW_PERF_COUNTER( EwDrawAttrText,           Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrTextSize,        Graphics_Engine_API )
EW_PERF_COUNTER( EwIsAttrTextRTL,          Graphics_Engine_API )
EW_PERF_COUNTER( EwGetNoOfAttrLinks,       Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrLinkName,        Graphics_Engine_API )
EW_PERF_COUNTER( EwGetNoOfAttrLinkRegions, Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrLinkRect,        Graphics_Engine_API )
EW_PERF_COUNTER( EwGetAttrLinkBaseline,    Graphics_Engine_API )


/* Map the Graphics Engine API to the corresponding functions if no performance
   counters are active  */
#ifndef EW_PRINT_PERF_COUNTERS
  #define EwParseFlowString_        EwParseFlowString
  #define EwGetFlowTextAdvance_     EwGetFlowTextAdvance
  #define EwCreateAttrSet_          EwCreateAttrSet
  #define EwFreeAttrSet_            EwFreeAttrSet
  #define EwGetNoOfAttrFonts_       EwGetNoOfAttrFonts
  #define EwGetNoOfAttrBitmaps_     EwGetNoOfAttrBitmaps
  #define EwGetNoOfAttrColors_      EwGetNoOfAttrColors
  #define EwGetAttrFont_            EwGetAttrFont
  #define EwGetAttrBitmap_          EwGetAttrBitmap
  #define EwGetAttrColor_           EwGetAttrColor
  #define EwSetAttrFont_            EwSetAttrFont
  #define EwSetAttrBitmap_          EwSetAttrBitmap
  #define EwSetAttrColor_           EwSetAttrColor
  #define EwParseAttrString_        EwParseAttrString
  #define EwFreeAttrString_         EwFreeAttrString
  #define EwDrawAttrText_           EwDrawAttrText
  #define EwGetAttrTextSize_        EwGetAttrTextSize
  #define EwIsAttrTextRTL_          EwIsAttrTextRTL
  #define EwGetNoOfAttrLinks_       EwGetNoOfAttrLinks
  #define EwGetAttrLinkName_        EwGetAttrLinkName
  #define EwGetNoOfAttrLinkRegions_ EwGetNoOfAttrLinkRegions
  #define EwGetAttrLinkRect_        EwGetAttrLinkRect
  #define EwGetAttrLinkBaseline_    EwGetAttrLinkBaseline
#endif


/*******************************************************************************
* FUNCTION:
*   EwInitAttrText
*
* DESCRIPTION:
*   The function EwAttrTextInit() is called during the initialization of the
*   Graphics Engine. Its intention is to give the attributed text module a
*   chance to initialize its private variables etc..
*
* ARGUMENTS:
*   aArgs - User argument passed in the EwInitGraphicsEngine().
*
* RETURN VALUE:
*   If successful, returns != 0.
*
*******************************************************************************/
int EwInitAttrText( void* aArgs )
{
  EW_UNUSED_ARG( aArgs );

  /* Prepare performance counters */
  EwAddPerfCounter( EwParseFlowString        );
  EwAddPerfCounter( EwGetFlowTextAdvance     );
  EwAddPerfCounter( EwCreateAttrSet          );
  EwAddPerfCounter( EwFreeAttrSet            );
  EwAddPerfCounter( EwGetNoOfAttrFonts       );
  EwAddPerfCounter( EwGetNoOfAttrBitmaps     );
  EwAddPerfCounter( EwGetNoOfAttrColors      );
  EwAddPerfCounter( EwGetAttrFont            );
  EwAddPerfCounter( EwGetAttrBitmap          );
  EwAddPerfCounter( EwGetAttrColor           );
  EwAddPerfCounter( EwSetAttrFont            );
  EwAddPerfCounter( EwSetAttrBitmap          );
  EwAddPerfCounter( EwSetAttrColor           );
  EwAddPerfCounter( EwParseAttrString        );
  EwAddPerfCounter( EwFreeAttrString         );
  EwAddPerfCounter( EwDrawAttrText           );
  EwAddPerfCounter( EwGetAttrTextSize        );
  EwAddPerfCounter( EwIsAttrTextRTL          );
  EwAddPerfCounter( EwGetNoOfAttrLinks       );
  EwAddPerfCounter( EwGetAttrLinkName        );
  EwAddPerfCounter( EwGetNoOfAttrLinkRegions );
  EwAddPerfCounter( EwGetAttrLinkRect        );
  EwAddPerfCounter( EwGetAttrLinkBaseline    );
  
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwParseFlowString
*
* DESCRIPTION:
*   The function EwParseFlowString() converts the given source string aString
*   into a new string consisting of an array of wrapped text lines. These lines
*   can then be drawn on the screen by using EwDrawText() function, so multiline
*   flow text output is possible.
*
*   The function parses the passed string and tries to wrap it in separate text
*   rows with the specified width aWidth. The function stops if the string has
*   been processed or the resulting set of text rows has reached aMaxNoOfRows.
*
*   The automatic line wrap is performed primarily between words. Additionally
*   the zero-width-space-sign) and soft-hyphen are possible candidates for the
*   wrap position. These can be specified as special characters '^' or '\x00A0' 
*   (for the zero-width-space) and '~' or '\x00AD' (for soft-hyphen).
*
*   The both special characters '^' and '~' are usually ignored and they are not
*   displayed until the line wrap took place at its position. Then the '~' sign
*   is converted into the hyphen '-'. The zero-width-space in contrast breaks a
*   row only and remains invisible. These both special characters provide a 
*   flexibel way to output multi-line text.
*
*   Beside the automatic text wrap, an explicit linefeed is possible when the
*   '\n' sign has been found in the string.
*
*   In order to be able to output the special characters as regular signs, the
*   character '%' can be applied in front of the affected sign to convert it
*   to a regular sign (e.g. "%~" for the '~' sign or "%%" for the '%' sign).
*
*   The returned string contains an array of text lines. These are stored in
*   following structure:
*
*   +-------------+
*   | No of lines |
*   +-------------+------+------+------+------+------+     +------+
*   |    Offset   | Char | Char | Char | Char | Char | ... | Char |
*   +-------------+------+------+------+------+------+     +------+
*   |    Offset   | Char | Char | Char | Char | Char | ... | Char |
*   +-------------+------+------+------+------+------+     +------+
*      ...
*   +-------------+------+------+------+------+------+     +------+
*   |    Offset   | Char | Char | Char | Char | Char | ... | Char |
*   +-------------+------+------+------+------+------+     +------+
*   |      0      |
*   +-------------+
*
*   The first character contains the number of lines inside the string. Each
*   line starts with the offset (in character) to the next line, followed by the
*   signs belonging to the line. The lines are not terminated. The entire string
*   is terminated with 0.
*
* ARGUMENTS:
*   aFont        - Pointer to the used font.
*   aString      - String to wrap.
*   aWidth       - Width to wrap the text rows. If this parameter is < 0, the
*     text is not automatically wrapped in rows expcept the linefeed signs.
*   aMaxNoOfRows - Limit when to stop the text processing. If this parameter
*     is < 0, the entire string is processed.
*   aBidi        - If not NULL refers the Bidi context to be used by the
*     function while processing the string.
*
* RETURN VALUE:
*   The function returns a string containing an array of text lines.
*
*   Please note, the returned string is controlled by the Embedded Wizard
*   Runtime Environment EWRTE. If not used anymore, the string is released
*   automatically.
*
*******************************************************************************/
XString EwParseFlowString_( XFont* aFont, XChar* aString, XInt32 aWidth,
  XInt32 aMaxNoOfRows, XHandle aBidi )
{
  /* Estimate how much memory is needed for the buffer to store the wrapped
     string - worst case one character per row */
  XHandle kernFont    = aFont? aFont->Handle: 0;
  int     charWidth   = aFont? aFont->Ascent + aFont->Descent : 0;
  int     maxWidth    = ( aWidth < 0 )? 0x7FFFFFFE : aWidth;
  int     maxNoOfRows = ( aMaxNoOfRows < 0 )? 0x7FFFFFFF : aMaxNoOfRows;
  int     strLen      = EwGetStringLength( aString );
  int     noOfRows1   = CountLineFeeds( aString );
  int     noOfRows2   = (( charWidth * ( strLen - noOfRows1 )) / ( maxWidth + 1 )) + 1;
  int     noOfRows    = MIN( noOfRows1 + noOfRows2, strLen );
  int     dstLen      = strLen + noOfRows + 1;
  XChar*  dst         = EwNewStringChar( 0, dstLen );
  XChar*  dstLine     = dst     + 1;
  XChar*  dstPtr      = dstLine + 1;
  XChar*  srcStart    = dst     + dstLen - strLen;
  XChar*  srcPtr      = srcStart;
  XChar*  wrapDstPtr  = 0;
  XChar   prevCode    = 0;
  int     prevLevel   = -1;
  int     wrapActive  = 0;
  int     width       = 0;
  int     count       = 0;
  XChar   ch;

  /* Nothing to do - return empty string */
  if ( !aFont || !dst || !strLen || !maxNoOfRows )
    return 0;

  /* Copy the original string to the end of the reserved 'dst' string. The
     following processing steps will evaluate and modify the signs in this
     copied string */
  EwCopy( srcPtr, aString, strLen * sizeof( XChar ));

  /* Process the special characters ^, ~ and the escape sequence % */
  for ( ; srcPtr < ( dst + dstLen ); srcPtr++ )
    switch ( *srcPtr )
    {
      case '%' : *srcPtr++ = 0xFEFF; break;
      case '^' : *srcPtr   = 0x200B; break;
      case '~' : *srcPtr   = 0x00AD; break;
    }

  /* Should the string be processed with Bidi? */
  if ( aBidi && !EwBidiInit( aBidi, srcStart, strLen, -1 ))
    aBidi = 0;

  /* Does the string contain bidirectional text? */
  if ( aBidi )
  {
    EwBidiProcess     ( aBidi, srcStart ); 
    EwBidiMirrorGlyphs( aBidi, srcStart );
    EwBidiApplyShaping( aBidi, srcStart, 0, &aFont, 1 );
  }

  /* Repeat until all signs has been evaluated */
  for ( srcPtr = srcStart; ( ch = *srcPtr ) != 0; )
  {
    /* As long as regular signs are found */
    if ( ch != '\n' )
    {
      XGlyphMetrics glyph;
      int           chWidth   = 0;
      XChar*        curSrcPtr = srcPtr;

      /* Replace all BIDI control character codes by zero-width-non-break-space */
      if (( ch == 0x061C ) || 
         (( ch >= 0x200C ) && ( ch <= 0x200F )) ||
         (( ch >= 0x202A ) && ( ch <= 0x202E )) ||
         (( ch >= 0x2066 ) && ( ch <= 0x2069 )))
        ch = 0xFEFF;

      /* The actual character can't be appended to a wrapped row.
         End the wrap active mode. */
      if ( wrapActive && ( ch != 0xFEFF ) && ( ch != 0x200B ) && 
         ( ch != 0x00AD ) && ( ch != ' ' ))
        wrapActive = 0;

      /* Is this a line wrap candidate! Don't wrap at the begin of a line! */
      if (( ch == 0x200B ) && width && !wrapActive )
      {
        /* Discard the last soft-hyphen wrap candidated */
        if ( wrapDstPtr && ( *wrapDstPtr == 0x00AD ))
          *wrapDstPtr = 0xFEFF;

        wrapDstPtr = dstPtr;
        wrapActive = 1;
      }

      /* Is this a line wrap candidate! Don't wrap at the begin of a line! */
      if (( ch == ' ' ) && width )
      {
        /* Discard the last soft-hyphen wrap candidated */
        if ( wrapDstPtr && ( *wrapDstPtr == 0x00AD ))
          *wrapDstPtr = 0xFEFF;

        wrapDstPtr = dstPtr;
        wrapActive = 1;
      }

      /* The soft-hyphen lies at the begin of a text row or it follows another
         wrap candidate - ignore this hyphen */
      if (( ch == 0x00AD ) && ( !width || wrapActive ))
        ch = 0xFEFF;

      /* Is this a soft-hyphen? This sign does not appear on the screen,
         unless the silent line wrap is applied - then a '-' minus sign is
         displayed. */
      if ( ch == 0x00AD )
      {
        int shWidth = 0;

        /* If applied - the soft-hyphen sign should be displayed ... Get the size
           of the soft-hyphen sign ... */
        if ( EwGetGlyphMetrics( aFont, 0x00AD, &glyph ))
        {
          int level = aBidi? EwBidiGetCharLevel( aBidi, (int)( srcPtr - srcStart )) : 0;

          shWidth = glyph.Advance;

          /* Does the actual character and the preceding one belong to the same
             Bidi level run? Then calculate the kerning between them. Otherwise,
             the glyphs will be separated after reordering of the text row, so no
             kerning is applicable on them */
          if ( level == prevLevel )
          {
            if ( level & 1 ) shWidth += EwFntGetKerning( kernFont, ch, prevCode );
            else             shWidth += EwFntGetKerning( kernFont, prevCode, ch );
          }
        }

        /* ... and check, whether there is space in the current line for this
           soft-hyphen sign. Please note, if the soft-hyphen is the unique
           possible wrap position - accept it regardless of the size limitation */
        if (((( width + shWidth ) <= maxWidth ) && shWidth ) || !wrapDstPtr )
        {
          /* Discard the last soft-hyphen wrap candidated */
          if ( wrapDstPtr && ( *wrapDstPtr == 0x00AD ))
            *wrapDstPtr = 0xFEFF;

          /* Avoid that the width of the hyphen is accumulated */
          width     -= shWidth;
          wrapDstPtr = dstPtr;
          wrapActive = 1;
        }
      }

      /* All processed special characters are represented by the zero-width-
         non-break-space */
      if ( ch == 0x200B )
        ch = 0xFEFF;

      /* Trying to access the horizontal-ellipsis sign, but it is not available
         in the font. Use 3x dot sign instead */
      if (( ch == 0x2026 ) && !EwIsGlyphAvailable( aFont, ch ) &&
            EwGetGlyphMetrics( aFont, '.', &glyph ))
      {
        int level = aBidi? EwBidiGetCharLevel( aBidi, (int)( srcPtr - srcStart )) : 0;

        /* The advance resulting from the composition of the character of three
           glyphs */
        chWidth  = glyph.Advance * 3;
        chWidth += EwFntGetKerning( kernFont, '.', '.' ) * 2;

        /* Does the actual character and the preceding one belong to the same
           Bidi level run? Then calculate the kerning between them. Otherwise,
           the glyphs will be separated after reordering of the text row, so no
           kerning is applicable on them */
        if ( level == prevLevel )
        {
          if ( level & 1 ) chWidth += EwFntGetKerning( kernFont, '.', prevCode );
          else             chWidth += EwFntGetKerning( kernFont, prevCode, '.' );
        }

        prevCode  = '.';
        prevLevel = level;
      }

      /* Get the metric of the affected glyph - Remember the size of the glyph
         for further processing*/
      else if (( ch != 0xFEFF ) && EwGetGlyphMetrics( aFont, ch, &glyph ))
      {
        int level = aBidi? EwBidiGetCharLevel( aBidi, (int)( srcPtr - srcStart )) : 0;

        chWidth = glyph.Advance;

        /* Does the actual character and the preceding one belong to the same
           Bidi level run? Then calculate the kerning between them. Otherwise,
           the glyphs will be separated after reordering of the text row, so no
           kerning is applicable on them */
        if ( level == prevLevel )
        {
          if ( level & 1 ) chWidth += EwFntGetKerning( kernFont, ch, prevCode );
          else             chWidth += EwFntGetKerning( kernFont, prevCode, ch );
        }

        prevCode  = ch;
        prevLevel = level;
      }

      srcPtr++;

      /* Test, whether the sign can be displayed within the line. To avoid
         endless line wrapping, the first sign is always taken over! */
      if ((( chWidth + width ) <= maxWidth ) || !width )
      {
        width    += chWidth;
        *dstPtr++ = ch;
      }

      /* There is not enought space for the sign - skip back to the last wrap
         canditate - if any! */
      else if ( wrapDstPtr )
      {
        /* Breaking a line at blank? Replace it by a zero-width sign */
        if ( *wrapDstPtr == ' ' )
          *wrapDstPtr = 0xFEFF;

        /* Move back all characters following the wrap position */
        for ( srcPtr -= 2, dstPtr--; dstPtr > wrapDstPtr; srcPtr--, dstPtr-- )
          *srcPtr = *dstPtr;

        /* Force this row to be terminated */
        wrapDstPtr = 0;
        ch         = '\n';
        srcPtr++;
        dstPtr++;
      }

      /* No word wrap candidate found - force a wrap before the current sign */
      else
      {
        srcPtr = curSrcPtr;
        ch     = '\n';
      }

      /* At the end of wrapped row skip all blank, zero-width and wrap candidate
         characters following the wrap position. Append them to the just wrapped
         row. The next row, if any, should start with a visible character. Note,
         space is a visible character! If text wrap occured at space position,
         the affected space character is consumed. Other following spaces are
         retained */
      if ( ch == '\n' )
      {
        if ( *srcPtr == ' ' )
        {
          *dstPtr++ = 0xFEFF;
          srcPtr++;
        }

        while ((( ch = *srcPtr ) == 0xAD ) ||
                ( ch == 0x061C ) || ( ch == 0xFEFF )  || 
               (( ch >= 0x200B ) && ( ch <= 0x200F )) ||
               (( ch >= 0x202A ) && ( ch <= 0x202E )) ||
               (( ch >= 0x2066 ) && ( ch <= 0x2069 )))
        {
          *dstPtr++ = 0xFEFF;
          srcPtr++;
        }

        ch = '\n';
      }
    }

    /* New line terminated line - store it for justified text */
    else
    {
      srcPtr++;
      *dstPtr++ = ch;
    }

    /* Terminate the text line and prepare the preprocessor for the next text row */
    if (( ch == '\n' ) && *srcPtr )
    {
      /* Terminating a text row but there is no space for a furter row. Stop. */
      if ( --maxNoOfRows <= 0 )
        break;

      /* Discard the last soft-hyphen wrap candidated */
      if ( wrapDstPtr && ( *wrapDstPtr == 0x00AD ))
        *wrapDstPtr = 0xFEFF;

      /* Is there enough space in the 'dst' buffer? If not resize the buffer */
      if (( srcPtr - dstPtr ) < 2 )
      {
        int    diff   = ( dstLen >> 2 ) + 2;
        XChar* newDst = EwNewStringChar( 0, dstLen + diff );

        /* No memory to enlarge the buffer */
        if ( !newDst )
          return 0;

        /* Copy the contents into the new buffer */
        EwCopy( newDst, dst, (int)(( dstPtr - dst ) * sizeof( XChar )));
        EwCopy( newDst + dstLen + diff - (( dst + dstLen ) - srcPtr ), srcPtr,
                (int)((( dst + dstLen ) - srcPtr ) * sizeof( XChar )));

        /* Adjust the pointers to refer to the new buffer */
        dstPtr     = newDst + ( dstPtr   - dst );
        dstLine    = newDst + ( dstLine  - dst );
        srcPtr     = newDst + ( srcPtr   - dst ) + diff;
        srcStart   = newDst + ( srcStart - dst ) + diff;
        dst        = newDst;
        dstLen    += diff;                
      }

      /* Complete the Bidi-Algorithm on the just estimated row? */
      if ( aBidi )
      {
        int endInx   = (int)( srcPtr - srcStart );
        int startInx = (int)( endInx - ( dstPtr - dstLine ) + 1 );

        EwBidiCompleteRow ( aBidi, startInx, endInx );
        EwBidiReorderChars( aBidi, startInx, endInx, dstLine + 1 );
      }

      /* Complete the current text line and ... */
      dstLine[0] = (XChar)( dstPtr - dstLine );
      dstPtr[0]  = 0;

      /* ... continue with the next */
      dstLine    = dstPtr;
      dstPtr    += 1;
      wrapDstPtr = 0;
      wrapActive = 0;
      width      = 0;
      prevCode   = 0;
      prevLevel  = -1;
      count++;
    }
  }

  /* Discard the last soft-hyphen wrap candidated */
  if ( wrapDstPtr && ( *wrapDstPtr == 0x00AD ))
    *wrapDstPtr = 0xFEFF;

  /* Terminate the last text line */
  if (( dstPtr - dstLine ) > 1 )
  {
    /* Complete the Bidi-Algorithm on the just estimated row? */
    if ( aBidi )
    {
      int endInx   = (int)( srcPtr - srcStart );
      int startInx = (int)( endInx - ( dstPtr - dstLine ) + 1 );

      EwBidiCompleteRow ( aBidi, startInx, endInx );
      EwBidiReorderChars( aBidi, startInx, endInx, dstLine + 1 );
    }

    /* Complete the current text line and ... */
    dstLine[0] = (XChar)( dstPtr - dstLine );
    dstPtr[0]  = 0;
    count++;
  }

  /* Terminate the entire text */
  *dst = (XChar)count;

  return dst;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwParseFlowString, XString, ( XFont* aFont, XChar* aString,
  XInt32 aWidth, XInt32 aMaxNoOfRows, XHandle aBidi ), ( aFont, aString, aWidth,
  aMaxNoOfRows, aBidi ))


/*******************************************************************************
* FUNCTION:
*   EwGetFlowTextAdvance
*
* DESCRIPTION:
*   The function EwGetFlowTextAdvance() calculates the advance of the widest
*   text row in the multi-line text block aFlowString by using the font aFont.
*   The value is calculated by the sum of advance values of all glyphs needed to 
*   display the respective text row and then by selecting the result of the row
*   which is the widest one.
*
* ARGUMENTS:
*   aFont       - Font to use for the text calculation.
*   aFlowString - Text to process as already parsed flow string (see the 
*     function EwParseFlowString())
*
* RETURN VALUE:
*   Returns the advance value for the widest text row.
*
*******************************************************************************/
XInt32 EwGetFlowTextAdvance_( XFont* aFont, XChar* aFlowString )
{
  int len = EwGetStringLength( aFlowString );
  int inx = 1;
  int w   = 0;

  /* Nothing to do */
  if ( !len )
    return 0;

  if ( !aFont )
  {
    EwError( 201 );
    return 0;
  }

  /* Evaluate all text rows and determine the size of resulting text block */
  while ( inx < len )
  {
    int offset = aFlowString[ inx ];
    int rowLen = offset - 1;
    int rowInx = inx + 1;
    int rowW;

    /* The string is not a valid flow string. Abort. */
    if (( rowInx + rowLen ) > len )
      return 0;
    
    /* Calculate the advance of the text row */
    rowW = EwGetTextAdvance( aFont, aFlowString + rowInx, rowLen );

    /* Track the widest row */
    if ( rowW > w )
      w = rowW;

    /* Skip to the next row */
    inx += offset;
  }

  return w;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetFlowTextAdvance, XInt32, ( XFont* aFont, 
  XChar* aFlowString ), ( aFont, aFlowString ))


/*******************************************************************************
* FUNCTION:
*   EwCreateAttrSet
*
* DESCRIPTION:
*   The function EwCreateAttrSet() creates and initializes a new XAttrSet. The
*   set is used as a simple container for storing of fonts, bitmaps and colors.
*   These resources are necessary for parsing and drawing of attributed strings. 
*
*   The size of the set (the max. number of entries, it can store) is passed in
*   the arguments aNoOfFonts, aNoOfBitmaps and aNoOfColors. After creation, the
*   size of the set is fixed and can not be changed any more.
*
*   All entries of a newly created set are always initialized with 0. To start
*   working with attributed strings, the set should be loaded with necessary 
*   fonts, bitmaps and colors. The functions, defined below allow an access to
*   these entries.
*
* ARGUMENTS:
*   aNoOfFonts   - Max. number of fonts.
*   aNoOfBitmaps - Max. number of bitmaps.
*   aNoOfColors  - Max. number of colors.
*
* RETURN VALUE:
*   If successful, the function returns a pointer to the newly created set.
*   Otherwise the function returns 0.
*
*******************************************************************************/
XAttrSet* EwCreateAttrSet_( XInt32 aNoOfFonts, XInt32 aNoOfBitmaps,
  XInt32 aNoOfColors )
{
  int       size = sizeof( XAttrSet );
  XAttrSet* set;

  /* Limit the number of fonts, bitmaps and colors the set may contain */
  if ( aNoOfFonts   < 0   ) aNoOfFonts   = 0;
  if ( aNoOfBitmaps < 0   ) aNoOfBitmaps = 0;
  if ( aNoOfColors  < 0   ) aNoOfColors  = 0;
  if ( aNoOfFonts   > 256 ) aNoOfFonts   = 256;
  if ( aNoOfBitmaps > 256 ) aNoOfBitmaps = 256;
  if ( aNoOfColors  > 256 ) aNoOfColors  = 256;

  /* Calculate the memory size for the set */
  size += aNoOfFonts   * sizeof( XFont*   );
  size += aNoOfBitmaps * sizeof( XBitmap* );
  size += aNoOfColors  * sizeof( XColor   );

  /* ... and reserve memory for the set */
  do
    set = EwAlloc( size );
  while ( !set && EwImmediateReclaimMemory( 19 ));

  /* Report a message if there is no more memory */
  if ( !set )
  {
    EwError( 19 );
    return 0;
  }

  /* Ok? Then initialize the newly created set */
  EwZero( set, size );
  set->NoOfFonts   = aNoOfFonts;
  set->NoOfBitmaps = aNoOfBitmaps;
  set->NoOfColors  = aNoOfColors;
  set->Fonts       = (XFont**  )( set + 1 );
  set->Bitmaps     = (XBitmap**)( set->Fonts + aNoOfFonts );
  set->Colors      = (XColor*  )( set->Bitmaps + aNoOfBitmaps );

  /* Track the RAM usage */
  EwResourcesMemory += size;

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return set;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwCreateAttrSet, XAttrSet*, ( XInt32 aNoOfFonts,
  XInt32 aNoOfBitmaps, XInt32 aNoOfColors ), ( aNoOfFonts, aNoOfBitmaps,
  aNoOfColors ))


/*******************************************************************************
* FUNCTION:
*   EwFreeAttrSet
*
* DESCRIPTION:
*   The function EwFreeAttrSet() frees the memory occupied by the set. The
*   function frees the memory only - the corresponding fonts, bitmaps are not
*   affected!
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set to free.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeAttrSet_( XAttrSet* aAttrSet )
{
  if ( !aAttrSet )
    return;

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XAttrSet );
  EwResourcesMemory -= aAttrSet->NoOfFonts   * sizeof( XFont*   );
  EwResourcesMemory -= aAttrSet->NoOfBitmaps * sizeof( XBitmap* );
  EwResourcesMemory -= aAttrSet->NoOfColors  * sizeof( XColor   );

  aAttrSet->NoOfFonts   = 0;
  aAttrSet->NoOfBitmaps = 0;
  aAttrSet->NoOfColors  = 0;
  aAttrSet->Fonts       = 0;
  aAttrSet->Bitmaps     = 0;
  aAttrSet->Colors      = 0;
  EwFree( aAttrSet );
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwFreeAttrSet, ( XAttrSet* aAttrSet ), ( aAttrSet ))


/*******************************************************************************
* FUNCTION:
*   EwGetNoOfAttrFonts
*
* DESCRIPTION:
*   The function EwGetNoOfAttrFonts() determinate how many fonts can be stored
*   within the given set.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*
* RETURN VALUE:
*   If successful, the function returns the max. number of fonts. Otherwise the
*   function returns 0.
*
*******************************************************************************/
XInt32 EwGetNoOfAttrFonts_( XAttrSet* aAttrSet )
{
  if ( aAttrSet )
    return aAttrSet->NoOfFonts;

  return 0;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetNoOfAttrFonts, XInt32, ( XAttrSet* aAttrSet ), 
  ( aAttrSet ))


/*******************************************************************************
* FUNCTION:
*   EwGetNoOfAttrBitmaps
*
* DESCRIPTION:
*   The function EwGetNoOfAttrBitmaps() determinate how many bitmaps can be 
*   stored within the given set.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*
* RETURN VALUE:
*   If successful, the function returns the max. number of bitmaps. Otherwise
*   the function returns 0.
*
*******************************************************************************/
XInt32 EwGetNoOfAttrBitmaps_( XAttrSet* aAttrSet )
{
  if ( aAttrSet )
    return aAttrSet->NoOfBitmaps;

  return 0;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetNoOfAttrBitmaps, XInt32, ( XAttrSet* aAttrSet ),
  ( aAttrSet ))


/*******************************************************************************
* FUNCTION:
*   EwGetNoOfAttrColors
*
* DESCRIPTION:
*   The function EwGetNoOfAttrColors() determinate how many colors can be stored
*   within the given set.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*
* RETURN VALUE:
*   If successful, the function returns the max. number of colors. Otherwise
*   the function returns 0.
*
*******************************************************************************/
XInt32 EwGetNoOfAttrColors_( XAttrSet* aAttrSet )
{
  if ( aAttrSet )
    return aAttrSet->NoOfColors;

  return 0;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetNoOfAttrColors, XInt32, ( XAttrSet* aAttrSet ),
  ( aAttrSet ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrFont
*
* DESCRIPTION:
*   The function EwGetAttrFont() returns the font entry aFontNo from the given
*   set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*   aFontNo  - Number of the affected font entry within the set. The first
*     entry has the number 0, the second 1, and so far.
*
* RETURN VALUE:
*   If successful, the function returns the pointer to the corresponding font
*   object. Otherwise the function returns 0.
*
*******************************************************************************/
XFont* EwGetAttrFont_( XAttrSet* aAttrSet, XInt32 aFontNo )
{
  if ( !aAttrSet || ( aFontNo < 0 ))
    return 0;

  if ( aFontNo < aAttrSet->NoOfFonts )
    return aAttrSet->Fonts[ aFontNo ];

  return 0;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrFont, XFont*, ( XAttrSet* aAttrSet, XInt32 aFontNo ),
  ( aAttrSet, aFontNo ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrBitmap
*
* DESCRIPTION:
*   The function EwGetAttrBitmap() returns the bitmap entry aBitmapNo from the
*   given set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet  - Pointer to the set.
*   aBitmapNo - Number of the affected bitmap entry within the set. The first
*     entry has the number 0, the second 1, and so far.
*
* RETURN VALUE:
*   If successful, the function returns the pointer to the corresponding bitmap
*   object. Otherwise the function returns 0.
*
*******************************************************************************/
XBitmap* EwGetAttrBitmap_( XAttrSet* aAttrSet, XInt32 aBitmapNo )
{
  if ( !aAttrSet || ( aBitmapNo < 0 ))
    return 0;

  if ( aBitmapNo < aAttrSet->NoOfBitmaps )
    return aAttrSet->Bitmaps[ aBitmapNo ];

  return 0;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrBitmap, XBitmap*, ( XAttrSet* aAttrSet, 
  XInt32 aBitmapNo ), ( aAttrSet, aBitmapNo ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrColor
*
* DESCRIPTION:
*   The function EwGetAttrColor() returns the color entry aColorNo from the
*   given set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*   aColorNo - Number of the affected color entry within the set. The first
*     entry has the number 0, the second 1, and so far.
*
* RETURN VALUE:
*   If successful, the function returns the affected color value. Otherwise the
*   function returns 0.
*
*******************************************************************************/
XColor EwGetAttrColor_( XAttrSet* aAttrSet, XInt32 aColorNo )
{
  if ( !aAttrSet || ( aColorNo < 0 ))
    return EwNewColor( 0, 0, 0, 0 );

  if ( aColorNo >= aAttrSet->NoOfColors )
    return EwNewColor( 0, 0, 0, 0 );

  return aAttrSet->Colors[ aColorNo ];
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrColor, XColor, ( XAttrSet* aAttrSet, 
  XInt32 aColorNo ), ( aAttrSet, aColorNo ))


/*******************************************************************************
* FUNCTION:
*   EwSetAttrFont
*
* DESCRIPTION:
*   The function EwSetAttrFont() assigns new font aFont to the entry aFontNo
*   within the given set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*   aFontNo  - Number of the affected font entry. The first entry has the number
*     0, the second 1, and so far.
*   aFont    - Pointer to the font object to be stored within the set. If aFont
*     is 0, the affected font entry is set to zero.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwSetAttrFont_( XAttrSet* aAttrSet, XInt32 aFontNo, XFont* aFont )
{
  if ( !aAttrSet || ( aFontNo < 0 ))
    return;

  if ( aFontNo < aAttrSet->NoOfFonts )
    aAttrSet->Fonts[ aFontNo ] = aFont;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwSetAttrFont, ( XAttrSet* aAttrSet, XInt32 aFontNo,
  XFont* aFont ), ( aAttrSet, aFontNo, aFont ))


/*******************************************************************************
* FUNCTION:
*   EwSetAttrBitmap
*
* DESCRIPTION:
*   The function EwSetAttrBitmap() assigns new bitmap aBitmap to the entry 
*   aBitmapNo within the given set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet  - Pointer to the set.
*   aBitmapNo - Number of the affected bitmap entry. The first entry has the
*     number 0, the second 1, and so far.
*   aBitmap   - Pointer to the bitmap object to be stored within the set. If
*     aBitmap is 0, the affected bitmap entry is set to zero.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwSetAttrBitmap_( XAttrSet* aAttrSet, XInt32 aBitmapNo, XBitmap* aBitmap )
{
  if ( !aAttrSet || ( aBitmapNo < 0 ))
    return;

  if ( aBitmapNo < aAttrSet->NoOfBitmaps )
    aAttrSet->Bitmaps[ aBitmapNo ] = aBitmap;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwSetAttrBitmap, ( XAttrSet* aAttrSet, XInt32 aBitmapNo,
  XBitmap* aBitmap ), ( aAttrSet, aBitmapNo, aBitmap ))


/*******************************************************************************
* FUNCTION:
*   EwSetAttrColor
*
* DESCRIPTION:
*   The function EwSetAttrColor() assigns new color value aColor to the entry 
*   aColorNo within the given set aAttrSet.
*
* ARGUMENTS:
*   aAttrSet - Pointer to the set.
*   aColorNo - Number of the affected color entry. The first entry has the
*     number 0, the second 1, and so far.
*   aColor   - The color value to store within the set.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwSetAttrColor_( XAttrSet* aAttrSet, XInt32 aColorNo, XColor aColor )
{
  if ( !aAttrSet || ( aColorNo < 0 ))
    return;

  if ( aColorNo < aAttrSet->NoOfColors )
    aAttrSet->Colors[ aColorNo ] = aColor;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwSetAttrColor, ( XAttrSet* aAttrSet, XInt32 aColorNo,
  XColor aColor ), ( aAttrSet, aColorNo, aColor ))


/*******************************************************************************
* FUNCTION:
*   EwParseAttrString
*
* DESCRIPTION:
*   The function EwParseAttrString() converts the given source string aString
*   into a list of drawing statements. Unlike simple text drawing operations,
*   attributed strings may appear with multiple fonts, colors and images - like
*   HTML.
*
*   The appearance of the string is determinated by the used attributes and by 
*   the passed aAttrSet container. The attributes describe what to do with the
*   text, which font/color should be used or what images should be displayed
*   together with the text. The attributes are always enclosed in '{ .... }'
*   braces. For example, the attribute for color selection looks like this:
*
*     "The text with other {clr1}text color{clr0}."
*
*   The attribute {clr1} forces the Graphics Engine to draw the following text
*   with the color number 1. The value of the desired color is stored in the
*   aAttrSet container. In the same manner a font selection can be applied or
*   an image can be displayed together with the text. The attributed strings 
*   use a small but very powerfull set of attributes. They control the entire
*   layout and the appearance of the displayed text.
*
*   The function EwParseAttrString() returns a memory block containing the
*   preprocessed drawing statements. These statements can then be executed
*   very fast by the EwDrawAttrText() function. In this manner the parsing of
*   the string is done only once - the text can be displayed several times.
*
* ARGUMENTS:
*   aAttrSet        - Pointer to the set containing fonts, bitmaps and colors
*     for the attributed string.
*   aString         - Source string containing the text and the attributes.
*   aWidth          - Width of the rectangular area used for the text 
*     formatting and line wrapping.
*   aEnableBidiText - If != 0, the Unicode Bidi Algorithm is applied on every
*     paragraph content.
*
* RETURN VALUE:
*   If successful, the function returns a pointer to a memory area containing
*   drawing statements. If the parsing is failed, or the memory is exhausted
*   the function returns 0.
*
*******************************************************************************/
XAttrString* EwParseAttrString_( XAttrSet* aAttrSet, XChar* aString, 
  XInt32 aWidth, XBool aEnableBidiText )
{
  XBuffer*     buffer;
  XAttrString* attrString  = 0;
  int          height      = 0;
  int          lastLeading = 0;
  int          isRTL       = -1;
  int          noOfLinks   = 0;
  int          namesArea   = 0;

  /* Anything to do? */
  if ( !aAttrSet || !aString || !*aString || ( aWidth <= 0 ))
    return 0;

  buffer = AllocBuffer();

  /* Start the parsing - the resulting attributed string is stored in the
     temporary buffer */
  if ( Body( aAttrSet, aString, aWidth, aEnableBidiText, buffer, &height, 
             &lastLeading, &isRTL, &noOfLinks, &namesArea ))
  {
    int    size      = BUFSIZE( buffer );
    int    size2     = size;
    XChar* linkNames = 0;

    /* Align to next DWORD */
    if ( size2 % 2 )
      size2++;

    /* Take in account the additional zero characters terminating the link 
       names */
    namesArea += noOfLinks;

    /* Reserve memory for the already parsed attributed string and an
       optional list of links */
    do
      attrString = EwAlloc( sizeof( XAttrString ) + ( size2 * sizeof( short )) +
        ( noOfLinks * sizeof( XAttrLink )) + ( namesArea * sizeof( XChar )));
    while ( !attrString && EwImmediateReclaimMemory( 20 ));

    /* Report a message if there is no more memory */
    if ( !attrString )
      EwError( 20 );

    /* If successful, copy the buffer into an attributed string */
    if ( attrString )
    {
      attrString->Size      = size;
      attrString->Data      = (unsigned short*)( attrString + 1 );
      attrString->IsRTL     = isRTL == 1;
      attrString->NoOfLinks = noOfLinks;
      attrString->NamesArea = namesArea;
      attrString->Links     = (XAttrLink*)( attrString->Data + size2 );
      linkNames             = (XChar*)( attrString->Links + noOfLinks );

      /* The prepared instruction follow the Attr String structure */
      EwMove( attrString->Data, buffer->Buffer, size * sizeof( short ));

      /* Collect the links existing in the string */
      if ( noOfLinks )
      {
        CollectLinks( aString, attrString->Links, linkNames );
        CollectLinkRegions( attrString->Links, 0, 0, attrString->Data, 
                            attrString->Data + size );
      }

      /* Track the RAM usage */
      EwResourcesMemory += sizeof( XAttrString );
      EwResourcesMemory += size2 * sizeof( short );
      EwResourcesMemory += noOfLinks * sizeof( XAttrLink );
      EwResourcesMemory += namesArea * sizeof( XChar );

      /* Also track the max. memory pressure */
      if ( EwResourcesMemory > EwResourcesMemoryPeak )
        EwResourcesMemoryPeak = EwResourcesMemory;

      if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
        EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
    }
  }

  /* Release the temporary buffer */
  FreeBuffer( buffer );

  return attrString;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwParseAttrString, XAttrString*, ( XAttrSet* aAttrSet, 
  XChar* aString, XInt32 aWidth, XBool aEnableBidiText ), ( aAttrSet, aString, 
  aWidth, aEnableBidiText ))


/*******************************************************************************
* FUNCTION:
*   EwFreeAttrString
*
* DESCRIPTION:
*   The function EwFreeAttrString() releases the memory area reserved by the
*   function EwParseAttrString() for drawing statements.
*
* ARGUMENTS:
*   aAttrString - Pointer to the attributed string to free the memory.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeAttrString_( XAttrString* aAttrString )
{
  if ( !aAttrString )
    return;

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XAttrString );
  EwResourcesMemory -= ( aAttrString->Size + ( aAttrString->Size % 2 )) * 
                         sizeof( short );
  EwResourcesMemory -= aAttrString->NoOfLinks * sizeof( XAttrLink );
  EwResourcesMemory -= aAttrString->NamesArea * sizeof( XChar );

  aAttrString->Size      = 0;
  aAttrString->Data      = 0;
  aAttrString->Links     = 0;
  aAttrString->NoOfLinks = 0;
  aAttrString->NamesArea = 0;
  EwFree( aAttrString );
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwFreeAttrString, ( XAttrString* aAttrString ),
  ( aAttrString ))


/*******************************************************************************
* FUNCTION:
*   EwDrawAttrText
*
* DESCRIPTION:
*   The function EwDrawAttrText() executes the drawing statements from the
*   attributed string aAttrString and draws the text and images into the aDst
*   bitmap at the origin (aDstX, aDstY). The drawing area is clipped by the 
*   aClipping rectangle. The necessary fonts, bitmaps and colors are passed in
*   the aAttrSet container.
*
* ARGUMENTS:
*   aDst        - Destination bitmap.
*   aAttrSet    - Pointer to the set of fonts, bitmaps and colors.
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*   aDstFrameNo - Frame within the destination bitmap affected by the drawing
*     operation.
*   aClipRect   - Area to limit the drawing operation (Relative to the top-left
*     corner of the destination bitmap frame).
*   aDstRect    - Area to fill with the text (Relative to the top-left corner
*     of the destination bitmap frame).
*   aSrcPos     - Text output origin (relative to the top-left corner of the 
*     text area).
*   aOpacityTL,
*   aOpacityTR,
*   aOpacityBR,
*   aOpacityBL  - Opacity values corresponding to the four corners of aDstRect.
*   aBlend      - If != 0, the drawn pixel will be alpha-blended with the pixel
*     in the background.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDrawAttrText_( XBitmap* aDst, XAttrSet* aAttrSet, 
  XAttrString* aAttrString, XInt32 aDstFrameNo, XRect aClipRect, XRect aDstRect,
  XPoint aSrcPos, XInt32 aOpacityTL, XInt32 aOpacityTR, XInt32 aOpacityBR,
  XInt32 aOpacityBL, XBool aBlend )
{
  unsigned short* ptr;
  XGradient       gradient;
  unsigned int    colors[4];

  /* Nothing to do */
  if ( !aDst || !aAttrSet || !aAttrString || ( aDstFrameNo < 0 ) ||
       ( aDstFrameNo >= aDst->NoOfFrames ))
    return;

  /* Prepare the opacity values */
  colors[0] = aOpacityTL << 24;
  colors[1] = aOpacityTR << 24;
  colors[2] = aOpacityBR << 24;
  colors[3] = aOpacityBL << 24;

  /* ... as linear 2D gradient */
  EwInitOpacityGradient( aDstRect.Point2.X - aDstRect.Point1.X, 
    aDstRect.Point2.Y - aDstRect.Point1.Y, colors, &gradient );

  /* Limit the drawing area to the specified coordinates. */
  aClipRect = EwIntersectRect( aClipRect, aDstRect );

  ptr = aAttrString->Data;

  /* Execute the drawing statements. First the filled backgrounds ...  */
  DrawAttrTextBackground( aDst, aDstFrameNo, aClipRect, aDstRect.Point1, 
    aDstRect.Point1.X - aSrcPos.X, aDstRect.Point1.Y - aSrcPos.Y, aAttrSet,
    ptr, ptr + aAttrString->Size, aBlend, &gradient );

  /* ... the the text and images */
  DrawAttrTextContent( aDst, aDstFrameNo, aClipRect, aDstRect.Point1, 
    aDstRect.Point1.X - aSrcPos.X, aDstRect.Point1.Y - aSrcPos.Y, aAttrSet,
    ptr, ptr + aAttrString->Size, aBlend, &gradient );
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwDrawAttrText, ( XBitmap* aDst, XAttrSet* aAttrSet, 
  XAttrString* aAttrString, XInt32 aDstFrameNo, XRect aClipRect, XRect aDstRect,
  XPoint aSrcPos, XInt32 aOpacityTL, XInt32 aOpacityTR, XInt32 aOpacityBR,
  XInt32 aOpacityBL, XBool aBlend ), ( aDst, aAttrSet, aAttrString, aDstFrameNo,
  aClipRect, aDstRect, aSrcPos, aOpacityTL, aOpacityTR, aOpacityBR, aOpacityBL,
  aBlend ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrTextSize
*
* DESCRIPTION:
*   The function EwGetAttrTextSize() calculates the rectangular area necessary
*   to draw the given attributed string. The rectangular area is calculated by 
*   building an union of all drawing statements of the attributed string.
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*
* RETURN VALUE:
*   Returns the size of the rectangular area needed to display the string.
*
*******************************************************************************/
XPoint EwGetAttrTextSize_( XAttrString* aAttrString )
{
  unsigned short* ptr  = 0;
  unsigned short* end  = 0;
  XRect           rect = {{ 0, 0 }, { 0, 0 }};

  if ( aAttrString )
  {
    ptr = aAttrString->Data;
    end = ptr + aAttrString->Size;
  }

  /* Repeat until all drawing statements are evaluated. This function evaluates
     only 'rectangle' and 'group' drawing statement. All other statements are
     already nested within the groups. */
  while ( ptr < end )
  {
    XRect r;

    /* Get the rect. area of the current drawing statement */
    r.Point1.X = WORDNUM( ptr[1]);
    r.Point1.Y = WORDNUM( ptr[2]);
    r.Point2.X = WORDNUM( ptr[3]);
    r.Point2.Y = WORDNUM( ptr[4]);

    /* ... and build an union with this area */
    rect = EwUnionRect( rect, r );

    /* Continue with the next drawing statement */
    if ( *ptr == 'R' ) ptr += 6;
    else               ptr += ptr[ 5 ]; /* Skip over the group */
  }

  /* Return the calculated size */
  return rect.Point2;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrTextSize, XPoint, ( XAttrString* aAttrString ),
  ( aAttrString ))


/*******************************************************************************
* FUNCTION:
*   EwIsAttrTextRTL
*
* DESCRIPTION:
*   The function EwIsAttrTextRTL() returns the basic paragraph direction 
*   resulting from the very first processed paragraph in the attributed string.
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString()
*     function.
*
* RETURN VALUE:
*   Returns != 0 if the first paragraph of the attributed string has RTL
*   paragraph direction. Otherwise return 0.
*
*******************************************************************************/
XBool EwIsAttrTextRTL_( XAttrString* aAttrString )
{
  return aAttrString && aAttrString->IsRTL;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwIsAttrTextRTL, XBool, ( XAttrString* aAttrString ),
  ( aAttrString ))


/*******************************************************************************
* FUNCTION:
*   EwGetNoOfAttrLinks
*
* DESCRIPTION:
*   The function EwGetNoOfAttrLinks() returns the total number of links stored
*   within the given attributed string aAttrString.
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*
* RETURN VALUE:
*   The function returns the number of links or zero if no links are defined in
*   the given attributed string.
*
*******************************************************************************/
XInt32 EwGetNoOfAttrLinks_( XAttrString* aAttrString )
{
  if ( !aAttrString )
    return 0;

  return aAttrString->NoOfLinks;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetNoOfAttrLinks, XInt32, ( XAttrString* aAttrString ),
  ( aAttrString ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrLinkName
*
* DESCRIPTION:
*   The function EwGetAttrLinkName() returns the name of a link, which is stored
*   within the given attributed string aAttrString. The number of the desired
*   link is passed in the argument aLinkNo. The first link has the number 0,
*   the second 1, ...
*
*   The total number of available links can be determinated by the call to the
*   function EwGetNoOfAttrLinks().
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*   aLinkNo     - The number of the affected link. The first link has the 
*     number 0, the second 1, ...
*
* RETURN VALUE:
*   The function returns a pointer to the zero terminated string containing the
*   link's name. If the link has no name or the desired link does not exist,
*   null pointer is returned.
*
*   Please note, the returned string is controlled by the Embedded Wizard
*   Runtime Environment EWRTE. If not used anymore, the string is released
*   automatically.
*
*******************************************************************************/
XString EwGetAttrLinkName_( XAttrString* aAttrString, XInt32 aLinkNo )
{
  if ( !aAttrString )
    return 0;

  /* Is the link number valid? */
  if (( aLinkNo < 0 ) || ( aLinkNo >= aAttrString->NoOfLinks ))
    return 0;

  /* Return the pointer to the zero terminated string */
  return EwNewString((XChar*)( aAttrString->Links[ aLinkNo ].Name ));
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrLinkName, XString, ( XAttrString* aAttrString, 
  XInt32 aLinkNo ), ( aAttrString, aLinkNo ))


/*******************************************************************************
* FUNCTION:
*   EwGetNoOfAttrLinkRegions
*
* DESCRIPTION:
*   The function EwGetNoOfAttrLinkRegions() returns the number of rectangular 
*   text areas enclosed by the given link aLinkNo within the attributed string
*   aAttrString. 
*
*   Due to the line wrap, the text enclosed by a single link can be wrapped in 
*   several text lines, so the link area may become a very complex polygon. To
*   describe this polygon, it is divided in several rectangular areas, one for
*   each text line enclosed by the link. To get the origin and the size of an
*   area, the function EwGetAttrLinkRect() should be used.
*
*   The regions are very useful, if a selection frame or an other kind of 
*   decoration should be drawn together with the link. 
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*   aLinkNo     - Number of the affected link. The first link has the number 0,
*     the second 1, ...
*
* RETURN VALUE:
*   The function returns the number of rectangular areas or zero if the desired
*   link is not defined within the attributed string, or the link does not
*   enclose any text.
*
*******************************************************************************/
XInt32 EwGetNoOfAttrLinkRegions_( XAttrString* aAttrString, XInt32 aLinkNo )
{
  if ( !aAttrString )
    return 0;

  /* Is the link number valid? */
  if (( aLinkNo < 0 ) || ( aLinkNo >= aAttrString->NoOfLinks ))
    return 0;

  /* Return the number of regions */
  return aAttrString->Links[ aLinkNo ].NoOfRegions;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetNoOfAttrLinkRegions, XInt32, ( XAttrString* aAttrString,
  XInt32 aLinkNo ), ( aAttrString, aLinkNo ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrLinkRect
*
* DESCRIPTION:
*   The function EwGetAttrLinkRect() returns the origin and the size of an area
*   occupied by the link aLinkNo within the attributed string aAttrString.
*
*   Due to the line wrap, the text enclosed by a single link can be wrapped in 
*   several text lines, so the link area may become a very complex polygon. To
*   describe this polygon, it is divided in several rectangular areas, one for
*   each text line enclosed by the link. The number of the desired rectangular
*   area should be passed in the argument aRegionNo. The first region has the 
*   number 0, the second 1, ...
*
*   The total number of available regions can be determinated by the call to the
*   function EwGetNoOfAttrLinkRegions().
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*   aLinkNo     - Number of the affected link. The first link has the number 0,
*     the second 1, ...
*   aRegionNo   - Number of the affected region. The first region has the number
*     0, the second 1, ...
*
* RETURN VALUE:
*   Returns the origin and the size of the area relative to the origin of the
*   drawn attributed text. If the desired link or region do not exist, an empty
*   rectangle is returned.
*
*******************************************************************************/
XRect EwGetAttrLinkRect_( XAttrString* aAttrString, XInt32 aLinkNo, 
  XInt32 aRegionNo )
{
  XAttrLink*      link;
  unsigned short* ptr;
  XRect           rect = {{ 0, 0 }, { 0, 0 }};

  if ( !aAttrString )
    return rect;

  /* Is the link number valid? */
  if (( aLinkNo < 0 ) || ( aLinkNo >= aAttrString->NoOfLinks ))
    return rect;

  link = &aAttrString->Links[ aLinkNo ];

  /* Is the region number valid? */
  if (( aRegionNo < 0 ) || ( aRegionNo >= link->NoOfRegions ))
    return rect;

  /* Find the desired link region */
  ptr = SkipToRegion( aLinkNo + 1, aRegionNo, link->StartPtr, link->EndPtr );

  /* Return the rect. area */
  if ( ptr )
  {
    rect.Point1.X = WORDNUM( ptr[2]) + link->X;
    rect.Point1.Y = WORDNUM( ptr[3]) + link->Y;
    rect.Point2.X = WORDNUM( ptr[4]) + link->X;
    rect.Point2.Y = WORDNUM( ptr[5]) + link->Y;
  }

  return rect;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrLinkRect, XRect, ( XAttrString* aAttrString, 
  XInt32 aLinkNo, XInt32 aRegionNo ), ( aAttrString, aLinkNo, aRegionNo ))


/*******************************************************************************
* FUNCTION:
*   EwGetAttrLinkBaseline
*
* DESCRIPTION:
*   The function EwGetAttrLinkBaseline() returns the vertical offset to the base
*   line of the text enclosed by the link aLinkNo within the attributed string 
*   aAttrString. The base line is used for vertical text alignment.
*
*   Due to the line wrap, the text enclosed by a single link can be wrapped in 
*   several text lines. For each text line a different base line may be used.
*   This function provides you with an access to this base line offset for each
*   text region enclosed by the link. The number of the desired region should be
*   passed in the argument aRegionNo. The first region has the number 0, the 
*   second 1, ...
*
*   The total number of available regions can be determinated by the call to the
*   function EwGetNoOfAttrLinkRegions().
*
* ARGUMENTS:
*   aAttrString - Pointer to the memory area containing the parsed, preprocessed
*     drawing statements. This area is created by the EwParseAttrString() 
*     function.
*   aLinkNo     - Number of the affected link. The first link has the number 0,
*     the second 1, ...
*   aRegionNo   - Number of the affected region. The first region has the number
*     0, the second 1, ...
*
* RETURN VALUE:
*   The function returns the vertical offset to the base line of the affected
*   region relative to the origin of the drawn attributed text. If the desired
*   link or region do not exist, the function returns 0.
*
*******************************************************************************/
XInt32 EwGetAttrLinkBaseline_( XAttrString* aAttrString, XInt32 aLinkNo,
  XInt32 aRegionNo )
{
  XAttrLink*      link;
  unsigned short* ptr;

  if ( !aAttrString )
    return 0;

  /* Is the link number valid? */
  if (( aLinkNo < 0 ) || ( aLinkNo >= aAttrString->NoOfLinks ))
    return 0;

  link = &aAttrString->Links[ aLinkNo ];

  /* Is the region number valid? */
  if (( aRegionNo < 0 ) || ( aRegionNo >= link->NoOfRegions ))
    return 0;

  /* Find the desired link region */
  ptr = SkipToRegion( aLinkNo + 1, aRegionNo, link->StartPtr, link->EndPtr );

  if ( !ptr )
    return 0;

  return WORDNUM( ptr[6]) + link->Y;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwGetAttrLinkBaseline, XInt32, ( XAttrString* aAttrString,
  XInt32 aLinkNo, XInt32 aRegionNo ), ( aAttrString, aLinkNo, aRegionNo ))


/* This helper function blends the color with the given opacity in the range 
   0 .. 255 */
static XColor BlendColor( XColor aColor, int aOpacity )
{
  aColor.Alpha = (unsigned char)(( aColor.Alpha * ( aOpacity + 1 )) >> 8 );
  return aColor;
}


/* This function creates a new instance of the internal buffer. It is used to 
   store preprocessed attributed strings temporary */
static XBuffer* AllocBuffer( void )
{
  XBuffer* tmp;

  do
    tmp = EwAlloc( sizeof( XBuffer ));
  while ( !tmp && EwImmediateReclaimMemory( 21 ));

  /* Report a message if there is no more memory */
  if ( !tmp )
  {
    EwError( 21 );
    return 0;
  }

  /* If successful, initialize the buffer */
  tmp->Buffer = 0;
  tmp->Ptr    = 0;
  tmp->Size   = 0;

  /* Track the temporary used memory */
  EwResourcesMemory += sizeof( XBuffer );

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return tmp;
}


/* This function frees the buffer and it's entire content */
static void FreeBuffer( XBuffer* aBuffer )
{
  /* No buffer passed - nothing to do */
  if ( !aBuffer )
    return;

  /* Release the buffer's content */
  if ( aBuffer->Buffer )
  {
    EwFree( aBuffer->Buffer );
    EwResourcesMemory -= aBuffer->Size * sizeof( unsigned short );
  }

  /*  ... and the structure */
  aBuffer->Buffer = 0;
  aBuffer->Ptr    = 0;
  aBuffer->Size   = 0;
  EwFree( aBuffer );

  EwResourcesMemory -= sizeof( XBuffer );;
}


/* This function ensures, that the given buffer has enought space for up to
   aCount words. If not, the buffer will grow. The function returns a value
   != 0, if the required words can be written into the buffer. If the growing
   is failed, the function returns 0 */
static int RequireBuffer( XBuffer* aBuffer, int aCount )
{
  /* No buffer passed - nothing to do */
  if ( !aBuffer )
    return 0;

  /* Check, whether there is space for aCount words */
  if (( aBuffer->Size - ( aBuffer->Ptr - aBuffer->Buffer )) < aCount )
  {
    int    tmpSize   = (int)( aBuffer->Ptr - aBuffer->Buffer );
    int    newSize   = tmpSize + aCount;
    XChar* oldBuffer = aBuffer->Buffer;
    XChar* tmpBuffer = oldBuffer;
    XChar* newBuffer = 0;

    /* Calculate the size of the new buffer - align it to the next 4kB
       border. */
    newSize  += 1024 - ( newSize % 1024 );

    /* Then try to allocate memory for this new buffer. */
    do
      newBuffer = EwAlloc( newSize * sizeof( unsigned short ));
    while ( !newBuffer && EwImmediateReclaimMemory( 22 ));

    /* Report a message if there is no more memory */
    if ( !newBuffer )
    {
      EwError( 22 );
      return 0;
    }

    /* Track the temporary used memory */
    EwResourcesMemory += newSize * sizeof( unsigned short );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    EwResourcesMemory -= aBuffer->Size * sizeof( unsigned short );

    /* Ok, store the pointer to the new buffer */
    aBuffer->Buffer = newBuffer;
    aBuffer->Ptr   += newBuffer - oldBuffer;
    aBuffer->Size   = newSize;

    /* ... and copy the old content to this new buffer */
    EwMove( newBuffer, oldBuffer, tmpSize * sizeof( unsigned short ));

    /* Release the old buffer */
    if ( tmpBuffer )
      EwFree( tmpBuffer );
  }

  /* Ok - the words may be written into the buffer */
  return 1;
}


/* This function parses an unsigned number from the given string. The number is
   returned in the argument aNumber. If the number could not be read, aNumber
   will be initialized with 0. The function returns the pointer to the next 
   sign. */
static XChar* ParseNumber( XChar* aString, int* aNumber )
{
  int   tmp = 0;
  XChar c   = *aString;

  /* Repeat until all digits are processed and build the number. */
  while (( c >= '0' ) && ( c <= '9' ))
  {
    tmp = ( tmp * 10 ) + ( c - '0' );
    c   = *++aString;
  }

  /* Return the number to the caller */
  *aNumber = tmp;

  return aString;
}


/* This function parses an unsigned number from the given string. Optionaly a
   percent value of aRange will be calculated, if the number is followed by 
   the '%' sign. */
static XChar* ParseNumber2( XChar* aString, int aRange, int* aNumber )
{
  XChar* ptr = ParseNumber( aString, aNumber );

  /* Ist this a percent value of aRange value? */
  if ( *ptr == '%' )
  {
    int num = *aNumber * aRange;

    *aNumber = num / 100;

    if (( num - ( *aNumber * 100 )) > 50 )
      *aNumber = *aNumber + 1;

    /* Skip over the '%' sign */
    ptr++;
  }

  return ptr;
}


/* This function parses up to 4 numbers as 'left,right,top,bottom' borders. The
   numbers are separated by ',' comma sign. The numbers can be ommited - in this
   case the corresponding border is initialized with 0. The borders are returned
   in the arguments aLeft,aRight,aTop,aBottom. The argument aWidth is necessary 
   if the left or right border uses the percent notation. In this case the 
   corresponding border is initialized with the percent value of aWidth. */
static XChar* ParseBorders( XChar* aString, int aWidth, int* aLeft, int* aRight,
  int* aTop, int* aBottom )
{
  /* Default value, if no borders are specified in the string. */
  int l = 0;
  int r = 0;
  int t = 0;
  int b = 0;

  /* Start with the 'left' border. The border can be a percent value of aWidth. */
  aString = ParseNumber2( aString, aWidth, &l );

  /* Is there anything more to parse? The numbers are delimited by the ',' sign. */
  if ( *aString == ',' )
  {
    /* Parse the second number as the 'right' border. The border can be a percent
       value of aWidth. */
    aString = ParseNumber2( aString + 1, aWidth, &r );

    /* Then continue with the 'top' border... */
    if ( *aString == ',' )
    {
      aString = ParseNumber( aString + 1, &t );

      /* ... and 'bottom' border */
      if ( *aString == ',' )
        aString = ParseNumber( aString + 1, &b );
    }
  }

  /* Ensure the given width aWidth is not exceeded by the left border */
  if ( l > aWidth )
    l = aWidth;

  /* ... and by the right border */
  if ( r > ( aWidth - l ))
    r = aWidth - l;

  /* Return the determinated borders */
  *aLeft   = l;
  *aRight  = r;
  *aTop    = t;
  *aBottom = b;

  return aString;
}


/* This function verifies, whether the next sign or signs in aString define a
   valid horizontal alignment and returns it in the aAlign argument. Following
   table shows the relation between the found alignment signs at the resulting
   value returned by the function. Returning uppercase letter means justified
   mode:
   'l'          -> 'l' -> left
   'r'          -> 'r' -> right
   'c'          -> 'c' -> center
   'a'          -> 'a' -> auto left/right
   'j'          -> 'L' -> left + justified
   'lj' or 'jl' -> 'L' -> left + justified
   'rj' or 'jr' -> 'R' -> right + justified
   'cj' or 'jc' -> 'C' -> center + justified
   'aj' or 'ja' -> 'A' -> auto left/right + justified
   any-other    -> 'l' -> left */
static XChar* ParseAlignH( XChar* aString, int* aAlign )
{
  int   tmp = 'l'; /* Left alignment per default */
  XChar c1  = *aString;
  XChar c2  = c1? aString[1] : 0;

  if ((( c1 == 'l' ) || ( c1 == 'r' ) || ( c1 == 'c' ) || ( c1 == 'a' )) &&
       ( c2 == 'j' ))
  {
    tmp = c1 + ( 'A' - 'a' );
    aString += 2;
  }

  else if (( c1 == 'j' ) &&
          (( c2 == 'l' ) || ( c2 == 'r' ) || ( c2 == 'c' ) || ( c2 == 'a' )))
  {
    tmp = c2 + ( 'A' - 'a' );
    aString += 2;
  }

  else if (( c1 == 'l' ) || ( c1 == 'r' ) || ( c1 == 'c' ) || ( c1 == 'a' ))
  {
    tmp = c1;
    aString++;
  }

  else if ( c1 == 'j' )
  {
    tmp = 'L';
    aString++;
  }

  /* Return the determinated alignment */
  *aAlign = tmp;

  return aString;
}


/* This function verifies, whether the next sign in aString defines a valid
   vertical alignment and returns it in the aAlign argument. If no alignment
   is specified the 'Middle' alignment is returned. */
static XChar* ParseAlignV( XChar* aString, int* aAlign )
{
  int   tmp = 'm'; /* Middle alignment per default */
  XChar c   = *aString;

  /* Is this a valid vertical alignment? (Top/Middle/Bottom) */
  if (( c == 't' ) || ( c == 'm' ) || ( c == 'b' ))
    tmp = *aString++;

  /* Return the determinated alignment */
  *aAlign = tmp;

  return aString;
}


/* This function parses the width of up to 3 layout columns. The number of
   columns is returned in aCount argument. The width of these columns is
   returned in aColumns. For each column the function accepts a number or a
   '*' sign. If the number is specified, the width of the column is fixed.
   Columns signed with '*' are elastic - they are stretched in order to 
   fill the remaining width, not occupied by other columns. The entire width
   is specified in the aWidth argument. */
static XChar* ParseColumns( XChar* aString, int aWidth, int* aCount,
  int* aColumns )
{
  /* 'tmp' will store the width of the layout columns temporary. */
  int   tmp[3] = { 0, 0, 0 };
  XChar c  = *aString;
  int   i  = 0;
  int   j  = 0;
  int   ew = aWidth;
  int   ec = 0;

  /* Get the width of up to 3 layout columns. Columns signed with '*' are 
     elastic. */
  while (( i < 3 ) && ((( c >= '0' ) && ( c <= '9' ))  || ( c == '*' )))
  {
    /* Is this an elastic column? In this case the width is calculated later. */
    if ( c == '*' )
    {
      tmp[i] = -1;
      ec++;
      aString++;
    }

    /* Otherwise get the exact width of the column in pixel. */
    else
    {
      aString = ParseNumber2( aString, aWidth, tmp + i );
      ew      = ew - tmp[i];
    }

    /* Skip over the delimiter between the columns. */
    if ( *aString == ',' )
      aString++;

    /* Continue with the next sign. */
    c = *aString;
    i++;
  }

  /* Default layout with a single column? */
  if ( !i )
  {
    *tmp = aWidth;
    i    = 1;
  }

  /* Adapt the width of elastic columns (if any) */
  for ( ; j < ec; j++ )
  {
    int w = ew / ec;

    /* The last elastic column will take the remaining width */
    if ( j == ( ec - 1 ))
      w = ew - ( ec - 1 ) * w;

    if      ( tmp[0] < 0 ) tmp[0] = w;
    else if ( tmp[1] < 0 ) tmp[1] = w;
    else if ( tmp[2] < 0 ) tmp[2] = w;
  }

  /* Set the number of available columns ... */
  *aCount = i;

  /* ... and store the width of these columns in the aColumns array. */
  for ( i = 0; i < 3; i++ )
  {
    /* All columns have to have positive width. */
    if ( tmp[i] < 0 )
      tmp[i] = 0;

    /* Ensure, the entire width is not exceeded. */
    if ( tmp[i] > aWidth )
      tmp[i] = aWidth;

    aWidth = aWidth - tmp[i];

    /* Store the result for the caller. */
    aColumns[i] = tmp[i];
  }

  return aString;
}


/* This function verifies, whether the string aString contains the given keyword
   aKeyword. If true, the function returns a value != 0 */
static int IsKeyword( XChar* aString, const char* aKeyword )
{
  /* Repeat until the entire keyword is processed */
  while ( *aKeyword && ( *aKeyword == *aString ))
  {
    aKeyword++;
    aString++;
  }

  return !*aKeyword;
}


/* The function evaluates the given string aString as containing a paragraph
   and searches for an attribute identifying the end of this paragraph or for
   the end of the string. Finally returns the character which does not belong
   to the paragraph. */
static XChar* FindEndOfParagraph( XChar* aString )
{
  /* Search the string until its end */
  for ( ; *aString; aString++ )
  {
    /* The attributes {par, {lay, {col and {end terminate the paragraph */
    if (( *aString == '{' ) && ( IsKeyword( aString + 1, "par" ) ||
           IsKeyword( aString + 1, "lay" ) || 
           IsKeyword( aString + 1, "col" ) ||
           IsKeyword( aString + 1, "end" )))
      break;

    /* Skip over escape sequences starting with the '%' sign */
    if (( *aString == '%' ) && aString[1])
      aString++;
  }

  return aString;
}


/* The function processes the given string aString till the end aEnd as content
   of a paragraph and stores in aResChars and aResAttrs the resulting character
   codes with their corresponding attributes. All {clr, {fnt and {ul attributes
   are evaluated during this phase affecting the attributes of all following
   characters as these are stored in aResAttrs. If the paragraph contains links
   {lnk:...}, the link number enclosing the character is stored in aResLinks.
   If the character is not enclosed in any link, the corresponding link id is 0.

   If the function encounters the {img attribute, it stores in aResChars the
   code \xFEFF representing an 'embedded content' and in aResAttrs the numbers
   of the corresponding bitmap and frame. In aResLinks stores the number of the
   link enclosing this image or 0 if no link is enclosing the image.

   The function returns the number of entities resulting in aResChars, 
   aResAttrs and aResLinks. */
static int ParseParagraph( XChar* aString, XChar* aEnd, XChar* aResChars, 
  XUInt32* aResAttrs, XChar* aResLinks, int* aLinkCount, int* aLinkNamesArea )
{
  XChar* startResChars = aResChars;
  int    colorNo       = 0;
  int    fontNo        = 0;
  int    ul            = 0;
  int    linkOn        = 0;

  /* Repeat until the entire content of the paragraph has been processed */
  while ( aString < aEnd )
  {
    XChar ch = *aString;

    /* Is this an attribute? */
    if ( ch == '{' )
    {
      /* Evaluate the image attribute */
      if ( IsKeyword( aString, "{img" ))
      {
        int bitmapNo = 0;
        int frameNo  = 0;
        int align    = 0;

        /* Skip over the alignment and ... */
        aString = ParseAlignV( aString + 4, &align );

        /* ... get the index of the bitmap */
        aString = ParseNumber( aString, &bitmapNo );

        /* Frame number specified? */
        if ( *aString == '.' )
        {
          aString++;
          aString = ParseNumber( aString, &frameNo );
        }

        if ( *aString == '}' )
          aString++;

        /* If the specified numbers are out of range - fall back to default 
           number 0 */
        if ( frameNo  > 4095 ) frameNo  = 0;
        if ( bitmapNo > 255  ) bitmapNo = 0;

        *aResChars++ = 0xFEFF;
        *aResAttrs++ = ATTR_IMAGE( colorNo, bitmapNo, frameNo, align );
        *aResLinks++ = (XChar)( linkOn? *aLinkCount : 0 );
        continue;
      }

      /* Evaluate the change font attribute */
      if ( IsKeyword( aString, "{fnt" ))
      {
        if ( *( aString = ParseNumber( aString + 4, &fontNo )) == '}' )
          aString++;

        /* If the specified number is out of range - fall back to default 
           number 0 */
        if ( fontNo > 255 ) fontNo = 0;
        continue;
      }

      /* Skip over the color selection attribute */
      if ( IsKeyword( aString, "{clr" ))
      {
        if ( *( aString = ParseNumber( aString + 4, &colorNo )) == '}' )
          aString++;

        /* If the specified number is out of range - fall back to default 
           number 0 */
        if ( colorNo > 255 ) colorNo = 0;
        continue;
      }

      /* Skip over the underline on attribute */
      if ( IsKeyword( aString, "{ul+}" ))
      {
        ul = 1;
        aString += 5;
        continue;
      }

      /* Skip over the underline off attribute */
      if ( IsKeyword( aString, "{ul-}" ))
      {
        ul = 0;
        aString += 5;
        continue;
      }

      /* Skip over the link attribute */
      if ( IsKeyword( aString, "{lnk:" ))
      {
        aString     += 5;
        linkOn       = 1;
        *aLinkCount += 1;

        /* Skip over the link name */
        while (( aString < aEnd ) && ( *aString != '}' ) && ( *aString != '{' ))
        {
          aString++;
          *aLinkNamesArea += 1;
        }

        if ( *aString == '}' )
          aString++;

        continue;
      }

      /* Skip over the link attribute */
      if ( IsKeyword( aString, "{lnk}" ))
      {
        aString += 5;
        linkOn  = 0;
        continue;
      }
    }

    /* Alias for the non-zero width space character? */
    if ( ch == '^' )
    {
      *aResChars++ = 0x200B;
      *aResAttrs++ = ATTR_GLYPH( colorNo, fontNo, ul );
      *aResLinks++ = (XChar)( linkOn? *aLinkCount : 0 );
      aString++;
      continue;
    }

    /* Alias for the soft-hyphen? */
    if ( ch == '~' )
    {
      *aResChars++ = 0xAD;
      *aResAttrs++ = ATTR_GLYPH( colorNo, fontNo, ul );
      *aResLinks++ = (XChar)( linkOn? *aLinkCount : 0 );
      aString++;
      continue;
    }

    /* Starting an escape sequence? */
    if ( ch == '%' )
    {
      ch = *++aString;
      if ( aString == aEnd )
        break;
    }
 
    *aResChars++ = ch;
    *aResAttrs++ = ATTR_GLYPH( colorNo, fontNo, ul );
    *aResLinks++ = (XChar)( linkOn? *aLinkCount : 0 );
    aString++;
  }

  /* Terminate the parsed string */
  *aResChars = 0;
  *aResAttrs = 0;
  *aResLinks = 0;

  return (int)( aResChars - startResChars );
}


/* Forward declaration */
static XChar* Paragraph( XAttrSet* aAttrSet, XChar* aString, int aWidth,
  int aAlignH, int aEnableBidiText, XBuffer* aDest, int* aHeight,
  int* aLastLeading, int* aIsRTL, int* aLinkCount, int* aLinkNamesArea );

static XChar* Layout( XAttrSet* aAttrSet, XChar* aString, int aCount, 
  int* aColumns, int aEnableBidiText, XBuffer* aDest, int* aHeight,
  int* aLastLeading, int* aIsRTL, int* aLinkCount, int* aLinkNamesArea );


/* This function parses the body block in aString and stores the preprocessed
   attributes in aDest buffer. The function determinates the height occupied by
   the body and returns it in aHeight argument. */
static XChar* Body( XAttrSet* aAttrSet, XChar* aString, int aWidth,
  int aEnableBidiText, XBuffer* aDest, int* aHeight, int* aLastLeading,
  int* aIsRTL, int* aLinkCount, int* aLinkNamesArea )
{
  /* 'y' stores the vertical offset to the paragraphs */
  int y;
  int lastLeading;
  int tmpInx = BUFSIZE( aDest );

  /* It is allowed to start the body without the {par ... attribute. In this
     case a default left aligned paragraph is assumed. Reserve space for this
     paragraph. The paragraph starts always with a 'group' */
  if ( !RequireBuffer( aDest, 7 ))
    return 0;

  /* Store the clipping area and width of the paragraph's interior */
  *aDest->Ptr++ = 'G';
  *aDest->Ptr++ = NUMWORD( 0 );
  *aDest->Ptr++ = NUMWORD( 0 );
  *aDest->Ptr++ = NUMWORD( aWidth );
  *aDest->Ptr++ = 0;     /* The height is still unknown */
  *aDest->Ptr++ = 0;     /* The length of the paragraph is still unknown */
  *aDest->Ptr++ = 0;     /* Additional horizontal offset */

  /* Try to parse the paragraph's content */
  aString = Paragraph( aAttrSet, aString, aWidth, 'l', aEnableBidiText, aDest,
                       &y, &lastLeading, aIsRTL, aLinkCount, aLinkNamesArea );

  /* Did the paragraph contain any text/images? Then update the paragraph's
     height and the offset to the next paragraph in the buffer. */
  if ( BUFSIZE( aDest ) > ( tmpInx + 7 ))
  {
    aDest->Buffer[ tmpInx + 4 ] = NUMWORD( y );
    aDest->Buffer[ tmpInx + 5 ] = NUMWORD( BUFSIZE( aDest ) - tmpInx );
  }

  /* Otherwise the paragraph is empty and the group is useless. Discard the
     previously prepared insruction */
  else
    aDest->Ptr -= 7;

  /* Repeat until the whole source string is evaluated or an unknown attribute 
     is found. */
  while ( aString && *aString )
  {
    /* Is this a begin of a new paragraph? */
    if ( IsKeyword( aString, "{par" ))
    {
      int l, r, t, b;
      int alignH;
      int width;
      int height;
      int bc = -1;

      /* Get the alignment and the borders of the paragraph */
      aString = ParseAlignH ( aString + 4, &alignH );
      aString = ParseBorders( aString, aWidth, &l, &r, &t, &b );

      /* Is the paragraph filled with a solid color? */
      if ( *aString == ':' )
        aString = ParseNumber( aString + 1, &bc );

      /* Skip over the '}' delimiter at the end of the {par attribute */
      if ( *aString == '}' )
        aString++;

      /* Prepare the drawing statement for the filling of the paragraph */
      if ( bc != -1 )
      {
        if ( !RequireBuffer( aDest, 6 ))
          return 0;

        /* The entire column should be filled with the solid color */
        *aDest->Ptr++ = 'R';
        *aDest->Ptr++ = NUMWORD( 0 );
        *aDest->Ptr++ = NUMWORD( y + lastLeading );
        *aDest->Ptr++ = NUMWORD( aWidth );
        *aDest->Ptr++ = 0;   /* The height is still unknown */
        *aDest->Ptr++ = NUMWORD( bc );
      }

      /* Remember the start position of the paragraph */
      tmpInx = BUFSIZE( aDest );

      /* Reserve space for the paragraph's attribute. The paragraph starts
         always with a 'group' */
      if ( !RequireBuffer( aDest, 7 ))
        return 0;

      /* Calculate the width of the paragraph's content. The height is still
         unknown. */
      width = aWidth - l - r;

      /* Store the clipping area and width of the paragraph's interior */
      *aDest->Ptr++ = 'G';
      *aDest->Ptr++ = NUMWORD( 0 );
      *aDest->Ptr++ = NUMWORD(( y += lastLeading ) + t );
      *aDest->Ptr++ = NUMWORD( aWidth );
      *aDest->Ptr++ = 0;     /* The height is still unknown */
      *aDest->Ptr++ = 0;     /* The length of the paragraph is still unknown */
      *aDest->Ptr++ = NUMWORD( l ); /* Additional horizontal offset */

      /* Now, parse the paragraph's content and ... */
      aString = Paragraph( aAttrSet, aString, width, alignH, aEnableBidiText,
                           aDest, &height, &lastLeading, aIsRTL, aLinkCount, 
                           aLinkNamesArea );

      /* Aborted? */
      if ( !aString )
        return 0;

      /* Where does the next paragraph appear? */
      y += t + height + b;

      /* Update the paragraph's height and the offset to the next paragraph
         in the buffer. */
      aDest->Buffer[ tmpInx + 4 ] = NUMWORD( y - b );
      aDest->Buffer[ tmpInx + 5 ] = NUMWORD( BUFSIZE( aDest ) - tmpInx );

      /* Don't forget to adjust the rectangle of the filling paragraph */
      if ( bc != -1 )
        aDest->Buffer[ tmpInx - 2 ] = NUMWORD( y );
    }

    /* Is this a begin of a layout? */
    else if ( IsKeyword( aString, "{lay" ))
    {
      int columns[3];
      int count  = 0;
      int height = 0;

      tmpInx = BUFSIZE( aDest );

      /* Get the number and the width of the layout columns. */
      aString = ParseColumns( aString + 4, aWidth, &count, columns );

      /* Skip over the '}' delimiter at the end of {lay */
      if ( *aString == '}' )
        aString++;

      /* Reserve space for the layout's attribute. The layout starts always 
         with a group */
      if ( !RequireBuffer( aDest, 7 ))
        return 0;

      /* Store the origin and the size of the layout's group */
      *aDest->Ptr++ = 'G';
      *aDest->Ptr++ = NUMWORD( 0 );
      *aDest->Ptr++ = NUMWORD( y += lastLeading );
      *aDest->Ptr++ = NUMWORD( aWidth );
      *aDest->Ptr++ = 0;     /* The height is still unknown */
      *aDest->Ptr++ = 0;     /* The length of the paragraph is still unknown */
      *aDest->Ptr++ = NUMWORD( 0 ); /* Additional horizontal offset */

      /* Now, parse the layout's content and ... */
      aString = Layout( aAttrSet, aString, count, columns, aEnableBidiText,
                        aDest, &height, &lastLeading, aIsRTL, aLinkCount, 
                        aLinkNamesArea );

      /* Aborted? */
      if ( !aString )
        return 0;

      /* Where does the next paragraph appear ? */
      y += height;

      /* ... update the layout's height and the offset to the next paragraph
         in the buffer. */
      aDest->Buffer[ tmpInx + 4 ] = NUMWORD( y );
      aDest->Buffer[ tmpInx + 5 ] = NUMWORD( BUFSIZE( aDest ) - tmpInx );
    }

    /* Neither paragraph nor layout found - return to the caller */
    else
      break;
  }

  /* Return the entire height of the body to the caller. */
  *aHeight      = y;
  *aLastLeading = lastLeading;

  return aString;
}


/* This function parses the layout in aString and stores the preprocessed
   attributes in aDest buffer. The function determinates the height occupied by
   the layout's body and returns it in aHeight argument. */
static XChar* Layout( XAttrSet* aAttrSet, XChar* aString, int aCount, 
  int* aColumns, int aEnableBidiText, XBuffer* aDest, int* aHeight, 
  int* aLastLeading, int* aIsRTL, int* aLinkCount, int* aLinkNamesArea )
{
  int aligns[3];
  int colInx[3];
  int colH  [3];
  int bc    [3] = { -1, -1, -1 };
  int ll    [3] = {  0,  0,  0 };
  int i      = 0;
  int j      = 0;
  int x      = 0;
  int height = 0;
  int total  = 0;

  /* Evaluate up to aCount columns of the layout. */
  while ( aString && *aString && ( i < aCount ))
  {
    /* Is this a begin of the next layout's column? */
    if ( IsKeyword( aString, "{col" ))
    {
      int width;
      int l, r, t, b;

      /* Evaluate the vertical alignment and the borders of the column. */
      aString = ParseAlignV( aString + 4, aligns + i );
      aString = ParseBorders( aString, aColumns[i], &l, &r, &t, &b );

      /* Is the column filled with a solid color? */
      if ( *aString == ':' )
        aString = ParseNumber( aString + 1, bc + i );

      /* Skip over the '}' delimiter at the end of the {lay attribute */
      if ( *aString == '}' )
        aString++;

      /* Prepare the drawing statement for the filling of the column */
      if ( bc[i] != -1 )
      {
        if ( !RequireBuffer( aDest, 6 ))
          return 0;

        /* The entire column should be filled with the solid color */
        *aDest->Ptr++ = 'R';
        *aDest->Ptr++ = NUMWORD( x );
        *aDest->Ptr++ = NUMWORD( 0 );
        *aDest->Ptr++ = NUMWORD( x + aColumns[i]);
        *aDest->Ptr++ = 0;   /* The height is still unknown. */
        *aDest->Ptr++ = NUMWORD( bc[i]);
      }

      /* Remember the start position of the column */
      colInx[i] = BUFSIZE( aDest );

      /* Reserve space for the column's header. The column starts always 
         with a group */
      if ( !RequireBuffer( aDest, 7 ))
        return 0;

      /* The width of the column's content */
      width = aColumns[i] - l - r;

      /* Store the origin and the size of the column's group */
      *aDest->Ptr++ = 'G';
      *aDest->Ptr++ = NUMWORD( x + l );
      *aDest->Ptr++ = NUMWORD( t );
      *aDest->Ptr++ = NUMWORD( x + l + width );
      *aDest->Ptr++ = 0;   /* The height is still unknown. */
      *aDest->Ptr++ = 0;   /* The length of the column is still unknown */
      *aDest->Ptr++ = 0;   /* Additional horizontal offset */

      /* Parse the body. */
      aString = Body( aAttrSet, aString, width, aEnableBidiText, aDest, 
                      colH + i, ll + i, aIsRTL, aLinkCount, aLinkNamesArea );

      /* Aborted? */
      if ( !aString )
        return 0;

      /* Remember the height of the highest column. Also do this by taking the
         remaining leading in acount */
      colH[i] += t + b;
      height   = MAX( height, colH[i]);
      total    = MAX( total,  colH[i] + ll[i]);

      /* ... update the layout's height and the offset to the next paragraph
         in the buffer. */
      aDest->Buffer[ colInx[i] + 4 ] = NUMWORD( colH[i] - b );
      aDest->Buffer[ colInx[i] + 5 ] = NUMWORD( BUFSIZE( aDest ) - colInx[i]);

      /* Where does the next column appear? */
      x += aColumns[i];

      /* Continue with next column */
      i++;
    }

    /* Neither paragraph nor layout column found - return to the caller */
    else
      break;
  }

  /* Now, we know about the height and alignment of all columns. Arrange the 
     columns depending on their alignment. */
  for ( j = 0; j < i; j++ )
  {
    int tmpInx = colInx[j];
    int o      = height - colH[j];

    /* calculate the vertical movement of the column 'j'. */
    switch ( aligns[j])
    {
      case 't' : o  = 0; break;
      case 'm' : o /= 2; break;
    }

    /* ... and adapt the column's vertical position. */
    aDest->Buffer[ tmpInx + 2 ] = (unsigned short)( aDest->Buffer[ tmpInx + 2 ] + 
      NUMWORD( o ));
    aDest->Buffer[ tmpInx + 4 ] = (unsigned short)( aDest->Buffer[ tmpInx + 4 ] +
      NUMWORD( o ));

    /* Don't forget to adjust the background rectangle if the column is
       filled */
    if ( bc[j] != -1 )
      aDest->Buffer[ tmpInx - 2 ] = NUMWORD( height );
  }

  /* Remember the height of the entire layout. */
  *aHeight      = height;
  *aLastLeading = total - height;

  /* The layout attribute should be terminated with the {end} keyword. */
  if ( IsKeyword( aString, "{end}" ))
    aString += 5;

  return aString;
}


/* This function evaluates the entities in aChars and aAttrs and searches for 
   the end of the next row fitting within aMaxWidth. The function determinates
   the width, the height of the line and returns back the number of processed
   entities. */
static int ProcessAttrTextRow( XAttrSet* aAttrSet, XChar* aChars, 
  XUInt32* aAttrs, int aMaxWidth, XHandle aBidi, int aBidiIndex, int* aWidth,
  int* aHeight, int* aSpace, int* aBaseLine, int* aLeading )
{
  XChar*   startChars   = aChars;
  XFont*   font         = 0;
  int      fontNo       = -1;
  XChar*   wrapChars    = 0;
  XUInt32* wrapAttrs    = 0;
  int      wrapWidth    = 0;
  int      wrapHeight   = 0;
  int      wrapSpace    = 0;
  int      wrapBaseLine = 0;
  int      wrapDescent  = 0;
  int      wrapLeading  = 0;
  int      wrapActive   = 0;
  int      descent      = 0;
  XChar    prevCode     = 0;
  int      prevLevel    = -1;

  *aWidth    = 0;
  *aHeight   = 0;
  *aSpace    = 0;
  *aBaseLine = 0;
  *aLeading  = 0;

  /* Repeat until the entire content of the paragraph is evaluated */
  for ( ; *aChars; aChars++, aAttrs++ )
  {
    XUInt32 attr        = *aAttrs;
    XChar   code        = *aChars;
    int     type        = GET_ATTR_TYPE( attr );
    int     curWidth    = 0;
    int     curHeight   = 0;
    int     curSpace    = 0;
    int     curBaseLine = 0;
    int     curDescent  = 0;
    int     curLeading  = 0;

    /* Track the font */
    if ((( type == TYPE_GLYPH ) || ( type == TYPE_GLYPH_UL )) &&
         ( GET_ATTR_FONT( attr ) != (unsigned int)fontNo ))
    {
      font = EwGetAttrFont( aAttrSet, ( fontNo = GET_ATTR_FONT( attr )));
      
      /* If the desired font is not available in the AttrSet -> Use the
         default font #0 */
      if ( !font )
        font  = EwGetAttrFont( aAttrSet, ( fontNo = 0 ));

      /* No kerning between glyphs from different fonts */
      prevLevel = -1;
      prevCode  = 0;
    }

    /* The <NewLine> affects the height of the line */
    if (( code == '\n' ) && font )
    {
      *aBaseLine = MAX( *aBaseLine, font->Ascent );
      *aLeading  = MAX( *aLeading,  font->Leading );
      descent    = MAX( descent,    font->Descent );
    }

    /* <NewLine> sign terminates the line */
    if ( code == '\n' )
      break;

    /* The actual entity is an image */
    if (( type == TYPE_IMAGE_MIDDLE ) || ( type == TYPE_IMAGE_BOTTOM ) ||
        ( type == TYPE_IMAGE_TOP ))
    {
      XBitmap* bmp = EwGetAttrBitmap( aAttrSet, GET_ATTR_BITMAP( attr ));

      /* If the desired bitmap is not available in the AttrSet -> Use the
         default bitmap #0 */
      if ( !bmp )
        bmp = EwGetAttrBitmap( aAttrSet, 0 );

      /* Get the size of the bitmap */
      if ( bmp )
      {
        curWidth  = bmp->FrameSize.X;
        curHeight = bmp->FrameSize.Y;
      }

      /* No kerning between images and glyphs */
      prevLevel = -1;
      prevCode  = 0;
    }

    /* ... or it is an ordinary glyph or special character */
    else
    {
      XGlyphMetrics glyph;

      /* Is this a line wrap candidate! Don't wrap at the begin of a line! */
      if (( code == 0x200B ) && ( !*aWidth || wrapActive ))
        code = 0xFEFF;

      /* The soft-hyphen lies at the begin of a text row or it follows another
         wrap candidate - ignore this hyphen */
      if (( code == 0x00AD ) && ( !*aWidth || wrapActive ))
        code = 0xFEFF;

      /* Blank at the begin of the row can't cause line wrap */
      if (( code == ' ' ) && !*aWidth )
        code = 0x00A0;

      /* Is this a line wrap candidate? */
      if (( code == ' ' ) || ( code == 0x200B ))
      {
        /* Forget the preceding wrap candidate if it was soft-hyphen */
        if ( wrapChars && ( *wrapChars == 0x00AD ))
          *wrapChars = 0xFEFF;

        wrapChars    = aChars;
        wrapAttrs    = aAttrs;
        wrapWidth    = *aWidth;
        wrapHeight   = *aHeight;
        wrapSpace    = *aSpace;
        wrapBaseLine = *aBaseLine;
        wrapLeading  = *aLeading;
        wrapDescent  = descent;
        wrapActive   = 1;
      }

      /* Is this a soft-hyphen? This sign does not appear on the screen unless
         line wrap is applied at its position -> then a '-' minus sign is 
         shown. */
      if ( code == 0x00AD )
      {
        /* If applied - the '-' minus sign should be displayed ... Get the size
           of the '-' minus sign ... */
        if ( font && *aWidth && EwGetGlyphMetrics( font, code, &glyph ))
        {
          int level = aBidi? EwBidiGetCharLevel( aBidi, aBidiIndex + 
                                            (int)( aChars - startChars )) : 0;
          curWidth    = glyph.Advance;
          curBaseLine = font->Ascent;
          curDescent  = font->Descent;
          curLeading  = font->Leading;

          /* Does the actual character and the preceding one belong to the same
             Bidi level run? Then calculate the kerning between them. Otherwise,
             the glyphs will be separated after reordering of the text row, so no
             kerning is applicable on them */
          if ( level == prevLevel )
          {
            if ( level & 1 )
              curWidth += EwFntGetKerning( font->Handle, code, prevCode );
            else
              curWidth += EwFntGetKerning( font->Handle, prevCode, code );
          }
        }

        /* ... and check, whether there is space in the current line for this '-' 
           sign */
        if ((( *aWidth + curWidth ) <= aMaxWidth ) && curWidth )
        {
          /* Forget the preceding wrap candidate if it was soft-hyphen */
          if ( wrapChars && ( *wrapChars == 0xAD ))
            *wrapChars = 0xFEFF;

          wrapChars    = aChars;
          wrapAttrs    = aAttrs;
          wrapWidth    = *aWidth + curWidth;
          wrapBaseLine = MAX( *aBaseLine, curBaseLine );
          wrapLeading  = MAX( *aLeading,  curLeading );
          wrapDescent  = MAX( descent,    curDescent );
          wrapSpace    = *aSpace;
          wrapActive   = 1;
        }

        /* As described above, soft-hyphen is ignored. */
        continue;
      }

      /* Replace all BIDI control character codes by zero-width-non-break-space */
      if (( code == 0x061C ) ||
         (( code >= 0x200B ) && ( code <= 0x200F )) ||
         (( code >= 0x202A ) && ( code <= 0x202E )) ||
         (( code >= 0x2066 ) && ( code <= 0x2069 )))
        code = 0xFEFF;

      /* The preceding steps have eventually eliminated the glyph (it is zero-
         width sign now). Zero-width signs should not have impact on the text
         row dimension */
      if ( code == 0xFEFF )
      {
        *aChars = 0xFEFF;
        continue;
      }

      /* A visible character has been fopund - end the wrap mode */
      if ( wrapChars != aChars )
        wrapActive = 0;

      /* Trying to access the horizontal-ellipsis sign, but it is not available
         in the font. Use 3x dot sign instead */
      if ( font && ( code == 0x2026 ) && !EwIsGlyphAvailable( font, code ) &&
            EwGetGlyphMetrics( font, '.', &glyph ))
      {
        int level = aBidi? EwBidiGetCharLevel( aBidi, aBidiIndex + 
                                         (int)( aChars - startChars )) : 0;
        curWidth    = glyph.Advance * 3;
        curWidth   += EwFntGetKerning( font->Handle, '.', '.' ) * 2;
        curBaseLine = font->Ascent;
        curDescent  = font->Descent;
        curLeading  = font->Leading;

        /* Does the actual character and the preceding one belong to the same
           Bidi level run? Then calculate the kerning between them. Otherwise,
           the glyphs will be separated after reordering of the text row, so no
           kerning is applicable on them */
        if ( level == prevLevel )
        {
          if ( level & 1 )
            curWidth += EwFntGetKerning( font->Handle, '.', prevCode );
          else
            curWidth += EwFntGetKerning( font->Handle, prevCode, '.' );
        }

        prevCode  = '.';
        prevLevel = level;
      }

      /* Get the metric of the affected glyph - Remember the size of the glyph
         for further processing */
      else if ( font && EwGetGlyphMetrics( font, code, &glyph ))
      {
        int level = aBidi? EwBidiGetCharLevel( aBidi, aBidiIndex + 
                                          (int)( aChars - startChars )) : 0;
        curWidth    = glyph.Advance;
        curBaseLine = font->Ascent;
        curDescent  = font->Descent;
        curLeading  = font->Leading;

        /* Count the width of all blank signs - for justified text only */
        if (( code == ' ' ) || ( code == 0x00A0 ))
          curSpace = curWidth;

        /* Does the actual character and the preceding one belong to the same
           Bidi level run? Then calculate the kerning between them. Otherwise,
           the glyphs will be separated after reordering of the text row, so no
           kerning is applicable on them */
        if ( level == prevLevel )
        {
          if ( level & 1 )
            curWidth += EwFntGetKerning( font->Handle, code, prevCode );
          else
            curWidth += EwFntGetKerning( font->Handle, prevCode, code );
        }

        prevCode  = code;
        prevLevel = level;
      }
    }

    /* Test, whether the sign or the image can be displayed within the line.
       To avoid endless line wrapping, the first sign is always taken over! */
    if ((( curWidth + *aWidth ) <= aMaxWidth ) || !*aWidth )
    {
      *aWidth   += curWidth;
      *aHeight   = MAX( *aHeight,   curHeight );
      *aBaseLine = MAX( *aBaseLine, curBaseLine );
      *aLeading  = MAX( *aLeading,  curLeading );
      descent    = MAX( descent,    curDescent );
      *aSpace   += curSpace;
      continue;
    }

    /* There is not enought space for the sign - skip back to the last wrap
       canditate - if any! */
    if ( wrapChars )
    {
      *aWidth     = wrapWidth;
      *aHeight    = wrapHeight;
      *aSpace     = wrapSpace;
      *aBaseLine  = wrapBaseLine;
      *aLeading   = wrapLeading;
      descent     = wrapDescent;
      aChars      = wrapChars + 1;
      aAttrs      = wrapAttrs + 1;

      /* Breaking a line at blank? Replace it by a zero-width sign */
      if ( *wrapChars == ' ' )
        *wrapChars = 0xFEFF;

      /* Any zero-width glyphs following the wrap position? Then append them
         to the actual row. */
      while (((( type = GET_ATTR_TYPE( *aAttrs )) == TYPE_GLYPH ) ||
               ( type == TYPE_GLYPH_UL )) &&
               ((( code = *aChars ) == 0xFEFF ) || ( code == 0x200B ) || 
                 ( code == 0xAD )))
      {
        aChars++;
        aAttrs++;
      }

      /* The wrap position is processed */
      wrapChars = 0;
    }

    /* The line is completed */
    break;
  }

  /* Forget the preceding wrap candidate if it was soft-hyphen */
  if ( wrapChars && ( *wrapChars == 0xAD ))
    *wrapChars = 0xFEFF;

  /* Adapt the height of the text line to the heighest font/image. Adjust the
     baseline to the center of the line */
  *aHeight    = MAX( *aHeight, *aBaseLine + descent );
  *aBaseLine += ( *aHeight - *aBaseLine - descent ) / 2;

  return (int)( aChars - startChars );
}


/* The function converts the given string into a sequence of drawing statements
   and stores them into aDest buffer. */
static XChar* Paragraph( XAttrSet* aAttrSet, XChar* aString, int aWidth, 
  int aAlignH, int aEnableBidiText, XBuffer* aDest, int* aHeight,
  int* aLastLeading, int* aIsRTL, int* aLinkCount, int* aLinkNamesArea )
{
  XChar*   end       = FindEndOfParagraph( aString );
  XFont*   font      = EwGetAttrFont( aAttrSet, 0 );
  int      len       = (int)( end - aString + 1 );
  int      memUsage  = len * ( sizeof(XChar) + sizeof(XUInt32) + sizeof(XChar));
  int      inx       = 0;
  int      fontNo    = 0;
  int      colorNo   = 0;
  int      ulOn      = 0;
  int      linkId    = 0;
  XHandle  bidi      = 0;
  int      isRTL     = 0;
  int      linkCount = 0;
  XChar*   chars     = 0;
  XUInt32* attrs     = 0;
  XChar*   links     = 0;

  /* Try to allocate the temporarily needed memory */
  do
  {
    if ( !chars ) chars = EwAlloc( len * sizeof( XChar ));
    if ( !attrs ) attrs = EwAlloc( len * sizeof( XUInt32 ));
    if ( !links ) links = EwAlloc( len * sizeof( XChar ));
  }
  while (( !chars || !attrs || !links ) && EwImmediateReclaimMemory( 23 ));

  /* No memory for the temporarily needed buffers with processed character
     codes and their associated attributes */
  if ( !chars || !attrs || !links )
  {
    /* Report a message if there is no more memory */
    EwError( 23 );

    if ( chars ) EwFree( chars );
    if ( attrs ) EwFree( attrs );
    if ( links ) EwFree( links );
    return 0;
  }

  /* Track the temporary used memory */
  EwResourcesMemory += memUsage;

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* The entire paragraph content is evaluated now. For every entity create
     an entry in chars/attrs arrays containing information about this entry
     (glyph, image, ...) and its atributes. The resulting 'attributed string'
     is terminated with 0 (zero) sign. */
  len = ParseParagraph( aString, end, chars, attrs, links, &linkCount,
                        aLinkNamesArea );

  /* If the text should be processed by the Bidi algorithm - create a new
     Bidi context large enough for all characters from the paragraph */
  if ( len && aEnableBidiText )
    bidi = EwCreateBidi( len );

  /* Load the Bidi context with the characters. If the Bidi decides that there
     is nothing to do with the text - release the context again */
  if ( bidi && !EwBidiInit( bidi, chars, len, -1 ))
  {
    EwFreeBidi( bidi );
    bidi = 0;
  }

  /* Apply the Bidi algorithm on the text */
  if ( bidi )
  {
    EwBidiProcess     ( bidi, chars );
    EwBidiMirrorGlyphs( bidi, chars );
    EwBidiApplyShaping( bidi, chars, attrs, aAttrSet->Fonts, 
                        aAttrSet->NoOfFonts );

    /* Estimate the base direction of this paragraph */
    isRTL = EwBidiIsRTL( bidi );

    /* Track the base direction of the first processed paragraph */
    if ( *aIsRTL < 0 )
      *aIsRTL  = isRTL;
  }

  *aHeight      = 0;
  *aLastLeading = 0;

  /* Repeat until the entire paragraph's content is evaluated */
  while ( inx < len )
  {
    int   lineWidth;
    int   lineHeight;
    int   lineSpace;
    int   lineBase;
    int   lineLeading;
    int   lineAlign = aAlignH;
    int   x         = 0;
    int   lnkRegX   = 0;
    int   spanInx   = 0;
    int   spanSpace = 0;
    int   spanErr   = 0;
    XChar prevCode  = 0;
    int   count;

    /* First step: Wrap the next text line. Get its width, height, etc.
       How many entities from 'chars' 'attrs' do belong to this row? */
    count = ProcessAttrTextRow( aAttrSet, chars + inx, attrs + inx, aWidth,
              bidi, inx, &lineWidth, &lineHeight, &lineSpace, &lineBase, 
              &lineLeading );

    /* Complete the Bidi-Algorithm on the just estimated row? */
    if ( bidi )
    {
      EwBidiCompleteRow  ( bidi, inx, inx + count );
      EwBidiReorderChars ( bidi, inx, inx + count, chars + inx );
      EwBidiReorderDWords( bidi, inx, inx + count, attrs + inx );
      EwBidiReorderChars ( bidi, inx, inx + count, links + inx );
    }

    /* Should the alignment of the paragraph be derived from its base direction? */
    if ( lineAlign == 'a' ) lineAlign = isRTL? 'r' : 'l';
    if ( lineAlign == 'A' ) lineAlign = isRTL? 'R' : 'L';

    /* Is this the last row of a justified paragraph? Or the row does not contain
       any space characters? Then don't stretch it. */
    if (( lineAlign == 'L' ) || ( lineAlign == 'R' ) || ( lineAlign == 'C' ))
    {
      if (( !chars[ inx + count ] || ( chars[ inx + count ] == '\n' ) || 
            !lineSpace ))
      {
        lineAlign -= 'A' - 'a';
        lineSpace  = 0;
      }

      /* If the row should be stretched - the right/left/center alignment does
         not matter. Assume left - justified alignment */
      else
        lineAlign = 'L';
    }

    /* Adjust the horizontal offset for right or center aligned text line */
    if      ( lineAlign == 'r' ) x +=   aWidth - lineWidth;
    else if ( lineAlign == 'c' ) x += ( aWidth - lineWidth ) / 2;

    /* Second step: generate drawing statements for the text line */
    for ( ; count >= 0; count--, inx++ )
    {
      XChar         code        = count? chars[ inx ] : 0;
      XUInt32       attr        = count? attrs[ inx ] : 0;
      int           type        = GET_ATTR_TYPE( attr );
      int           newColorNo  = colorNo;
      int           newFontNo   = fontNo;
      int           newUlOn     = ulOn;
      int           newLinkId   = count? links[ inx ] : 0;
      XGlyphMetrics glyph;

      /* Ensure there is enought space for the longest drawing statement */
      if ( !RequireBuffer( aDest, 20 ))
      {
        EwFree( chars );
        EwFree( attrs );
        EwFree( links );
        EwResourcesMemory -= memUsage;

        if ( bidi )
          EwFreeBidi( bidi );

        return 0;
      }

      /* Track the currently used color */
      if (( type == TYPE_IMAGE_BOTTOM ) || ( type == TYPE_IMAGE_MIDDLE ) || 
          ( type == TYPE_IMAGE_TOP    ) || ( type == TYPE_GLYPH        ) || 
          ( type == TYPE_GLYPH_UL    ))
        newColorNo = GET_ATTR_COLOR( attr );

      /* Track the currently used font */
      if (( type == TYPE_GLYPH ) || ( type == TYPE_GLYPH_UL ))
        newFontNo = GET_ATTR_FONT( attr );

      /* Track the currently used underlined mode */
      if ( type == TYPE_GLYPH    ) newUlOn = 0;
      if ( type == TYPE_GLYPH_UL ) newUlOn = 1;

      /* Terminating the latest span? */
      if ( spanInx && ((( type != TYPE_GLYPH ) && ( type != TYPE_GLYPH_UL )) ||
         ( colorNo != newColorNo ) || ( fontNo != newFontNo ) || 
         ( ulOn != newUlOn ) || ( linkId != newLinkId )))
      {
        /* Justified text lines are stretched in order to fill the entire
           paragraph's width. Calculate the width of the stretched text */
        if ( spanSpace && ( lineAlign == 'L' ))
        {
          int tmp1 = ( spanSpace * ( aWidth - lineWidth )) + spanErr;
          int tmp2 = tmp1 / lineSpace;
          spanErr  = tmp1 - ( tmp2 * lineSpace );
          x       += tmp2;

          /* Change to 'justified' drawing statement and remember the desired
             width of the stretched text */
          aDest->Buffer[ spanInx ] = 'J';
          *aDest->Ptr++ = NUMWORD( x - WORDNUM( aDest->Buffer[ spanInx + 1 ]));
        }

        /* Remember the length of the drawing statement */
        aDest->Buffer[ spanInx + 3 ] = NUMWORD( BUFSIZE( aDest ) - spanInx );

        /* If the text is underlined - Draw a thin rectangle below the base 
           line. The thickness of the underline depends on the height of the '-'
           minus sign */
        if ( ulOn )
        {
          if ( font && EwGetGlyphMetrics( font, '-', &glyph ))
            ulOn = glyph.Size.Y;

          /* The underline is at least 1 pixel thick */
          if ( ulOn <= 0 )
            ulOn = 1;

          *aDest->Ptr++ = 'R';
          *aDest->Ptr++ = aDest->Buffer[ spanInx + 1 ];
          *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading + lineBase + ulOn );
          *aDest->Ptr++ = NUMWORD( x );
          *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading + lineBase + ( ulOn * 2 ));
          *aDest->Ptr++ = NUMWORD( colorNo );
        }

        /* The drawing statement is terminated now */
        spanInx   = 0;
        spanSpace = 0;
      }

      /* If the desired font is not available in the AttrSet -> Use the
         default font #0 */
      if (( newFontNo != fontNo ) &&
          ((( font = EwGetAttrFont( aAttrSet, newFontNo )) == 0 )))
        font = EwGetAttrFont( aAttrSet, 0 );

      /* Terminating a span enclosed within a link? */
      if ( linkId && ( linkId != newLinkId ))
      {
        *aDest->Ptr++ = 'A';
        *aDest->Ptr++ = NUMWORD( linkId + *aLinkCount );
        *aDest->Ptr++ = NUMWORD( lnkRegX );  /* Left upper corner */
        *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading );
        *aDest->Ptr++ = NUMWORD( x );        /* Right bottom corner */
        *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading + lineHeight );
        *aDest->Ptr++ = NUMWORD( lineBase );
      }

      /* Track the origin of the span enclosed in the link */
      if ( linkId != newLinkId )
        lnkRegX = x;

      /* No kerning between glyphs belonging to different fonts */
      if ( fontNo != newFontNo )
        prevCode = 0;

      /* The parameters valid from now */
      colorNo = newColorNo;
      fontNo  = newFontNo;
      ulOn    = newUlOn;
      linkId  = newLinkId;

      /* Actual entity represents an image */
      if (( type == TYPE_IMAGE_BOTTOM ) || ( type == TYPE_IMAGE_MIDDLE ) || 
          ( type == TYPE_IMAGE_TOP ))
      {
        int      bmpNo   = GET_ATTR_BITMAP( attr );
        int      frameNo = GET_ATTR_FRAME( attr );
        XBitmap* bmp     = EwGetAttrBitmap( aAttrSet, bmpNo );

        /* If the desired bitmap is not available in the AttrSet -> Use the
           default bitmap #0 */
        if ( !bmp )
          bmp = EwGetAttrBitmap( aAttrSet, ( bmpNo = 0 ));

        /* Invalid frame? Use the frame #0 */
        if ( bmp && ( frameNo >= bmp->NoOfVirtFrames ))
          frameNo = 0;

        /* Store a single drawing image statement */
        if ( bmp )
        {
          int ofs = lineHeight - bmp->FrameSize.Y;

          /* calculate the vertical offset of the image - according to its
             alignment */
          switch ( type )
          {
            case TYPE_IMAGE_MIDDLE : ofs /= 2; break;
            case TYPE_IMAGE_TOP    : ofs  = 0; break;
          }

          /* Prepare the statement */
          *aDest->Ptr++ = 'I';
          *aDest->Ptr++ = NUMWORD( x );
          *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading + ofs );
          *aDest->Ptr++ = NUMWORD( bmpNo );
          *aDest->Ptr++ = NUMWORD( frameNo );
          *aDest->Ptr++ = NUMWORD( colorNo );

          /* Get the position of the next sign/image */
          x += bmp->FrameSize.X;
        }

        /* No kerning between image and glyph */
        prevCode = 0;
      }

      /* A regular sign? */
      if ((( type == TYPE_GLYPH ) || ( type == TYPE_GLYPH_UL )) && font &&
           ( code != 0xFEFF ))
      {
        int fact = 1;

        /* Trying to access the horizontal-ellipsis sign, but it is not available
           in the font. Use 3x dot sign instead */
        if (( code == 0x2026 ) && !EwIsGlyphAvailable( font, code ))
        {
          code = '.';
          fact = 3;
        }

        /* Get the metric of the affected glyph */
        if ( EwGetGlyphMetrics( font, code, &glyph ))
        {
          int kern = EwFntGetKerning( font->Handle, prevCode, code );

          /* Ensure, there is an opened text drawing statement to take the
             sign */
          if ( !spanInx )
          {
            /* Remember the start position of the drawings statement */
            spanInx = BUFSIZE( aDest );

            /* Prepare the horizontal origin of the drawing statement. */
            *aDest->Ptr++ = 'T'; /* Left aligned text per default */
            *aDest->Ptr++ = NUMWORD( x + kern );
            *aDest->Ptr++ = NUMWORD( *aHeight + *aLastLeading + lineBase );
            *aDest->Ptr++ = 0;
            *aDest->Ptr++ = NUMWORD( fontNo );
            *aDest->Ptr++ = NUMWORD( colorNo );
          }

          /* Take over the sign and calculate the position of the next sign */
          while ( fact-- )
          {
            *aDest->Ptr++ = code;
            x            += glyph.Advance + kern;

            /* Accumulate the width of all blanks within the current span */
            if (( code == ' ' ) || ( code == 0xA0 ))
              spanSpace += glyph.Advance;

            /* Kerning between the repeated glyphs? */
            if ( fact )
              kern = EwFntGetKerning( font->Handle, code, code );
          }

          /* For the kerning of the next glyph in the row */
          prevCode = code;
        }
      }
    }

    /* 'inx' is one iteration too advanced. Restore it. */
    if ( chars[ inx - 1 ] != '\n' )
      inx--;
    
    /* Add the text line to the paragraph - the paragraph becomes heigher */
    *aHeight     += lineHeight + *aLastLeading;
    *aLastLeading = lineLeading;
  }

  /* Release the temporarily used memory */
  EwFree( chars );
  EwFree( attrs );
  EwFree( links );
  EwResourcesMemory -= memUsage;

  if ( bidi )
    EwFreeBidi( bidi );

  /* Accumulate the links found in this paragraph */
  *aLinkCount += linkCount;

  return end;
}


/* This function executes all fill rectangle drawing statements from aPtr memory
   area and draws the background of the attributed string into the aDst bitmap at
   the position aOffset. */
static void DrawAttrTextBackground( XBitmap* aDst, int aDstFrameNo, 
  XRect aClipRect, XPoint aDstPos, int aOfsX, int aOfsY, XAttrSet* aAttrSet,
  unsigned short* aPtr, unsigned short* aEnd, XBool aBlend,
  XGradient* aOpacity )
{
  /* Repeat until all drawing statements are evaluated. */
  while ( aPtr < aEnd )
  {
    /* Is this a 'rectangle' or 'group' drawing statement? */ 
    if (( *aPtr == 'G' ) || ( *aPtr == 'R' ))
    {
      XRect r;

      /* Get the clipping area of the drawing statement */
      r.Point1.X = WORDNUM( aPtr[1]) + aOfsX;
      r.Point1.Y = WORDNUM( aPtr[2]) + aOfsY;
      r.Point2.X = WORDNUM( aPtr[3]) + aOfsX;
      r.Point2.Y = WORDNUM( aPtr[4]) + aOfsY;

      /* Does the statement lie within the clipping area? */
      switch ( *aPtr )
      {
        /* Draw a nested group of drawing statements recursively */
        case 'G' :
        {
          int count = aPtr[ 5 ];

          if (( aClipRect.Point1.X < aClipRect.Point2.X ) && 
              ( aClipRect.Point1.Y < aClipRect.Point2.Y ))
            DrawAttrTextBackground( aDst, aDstFrameNo, aClipRect, aDstPos, 
              r.Point1.X + WORDNUM( aPtr[ 6 ]), r.Point1.Y, aAttrSet, 
              aPtr + 7, aPtr + count, aBlend, aOpacity );

          aPtr += count;
        } break;

        /* Draw filled rectangle */
        case 'R' :
        {
          XColor color  = EwGetAttrColor( aAttrSet, aPtr[ 5 ]);
          int    x1     = r.Point1.X - aDstPos.X;
          int    x2     = r.Point2.X - aDstPos.X;
          int    y1     = r.Point1.Y - aDstPos.Y;
          int    y2     = r.Point2.Y - aDstPos.Y;
          int    opacTL = EwGetOpacityFromGradient( aOpacity, x1, y1 ) >> 24;
          int    opacTR = EwGetOpacityFromGradient( aOpacity, x2, y1 ) >> 24;
          int    opacBR = EwGetOpacityFromGradient( aOpacity, x2, y2 ) >> 24;
          int    opacBL = EwGetOpacityFromGradient( aOpacity, x1, y2 ) >> 24;

          EwFillRectangle( aDst,aDstFrameNo, aClipRect, r, 
            BlendColor( color, opacTL ), BlendColor( color, opacTR ),
            BlendColor( color, opacBR ), BlendColor( color, opacBL ),
            aBlend );

          aPtr += 6;
        } break;
      }
    }

    /* Otherwise this is an ordinary Text drawing statement. */
    else if (( *aPtr == 'T' ) || ( *aPtr == 'J' ))
      aPtr += aPtr[ 3 ];

    /* Otherwise this is an ordinary Image drawing statement. */
    else if ( *aPtr == 'I' ) aPtr += 6;

    /* or a link span - the links are not drawn - skip over! */
    else if ( *aPtr == 'A' ) aPtr += 7;
  }
}


/* This function executes all text/image drawing statements from aPtr memory
   area and draws the content of the attributed string into the aDst bitmap at
   the position aOffset. */
static void DrawAttrTextContent( XBitmap* aDst, int aDstFrameNo, XRect aClipRect,
  XPoint aDstPos, int aOfsX, int aOfsY, XAttrSet* aAttrSet, unsigned short* aPtr,
  unsigned short* aEnd, XBool aBlend, XGradient* aOpacity )
{
  /* Repeat until all drawing statements are evaluated. */
  while ( aPtr < aEnd )
  {
    /* Is this a 'rectangle' or 'group' drawing statement? */ 
    if (( *aPtr == 'G' ) || ( *aPtr == 'R' ))
    {
      XRect r;

      /* Get the clipping area of the drawing statement */
      r.Point1.X = WORDNUM( aPtr[1]) + aOfsX;
      r.Point1.Y = WORDNUM( aPtr[2]) + aOfsY;
      r.Point2.X = WORDNUM( aPtr[3]) + aOfsX;
      r.Point2.Y = WORDNUM( aPtr[4]) + aOfsY;

      /* Does the statement lie within the clipping area? */
      switch ( *aPtr )
      {
        /* Draw a nested group of drawing statements recursively */
        case 'G' :
        {
          int count = aPtr[ 5 ];

          if (( aClipRect.Point1.X < aClipRect.Point2.X ) && 
              ( aClipRect.Point1.Y < aClipRect.Point2.Y ))
            DrawAttrTextContent( aDst, aDstFrameNo, aClipRect, aDstPos, r.Point1.X + 
              WORDNUM( aPtr[ 6 ]), r.Point1.Y, aAttrSet, aPtr + 7, aPtr + count,
              aBlend, aOpacity );

          aPtr += count;
        } break;

        /* Draw filled rectangle */
        case 'R' : aPtr += 6; break;
      }
    }

    /* Otherwise this is an ordinary drawing statement. */
    else if (( *aPtr == 'T' ) || ( *aPtr == 'J' ) || ( *aPtr == 'I' ))
    {
      /* Get the origin position for the following drawing statement */
      XPoint pos;

      pos.X = WORDNUM( aPtr[1]) + aOfsX;
      pos.Y = WORDNUM( aPtr[2]) + aOfsY;

      /* What kind of drawing should be done? */
      switch ( *aPtr )
      {
        /* Draw left aligned text */
        case 'T' :
        {
          XFont* font   = EwGetAttrFont ( aAttrSet, aPtr[ 4 ]);
          XColor color  = EwGetAttrColor( aAttrSet, aPtr[ 5 ]);
          int    count  = aPtr[ 3 ];
          int    x1     = aClipRect.Point1.X - aDstPos.X;
          int    y1     = aClipRect.Point1.Y - aDstPos.Y;
          int    x2     = aClipRect.Point2.X - aDstPos.X;
          int    y2     = aClipRect.Point2.Y - aDstPos.Y;
          int    opacTL = EwGetOpacityFromGradient( aOpacity, x1, y1 ) >> 24;
          int    opacTR = EwGetOpacityFromGradient( aOpacity, x2, y1 ) >> 24;
          int    opacBR = EwGetOpacityFromGradient( aOpacity, x2, y2 ) >> 24;
          int    opacBL = EwGetOpacityFromGradient( aOpacity, x1, y2 ) >> 24;

          /* Text drawing position relative to the origin of the destination 
             area */
          pos.X = aClipRect.Point1.X - pos.X;
          pos.Y = aClipRect.Point1.Y - pos.Y;

          /* If the desired font is not available in the AttrSet -> Use the
             default font #0 */
          if ( !font )
            font = EwGetAttrFont( aAttrSet, 0 );

          if ( font )
            EwDrawText( aDst, font, (XChar*)( aPtr + 6 ), count - 6, 
              aDstFrameNo, aClipRect, aClipRect, pos, 0, 0,
              BlendColor( color, opacTL ), BlendColor( color, opacTR ),
              BlendColor( color, opacBR ), BlendColor( color, opacBL ),
              aBlend );

          aPtr += count;
        } break;

        /* Draw justified text */
        case 'J' :
        {
          XFont* font   = EwGetAttrFont ( aAttrSet, aPtr[ 4 ]);
          XColor color  = EwGetAttrColor( aAttrSet, aPtr[ 5 ]);
          int    count  = aPtr[ 3 ];
          int    width  = WORDNUM( aPtr[ count - 1 ]);
          int    x1     = aClipRect.Point1.X - aDstPos.X;
          int    y1     = aClipRect.Point1.Y - aDstPos.Y;
          int    x2     = aClipRect.Point2.X - aDstPos.X;
          int    y2     = aClipRect.Point2.Y - aDstPos.Y;
          int    opacTL = EwGetOpacityFromGradient( aOpacity, x1, y1 ) >> 24;
          int    opacTR = EwGetOpacityFromGradient( aOpacity, x2, y1 ) >> 24;
          int    opacBR = EwGetOpacityFromGradient( aOpacity, x2, y2 ) >> 24;
          int    opacBL = EwGetOpacityFromGradient( aOpacity, x1, y2 ) >> 24;

          /* Text drawing position relative to the origin of the destination 
             area */
          pos.X = aClipRect.Point1.X - pos.X;
          pos.Y = aClipRect.Point1.Y - pos.Y;

          /* If the desired font is not available in the AttrSet -> Use the
             default font #0 */
          if ( !font )
            font = EwGetAttrFont( aAttrSet, 0 );

          if ( font )
            EwDrawText( aDst, font, (XChar*)( aPtr + 6 ), count - 7, 
                        aDstFrameNo, aClipRect, aClipRect, pos, width, 0,
              BlendColor( color, opacTL ), BlendColor( color, opacTR ),
              BlendColor( color, opacBR ), BlendColor( color, opacBL ),
              aBlend );

          aPtr += count;
        } break;

        /* Draw an image */
        case 'I' :
        {
          XBitmap* bmp     = EwGetAttrBitmap( aAttrSet, aPtr[ 3 ]);
          int      frameNo = aPtr[ 4 ];

          /* If the desired bitmap is not available in the AttrSet -> Use the
             default bitmap #0 */
          if ( !bmp )
            bmp = EwGetAttrBitmap( aAttrSet, 0 );

          if ( bmp )
          {
            int x1       = pos.X - aDstPos.X;
            int y1       = pos.Y - aDstPos.Y;
            int x2       = x1 + bmp->FrameSize.X;
            int y2       = y1 + bmp->FrameSize.Y;
            int opacTL   = EwGetOpacityFromGradient( aOpacity, x1, y1 ) >> 24;
            int opacTR   = EwGetOpacityFromGradient( aOpacity, x2, y1 ) >> 24;
            int opacBR   = EwGetOpacityFromGradient( aOpacity, x2, y2 ) >> 24;
            int opacBL   = EwGetOpacityFromGradient( aOpacity, x1, y2 ) >> 24;

            /* Image should use the current text color for image opacity and alpha8
               bitmaps used as image source */
            XColor color  = EwGetAttrColor( aAttrSet, aPtr[ 5 ]);
            XRect  dstRect;
            XPoint srcPos = { 0, 0 };

            dstRect.Point1   = pos;
            dstRect.Point2.X = pos.X + bmp->FrameSize.X;
            dstRect.Point2.Y = pos.Y + bmp->FrameSize.Y;

            EwCopyBitmap( aDst, bmp, aDstFrameNo, frameNo, aClipRect, dstRect,
              srcPos,
              BlendColor( color, opacTL ), BlendColor( color, opacTR ),
              BlendColor( color, opacBR ), BlendColor( color, opacBL ),
              aBlend );
          }

          aPtr += 6;
        } break;
      }
    }

    /* or a link span - the links are not drawn - skip over! */
    else if ( *aPtr == 'A' )
      aPtr += 7;
  }
}


/* This function counts the line feed signs in the given string. */
static int CountLineFeeds( XChar* aString )
{
  int count = 0;

  for ( ; aString && *aString; aString++ )
    if ( *aString == '\n' )
      count++;
  
  return count;
}


/* The following function parses the string aString for {lnk:... attributes
   and stores for every found valid attribute an entry in aLinks array and the
   link name in aNames array. */
static void CollectLinks( XChar* aString, XAttrLink* aLinks, XChar* aNames )
{
  XChar ch;

  /* Repeat until the entire content of the string has been processed */
  while (( ch = *aString ) != 0 )
  {
    /* Is this an attribute? */
    if (( ch == '{' ) && IsKeyword( aString, "{lnk:" ))
    {
      XChar* start = aString += 5;

      /* Skip over the link name */
      while ( *aString && ( *aString != '}' ) && ( *aString != '{' ))
        aString++;

      /* Copy the link name and terminate it with a zero terminator 
         sign */
      EwCopy( aNames, start, (int)(( aString - start ) * sizeof( XChar )));
      aNames[ aString - start ] = 0;

      /* Prepare the link description. At the moment the link has no 
         content - just name */
      aLinks->EndPtr      = 0;
      aLinks->StartPtr    = (unsigned short*)0xFFFFFFFE;
      aLinks->NoOfRegions = 0;
      aLinks->X           = 0;
      aLinks->Y           = 0;
      aLinks->Name        = aNames;

      /* Skip to the next link to process */
      aLinks++;
      aNames += aString - start + 1;

      continue;
    }

    /* Starting an escape sequence? */
    if (( ch == '%' ) && aString[1])
      aString++;

    aString++;
  }
}


/* This function searches recursively the memory area aPtr for link region
   statements and if found updates the corresponding link entries in aLinks. */
static void CollectLinkRegions( XAttrLink* aLinks, int aOfsX, int aOfsY,
  unsigned short* aPtr, unsigned short* aEnd )
{
  /* Repeat until all drawing statements are evaluated */
  while ( aPtr < aEnd )
  {
    /* Search for links region only - skip over all other statements */
    switch ( *aPtr )
    {
      /* The content of a group is processed recursively */
      case 'G' :
      {
        int count = aPtr[ 5 ];
        int x     = WORDNUM( aPtr[1]) + WORDNUM( aPtr[ 6 ]) + aOfsX;
        int y     = WORDNUM( aPtr[2]) + aOfsY;

        /* This recursion is important for the X,Y offset calculation */
        CollectLinkRegions( aLinks, x, y, aPtr + 7, aPtr + count );
        aPtr += count;
      } break;

      /* Skip over text/image/rectangle ... */
      case 'T' :
      case 'J' : aPtr += aPtr[ 3 ]; break;
      case 'I' : aPtr += 6;         break;
      case 'R' : aPtr += 6;         break;

      /* Link region found - update the corresponding link entry */
      case 'A' :
      {
        XAttrLink* link = aLinks + aPtr[ 1 ] - 1;

        link->StartPtr = MIN( link->StartPtr, aPtr );
        link->EndPtr   = MAX( link->EndPtr,   aPtr + 7 );
        link->X        = aOfsX;
        link->Y        = aOfsY;
        link->NoOfRegions++;

        /* Skip over the region */
        aPtr += 7;
      } break;
    }
  }
}


/* This function searches in the given area of drawing statements for the link
   region statement with the number aRegionNo and aLinkNo */
static unsigned short* SkipToRegion( int aLinkNo, int aRegionNo, 
  unsigned short* aStartPtr, unsigned short* aEndPtr )
{
  unsigned short* ptr = aStartPtr;

  /* Repeat until the desired region is found or the end of attributed string
     is reached */
  while ( ptr < aEndPtr )
  {
    /* Search for regions only - skip over all other statements */
    switch ( *ptr )
    {
      case 'T' :
      case 'J' : ptr += ptr[ 3 ]; break;
      case 'I' : ptr += 6;        break;
      case 'R' : ptr += 6;        break;

      /* Region statament found - is this the desired region? */
      case 'A' :
        if (( ptr[1] == aLinkNo ) && !aRegionNo )
          return ptr;

        if ( ptr[1] == aLinkNo )
          aRegionNo--;

        /* No - then search for the next region */
        ptr += 7;
      break;

      /* End of paragraph reached - a link may not cross over the paragraph */
      default :
        ptr = aEndPtr;
    }
  }

  /* Not found */
  return 0;
}


/* pba */

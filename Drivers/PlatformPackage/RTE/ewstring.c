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
*   The module 'ewstring.c' implements a set of string manipulation routines. 
*   These routines will be used in the automatic generated 'C' code to copy, 
*   compare, concat, etc. strings. Generally every string operation available in
*   the Chora language is realized as one or more 'C' functions implemented in 
*   this module.
*
*   Because the instant datatype 'string' implemented in Chora will work with
*   wide-characters only, all ewstring.c routines are designed to support them. 
*   A single wide-character occupies 16 bit in the memory, so it is capable to 
*   encode all known characters. The usage of the worldwide character-encoding 
*   standard UNICODE is possible.
*
*   During the runtime all strings will be stored on the heap. Each time a new
*   string should be created, ewstring.c module reserves memory for the string
*   and initializes it with the content of this string properly.
*
*   Strings which are not used any more will be detected and disposed by the
*   Garbage Collector. The Garbage Collector knows about all existing objects 
*   and strings so it can decide which strings are not used anymore and can 
*   dispose them withou any risk.
*
* SUBROUTINES:
*   EwFormatUIntToAnsiString
*   EwFormatIntToAnsiString
*   EwFormatUInt64ToAnsiString
*   EwFormatInt64ToAnsiString
*   EwFormatFloatToAnsiString
*   EwNewString
*   EwNewStringAnsi
*   EwNewStringUtf8
*   EwNewStringUInt
*   EwNewStringInt
*   EwNewStringUInt64
*   EwNewStringInt64
*   EwNewStringFloat
*   EwNewStringChar
*   EwCompString
*   EwLoadString
*   EwShareString
*   EwConcatString
*   EwConcatStringChar
*   EwConcatCharString
*   EwGetStringChar
*   EwSetStringChar
*   EwGetStringLength
*   EwIsStringEmpty
*   EwGetStringUpper
*   EwGetStringLower
*   EwStringLeft
*   EwStringRight
*   EwStringMiddle
*   EwStringInsert
*   EwStringRemove
*   EwStringFindChar
*   EwStringFind
*   EwStringParseInt32
*   EwStringParseUInt32
*   EwStringParseInt64
*   EwStringParseUInt64
*   EwStringParseFloat
*   EwStringToAnsi
*   EwGetUtf8StringLength
*   EwStringToUtf8
*   EwGetVariantOfString
*   EwGetVariantOfChar
*   EwGetCharUpper
*   EwGetCharLower
*
*******************************************************************************/

#include "ewrte.h"


/* Undo the macro overloading of following functions */
#undef EwReleaseStrings


int EwMaxStringCacheSize = EW_MAX_STRING_CACHE_SIZE;


/* Empty string */
static const XChar NullString[] = { 0 };


/* EwLangId contains the ID of the currently selected language. LangId will be
   used to find out the correct variant of a string constant in the function 
   EwGetVariantOfXXX() */
extern int EwLangId;


/* XStringStruct stores the control data for dynamic created strings. Such
   strings are created durring the runtime by one of the EwNewStringXXX()
   functions. Dynamic created strings are managed in a global, single chained
   list 'Strings'. The Garbage Collector traverses this list in order to find
   and free unused strings automatically. As long as the string's usage counter
   is greater than zero, the Garbage Collector does not dispose it. */
struct XStringStruct
{
  struct XStringStruct* Next;
  int                   Size;
};


/* Strings points to the chain of dynamic created strings. All strings will be
   stored in this chain. This is necessary to find unused strings by the 
   Garbage Collector. Note: string constants are stored separately in the 
   cache. */
static struct XStringStruct* Strings = 0;


/* XStringBlock describes a block, which is filled with string constants. All 
   blocks are stored in a double chained list and they are managed by the
   priority sorted cache. In this manner it is possible to find frequently
   used blocks (and strings) much more quickly. The structure is followed by
   the content of the block. */
struct XStringBlock
{
  struct XStringBlock* Next;
  struct XStringBlock* Prev;
  int                  Size;
  const void*          Block;  /* Pointer to the compressed data. */
};


/* Pointer to the first and last entry of the global cache of string constants
   and the size of the memory currently occupied by all cached blocks. */
static struct XStringBlock* CacheFirst = 0;
static struct XStringBlock* CacheLast  = 0;
static int                  CacheSize  = 0;


/* Prefix identifying Embedded Wizarda managed strings. If during runtime
   Embedded Wizard detects a strings not having this ID, a runtime error is
   generated. The second number identifies additionally a string created
   privately as result of a string modification with the index operator [].
   As long as such string is not used elsewhere the private string can be
   modified by the [] operator without having do clone it. */
#define EW_STRING_MAGIC_NO       0x4557
#define EW_PRIV_STRING_MAGIC_NO  0x4677


/* Flag added to string magic numbers to sign the string as marked by garbage
   collection. Such strings are not disposed. */
#define EW_STRING_MARK_FLAG      0x8000


#ifndef EW_DONT_CHECK_STRINGS
  /* The following function verifies whether the given string is a valid
     string created by the Runtime Envorinment. If the string is created
     by the user it may cause fatal problems with memory management. */
  static XString VerifyString( XString aString )
  {
    if ( aString && (( aString[-1] & ~EW_STRING_MARK_FLAG ) != EW_STRING_MAGIC_NO ) && 
       (( aString[-1] & ~EW_STRING_MARK_FLAG ) != EW_PRIV_STRING_MAGIC_NO ))
    {
      EwPrint( "[FATAL ERROR] Unmanaged string found '" );

      /* Print the content of the string to the user */
      for ( ; *aString; aString++ )
      {
        /* A printable character? */
        if (( *aString >= 0x20 ) && ( *aString <= 0x7F ))
          EwPrint( "%c", *aString );

        /* If not, print the character in the \x#### hex. notation */
        else
          EwPrint( "\\x%04X", *aString );
      }

      EwPrint( "'" );
      EwPanic();
      return 0;
    }

    return aString;
  }
#else
  #define VerifyString( aString ) (aString)
#endif


/* Following variables describe Flash memory which is not directly accessible
   by the CPU. In order to read the Flash memory, a user own reader procedure
   is used. Please see the function EwRegisterFlashAreaReader(). */
extern XFlashAreaReaderProc EwFlashAreaReaderProc;
extern void*                EwFlashAreaStartAddress;
extern void*                EwFlashAreaEndAddress;


/* These global variables stores the number of bytes occupied by strings. */
extern int EwStringsMemory;
extern int EwStringsMemoryPeak;
extern int EwNoOfStrings;
extern int EwObjectsMemory;
extern int EwResourcesMemory;
extern int EwMemoryPeak;


/* This helper function counts the strings stored within the block aBlock.
   The blocks are usually stored compressed within the code area and they
   are decompressed on request only. A single block may contain several
   strings as single chained list. */
static int CountStringsOfBlock( struct XStringBlock* aBlock )
{
  unsigned short* ptr   = (unsigned short*)( aBlock + 1 );
  int             count = 0;

  /* Traverse the chain of enclosed strings */
  while ( *ptr )
  {
    ptr += *ptr;
    count++;
  }

  return count;
}


/* This helper function implements the lower to upper case conversion of the given
   UNICODE character aCode. Please note, the algorithm is limited to code points
   lying below 0x052F (UNICODE blocks: Basic Latin, Latin-1 Supplement,
   Latin-Extended-A, Latin-Extended-B, IPA Extensions, Greek and Coptic, Cyrillic,
   Cyrillic Supplement. Other code points are not converted. */
static int ToUpper( XChar aCode )
{
  static const XChar table[] = {
    0x00B5, 0x039C, 0x00FF, 0x0178, 0x0131, 0x0049, 0x017F, 0x0053, 0x0180, 0x0243,
    0x0183, 0x0182, 0x0185, 0x0184, 0x0188, 0x0187, 0x018C, 0x018B, 0x0192, 0x0191,
    0x0195, 0x01F6, 0x0199, 0x0198, 0x019A, 0x023D, 0x019E, 0x0220, 0x01A8, 0x01A7,
    0x01AD, 0x01AC, 0x01B0, 0x01AF, 0x01B4, 0x01B3, 0x01B6, 0x01B5, 0x01B9, 0x01B8,
    0x01BD, 0x01BC, 0x01BF, 0x01F7, 0x01C5, 0x01C4, 0x01C6, 0x01C4, 0x01C8, 0x01C7,
    0x01C9, 0x01C7, 0x01CB, 0x01CA, 0x01CC, 0x01CA, 0x01DD, 0x018E, 0x01F2, 0x01F1,
    0x01F3, 0x01F1, 0x01F5, 0x01F4, 0x023C, 0x023B, 0x023F, 0x2C7E, 0x0240, 0x2C7F,
    0x0242, 0x0241, 0x0250, 0x2C6F, 0x0251, 0x2C6D, 0x0252, 0x2C70, 0x0253, 0x0181,
    0x0254, 0x0186, 0x0256, 0x0189, 0x0257, 0x018A, 0x0259, 0x018F, 0x025B, 0x0190,
    0x0260, 0x0193, 0x0263, 0x0194, 0x0268, 0x0197, 0x0269, 0x0196, 0x026B, 0x2C62,
    0x026F, 0x019C, 0x0271, 0x2C6E, 0x0272, 0x019D, 0x0275, 0x019F, 0x027D, 0x2C64,
    0x0280, 0x01A6, 0x0283, 0x01A9, 0x0288, 0x01AE, 0x0289, 0x0244, 0x028A, 0x01B1,
    0x028B, 0x01B2, 0x028C, 0x0245, 0x0292, 0x01B7, 0x0345, 0x0399, 0x0371, 0x0370,
    0x0373, 0x0372, 0x0377, 0x0376, 0x03AC, 0x0386, 0x03C2, 0x03A3, 0x03CC, 0x038C,
    0x03CD, 0x038E, 0x03CE, 0x038F, 0x03D0, 0x0392, 0x03D1, 0x0398, 0x03D5, 0x03A6,
    0x03D6, 0x03A0, 0x03D7, 0x03CF, 0x03F0, 0x039A, 0x03F1, 0x03A1, 0x03F2, 0x03F9,
    0x03F3, 0x037F, 0x03F5, 0x0395, 0x03F8, 0x03F7, 0x03FB, 0x03FA, 0x04CF, 0x04C0,
    0xFFFF };

  const XChar* p = table; 

  /* The code is out of the supported UNICODE range */
  if ( aCode > 0x0531 )
    return aCode;

  /* Continuous ranges */
  if (( aCode >= 0x0061 ) && ( aCode <= 0x007A )) return aCode - 32;
  if (( aCode >= 0x00E0 ) && ( aCode <= 0x00F6 )) return aCode - 32;
  if (( aCode >= 0x00F8 ) && ( aCode <= 0x00FE )) return aCode - 32;
  if (( aCode >= 0x037B ) && ( aCode <= 0x037D )) return aCode + 130;
  if (( aCode >= 0x03AD ) && ( aCode <= 0x03AF )) return aCode - 37;
  if (( aCode >= 0x03B1 ) && ( aCode <= 0x03C1 )) return aCode - 32;
  if (( aCode >= 0x03C3 ) && ( aCode <= 0x03CB )) return aCode - 32;
  if (( aCode >= 0x0430 ) && ( aCode <= 0x044F )) return aCode - 32;
  if (( aCode >= 0x0450 ) && ( aCode <= 0x045F )) return aCode - 80;

  /* Ranges with even code points */
  if (( aCode % 2 ) == 0 )
  {
    if (( aCode >= 0x013A ) && ( aCode <= 0x0148 )) return aCode -1;
    if (( aCode >= 0x017A ) && ( aCode <= 0x017E )) return aCode -1;
    if (( aCode >= 0x01CE ) && ( aCode <= 0x01DC )) return aCode -1;
    if (( aCode >= 0x04C2 ) && ( aCode <= 0x04CE )) return aCode -1;
  }

  /* Ranges with odd code points */
  else
  {
    if (( aCode >= 0x0101 ) && ( aCode <= 0x012F )) return aCode -1;
    if (( aCode >= 0x0133 ) && ( aCode <= 0x0137 )) return aCode -1;
    if (( aCode >= 0x014B ) && ( aCode <= 0x0177 )) return aCode -1;
    if (( aCode >= 0x01A1 ) && ( aCode <= 0x01A5 )) return aCode -1;
    if (( aCode >= 0x01DF ) && ( aCode <= 0x01EF )) return aCode -1;
    if (( aCode >= 0x01F9 ) && ( aCode <= 0x021F )) return aCode -1;
    if (( aCode >= 0x0223 ) && ( aCode <= 0x0233 )) return aCode -1;
    if (( aCode >= 0x0247 ) && ( aCode <= 0x024F )) return aCode -1;
    if (( aCode >= 0x03D9 ) && ( aCode <= 0x03EF )) return aCode -1;
    if (( aCode >= 0x0461 ) && ( aCode <= 0x0481 )) return aCode -1;
    if (( aCode >= 0x048B ) && ( aCode <= 0x04BF )) return aCode -1;
    if (( aCode >= 0x04D1 ) && ( aCode <= 0x052F )) return aCode -1;
  }

  /* Search the code-2-code mapping table */
  for ( ; aCode > *p; p += 2 )
    ;

  /* Found? */
  if ( aCode == *p )
    return p[1];

  /* No corresponding code point */
  return aCode;
}


/* This helper function implements the upper to lower case conversion of the given
   UNICODE character aCode. Please note, the algorithm is limited to code points
   lying below 0x052F (UNICODE blocks: Basic Latin, Latin-1 Supplement,
   Latin-Extended-A, Latin-Extended-B, IPA Extensions, Greek and Coptic, Cyrillic,
   Cyrillic Supplement. Other code points are not converted. */
int ToLower( XChar aCode )
{
  static const XChar table[] = {
    0x0130, 0x0069, 0x0178, 0x00FF, 0x0181, 0x0253, 0x0182, 0x0183, 0x0184, 0x0185,
    0x0186, 0x0254, 0x0187, 0x0188, 0x0189, 0x0256, 0x018A, 0x0257, 0x018B, 0x018C,
    0x018E, 0x01DD, 0x018F, 0x0259, 0x0190, 0x025B, 0x0191, 0x0192, 0x0193, 0x0260,
    0x0194, 0x0263, 0x0196, 0x0269, 0x0197, 0x0268, 0x0198, 0x0199, 0x019C, 0x026F,
    0x019D, 0x0272, 0x019F, 0x0275, 0x01A6, 0x0280, 0x01A7, 0x01A8, 0x01A9, 0x0283,
    0x01AC, 0x01AD, 0x01AE, 0x0288, 0x01AF, 0x01B0, 0x01B1, 0x028A, 0x01B2, 0x028B,
    0x01B3, 0x01B4, 0x01B5, 0x01B6, 0x01B7, 0x0292, 0x01B8, 0x01B9, 0x01BC, 0x01BD,
    0x01C4, 0x01C6, 0x01C5, 0x01C6, 0x01C7, 0x01C9, 0x01C8, 0x01C9, 0x01CA, 0x01CC,
    0x01F1, 0x01F3, 0x01F2, 0x01F3, 0x01F4, 0x01F5, 0x01F6, 0x0195, 0x01F7, 0x01BF,
    0x0220, 0x019E, 0x023A, 0x2C65, 0x023B, 0x023C, 0x023D, 0x019A, 0x023E, 0x2C66,
    0x0241, 0x0242, 0x0243, 0x0180, 0x0244, 0x0289, 0x0245, 0x028C, 0x0370, 0x0371,
    0x0372, 0x0373, 0x0376, 0x0377, 0x037F, 0x03F3, 0x0386, 0x03AC, 0x038C, 0x03CC,
    0x038E, 0x03CD, 0x038F, 0x03CE, 0x03CF, 0x03D7, 0x03F4, 0x03B8, 0x03F7, 0x03F8,
    0x03F9, 0x03F2, 0x03FA, 0x03FB, 0x04C0, 0x04CF, 0xFFFF };

  const XChar* p = table; 

  /* The code is out of the supported UNICODE range */
  if ( aCode > 0x0531 )
    return aCode;

  /* Continuous ranges */
  if (( aCode >= 0x0041 ) && ( aCode <= 0x005A )) return aCode + 32;
  if (( aCode >= 0x00C0 ) && ( aCode <= 0x00D6 )) return aCode + 32;
  if (( aCode >= 0x00D8 ) && ( aCode <= 0x00DE )) return aCode + 32;
  if (( aCode >= 0x0388 ) && ( aCode <= 0x038A )) return aCode + 37;
  if (( aCode >= 0x0391 ) && ( aCode <= 0x03A1 )) return aCode + 32;
  if (( aCode >= 0x03A3 ) && ( aCode <= 0x03AB )) return aCode + 32;
  if (( aCode >= 0x03FD ) && ( aCode <= 0x03FF )) return aCode - 130;
  if (( aCode >= 0x0400 ) && ( aCode <= 0x040F )) return aCode + 80;
  if (( aCode >= 0x0410 ) && ( aCode <= 0x042F )) return aCode + 32;


  /* Ranges with even code points */
  if (( aCode % 2 ) == 0 )
  {
    if (( aCode >= 0x0100 ) && ( aCode <= 0x012E )) return aCode +1;
    if (( aCode >= 0x0132 ) && ( aCode <= 0x0136 )) return aCode +1;
    if (( aCode >= 0x014A ) && ( aCode <= 0x0176 )) return aCode +1;
    if (( aCode >= 0x01A0 ) && ( aCode <= 0x01A4 )) return aCode +1;
    if (( aCode >= 0x01DE ) && ( aCode <= 0x01EE )) return aCode +1;
    if (( aCode >= 0x01F8 ) && ( aCode <= 0x021E )) return aCode +1;
    if (( aCode >= 0x0222 ) && ( aCode <= 0x0232 )) return aCode +1;
    if (( aCode >= 0x0246 ) && ( aCode <= 0x024E )) return aCode +1;
    if (( aCode >= 0x03D8 ) && ( aCode <= 0x03EE )) return aCode +1;
    if (( aCode >= 0x0460 ) && ( aCode <= 0x0480 )) return aCode +1;
    if (( aCode >= 0x048A ) && ( aCode <= 0x04BE )) return aCode +1;
    if (( aCode >= 0x04D0 ) && ( aCode <= 0x052E )) return aCode +1;
  }

  /* Ranges with odd code points */
  else
  {
    if (( aCode >= 0x0139 ) && ( aCode <= 0x0147 )) return aCode +1;
    if (( aCode >= 0x0179 ) && ( aCode <= 0x017D )) return aCode +1;
    if (( aCode >= 0x01CB ) && ( aCode <= 0x01DB )) return aCode +1;
    if (( aCode >= 0x04C1 ) && ( aCode <= 0x04CD )) return aCode +1;
  }

  /* Search the code-2-code mapping table */
  for ( ; aCode > *p; p += 2 )
    ;

  /* Found? */
  if ( aCode == *p )
    return p[1];

  /* No corresponding code point */
  return aCode;
}


/* WcsLen() returns the number of wide-characters in the string aString 
   without the zero-terminator sign */
static int WcsLen( const XChar* aString )
{
  const XChar* tmp = aString;

  /* Null string -> Nothing to do */
  if ( !tmp )
    return 0;

  /* Look for the zero-terminator sign */
  while ( *tmp )
    tmp++;

  return (int)( tmp - aString );
}


/* WcsCpy() copies aCount wide-characters from the string aSrc into aDest */
static void WcsCpy( XChar* aDest, const XChar* aSrc, int aCount )
{
  /* Nothing to do? */
  if ( !aSrc || !aDest || !aCount )
    return;

  /* repeat until all aCount characters has been copied from aSrc into aDest */
  while ( aCount-- )
    *aDest++ = *aSrc++;

  /* Don't forget the zero-terminator sign */
  *aDest = 0;
}


/* The WcsCmp() function compares aString1 and aString2 and returns a value 
   indicating the lexicographic relation between the strings:

     <  0 aString1 is less than aString2 
     == 0 aString1 is identical to aString2 
     >  0 aString1 is greater than aString2 
*/
static int WcsCmp( const XChar* aString1, const XChar* aString2 )
{
  int ret;

  /* Make sure the both pointers are valid. A NULL string should point to a 
     zero-terminator sign */
  if ( !aString1 ) aString1 = NullString;
  if ( !aString2 ) aString2 = NullString;

  /* Compare the both strings, character by character */
  while (( *aString1 == *aString2 ) && *aString2 )
  {
    aString1++;
    aString2++;
  }

  ret = *aString1 - *aString2;

  if      ( ret < 0 ) ret = -1;
  else if ( ret > 0 ) ret = +1;

  return ret;
}


/* The WcsChr() function finds the first occurrence of the character aChar 
   in the string aString. The function returns 0 if aChar is not found. */
static XChar* WcsChr( XChar* aString, XChar aChar )
{
  /* Nothing to do? */
  if ( !aString )
    return 0;

  /* Repeat until the character aChar has been found or the zero-terminator
     has been reached */
  while ( *aString && ( *aString != aChar ))
    aString++;

  /* The character has not been found ... */
  if ( !*aString )
    aString = 0;

  return aString;
}


/* The WcsStr() function returns a pointer to the first occurrence of aString2
   in the string aString1. The function returns 0 if aString2 is not found. */
static XChar* WcsStr( XChar* aString1, const XChar* aString2 )
{
  /* Nothing to do? */
  if ( !aString1 || !aString2 || !*aString2 )
    return aString1;

  /* Look for aString2 in aString1 */
  while ( *aString1 )
  {
    const XChar* s1 = aString1;
    const XChar* s2 = aString2;

    while (( *s1 == *s2 ) && *s1 )
    {
      s1++;
      s2++;
    }

    if ( !*s2 )
      return aString1;

    aString1++;
  }

  /* Not found */
  return 0;
}


/* WcsCanAccess() verifies whether the given character index aIndex is valid
   within the string - means, it is before the zero-terminator sign */
static int WcsCanAccess( const XChar* aString, int aIndex )
{
  const XChar* tmp = aString;
  const XChar* end = tmp + aIndex;

  /* Null string -> Nothing to do */
  if ( !tmp || ( aIndex < 0 ))
    return 0;

  /* Look for the zero-terminator sign */
  while (( tmp < end ) && *tmp )
    tmp++;

  return *tmp;
}


/* Utf8Len() returns the number of wide-characters in the UTF-8 string aString 
   without the zero-terminator sign. Note the aCount parameter, which determines
   the number of characters within the utf-8 input string excluding the zero
   terminator. */
static int Utf8Len( const unsigned char* aString, int aCount )
{
  int count = 0;

  /* Repeat until all utf-8 signs have been evaluated */
  while ( aCount > 0 )
  {
    /* Get the next following character from the utf-8 input string */
    unsigned char c = *aString;

    /* 1 byte sequence: 0aaa aaaa -> 0aaa aaaa */
    if ( c < 0x80 )
    {
      aCount--;
      aString++;
    }

    /* 2 byte sequence: 110a aaaa  10bb bbbb -> 0000 0aaa  aabb bbbb */
    else if ( c < 0xE0 )
    {
      aCount  -= 2;
      aString += 2;
    }

    /* 3 byte sequence: 1110aaaa 10bbbbbb 10cccccc -> aaaabbbb bbcccccc */
    else if ( c < 0xF0 )
    {
      aCount  -= 3;
      aString += 3;
    }

    /* 4 byte sequence: 11110aaa 10bbbbbb 10cccccc 10dddddd -> 
                        000aaabb bbbbcccc ccdddddd */
    else
    {
      aCount  -= 4;
      aString += 4;
    }

    /*  Count the unicode sign - note the posibility of an error in the utf-8
        string --> the error will cause an overflow in the aCount counter. */
    if ( aCount >= 0 )
      count++;
  }

  return count;
}


/* DecodeUtf8() function decods the given utf-8 input string aString into a
   unicode 16 string and stores it within the buffer aDest. Note the aCount
   parameter, which determines the number of characters within the utf-8 input
   string excluding the zero terminator. The parameter sDestSize determines
   the max. capacity of the aDest destination buffer. */
static void DecodeUtf8( XChar* aDest, int aDestSize, 
  const unsigned char* aString, int aCount )
{
  /* Repeat until all utf-8 sign have been evaluated */
  while (( aDestSize > 0 ) && ( aCount > 0 ))
  {
    unsigned int c = *aString++;

    /* 1 byte sequence: 0aaa aaaa -> 0aaa aaaa */
    if ( c < 0x80 )
      aCount--;

    /* 2 byte sequence: 110a aaaa  10bb bbbb -> 0000 0aaa  aabb bbbb */
    else if (( c < 0xE0 ) && ( aCount >= 2 ))
    {
      c  = (( c & 0x1F ) << 6 );
      c += ( *aString++ & 0x3F );
      aCount -= 2;
    }

    /* 3 byte sequence: 1110aaaa 10bbbbbb 10cccccc -> aaaabbbb bbcccccc */
    else if (( c < 0xF0 ) && ( aCount >= 3 ))
    {
      c  = ( c & 0x0F ) << 12;
      c += ( *aString++ & 0x3F ) << 6;
      c += ( *aString++ & 0x3F );
      aCount -= 3;
    }

    /**** 4 byte sequence: 11110aaa 10bbbbbb 10cccccc 10dddddd -> 
                           000aaabb bbbbcccc ccdddddd */
    else if ( aCount >= 4 )
    {
      c  = ( c & 0x07 ) << 18;
      c += ( *aString++ & 0x3F ) << 12;
      c += ( *aString++ & 0x3F ) << 6;
      c += ( *aString++ & 0x3F );
      aCount -= 4;

      /* Avoid illegal 24 bit characters */
      if ( c > 0xFFFF )
        c = 0xFFFF;
    }

    /* Unexpected end of utf-8 sequence */
    else
      break;

    /* Take over the decoded character */
    *aDest++ = (XChar)c;
    aDestSize--;
  }
}


/* SkipSpace() returns the pointer to the next non-whitespace sign 
   starting with the sign aChar */
static const XChar* SkipSpace( const XChar* aChar )
{
  while ( aChar && (( *aChar == ' '  ) || ( *aChar == '\t' ) ||
                    ( *aChar == '\n' ) || ( *aChar == '\v' ) ||
                    ( *aChar == '\f' ) || ( *aChar == '\r' )))
    aChar++;

  return aChar;
}


/* The function CacheBlock() stores the block aBlock at the begin of the
   global cache. */
static void CacheBlock( struct XStringBlock* aBlock )
{
  /* insert the block into the cache */
  aBlock->Next = CacheFirst;
  aBlock->Prev = 0;

  if ( CacheFirst )
    CacheFirst->Prev = aBlock;
  else
    CacheLast = aBlock;

  CacheFirst = aBlock;

  /* new block is added to the cache - the cache is bigger now */
  CacheSize += aBlock->Size;
}


/* The function UncacheBlock() removes the block aBlock from the global 
   cache. */
static void UncacheBlock( struct XStringBlock* aBlock )
{
  /* remove the block from the cache */
  if ( aBlock->Next )
    aBlock->Next->Prev = aBlock->Prev;
  else
    CacheLast = aBlock->Prev;

  if ( aBlock->Prev )
    aBlock->Prev->Next = aBlock->Next;
  else
    CacheFirst = aBlock->Next;

  aBlock->Next = 0;
  aBlock->Prev = 0;

  /* the block is removed - the cache is smaller now */
  CacheSize -= aBlock->Size;
}


/* The function FindBlock() searches the cache for the block loaded from
   the compressed string block aBlock. */
static struct XStringBlock* FindBlock( const void* aBlock )
{
  struct XStringBlock* block = CacheFirst;

  /* repeat until the desired block has been found or all blocks are
     evaluated */
  while ( block && ( block->Block != aBlock ))
    block = block->Next;

  return block;
}


#ifndef EW_DONT_USE_COMPRESSION

/* The function LoadBlock() is called in order to decompress all string 
   constants stored within the compressed block aBlock. The function reserves
   memory for the block and starts the decompression. */
static struct XStringBlock* LoadBlock( const unsigned int* aBlock, 
  int aUseReader )
{
  /* Get the size of the block after it is decompressed and reserve
     the memory for it. */
  int                  bs    = !aUseReader? *(const int*)aBlock :
                               *(const int*)EwFlashAreaReaderProc( aBlock );
  int                  size  = bs + sizeof( struct XStringBlock );
  struct XStringBlock* block = EwAlloc( size );

  /* Failed? */
  if ( !block )
    return 0;

  /* Initialize the new block */
  block->Block = aBlock;
  block->Size  = size;
  block->Next  = 0;
  block->Prev  = 0;

  /* ... and decompress all the strings stored within the block. */
  EwDecompress( aBlock + 1, (unsigned char*)( block + 1 ), 0, 0 );

  /* Depending on the target CPU architecture, adapt the order of the bytes */
  EwAdaptByteOrder2((unsigned short*)( block + 1 ), bs / 2 );

  /* Track the RAM usage */
  EwStringsMemory += size;
  EwNoOfStrings   += CountStringsOfBlock( block );

  /* Also track the max. memory pressure */
  if ( EwStringsMemory > EwStringsMemoryPeak )
    EwStringsMemoryPeak = EwStringsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return block;
}

#endif /* EW_DONT_USE_COMPRESSION */


/* WcsLen2() returns the number of wide-characters in the string aString 
   without the zero-terminator sign. This function works similarly to WcsLen()
   with the unique difference to access the string via flash reader. */
static int WcsLen2( const XChar* aString )
{
  const XChar* tmp = aString;

  /* Null string -> Nothing to do */
  if ( !tmp )
    return 0;

  /* Look for the zero-terminator sign */
  while ( *(const XChar*)EwFlashAreaReaderProc( tmp ))
    tmp++;

  return (int)( tmp - aString );
}


/* The function LoadString() is called in order to copy the string stored within
   a not directly accessigble memory aString. The function reserves memory for
   a new block and copies the string there. */
static struct XStringBlock* LoadString( const XChar* aString )
{
  /* Get the size of the block after it is decompressed and reserve
     the memory for it. */
  int                  count = WcsLen2( aString );
  int                  size  = ( count + 4 ) * sizeof( XChar ) + 
                                 sizeof( struct XStringBlock );
  struct XStringBlock* block = EwAlloc( size );
  int                  i;

  /* Failed? */
  if ( !block )
    return 0;

  /* Initialize the new block */
  block->Block = aString;
  block->Size  = size;
  block->Next  = 0;
  block->Prev  = 0;

  /* Prepare management data for the new block */
  ((XChar*)( block + 1 ))[0]       = (XChar)( count + 3 );
  ((XChar*)( block + 1 ))[1]       = 0x4557;
  ((XChar*)( block + 1 ))[count+2] = 0;
  ((XChar*)( block + 1 ))[count+3] = 0;

  /* This version of the LoadBlock() function is used only, if the original
     uncompressed string is not directly accessible by the CPU. To copy the
     string, the FlashAreaReaderProc() has to be used. Do it sign by sign. */
  for ( i = 0; i < count; i++ )
    ((XChar*)( block + 1 ))[i+2] = *(XChar*)EwFlashAreaReaderProc( aString + i );

  /* Track the RAM usage */
  EwStringsMemory += size;
  EwNoOfStrings   += CountStringsOfBlock( block );

  /* Also track the max. memory pressure */
  if ( EwStringsMemory > EwStringsMemoryPeak )
    EwStringsMemoryPeak = EwStringsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return block;
}


/* The private function AllocString() tries to reserve memory for a new
   string with the given number of characters aSize. AllocString() stores
   all strings in the global list 'Strings', so the Garbage Collector can
   traverse this list in order to detect strings not in use anymore (garbage)
   and to free them. */
static XChar* AllocString( int aSize )
{
  struct XStringStruct* str;

  /* Valid size? -> Return Null string */
  if ( aSize <= 0 )
    return 0;

  /* Don't forget the zero-terminator sign and the magic number */
  aSize += 2;

  /* Get the required size in bytes. We need additional space to store a
     XStringStruct object, which is needed by the Garbage Collector */
  aSize = ( aSize * 2 ) + sizeof( struct XStringStruct );

  /* Try to allocate the memory for the new string */
  do
    str = EwAlloc( aSize );
  while ( !str && EwImmediateReclaimMemory( 3 ));

  /* If the reservation has failed ... */
  if ( !str )
  {
    EwError( 3 );
    return 0;
  }

  /* Initialize the XStringStruct object and insert the string into the global
     chain of existing strings */
  str->Next = Strings;
  Strings   = str;

  /* Track the RAM usage */
  str->Size        = aSize;
  EwStringsMemory += aSize;
  EwNoOfStrings++;

  /* Also track the max. memory pressure */
  if ( EwStringsMemory > EwStringsMemoryPeak )
    EwStringsMemoryPeak = EwStringsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* Skip over the XStringStruct object and init the immediately following
     magic number */
  str++;
  ((XChar*)str)[0] = EW_PRIV_STRING_MAGIC_NO;

  /* Return the addresse of the first character in the string - NOT the address
     of the magic number! */
  return (XChar*)str + 1;
}


/* This function will be used internally when the Garbage Collector sweeps all
   memory blocks which are not in use anymore. The function returns != 0 if 
   there was at least one string released. */
int EwDisposeStrings( int aClearCache )
{
  struct XStringStruct** strings = &Strings;
  struct XStringBlock*   blocks  = CacheLast;
  int                    count   = 0;

  /* Find out which strings are not in use anymore and dispose them */
  while ( *strings )
  {
    /* If the string is not marked -> the string is not in use ... */
    if ( !( *((XChar*)( *strings + 1 )) & EW_STRING_MARK_FLAG ))
    {
      /* ... take it out from the global chain ... */
      struct XStringStruct* tmp = *strings;
      *strings = tmp->Next;

      /* For debugging only */
      EwStringsMemory -= tmp->Size;
      EwNoOfStrings--;

      /* and free the memory occuped by the string */
      EwFree( tmp );
      count++;
    }

    /* The string is still in use -> let it alive and continue with the next 
       string */
    else
    {
      *((XChar*)( *strings + 1 )) &= ~EW_STRING_MARK_FLAG;
      strings = &(*strings)->Next;
    }
  }

  /* The second step is to free blocks with string constants, which are no
     more in use. Only entire blocks may be freed. */
  while ( blocks && ( aClearCache || ( CacheSize > EwMaxStringCacheSize )))
  {
    /* tmpS should refer to the control data of the affected string */
    XChar*               tmpS = (XChar*)( blocks + 1 );
    struct XStringBlock* tmp  = blocks;

    /* Prepare the pointer to the next block in the cache */
    blocks = blocks->Prev;

    /* Search in the block for at least one string, which is signed as used
       (its usage counter is > 0) */
    while ( *tmpS && !( tmpS[1] & EW_STRING_MARK_FLAG ))
      tmpS += *tmpS;

    /* Any string in use? If not -> discard the whole block. */
    if ( !*tmpS )
    {
      /* Track the RAM usage */
      EwStringsMemory -= tmp->Size;
      EwNoOfStrings   -= CountStringsOfBlock( tmp );

      UncacheBlock( tmp );
      EwFree( tmp );
      count++;
    }
  }

  blocks = CacheLast;

  /* Third step: Reset the mark flags set during the garbage collection mark
     phase. */
  while ( blocks )
  {
    /* tmpS should refer to the control data of the affected string */
    XChar* tmpS = (XChar*)( blocks + 1 );

    /* Prepare the pointer to the next block in the cache */
    blocks = blocks->Prev;

    /* Clear the mark flags for every string found in the block */
    while ( *tmpS )
    {
      tmpS[1] &= ~EW_STRING_MARK_FLAG;
      tmpS += *tmpS;
    }
  }

  return count;
}


/* Verifies whether aString points to an existing string which can be marked
   during the garbage collection. If successfull returns the correct string 
   to mark - this can be a first string within the same block of string
   constants. Otherwise 0 ist returned. */
XString EwCanMarkString( XString aString )
{
  struct XStringBlock*  block   = CacheLast;
  struct XStringStruct* strings = Strings;

  /* Search for the string within the list of dynamically created strings. */
  while (
          strings && 
          (
            ( aString <  (XString)( strings + 1 )) ||
            ( aString >= (XString)((char*)strings + strings->Size ))
          )
        )
      strings = strings->Next;

  /* Found? */
  if ( strings )
    return (XString)( strings + 1 ) + 1;

  /* Search for the string within the list of loaded string constants. */
  while ( 
          block && 
          (
            ( aString <  (XString)( block + 1 )) ||
            ( aString >= (XString)((char*)block + block->Size ))
          )
        )
    block = block->Prev;
  
  /* If could find a block containing the string, return the first string of
     the block to mark it */
  if ( block )
    return (XString)( block + 1 ) + 2;
  
  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwFormatUIntToAnsiString
*
* DESCRIPTION:
*   The function EwFormatUIntToAnsiString() converts the given unsigned number
*   aValue in an Ansi string and stores the results in the buffer aBuf. The
*   function adds leading zeros '0' until the resulted string has reached the
*   length given in the aCount argument. If the paramneter aSign is != 0, an
*   additional '+' sign will prefix the formatted number.
*
* ARGUMENTS:
*   aBuf   - Buffer to store the resulting string.
*   aValue - Unsigned value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*   aUpper - If != 0, use upper case signs for hex digits.
*   aSign  - If != 0, instructs the function to prefix the number with the 
*     '+' sign.
*
* RETURN VALUE:
*   Returns the length of the resulting string. The string within aBuf is not
*   zero terminated.
*
*******************************************************************************/
int EwFormatUIntToAnsiString( char* aBuf, unsigned long aValue, int aCount,
  int aRadix, int aUpper, int aSign )
{
  char  hex  = aUpper? 'A' : 'a';
  char* ptr  = aBuf;
  char* ptr1 = aBuf;
  char* ptr2;

  /* Is aCount valid? */
  if ( aCount > 128 )
    aCount  = 128;

  /* Convert the number in binary notation */
  if ( aRadix == 2 )
    do
    {
      *ptr++ = (char)( '0' + ( aValue & 1 ));
      aValue >>= 1;
    }
    while ( aValue > 0 );

  /* Convert the number in octal notation */
  else if ( aRadix == 8 )
    do
    {
      *ptr++ = (char)( '0' + ( aValue & 7 ));
      aValue >>= 3;
    }
    while ( aValue > 0 );

  /* Convert the number in hexadecimal notation */
  else if ( aRadix == 16 )
    do
    {
      if (( aValue & 15 ) > 9 )
        *ptr++ = (char)( hex + ( aValue & 15 ) - 10 );
      else
        *ptr++ = (char)( '0' + ( aValue & 15 ));
      aValue >>= 4;
    }
    while ( aValue > 0 );

  /* Convert the number in decimal notation */
  else
    do
    {
      *ptr++ = (char)( '0' + ( aValue % 10 ));
      aValue /= 10;
    }
    while ( aValue > 0 );

  /* Calculate the number of additional '0' signs to add to the string. */
  aCount -= ( ptr - aBuf ) + ( aSign? 1 : 0 );

  /* Fill with zero signs */
  while ( aCount-- > 0 )
    *ptr++ = '0';

  if ( aSign )
    *ptr++ = '+';

  /* Finally revert the order of the characters in the formatted string */
  for ( ptr2 = ptr - 1; ptr2 > ptr1; ptr2--, ptr1++ )
  {
    char tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
  }

  return (int)( ptr - aBuf );
}


/*******************************************************************************
* FUNCTION:
*   EwFormatIntToAnsiString
*
* DESCRIPTION:
*   The function EwFormatIntToAnsiString() converts the given signed number 
*   aValue in an Ansi string and stores the results in the buffer aBuf. The
*   function adds leading zeros '0' until the resulted string has reached the
*   length given in the aCount argument. If the number is negative or aSign is
*   != 0, the function prefix the formatted number with the '+' or '-' minus
*   sign.
*
* ARGUMENTS:
*   aValue - Signed 32 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*   aUpper - If != 0, use upper case signs for hex digits.
*   aSign  - If != 0, instructs the function to prefix the number with the 
*     '+' sign even if the number is positive.
*
* RETURN VALUE:
*   Returns the length of the resulting string. The string within aBuf is not
*   zero terminated.
*
*******************************************************************************/
int EwFormatIntToAnsiString( char* aBuf, long aValue, int aCount, int aRadix,
  int aUpper, int aSign )
{
  unsigned long value;
  char          hex  = aUpper? 'A' : 'a';
  char*         ptr  = aBuf;
  char*         ptr1 = aBuf;
  char*         ptr2;

  /* Is aCount valid? */
  if ( aCount > 128 ) aCount = 128;
  if ( aValue < 0   ) { value  = (unsigned long)-aValue; aSign = 1; }
  else                  value  = (unsigned long) aValue;

  /* Convert the number in binary notation */
  if ( aRadix == 2 )
    do
    {
      *ptr++ = (char)( '0' + ( value & 1 ));
      value >>= 1;
    }
    while ( value > 0 );

  /* Convert the number in octal notation */
  else if ( aRadix == 8 )
    do
    {
      *ptr++ = (char)( '0' + ( value & 7 ));
      value >>= 3;
    }
    while ( value > 0 );

  /* Convert the number in hexadecimal notation */
  else if ( aRadix == 16 )
    do
    {
      if (( value & 15 ) > 9 )
        *ptr++ = (char)( hex + ( value & 15 ) - 10 );
      else
        *ptr++ = (char)( '0' + ( value & 15 ));
      value >>= 4;
    }
    while ( value > 0 );

  /* Convert the number in decimal notation */
  else
    do
    {
      *ptr++ = (char)( '0' + ( value % 10 ));
      value /= 10;
    }
    while ( value > 0 );

  /* Calculate the number of additional '0' signs to add to the string. */
  aCount -= ( ptr - aBuf ) + ( aSign? 1 : 0 );

  /* Fill with zero signs */
  while ( aCount-- > 0 )
    *ptr++ = '0';

  if ( aSign )
  {
    if ( aValue < 0 ) *ptr++ = '-';
    else              *ptr++ = '+';
  }

  /* Finally revert the order of the characters in the formatted string */
  for ( ptr2 = ptr - 1; ptr2 > ptr1; ptr2--, ptr1++ )
  {
    char tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
  }

  return (int)( ptr - aBuf );
}


/*******************************************************************************
* FUNCTION:
*   EwFormatUInt64ToAnsiString
*
* DESCRIPTION:
*   The function EwFormatUInt64ToAnsiString() converts the given unsigned number
*   aValue in an Ansi string and stores the results in the buffer aBuf. The
*   function adds leading zeros '0' until the resulted string has reached the
*   length given in the aCount argument. If the paramneter aSign is != 0, an
*   additional '+' sign will prefix the formatted number.
*
* ARGUMENTS:
*   aBuf   - Buffer to store the resulting string.
*   aValue - Unsigned 64 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*   aUpper - If != 0, use upper case signs for hex digits.
*   aSign  - If != 0, instructs the function to prefix the number with the 
*     '+' sign.
*
* RETURN VALUE:
*   Returns the length of the resulting string. The string within aBuf is not
*   zero terminated.
*
*******************************************************************************/
int EwFormatUInt64ToAnsiString( char* aBuf, XUInt64 aValue, int aCount, 
  int aRadix, int aUpper, int aSign )
{
  char  hex  = aUpper? 'A' : 'a';
  char* ptr  = aBuf;
  char* ptr1 = aBuf;
  char* ptr2;

  /* Is aCount valid? */
  if ( aCount > 128 )
    aCount  = 128;

  /* Convert the number in binary notation */
  if ( aRadix == 2 )
    do
    {
      *ptr++ = (char)( '0' + ( aValue & 1 ));
      aValue >>= 1;
    }
    while ( aValue > 0 );

  /* Convert the number in octal notation */
  else if ( aRadix == 8 )
    do
    {
      *ptr++ = (char)( '0' + ( aValue & 7 ));
      aValue >>= 3;
    }
    while ( aValue > 0 );

  /* Convert the number in hexadecimal notation */
  else if ( aRadix == 16 )
    do
    {
      if (( aValue & 15 ) > 9 )
        *ptr++ = (char)( hex + ( aValue & 15 ) - 10 );
      else
        *ptr++ = (char)( '0' + ( aValue & 15 ));
      aValue >>= 4;
    }
    while ( aValue > 0 );

  /* Convert the number in decimal notation */
  else
    do
    {
      *ptr++ = (char)( '0' + ( aValue % 10 ));
      aValue /= 10;
    }
    while ( aValue > 0 );

  /* Calculate the number of additional '0' signs to add to the string. */
  aCount -= ( ptr - aBuf ) + ( aSign? 1 : 0 );

  /* Fill with zero signs */
  while ( aCount-- > 0 )
    *ptr++ = '0';

  if ( aSign )
    *ptr++ = '+';

  /* Finally revert the order of the characters in the formatted string */
  for ( ptr2 = ptr - 1; ptr2 > ptr1; ptr2--, ptr1++ )
  {
    char tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
  }

  return (int)( ptr - aBuf );
}


/*******************************************************************************
* FUNCTION:
*   EwFormatInt64ToAnsiString
*
* DESCRIPTION:
*   The function EwFormatInt64ToAnsiString() converts the given signed number 
*   aValue in an Ansi string and stores the results in the buffer aBuf. The
*   function adds leading zeros '0' until the resulted string has reached the
*   length given in the aCount argument. If the number is negative or aSign is
*   != 0, the function prefix the formatted number with the '+' or '-' minus
*   sign.
*
* ARGUMENTS:
*   aValue - Signed 64 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*   aUpper - If != 0, use upper case signs for hex digits.
*   aSign  - If != 0, instructs the function to prefix the number with the 
*     '+' sign even if the number is positive.
*
* RETURN VALUE:
*   Returns the length of the resulting string. The string within aBuf is not
*   zero terminated.
*
*******************************************************************************/
int EwFormatInt64ToAnsiString( char* aBuf, XInt64 aValue, int aCount,
  int aRadix, int aUpper, int aSign )
{
  XUInt64 value;
  char    hex  = aUpper? 'A' : 'a';
  char*   ptr  = aBuf;
  char*   ptr1 = aBuf;
  char*   ptr2;

  /* Is aCount valid? */
  if ( aCount > 128 ) aCount = 128;
  if ( aValue < 0   ) { value  = (XUInt64)-aValue; aSign = 1; }
  else                  value  = (XUInt64) aValue;

  /* Convert the number in binary notation */
  if ( aRadix == 2 )
    do
    {
      *ptr++ = (char)( '0' + ( value & 1 ));
      value >>= 1;
    }
    while ( value > 0 );

  /* Convert the number in octal notation */
  else if ( aRadix == 8 )
    do
    {
      *ptr++ = (char)( '0' + ( value & 7 ));
      value >>= 3;
    }
    while ( value > 0 );

  /* Convert the number in hexadecimal notation */
  else if ( aRadix == 16 )
    do
    {
      if (( value & 15 ) > 9 )
        *ptr++ = (char)( hex + ( value & 15 ) - 10 );
      else
        *ptr++ = (char)( '0' + ( value & 15 ));
      value >>= 4;
    }
    while ( value > 0 );

  /* Convert the number in decimal notation */
  else
    do
    {
      *ptr++ = (char)( '0' + ( value % 10 ));
      value /= 10;
    }
    while ( value > 0 );

  /* Calculate the number of additional '0' signs to add to the string. */
  aCount -= ( ptr - aBuf ) + ( aSign? 1 : 0 );

  /* Fill with zero signs */
  while ( aCount-- > 0 )
    *ptr++ = '0';

  if ( aSign )
  {
    if ( aValue < 0 ) *ptr++ = '-';
    else              *ptr++ = '+';
  }

  /* Finally revert the order of the characters in the formatted string */
  for ( ptr2 = ptr - 1; ptr2 > ptr1; ptr2--, ptr1++ )
  {
    char tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
  }

  return (int)( ptr - aBuf );
}


/*******************************************************************************
* FUNCTION:
*   EwFormatFloatToAnsiString
*
* DESCRIPTION:
*   The function EwFormatFloatToAnsiString() converts the given floating point
*   number aValue in an Ansi string and stores the results in the buffer aBuf.
*   The function adds leading zeros '0' until the resulted string has reached
*   the length given in the aCount argument. If the number is negative or aSign
*   is != 0, the function prefixes the formatted number with the '+' or '-' 
*   minus sign.
*
* ARGUMENTS:
*   aValue     - The floating point value to be converted in a string.
*   aCount     - Minimum number of characters to store in the resulted string 
*     output.
*   aPrecision - The aPrecision argument specifies the exact number of digits
*     after the decimal point. If this parameter is < 0, the value specifies
*     the maximum number of digits. Any final '0' (zero) signs are removed
*     in this case.
*   aSign      - If != 0, instructs the function to prefix the number with the 
*     '+' sign even if the number is positive.
*
* RETURN VALUE:
*   Returns the length of the resulting string. The string within aBuf is not
*   zero terminated. If aValue does not contain a valid number, the function
*   returns 0 without storing any data in aBuf.
*
*******************************************************************************/
int EwFormatFloatToAnsiString( char* aBuf, float aValue, int aCount, 
  int aPrecision, int aSign )
{
  void*          valPtr  = (void*)&aValue;
  unsigned int   value   = *(unsigned int*)valPtr;
  int            sign    = value >> 31;  
  int            exp     = (unsigned char)( value >> 23 );
  int            fract   = ( value & 0x7FFFFF ) | ( 1 << 23 );
  unsigned short big[26] = { 0 };
  char*          ptr     = aBuf;
  char*          ptr1    = aBuf;
  char*          ptr2;
  int            shrink  = aPrecision < 0;
  int            done    = 0;
  int            i;
  
  /* Limit to valid range */
  if ( aPrecision < 0   ) aPrecision = -aPrecision;
  if ( aPrecision > 32  ) aPrecision = 32;
  if ( aCount     > 128 ) aCount     = 128;

  /* Invalid number */
  if ( exp == 255 )
    return 0;

  /* Store the value within the big integer. */
  exp += 10;
  i    = exp >> 4;
  big[ i + 0 ] = (unsigned short)( fract << ( exp & 15 ));
  big[ i + 1 ] = (unsigned short)( fract >> ( 16 - ( exp & 15 )));
  big[ i + 2 ] = (unsigned short)(( exp & 15 )? fract >> ( 32 - ( exp & 15 )) : 0 );

  /* Shift the content of the big number untile all precision digits are in
     the integer area (Multiply the number by 10) */
  for ( i = 0; i < aPrecision; i++ )
  {
    int over = 0;
    int j; 
    
    /* Multiply the big number by 10 */
    for ( j = 0; j < 26; j++ )
    {
      big[j] = (unsigned short)( over += big[j] * 10 );
      over >>= 16; 
    }
  }

  /* Round up */
  if ( big[9] & 0x8000 )
    for ( i = 10; i < 26; i++ )
      if ( ++big[i] )
        break;
      
  if ( sign || aSign )
    aCount--;

  /* Format the number */
  while (( aPrecision > 0 ) || !done )
  {
    unsigned int rest = 0;

    done = 1;
    
    /* Divide the whole big number by 10. Detect if there are more data to
       process. Estimate the reminder value */
    for ( i = 25; i > 9; i-- )
    {
      unsigned int a = big[i] + ( rest << 16 );

      if ( a )
      {
        big[i] = (unsigned short)( a / 10 );
        rest   = a % 10;
        done  &= !big[i];
      }
    }

    /* Format the reminder value */
    *ptr++ = (char)( rest + '0' );

    /* Add the digit separator */
    if ( !--aPrecision )
    {
      *ptr++ = '.';
      done   = 0;
      aCount--;
    }

    aCount--;
  }  

  /* Eliminate zero signs at the end of the number */
  while ( shrink && ( *ptr1 == '0' ) && ( ptr1[1] != '.' ))
  {
    ptr1++;
    aCount++;
  }

  /* Eliminate the zero signs */
  if ( ptr1 > aBuf )
  {
    EwMove( aBuf, ptr1, (int)( ptr - ptr1 ));
    ptr  -= ptr1 - aBuf;
    ptr1  = aBuf;
  }

  /* Fill with zero signs */
  while ( aCount-- > 0 )
    *ptr++ = '0';

  /* Append the sign */
  if      ( sign  ) *ptr++ = '-';
  else if ( aSign ) *ptr++ = '+';

  /* Finally revert the order of the characters in the formatted string */
  for ( ptr2 = ptr - 1; ptr2 > ptr1; ptr2--, ptr1++ )
  {
    char tmp = *ptr1;
    *ptr1 = *ptr2;
    *ptr2 = tmp;
  }

  return (int)( ptr - aBuf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewString
*
* DESCRIPTION:
*   The function EwNewString() creates a new copy of the string aString. The
*   function reserves memory for the string and copies it there.
*
*   EwNewString() implements the Chora instant constructor: 'string(aString)'.
*
* ARGUMENTS:
*   aString - 
*
* RETURN VALUE:
*   
*
*******************************************************************************/
XString EwNewString( const XChar* aString )
{
  /* Reserve memory for the copy of the string */
  int     size = WcsLen( aString );
  XString str  = AllocString( size );

  /* Copy the given string aString into the newly reserved memory */
  WcsCpy( str, aString, size );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringAnsi
*
* DESCRIPTION:
*   The function EwNewStringAnsi() creates a wide-character copy of the given
*   ANSI string. The function reserves memory for the new string and copies all
*   characters from the aAnsi string into this reserved memory area. During the
*   operation the function extends all ANSI characters to 16 bit wide-char
*   properly.
*
* ARGUMENTS:
*   aAnsi - Pointer to the 8 bit ANSI string.
*
* RETURN VALUE:
*   Returns a pointer to the wide-character copy of the given ANSI string.
*
*******************************************************************************/
XString EwNewStringAnsi( const char* aAnsi )
{
  int         size;
  const char* tmp1 = aAnsi;
  XString     str;
  XString     tmp2;

  /* Determinate the size of the ANSI string */
  while ( *tmp1 )
    tmp1++;

  /* ... and allocate memory for the Wide-Char string */
  size = (int)( tmp1 - aAnsi );
  str  = AllocString( size );

  /* aAnsi string was empty? -> return Null string */
  if ( !str )
    return 0;

  tmp1 = aAnsi;
  tmp2 = str;

  /* Now copy the ANSI string in the Wide-Char String */
  while ( size-- )
    *tmp2++ = (XChar)(unsigned char)*tmp1++;

  /* Don't forget the zero-terminator sign! */
  *tmp2 = 0;

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringUtf8
*
* DESCRIPTION:
*   The function EwNewStringUtf8() creates a wide-character copy of the given
*   UTF-8 string. The function reserves memory for the new string and decodes
*   the input characters from the aUtf8 string into this reserved memory area.
*   During the operation the function decodes all UTF-8 characters to 16 bit 
*   wide-char properly.
*
* ARGUMENTS:
*   aUtf8  - Pointer to the 8 bit UTF-8 string.
*   aCount - Number of characters (bytes) within the aUtf8 string without any
*     zero terminator signs.
*
* RETURN VALUE:
*   Returns a pointer to the wide-character copy of the decoded UTF-8 string.
*
*******************************************************************************/
XString EwNewStringUtf8( const unsigned char* aUtf8, int aCount )
{
  int     size;
  XString str;

  /* Determinate the size of the decoded UTF-8 string and allocate memory for
     the Wide-Char string */
  size = Utf8Len( aUtf8, aCount );
  str  = AllocString( size );

  /* aUtf8 string was empty? -> return Null string */
  if ( !str )
    return 0;

  /* Now decode the UTF-8 string in the Wide-Char string */
  DecodeUtf8( str, size, aUtf8, aCount );

  /* Don't forget the zero-terminator sign! */
  str[ size ] = 0;

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringUInt
*
* DESCRIPTION:
*   The function EwNewStringUInt() converts the given unsigned number aValue in
*   a string and returns the result. The function adds leading zeros '0' until 
*   the resulted string has reached the length given in the aCount argument.
*
*   EwNewStringUInt() implements the Chora instant constructor: 
*   'string(aValue,aNoOfDigits)'.
*
* ARGUMENTS:
*   aValue - Unsigned 32 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns a string containing the given number aValue converted in notation
*   according to the parameter aRadix.
*
*******************************************************************************/
XString EwNewStringUInt( XUInt32 aValue, XInt32 aCount, XInt32 aRadix )
{
  char buf[ 129 ];
  int  len = EwFormatUIntToAnsiString( buf, aValue, aCount, aRadix, 0, 0 );

  /* Terminate the string */
  buf[ len ] = 0;

  return EwNewStringAnsi( buf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringInt
*
* DESCRIPTION:
*   The function EwNewStringInt() converts the given signed number aValue in a 
*   string and returns the result. The function adds leading zeros '0' until the
*   resulted string has reached the length given in the aCount argument. If the
*   number is negative, the function adds '-' minus sign.
*
*   EwNewStringInt() implements the Chora instant constructor: 
*   'string(aValue,aNoOfDigits,aRadix)'.
*
* ARGUMENTS:
*   aValue - Signed 32 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns a string containing the given number aValue converted in notation
*   according to the parameter aRadix.
*
*******************************************************************************/
XString EwNewStringInt( XInt32 aValue, XInt32 aCount, XInt32 aRadix )
{
  char buf[ 129 ];
  int  len = EwFormatIntToAnsiString( buf, aValue, aCount, aRadix, 0, 0 );

  /* Terminate the string */
  buf[ len ] = 0;

  return EwNewStringAnsi( buf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringUInt64
*
* DESCRIPTION:
*   The function EwNewStringUInt64() converts the given unsigned number aValue 
*   in a string and returns the result. The function adds leading zeros '0' 
*   until the resulted string has reached the length given in the aCount 
*   argument.
*
*   EwNewStringUInt64() implements the Chora instant constructor: 
*   'string(aValue,aNoOfDigits)'.
*
* ARGUMENTS:
*   aValue - Unsigned 64 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns a string containing the given number aValue converted in notation
*   according to the parameter aRadix.
*
*******************************************************************************/
XString EwNewStringUInt64( XUInt64 aValue, XInt32 aCount, XInt32 aRadix )
{
  char buf[ 129 ];
  int  len = EwFormatUInt64ToAnsiString( buf, aValue, aCount, aRadix, 0, 0 );

  /* Terminate the string */
  buf[ len ] = 0;

  return EwNewStringAnsi( buf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringInt64
*
* DESCRIPTION:
*   The function EwNewStringInt64() converts the given signed number aValue in a
*   string and returns the result. The function adds leading zeros '0' until the
*   resulted string has reached the length given in the aCount argument. If the
*   number is negative, the function adds '-' minus sign.
*
*   EwNewStringInt() implements the Chora instant constructor: 
*   'string(aValue,aNoOfDigits,aRadix)'.
*
* ARGUMENTS:
*   aValue - Signed 64 bit value to be converted to string.
*   aCount - Desired length of the resulted string.
*   aRadix - Controls the format of the resulting string. This parameter can
*     assume values 2, 8, 10 or 16. Accordingly the number is converted in a
*     binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns a string containing the given number aValue converted in notation
*   according to the parameter aRadix.
*
*******************************************************************************/
XString EwNewStringInt64( XInt64 aValue, XInt32 aCount, XInt32 aRadix )
{
  char buf[ 129 ];
  int  len = EwFormatInt64ToAnsiString( buf, aValue, aCount, aRadix, 0, 0 );

  /* Terminate the string */
  buf[ len ] = 0;

  return EwNewStringAnsi( buf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringFloat
*
* DESCRIPTION:
*   The function EwNewStringFloat() converts the given floating point number 
*   aValue in a string and returns the result. 
*
*   EwNewStringFloat() implements the Chora instant constructor: 
*   'string(aValue,aNoOfDigits,aPrecision)'.
*
* ARGUMENTS:
*   aValue     - The floating point value to be converted in a string.
*   aCount     - Minimum number of characters to store in the resulted string 
*     output.
*   aPrecision - The aPrecision argument specifies the exact number of digits
*     after the decimal point. If this parameter is < 0, the function will
*     automatically estimate the number of digits after the decimal point.
*
* RETURN VALUE:
*   Returns a string containing the given floating point number aValue.
*
*******************************************************************************/
XString EwNewStringFloat( XFloat aValue, XInt32 aCount, XInt32 aPrecision )
{
  char buf[ 129 ];
  int  len = EwFormatFloatToAnsiString( buf, aValue, aCount, 
                                        ( aPrecision < 0 )? -6 : aPrecision, 0 );

  /* The floating point number was not valid -> Not-A-Number */
  if ( !len )
    return EwNewStringAnsi( "nan" );
    
  /* Terminate the string */
  buf[ len ] = 0;

  return EwNewStringAnsi( buf );
}


/*******************************************************************************
* FUNCTION:
*   EwNewStringChar
*
* DESCRIPTION:
*   The function EwNewStringChar() creates a string with the given length aCount 
*   and fills the whole string with the character aChar.
*
*   EwNewStringChar() implements the Chora instant constructor: 
*   'string(aChar,aNoOfDigits)'.
*
* ARGUMENTS:
*   aChar  - Character to fill the string.
*   aCount - Desired length of the resulted string.
*
* RETURN VALUE:
*   Returns a string containing aCount characters aChar.
*
*******************************************************************************/
XString EwNewStringChar( XChar aChar, XInt32 aCount )
{
  /* Reserve memory for a new string */
  XString str = AllocString((int)aCount );
  XString tmp = str;

  /* aCount was <= 0, so AllocString() has returned 0 too -> return Null 
     string */
  if ( !str )
    return 0;

  /* Fill the String with the character aChar */
  while ( aCount-- )
    *str++ = aChar;

  /* ... and terminate the string properly */
  *str = 0;
  return tmp;
}


/*******************************************************************************
* FUNCTION:
*   EwCompString
*
* DESCRIPTION:
*   The function EwCompString() compares aString1 and aString2 lexicographically
*   and returns a value indicating their relationship. 
*
*   EwCompString() implements the Chora instant operators: 'string == string'
*   'string != string', 'string < string', ...
*
* ARGUMENTS:
*   aString1 - The first string to be compared.
*   aString2 - The second string to be compared.
*
* RETURN VALUE:
*   If aString1 is identical to aString2 the function returns 0 (zero).
*   If aString1 is less than aString2 the function returns -1.
*   If aString1 is greater than aString2 the function returns +1.
*
*******************************************************************************/
int EwCompString( XString aString1, XString aString2 )
{
  /* Avoid that EmWi stores strings not created by its Runtime Environment */
  return WcsCmp( VerifyString( aString1 ), VerifyString( aString2 ));
}


/*******************************************************************************
* FUNCTION:
*   EwLoadString
*
* DESCRIPTION:
*   The function EwLoadString() returns the pointer to the 'string' constant
*   described by aStringConst. Depending on the configuration of the system, the
*   'string' may need to be decompressed before it can be used. In such case all
*   decompressed strings are managed within a common string cache. If possible,
*   the function will reuse such already existing strings.
*
*   The function returns the pointer to the first character of the decompressed
*   'string' constant. If 'string' constants are not compressed, the function
*   returns the pointer to the ROM area where the 'string' is stored.
*
* ARGUMENTS:
*   aStringConst - Pointer to the descriptor of the constant.
*
* RETURN VALUE:
*   Returns a pointer to the string constant.
*
*******************************************************************************/
XString EwLoadString( const XStringRes* aStringConst )
{
  const XChar* ptr        = (const XChar*)aStringConst->Block;
  int          ofs        = aStringConst->Offset;
  int          useReader  = EwFlashAreaReaderProc && 
                            ((const void*)ptr >= EwFlashAreaStartAddress ) &&
                            ((const void*)ptr <= EwFlashAreaEndAddress   );
  int          compressed = *((const unsigned int*)( useReader? 
                            EwFlashAreaReaderProc( ptr ) : ptr )) != 0xFFFFFFFF;

  /* Uncompressed string block starts always with a 0xFFFFFFFF magic word.
     Such strings can be accessed directly from ROM - as long as the CPU can
     address the memory area. */
  if ( !useReader && !compressed )
    return (XString)( ptr + ofs );

  /* The affected string resides within a compressed block. Decompress it
     first. Also possible, the string is stored uncompressed in the not 
     directly accessible memory area. Then copy the string in RAM. */
  else
  {
    /* At first search in th cache for the memory block, where the desired 
       string is stored. Note: compressed strings are managed in blocks with
       several strings together. If the string has been copied from a not 
       directly accessible memory area, we manage each string within its
       individual block to save RAM. Depending on this mode, we search either
       for the complete block or the particular string only. */
    struct XStringBlock* block = FindBlock( ptr + ( compressed? 0 : ofs ));

    do
    {
      /* If not found load the block from the ROM code. Note, the block can 
         contain compressed or uncompressed data. Select the operation. */
      if ( !block )
      {
        if ( compressed )
        {
          #ifndef EW_DONT_USE_COMPRESSION
            block = LoadBlock((const unsigned int*)ptr, useReader );

          /* The decompression is excluded - thus compressed strings can't be
             used. */
          #else
            EwError( 391 );
            return 0;
          #endif
        }
        else
          block = LoadString( ptr + ofs );
      }

      /* otherwise, remove the block from the cache temporary - so it can be moved
         later to the begin of the cache. */
      else
        UncacheBlock( block );
    }
    while ( !block && EwImmediateReclaimMemory( 2 ));

    /* Failed? Return the empty string */
    if ( !block )
    {
      EwError( 2 );
      return 0;
    }

    /* Now store the block at the begin of the cache */
    CacheBlock( block );

    /* Return the pointer to the first character of the desired string. */
    return (XString)( block + 1 ) + ( compressed? ofs : 2 );
  }
}


/*******************************************************************************
* FUNCTION:
*   EwShareString
*
* DESCRIPTION:
*   The function EwShareString() verifies whether the string passed in the
*   parameter aString is a private copy of the string and changes it to a
*   shared copy. Private string copies are created when a string is modified
*   by the string index operator []. In such case, the operator creates a copy
*   of the original string. Later the index operator can operate on the string
*   without the necessity to copy it again and again. As soon as the string is
*   assigned to a new variable, etc. the optimization will not work anymore.
*   To avoid any incoherence in the string contents, assigning private strings
*   to a variable changes their state to shared. Next time when such shared
*   string is modified with the index operator, a new copy of the string is
*   created. This is a kind of optimization to perfmorm an efficient copy-on-
*   write string behavior.
*
* ARGUMENTS:
*   aString - Pointer to the string to be changed from private to shared.
*
* RETURN VALUE:
*   Returns the string passed in aString.
*
*******************************************************************************/
XString EwShareString( XString aString )
{
  VerifyString( aString );

  /* Change the status of the string from 'private' to 'normal' or 'shared' */
  if ( aString && ( aString[-1] == EW_PRIV_STRING_MAGIC_NO ))
    aString[-1] = EW_STRING_MAGIC_NO;

  return aString;    
}


/*******************************************************************************
* FUNCTION:
*   EwConcatString
*
* DESCRIPTION:
*   The function EwConcatString() creates a new string large enought to store a
*   copy of aString1 and aString2. Then the function copies the content of 
*   aString1 and appends the content of the second string aStrings2.
*
*   EwConcatString() implements the Chora instant operator: 'string + string'.
*
* ARGUMENTS:
*   aString1 - The first string.
*   aString2 - The second string.
*
* RETURN VALUE:
*   Returns a string containing a copy of aString1 followed by a copy of 
*   aString2.
*
*******************************************************************************/
XString EwConcatString( XString aString1, XString aString2 )
{
  int     size1 = WcsLen( VerifyString( aString1 ));
  int     size2 = WcsLen( VerifyString( aString2 ));
  XString str;

  /* One of the strings is empty? -> No concatation necessary */
  if ( !size1 ) return aString2;
  if ( !size2 ) return aString1;

  /* Reserve memory for the new string */
  str = AllocString( size1 + size2 );

  /* Allocation failed? */
  if ( !str )
    return 0;

  /* The both strings aString1 and aString2 will be concated together to get
     one long string */
  WcsCpy( str, aString1, size1 );
  WcsCpy( str + size1, aString2, size2 );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwConcatStringChar
*
* DESCRIPTION:
*   The function EwConcatStringChar() appends the character aChar at the end of
*   the string aString and returns the result.
*
*   EwConcatStringChar() implements the Chora instant operator: 
*   'string + char'.
*
* ARGUMENTS:
*   aString - The string.
*   aChar   - The character to be appended to the string.
*
* RETURN VALUE:
*   Returns a string containing a copy of aString followed by aChar.
*
*******************************************************************************/
XString EwConcatStringChar( XString aString, XChar aChar )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Nothing to do (zero-terminator sign) */
  if ( !aChar )
    return aString;

  /* Ok, reserve memory for the new string */
  str = AllocString( size + 1 );

  /* Allocation failed? */
  if ( !str )
    return 0;

  /* Append aChar at the end of aString */
  WcsCpy( str, aString, size );
  str[ size ]     = aChar;
  str[ size + 1 ] = 0;

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwConcatCharString
*
* DESCRIPTION:
*   The function EwConcatCharString() appends aString to the character aChar 
*   and returns the result.
*
*   EwConcatCharString() implements the Chora instant operator: 
*   'char + string'.
*
* ARGUMENTS:
*   aChar   - The character.
*   aString - The string to be apended.
*
* RETURN VALUE:
*   Returns a string containing aChar followed by a copy of aString.
*
*******************************************************************************/
XString EwConcatCharString( XChar aChar, XString aString )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Nothing to do (zero-terminator sign) */
  if ( !aChar )
    return aString;

  /* Ok, reserve memory for the new string */
  str = AllocString( size + 1 );

  /* Allocation failed? */
  if ( !str )
    return 0;

  /* Append aString at the end of aChar */
  str[ 0 ] = aChar;
  str[ 1 ] = 0;
  WcsCpy( str + 1, aString, size );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwGetStringChar
*
* DESCRIPTION:
*   The function EwGetStringChar() determinates the character stored at position 
*   aIndex in the string aString. The function returns the zero character if the
*   specified aIndex addresses a character lying beyond the string boundary.
*
*   EwGetStringChar() implements the read access to Chora instant operator: 
*   'string[aIndex]'
*
* ARGUMENTS:
*   aString - The string to get the character.
*   aIndex  - The position of the desired character in the string.
*
* RETURN VALUE:
*   Returns the character stored at position aIndex within the string aString.
*
*******************************************************************************/
XChar EwGetStringChar( XString aString, XInt32 aIndex )
{
  /* Is the given index valid? */
  if ( !WcsCanAccess( VerifyString( aString ), aIndex ))
    return 0;

  /* Return the required character */
  return aString[ aIndex ];
}


/*******************************************************************************
* FUNCTION:
*   EwSetStringChar
*
* DESCRIPTION:
*   The function EwSetStringChar() changes the character stored at position 
*   aIndex in the string passed in the parameter aString to a new value aChar.
*   The function throws an error if the desired character position aIndex is
*   not valid.
*
*   If the originally passed string is a marked internally as being a private
*   string copy, the operation modifies the string in-place. In turn, if the
*   string is marked as being shared (e.g. stored in multiple variables), the
*   operation creates first a copy of the original string and uses it to 
*   change the character.
*
*   EwSetStringChar() implements the write access to Chora instant operator: 
*   'string[aIndex]'
*
* ARGUMENTS:
*   aString - The string to set the character.
*   aIndex  - The position of the desired character in the string.
*
* RETURN VALUE:
*   Returns a string with the changed character. This can be the either the
*   string passed originally in the parameter aString if the string is marked
*   internally as being a private string copy or it can be a copy of the string
*   if it is marked as being shared between several variables. Therefore you
*   should always use the returned value.
*
*******************************************************************************/
XString EwSetStringChar( XString aString, XInt32 aIndex, XChar aChar )
{
  /* Is the given index valid? */
  if ( !WcsCanAccess( VerifyString( aString ), aIndex ))
  {
    EwError( 302 );
    EwPanic();
    return 0;
  }

  /* Trying to modify a non private version of the string. Thus create first
     a private copy of it. */
  if ( aString[-1] != EW_PRIV_STRING_MAGIC_NO )
    aString = EwNewString( aString );

  /* Replace the strings character aIndex with aChar */
  aString[ aIndex ] = aChar;
  return aString;
}


/*******************************************************************************
* FUNCTION:
*   EwGetStringLength
*
* DESCRIPTION:
*   The function EwGetStringLength() returns the length of the string aString 
*   in characters. If the string is empty, the function returns 0.
*
*   EwGetStringLength() implements the Chora instant property: 'string.length'
*
* ARGUMENTS:
*   aString - The string to determinate the length of.
*
* RETURN VALUE:
*   Returns the length of the string in characters or 0 if the string is empty.
*
*******************************************************************************/
XInt32 EwGetStringLength( XString aString )
{
  return WcsLen( VerifyString( aString ));
}


/*******************************************************************************
* FUNCTION:
*   EwIsStringEmpty
*
* DESCRIPTION:
*   The function EwIsStringEmpty() returns != 0 if the string aString does not
*   contain any characters. If the string is not empty, the function returns 0.
*
* ARGUMENTS:
*   aString - The string to verify.
*
* RETURN VALUE:
*   Returns != 0 if the string is empty.
*
*******************************************************************************/
XBool EwIsStringEmpty( XString aString )
{
  return !aString || !*VerifyString( aString );
}


/*******************************************************************************
* FUNCTION:
*   EwGetStringUpper
*
* DESCRIPTION:
*   The function EwGetStringUpper() converts the string aString to uppercase
*   and returns an altered copy of the string.
*
*   EwGetStringUpper() implements the Chora instant property: 'string.upper'
*
* ARGUMENTS:
*   aString - String to capitalize.
*
* RETURN VALUE:
*   Returns uppercase copy of the string.
*
*******************************************************************************/
XString EwGetStringUpper( XString aString )
{
  /* Reserve memory for a new string */
  int     size = WcsLen( VerifyString( aString ));
  XString str  = AllocString( size );
  XString str2 = str;
  XChar   ch;

  /* Copy the source string aString into the newly reserved memory and ... */
  WcsCpy( str, aString, size );

  /* convert lowercase characters into uppercase (please see description of the
     above function ToUpper() containing a list of supported UNICODE Blocks) */
  if ( str2 )
    while (( ch = *str2 ) != 0 )
      *str2++ = (XChar)ToUpper( ch );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwGetStringLower
*
* DESCRIPTION:
*   The function EwGetStringLower() converts the string aString to lowercase
*   and returns an altered copy of the string.
*
*   EwGetStringLower() implements the Chora instant property: 'string.lower'
*
* ARGUMENTS:
*   aString - String to convert to lowercase.
*
* RETURN VALUE:
*   Returns lowercase copy of the string.
*
*******************************************************************************/
XString EwGetStringLower( XString aString )
{
  /* Reserve memory for a new string */
  int     size = WcsLen( VerifyString( aString ));
  XString str  = AllocString( size );
  XString str2 = str;
  XChar   ch;

  /* Copy the source string aString into the newly reserved memory and ... */
  WcsCpy( str, aString, size );

  /* convert uppercase characters into lowercase (please see description of the
     above function ToLower() containing a list of supported UNICODE Blocks) */
  if ( str2 )
    while (( ch = *str2 ) != 0 )
      *str2++ = (XChar)ToLower( ch );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringLeft
*
* DESCRIPTION:
*   The function EwStringLeft() extracts the first (that is, leftmost) aCount 
*   characters from the string aString and returns a copy of the extracted 
*   substring. If aCount exceeds the string length, then the entire string is 
*   extracted.
*
*   EwStringLeft() implements the Chora instant method: 'string.left(aCount)'
*
* ARGUMENTS:
*   aString - The string to extract a substring.
*   aCount  - The maximum number of characters to extract from aString.
*
* RETURN VALUE:
*   Returns the extracted substring or if aCount == 0 the function returns an
*   empty string.
*
*******************************************************************************/
XString EwStringLeft( XString aString, XInt32 aCount )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Don't copy characters outside aString */
  if ( aCount > size )
    aCount = size;

  /* If the whole aString will be copied -> then there is no extra memory
     allocation necessary -> we can return aString immediately */
  if ( aCount == size )
    return aString;

  /* Empty string? */
  if ( !aCount )
    return 0;

  /* Otherwise we need extra memory for a new string to store ... */
  str = AllocString((int)aCount );

  /* ... a part of aString */
  WcsCpy( str, aString, (int)aCount );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringRight
*
* DESCRIPTION:
*   The function EwStringRight() extracts the last (that is, rightmost) aCount 
*   characters from the string aString and returns a copy of the extracted 
*   substring. If aCount exceeds the string length, then the entire string is 
*   extracted. 
*
*   EwStringRight() implements the Chora instant method: 'string.right(aCount)'
*
* ARGUMENTS:
*   aString - The string to extract a substring.
*   aCount  - The maximum number of characters to extract from aString.
*
* RETURN VALUE:
*   Returns the extracted substring or if aCount == 0 the function returns an
*   empty string.
*
*******************************************************************************/
XString EwStringRight( XString aString, XInt32 aCount )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Don't copy characters outside aString */
  if ( aCount > size )
    aCount = size;

  /* If the whole aString will be copied -> then there is no extra memory
     allocation necessary -> we can return aString immediately */
  if ( aCount == size )
    return aString;

  /* Empty string? */
  if ( !aCount )
    return 0;

  /* Otherwise we need extra memory for a new string to store ... */
  str = AllocString((int)aCount );

  /* ... a part of aString */
  WcsCpy( str, aString + size - aCount, (int)aCount );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringMiddle
*
* DESCRIPTION:
*   The function EwStringMiddle() extracts a substring of length aCount 
*   characters from the string aString, starting at position aIndex. The 
*   function returns a copy of the extracted substring. 
*
*   EwStringMiddle() implements the Chora instant method: 
*   'string.middle(aIndex,aCount)'
*
* ARGUMENTS:
*   aString - The string to extract a substring.
*   aIndex  - The index of the first character in the string where the extracted
*     substring begins.
*   aCount  - The maximum number of characters to extract from aString.
*
* RETURN VALUE:
*   Returns the extracted substring or if aCount == 0 the function returns an
*   empty string.
*
*******************************************************************************/
XString EwStringMiddle( XString aString, XInt32 aIndex, XInt32 aCount )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* aIndex out of range? */
  if ( aIndex < 0 )
  {
    aCount += aIndex;
    aIndex  = 0;
  }

  /* aIndex out of range (outside the string) -> then don't copy any characters
     from aString */
  if ( aIndex >= size )
    aCount = 0;

  /* Don't copy characters outside aString */
  if (( aIndex + aCount ) > size )
    aCount = size - aIndex;

  /* If the whole aString will be copied -> then there is no extra memory
     allocation necessary -> we can return aString immediately */
  if ( aCount == size )
    return aString;

  /* Empty string? */
  if ( !aCount )
    return 0;

  /* Otherwise we need extra memory for a new string to store ... */
  str = AllocString((int)aCount );

  /* ... a part of aString */
  WcsCpy( str, aString + aIndex, (int)aCount );

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringInsert
*
* DESCRIPTION:
*   The function EwStringInsert() inserts the substring aString2 at the given 
*   position aIndex within the string aString1. If aIndex is <= 0 zero, the 
*   insertion will occur before the entire string. If aIndex is > than the 
*   length of the string, the function will concatenate aString1 and aString2
*   together.
*
*   EwStringInsert() implements the Chora instant method: 
*   'string.insert(aString,aIndex)'
*
* ARGUMENTS:
*   aString1 - The string to insert a substring.
*   aString2 - The substring to be inserted.
*   aIndex   - The position in aString1 where the substring should be inserted.
*
* RETURN VALUE:
*   Returns a new string containing aString1 and aString2.
*
*******************************************************************************/
XString EwStringInsert( XString aString1, XString aString2, XInt32 aIndex )
{
  int     size1 = WcsLen( VerifyString( aString1 ));
  int     size2 = WcsLen( VerifyString( aString2 ));
  XString str;

  /* Nothing to insert? */
  if ( !size2 )
    return aString1;

  /* Nothing to insert to? */
  if ( !size1 )
    return aString2;

  /* Outside the string */
  if ( aIndex < 0 )
    aIndex = 0;

  if ( aIndex >= size1 )
    aIndex = size1;

  /* Get memory for a new string */
  str = AllocString((int)( size1 + size2 ));

  /* Allocation failed? */
  if ( !str )
    return 0;

  /* Insert aString2 into aString1 */
  WcsCpy( str, aString1, (int)aIndex );
  WcsCpy( str + aIndex, aString2, size2 );
  WcsCpy( str + aIndex + size2, aString1 + aIndex, (int)( size1 - aIndex ));

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringRemove
*
* DESCRIPTION:
*   The function EwStringRemove() removes up to aCount characters from the 
*   string aString starting with the character at position aIndex. 
*
*   EwStringRemove() implements the Chora instant method: 
*   'string.remove(aIndex,aCount)'
*
* ARGUMENTS:
*   aString - The string to remove characters.
*   aIndex  - The index of the first character to remove.
*   aCount  - The number of characters to be removed.
*
* RETURN VALUE:
*   Returns a new string containing altered aString.
*
*******************************************************************************/
XString EwStringRemove( XString aString, XInt32 aIndex, XInt32 aCount )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Outside the string? */
  if ( aIndex < 0 )
  {
    aCount += aIndex;
    aIndex  = 0;
  }

  /* Don't remove characters outside the string */
  if (( aIndex + aCount ) > size )
    aCount = size - aIndex;

  /* Nothing to remove -> aString will not change */
  if ( aCount <= 0 )
    return aString;

  /* All characters of aString will be removed -> return empty (zero) string */
  if ( aCount == size )
    return 0;

  /* Get memory for a new string */
  str = AllocString((int)( size - aCount ));

  /* Allocation failed? */
  if ( !str )
    return 0;

  /* Remove aCount characters from aString */
  WcsCpy( str, aString, (int)aIndex );
  WcsCpy( str + aIndex, aString + aIndex + aCount, (int)( size - aIndex - aCount ));

  return str;
}


/*******************************************************************************
* FUNCTION:
*   EwStringFindChar
*
* DESCRIPTION:
*   The function EwStringFindChar() searches the string aString for the first 
*   occurence of a character aChar. EwStringFindChar() starts the search 
*   operation at the position aStartIndex in the string. The function returns 
*   the index of the first character in aString1 that matches the requested 
*   character.
*
*   EwStringFindChar() implements the Chora instant method: 
*   'string.find(aChar,aStartIndex)'
*
* ARGUMENTS:
*   aString     - The string to serach with.
*   aChar       - A character to search for.
*   aStartIndex - The position of the character to begin the search.
*
* RETURN VALUE:
*   Returns the position in the string aString where the character aChar has 
*   been found or -1 if aChar has not been found.
*
*******************************************************************************/
XInt32 EwStringFindChar( XString aString, XChar aChar, XInt32 aStartIndex )
{
  int     size = WcsLen( VerifyString( aString ));
  XString str;

  /* Is aStartIndex valid? */
  if (( aStartIndex < 0 ) || ( aStartIndex >= size ))
    return -1;

  /* Search or aChar character */
  aString += aStartIndex;
  str      = WcsChr( aString, aChar );

  /* Not found? */
  if ( !str )
    return -1;

  /* Return the index of the found character */
  return aStartIndex + (int)( str - aString );
}


/*******************************************************************************
* FUNCTION:
*   EwStringFind
*
* DESCRIPTION:
*   The function StringFind() searches the string aString for the first 
*   occurence of a substring aString2. EwStringFind() starts the search 
*   operation at the position aStartIndex in the string. The function returns 
*   the index of the first character in aString1 that matches the requested 
*   substring.
*
*   EwStringFind() implements the Chora instant method: 
*   'string.find(aString,aStartIndex)'
*
* ARGUMENTS:
*   aString1    - The string to serach with.
*   aString2    - A substring to search for.
*   aStartIndex - The position of the character to begin the search.
*
* RETURN VALUE:
*   Returns the position in the string aString1 where the substring aString2 
*   has been found or -1 if aString2 has not been found.
*
*******************************************************************************/
XInt32 EwStringFind( XString aString1, XString aString2, XInt32 aStartIndex )
{
  int     size = WcsLen( VerifyString( aString1 ));
  XString str;

  VerifyString( aString2 );

  /* Is aStartIndex valid? */
  if (( aStartIndex < 0 ) || ( aStartIndex >= size ))
    return -1;

  /* Search or aString2 */
  aString1 += aStartIndex;
  str       = WcsStr( aString1, aString2 );

  /* Not found? */
  if ( !str )
    return -1;

  /* Return the index of the found string */
  return aStartIndex + (int)( str - aString1 );
}


/*******************************************************************************
* FUNCTION:
*   EwStringParseInt32
*
* DESCRIPTION:
*   The function EwStringParseInt32() parses the string interpreting its content
*   as an integral number. The number can be prefixed by an optional +/- sign.
*   Whitespace signs lying at the begin of the string are skipped over.
*
*   The function parses the numbers according to the notation resulting from the
*   parameter aRadix. In this manner strings with binary, octal, decimal or even
*   hexadecimal notation can be read.
*
*   Generally the function tries to read as many signs as possible. If the end of
*   the string or an unexpected sign is found, the function stops and returns the
*   already read number as signed integer value. If the string is invalid (it
*   doesn't contain any expected digit or letter), the value passed in the
*   parameter aDefault is returned instead.
*
*   EwStringParseInt32() implements the Chora instant method: 
*   'string.parse_int32(aDefault,aRadix)'
*
* ARGUMENTS:
*   aString  - The string to parse its content.
*   aDefault - Value to return if the string doesn't contain a valid number.
*   aRadix   - Determines the format of the number in the string. This parameter
*     can assume values 2, 8, 10 or 16. Accordingly the string content is assumed
*     as being a number with binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns the parsed number value as signed integer or the value from the
*   parameter aDefault if the string doesn't contain a valid number.
*
*******************************************************************************/
XInt32 EwStringParseInt32( XString aString, XInt32 aDefault, XInt32 aRadix )
{
  const XChar* ptr  = SkipSpace( VerifyString( aString ));
  XInt32       num  = 0;
  XInt32       sign = 1;

  /* Empty string passed */
  if ( !ptr )
    return aDefault;

  /* Evaluate the +/- sign */
  if (( *ptr == '+' ) || ( *ptr == '-' ))
    sign = ( *ptr++ == '-' )? -1 : 1;

  /* If no valid radix is specified - use base 10 */
  if (( aRadix != 2 ) && ( aRadix != 8 ) && ( aRadix != 16 ))
    aRadix = 10;

  /* Verify whether the number contains at least one valid sign. If no, the
     default value is returned. */
  if ((( aRadix != 16 ) && (( *ptr < '0' ) || ( *ptr > ( '0' + aRadix - 1 )))) ||
      ((( *ptr < '0' ) || ( *ptr > '9' )) && 
       (( *ptr < 'a' ) || ( *ptr > 'f' )) &&
       (( *ptr < 'A' ) || ( *ptr > 'F' ))))
    return aDefault;

  /* Parse as binary number? */
  if ( aRadix == 2 )
    do
      num = num * 2 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '1' ));
  
  /* Parse as octal number? */
  else if ( aRadix == 8 )
    do
      num = num * 8 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '7' ));

  /* Parse as decimal number? */
  else if ( aRadix == 10 )
    do
      num = num * 10 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '9' ));
  
  /* Parse as hex-decimal number? */
  else
    do
    {
      if (( *ptr >= 'a' ) && ( *ptr <= 'f' ))
        num = num * 16 + ( *ptr++ - 'a' + 10 );
      else if (( *ptr >= 'A' ) && ( *ptr <= 'F' ))
        num = num * 16 + ( *ptr++ - 'A' + 10 );
      else
        num = num * 16 + ( *ptr++ - '0' );
    }
    while ((( *ptr >= '0' ) && ( *ptr <= '9' )) ||
           (( *ptr >= 'a' ) && ( *ptr <= 'f' )) ||
           (( *ptr >= 'A' ) && ( *ptr <= 'F' )));

  return num * sign;
}


/*******************************************************************************
* FUNCTION:
*   EwStringParseUInt32
*
* DESCRIPTION:
*   The function EwStringParseUInt32() parses the string interpreting its content
*   as an integral number. The number can be prefixed by an optional +/- sign.
*   Whitespace signs lying at the begin of the string are skipped over.
*
*   The function parses the numbers according to the notation resulting from the
*   parameter aRadix. In this manner strings with binary, octal, decimal or even
*   hexadecimal notation can be read.
*
*   Generally the function tries to read as many signs as possible. If the end of
*   the string or an unexpected sign is found, the function stops and returns the
*   already read number as unsigned integer value. If the string is invalid (it
*   doesn't contain any expected digit or letter), the value passed in the 
*   parameter aDefault is returned instead.
*
*   EwStringParseUInt32() implements the Chora instant method: 
*   'string.parse_uint32(aDefault,aRadix)'
*
* ARGUMENTS:
*   aString  - The string to parse its content.
*   aDefault - Value to return if the string doesn't contain a valid number.
*   aRadix   - Determines the format of the number in the string. This parameter
*     can assume values 2, 8, 10 or 16. Accordingly the string content is assumed
*     as being a number with binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns the parsed number value as unsigned integer or the value from the
*   parameter aDefault if the string doesn't contain a valid number.
*
*******************************************************************************/
XUInt32 EwStringParseUInt32( XString aString, XUInt32 aDefault, XInt32 aRadix )
{
  const XChar* ptr  = SkipSpace( VerifyString( aString ));
  XUInt32      num  = 0;
  int          sign = 1;

  /* Empty string passed */
  if ( !ptr )
    return aDefault;

  /* Evaluate the +/- sign */
  if (( *ptr == '+' ) || ( *ptr == '-' ))
    sign = ( *ptr++ == '-' )? -1 : 1;

  /* If no valid radix is specified - use base 10 */
  if (( aRadix != 2 ) && ( aRadix != 8 ) && ( aRadix != 16 ))
    aRadix = 10;

  /* Verify whether the number contains at least one valid sign. If no, the
     default value is returned. */
  if ((( aRadix != 16 ) && (( *ptr < '0' ) || ( *ptr > ( '0' + aRadix - 1 )))) ||
      ((( *ptr < '0' ) || ( *ptr > '9' )) && 
       (( *ptr < 'a' ) || ( *ptr > 'f' )) &&
       (( *ptr < 'A' ) || ( *ptr > 'F' ))))
    return aDefault;

  /* Parse as binary number? */
  if ( aRadix == 2 )
    do
      num = num * 2 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '1' ));
  
  /* Parse as octal number? */
  else if ( aRadix == 8 )
    do
      num = num * 8 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '7' ));

  /* Parse as decimal number? */
  else if ( aRadix == 10 )
    do
      num = num * 10 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '9' ));
  
  /* Parse as hex-decimal number? */
  else
    do
    {
      if (( *ptr >= 'a' ) && ( *ptr <= 'f' ))
        num = num * 16 + ( *ptr++ - 'a' + 10 );
      else if (( *ptr >= 'A' ) && ( *ptr <= 'F' ))
        num = num * 16 + ( *ptr++ - 'A' + 10 );
      else
        num = num * 16 + ( *ptr++ - '0' );
    }
    while ((( *ptr >= '0' ) && ( *ptr <= '9' )) ||
           (( *ptr >= 'a' ) && ( *ptr <= 'f' )) ||
           (( *ptr >= 'A' ) && ( *ptr <= 'F' )));

  /* Apply the sign */
  if ( sign < 0 )
    return (XUInt32)-(XInt32)num;
  else
    return num;
}


/*******************************************************************************
* FUNCTION:
*   EwStringParseInt64
*
* DESCRIPTION:
*   The function EwStringParseInt64() parses the string interpreting its content
*   as an integral number. The number can be prefixed by an optional +/- sign.
*   Whitespace signs lying at the begin of the string are skipped over.
*
*   The function parses the numbers according to the notation resulting from the
*   parameter aRadix. In this manner strings with binary, octal, decimal or even
*   hexadecimal notation can be read.
*
*   Generally the function tries to read as many signs as possible. If the end of
*   the string or an unexpected sign is found, the function stops and returns the
*   already read number as signed integer value. If the string is invalid (it
*   doesn't contain any expected digit or letter), the value passed in the
*   parameter aDefault is returned instead.
*
*   EwStringParseInt64() implements the Chora instant method: 
*   'string.parse_int64(aDefault,aRadix)'
*
* ARGUMENTS:
*   aString  - The string to parse its content.
*   aDefault - Value to return if the string doesn't contain a valid number.
*   aRadix   - Determines the format of the number in the string. This parameter
*     can assume values 2, 8, 10 or 16. Accordingly the string content is assumed
*     as being a number with binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns the parsed number value as signed integer or the value from the
*   parameter aDefault if the string doesn't contain a valid number.
*
*******************************************************************************/
XInt64 EwStringParseInt64( XString aString, XInt64 aDefault, XInt32 aRadix )
{
  const XChar* ptr  = SkipSpace( VerifyString( aString ));
  XInt64       num  = 0;
  XInt64       sign = 1;

  /* Empty string passed */
  if ( !ptr )
    return aDefault;

  /* Evaluate the +/- sign */
  if (( *ptr == '+' ) || ( *ptr == '-' ))
    sign = ( *ptr++ == '-' )? -1 : 1;

  /* If no valid radix is specified - use base 10 */
  if (( aRadix != 2 ) && ( aRadix != 8 ) && ( aRadix != 16 ))
    aRadix = 10;

  /* Verify whether the number contains at least one valid sign. If no, the
     default value is returned. */
  if ((( aRadix != 16 ) && (( *ptr < '0' ) || ( *ptr > ( '0' + aRadix - 1 )))) ||
      ((( *ptr < '0' ) || ( *ptr > '9' )) && 
       (( *ptr < 'a' ) || ( *ptr > 'f' )) &&
       (( *ptr < 'A' ) || ( *ptr > 'F' ))))
    return aDefault;

  /* Parse as binary number? */
  if ( aRadix == 2 )
    do
      num = num * 2 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '1' ));
  
  /* Parse as octal number? */
  else if ( aRadix == 8 )
    do
      num = num * 8 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '7' ));

  /* Parse as decimal number? */
  else if ( aRadix == 10 )
    do
      num = num * 10 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '9' ));
  
  /* Parse as hex-decimal number? */
  else
    do
    {
      if (( *ptr >= 'a' ) && ( *ptr <= 'f' ))
        num = num * 16 + ( *ptr++ - 'a' + 10 );
      else if (( *ptr >= 'A' ) && ( *ptr <= 'F' ))
        num = num * 16 + ( *ptr++ - 'A' + 10 );
      else
        num = num * 16 + ( *ptr++ - '0' );
    }
    while ((( *ptr >= '0' ) && ( *ptr <= '9' )) ||
           (( *ptr >= 'a' ) && ( *ptr <= 'f' )) ||
           (( *ptr >= 'A' ) && ( *ptr <= 'F' )));

  return num * sign;
}


/*******************************************************************************
* FUNCTION:
*   EwStringParseUInt64
*
* DESCRIPTION:
*   The function EwStringParseUInt64() parses the string interpreting its content
*   as an integral number. The number can be prefixed by an optional +/- sign.
*   Whitespace signs lying at the begin of the string are skipped over.
*
*   The function parses the numbers according to the notation resulting from the
*   parameter aRadix. In this manner strings with binary, octal, decimal or even
*   hexadecimal notation can be read.
*
*   Generally the function tries to read as many signs as possible. If the end of
*   the string or an unexpected sign is found, the function stops and returns the
*   already read number as unsigned integer value. If the string is invalid (it
*   doesn't contain any expected digit or letter), the value passed in the 
*   parameter aDefault is returned instead.
*
*   EwStringParseUInt64() implements the Chora instant method: 
*   'string.parse_uint64(aDefault,aRadix)'
*
* ARGUMENTS:
*   aString  - The string to parse its content.
*   aDefault - Value to return if the string doesn't contain a valid number.
*   aRadix   - Determines the format of the number in the string. This parameter
*     can assume values 2, 8, 10 or 16. Accordingly the string content is assumed
*     as being a number with binary, octal, decimal or hexadecimal notation.
*
* RETURN VALUE:
*   Returns the parsed number value as unsigned integer or the value from the
*   parameter aDefault if the string doesn't contain a valid number.
*
*******************************************************************************/
XUInt64 EwStringParseUInt64( XString aString, XUInt64 aDefault, XInt32 aRadix )
{
  const XChar* ptr  = SkipSpace( VerifyString( aString ));
  XUInt64      num  = 0;
  int          sign = 1;

  /* Empty string passed */
  if ( !ptr )
    return aDefault;

  /* Evaluate the +/- sign */
  if (( *ptr == '+' ) || ( *ptr == '-' ))
    sign = ( *ptr++ == '-' )? -1 : 1;

  /* If no valid radix is specified - use base 10 */
  if (( aRadix != 2 ) && ( aRadix != 8 ) && ( aRadix != 16 ))
    aRadix = 10;

  /* Verify whether the number contains at least one valid sign. If no, the
     default value is returned. */
  if ((( aRadix != 16 ) && (( *ptr < '0' ) || ( *ptr > ( '0' + aRadix - 1 )))) ||
      ((( *ptr < '0' ) || ( *ptr > '9' )) && 
       (( *ptr < 'a' ) || ( *ptr > 'f' )) &&
       (( *ptr < 'A' ) || ( *ptr > 'F' ))))
    return aDefault;

  /* Parse as binary number? */
  if ( aRadix == 2 )
    do
      num = num * 2 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '1' ));
  
  /* Parse as octal number? */
  else if ( aRadix == 8 )
    do
      num = num * 8 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '7' ));

  /* Parse as decimal number? */
  else if ( aRadix == 10 )
    do
      num = num * 10 + ( *ptr++ - '0' );
    while (( *ptr >= '0' ) && ( *ptr <= '9' ));
  
  /* Parse as hex-decimal number? */
  else
    do
    {
      if (( *ptr >= 'a' ) && ( *ptr <= 'f' ))
        num = num * 16 + ( *ptr++ - 'a' + 10 );
      else if (( *ptr >= 'A' ) && ( *ptr <= 'F' ))
        num = num * 16 + ( *ptr++ - 'A' + 10 );
      else
        num = num * 16 + ( *ptr++ - '0' );
    }
    while ((( *ptr >= '0' ) && ( *ptr <= '9' )) ||
           (( *ptr >= 'a' ) && ( *ptr <= 'f' )) ||
           (( *ptr >= 'A' ) && ( *ptr <= 'F' )));

  /* Apply the sign */
  if ( sign < 0 )
    return (XUInt64)-(XInt64)num;
  else
    return num;
}


/*******************************************************************************
* FUNCTION:
*   EwStringParseFloat
*
* DESCRIPTION:
*   The function EwStringParseFloat() parses the string interpreting its content
*   as a floating point number. The number can be prefixed by an optional +/- 
*   sign. Whitespace signs lying at the begin of the string are skipped over. 
*
*   In the floating point notation the number can consist of an integer and/or
*   a fractional portion. The fractional portion starts with a '.'. Optionally
*   the number can be followed by an exponent portion. The exponent portion
*   starts with the e or E sign followed by an optional +/- sign and an integral
*   number. The following example shows the complete number with all portions:
*
*   "123.456e-07"
*
*   Generally the function tries to read as many signs as possible. If the end
*   of the string or an unexpected sign is found, the function stops and returns
*   the already read number as floating point value. If the string is invalid
*   (it doesn't contain any expected digit), the value passed in the parameter
*   aDefault is returned instead.
*
*   EwStringParseFloat() implements the Chora instant method: 
*   'string.parse_float(aDefault)'
*
* ARGUMENTS:
*   aString  - The string to parse its content.
*   aDefault - Value to return if the string doesn't contain a valid number.
*
* RETURN VALUE:
*   Returns the parsed number value as floating point or the value from the
*   parameter aDefault if the string doesn't contain a valid number.
*
*******************************************************************************/
XFloat EwStringParseFloat( XString aString, XFloat aDefault )
{
  const XChar* ptr  = SkipSpace( VerifyString( aString ));
  XFloat       num  = 0.0f;
  int          sign = 1;
  int          exp  = 0;

  /* Empty string passed */
  if ( !ptr )
    return aDefault;

  /* Evaluate the +/- sign */
  if (( *ptr == '+' ) || ( *ptr == '-' ))
    sign = ( *ptr++ == '-' )? -1 : 1;

  /* Verify whether the number contains at least one valid sign. If no, the
     default value is returned. */
  if ((( *ptr < '0' ) || ( *ptr > '9' )) &&
      (( *ptr != '.' ) || ( ptr[1] < '0' ) || ( ptr[1] > '9' )))
    return aDefault;

  /* Parse the 'integer portion' */
  while (( *ptr >= '0' ) && ( *ptr <= '9' ))
    num = num * 10.0f + (XFloat)( *ptr++ - '0' );

  /* Any 'fractional portion' specified? */
  if ( *ptr == '.' )
  {
    XFloat exp = 0.1f;
    ptr++;

    /* Parse the 'fractional portion' */
    while (( *ptr >= '0' ) && ( *ptr <= '9' ))
    {
      num += exp * (XFloat)( *ptr++ - '0' );
      exp *= 0.1f;
    }
  }

  /* Any 'exponent' specified? */
  if (( *ptr == 'e' ) || ( *ptr == 'E' ))
  {
    int sign = 1;

    ptr++;

    /* Read the sign of the exponent */
    if      ( *ptr == '+' ) ptr++;
    else if ( *ptr == '-' ) { ptr++; sign = -1; }

    /* Parse the exponent */
    while (( *ptr >= '0' ) && ( *ptr <= '9' ))
      exp = exp * 10 + ( *ptr++ - '0' );

    exp *= sign;
  }

  /* Apply the exponent */
  if ( exp )
    num *= EwMathPow( 10.0f, (XFloat)( exp ));

  /* Apply the sign */
  if ( sign < 0 )
    return -num;
  else
    return num;
}


/*******************************************************************************
* FUNCTION:
*   EwStringToAnsi
*
* DESCRIPTION:
*   The function EwStringToAnsi() converts up to aCount characters from the 
*   given 16 bit wide char string aString to an 8 bit ANSI string and stores it
*   in the memory area aDest. The resulted string is terminated by the zero
*   0x00.
*
*   Character codes greater than 255 are replaced by the aDefChar value. This
*   is necessary, because the character codes in the source string can became
*   values in range 0 .. 65535. The destination string can accomodate codes
*   in the range 0 .. 255 only.
*
*   The function returns the number of converted characters incl. the last zero
*   terminator 0x00.
*
* ARGUMENTS:
*   aString  - The source 16 bit wide char string to convert.
*   aDest    - Pointer to the destination memory, where the converted 8 bit 
*     ANSI string should be stored in. This memory area has to be at least
*     aCount bytes large.
*   aCount   - The number of characters incl. the zero terminator to convert.
*   aDefChar - Default character to replace all characters, which are > 255.
*
* RETURN VALUE:
*   Returns the number of bytes written into the aDest memory area.
*
*******************************************************************************/
XInt32 EwStringToAnsi( XString aString, char* aDest, XInt32 aCount,
  char aDefChar )
{
  char* dest = aDest;

  VerifyString( aString );

  /* Is there space for the zero terminator character? */
  if ( aCount < 1 )
    return 0;

  /* Convert the string sign by sign until the zero terminator is reached,
     or the destination memory area is full */
  if ( aString )
    while (( --aCount > 0 ) && *aString )
    {
      /* Character code out of range? */
      if ( *aString > 255 )
        *dest = aDefChar;
      else
        *dest = (char)*aString;

      /* Continue with the next character */
      aString++;
      dest++;
    }

  /* Terminate the string */
  *dest++ = 0;

  /* How many characters are written */
  return (int)( dest - aDest );
}


/*******************************************************************************
* FUNCTION:
*   EwGetUtf8StringLength
*
* DESCRIPTION:
*   The function EwGetUtf8StringLength() analyzes the characters in the given
*   16 bit wide char string aString and estimates the length in bytes for a
*   corresponding UTF8 string. This value can be used to reserve memory before
*   using the function EwStringToUtf8().
*
* ARGUMENTS:
*   aString  - The source 16 bit wide char string to convert.
*
* RETURN VALUE:
*   Returns the number of bytes to store the corresponding string in UTF8 
*   format without the zero terminator sign.
*
*******************************************************************************/
XInt32 EwGetUtf8StringLength( XString aString )
{
  XInt32 count = 0;

  VerifyString( aString );

  /* Traverse all characters within the original string and estimate the memory
     usage for the corresponding UTF8 sequences */
  for ( ; aString && *aString; aString++ )
  {
    if      ( *aString <= 0x007F ) count += 1;
    else if ( *aString <= 0x07FF ) count += 2;
    else                           count += 3;
  }

  return count;
}


/*******************************************************************************
* FUNCTION:
*   EwStringToUtf8
*
* DESCRIPTION:
*   The function EwStringToUtf8() converts the characters from the given 16 bit
*   wide char string aString to an 8 bit UTF8 string and stores it in the memory
*   area aDest. The resulted string is terminated by the zero 0x00.
*
*   The function returns the number of bytes written in aDest incl. the last 
*   zero terminator 0x00.
*
* ARGUMENTS:
*   aString  - The source 16 bit wide char string to convert.
*   aDest    - Pointer to the destination memory, where the converted 8 bit 
*     UTF8 string should be stored in. This memory area has to be at least
*     aCount bytes large.
*   aCount   - The number of bytes within aDest available for the operation.
*
* RETURN VALUE:
*   Returns the number of bytes written into the aDest memory area incl. the 
*   zero terminator sign.
*
*******************************************************************************/
XInt32 EwStringToUtf8( XString aString, unsigned char* aDest, XInt32 aCount )
{
  unsigned char* dest = aDest;

  VerifyString( aString );

  /* At least space for one character to serve as zero terminator? */
  if ( aCount-- <= 0 )
    return 0;

  /* At least one character for the zero terminator is needed */
  for ( ; aString && *aString && ( aCount > 0 ); aString++ )
  {
    XChar c = *aString;

    if      ( c <= 0x007F ) { *dest++ = (unsigned char)c; aCount--; }
    else if ( c <= 0x07FF )
    {
      if ( aCount < 2 )
        break;

      *dest++ = 0xC0 | (unsigned char)( c >> 6   );
      *dest++ = 0x80 | (unsigned char)( c & 0x3F );
      aCount -= 2;
    } 
    else
    {
      if ( aCount < 3 )
        break;

      *dest++ = 0xE0 | (unsigned char)(  c >> 12 );
      *dest++ = 0x80 | (unsigned char)(( c >> 6  ) & 0x3F );
      *dest++ = 0x80 | (unsigned char)( c & 0x3F );
      aCount -= 3;
    } 
  }

  *dest++ = 0;
  return (int)( dest - aDest );
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfString
*
* DESCRIPTION:
*   The function EwGetVariantOfString() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple string values.
*
* RETURN VALUE:
*   Returns the determinated string value.
*
*******************************************************************************/
XString EwGetVariantOfString( const XVariant* aConstant )
{
  return EwLoadString( &EwGetVariantOf( aConstant, XVariantOfString )->Value );
}


/*******************************************************************************
* FUNCTION:
*   EwGetVariantOfChar
*
* DESCRIPTION:
*   The function EwGetVariantOfChar() will be called to determinate a value
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
*   aVariants - A pointer to the constant containing multiple char values.
*
* RETURN VALUE:
*   Returns the determinated char value.
*
*******************************************************************************/
XChar EwGetVariantOfChar( const XVariant* aConstant )
{
  return EwGetVariantOf( aConstant, XVariantOfChar )->Value;
}


/*******************************************************************************
* FUNCTION:
*   EwGetCharUpper
*
* DESCRIPTION:
*   The function EwGetCharUpper() converts the character aChar to uppercase.
*
*   EwGetCharUpper() implements the Chora instant property: 'char.upper'
*
* ARGUMENTS:
*   aChar - Character to convert to uppercase.
*
* RETURN VALUE:
*   Returns uppercase copy of the character.
*
*******************************************************************************/
XChar EwGetCharUpper( XChar aChar )
{
  return (XChar)ToUpper( aChar );
}


/*******************************************************************************
* FUNCTION:
*   EwGetCharLower
*
* DESCRIPTION:
*   The function EwGetCharLower() converts the character aChar to lowercase.
*
*   EwGetCharLower() implements the Chora instant property: 'char.lower'
*
* ARGUMENTS:
*   aChar - Character to convert to lowercase.
*
* RETURN VALUE:
*   Returns lowercase copy of the character.
*
*******************************************************************************/
XChar EwGetCharLower( XChar aChar )
{
  return (XChar)ToLower( aChar );
}


/* pba, msy */

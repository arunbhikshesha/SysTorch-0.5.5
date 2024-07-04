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
*   This module implements the UNICODE BIDIRECTIONAL ALGORITHM according to the
*   Unicode Standard Annex #9 Unicode version 10.0.0.
*
* SUBROUTINES:
*   EwCreateBidi
*   EwFreeBidi
*   EwBidiIsNeeded
*   EwBidiIsRTL
*   EwBidiGetCharLevel
*   EwBidiInit
*   EwBidiProcess
*   EwBidiApplyShaping
*   EwBidiMirrorGlyphs
*   EwBidiCompleteRow
*   EwBidiReorderChars
*   EwBidiReorderDWords
*   EwBidiReorderIndex
*   EwBidiReorderReverseIndex
*   EwBidiGetTypesAndLevels
*
*******************************************************************************/

#include "ewrte.h"
#include "ewgfxdriver.h"
#include "ewgfxcore.h"
#include "ewgfxres.h"
#include "ewgfx.h"


/* Following are the categories to associate to the characters in the processed
   text as well as diverse status types used by the BIDI algorithm to track the
   evaluated source text. */
#define BIDI_TYPE_L               1
#define BIDI_TYPE_R               2
#define BIDI_TYPE_AL              3
#define BIDI_TYPE_EN              4
#define BIDI_TYPE_ES              5
#define BIDI_TYPE_ET              6
#define BIDI_TYPE_AN              7
#define BIDI_TYPE_CS              8
#define BIDI_TYPE_NSM             9
#define BIDI_TYPE_BN             10
#define BIDI_TYPE_B              11
#define BIDI_TYPE_S              12
#define BIDI_TYPE_WS             13
#define BIDI_TYPE_ON             14
#define BIDI_TYPE_LRE            15
#define BIDI_TYPE_LRO            16
#define BIDI_TYPE_RLE            17
#define BIDI_TYPE_RLO            18
#define BIDI_TYPE_PDF            19
#define BIDI_TYPE_LRI            20
#define BIDI_TYPE_RLI            21
#define BIDI_TYPE_FSI            22
#define BIDI_TYPE_PDI            23
#define BIDI_TYPE_AL_R           24
#define BIDI_TYPE_AL_L           25
#define BIDI_TYPE_AL_D           26
#define BIDI_TYPE_AL_C           27
#define BIDI_TYPE_GAP            31
#define BIDI_TYPE_MASK           31

#define BRACKET_TYPE_OPEN        ( 1 << 5 ) 
#define BRACKET_TYPE_CLOSE       ( 2 << 5 )
#define MIRROR_GLYPH             ( 3 << 5 )
#define BRACKET_MASK             ( 3 << 5 )
#define JOIN_TYPE_T              ( 1 << 7 )

#define STATUS_BRACKET_OPEN       1
#define STATUS_BRACKET_CLOSE      2
#define STATUS_MIRROR_GLYPH       3
#define STATUS_WAS_TYPE_NSM       4  /* Implies join type 'T' */
#define STATUS_WAS_TYPE_WS        5
#define STATUS_WAS_TYPE_SB        6
#define STATUS_WAS_TYPE_ISOLATE   7
#define STATUS_JOIN_TYPE_R        8
#define STATUS_JOIN_TYPE_L        9
#define STATUS_JOIN_TYPE_D       10
#define STATUS_JOIN_TYPE_C       11
#define STATUS_JOIN_TYPE_T       12


/* Size constraints resulting from the BIDI specification */
#define BIDI_STACK_MAX_DEPTH    125
#define BRACKET_STACK_MAX_DEPTH  63


/* Following data structure represents a single character of the processed
   text as it is stored internally by the BIDI algorithm. */
typedef struct
{
  unsigned short Type   : 5;  /* See above the BIDI_TYPE_XXX defines */
  unsigned short Status : 4;  /* See above the STATUS_XXX defines */
  unsigned short Level  : 7;
} XBidiData;


/* Following structure represents a single entry on the BIDI stack. */
typedef struct
{
  unsigned char Level;
  unsigned char Override : 6; /* See above BIDI_TYPE_XXX defines */
  unsigned char Isolate  : 1;
} XBidiStackEntry;


/* The following structure stores all information associated with a respective
   source text and the corresponding BIDI process. */
typedef struct
{
  int           MaxSize;        /* Max. capacity of Data */
  int           Size;           /* Actual number of entries in Data */
  XBidiData*    Data;           /* Array with BIDI entries */
  unsigned char ParagraphLevel; /* 0 -> LTR, 1 -> RTL */
  unsigned char IsBidi;         /* != if the original text needs Bidi */
} XBidiContext;


/* Helper macros for accessing the attribute information associated to entities
   within an attributed text paragraph. Every entity manages its attributes as a
   32-bit DWORD value. Please note, these definitions should be the same as in
   ewgfxattrtext.c */
#define GET_ATTR_TYPE( aA )   ((aA)  & 0x0000000F)
#define GET_ATTR_FONT( aA )   (((aA) & 0x000FF000) >> 12 )


/* This helper macro returns the minimum one of the both arguments */
#ifdef MIN
  #undef MIN
#endif
#define MIN( a, b ) (((a) < (b))? (a) : (b))


/* Memory usage profiler */
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwMemoryPeak;


/* Performance counter for performance investigation and debugging only */
EW_PERF_COUNTER( EwCreateBidi,              Bidi_Text_API )
EW_PERF_COUNTER( EwFreeBidi,                Bidi_Text_API )
EW_PERF_COUNTER( EwBidiIsNeeded,            Bidi_Text_API )
EW_PERF_COUNTER( EwBidiIsRTL,               Bidi_Text_API )
EW_PERF_COUNTER( EwBidiGetCharLevel,        Bidi_Text_API )
EW_PERF_COUNTER( EwBidiInit,                Bidi_Text_API )
EW_PERF_COUNTER( EwBidiProcess,             Bidi_Text_API )
EW_PERF_COUNTER( EwBidiApplyShaping,        Bidi_Text_API )
EW_PERF_COUNTER( EwBidiMirrorGlyphs,        Bidi_Text_API )
EW_PERF_COUNTER( EwBidiCompleteRow,         Bidi_Text_API )
EW_PERF_COUNTER( EwBidiReorderChars,        Bidi_Text_API )
EW_PERF_COUNTER( EwBidiReorderDWords,       Bidi_Text_API )
EW_PERF_COUNTER( EwBidiReorderIndex,        Bidi_Text_API )
EW_PERF_COUNTER( EwBidiReverseReorderIndex, Bidi_Text_API )


/* Map the Graphics Engine API to the corresponding functions if no performance
   counters are active  */
#ifndef EW_PRINT_PERF_COUNTERS
  #define EwCreateBidi_              EwCreateBidi
  #define EwFreeBidi_                EwFreeBidi
  #define EwBidiIsNeeded_            EwBidiIsNeeded
  #define EwBidiIsRTL_               EwBidiIsRTL
  #define EwBidiGetCharLevel_        EwBidiGetCharLevel
  #define EwBidiInit_                EwBidiInit
  #define EwBidiProcess_             EwBidiProcess
  #define EwBidiApplyShaping_        EwBidiApplyShaping
  #define EwBidiMirrorGlyphs_        EwBidiMirrorGlyphs
  #define EwBidiCompleteRow_         EwBidiCompleteRow
  #define EwBidiReorderChars_        EwBidiReorderChars
  #define EwBidiReorderDWords_       EwBidiReorderDWords
  #define EwBidiReorderIndex_        EwBidiReorderIndex
  #define EwBidiReverseReorderIndex_ EwBidiReverseReorderIndex
#endif


#ifndef EW_DONT_USE_BIDI_FUNCTIONS

/* The following constant data array contains Unicode character codes in sorted
   order extracted from the Unicode database. Every consecutive two entries in
   that array determine a range of Unicode charcaters <from>-<to> having equal
   BidiClass+BracketType properties. This array is used to find entries for a 
   given Unicode character code. */
static const unsigned short SortedCodes[] =
{ 
  0x0000, 0x0009, 0x000A, 0x000B, 0x000C, 0x000D, 0x000E, 0x001C, 0x001F, 0x0020, 
  0x0021, 0x0023, 0x0026, 0x0028, 0x0029, 0x002A, 0x002B, 0x002C, 0x002D, 0x002E, 
  0x0030, 0x003A, 0x003B, 0x003C, 0x003D, 0x003E, 0x003F, 0x0041, 0x005B, 0x005C, 
  0x005D, 0x005E, 0x0061, 0x007B, 0x007C, 0x007D, 0x007E, 0x007F, 0x0085, 0x0086, 
  0x00A0, 0x00A1, 0x00A2, 0x00A6, 0x00AA, 0x00AB, 0x00AC, 0x00AD, 0x00AE, 0x00B0, 
  0x00B2, 0x00B4, 0x00B5, 0x00B6, 0x00B9, 0x00BA, 0x00BB, 0x00BC, 0x00C0, 0x00D7, 
  0x00D8, 0x00F7, 0x00F8, 0x02B9, 0x02BB, 0x02C2, 0x02D0, 0x02D2, 0x02E0, 0x02E5, 
  0x02EE, 0x02EF, 0x0300, 0x0370, 0x0374, 0x0376, 0x037E, 0x037F, 0x0384, 0x0386, 
  0x0387, 0x0388, 0x03F6, 0x03F7, 0x0483, 0x048A, 0x058A, 0x058B, 0x058D, 0x058F, 
  0x0590, 0x0591, 0x05BE, 0x05BF, 0x05C0, 0x05C1, 0x05C3, 0x05C4, 0x05C6, 0x05C7, 
  0x05C8, 0x0600, 0x0606, 0x0608, 0x0609, 0x060B, 0x060C, 0x060D, 0x060E, 0x0610, 
  0x061B, 0x061C, 0x061D, 0x0620, 0x0621, 0x0622, 0x0626, 0x0627, 0x0628, 0x0629, 
  0x062A, 0x062F, 0x0633, 0x0640, 0x0641, 0x0648, 0x0649, 0x064B, 0x0660, 0x066A, 
  0x066B, 0x066D, 0x066E, 0x0670, 0x0671, 0x0674, 0x0675, 0x0678, 0x0688, 0x069A, 
  0x06C0, 0x06C1, 0x06C3, 0x06CC, 0x06CD, 0x06CE, 0x06CF, 0x06D0, 0x06D2, 0x06D4, 
  0x06D5, 0x06D6, 0x06DD, 0x06DE, 0x06DF, 0x06E5, 0x06E7, 0x06E9, 0x06EA, 0x06EE, 
  0x06F0, 0x06FA, 0x06FD, 0x06FF, 0x0700, 0x070F, 0x0710, 0x0711, 0x0712, 0x0715, 
  0x071A, 0x071E, 0x071F, 0x0728, 0x0729, 0x072A, 0x072B, 0x072C, 0x072D, 0x072F, 
  0x0730, 0x074B, 0x074D, 0x074E, 0x0759, 0x075C, 0x076B, 0x076D, 0x0771, 0x0772, 
  0x0773, 0x0775, 0x0778, 0x077A, 0x0780, 0x07A6, 0x07B1, 0x07C0, 0x07EB, 0x07F4, 
  0x07F6, 0x07FA, 0x0816, 0x081A, 0x081B, 0x0824, 0x0825, 0x0828, 0x0829, 0x082E, 
  0x0859, 0x085C, 0x0860, 0x0861, 0x0862, 0x0866, 0x0867, 0x0868, 0x0869, 0x086B, 
  0x0870, 0x08A0, 0x08AA, 0x08AD, 0x08AE, 0x08AF, 0x08B1, 0x08B3, 0x08B5, 0x08B6, 
  0x08B9, 0x08BA, 0x08BE, 0x08D4, 0x08E2, 0x08E3, 0x0903, 0x093A, 0x093B, 0x093C, 
  0x093D, 0x0941, 0x0949, 0x094D, 0x094E, 0x0951, 0x0958, 0x0962, 0x0964, 0x0981, 
  0x0982, 0x09BC, 0x09BD, 0x09C1, 0x09C5, 0x09CD, 0x09CE, 0x09E2, 0x09E4, 0x09F2, 
  0x09F4, 0x09FB, 0x09FC, 0x0A01, 0x0A03, 0x0A3C, 0x0A3D, 0x0A41, 0x0A43, 0x0A47, 
  0x0A49, 0x0A4B, 0x0A4E, 0x0A51, 0x0A52, 0x0A70, 0x0A72, 0x0A75, 0x0A76, 0x0A81, 
  0x0A83, 0x0ABC, 0x0ABD, 0x0AC1, 0x0AC6, 0x0AC7, 0x0AC9, 0x0ACD, 0x0ACE, 0x0AE2, 
  0x0AE4, 0x0AF1, 0x0AF2, 0x0AFA, 0x0B00, 0x0B01, 0x0B02, 0x0B3C, 0x0B3D, 0x0B3F, 
  0x0B40, 0x0B41, 0x0B45, 0x0B4D, 0x0B4E, 0x0B56, 0x0B57, 0x0B62, 0x0B64, 0x0B82, 
  0x0B83, 0x0BC0, 0x0BC1, 0x0BCD, 0x0BCE, 0x0BF3, 0x0BF9, 0x0BFA, 0x0BFB, 0x0C00, 
  0x0C01, 0x0C3E, 0x0C41, 0x0C46, 0x0C49, 0x0C4A, 0x0C4E, 0x0C55, 0x0C57, 0x0C62, 
  0x0C64, 0x0C78, 0x0C7F, 0x0C81, 0x0C82, 0x0CBC, 0x0CBD, 0x0CBF, 0x0CC0, 0x0CC6, 
  0x0CC7, 0x0CCC, 0x0CCE, 0x0CE2, 0x0CE4, 0x0D00, 0x0D02, 0x0D3B, 0x0D3D, 0x0D41, 
  0x0D45, 0x0D4D, 0x0D4E, 0x0D62, 0x0D64, 0x0DCA, 0x0DCB, 0x0DD2, 0x0DD5, 0x0DD6, 
  0x0DD7, 0x0E31, 0x0E32, 0x0E34, 0x0E3B, 0x0E3F, 0x0E40, 0x0E47, 0x0E4F, 0x0EB1, 
  0x0EB2, 0x0EB4, 0x0EBA, 0x0EBB, 0x0EBD, 0x0EC8, 0x0ECE, 0x0F18, 0x0F1A, 0x0F35, 
  0x0F36, 0x0F37, 0x0F38, 0x0F39, 0x0F3A, 0x0F3B, 0x0F3C, 0x0F3D, 0x0F3E, 0x0F71, 
  0x0F7F, 0x0F80, 0x0F85, 0x0F86, 0x0F88, 0x0F8D, 0x0F98, 0x0F99, 0x0FBD, 0x0FC6, 
  0x0FC7, 0x102D, 0x1031, 0x1032, 0x1038, 0x1039, 0x103B, 0x103D, 0x103F, 0x1058, 
  0x105A, 0x105E, 0x1061, 0x1071, 0x1075, 0x1082, 0x1083, 0x1085, 0x1087, 0x108D, 
  0x108E, 0x109D, 0x109E, 0x135D, 0x1360, 0x1390, 0x139A, 0x1400, 0x1401, 0x1680, 
  0x1681, 0x169B, 0x169C, 0x169D, 0x1712, 0x1715, 0x1732, 0x1735, 0x1752, 0x1754, 
  0x1772, 0x1774, 0x17B4, 0x17B6, 0x17B7, 0x17BE, 0x17C6, 0x17C7, 0x17C9, 0x17D4, 
  0x17DB, 0x17DC, 0x17DD, 0x17DE, 0x17F0, 0x17FA, 0x1800, 0x180B, 0x180E, 0x180F, 
  0x1885, 0x1887, 0x18A9, 0x18AA, 0x1920, 0x1923, 0x1927, 0x1929, 0x1932, 0x1933, 
  0x1939, 0x193C, 0x1940, 0x1941, 0x1944, 0x1946, 0x19DE, 0x1A00, 0x1A17, 0x1A19, 
  0x1A1B, 0x1A1C, 0x1A56, 0x1A57, 0x1A58, 0x1A5F, 0x1A60, 0x1A61, 0x1A62, 0x1A63, 
  0x1A65, 0x1A6D, 0x1A73, 0x1A7D, 0x1A7F, 0x1A80, 0x1AB0, 0x1ABF, 0x1B00, 0x1B04, 
  0x1B34, 0x1B35, 0x1B36, 0x1B3B, 0x1B3C, 0x1B3D, 0x1B42, 0x1B43, 0x1B6B, 0x1B74, 
  0x1B80, 0x1B82, 0x1BA2, 0x1BA6, 0x1BA8, 0x1BAA, 0x1BAB, 0x1BAE, 0x1BE6, 0x1BE7, 
  0x1BE8, 0x1BEA, 0x1BED, 0x1BEE, 0x1BEF, 0x1BF2, 0x1C2C, 0x1C34, 0x1C36, 0x1C38, 
  0x1CD0, 0x1CD3, 0x1CD4, 0x1CE1, 0x1CE2, 0x1CE9, 0x1CED, 0x1CEE, 0x1CF4, 0x1CF5, 
  0x1CF8, 0x1CFA, 0x1DC0, 0x1DFA, 0x1DFB, 0x1E00, 0x1FBD, 0x1FBE, 0x1FBF, 0x1FC2, 
  0x1FCD, 0x1FD0, 0x1FDD, 0x1FE0, 0x1FED, 0x1FF0, 0x1FFD, 0x1FFF, 0x2000, 0x200B, 
  0x200C, 0x200D, 0x200E, 0x200F, 0x2010, 0x2028, 0x2029, 0x202A, 0x202B, 0x202C, 
  0x202D, 0x202E, 0x202F, 0x2030, 0x2035, 0x2039, 0x203B, 0x2044, 0x2045, 0x2046, 
  0x2047, 0x205F, 0x2060, 0x2065, 0x2066, 0x2067, 0x2068, 0x2069, 0x206A, 0x2070, 
  0x2071, 0x2074, 0x207A, 0x207C, 0x207D, 0x207E, 0x207F, 0x2080, 0x208A, 0x208C, 
  0x208D, 0x208E, 0x208F, 0x20A0, 0x20D0, 0x20F1, 0x2100, 0x2102, 0x2103, 0x2107, 
  0x2108, 0x210A, 0x2114, 0x2115, 0x2116, 0x2119, 0x211E, 0x2124, 0x2125, 0x2126, 
  0x2127, 0x2128, 0x2129, 0x212A, 0x212E, 0x212F, 0x213A, 0x213C, 0x2140, 0x2145, 
  0x214A, 0x214E, 0x2150, 0x2160, 0x2189, 0x218C, 0x2190, 0x2208, 0x220E, 0x2212, 
  0x2213, 0x2214, 0x2215, 0x2216, 0x223C, 0x223E, 0x2243, 0x2244, 0x2252, 0x2256, 
  0x2264, 0x226C, 0x226E, 0x228C, 0x228F, 0x2293, 0x2298, 0x2299, 0x22A2, 0x22A4, 
  0x22A6, 0x22A7, 0x22A8, 0x22AA, 0x22AB, 0x22AC, 0x22B0, 0x22B8, 0x22C9, 0x22CE, 
  0x22D0, 0x22D2, 0x22D6, 0x22EE, 0x22F0, 0x22F5, 0x22F6, 0x22F8, 0x22FA, 0x22FF, 
  0x2308, 0x2309, 0x230A, 0x230B, 0x230C, 0x2329, 0x232A, 0x232B, 0x2336, 0x237B, 
  0x2395, 0x2396, 0x2427, 0x2440, 0x244B, 0x2460, 0x2488, 0x249C, 0x24EA, 0x26AC, 
  0x26AD, 0x2768, 0x2769, 0x276A, 0x276B, 0x276C, 0x276D, 0x276E, 0x276F, 0x2770, 
  0x2771, 0x2772, 0x2773, 0x2774, 0x2775, 0x2776, 0x27C3, 0x27C5, 0x27C6, 0x27C7, 
  0x27C8, 0x27CA, 0x27CB, 0x27CC, 0x27CD, 0x27CE, 0x27D5, 0x27D7, 0x27DD, 0x27DF, 
  0x27E2, 0x27E6, 0x27E7, 0x27E8, 0x27E9, 0x27EA, 0x27EB, 0x27EC, 0x27ED, 0x27EE, 
  0x27EF, 0x27F0, 0x2800, 0x2900, 0x2983, 0x2984, 0x2985, 0x2986, 0x2987, 0x2988, 
  0x2989, 0x298A, 0x298B, 0x298C, 0x298D, 0x298E, 0x298F, 0x2990, 0x2991, 0x2992, 
  0x2993, 0x2994, 0x2995, 0x2996, 0x2997, 0x2998, 0x2999, 0x29B8, 0x29B9, 0x29C0, 
  0x29C2, 0x29C4, 0x29C6, 0x29CF, 0x29D3, 0x29D4, 0x29D6, 0x29D8, 0x29D9, 0x29DA, 
  0x29DB, 0x29DC, 0x29F5, 0x29F6, 0x29F8, 0x29FA, 0x29FC, 0x29FD, 0x29FE, 0x2A2B, 
  0x2A2F, 0x2A34, 0x2A36, 0x2A3C, 0x2A3E, 0x2A64, 0x2A66, 0x2A79, 0x2A7B, 0x2A7D, 
  0x2A85, 0x2A8B, 0x2A8D, 0x2A91, 0x2A9D, 0x2AA1, 0x2AA3, 0x2AA6, 0x2AAE, 0x2AAF, 
  0x2AB1, 0x2AB3, 0x2AB5, 0x2ABB, 0x2AC7, 0x2ACD, 0x2AD7, 0x2ADE, 0x2ADF, 0x2AE3, 
  0x2AE6, 0x2AEC, 0x2AEE, 0x2AF7, 0x2AFB, 0x2B74, 0x2B76, 0x2B96, 0x2B98, 0x2BBA, 
  0x2BBD, 0x2BC9, 0x2BCA, 0x2BD3, 0x2BEC, 0x2BF0, 0x2CE5, 0x2CEB, 0x2CEF, 0x2CF2, 
  0x2CF9, 0x2D00, 0x2D7F, 0x2D80, 0x2DE0, 0x2E00, 0x2E02, 0x2E06, 0x2E09, 0x2E0B, 
  0x2E0C, 0x2E0E, 0x2E1C, 0x2E1E, 0x2E20, 0x2E22, 0x2E23, 0x2E24, 0x2E25, 0x2E26, 
  0x2E27, 0x2E28, 0x2E29, 0x2E2A, 0x2E4A, 0x2E80, 0x2E9A, 0x2E9B, 0x2EF4, 0x2F00, 
  0x2FD6, 0x2FF0, 0x2FFC, 0x3000, 0x3001, 0x3005, 0x3008, 0x3009, 0x300A, 0x300B, 
  0x300C, 0x300D, 0x300E, 0x300F, 0x3010, 0x3011, 0x3012, 0x3014, 0x3015, 0x3016, 
  0x3017, 0x3018, 0x3019, 0x301A, 0x301B, 0x301C, 0x3021, 0x302A, 0x302E, 0x3030, 
  0x3031, 0x3036, 0x3038, 0x303D, 0x3040, 0x3099, 0x309B, 0x309D, 0x30A0, 0x30A1, 
  0x30FB, 0x30FC, 0x31C0, 0x31E4, 0x321D, 0x321F, 0x3250, 0x3260, 0x327C, 0x327F, 
  0x32B1, 0x32C0, 0x32CC, 0x32D0, 0x3377, 0x337B, 0x33DE, 0x33E0, 0x33FF, 0x3400, 
  0x4DC0, 0x4E00, 0xA490, 0xA4C7, 0xA60D, 0xA610, 0xA66F, 0xA673, 0xA674, 0xA67E, 
  0xA680, 0xA69E, 0xA6A0, 0xA6F0, 0xA6F2, 0xA700, 0xA722, 0xA788, 0xA789, 0xA802, 
  0xA803, 0xA806, 0xA807, 0xA80B, 0xA80C, 0xA825, 0xA827, 0xA828, 0xA82C, 0xA838, 
  0xA83A, 0xA874, 0xA878, 0xA8C4, 0xA8C6, 0xA8E0, 0xA8F2, 0xA926, 0xA92E, 0xA947, 
  0xA952, 0xA980, 0xA983, 0xA9B3, 0xA9B4, 0xA9B6, 0xA9BA, 0xA9BC, 0xA9BD, 0xA9E5, 
  0xA9E6, 0xAA29, 0xAA2F, 0xAA31, 0xAA33, 0xAA35, 0xAA37, 0xAA43, 0xAA44, 0xAA4C, 
  0xAA4D, 0xAA7C, 0xAA7D, 0xAAB0, 0xAAB1, 0xAAB2, 0xAAB5, 0xAAB7, 0xAAB9, 0xAABE, 
  0xAAC0, 0xAAC1, 0xAAC2, 0xAAEC, 0xAAEE, 0xAAF6, 0xAAF7, 0xABE5, 0xABE6, 0xABE8, 
  0xABE9, 0xABED, 0xABEE, 0xFB1D, 0xFB1E, 0xFB1F, 0xFB29, 0xFB2A, 0xFB50, 0xFD3E, 
  0xFD40, 0xFDD0, 0xFDF0, 0xFDFD, 0xFDFE, 0xFE00, 0xFE10, 0xFE1A, 0xFE20, 0xFE30, 
  0xFE50, 0xFE51, 0xFE52, 0xFE53, 0xFE54, 0xFE55, 0xFE56, 0xFE59, 0xFE5A, 0xFE5B, 
  0xFE5C, 0xFE5D, 0xFE5E, 0xFE5F, 0xFE60, 0xFE62, 0xFE64, 0xFE66, 0xFE67, 0xFE68, 
  0xFE69, 0xFE6B, 0xFE6C, 0xFE70, 0xFEFF, 0xFF00, 0xFF01, 0xFF03, 0xFF06, 0xFF08, 
  0xFF09, 0xFF0A, 0xFF0B, 0xFF0C, 0xFF0D, 0xFF0E, 0xFF10, 0xFF1A, 0xFF1B, 0xFF1C, 
  0xFF1D, 0xFF1E, 0xFF1F, 0xFF21, 0xFF3B, 0xFF3C, 0xFF3D, 0xFF3E, 0xFF41, 0xFF5B, 
  0xFF5C, 0xFF5D, 0xFF5E, 0xFF5F, 0xFF60, 0xFF61, 0xFF62, 0xFF63, 0xFF64, 0xFF66, 
  0xFFE0, 0xFFE2, 0xFFE5, 0xFFE7, 0xFFE8, 0xFFEF, 0xFFF9, 0xFFFC, 0xFFFE, 0xFFFF 
};


/* The following constant data array contains the values BidiClass+BracketPair
   for Unicode character ranges from the corresponding entry in the above array
   SortedCodes[]. The values are composed from BIDI_TYPE_XXX, BRACKET_TYPE_XXX, 
   MIRROR_GLYPH, JOIN_TYPE_XXX. */
static const unsigned char UnicodeData[] =
{
  0x0A, 0x0C, 0x0B, 0x0C, 0x0D, 0x0B, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x06, 0x0E, 
  0x2E, 0x4E, 0x0E, 0x05, 0x08, 0x05, 0x08, 0x04, 0x08, 0x0E, 0x6E, 0x0E, 0x6E, 
  0x0E, 0x01, 0x2E, 0x0E, 0x4E, 0x0E, 0x01, 0x2E, 0x0E, 0x4E, 0x0E, 0x0A, 0x0B, 
  0x0A, 0x08, 0x0E, 0x06, 0x0E, 0x01, 0x6E, 0x0E, 0x8A, 0x0E, 0x06, 0x04, 0x0E, 
  0x01, 0x0E, 0x04, 0x01, 0x6E, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x06, 0x02, 
  0x89, 0x02, 0x89, 0x02, 0x89, 0x02, 0x89, 0x02, 0x89, 0x02, 0x07, 0x0E, 0x03, 
  0x06, 0x03, 0x08, 0x03, 0x0E, 0x89, 0x03, 0x83, 0x03, 0x1A, 0x03, 0x18, 0x1A, 
  0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x1B, 0x1A, 0x18, 0x1A, 0x89, 0x07, 0x06, 
  0x07, 0x03, 0x1A, 0x89, 0x18, 0x03, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 
  0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x03, 0x18, 0x89, 0x07, 0x0E, 0x89, 0x03, 
  0x89, 0x0E, 0x89, 0x18, 0x04, 0x1A, 0x03, 0x1A, 0x03, 0x83, 0x18, 0x89, 0x1A, 
  0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x89, 0x03, 
  0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x18, 0x1A, 0x03, 
  0x89, 0x03, 0x02, 0x89, 0x02, 0x0E, 0x02, 0x89, 0x02, 0x89, 0x02, 0x89, 0x02, 
  0x89, 0x02, 0x89, 0x02, 0x1A, 0x03, 0x1A, 0x03, 0x18, 0x1A, 0x18, 0x03, 0x02, 
  0x1A, 0x18, 0x03, 0x18, 0x1A, 0x18, 0x1A, 0x03, 0x1A, 0x18, 0x1A, 0x03, 0x89, 
  0x07, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x06, 
  0x01, 0x06, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x06, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x0E, 0x06, 0x0E, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x89, 0x01, 0x89, 0x01, 0x81, 
  0x01, 0x81, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x06, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x2E, 0x4E, 0x2E, 0x4E, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x0D, 0x01, 0x2E, 0x4E, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x06, 0x01, 0x89, 0x01, 0x0E, 
  0x01, 0x0E, 0x89, 0x0A, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0D, 
  0x8A, 0x0A, 0x8A, 0x81, 0x82, 0x0E, 0x0D, 0x0B, 0x8F, 0x91, 0x93, 0x90, 0x92, 
  0x08, 0x06, 0x0E, 0x6E, 0x0E, 0x08, 0x2E, 0x4E, 0x0E, 0x0D, 0x8A, 0x01, 0x14, 
  0x15, 0x16, 0x17, 0x8A, 0x04, 0x01, 0x04, 0x05, 0x0E, 0x2E, 0x4E, 0x01, 0x04, 
  0x05, 0x0E, 0x2E, 0x4E, 0x01, 0x06, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 
  0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x06, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 
  0x6E, 0x0E, 0x05, 0x06, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 
  0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x0E, 0x2E, 0x4E, 0x2E, 0x4E, 0x0E, 0x2E, 0x4E, 0x0E, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x04, 0x01, 0x0E, 0x01, 0x0E, 0x2E, 
  0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 
  0x0E, 0x6E, 0x2E, 0x4E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 
  0x0E, 0x01, 0x0E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 
  0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x2E, 0x4E, 0x2E, 
  0x4E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x2E, 0x4E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 
  0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 
  0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x89, 0x01, 0x89, 
  0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x6E, 0x2E, 0x4E, 0x2E, 
  0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x0E, 0x01, 0x0D, 0x0E, 0x01, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 
  0x2E, 0x4E, 0x0E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x2E, 0x4E, 0x0E, 0x01, 
  0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x89, 0x0E, 0x01, 0x0E, 0x01, 
  0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 
  0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x89, 0x0E, 0x89, 0x0E, 0x01, 0x89, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x0E, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x0E, 0x01, 0x06, 0x01, 0x0E, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 
  0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 
  0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x89, 0x01, 0x02, 
  0x89, 0x02, 0x05, 0x02, 0x03, 0x0E, 0x03, 0x01, 0x03, 0x0E, 0x03, 0x89, 0x0E, 
  0x01, 0x89, 0x0E, 0x08, 0x0E, 0x08, 0x01, 0x0E, 0x08, 0x0E, 0x2E, 0x4E, 0x2E, 
  0x4E, 0x2E, 0x4E, 0x06, 0x0E, 0x05, 0x6E, 0x0E, 0x01, 0x0E, 0x06, 0x0E, 0x01, 
  0x03, 0x8A, 0x01, 0x0E, 0x06, 0x0E, 0x2E, 0x4E, 0x0E, 0x05, 0x08, 0x05, 0x08, 
  0x04, 0x08, 0x0E, 0x6E, 0x0E, 0x6E, 0x0E, 0x01, 0x2E, 0x0E, 0x4E, 0x0E, 0x01, 
  0x2E, 0x0E, 0x4E, 0x0E, 0x2E, 0x4E, 0x0E, 0x2E, 0x4E, 0x0E, 0x01, 0x06, 0x0E, 
  0x06, 0x01, 0x0E, 0x01, 0x8E, 0x0E, 0x01, 0x01 
};


/* The following constant data array contains Unicode character codes in sorted
   order extracted from the Unicode database. Every consecutive two entries in
   that array determine the code of a bracket character and its corresponding
   counterpart (eg. open-close or close-open). */
static const unsigned short MirrorGlyphs[] =
{
  0x0028, 0x0029, 0x0029, 0x0028, 0x003C, 0x003E, 0x003E, 0x003C, 0x005B, 0x005D, 
  0x005D, 0x005B, 0x007B, 0x007D, 0x007D, 0x007B, 0x00AB, 0x00BB, 0x00BB, 0x00AB, 
  0x0F3A, 0x0F3B, 0x0F3B, 0x0F3A, 0x0F3C, 0x0F3D, 0x0F3D, 0x0F3C, 0x169B, 0x169C, 
  0x169C, 0x169B, 0x2039, 0x203A, 0x203A, 0x2039, 0x2045, 0x2046, 0x2046, 0x2045, 
  0x207D, 0x207E, 0x207E, 0x207D, 0x208D, 0x208E, 0x208E, 0x208D, 0x2208, 0x220B, 
  0x2209, 0x220C, 0x220A, 0x220D, 0x220B, 0x2208, 0x220C, 0x2209, 0x220D, 0x220A, 
  0x2215, 0x29F5, 0x223C, 0x223D, 0x223D, 0x223C, 0x2243, 0x22CD, 0x2252, 0x2253, 
  0x2253, 0x2252, 0x2254, 0x2255, 0x2255, 0x2254, 0x2264, 0x2265, 0x2265, 0x2264, 
  0x2266, 0x2267, 0x2267, 0x2266, 0x2268, 0x2269, 0x2269, 0x2268, 0x226A, 0x226B, 
  0x226B, 0x226A, 0x226E, 0x226F, 0x226F, 0x226E, 0x2270, 0x2271, 0x2271, 0x2270, 
  0x2272, 0x2273, 0x2273, 0x2272, 0x2274, 0x2275, 0x2275, 0x2274, 0x2276, 0x2277, 
  0x2277, 0x2276, 0x2278, 0x2279, 0x2279, 0x2278, 0x227A, 0x227B, 0x227B, 0x227A, 
  0x227C, 0x227D, 0x227D, 0x227C, 0x227E, 0x227F, 0x227F, 0x227E, 0x2280, 0x2281, 
  0x2281, 0x2280, 0x2282, 0x2283, 0x2283, 0x2282, 0x2284, 0x2285, 0x2285, 0x2284, 
  0x2286, 0x2287, 0x2287, 0x2286, 0x2288, 0x2289, 0x2289, 0x2288, 0x228A, 0x228B, 
  0x228B, 0x228A, 0x228F, 0x2290, 0x2290, 0x228F, 0x2291, 0x2292, 0x2292, 0x2291, 
  0x2298, 0x29B8, 0x22A2, 0x22A3, 0x22A3, 0x22A2, 0x22A6, 0x2ADE, 0x22A8, 0x2AE4, 
  0x22A9, 0x2AE3, 0x22AB, 0x2AE5, 0x22B0, 0x22B1, 0x22B1, 0x22B0, 0x22B2, 0x22B3, 
  0x22B3, 0x22B2, 0x22B4, 0x22B5, 0x22B5, 0x22B4, 0x22B6, 0x22B7, 0x22B7, 0x22B6, 
  0x22C9, 0x22CA, 0x22CA, 0x22C9, 0x22CB, 0x22CC, 0x22CC, 0x22CB, 0x22CD, 0x2243, 
  0x22D0, 0x22D1, 0x22D1, 0x22D0, 0x22D6, 0x22D7, 0x22D7, 0x22D6, 0x22D8, 0x22D9, 
  0x22D9, 0x22D8, 0x22DA, 0x22DB, 0x22DB, 0x22DA, 0x22DC, 0x22DD, 0x22DD, 0x22DC, 
  0x22DE, 0x22DF, 0x22DF, 0x22DE, 0x22E0, 0x22E1, 0x22E1, 0x22E0, 0x22E2, 0x22E3, 
  0x22E3, 0x22E2, 0x22E4, 0x22E5, 0x22E5, 0x22E4, 0x22E6, 0x22E7, 0x22E7, 0x22E6, 
  0x22E8, 0x22E9, 0x22E9, 0x22E8, 0x22EA, 0x22EB, 0x22EB, 0x22EA, 0x22EC, 0x22ED, 
  0x22ED, 0x22EC, 0x22F0, 0x22F1, 0x22F1, 0x22F0, 0x22F2, 0x22FA, 0x22F3, 0x22FB, 
  0x22F4, 0x22FC, 0x22F6, 0x22FD, 0x22F7, 0x22FE, 0x22FA, 0x22F2, 0x22FB, 0x22F3, 
  0x22FC, 0x22F4, 0x22FD, 0x22F6, 0x22FE, 0x22F7, 0x2308, 0x2309, 0x2309, 0x2308, 
  0x230A, 0x230B, 0x230B, 0x230A, 0x2329, 0x232A, 0x232A, 0x2329, 0x2768, 0x2769, 
  0x2769, 0x2768, 0x276A, 0x276B, 0x276B, 0x276A, 0x276C, 0x276D, 0x276D, 0x276C, 
  0x276E, 0x276F, 0x276F, 0x276E, 0x2770, 0x2771, 0x2771, 0x2770, 0x2772, 0x2773, 
  0x2773, 0x2772, 0x2774, 0x2775, 0x2775, 0x2774, 0x27C3, 0x27C4, 0x27C4, 0x27C3, 
  0x27C5, 0x27C6, 0x27C6, 0x27C5, 0x27C8, 0x27C9, 0x27C9, 0x27C8, 0x27CB, 0x27CD, 
  0x27CD, 0x27CB, 0x27D5, 0x27D6, 0x27D6, 0x27D5, 0x27DD, 0x27DE, 0x27DE, 0x27DD, 
  0x27E2, 0x27E3, 0x27E3, 0x27E2, 0x27E4, 0x27E5, 0x27E5, 0x27E4, 0x27E6, 0x27E7, 
  0x27E7, 0x27E6, 0x27E8, 0x27E9, 0x27E9, 0x27E8, 0x27EA, 0x27EB, 0x27EB, 0x27EA, 
  0x27EC, 0x27ED, 0x27ED, 0x27EC, 0x27EE, 0x27EF, 0x27EF, 0x27EE, 0x2983, 0x2984, 
  0x2984, 0x2983, 0x2985, 0x2986, 0x2986, 0x2985, 0x2987, 0x2988, 0x2988, 0x2987, 
  0x2989, 0x298A, 0x298A, 0x2989, 0x298B, 0x298C, 0x298C, 0x298B, 0x298D, 0x2990, 
  0x298E, 0x298F, 0x298F, 0x298E, 0x2990, 0x298D, 0x2991, 0x2992, 0x2992, 0x2991, 
  0x2993, 0x2994, 0x2994, 0x2993, 0x2995, 0x2996, 0x2996, 0x2995, 0x2997, 0x2998, 
  0x2998, 0x2997, 0x29B8, 0x2298, 0x29C0, 0x29C1, 0x29C1, 0x29C0, 0x29C4, 0x29C5, 
  0x29C5, 0x29C4, 0x29CF, 0x29D0, 0x29D0, 0x29CF, 0x29D1, 0x29D2, 0x29D2, 0x29D1, 
  0x29D4, 0x29D5, 0x29D5, 0x29D4, 0x29D8, 0x29D9, 0x29D9, 0x29D8, 0x29DA, 0x29DB, 
  0x29DB, 0x29DA, 0x29F5, 0x2215, 0x29F8, 0x29F9, 0x29F9, 0x29F8, 0x29FC, 0x29FD, 
  0x29FD, 0x29FC, 0x2A2B, 0x2A2C, 0x2A2C, 0x2A2B, 0x2A2D, 0x2A2E, 0x2A2E, 0x2A2D, 
  0x2A34, 0x2A35, 0x2A35, 0x2A34, 0x2A3C, 0x2A3D, 0x2A3D, 0x2A3C, 0x2A64, 0x2A65, 
  0x2A65, 0x2A64, 0x2A79, 0x2A7A, 0x2A7A, 0x2A79, 0x2A7D, 0x2A7E, 0x2A7E, 0x2A7D, 
  0x2A7F, 0x2A80, 0x2A80, 0x2A7F, 0x2A81, 0x2A82, 0x2A82, 0x2A81, 0x2A83, 0x2A84, 
  0x2A84, 0x2A83, 0x2A8B, 0x2A8C, 0x2A8C, 0x2A8B, 0x2A91, 0x2A92, 0x2A92, 0x2A91, 
  0x2A93, 0x2A94, 0x2A94, 0x2A93, 0x2A95, 0x2A96, 0x2A96, 0x2A95, 0x2A97, 0x2A98, 
  0x2A98, 0x2A97, 0x2A99, 0x2A9A, 0x2A9A, 0x2A99, 0x2A9B, 0x2A9C, 0x2A9C, 0x2A9B, 
  0x2AA1, 0x2AA2, 0x2AA2, 0x2AA1, 0x2AA6, 0x2AA7, 0x2AA7, 0x2AA6, 0x2AA8, 0x2AA9, 
  0x2AA9, 0x2AA8, 0x2AAA, 0x2AAB, 0x2AAB, 0x2AAA, 0x2AAC, 0x2AAD, 0x2AAD, 0x2AAC, 
  0x2AAF, 0x2AB0, 0x2AB0, 0x2AAF, 0x2AB3, 0x2AB4, 0x2AB4, 0x2AB3, 0x2ABB, 0x2ABC, 
  0x2ABC, 0x2ABB, 0x2ABD, 0x2ABE, 0x2ABE, 0x2ABD, 0x2ABF, 0x2AC0, 0x2AC0, 0x2ABF, 
  0x2AC1, 0x2AC2, 0x2AC2, 0x2AC1, 0x2AC3, 0x2AC4, 0x2AC4, 0x2AC3, 0x2AC5, 0x2AC6, 
  0x2AC6, 0x2AC5, 0x2ACD, 0x2ACE, 0x2ACE, 0x2ACD, 0x2ACF, 0x2AD0, 0x2AD0, 0x2ACF, 
  0x2AD1, 0x2AD2, 0x2AD2, 0x2AD1, 0x2AD3, 0x2AD4, 0x2AD4, 0x2AD3, 0x2AD5, 0x2AD6, 
  0x2AD6, 0x2AD5, 0x2ADE, 0x22A6, 0x2AE3, 0x22A9, 0x2AE4, 0x22A8, 0x2AE5, 0x22AB, 
  0x2AEC, 0x2AED, 0x2AED, 0x2AEC, 0x2AF7, 0x2AF8, 0x2AF8, 0x2AF7, 0x2AF9, 0x2AFA, 
  0x2AFA, 0x2AF9, 0x2E02, 0x2E03, 0x2E03, 0x2E02, 0x2E04, 0x2E05, 0x2E05, 0x2E04, 
  0x2E09, 0x2E0A, 0x2E0A, 0x2E09, 0x2E0C, 0x2E0D, 0x2E0D, 0x2E0C, 0x2E1C, 0x2E1D, 
  0x2E1D, 0x2E1C, 0x2E20, 0x2E21, 0x2E21, 0x2E20, 0x2E22, 0x2E23, 0x2E23, 0x2E22, 
  0x2E24, 0x2E25, 0x2E25, 0x2E24, 0x2E26, 0x2E27, 0x2E27, 0x2E26, 0x2E28, 0x2E29, 
  0x2E29, 0x2E28, 0x3008, 0x3009, 0x3009, 0x3008, 0x300A, 0x300B, 0x300B, 0x300A, 
  0x300C, 0x300D, 0x300D, 0x300C, 0x300E, 0x300F, 0x300F, 0x300E, 0x3010, 0x3011, 
  0x3011, 0x3010, 0x3014, 0x3015, 0x3015, 0x3014, 0x3016, 0x3017, 0x3017, 0x3016, 
  0x3018, 0x3019, 0x3019, 0x3018, 0x301A, 0x301B, 0x301B, 0x301A, 0xFE59, 0xFE5A, 
  0xFE5A, 0xFE59, 0xFE5B, 0xFE5C, 0xFE5C, 0xFE5B, 0xFE5D, 0xFE5E, 0xFE5E, 0xFE5D, 
  0xFE64, 0xFE65, 0xFE65, 0xFE64, 0xFF08, 0xFF09, 0xFF09, 0xFF08, 0xFF1C, 0xFF1E, 
  0xFF1E, 0xFF1C, 0xFF3B, 0xFF3D, 0xFF3D, 0xFF3B, 0xFF5B, 0xFF5D, 0xFF5D, 0xFF5B, 
  0xFF5F, 0xFF60, 0xFF60, 0xFF5F, 0xFF62, 0xFF63, 0xFF63, 0xFF62 
}; 


/* The following constant data array contains Unicode character codes in sorted
   order extracted from the Unicode database. Every consecutive four entries in
   that array determine the code of an Arabic letter with its four presentation
   codes for the initial, medial and final shaping variant. */
static const unsigned short ShapingGlyphs[] =
{
  0x0621, 0x0000, 0x0000, 0x0000,   0x0622, 0x0000, 0x0000, 0xFE82, 
  0x0623, 0x0000, 0x0000, 0xFE84,   0x0624, 0x0000, 0x0000, 0xFE86, 
  0x0625, 0x0000, 0x0000, 0xFE88,   0x0626, 0xFE8B, 0xFE8C, 0xFE8A, 
  0x0627, 0x0000, 0x0000, 0xFE8E,   0x0628, 0xFE91, 0xFE92, 0xFE90, 
  0x0629, 0x0000, 0x0000, 0xFE94,   0x062A, 0xFE97, 0xFE98, 0xFE96, 
  0x062B, 0xFE9B, 0xFE9C, 0xFE9A,   0x062C, 0xFE9F, 0xFEA0, 0xFE9E, 
  0x062D, 0xFEA3, 0xFEA4, 0xFEA2,   0x062E, 0xFEA7, 0xFEA8, 0xFEA6, 
  0x062F, 0x0000, 0x0000, 0xFEAA,   0x0630, 0x0000, 0x0000, 0xFEAC, 
  0x0631, 0x0000, 0x0000, 0xFEAE,   0x0632, 0x0000, 0x0000, 0xFEB0, 
  0x0633, 0xFEB3, 0xFEB4, 0xFEB2,   0x0634, 0xFEB7, 0xFEB8, 0xFEB6, 
  0x0635, 0xFEBB, 0xFEBC, 0xFEBA,   0x0636, 0xFEBF, 0xFEC0, 0xFEBE, 
  0x0637, 0xFEC3, 0xFEC4, 0xFEC2,   0x0638, 0xFEC7, 0xFEC8, 0xFEC6, 
  0x0639, 0xFECB, 0xFECC, 0xFECA,   0x063A, 0xFECF, 0xFED0, 0xFECE, 
  0x0641, 0xFED3, 0xFED4, 0xFED2,   0x0642, 0xFED7, 0xFED8, 0xFED6, 
  0x0643, 0xFEDB, 0xFEDC, 0xFEDA,   0x0644, 0xFEDF, 0xFEE0, 0xFEDE, 
  0x0645, 0xFEE3, 0xFEE4, 0xFEE2,   0x0646, 0xFEE7, 0xFEE8, 0xFEE6, 
  0x0647, 0xFEEB, 0xFEEC, 0xFEEA,   0x0648, 0x0000, 0x0000, 0xFEEE, 
  0x0649, 0xFBE8, 0xFBE9, 0xFEF0,   0x064A, 0xFEF3, 0xFEF4, 0xFEF2, 
  0x0671, 0x0000, 0x0000, 0xFB51,   0x0677, 0x0000, 0x0000, 0x0000, 
  0x0679, 0xFB68, 0xFB69, 0xFB67,   0x067A, 0xFB60, 0xFB61, 0xFB5F, 
  0x067B, 0xFB54, 0xFB55, 0xFB53,   0x067E, 0xFB58, 0xFB59, 0xFB57, 
  0x067F, 0xFB64, 0xFB65, 0xFB63,   0x0680, 0xFB5C, 0xFB5D, 0xFB5B, 
  0x0683, 0xFB78, 0xFB79, 0xFB77,   0x0684, 0xFB74, 0xFB75, 0xFB73, 
  0x0686, 0xFB7C, 0xFB7D, 0xFB7B,   0x0687, 0xFB80, 0xFB81, 0xFB7F, 
  0x0688, 0x0000, 0x0000, 0xFB89,   0x068C, 0x0000, 0x0000, 0xFB85, 
  0x068D, 0x0000, 0x0000, 0xFB83,   0x068E, 0x0000, 0x0000, 0xFB87, 
  0x0691, 0x0000, 0x0000, 0xFB8D,   0x0698, 0x0000, 0x0000, 0xFB8B, 
  0x06A4, 0xFB6C, 0xFB6D, 0xFB6B,   0x06A6, 0xFB70, 0xFB71, 0xFB6F, 
  0x06A9, 0xFB90, 0xFB91, 0xFB8F,   0x06AD, 0xFBD5, 0xFBD6, 0xFBD4, 
  0x06AF, 0xFB94, 0xFB95, 0xFB93,   0x06B1, 0xFB9C, 0xFB9D, 0xFB9B, 
  0x06B3, 0xFB98, 0xFB99, 0xFB97,   0x06BA, 0x0000, 0x0000, 0xFB9F, 
  0x06BB, 0xFBA2, 0xFBA3, 0xFBA1,   0x06BE, 0xFBAC, 0xFBAD, 0xFBAB, 
  0x06C0, 0x0000, 0x0000, 0xFBA5,   0x06C1, 0xFBA8, 0xFBA9, 0xFBA7, 
  0x06C5, 0x0000, 0x0000, 0xFBE1,   0x06C6, 0x0000, 0x0000, 0xFBDA, 
  0x06C7, 0x0000, 0x0000, 0xFBD8,   0x06C8, 0x0000, 0x0000, 0xFBDC, 
  0x06C9, 0x0000, 0x0000, 0xFBE3,   0x06CB, 0x0000, 0x0000, 0xFBDF, 
  0x06CC, 0xFBFE, 0xFBFF, 0xFBFD,   0x06D0, 0xFBE6, 0xFBE7, 0xFBE5, 
  0x06D2, 0x0000, 0x0000, 0xFBAF,   0x06D3, 0x0000, 0x0000, 0xFBB1, 
  0xFEF5, 0x0000, 0x0000, 0xFEF6,   0xFEF7, 0x0000, 0x0000, 0xFEF8, 
  0xFEF9, 0x0000, 0x0000, 0xFEFA,   0xFEFB, 0x0000, 0x0000, 0xFEFC 
};


/* The following internal function determines for the given Unicode character
   aCharCode its BidiClass, BracketType, JoiningType properties, etc. */
static unsigned char GetUnicodeInfo( unsigned short aCharCode )
{
  int inx  = 0;
  int min  = 0;
  int max  = ( sizeof( SortedCodes ) / sizeof( *SortedCodes )) - 1;
  
  /* Repeat until the range including the requested code is found, or the end
     of the array is reached */
  while( max >= min )
  {
    inx = ( max + min ) / 2;

    if      (  aCharCode == SortedCodes[ inx ])      break;
    else if (  aCharCode <  SortedCodes[ inx ])      max = inx - 1;
    else if (( aCharCode >= SortedCodes[ inx + 1 ])) min = inx + 1;
    else                                             break;
  }

  /* Found */
  return UnicodeData[ inx ];
} 


/* The following internal function determines for the given Unicode character
   aCharCode its counterpart mirror glyph. If aCharCode has not a mirrored
   counterpart, function returns 0. */
static unsigned short GetMirrorGlyph( unsigned short aCharCode )
{
  int inx  = 0;
  int min  = 0;
  int max  = (( sizeof( MirrorGlyphs ) / sizeof( *MirrorGlyphs )) / 2 ) - 1;
  int comp = -1;
  
  /* Repeat until the requested code is found, or the end of the array is
     reached */
  while( max >= min )
  {
    inx  = ( max + min ) / 2;
    comp = aCharCode - MirrorGlyphs[ inx * 2 ]; 

    if ( !comp     ) return MirrorGlyphs[ inx * 2 + 1 ]; 
    if (  comp < 0 ) max = inx - 1;
    else             min = inx + 1;
  }

  /* Not found */
  return 0;
} 


/* The following internal function determines for the given Unicode character
   aCharCode whether it has different shaping presentations. If successful, the
   funtion returns a pointer to a const array with 3 glyph codes corresponding
   to the 3 possible variants [ <initial>, <medial>, <final> ]. The array may
   contain 0 values for glyph variants which are not available. If aCharCode 
   has not the specified variant, function returns 0. */
static const unsigned short* GetShapingGlyphs( unsigned short aCharCode )
{
  int inx  = 0;
  int min  = 0;
  int max  = (( sizeof( ShapingGlyphs ) / sizeof( *ShapingGlyphs )) / 4 ) - 1;
  int comp = -1;
  
  /* Repeat until the requested code is found, or the end of the array is 
     reached */
  while( max >= min )
  {
    inx  = ( max + min ) / 2;
    comp = aCharCode - ShapingGlyphs[ inx * 4 ]; 

    if ( !comp     ) return &ShapingGlyphs[ inx * 4 + 1 ]; 
    if (  comp < 0 ) max = inx - 1;
    else             min = inx + 1;
  }

  /* Not found */
  return 0;
} 


/* The following function searches backwards within the array aData for a RLI,
   LRI or FSI matching the PDI found at the actual aData[0] position. The 
   function returns a pointer to the found RLI, LRI or FSI or NULL if it has
   not been found. The search operation stops by reaching aStart. */
static XBidiData* FindIsolateInitiator( XBidiData* aData, XBidiData* aStart )
{
  int            isolates = 0;
  unsigned short bidiType = aData->Type; 
  
  /* There is no PDI at the actual position ... */
  if ( bidiType != BIDI_TYPE_PDI )
    return 0;

  /* Scan the bidi types */
  for ( ; aData >= aStart; aData-- )
  {
    bidiType = aData->Type; 

    /* Isolate initiator? */
    if ( bidiType == BIDI_TYPE_PDI )
      isolates--;

    /* The end of the isolate */
    if ( isolates && 
       (( bidiType == BIDI_TYPE_LRI ) || ( bidiType == BIDI_TYPE_RLI ) ||
        ( bidiType == BIDI_TYPE_FSI )))
      if ( ++isolates == 0 )
        return aData;
  }

  /* Not found */
  return 0;
}


/* The following function searches within the array aData for a PDI matching
   the RLI, LRI or FSI found at the actual aData[0] position. The function
   returns a pointer to the found PDI or NULL if no corresponding PDI has been
   found. The search operation stops before reaching aStop. */
static XBidiData* FindIsolateTerminator( XBidiData* aData, XBidiData* aStop )
{
  int            isolates = 0;
  unsigned short bidiType = aData->Type; 

  /* There is no RLI, LRI nor FSI at the actual position ... */
  if (( bidiType != BIDI_TYPE_LRI ) && ( bidiType != BIDI_TYPE_RLI ) &&
      ( bidiType != BIDI_TYPE_FSI ))
    return 0;
  
  /* Scan the bidi types */
  for ( ; aData < aStop; aData++ )
  {
    bidiType = aData->Type; 

    /* Isolate initiator? */
    if (( bidiType == BIDI_TYPE_LRI ) || ( bidiType == BIDI_TYPE_RLI ) ||
        ( bidiType == BIDI_TYPE_FSI ))
      isolates++;
    
    /* The end of the isolate */
    else if ( isolates && ( bidiType == BIDI_TYPE_PDI ))
    {
      if ( isolates == 1 )
        return aData;

      if ( isolates > 1 )
        isolates--;
    }
  }

  /* Not found */
  return 0;
}


/* The following function searches within the array aData for the first strong
   BidiType L, R or AL. Characters enclosed between LRI, RLI, FSI and PDI are
   not taken in account. The function returns a pointer to found character or
   NULL if no character has been found. The search operation stops before 
   reaching aStop. */
static XBidiData* FindFirstStrongChar( XBidiData* aData, XBidiData* aStop )
{
  /* Scan the bidi types */
  for ( ; aData < aStop; aData++ )
  {
    unsigned short bidiType = aData->Type; 

    /* Found a strong character? */
    if (( bidiType == BIDI_TYPE_L ) ||
        ( bidiType == BIDI_TYPE_R ) ||
        ( bidiType == BIDI_TYPE_AL ))
      return aData;

    /* Other non isolate initiator character? */
    if (( bidiType != BIDI_TYPE_LRI ) && ( bidiType != BIDI_TYPE_RLI ) &&
        ( bidiType != BIDI_TYPE_FSI ))
      continue;

    /* Search for the PDI corresponding to the found isolate initiator */
    if (( aData = FindIsolateTerminator( aData, aStop )) == 0 )
      aData = aStop;
  } 

  /* Not found */
  return 0;
}


/* The following function searches within aData for the end of the level-run
   starting at aData[0]. The level-run is a sequence of characters with the 
   same level. The function returns a pointer to the last character belonging
   to the level-run. The search operation stops before reaching aStop.
   (Definition BD7) */
static XBidiData* FindEndOfLevelRun( XBidiData* aData, XBidiData* aStop )
{
  unsigned short level = aData->Level;
  
  if ( aData >= aStop )
    return 0;

  while (( aData < aStop ) && ( aData->Level == level ))
    aData++;

  return aData - 1;
}


/* The following function searches the given isolating run sequence aSeqData
   for nested bracket pairs. Brackets, which have no valid counterpart are 
   marked as invalid by setting their state to MIRROR. The function returns 
   != 0 if there is at least one valid bracket pair in the sequence */
static int PreprocessBrackets( XBidiData** aSeqData, XBidiData** aSeqStop,
  XBidiData* aStart, XChar* aString, XBidiData** aGaps, int aGapCount )
{
  unsigned short codeStack[ BRACKET_STACK_MAX_DEPTH ];
  XBidiData*     dataStack[ BRACKET_STACK_MAX_DEPTH ];
  int            stackInx = -1;
  int            count    = 0;
  int            ofs      = 0;   

  for ( ; aSeqData < aSeqStop; aSeqData++ )
  {
    while (( aGapCount > 0 ) && (( *aSeqData + ofs ) >= aGaps[ -ofs ]))
    {
      ofs++;
      aGapCount--;
    }

    if ((*aSeqData)->Type != BIDI_TYPE_ON )
      continue;
    
    /* An opening paired bracket */
    if ((*aSeqData)->Status == STATUS_BRACKET_OPEN )
    {
      /* Stack capacity enhausted (Definition BD16) */
      if ( stackInx >= ( BRACKET_STACK_MAX_DEPTH - 1 ))
        break;

      /* Store the found opening bracket on the stack */
      dataStack[ ++stackInx ] = *aSeqData; 
      codeStack[   stackInx ] = GetMirrorGlyph( aString[ *aSeqData - aStart + ofs ]);  
    }
    
    /* A closing paired bracket */
    else if ((*aSeqData)->Status == STATUS_BRACKET_CLOSE )
    {
      unsigned short code      = aString[ *aSeqData - aStart + ofs ];
      int            stackInx2 = stackInx;
      
      /* Searches the stack for an open bracket corresponding to the actual
         close bracket. Note the special case of the canonical equivalences 
         for U+2329 and U+232A according to BIDI reference implementation. 
         This aspect is not mentioned in the specification! */
      while (( stackInx2 > 0 ) && ( code != codeStack[ stackInx2 ]) && 
            (( code != 0x232A ) || ( codeStack[ stackInx2 ] != 0x3009 )) && 
            (( code != 0x3009 ) || ( codeStack[ stackInx2 ] != 0x232A )))
        stackInx2--; 
      
      /* Found? Note the special case of the canonical equivalences for U+2329
         and U+232A according to BIDI reference implementation. This aspect is
         not mentioned in the specification! */
      if (( stackInx2 >= 0 ) && (( code == codeStack[ stackInx2 ]) || 
            (( code == 0x232A ) && ( codeStack[ stackInx2 ] == 0x3009 )) || 
            (( code == 0x3009 ) && ( codeStack[ stackInx2 ] == 0x232A ))))
      {
        /* All skipped over opening brackets are marked as not valid. Remove
           the corresponding entries from the stack */
        while ( stackInx > stackInx2 )
          dataStack[ stackInx-- ]->Status = STATUS_MIRROR_GLYPH;

        /* Remove the entry corresponding to the processed closing bracket */
        stackInx--;
        count++;
      }

      /* No corresponding opening bracket found. Just mark this closing bracket
         as not valid  */
      else      
        (*aSeqData)->Status = STATUS_MIRROR_GLYPH;
    }
  }

  /* All not closed opening brackets are marked as not valid. */
  while ( stackInx >= 0 )
    dataStack[ stackInx-- ]->Status = STATUS_MIRROR_GLYPH;

  /* If the stack is full, invalidate all brackets remaining in the sequence */
  for ( ; aSeqData < aSeqStop; aSeqData++ )
    if (((*aSeqData)->Type == BIDI_TYPE_ON ) && 
        (((*aSeqData)->Status == STATUS_BRACKET_OPEN  ) ||
         ((*aSeqData)->Status == STATUS_BRACKET_CLOSE )))
      (*aSeqData)->Status = STATUS_MIRROR_GLYPH;

  return count;
}


/* The following function searches the given isolating run sequence aSeqData
   for a closing bracket corresponding to the opening bracket at the actual
   position aSeqData. */
static XBidiData** FindClosingBracket( XBidiData** aSeqData, XBidiData** aSeqStop )
{
  int count = 1;   

  /* Only if the character at the actual position is a valid opening bracket */
  if (((*aSeqData)->Type != BIDI_TYPE_ON ) || 
      ((*aSeqData)->Status != STATUS_BRACKET_OPEN ))
    return 0;

  for ( aSeqData++; aSeqData < aSeqStop; aSeqData++ )
  {
    /* Count opening brackets */
    if (((*aSeqData)->Type == BIDI_TYPE_ON ) && 
        ((*aSeqData)->Status == STATUS_BRACKET_OPEN ))
      count++;
    
    /* Count closing brackets */
    else if (((*aSeqData)->Type == BIDI_TYPE_ON ) &&
        ((*aSeqData)->Status == STATUS_BRACKET_CLOSE ))
      if ( --count == 0 )
        return aSeqData;
  }

  /* Not found */
  return 0;
}


/* The following function determines the SOS (start of sequence) type for the
   sequence starting with the character aData. */
static unsigned char GetSOSType( XBidiData* aData, XBidiData* aStart,
  unsigned char aParagraphLevel )
{
  unsigned short level     = aData->Level;
  unsigned short prevLevel = aParagraphLevel;

  if ( aData > aStart )
    prevLevel = aData[-1].Level;

  if ( prevLevel > level )
    level = prevLevel;

  if ( level & 1 ) return BIDI_TYPE_R;
  else             return BIDI_TYPE_L;
}


/* The following function determines the EOS (end of sequence) type for the
   sequence ending with the character aData. */
static unsigned char GetEOSType( XBidiData* aData, XBidiData* aStop,
  unsigned char aParagraphLevel )
{
  unsigned short level     = aData->Level;
  unsigned short nextLevel = aParagraphLevel;

  if (( aData < ( aStop - 1 )) && ( aData->Type != BIDI_TYPE_RLI ) &&
       ( aData->Type != BIDI_TYPE_LRI ) && ( aData->Type != BIDI_TYPE_FSI ))
    nextLevel = aData[1].Level;

  if ( nextLevel > level )
    level = nextLevel;

  if ( level & 1 ) return BIDI_TYPE_R;
  else             return BIDI_TYPE_L;
}

#endif /* EW_DONT_USE_BIDI_FUNCTIONS */


/*******************************************************************************
* FUNCTION:
*   EwInitBidi
*
* DESCRIPTION:
*   The function EwInitBidi() is called during the initialization of the
*   Graphics Engine. Its intention is to give the Bidi module a chance to
*   initialize its private variables etc..
*
* ARGUMENTS:
*   aArgs - User argument passed in the EwInitGraphicsEngine().
*
* RETURN VALUE:
*   If successful, returns != 0.
*
*******************************************************************************/
int EwInitBidi( void* aArgs )
{
  EW_UNUSED_ARG( aArgs );

  /* Prepare performance counters */
  EwAddPerfCounter( EwCreateBidi              );
  EwAddPerfCounter( EwFreeBidi                );
  EwAddPerfCounter( EwBidiIsNeeded            );
  EwAddPerfCounter( EwBidiIsRTL               );
  EwAddPerfCounter( EwBidiGetCharLevel        );
  EwAddPerfCounter( EwBidiInit                );
  EwAddPerfCounter( EwBidiProcess             );
  EwAddPerfCounter( EwBidiApplyShaping        );
  EwAddPerfCounter( EwBidiMirrorGlyphs        );
  EwAddPerfCounter( EwBidiCompleteRow         );
  EwAddPerfCounter( EwBidiReorderChars        );
  EwAddPerfCounter( EwBidiReorderDWords       );
  EwAddPerfCounter( EwBidiReorderIndex        );
  EwAddPerfCounter( EwBidiReverseReorderIndex );
  
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwCreateBidi
*
* DESCRIPTION:
*   The function EwCreateBidi() creates a new Bidi-Context able to manage Bidi
*   information for at the least as many characters as specified in parameter
*   aMaxSize.
*
*   The Bidi-information in the context is not initialized. Use the function
*   EwBidiInit() for this purpose. If the returned Bidi-Context is not needed
*   anymore, it has to be released by using the function EwFreeBidi().
*
* ARGUMENTS:
*   aMaxSize - The desired capacity for the Bidi-Context expressed in text
*     characters this context should be able to process maximally.
*
* RETURN VALUE:
*   The function returns a pointer referring to the Bidi-Context.
*
*******************************************************************************/
XHandle EwCreateBidi_( XInt32 aMaxSize )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    int size = (( aMaxSize + 1 ) * sizeof( XBidiData ) + sizeof( XBidiContext ));
    XBidiContext* context = 0;

    /* No content intended to stzore in this context */
    if ( aMaxSize <= 0 )
      return 0;

    /* Reserve memory for the new context and the Bidi-Data entries */
    do
      context = EwAlloc( size );
    while ( !context && EwImmediateReclaimMemory( 17 ));

    /* Failed to allocate memory for the Bidi data */
    if ( !context )
    {
      EwError( 17 );
      return 0;
    }

    /* Complete the initialization of the Bidi context structure */
    context->Data           = (XBidiData*)( context + 1 );
    context->MaxSize        = aMaxSize;
    context->Size           = 0;
    context->IsBidi         = 0;
    context->ParagraphLevel = 0;

    /* Track the RAM usage */
    EwResourcesMemory += size;

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    return (XHandle)context;
  #else
    EW_UNUSED_ARG( aMaxSize );
    return 0;
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwCreateBidi, XHandle, ( XInt32 aMaxSize ), ( aMaxSize ))


/*******************************************************************************
* FUNCTION:
*   EwFreeBidi
*
* DESCRIPTION:
*   The function EwFreeBidi() frees all resources associated with the given
*   Bidi-Context aBidi again.
*
* ARGUMENTS:
*   aBidi - The Bidi-Context to release.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeBidi_( XHandle aBidi )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;

    if ( !context )
      return;

    /* Track the RAM usage */
    EwResourcesMemory -= ( context->MaxSize + 1 ) * sizeof( XBidiData );
    EwResourcesMemory -= sizeof( XBidiContext );

    EwFree( context );
  #else
    EW_UNUSED_ARG( aBidi );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwFreeBidi, ( XHandle aBidi ), ( aBidi ))


/*******************************************************************************
* FUNCTION:
*   EwBidiIsNeeded
*
* DESCRIPTION:
*   The function EwBidiIsNeeded() returns != 0 if further Bidi processing is
*   required. If the original text does not contain any RTL signs nor Bidi
*   control signs, the function returns 0.
*
* ARGUMENTS:
*   aBidi - The Bidi-Context to query the mode.
*
* RETURN VALUE:
*   Returns != 0 if the context describes text which needs Bidi processing.
*
*******************************************************************************/
XBool EwBidiIsNeeded_( XHandle aBidi )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;
    return context && context->IsBidi;
  #else
    EW_UNUSED_ARG( aBidi );
    return 0;
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiIsNeeded, XBool, ( XHandle aBidi ), ( aBidi ))


/*******************************************************************************
* FUNCTION:
*   EwBidiIsRTL
*
* DESCRIPTION:
*   The function EwBidiIsRTL() returns the basic paragraph direction stored in
*   the given Bidi-Context.
*
* ARGUMENTS:
*   aBidi - The Bidi-Context to query the paragraph direction.
*
* RETURN VALUE:
*   Returns != 0 if the context describes text with RTL paragraph direction. 
*   Otherwise return 0.
*
*******************************************************************************/
XBool EwBidiIsRTL_( XHandle aBidi )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;
    return context && ( context->ParagraphLevel == 1 );
  #else
    EW_UNUSED_ARG( aBidi );
    return 0;
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiIsRTL, XBool, ( XHandle aBidi ), ( aBidi ))


/*******************************************************************************
* FUNCTION:
*   EwBidiGetCharLevel
*
* DESCRIPTION:
*   The function EwBidiGetCharLevel() returns the embedding level corresponding
*   to the character with the specified number.
*
* ARGUMENTS:
*   aBidi  - The Bidi-Context to query the embedding level.
*   aIndex - Index identifying the character. The characters are counted 
*     starting with 0.
*
* RETURN VALUE:
*   Returns a number in range 0 .. 127 identifying the embedding level of the 
*   respective character as it is stored in the Bidi context. If the index
*   addresses a non existing character, the paragraph level is returned. If
*   the context is not valid, 0 is returned.
*
*******************************************************************************/
XInt32 EwBidiGetCharLevel_( XHandle aBidi, XInt32 aIndex )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;

    if ( !context )
      return 0;

    if (( aIndex < 0 ) || ( aIndex >= context->Size )) 
      return context->ParagraphLevel;

    return context->Data[ aIndex ].Level;
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aIndex );
    return 0;
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiGetCharLevel, XInt32, ( XHandle aBidi, XInt32 aIndex ),
  ( aBidi, aIndex ))


/*******************************************************************************
* FUNCTION:
*   EwBidiInit
*
* DESCRIPTION:
*   The function EwBidiInit() evaluates the characters in aString and stores in
*   the Bidi-Context aBidi for every evaluated character the corresponding Bidi
*   type. Finally the function determines the paragraph's base direction and
*   stores it also in the context.
*
*   Please note, before invoking this function you should create a new Bidi 
*   context with enough capacity so all string characters can be stored in it.
*   If the passed context is invalid or too small, the function fails.
*
* ARGUMENTS:
*   aBidi          - Bidi-Context where to store the Bidi-Information.
*   aString        - The original string to process.
*   aCount         - The number of characters in aString to process. If this
*     parameter is negative, the function processes all characters found in the
*     string.
*   aBaseDirection - Determines the default paragraph level. If this parameter
*     is 0, the paragraph level is set to 0 (LTR). If this parameter > 0, the 
*     paragraph level is set 1 (RTL). If this parameter is < 0, the paragraph 
*     level is determined automatically depending on the first strong character
*     in the string.
*
* RETURN VALUE:
*   The function returns != 0 if the original string contains at least one RTL
*   or any Bidi-Control sign. If the string does not contain any of those signs
*   the function returns 0. Thereupon no further Bidi processing is required.
*
*******************************************************************************/
XBool EwBidiInit_( XHandle aBidi, XChar* aString, XInt32 aCount, 
  XInt32 aBaseDirection  )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;
    XBidiData*    data    = context? context->Data : 0;
    int           maxSize = context? context->MaxSize : 0;
    int           length  = ( aCount < 0 )? EwGetStringLength( aString ) : aCount;
    int           found   = aBaseDirection > 0;
    int           inx;

    /* Invalid context or it is too small for the given string */
    if ( !data || ( maxSize < length ))
      return 0;

    /* Initialize the array with BidiClass, BracketType, JoiningType information
       for the corresponding characters */
    for ( inx = 0; inx < length; inx++ )
    {
      unsigned char info      = GetUnicodeInfo( aString[inx]);
      unsigned char type      = info & BIDI_TYPE_MASK;
      unsigned char bracket   = info & BRACKET_MASK;
      unsigned char joinTypeT = info & JOIN_TYPE_T;
      unsigned char status    = 0;

      if      ( type    == BIDI_TYPE_AL_R     ) status = STATUS_JOIN_TYPE_R;
      else if ( type    == BIDI_TYPE_AL_L     ) status = STATUS_JOIN_TYPE_L; 
      else if ( type    == BIDI_TYPE_AL_D     ) status = STATUS_JOIN_TYPE_D; 
      else if ( type    == BIDI_TYPE_AL_C     ) status = STATUS_JOIN_TYPE_C;
      else if ( bracket == BRACKET_TYPE_OPEN  ) status = STATUS_BRACKET_OPEN;
      else if ( bracket == BRACKET_TYPE_CLOSE ) status = STATUS_BRACKET_CLOSE;
      else if ( bracket == MIRROR_GLYPH       ) status = STATUS_MIRROR_GLYPH;
      else if ( type    == BIDI_TYPE_NSM      ) status = STATUS_WAS_TYPE_NSM;
      else if ( type    == BIDI_TYPE_WS       ) status = STATUS_WAS_TYPE_WS;
      else if ( type    == BIDI_TYPE_S        ) status = STATUS_WAS_TYPE_SB;
      else if ( type    == BIDI_TYPE_B        ) status = STATUS_WAS_TYPE_SB;
      else if (( type   == BIDI_TYPE_RLI ) || ( type == BIDI_TYPE_LRI ) ||
               ( type   == BIDI_TYPE_FSI ) || ( type == BIDI_TYPE_PDI ))
        status = STATUS_WAS_TYPE_ISOLATE;
      else if ( joinTypeT )                     status = STATUS_JOIN_TYPE_T;

      if (( type >= BIDI_TYPE_AL_R ) && ( type <= BIDI_TYPE_AL_C ))
        type = BIDI_TYPE_AL;    

      /* Did found any character which indicates the necesity to run BIDI? */
      if ( !found && (( type == BIDI_TYPE_AL ) || ( type == BIDI_TYPE_R ) ||
          ( type == BIDI_TYPE_RLE ) || ( type == BIDI_TYPE_RLO ) || 
          ( type == BIDI_TYPE_RLI )))
        found = 1;

      data[inx].Type   = type;
      data[inx].Status = status;
      data[inx].Level  = 0;
    }
  
    /* Terminate the array with an empty data entry. It is used as stop sign. */
    data[inx].Type   = 0;
    data[inx].Status = 0;
    data[inx].Level  = 0;

    /* Complete the initialization of the BIDI context structure */
    context->Size           = length;
    context->IsBidi         = (unsigned char)found;
    context->ParagraphLevel = ( aBaseDirection > 0 )? 1 : 0;

    /* The string does not contain any characters needing the BIDI process */
    if ( !found )
      return 0;

    /* Determine the default paragraph level of the text */
    if (( aBaseDirection < 0 ) && 
        ( data = FindFirstStrongChar( data, data + length )) != 0 )
      context->ParagraphLevel = ( data->Type == BIDI_TYPE_L )? 0 : 1;

    return 1;
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aString );
    EW_UNUSED_ARG( aCount );
    EW_UNUSED_ARG( aBaseDirection );
    return 0;
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiInit, XBool, ( XHandle aBidi, XChar* aString, 
  XInt32 aCount, XInt32 aBaseDirection ), ( aBidi, aString, aCount, aBaseDirection ))


/*******************************************************************************
* FUNCTION:
*   EwBidiProcess
*
* DESCRIPTION:
*   The function EwBidiProcess() performs the main part of the Bidi-Algorithm
*   deriving from the characters in the given string aString and their Bidi-
*   Types provided in aBidi the corresponding direction levels. Finally the
*   estimated levels are stored in aBidi for every character in aString.
*
* ARGUMENTS:
*   aBidi   - The Bidi-Context containing Bidi-Types for the characters in the
*     string aString.
*   aString - Characters to apply the Bidi-Algorithm.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiProcess_( XHandle aBidi, XChar* aString )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiStackEntry stack[ BIDI_STACK_MAX_DEPTH + 2 ];
    XBidiContext*   context            = (XBidiContext*)aBidi;
    int             length             = context? context->Size : 0;
    unsigned char   paragraphLevel     = context? context->ParagraphLevel : 0;
    XBidiData*      data               = context? context->Data : 0;
    XBidiData*      stop               = data + length;
    int             stackInx           = 0;
    int             overflowIsolates   = 0;
    int             overflowEmbeddings = 0;
    int             validIsolates      = 0;
    int             gapCount           = 0;
    XBidiData**     seqData;
    XBidiData*      ptr;
    XBidiData*      ptr2;

    /* No Bidi-Context */
    if ( !data )
      return;

    do
      seqData = EwAlloc(( length + 2 ) * sizeof( void* )); 
    while ( !seqData && EwImmediateReclaimMemory( 18 ));
  
    /* No memory for the temporary isolating run sequence buffer */
    if ( !seqData )
    {
      EwError( 18 );
      return;
    }

    /* Track the temporary used memory */
    EwResourcesMemory += ( length + 2 ) * sizeof( void* );

    /* Also track the max. memory pressure */
    if ( EwResourcesMemory > EwResourcesMemoryPeak )
      EwResourcesMemoryPeak = EwResourcesMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

    /* Initialization (rule X1). stackInx refers to the entry on top of 
       the stack - it is thus >= 0 if stack contains entries and -1 if it is 
       empty. */
    stack[0].Level    = paragraphLevel;
    stack[0].Override = 0;
    stack[0].Isolate  = 0;
  
    /* Process explicit embeddings (rule X1 (X2..X8)) */
    for ( ptr = data; ptr < stop; ptr++ )
    {
      unsigned short bidiType = ptr->Type;
    
      /* Rule X5c */
      if ( bidiType == BIDI_TYPE_FSI )
      {
        XBidiData* pdi = FindIsolateTerminator( ptr, stop );
        XBidiData* tmp = FindFirstStrongChar( ptr + 1, pdi? pdi : stop );
      
        if ( tmp && (( tmp->Type == BIDI_TYPE_AL ) || ( tmp->Type == BIDI_TYPE_R )))
          bidiType = BIDI_TYPE_RLI;
        else
          bidiType = BIDI_TYPE_LRI;
      }

      /* Rule X2..X5 */
      if (( bidiType == BIDI_TYPE_RLE ) || ( bidiType == BIDI_TYPE_LRE ) ||
          ( bidiType == BIDI_TYPE_RLO ) || ( bidiType == BIDI_TYPE_LRO )) 
      {
        int           level = stack[ stackInx ].Level;
        unsigned char type  = 0;
      
        if (( bidiType == BIDI_TYPE_RLE ) || ( bidiType == BIDI_TYPE_RLO ))
          level += ( level & 1 )? 2 : 1;
        else
          level += ( level & 1 )? 1 : 2;
      
        if ( bidiType == BIDI_TYPE_RLO ) type = BIDI_TYPE_R;
        if ( bidiType == BIDI_TYPE_LRO ) type = BIDI_TYPE_L;

        if (( level <= BIDI_STACK_MAX_DEPTH ) &&
              !overflowIsolates && !overflowEmbeddings )
        {
          stackInx++;
          stack[ stackInx ].Level    = (unsigned char)level;
          stack[ stackInx ].Override = type;
          stack[ stackInx ].Isolate  = 0;
        }
      
        if (( level > BIDI_STACK_MAX_DEPTH ) && !overflowIsolates )
          overflowEmbeddings++; 
      }

      /* Rule X5a and X5b (also 5c) */
      else if (( bidiType == BIDI_TYPE_RLI ) || ( bidiType == BIDI_TYPE_LRI ))
      {
        int level = stack[ stackInx ].Level;
      
        ptr->Level = (unsigned short)level;
      
        if ( stack[ stackInx ].Override )
          ptr->Type = stack[ stackInx ].Override; 

        if ( bidiType == BIDI_TYPE_RLI ) level += ( level & 1 )? 2 : 1;
        else                             level += ( level & 1 )? 1 : 2;

        if (( level <= BIDI_STACK_MAX_DEPTH ) &&
              !overflowIsolates && !overflowEmbeddings )
        {
          validIsolates++;
          stackInx++;
          stack[ stackInx ].Level    = (unsigned char)level;
          stack[ stackInx ].Override = 0;
          stack[ stackInx ].Isolate  = 1;
        }
      
        else
          overflowIsolates++;
      }
    
      /* Rule X6a */
      else if ( bidiType == BIDI_TYPE_PDI )
      {
        if ( overflowIsolates )
          overflowIsolates--;
      
        else if ( validIsolates )
        {
          overflowEmbeddings = 0;
        
          while ( !stack[ stackInx ].Isolate )
            stackInx--;

          stackInx--;
          validIsolates--;
        }

        ptr->Level = stack[ stackInx ].Level;

        if ( stack[ stackInx ].Override )
          ptr->Type = stack[ stackInx ].Override; 
      }
    
      /* Rule X7 */
      else if ( bidiType == BIDI_TYPE_PDF )
      {
        if ( !overflowIsolates )
        {
          if ( overflowEmbeddings )
            overflowEmbeddings--;
          
          else if (( stackInx && !stack[ stackInx ].Isolate ))
            stackInx--;
        }
      }

      /* Rule X6 */
      else if (( bidiType != BIDI_TYPE_B ) && ( bidiType != BIDI_TYPE_BN )) 
      {
        ptr->Level = stack[ stackInx ].Level;
      
        if ( stack[ stackInx ].Override )
          ptr->Type = stack[ stackInx ].Override; 
      }
    }

    /* Remove signs (rule X9) */
    for ( ptr2 = data, ptr = data; ptr < stop; ptr++, ptr2++ )
    {
      unsigned short bidiType = ptr->Type;

      /* Remove the followjng signs from the text temporarily. To restore them
         later record the position of every affected sign. */
      if (( bidiType == BIDI_TYPE_RLE ) || ( bidiType == BIDI_TYPE_LRE ) ||
          ( bidiType == BIDI_TYPE_RLO ) || ( bidiType == BIDI_TYPE_LRO ) ||
          ( bidiType == BIDI_TYPE_PDF ) || ( bidiType == BIDI_TYPE_BN ))
      {
        seqData[ length + 2 - ++gapCount ] = ptr;
        ptr2--;
      }

      else if ( ptr2 < ptr )
        *ptr2 = *ptr;
    }

    /* The preceding X9 rule has removed signs from the text. */
    stop -= gapCount;
  
    /* Process all level runs belonging to the same isolating run sequence 
       (rules W1 .. W7, N0 .. N2). */
    for ( ptr = data; ptr < stop; )
    {
      XBidiData**    seqPtr = seqData;
      XBidiData**    seqStop;
      XBidiData**    firstNI;
      XBidiData      sos = { 0, 0, 0 };
      XBidiData      eos = { 0, 0, 0 };
      XBidiData*     solr;
      XBidiData*     eolr;
      unsigned short strongType;
    
      /* Does the level-run belong to a previously processed isolating run 
         sequence? Then ignore it. (Definition BD13) */
      if (( ptr->Type == BIDI_TYPE_PDI ) && FindIsolateInitiator( ptr, data ))
      {
        ptr = FindEndOfLevelRun( ptr, stop ) + 1;
        continue;
      }

      /* Begin of a new isolating run sequence */
      sos.Type  = GetSOSType( ptr, data, paragraphLevel );
      *seqPtr++ = &sos;

      eolr = FindEndOfLevelRun( ptr, stop );

      /* Accumulare all characters belonging to the first level-run */
      for ( ; ptr <= eolr; ptr++ )
        *seqPtr++ = ptr;

      /* Search for the next following level-run belonging to the same isolating
         run sequence (Rule X10) */
      while (( solr = FindIsolateTerminator( eolr, stop )) != 0 )
      {
        eolr = FindEndOfLevelRun( solr, stop );
      
        /* Accumulare all characters belonging to this level-run */
        for ( ; solr <= eolr; solr++ )
          *seqPtr++ = solr;
      }

      /* End of the  */
      eos.Type = GetEOSType( eolr, stop, paragraphLevel );
      *seqPtr++ = &eos;

      /* The determines end of the isolating run sequence without the EOS */
      seqStop = seqPtr;

      /* Rule W1 */
      for ( seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Rule W1 */
        if ( cur->Type == BIDI_TYPE_NSM )
        {
          unsigned short prevType = seqPtr[-1]->Type; 

          if (( prevType == BIDI_TYPE_RLI ) || ( prevType == BIDI_TYPE_LRI ) ||
              ( prevType == BIDI_TYPE_FSI ) || ( prevType == BIDI_TYPE_PDI ))
            prevType = BIDI_TYPE_ON;

          cur->Type = prevType;
        }
      }

      /* Rule W2 .. W4 */
      for ( strongType = sos.Type, seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Track the latest strong type */
        if (( cur->Type == BIDI_TYPE_L  ) || ( cur->Type == BIDI_TYPE_R ) ||
            ( cur->Type == BIDI_TYPE_AL ))
          strongType = cur->Type; 

        /* Rule W2 */
        if (( cur->Type == BIDI_TYPE_EN ) && ( strongType == BIDI_TYPE_AL ))
          cur->Type = BIDI_TYPE_AN;

        /* Rule W3 */
        if ( cur->Type == BIDI_TYPE_AL )
          cur->Type = BIDI_TYPE_R;

        /* Rule W4 */
        if (( cur->Type == BIDI_TYPE_EN ) && ( seqPtr[-1]->Type == BIDI_TYPE_ES ) && 
            ( seqPtr[-2]->Type == cur->Type ))   
          seqPtr[-1]->Type = cur->Type;
      
        /* Rule W4 */
        if ((( cur->Type == BIDI_TYPE_AN ) || ( cur->Type == BIDI_TYPE_EN )) &&
             ( seqPtr[-1]->Type == BIDI_TYPE_CS ) && 
             ( seqPtr[-2]->Type == cur->Type ))   
          seqPtr[-1]->Type = cur->Type;
      }

      /* Rule W5 */
      for ( seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Rule W5 */
        if ( cur->Type == BIDI_TYPE_EN )
        {
          XBidiData** seqPtr2 = seqPtr - 1;

          while ((*seqPtr2)->Type == BIDI_TYPE_ET )
            (*seqPtr2--)->Type = BIDI_TYPE_EN;
        }
      
        /* Rule W5 */
        if (( cur->Type == BIDI_TYPE_ET ) && ( seqPtr[-1]->Type == BIDI_TYPE_EN ))
          cur->Type = BIDI_TYPE_EN;
      }

      /* Rule W6 */
      for ( seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Rule W6 */
        if (( cur->Type == BIDI_TYPE_ET ) || ( cur->Type == BIDI_TYPE_ES ) ||
            ( cur->Type == BIDI_TYPE_CS ))
          cur->Type = BIDI_TYPE_ON;
      }

      /* Rule W7 */
      for ( strongType = sos.Type, seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Rule W7 */
        if (( cur->Type == BIDI_TYPE_EN ) && ( strongType == BIDI_TYPE_L ))
          cur->Type = BIDI_TYPE_L;

        /* Track the latest strong type */
        if (( cur->Type == BIDI_TYPE_L  ) || ( cur->Type == BIDI_TYPE_R ) ||
            ( cur->Type == BIDI_TYPE_AL ))
          strongType = cur->Type; 
      }

      /* Rule N0 -> Search and process the bracket pairs */
      if ( PreprocessBrackets( seqPtr = seqData + 1, seqStop - 1, data, aString,
                               seqData + length + 1, gapCount ))
      {
        unsigned short embedType  = ((*seqPtr)->Level & 1 )? BIDI_TYPE_R : BIDI_TYPE_L;
        unsigned short strongType = sos.Type;

        /* Evaluate bracktet pair individually */
        for ( ; seqPtr < seqStop; seqPtr++ )
        {
          XBidiData**    close   = FindClosingBracket( seqPtr, seqStop );
          XBidiData**    seqPtr2 = seqPtr + 1;
          unsigned short type    = (*seqPtr)->Type;
          int            strongL = 0;  
          int            strongR = 0;
          unsigned short newType = 0;  

          if (( type == BIDI_TYPE_EN ) || ( type == BIDI_TYPE_AN ))
            type = BIDI_TYPE_R;
          
          /* Track the preceding string type */
          if (( type == BIDI_TYPE_R ) || ( type == BIDI_TYPE_L ))
            strongType = type; 

          /* No bracket pair starting at the actual position */
          if ( !close )
            continue;

          /* Between the both brackets search for strong signs */
          for ( ; ( seqPtr2 < close ) && ( !strongL || !strongR ); seqPtr2++ )
          {
            unsigned short type = (*seqPtr2)->Type;
          
             if ( type == BIDI_TYPE_L  ) strongL++; 
             if ( type == BIDI_TYPE_R  ) strongR++; 
             if ( type == BIDI_TYPE_AN ) strongR++; 
             if ( type == BIDI_TYPE_EN ) strongR++; 
          }

          /* Rule N0b: Strong type matching the actual embedding direction found */
          if ((( embedType == BIDI_TYPE_L ) && strongL ) ||
              (( embedType == BIDI_TYPE_R ) && strongR ))
          {
            (*seqPtr)->Type = embedType;
            (*close)->Type  = embedType;
            newType         = embedType;
            strongType      = embedType;
          }
        
          /* Rule N0c1: Opposite strong type found with established context */
          else if (( strongL || strongR ) && ( strongType != embedType ))
          {
            (*seqPtr)->Type = strongType;
            (*close)->Type  = strongType;
            newType         = strongType;
          }
        
          /* Rule N0c2 */
          else if ( strongL || strongR )
          {
            (*seqPtr)->Type = embedType;
            (*close)->Type  = embedType;
            newType         = embedType;
            strongType      = embedType;
          }

          /* Adapt the NSMs modified in W1 according to the note in N0 */
          if ( newType )
          {
            for ( seqPtr2 = seqPtr + 1; (*seqPtr2)->Status == STATUS_WAS_TYPE_NSM;
                  seqPtr2++ )
              (*seqPtr2)->Type = newType;

            for ( seqPtr2 = close + 1; (*seqPtr2)->Status == STATUS_WAS_TYPE_NSM;
                  seqPtr2++ )
              (*seqPtr2)->Type = newType;
          }
        }
      }

      /* Rules N1 and N2 */
      for ( firstNI = 0, seqPtr = seqData + 1; seqPtr < seqStop; seqPtr++ )
      {
        XBidiData* cur = *seqPtr;

        /* Start of the NI sequence */
        if ( !firstNI && 
           (( cur->Type == BIDI_TYPE_B   ) || ( cur->Type == BIDI_TYPE_S   ) ||
            ( cur->Type == BIDI_TYPE_WS  ) || ( cur->Type == BIDI_TYPE_ON  ) ||
            ( cur->Type == BIDI_TYPE_FSI ) || ( cur->Type == BIDI_TYPE_LRI ) ||
            ( cur->Type == BIDI_TYPE_RLI ) || ( cur->Type == BIDI_TYPE_PDI )))
          firstNI = seqPtr;

        /* End of the NI sequence */
        if ( firstNI && 
            ( cur->Type != BIDI_TYPE_B   ) && ( cur->Type != BIDI_TYPE_S   ) &&
            ( cur->Type != BIDI_TYPE_WS  ) && ( cur->Type != BIDI_TYPE_ON  ) &&
            ( cur->Type != BIDI_TYPE_FSI ) && ( cur->Type != BIDI_TYPE_LRI ) &&
            ( cur->Type != BIDI_TYPE_RLI ) && ( cur->Type != BIDI_TYPE_PDI ))
        {
          unsigned short typeStart = firstNI[-1]->Type;
          unsigned short typeEnd   = cur->Type;
        
          if (( typeStart == BIDI_TYPE_EN ) || ( typeStart == BIDI_TYPE_AN ))
            typeStart = BIDI_TYPE_R;

          if (( typeEnd == BIDI_TYPE_EN ) || ( typeEnd == BIDI_TYPE_AN ))
            typeEnd = BIDI_TYPE_R;

          if (( typeStart != typeEnd ) || (( typeStart != BIDI_TYPE_L ) &&
              ( typeStart != BIDI_TYPE_R )))
            typeStart = ((*firstNI)->Level & 1 )? BIDI_TYPE_R : BIDI_TYPE_L;

          for ( ; firstNI < seqPtr; firstNI++ )
            (*firstNI)->Type = typeStart;

          firstNI = 0;
        }
      }
    }

    /* Process implicit levels (rule I1 and I2) */
    for ( ptr = data; ptr < stop; ptr++ )
    {
      unsigned short type = ptr->Type;

      /* Rule I2 */
      if ( ptr->Level & 1 )
        if (( type == BIDI_TYPE_L ) || ( type == BIDI_TYPE_EN ) || 
            ( type == BIDI_TYPE_AN ))
          ptr->Level++;
        else
        {
        }

      /* Rule I1 */
      else if ( type == BIDI_TYPE_R )
        ptr->Level++;
      
      /* Rule I1 */
      else if (( type == BIDI_TYPE_EN ) || ( type == BIDI_TYPE_AN ))
        ptr->Level += 2;
    }
  
    /* Fill the gaps of the previously (rule X9) removed signs by an zero width
       glyph. This is necessary to ensure that the mapping between the indices
       in the original text and the processed text does still correspond */
    for ( ptr = stop - 1, ptr2 = ptr + gapCount; gapCount > 0; ptr--, ptr2-- )
    {
      if ( seqData[ length + 2 - gapCount ] == ptr2 )
      {
        unsigned short level = paragraphLevel;
        unsigned short code  = aString[ ptr2 - data ];  

        if (( ptr >= data ) && ( ptr->Level > level ))
          level = ptr->Level;
        
        if (( ptr2 < ( stop - 1 )) && ( ptr2[1].Level > level )) 
          level = ptr2[1].Level; 

        /* The restored gaps were originally BNs or RLE, LRE, RLO, LRO, PDF marks.
           While all RLE..PDF marks have in the Arabic Shaping algorithm a
           transparent joining type characteristic, some BNs have not it. Thus
           we have to examine the code (and so the type) of the character in order
           to decide whether it should become the transparent joining type or
           the 'join causing' (C) type. */ 
        if ( code == 0x200D )
          ptr2->Status = STATUS_JOIN_TYPE_C;
        else if (( code  >  0x0008 ) &&
                 (( code <  0x000E ) || ( code >  0x001B )) &&
                 (( code <  0x007F ) || ( code >  0x0084 )) &&
                 (( code <  0x0086 ) || ( code >  0x009F )) &&
                 (  code != 0x180E ) && ( code != 0x200C ))
          ptr2->Status = STATUS_JOIN_TYPE_T;
        else
          ptr2->Status = 0;
       
        ptr2->Type   = BIDI_TYPE_GAP;
        ptr2->Level  = level;
      
        ptr++;
        gapCount--;
        stop++;
      }

      else if ( ptr2 > ptr )
        *ptr2 = *ptr;
    }

    /* release temporarily used memory */
    EwFree( seqData );
    EwResourcesMemory -= ( length + 2 ) * sizeof( void* );
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aString );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiProcess, ( XHandle aBidi, XChar* aString ), 
  ( aBidi, aString ))


/*******************************************************************************
* FUNCTION:
*   EwBidiApplyShaping
*
* DESCRIPTION:
*   The function EwBidiApplyShaping() performs the part of the Bidi-Algorithm 
*   needed to handle the special case of Arabic glyphs and mandatory ligatures.
*   The function evaluates the characters in the string aString and adapts all
*   found Arabic glyphs accordingly.
*
* ARGUMENTS:
*   aBidi   - The Bidi-Context containing Bidi-Types for the characters in the
*     string aString.
*   aString - Characters to apply the Shaping-Algorithm.
*   aAttrs  - In case of an attributed string points to an array containing
*     the attributes (in particular the font numbers) for the corresponding
*     characters from aString. If aString is not an attributed string, aAttrs
*     is NULL.
*   aFonts  - An array containing the fonts used in aString. The fonts are
*     selected by the font-number value from aAttrs attributes. If aString
*     is not an attributed string, aFonts contains just one entry valid for
*     all characters in the string.
*   aNoOfFonts - Number of fonts in the array aFonts.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiApplyShaping_( XHandle aBidi, XChar* aString, XUInt32* aAttrs,
  XFont** aFonts, XInt32 aNoOfFonts )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext*  context = (XBidiContext*)aBidi;
    int            length  = context? context->Size : 0;
    XBidiData*     data    = context? context->Data : 0;
    XBidiData*     stop    = data + length;
    unsigned short level;
    XBidiData*     ptr;
    XBidiData*     ptr2;

    /* No context */
    if ( !data )
      return;

    /* Arabic Shaping (Unicode core documentation chapter 9.2. rules R1-R7) */
    for ( level = ( ptr2 = ptr = data )->Level; ( ptr <= stop ); ptr++ )
    {
      XBidiData* prev1 = 0;
      XBidiData* cur   = 0;
      XBidiData* next1 = 0;

      /* Does the character belong to the actual level run? */
      if (( level == ptr->Level ) && ( ptr < stop ))
        continue;

      /* Inside the level run detect sequences of signs with joining type 
         different to 'T' (transparent). Investigate every sequence to decide
         whether it should be replaced by a shaping or ligature variant. Also
         take in account the mandatory Arabic ligaturs Lam-Alef and replace them
         by the corresponding ligature glyph */
      for ( ; ( ptr2 < ptr ) || cur || prev1 || next1; ptr2++ )
      {
        XBidiData* left1;
        XBidiData* right1;

        /* Identify non transparent signs and ignore them (Rule R1) */
        if ( ptr2 < ptr )
        {
          unsigned short status = ptr2->Status;

          /* The actual sign has the joining type 'T' (transparent) - ignore it */
          if (( status == STATUS_JOIN_TYPE_T ) || ( status == STATUS_WAS_TYPE_NSM ))
            continue; 

          prev1 = cur;
          cur   = next1;
          next1 = ptr2;
        }
      
        else
        {
          prev1 = cur;
          cur   = next1;
          next1 = 0; 
        }

        /* Depending on the display direction determine which is the character
           on the left and on the right of 'cur' */
        if ( level & 1 ) { left1 = next1; right1 = prev1; }
        else             { left1 = prev1; right1 = next1; }

        /* Right joining character. (Rule R2 and R7) */
        if ( cur && ( cur->Status == STATUS_JOIN_TYPE_R ))
        {
          int                   inx    = (int)( cur - data ); 
          const unsigned short* codes  = GetShapingGlyphs( aString[ inx ]);
          int                   fontNo = aAttrs? GET_ATTR_FONT( aAttrs[inx]) : 0;
          XFont*                font   = aFonts[ MIN( fontNo, aNoOfFonts - 1 )];

          if ( !font )
            font = aFonts[ 0 ];

          /* Is there another right join-causing character on its right? Then
             replace it by its right-joining presentation variant, if available. */
          if ( codes && codes[2] && right1 && 
             (( right1->Status == STATUS_JOIN_TYPE_L ) ||
              ( right1->Status == STATUS_JOIN_TYPE_D ) ||
              ( right1->Status == STATUS_JOIN_TYPE_C )) &&
             ( !font || EwIsGlyphAvailable( font, codes[2])))
            aString[ inx ] = codes[2];
        }

        /* Left joining character. (Rule R3 and R7) */
        else if ( cur && ( cur->Status == STATUS_JOIN_TYPE_L ))
        {
          int                   inx   = (int)( cur - data ); 
          const unsigned short* codes = GetShapingGlyphs( aString[ inx ]);
          int                   fontNo = aAttrs? GET_ATTR_FONT( aAttrs[inx]) : 0;
          XFont*                font   = aFonts[ MIN( fontNo, aNoOfFonts - 1 )];

          if ( !font )
            font = aFonts[ 0 ];

          /* Is there another right join-causing character on its right? Then
             replace it by its right-joining presentation variant, if available. */
          if ( codes && codes[0] && left1 && 
             (( left1->Status == STATUS_JOIN_TYPE_R ) ||
              ( left1->Status == STATUS_JOIN_TYPE_D ) ||
              ( left1->Status == STATUS_JOIN_TYPE_C )) &&
             ( !font || EwIsGlyphAvailable( font, codes[0])))
            aString[ inx ] = codes[0];
        }

        /* Dual joining character. (Rules R4-R7) */
        else if ( cur && ( cur->Status == STATUS_JOIN_TYPE_D ))
        {
          int                   inx   = (int)( cur - data ); 
          const unsigned short* codes = GetShapingGlyphs( aString[ inx ]);
          int                   fontNo = aAttrs? GET_ATTR_FONT( aAttrs[inx]) : 0;
          XFont*                font   = aFonts[ MIN( fontNo, aNoOfFonts - 1 )];

          if ( !font )
            font = aFonts[ 0 ];

          /* Are there another right join-causing and left-join causing characters
             on its right and left? Then replace it by its dual-joining
             presentation variant, if available. (Rule R4) */
          if ( codes && codes[1] && right1 && left1 && 
             (( right1->Status == STATUS_JOIN_TYPE_L ) ||
              ( right1->Status == STATUS_JOIN_TYPE_D ) ||
              ( right1->Status == STATUS_JOIN_TYPE_C )) &&
             (( left1->Status == STATUS_JOIN_TYPE_R ) ||
              ( left1->Status == STATUS_JOIN_TYPE_D ) ||
              ( left1->Status == STATUS_JOIN_TYPE_C )))
            {
              if ( !font || EwIsGlyphAvailable( font, codes[1]))
                aString[ inx ] = codes[1];
            }
        
          /* Is there another right join-causing character on its right? Then
             replace it by its right-joining presentation variant, if available.
             (Rule R5) */
          else if ( codes && codes[2] && right1 && 
             (( right1->Status == STATUS_JOIN_TYPE_L ) ||
              ( right1->Status == STATUS_JOIN_TYPE_D ) ||
              ( right1->Status == STATUS_JOIN_TYPE_C )))
            {
              if ( !font || EwIsGlyphAvailable( font, codes[2]))
                aString[ inx ] = codes[2];
            }
        
          /* Is there another left join-causing character on its left? Then
             replace it by its left-joining presentation variant, if available.
             (Rule R6) */
          else if ( codes && codes[0] && left1 && 
             (( left1->Status == STATUS_JOIN_TYPE_R ) ||
              ( left1->Status == STATUS_JOIN_TYPE_D ) ||
              ( left1->Status == STATUS_JOIN_TYPE_C )))
            {
              if ( !font || EwIsGlyphAvailable( font, codes[0]))
                aString[ inx ] = codes[0];
            }
        }

        /* Determine the codes of the both latest adjacent non transparent
           characters. (Shaping rule L1) */
        if ( cur && prev1 )
        {
          int            inx1    = (int)((( level & 1 )? cur : prev1 ) - data ); 
          int            inx2    = (int)((( level & 1 )? prev1 : cur ) - data ); 
          unsigned short code1   = aString[ inx1 ];
          unsigned short code2   = aString[ inx2 ]; 
          unsigned short ligCode = 0;

          /* Detect whether the actually processed and the preceding glyphs does
             compose a ligature. Case 1: right-to-left display direction.
             (Shaping rules R2-R3). Note: we limit to the mandatory LAM-ALEF 
             Arabic ligatures only. */
          if ( code2 == 0xFEDF )
            switch ( code1 )
            {
              case 0xFE82 : ligCode = 0xFEF5; break;
              case 0xFE84 : ligCode = 0xFEF7; break;
              case 0xFE88 : ligCode = 0xFEF9; break;
              case 0xFE8E : ligCode = 0xFEFB; break;
            }

          if ( code2 == 0xFEE0 )
            switch ( code1 )
            {
              case 0xFE82 : ligCode = 0xFEF6; break;
              case 0xFE84 : ligCode = 0xFEF8; break;
              case 0xFE88 : ligCode = 0xFEFA; break;
              case 0xFE8E : ligCode = 0xFEFC; break;
            }


          /* Test whether the ligature glyph is available in the font. If not, dont
             touch the original characters */
          if ( ligCode )
          {
            int    fontNo = aAttrs? GET_ATTR_FONT( aAttrs[inx1]) : 0;
            XFont* font   = aFonts[ MIN( fontNo, aNoOfFonts - 1 )];

            if ( !font )
              font = aFonts[ 0 ];

            /* Not available? Don't use the ligature */
            if ( font && !EwIsGlyphAvailable( font, ligCode ))
              ligCode = 0;
          }

          /* Store the ligature code in the first of the characters (in display 
             order) while the second character is initialized with <zero-width-
             space> */
          if ( ligCode )
          {
            aString[ inx1 ] = ligCode;
            aString[ inx2 ] = 0xFEFF;
          }
        }
      }

      /* Start of the next level run */
      ptr2  = ptr;
      level = ptr->Level;
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aString );
    EW_UNUSED_ARG( aAttrs );
    EW_UNUSED_ARG( aFonts );
    EW_UNUSED_ARG( aNoOfFonts );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiApplyShaping, ( XHandle aBidi, XChar* aString,
  XUInt32* aAttrs, XFont** aFonts, XInt32 aNoOfFonts ), ( aBidi, aString, 
  aAttrs, aFonts, aNoOfFonts ))


/*******************************************************************************
* FUNCTION:
*   EwBidiMirrorGlyphs
*
* DESCRIPTION:
*   The function EwBidiMirrorGlyphs() performs the part of the Bidi-Algorithm 
*   responsable for the mirroring of glyphs within level-runs with reversed
*   writing direction. For every character within aString which is signed as
*   RTL the function verifies whether there is mirror version of the sign and
*   replaces it with this version.
*
* ARGUMENTS:
*   aBidi   - The Bidi-Context containing Bidi-Types for the characters in the
*     string aString.
*   aString - Characters to mirror.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiMirrorGlyphs_( XHandle aBidi, XChar* aString )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext*  context = (XBidiContext*)aBidi;
    int            length  = context? context->Size : 0;
    XBidiData*     data    = context? context->Data : 0;
    XBidiData*     dataEnd = data + length;
    XBidiData*     dataPtr;

    /* No context */
    if ( !data )
      return;

    /* Rule L4 */
    for ( dataPtr = dataEnd - 1; dataPtr >= data; dataPtr-- )
    {
      unsigned short status = dataPtr->Status; 

      /* Rule L4 */
      if (( dataPtr->Level & 1 ) && (( status == STATUS_BRACKET_OPEN ) ||
          ( status == STATUS_BRACKET_CLOSE ) || ( status == STATUS_MIRROR_GLYPH )))
      {
        unsigned short pair = GetMirrorGlyph( aString[ dataPtr - data ]);

        if ( pair )
          aString[ dataPtr - data ] = pair;  
      }
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aString );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiMirrorGlyphs, ( XHandle aBidi, XChar* aString ), 
  ( aBidi, aString ))


/*******************************************************************************
* FUNCTION:
*   EwBidiCompleteRow
*
* DESCRIPTION:
*   The function EwBidiCompleteRow() performs the part of the Bidi-Algorithm 
*   after text wrap has been applied on a row. Accordingly it runs the final 
*   rules of the Bidi-Algorithm on a single text row only.
*
* ARGUMENTS:
*   aBidi     - The Bidi-Context containing Bidi-Types for the text row.
*   aRowStart - The number of the data entry in aBidi addressing the begin of
*     the row to process.
*   aRowEnd   - The number of the data entry in aBidi addressing the end of
*     the row to process.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiCompleteRow_( XHandle aBidi, XInt32 aRowStart, XInt32 aRowEnd )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext*  context        = (XBidiContext*)aBidi;
    XBidiData*     data           = context? context->Data + aRowStart : 0;
    XBidiData*     dataEnd        = context? context->Data + aRowEnd : 0;
    unsigned char  paragraphLevel = context? context->ParagraphLevel : 0;
    int            applyL1        = 1;
    XBidiData*     dataPtr;

    /* No context */
    if ( !dataEnd )
      return;

    /* Rule L1 */
    for ( dataPtr = dataEnd - 1; dataPtr >= data; dataPtr-- )
    {
      unsigned short status = dataPtr->Status; 

      /* Rule L1 */
      if ( status == STATUS_WAS_TYPE_SB )
      {
        dataPtr->Level = paragraphLevel;
        applyL1 = 1;
      }

      else if ( applyL1 && (( status == STATUS_WAS_TYPE_WS ) ||
              ( status == STATUS_WAS_TYPE_ISOLATE )))
        dataPtr->Level = paragraphLevel;

      /* Ignore all marks removed and restored again at the and of the process
         phase. The algorithm expects these signs to not exist */
      else if ( dataPtr->Type != BIDI_TYPE_GAP )
        applyL1 = 0;
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aRowStart );
    EW_UNUSED_ARG( aRowEnd );
  #endif
}  


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiCompleteRow, ( XHandle aBidi, XInt32 aRowStart, 
  XInt32 aRowEnd ), ( aBidi, aRowStart, aRowEnd ))


/*******************************************************************************
* FUNCTION:
*   EwBidiReorderChars
*
* DESCRIPTION:
*   The function EwBidiReorderChars() performs the reordering of the orignal
*   text entities within a text row according to the previously estimated Bidi
*   run levels. The operation is limited to a single text row.
*
* ARGUMENTS:
*   aBidi     - The Bidi-Context containing Bidi-Types for the text row.
*   aRowStart - The number of the data entry in aBidi addressing the begin of
*     the row to process.
*   aRowEnd   - The number of the data entry in aBidi addressing the end of
*     the row to process.
*   aChars    - Pointer addressing the first character of the affected text 
*     row to reorder its content.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiReorderChars_( XHandle aBidi, XInt32 aRowStart, XInt32 aRowEnd,
  XChar* aChars )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiData*    stack[ BIDI_STACK_MAX_DEPTH + 2 ];
    XBidiContext* context        = (XBidiContext*)aBidi;
    XBidiData*    data           = context? context->Data + aRowStart : 0;
    XBidiData*    dataEnd        = context? context->Data + aRowEnd : 0;
    XBidiData*    start          = data;
    unsigned char level          = 0;
    XChar*        reorderStart   = 0;
    XChar*        reorderEnd     = 0;
    XBidiData*    dataPtr;

    /* Rule L2 */
    for ( dataPtr = data; dataPtr <= dataEnd; dataPtr++ )
    {
      unsigned char curLevel = ( dataPtr < dataEnd )? (unsigned char)dataPtr->Level : 0;

      /* Entering nested level run */
      while (( curLevel > level ) && ( level < ( BIDI_STACK_MAX_DEPTH + 2 )))
      {
        stack[ level++ ] = start;
        start            = dataPtr;
      }
    
      /* Leaving a nested level run. */
      while (( curLevel < level ) && ( level > 0 ))
      {
        XBidiData* start2 = start;

        /* ... and remove the entry from the stack */
        start = stack[ --level ];

        /* Reorder the characters between start und dataPtr */
        if (( dataPtr - start2 ) > 1 )
        {
          XChar* str1 = aChars + ( start2  - data );
          XChar* str2 = aChars + ( dataPtr - data - 1 );
        
          /* Avoid double reordering of the same character sequence */
          if (( str1 == reorderStart ) && ( str2 == reorderEnd ))
          {
            reorderStart = 0;
            reorderEnd   = 0;
            continue;
          }
        
          /* Reorder the remembered sequence now. */
          for ( ; reorderStart < reorderEnd; reorderStart++, reorderEnd-- )
          {
            XChar tmp = *reorderStart;
            *reorderStart = *reorderEnd;
            *reorderEnd = tmp;
          }
        
          /* Remember the new sequence for later reordering */
          reorderStart = str1;
          reorderEnd   = str2;
        }
      }
    }

    /* Any outstanding sequence waiting for reordering? */
    for ( ; reorderStart < reorderEnd; reorderStart++, reorderEnd-- )
    {
      XChar tmp     = *reorderStart;
      *reorderStart = *reorderEnd;
      *reorderEnd   = tmp;
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aRowStart );
    EW_UNUSED_ARG( aRowEnd );
    EW_UNUSED_ARG( aChars );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiReorderChars, ( XHandle aBidi, XInt32 aRowStart, 
  XInt32 aRowEnd, XChar* aChars ), ( aBidi, aRowStart, aRowEnd, aChars ))


/*******************************************************************************
* FUNCTION:
*   EwBidiReorderDWords
*
* DESCRIPTION:
*   The function EwBidiReorderDWords() performs the reordering of the orignal
*   text entities within a text row according to the previously estimated Bidi
*   run levels. The operation is limited to a single text row and unlike the
*   above function it is intended to handle with text entities of 32-bit size.
*
* ARGUMENTS:
*   aBidi     - The Bidi-Context containing Bidi-Types for the text row.
*   aRowStart - The number of the data entry in aBidi addressing the begin of
*     the row to process.
*   aRowEnd   - The number of the data entry in aBidi addressing the end of
*     the row to process.
*   aDWords   - Pointer addressing the first entity of the affected text row
*     to reorder its content.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiReorderDWords_( XHandle aBidi, XInt32 aRowStart, XInt32 aRowEnd,
  XUInt32* aDWords )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiData*    stack[ BIDI_STACK_MAX_DEPTH + 2 ];
    XBidiContext* context        = (XBidiContext*)aBidi;
    XBidiData*    data           = context? context->Data + aRowStart : 0;
    XBidiData*    dataEnd        = context? context->Data + aRowEnd : 0;
    XBidiData*    start          = data;
    unsigned char level          = 0;
    XUInt32*      reorderStart   = 0;
    XUInt32*      reorderEnd     = 0;
    XBidiData*    dataPtr;

    /* Rule L2 */
    for ( dataPtr = data; dataPtr <= dataEnd; dataPtr++ )
    {
      unsigned char curLevel = ( dataPtr < dataEnd )? (unsigned char)dataPtr->Level : 0;

      /* Entering nested level run */
      while (( curLevel > level ) && ( level < ( BIDI_STACK_MAX_DEPTH + 2 )))
      {
        stack[ level++ ] = start;
        start            = dataPtr;
      }
    
      /* Leaving a nested level run. */
      while (( curLevel < level ) && ( level > 0 ))
      {
        XBidiData* start2 = start;

        /* ... and remove the entry from the stack */
        start = stack[ --level ];

        /* Reorder the characters between start und dataPtr */
        if (( dataPtr - start2 ) > 1 )
        {
          XUInt32* str1 = aDWords + ( start2  - data );
          XUInt32* str2 = aDWords + ( dataPtr - data - 1 );
        
          /* Avoid double reordering of the same character sequence */
          if (( str1 == reorderStart ) && ( str2 == reorderEnd ))
          {
            reorderStart = 0;
            reorderEnd   = 0;
            continue;
          }
        
          /* Reorder the remembered sequence now. */
          for ( ; reorderStart < reorderEnd; reorderStart++, reorderEnd-- )
          {
            XUInt32 tmp   = *reorderStart;
            *reorderStart = *reorderEnd;
            *reorderEnd   = tmp;
          }
        
          /* Remember the new sequence for later reordering */
          reorderStart = str1;
          reorderEnd   = str2;
        }
      }
    }

    /* Any outstanding sequence waiting for reordering? */
    for ( ; reorderStart < reorderEnd; reorderStart++, reorderEnd-- )
    {
      XUInt32 tmp   = *reorderStart;
      *reorderStart = *reorderEnd;
      *reorderEnd   = tmp;
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aRowStart );
    EW_UNUSED_ARG( aRowEnd );
    EW_UNUSED_ARG( aDWords );
  #endif
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_VOID_FUNC( EwBidiReorderDWords, ( XHandle aBidi, XInt32 aRowStart, 
  XInt32 aRowEnd, XUInt32* aDWords ), ( aBidi, aRowStart, aRowEnd, aDWords ))


/*******************************************************************************
* FUNCTION:
*   EwBidiReorderIndex
*
* DESCRIPTION:
*   The function EwBidiReorderIndex() estimates the resulting index of a given
*   text entity after applying the Bidi reorder algorithm. In other words, it
*   returns the display position of a character within the original text row
*   after the row has been reordered.
*
* ARGUMENTS:
*   aBidi     - The Bidi-Context containing Bidi-Types for the text row.
*   aRowStart - The number of the data entry in aBidi addressing the begin of
*     the row to process.
*   aRowEnd   - The number of the data entry in aBidi addressing the end of
*     the row to process.
*   aIndex    - The number of the entity within the text row to get its display
*     position.
*
* RETURN VALUE:
*   Returns the display position for the given text entity aIndex.
*
*******************************************************************************/
XInt32 EwBidiReorderIndex_( XHandle aBidi, XInt32 aRowStart, XInt32 aRowEnd,
  XInt32 aIndex )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiData*    stack[ BIDI_STACK_MAX_DEPTH + 2 ];
    XBidiContext* context        = (XBidiContext*)aBidi;
    XBidiData*    data           = context? context->Data + aRowStart : 0;
    XBidiData*    dataEnd        = context? context->Data + aRowEnd : 0;
    XBidiData*    start          = data;
    unsigned char level          = 0;
    XBidiData*    dataPtr;

    /* Rule L2 */
    for ( dataPtr = data; dataPtr <= dataEnd; dataPtr++ )
    {
      unsigned char curLevel = ( dataPtr < dataEnd )? (unsigned char)dataPtr->Level : 0;

      /* Entering nested level run */
      while (( curLevel > level ) && ( level < ( BIDI_STACK_MAX_DEPTH + 2 )))
      {
        stack[ level++ ] = start;
        start            = dataPtr;
      }
    
      /* Leaving a nested level run. */
      while (( curLevel < level ) && ( level > 0 ))
      {
        XBidiData* start2 = start;

        /* ... and remove the entry from the stack */
        start = stack[ --level ];

        /* Reorder the characters between start und dataPtr */
        if (( dataPtr - start2 ) > 1 )
        {
          int inx1 = (int)( start2  - data );
          int inx2 = (int)( dataPtr - data - 1 );

          if (( aIndex >= inx1 ) && ( aIndex <= inx2 ))
            aIndex = inx2 - ( aIndex - inx1 );
        }
      }
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aRowStart );
    EW_UNUSED_ARG( aRowEnd );
    EW_UNUSED_ARG( aIndex );
  #endif

  return aIndex;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiReorderIndex, XInt32, ( XHandle aBidi, XInt32 aRowStart, 
  XInt32 aRowEnd, XInt32 aIndex ), ( aBidi, aRowStart, aRowEnd, aIndex ))


/*******************************************************************************
* FUNCTION:
*   EwBidiReverseReorderIndex
*
* DESCRIPTION:
*   The function EwBidiReverseReorderIndex() estimates the original index of a
*   given text entity before applying the Bidi reorder algorithm. In other 
*   words, it returns the position of the character within the original text
*   row corresponding to the given display position in the reordered row.
*
* ARGUMENTS:
*   aBidi     - The Bidi-Context containing Bidi-Types for the text row.
*   aRowStart - The number of the data entry in aBidi addressing the begin of
*     the row to process.
*   aRowEnd   - The number of the data entry in aBidi addressing the end of
*     the row to process.
*   aIndex    - The number of the entity within the final reordered text row
*     to get its original position.
*
* RETURN VALUE:
*   Returns the original position for the given text entity aIndex.
*
*******************************************************************************/
XInt32 EwBidiReverseReorderIndex_( XHandle aBidi, XInt32 aRowStart, 
  XInt32 aRowEnd, XInt32 aIndex )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    unsigned short starts[ BIDI_STACK_MAX_DEPTH + 2 ];
    unsigned short ends  [ BIDI_STACK_MAX_DEPTH + 2 ];
    XBidiContext*  context = (XBidiContext*)aBidi;
    XBidiData*     data    = context? context->Data + aRowStart : 0;
    int            start   = 0;
    int            end     = aRowEnd - aRowStart - 1;
    int            level   = 0;
    int            inx     = 0;

    /* No Bidi context to apply the algorithm */
    if ( !context )
      return aIndex;

    /* This algorithm is optimized to not to use too much of the CPU stack.
       This, however, limits a single text row to 65535 characters. In such case
       return the index without reordering it */
    if ( end > 65535 )
      return aIndex;

    /* Build up a stack with start/end indices in the text row determining the
       begin and the end of a particular level */
    while ( start < end )
    {
      int curLevelStart = data[ start ].Level;
      int curLevelEnd   = data[ end   ].Level;

      /* Entering an area with nested level runs? */
      if (( curLevelStart > level ) && ( curLevelEnd > level ) && 
          ( level < ( BIDI_STACK_MAX_DEPTH + 2 )))
      {
        starts[ level   ] = (unsigned short)start;
        ends  [ level++ ] = (unsigned short)end;
      }

      /* Within the same level run? */
      if ( curLevelStart <= level ) start++;
      if ( curLevelEnd   <= level ) end--;
    }

    /* Evaluate the estimated areas of level runs and test whether the area
       affects position of the original index */
    while (( inx < level ) && ( aIndex >= starts[inx]) && ( aIndex <= ends[inx]))
    {
      int areaStart = starts[ inx ];
      int areaEnd   = ends[ inx ];
      int inx1      = aIndex;
      int inx2      = aIndex;

      /* Estimate the start/end of the level run */
      while (( inx1 > areaStart ) && ( data[ inx1 - 1 ].Level > inx )) inx1--;
      while (( inx2 < areaEnd   ) && ( data[ inx2 + 1 ].Level > inx )) inx2++;

      /* Convert the position with the level run */
      aIndex = inx2 - ( aIndex - inx1 );
      inx++;
    }
  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aRowStart );
    EW_UNUSED_ARG( aRowEnd );
    EW_UNUSED_ARG( aIndex );
  #endif

  return aIndex;
}


/* Wrapper function if performance counters are enabled */
EW_INSTRUMENT_FUNC( EwBidiReverseReorderIndex, XInt32, ( XHandle aBidi, 
  XInt32 aRowStart, XInt32 aRowEnd, XInt32 aIndex ), ( aBidi, aRowStart, 
  aRowEnd, aIndex ))


/*******************************************************************************
* FUNCTION:
*   EwBidiGetTypesAndLevels
*
* DESCRIPTION:
*   The function EwBidiGetTypesAndLevels() exists for test purpose only. The 
*   function copies all estimates Bidi types and levels from aBidi context to
*   the corresponding entries in the both arrays aTypes and aLevels. Note, the
*   arrays have to be large enough!
*
* ARGUMENTS:
*   aBidi   - The Bidi-Context containing the estimated types and levels.
*   aTypes  - Array to fill with the estimated Bidi types.
*   aLevels - Array to fill with the estimated embedding levels.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwBidiGetTypesAndLevels( XHandle aBidi, unsigned char* aTypes, 
  unsigned char* aLevels )
{
  #ifndef EW_DONT_USE_BIDI_FUNCTIONS
    XBidiContext* context = (XBidiContext*)aBidi;
    XBidiData*    data    = context? context->Data : 0;
    XBidiData*    dataEnd = context? context->Data + context->Size : 0;

    /* Iterate through the estimated types/levels and copy them */
    while ( data < dataEnd )
    {
      *aTypes++  = (unsigned char)data->Type;
      *aLevels++ = (unsigned char)data->Level;
      data++;
    }

  #else
    EW_UNUSED_ARG( aBidi );
    EW_UNUSED_ARG( aTypes );
    EW_UNUSED_ARG( aLevels );
  #endif
}


/* pba */

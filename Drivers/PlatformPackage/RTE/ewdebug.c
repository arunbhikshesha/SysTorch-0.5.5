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
*   The module 'ewdebug.c' implements a set of tool functions usefull for 
*   debugging and error detection.
*
* SUBROUTINES:
*   EwPrint
*   EwTrace
*   EwThrow
*   EwError
*   EwErrorS
*   EwErrorSD
*   EwErrorPD
*   EwErrorDD
*   EwErrorPDS
*   EwAddPerfCounter
*   EwStartPerfCounter
*   EwStopPerfCounter
*   EwResetPerfCounters
*   EwPrintPerfCounters
*
*******************************************************************************/

#include "ewrte.h"
#include "ewextrte.h"


#ifdef EW_PRINT_PERF_COUNTERS
  /* Global list of performance counters and the time of the last performance
     counters reset */
  static XPerfCounter* PerfCounters = 0;
  static unsigned long ResetTime1   = 0;
  static unsigned long ResetTime2   = 0;
  static unsigned long ResetTime3   = 0;

  #undef EwAddPerfCounter
  #undef EwStartPerfCounter
  #undef EwStopPerfCounter
#endif


/*******************************************************************************
* FUNCTION:
*   EwPrint
*
* DESCRIPTION:
*   The function EwPrint() prints formatted output to the debug console. This
*   function works similary to the ANSI 'C' printf() function. However, only
*   following escape sequences are supported:
*
*     %d, %u, %p, %f, %s, %X, %x, %c
*
*   The escape sequences %d, %u, %X, %x can additionally be prefixed by the 
*   double 'll' sign identifying a 64-bit operand:
*
*     %lld, %llu, %llX, %llx
*
*   The escape sequences %d, %u, %X, %x can additionally be prefixed by a sign
*   and number specifying the desired length of the resulting string:
*
*     %[+][0][len]d
*     %[+][0][len]u
*     %[+][0][len]X
*     %[+][0][len]x
*
*   The escape sequence %f can additionally be prefixed by a sign, the number
*   specifying the desired length of the resulting string and a second number
*   for the count of digits following the decomal sign:
*
*     %[+][0][len][.prec]f
*
*   For the escape sequence %s it is possible to specify the desired length to
*   fill with space signs as well as the max. width when to truncate the 
*   string. If the sequence starts with '-' sign, the text is left aligned:
*
*     %[len][.max-len]s
*     %-[len][.max-len]s
*
*   Instead of specifying [len], [prec] and [max-len] as literals within the
*   string, it is possible the use the '*' (asterix) sign as instruction to
*   get the corresponding value from the argzments following aFormat. For
*   example:
*
*    %+0*.*f
*
*   Besides the above escape sequences known from printf(), following two are
*   also supported:
*
*     %S - prints the content of a 16-bit zero terminated string.
*     %C - prints a 16-bit character.
*
* ARGUMENTS:
*   aFormat - Zero-terminated string containing the message and the above
*     explained escape sequences.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/


void EwPrint( const char* aFormat, ... )  //XXXX nur Stub für SysBrenner
{
   // do nothing
}


#if 0 //XXXX #######################################################################################################
void EwPrint( const char* aFormat, ... )
{
  static const char space[] = "                                               ";
  char              buf[ 64 ];
  char*             end = buf + sizeof( buf ) - 1;
  va_list           marker;

  /* Init arguments. */
  va_start( marker, aFormat );

  while ( *aFormat )
  {
    char* ptr = buf;
    int   l   = -1;
    int   p   = -6;
    int   s   = 0;
    int   z   = 0;
    int   ll  = 0;

    /* Scan the format string for regular characters and take them into the
       'buf' buffer */
    while ( *aFormat && ( ptr < end ))
    {
      if      ( *aFormat   != '%' ) { *ptr++ = *aFormat++;        }
      else if ( aFormat[1] == '%' ) { *ptr++ = '%'; aFormat += 2; }
      else break;
    }

    /* Any regular characters to output? */
    if ( ptr > buf )
    {
      *ptr = 0;
      ptr  = buf;
      EwConsoleOutput( buf );
    }

    /* Not the begin of an escape sequence? */
    if (( *aFormat != '%' ) || ( aFormat[1] == '%' ))
      continue;

    /* Consume the percent sign starting the escape sequence */
    aFormat++;
    
    /* Is the '+' sign expected and should fill numbers with zero? */
    if ( *aFormat == '+' ) { aFormat++; s =  1; }
    if ( *aFormat == '-' ) { aFormat++; s = -1; }
    if ( *aFormat == '0' ) { aFormat++; z = -1; }

    /* Read the length information from the argument list */
    if ( *aFormat == '*' )
    {
      aFormat++;
      l = va_arg( marker, int );

      /* Limit the read values */
      if ( l < 0  ) l = 0;
      if ( l > 45 ) l = 45;
    }
   
    /* Parse the length information, if any */
    else if (( *aFormat >= '0' ) && ( *aFormat <= '9' ))
    {
      l = 0;
      while (( *aFormat >= '0' ) && ( *aFormat <= '9' ))
        l = ( l * 10 ) + *aFormat++ - '0';

      /* Limit the read values */
      if ( l < 0  ) l = 0;
      if ( l > 45 ) l = 45;
    }

    /* Precision/max length specified? */
    if ( *aFormat == '.' )
    {
      aFormat++;
      
      /* Read the length information from the argument list */
      if ( *aFormat == '*' )
      {
        aFormat++;
        p = va_arg( marker, int );
      }
      
      /* Parse the precision information if any */
      else
      {
        p = 0;
        while (( *aFormat >= '0' ) && ( *aFormat <= '9' ))
          p = ( p * 10 ) + *aFormat++ - '0';
      }

      /* Limit the read values */
      if ( p < 0  ) p = 0;
      if ( p > 32 ) p = 32;
    }
    
    /* Incomplete escape sequence */
    if ( !*aFormat )
      break;

    /* Optional 'll' 64-Bit operand specifier? */
    while ( *aFormat == 'l' )
    {
      aFormat++;
      ll = 1;
    }

    /* Interpret the found escape sequence */
    switch ( *aFormat++ )
    {
      /* Format a signed integer in decimal notation? */
      case 'd' :
      {
        if ( ll )
        {
          XInt64 tmp = va_arg( marker, XInt64 );
          ptr += EwFormatInt64ToAnsiString( ptr, tmp, l & z, 10, 0, s );
        }
        else
        {
          long tmp = va_arg( marker, long );
          ptr += EwFormatIntToAnsiString( ptr, tmp, l & z, 10, 0, s );
        }
      }
      break;
      
      /* Format an unsigned integer in decimal notation? */
      case 'u' :
      {
        if ( ll )
        {
          XUInt64 tmp = va_arg( marker, XUInt64 );
          ptr += EwFormatUInt64ToAnsiString( ptr, tmp, l & z, 10, 0, s );
        }
        else
        {
          unsigned long tmp = va_arg( marker, unsigned long );
          ptr += EwFormatUIntToAnsiString( ptr, tmp, l & z, 10, 0, s );
        }
      }
      break;
      
      /* Format an unsigned integer in hex notation? */
      case 'X' :
      {
        if ( ll )
        {
          XUInt64 tmp = va_arg( marker, XUInt64 );
          ptr += EwFormatUInt64ToAnsiString( ptr, tmp, l & z, 16, 1, s );
        }
        else
        {
          unsigned long tmp = va_arg( marker, unsigned long );
          ptr += EwFormatUIntToAnsiString( ptr, tmp, l & z, 16, 1, s );
        }
      }
      break;

      case 'x' :
      {
        if ( ll )
        {
          XUInt64 tmp = va_arg( marker, XUInt64 );
          ptr += EwFormatUInt64ToAnsiString( ptr, tmp, l & z, 16, 0, s );
        }
        else
        {
          unsigned long tmp = va_arg( marker, unsigned long );
          ptr += EwFormatUIntToAnsiString( ptr, tmp, l & z, 16, 0, s );
        }
      }
      break;
      
      /* Format floating point number? */
      case 'f' :
      {
        float tmp = (float)va_arg( marker, double );
        int   len = EwFormatFloatToAnsiString( ptr, tmp, l & z, p, s );

        if ( !len )
          EwConsoleOutput( "nan" );

        ptr += len;
      }
      break;
      
      /* Format a pointer? */
      case 'p' :
      {
        void* tmp = va_arg( marker, void* );
        ptr += EwFormatUIntToAnsiString( ptr, (unsigned long)tmp, 
                                         sizeof( void* ) * 2, 16, 1, 0 );
        l = 0;
      }  
      break;
      
      /* Format a character */
      case 'c' :
      {
        char tmp = (char)va_arg( marker, int );

        if ( tmp )
          *ptr++ = tmp;
        else
          EwConsoleOutput( "\\x0000" );

        l = 0;
      }
      break;
      
      /* Format a wide character */
      case 'C' :
      {
        XChar tmp = (XChar)va_arg( marker, int );

        if (( tmp >= 0x20 ) && ( tmp <= 0x7F ))
          *ptr++ = (char)tmp;
        else
        {
          *ptr++ = '\\';
          *ptr++ = 'x';
          ptr   += EwFormatUIntToAnsiString( ptr, tmp, 4, 16, 1, 0 );
        }
        l = 0;
      }
      break;
      
      /* Format a string */
      case 's' :
      {
        const char* tmp = va_arg( marker, const char* );

        /* Print leading space characters if length has been specified? */
        if (( l > 0 ) && ( s >= 0 ))
        {
          const char* tmp1 = tmp;

          /* Scan to the end of the string */
          while ( tmp1 && *tmp1 )
            tmp1++;

          /* Prefix with space characters */
          if ( l > ( tmp1 - tmp ))
            EwConsoleOutput( space + sizeof( space ) - l + ( tmp1 - tmp ) - 1 );
        }
        
        /* Truncate the string */
        if ( p > 0 )
        {
          EwCopy( buf, tmp, p );
          buf[p] = 0;
          tmp    = buf;
        }

        EwConsoleOutput( tmp );

        /* Print final space characters if length has been specified? */
        if (( l > 0 ) && ( s < 0  ))
        {
          const char* tmp1 = tmp;

          /* Scan to the end of the string */
          while ( tmp1 && *tmp1 )
            tmp1++;

          /* Suffix with space characters */
          if ( l > ( tmp1 - tmp ))
            EwConsoleOutput( space + sizeof( space ) - l + ( tmp1 - tmp ) - 1 );
        }
      }
      break;

      /* Format a wide string */
      case 'S' :
      {
        const XChar* tmp = va_arg( marker, const XChar* );

        /* The 16-bit chars have to be converted in 8-bit. If the character
           is out of 8-bit range, display it in the \xXXXX hex notation. */
        while ( tmp && *tmp )
        {
          /* Store the converted characters in the buffer */
          while ( *tmp && ( ptr < ( end - 6 )))
          {
            if (( *tmp >= 0x20 ) && ( *tmp <= 0x7F ))
              *ptr++ = (char)*tmp++;
            else
            {
              *ptr++ = '\\';
              *ptr++ = 'x';
              ptr   += EwFormatUIntToAnsiString( ptr, *tmp++, 4, 16, 1, 0 );
            }
          }

          /* Flush the buffer */
          if ( ptr > buf )
          {
            *ptr = 0;
            ptr  = buf;
            EwConsoleOutput( buf );
          }
        }
      }
      break;
    }

    /* Flush the buffer content */
    if ( ptr > buf )
    {
      /* Print leading space characters if length has been specified */
      if ( l > ( ptr - buf ))
        EwConsoleOutput( space + sizeof( space ) - l + ( ptr - buf ) - 1 );

      *ptr = 0;
      EwConsoleOutput( buf );
    }
  }

  va_end( marker );
}
#endif // 0 // #######################################################################################################


/*******************************************************************************
* FUNCTION:
*   EwTrace
*
* DESCRIPTION:
*   The function EwTrace() works similary to the printf() function. EwTrace()
*   expects a format string and one or more additional arguments. The format
*   string contains escape sequences, one for each argument. These sequences
*   describe how the additional arguments should be interpreted and printed out 
*   on the debug console. 
*
*   Every escape sequence starts with a '%' percent sign followed by a single
*   sign as a command.
*
*   EwTrace() will be called from the automatic generated 'C' code in response
*   to Choras 'trace' statement.
*
* ARGUMENTS:
*   aFormat - Contains a zero-terminated string with escape sequences. The 
*     following escape sequences are supported:
*       %i - print a signed integer value (int8, int16, int32)
*       %u - print an unsigned integer value (uint8, uint16, uint32)
*       %I - print a signed 64-bit integer value (int64)
*       %U - print an unsigned 64-bit integer value (uint64)
*       %b - print a boolean value (bool)
*       %c - print a character (char)
*       %s - print a string value (string)
*       %f - print a floating point value (float)
*       %o - print a color value (color)
*       %l - print a language id
*       %g - print a styles set value (styles)
*       %e - print an enumeration value (enum)
*       %t - print a set value (set)
*       %p - print a point value (point)
*       %r - print a rectangle value (rect)
*       %^ - print a value of a reference to a property (ref to)
*       %* - print a pointer to an object and the class of the object
*       %$ - print a class
*       %h - print a native handle
*       %& - print a value of a slot (responder for signals)
*       %0 - print 'null'
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwTrace( const char* aFormat, ... )
{
  int           comma = 0;
  va_list       marker;

  /* Init arguments. */
  va_start( marker, aFormat );

  EwPrint( "trace: " );

  /* Evaluate format escape sequences aus aFormat */
  while ( *aFormat )
  {
    if ( *aFormat++ == '%' )
    {
      /* Print a comma as delimiter in the trace output */
      if ( comma )
        EwPrint( ", " );
      comma = 1;

      /* Determinate the type of the expression to output on the console */
      switch ( *aFormat++ )
      {
        case 'i' : EwPrint( "%d",   va_arg( marker, XInt32           )); break;
        case 'u' : EwPrint( "%u",   va_arg( marker, XUInt32          )); break;
        case 'I' : EwPrint( "%lld", va_arg( marker, XInt64           )); break;
        case 'U' : EwPrint( "%llu", va_arg( marker, XUInt64          )); break;
        case 'e' : EwPrint( "enum:0x%08X", va_arg( marker, XUInt32 )); break;
        case 't' : EwPrint( "set:0x%08X",  va_arg( marker, XUInt32 )); break;
        case '0' : EwPrint( "null" ); break;

        case 'f' : /* float */
        {
          float v = (float)va_arg( marker, double );
          int   x = 0;

          /* Adjust the number so that it presentes its most significant digits.
             For this purpose estimate the exponent for the e^n notation. */
          if ( !EwIsFloatInf( v ) && !EwIsFloatNaN( v ) && ( v != 0.0f ))
          {
            /* The number is very large. */
            while (( v >= 10000000000.0f ) || ( v <= -10000000000.0f ))
            {
              v /= 10.0f;
              x++;
            }

            /* The number is very small. */
            while ((( v > 0.0f ) && (  v < 0.1f )) ||
                   (( v < 0.0f ) && ( -v < 0.1f )))
            {
              v *= 10.0f;
              x--;
            }
          }

          /* Display the number with the appended e^n exponent? */
          if ( x )
            EwPrint( "%fe%d", v, x );
          else
            EwPrint( "%f", v );
        } break;

        case 'b' : /* bool */
        {
          if ( va_arg( marker, XUInt32 ))
            EwPrint( "true" );
          else
            EwPrint( "false" );
        } break;

        case 'c' : /* char */
        {
          XChar ch = (XChar)va_arg( marker, XUInt32 );

          /* Is the character 'printable'? */
          if (( ch >= 0x20 ) && ( ch <= 0x7F ))
            EwPrint( "\'%c\'", ch );
          else 
            EwPrint( "\'\\x%04X\'", ch );
        } break;

        case 'o' : /* color */
        {
          XColor co = va_arg( marker, XColor );
          EwPrint( "#%02X%02X%02X%02X" , co.Red, co.Green, co.Blue, co.Alpha );
        } break;

        case 'p' : /* point */
        {
          XPoint po = va_arg( marker, XPoint );
          EwPrint( "<%d,%d>" , po.X, po.Y );
        } break;

        case 'r' : /* rect */
        {
          XRect re = va_arg( marker, XRect );
          EwPrint( "<%d,%d,%d,%d>", re.Point1.X, re.Point1.Y, re.Point2.X, 
            re.Point2.Y );
        } break;

        case 'l' : /* language */
        {
          EwPrint( "language:0x%08X", va_arg( marker, XLangId ));
        } break;

        case 'g' : /* styles set */
        {
          EwPrint( "styles:0x%08X", va_arg( marker, XStylesSet ));
        } break;

        case '^' : /* ref to */
        {
          XRef     ref = va_arg( marker, XRef );
          XObject  obj = ref.Object;

          if ( !obj )
            EwPrint( "null" );
          else
            EwPrint( "ref:(Object:(%s)0x%p,OnGet:0x%p,OnSet:0x%p)", 
              obj->_.VMT->_Name, obj, ref.OnGet, ref.OnSet );
        } break;

        case '*' : /* object */
        {
          XObject  obj = va_arg( marker, XObject );

          if ( !obj )
            EwPrint( "null" );
          else
            EwPrint( "(%s)0x%p", obj->_.VMT->_Name, obj );
        } break;

        case '$' : /* class */
        {
          XClass  theClass = va_arg( marker, XClass );

          if ( !theClass )
            EwPrint( "null" );
          else
            EwPrint( "%s", ((const struct _vmt_XObject*)theClass)->_Name );
        } break;

        case '&' : /* slot */
        {
          XSlot    slot = va_arg( marker, XSlot );
          XObject  obj  = slot.Object;

          if ( !obj )
            EwPrint( "null" );
          else
            EwPrint( "slot:(Object:(%s)0x%p,SlotProc:0x%p)", obj->_.VMT->_Name,
              obj, slot.SlotProc );
        } break;

        case 's' : /* string */
        {
          XString str = va_arg( marker, XString );

          EwPrint( "\"%S\"", str );
        } break;

        case 'h' : EwPrint( "handle:0x%p", va_arg( marker, XHandle )); break;
      }
    }
  }

  EwPrint( "\n" );

  /* Reset arguments. */
  va_end( marker );
}


/*******************************************************************************
* FUNCTION:
*   EwThrow
*
* DESCRIPTION:
*   The function EwThrow() implements the Chora 'throw' statement. The function
*   will be called by the Run Time Environment if a Chora exception has been
*   thrown.
*
*   Internally, the function prints the message and calls EwPanic() in order
*   to stop or restart the system.
*
* ARGUMENTS:
*   aMessage - Contains a failure description.
*
* RETURN VALUE:
*   EwThrow() never returns.
*
*******************************************************************************/
void EwThrow( const XChar* aMessage )
{
  EwPrint( "%S\n", aMessage );
  EwPanic();
}


/*******************************************************************************
* FUNCTION:
*   EwError
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The function reports the message by calling the function EwPrint().
*
* ARGUMENTS:
*   aCode - Number identifying the error.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwError( int aCode )
{
  EwPrint( "EMWI ERROR: code: %d\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode );
}


/*******************************************************************************
* FUNCTION:
*   EwErrorS
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The reported message will additionally include an ANSI C string enclosed
*   between two "". The function reports the message by calling the function 
*   EwPrint().
*
* ARGUMENTS:
*   aCode   - Number identifying the error.
*   aString - Pointer to an ANSI C, zero terminated string.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwErrorS( int aCode, const char* aString )
{
  EwPrint( "EMWI ERROR: code: %d, info: \"%s\"\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode, aString );
}


/*******************************************************************************
* FUNCTION:
*   EwErrorPD
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The reported message will additionally include an ANSI C string enclosed
*   between two "" and a signed decimal number. The function reports the message
*   by calling the function EwPrint().
*
* ARGUMENTS:
*   aCode          - Number identifying the error.
*   aString        - Pointer to an ANSI C, zero terminated string.
*   aSignedDecimal - Value to display in decimal notation.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwErrorSD( int aCode, const char* aString, int aSignedDecimal )
{
  EwPrint( "EMWI ERROR: code: %d, info1: \"%s\", info2: %d\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode, aString, aSignedDecimal );
}


/*******************************************************************************
* FUNCTION:
*   EwErrorPD
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The reported message will additionally include a pointer in hex-notation and
*   a signed decimal number. The function reports the message by calling the 
*   function EwPrint().
*
* ARGUMENTS:
*   aCode          - Number identifying the error.
*   aPointer       - Pointer to display in hex notation.
*   aSignedDecimal - Value to display in decimal notation.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwErrorPD( int aCode, const void* aPointer, int aSignedDecimal )
{
  EwPrint( "EMWI ERROR: code: %d, info1: %p, info2: %d\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode, aPointer, aSignedDecimal );
}


/*******************************************************************************
* FUNCTION:
*   EwErrorDD
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The reported message will additionally include two signed decimal numbers.
*   The function reports the message by calling the function EwPrint().
*
* ARGUMENTS:
*   aCode           - Number identifying the error.
*   aSignedDecimal1,
*   aSignedDecimal2 - Value to display in decimal notation.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwErrorDD( int aCode, int aSignedDecimal1, int aSignedDecimal2 )
{
  EwPrint( "EMWI ERROR: code: %d, info1: %d, info2: %d\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode, aSignedDecimal1, aSignedDecimal2 );
}


/*******************************************************************************
* FUNCTION:
*   EwErrorPDS
*
* DESCRIPTION:
*   This function is intended to format and report fatal runtime error messages.
*   The reported message will additionally include a pointer in hex-notation,
*   a signed decimal number and an ANSI C string enclosed between two "" . The
*   function reports the message by calling the function EwPrint().
*
* ARGUMENTS:
*   aCode          - Number identifying the error.
*   aPointer       - Pointer to display in hex notation.
*   aSignedDecimal - Value to display in decimal notation.
*   aString        - Pointer to an ANSI C, zero terminated string.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwErrorPDS( int aCode, const void* aPointer, int aSignedDecimal, 
  const char* aString )
{
  EwPrint( "EMWI ERROR: code: %d, info1: %p, info2: %d, info3: \"%s\"\n"
           "For details please visit https://doc.embedded-wizard.de/errors\n",
           aCode, aPointer, aSignedDecimal, aString );
}


#ifdef EW_PRINT_PERF_COUNTERS

/*******************************************************************************
* FUNCTION:
*   EwAddPerfCounter
*
* DESCRIPTION:
*   The function EwAddPerfCounter() adds the given performance counter to the
*   global list of counters. In this manner the values collected by the counter
*   can be printed out in the EwPrintPerfCounters() function.
*
*   Once added there is no way to remove the counter from the list. This feature
*   exists for performance investigation and debugging purpose only. In the real
*   release application no performance counters should be used.
*
* ARGUMENTS:
*   aPerfCounter - Pointer to the structure describing the performance counter
*     to add.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwAddPerfCounter( XPerfCounter* aPerfCounter )
{
  XPerfCounter** perfCounter = &PerfCounters;
  XPerfCounter*  groupFirst;

  /* Search the global list for the group the performance counter belongs to */
  while ( *perfCounter && EwStrCmp((*perfCounter)->Group, aPerfCounter->Group ))
    perfCounter = &(*perfCounter)->Next;
  
  /* The group of counters is represented by the first counter within this
     group */
  if ( *perfCounter ) groupFirst = *perfCounter;
  else                groupFirst = aPerfCounter;

  /* Within the found group search for the last entry */
  while ( *perfCounter && !EwStrCmp((*perfCounter)->Group, aPerfCounter->Group ))
    perfCounter = &(*perfCounter)->Next;

  /* Append the performance counter to the end of the group */
  aPerfCounter->Next = *perfCounter;
  *perfCounter       = aPerfCounter;
  
  /* This counter belongs to the found group */
  aPerfCounter->GroupFirst = groupFirst;
}


/*******************************************************************************
* FUNCTION:
*   EwStartPerfCounter
*
* DESCRIPTION:
*   The function EwStartPerfCounter() starts the time measuring for the given
*   performance counter. Each call of this function is counted by the counter.
*
*   The time measuring runs till the function EwStopPerfCounter() is called for
*   this performance counter. In case of recursive calls with the same counter
*   the measuring starts with the first EwStartPerfCounter() and ends with the
*   last EwStopPerfCounter().
*
* ARGUMENTS:
*   aPerfCounter - Pointer to the structure describing the performance counter
*     to start the time measuring.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwStartPerfCounter( XPerfCounter* aPerfCounter )
{
  /* The measuring starts with the first non-recursive call */
  if ( !aPerfCounter->Recursions++ )
  {
    EwGetPerfCounter( &aPerfCounter->StartTime1, &aPerfCounter->StartTime2, 0 );
    aPerfCounter->GroupRecursions++;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwStopPerfCounter
*
* DESCRIPTION:
*   The function EwStopPerfCounter() stops the time measuring for the given
*   performance counter. From the measured time values the function stores the
*   minimum and the maximum as well as the total time the performance counter 
*   has collected.
*
*   The time measuring starts with the function call EwStopPerfCounter(). In
*   case of recursive calls with the same counter the measuring stops with the
*   last EwStopPerfCounter() invocation.
*
* ARGUMENTS:
*   aPerfCounter - Pointer to the structure describing the performance counter
*     to stop the time measuring.
*
* RETURN VALUE:
*   None.
*
*******************************************************************************/
void EwStopPerfCounter( XPerfCounter* aPerfCounter )
{
  unsigned long endTime1, endTime2;
  unsigned long span;

  /* The measuring ends with the last non-recursive call */
  if ( !--aPerfCounter->Recursions )
  {
    EwGetPerfCounter( &endTime1, &endTime2, 0 );

    /* The time ellapsed since the EwStartPerfCounter() */
    span = ( endTime1 - aPerfCounter->StartTime1 ) * 1000000 +
           ( endTime2 - aPerfCounter->StartTime2 );

    /* Collect the measured time value */
    aPerfCounter->TotalTime += span;
    aPerfCounter->Calls++;
    
    /* Remember the minimum/maximum measured time value */
    if ( span < aPerfCounter->MinTime ) aPerfCounter->MinTime = span;
    if ( span > aPerfCounter->MaxTime ) aPerfCounter->MaxTime = span;

    /* Accumulate the measured time also in the group this counter belongs to */
    if ( !--aPerfCounter->GroupRecursions )
    {
      aPerfCounter->GroupFirst->GroupTotalTime += span;
      aPerfCounter->GroupFirst->GroupCalls++;
    }
  }
}


/*******************************************************************************
* FUNCTION:
*   EwResetPerfCounters
*
* DESCRIPTION:
*   The function EwResetPerfCounters() resets all performance counters so new
*   performance test can run. This function is usually called immediately before
*   a performance test begins.
*
*   After this function call EwStartPerfCounter() and EwStopPerfCounter() are
*   used to measure and collect time values. The collected values can then be
*   printed to the console by using the function EwPrintPerfCounters().
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwResetPerfCounters( void )
{
  XPerfCounter* perfCounter = PerfCounters;
  
  /* Repeat until all counters are reset */
  while ( perfCounter )
  {
    perfCounter->Calls           = 0;
    perfCounter->Recursions      = 0;
    perfCounter->TotalTime       = 0;
    perfCounter->MinTime         = 0xFFFFFFFF;
    perfCounter->MaxTime         = 0;
    perfCounter->GroupRecursions = 0;
    perfCounter->GroupCalls      = 0;
    perfCounter->GroupTotalTime  = 0;

    /* Continue with the next counter */
    perfCounter = perfCounter->Next;
  }

  /* Leave a message in the log */
  EwPrint( "Reset performance counters\n" );

  /* Remember the time when the reset has been performed */
  EwGetPerfCounter( &ResetTime1, &ResetTime2, &ResetTime3 );
}


/*******************************************************************************
* FUNCTION:
*   EwPrintPerfCounters
*
* DESCRIPTION:
*   The function EwPrintPerfCounters() prints the time values measured and
*   collected in all performance counters.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwPrintPerfCounters( void )
{
  const char*   separator = "-------------------------------------------------"
                            "----------------------------------------------\n";
  const char*   groupHead = "Group Name                    [ Calls ]  [  Time"
                            "  Total  ]\n";
  const char*   totalRow  = "Total Time                    %9d  %7.3f %s"
                            " %5.1f%% of real CPU time\n";
  const char*   groupRow  = "%.28s%*s %9d  %7.3f %s %5.1f%%\n";
  const char*   entryHead = "%.28s%*s [ Calls ]  [  Time  Total  ]  [Time Avg]"
                            "  [Time Min]  [Time Max]\n";
  const char*   entryRow  = "%.28s%*s %9d  %7.3f %s %5.1f%%  %7.3f %s  %7.3f"
                            " %s  %7.3f %s\n";
  XPerfCounter* perfCounter = PerfCounters;
  unsigned long printTime1, printTime2, printTime3;
  unsigned long span1;
  unsigned long span2;
  double        t1, t2, t3, t4, t5;
  const char*   t1Unit;
  const char*   t2Unit;
  const char*   t3Unit;
  const char*   t4Unit;

  /* Get the current time */
  EwGetPerfCounter( &printTime1, &printTime2, &printTime3 );

  /* The time ellapsed since the EwResetPerfCounters() */
  span1 = ( printTime1 - ResetTime1 ) * 1000000 + ( printTime2 - ResetTime2 );
  span2 =   printTime3 - ResetTime3;
  
  /* Print the overview about the performance counter groups */
  EwPrint( separator );
  EwPrint( groupHead );
  EwPrint( separator );

  /* Total time between EwResetPerfCounters() and this EwPrintPerfCounters() */
  if ( span1 >= 1000000 ) { t1 = (double)span1 * 0.000001; t1Unit = "s "; }
  else                    { t1 = (double)span1 * 0.001000; t1Unit = "ms"; }
  if ( span1 > 0        )   t5 = (double)span2 * ( 100.0 / (double)span1 );
  else                      t5 = 100.0;

  /* Print the global performance information */
  EwPrint( totalRow, 1, t1, t1Unit, t5 );
  
  /* Print the group specific performance information */
  while ( perfCounter )
  {
    XPerfCounter* group = perfCounter;
    unsigned long calls = group->GroupCalls;
    unsigned long span3 = group->GroupTotalTime;
    
    /* Skip to the next group */
    while ( perfCounter && !EwStrCmp( perfCounter->Group, group->Group ))
      perfCounter = perfCounter->Next;

    /* Calculate the time measured by all counters of the current group */
    if ( span3 >= 1000000 ) { t1 = (double)span3 * 0.000001; t1Unit = "s "; }
    else                    { t1 = (double)span3 * 0.001000; t1Unit = "ms"; }
    if ( span1 > 0        )   t5 = (double)span3 * ( 100.0 / (double)span1 );
    else                      t5 = 100.0;

    /* Print the group performance information */
    EwPrint( groupRow, group->Group, 29 - EwStrLen( group->Group ), " ", calls,
             t1, t1Unit, t5 );
  }
  
  /* Terminate the group table */
  EwPrint( separator );
  EwPrint( "\n" );
  
  /* Print all counters group-wise */
  for ( perfCounter = PerfCounters; perfCounter; )
  {
    XPerfCounter* group = perfCounter;
    
    EwPrint( separator );
    EwPrint( entryHead, group->Group, 29 - EwStrLen( group->Group ), " " );
    EwPrint( separator );
    
    /* Print all counters of this group */
    while ( perfCounter && !EwStrCmp( perfCounter->Group, group->Group ))
    {
      /* Ignore empty entries */
      if ( perfCounter->Calls )
      {
        unsigned long span3 = perfCounter->TotalTime;
        unsigned long span4 = perfCounter->TotalTime / perfCounter->Calls;
        unsigned long span5 = perfCounter->MinTime;
        unsigned long span6 = perfCounter->MaxTime;
        
        /* Calculate the time measured by all counters of the current group */
        if ( span3 >= 1000000 ) { t1 = (double)span3 * 0.000001; t1Unit = "s "; }
        else                    { t1 = (double)span3 * 0.001000; t1Unit = "ms"; }
        if ( span4 >= 1000000 ) { t2 = (double)span4 * 0.000001; t2Unit = "s "; }
        else                    { t2 = (double)span4 * 0.001000; t2Unit = "ms"; }
        if ( span5 >= 1000000 ) { t3 = (double)span5 * 0.000001; t3Unit = "s "; }
        else                    { t3 = (double)span5 * 0.001000; t3Unit = "ms"; }
        if ( span6 >= 1000000 ) { t4 = (double)span6 * 0.000001; t4Unit = "s "; }
        else                    { t4 = (double)span6 * 0.001000; t4Unit = "ms"; }
        if ( span1 > 0        )   t5 = (double)span3 * ( 100.0 / (double)span1 );
        else                      t5 = 100.0;

        /* Print the counter parameters */
        EwPrint( entryRow, perfCounter->Name, 29 - EwStrLen( perfCounter->Name ),
                 " ", perfCounter->Calls, t1, t1Unit, t5, t2, t2Unit, t3, t3Unit,
                 t4, t4Unit );
      }
      
      /* Continue with the next */
      perfCounter = perfCounter->Next;
    }

    /* Terminate the table */
    EwPrint( separator );
    EwPrint( "\n" );
  }
}

#endif /* EW_PRINT_PERF_CONTERS */

/* pba, msy */

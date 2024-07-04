/*******************************************************************************
*
* E M B E D D E D   W I Z A R D   P R O J E C T
*
*                                                Copyright (c) TARA Systems GmbH
*                                    written by Paul Banach and Manfred Schweyer
*
********************************************************************************
*
* This software is delivered "as is" and shows the usage of other software
* components. It is provided as an example software which is intended to be
* modified and extended according to particular requirements.
*
* TARA Systems hereby disclaims all warranties and conditions with regard to the
* software, including all implied warranties and conditions of merchantability
* and non-infringement of any third party IPR or other rights which may result
* from the use or the inability to use the software.
*
********************************************************************************
*
* DESCRIPTION:
*   This file is part of the interface (glue layer) between an Embedded Wizard
*   generated UI application and the board support package (BSP) of a dedicated
*   target.
*
*******************************************************************************/

#include "ewconfig.h"
#include "ewrte.h"
#include "stm32g4xx_hal.h"
#include "stm32g4xx_nucleo.h"
#include "SpiFlashDriver.h"

//#include "Graphics.h"

#include "ew_bsp_flash_reader.h"

#if EW_USE_EXTERNAL_FLASH == 1

/* block size that has to be read at once from the external flash
   and the size of a single cach line - must be power of 2 */
#define FLASH_READER_BLOCK_SIZE	64

/* the flash reader works with 4 cache lines (= 4 blocks that are cached)
   and the start address of each cache line has to be dword aligned */
static unsigned char Cache[ 4 * FLASH_READER_BLOCK_SIZE + 3 ];

static unsigned int FlashAddr[ 4 ]; /* origin addresses of all cached blocks */
static void* CacheAddr[ 4 ]; /* start addresses of corrresponding cache line */

/*******************************************************************************
* FUNCTION:
*   EwBspFlashReaderProc
*
* DESCRIPTION:
*   This callback function is used to read data from the external flash memory,
*   which cannot be accessed directly by the CPU. Embedded Wizard invokes this
*   function when accessing strings or bitmap/font data automatically to 'map'
*   the flash contents into the CPU address space. In this way, the not directly
*   accessible Flash data can be processed by Embedded Wizard.
*
* ARGUMENTS:
*   aAddress - The address to load the corresponding area from the Flash.
*
* RETURN VALUE:
*   Returns the address of the location in an internal buffer corresponding to
*   the original aAddress from the Flash. If the loading fails or aAddress does
*   not lie within the Flash memory, the function should return aAddress without
*   any changes on it.
*
*******************************************************************************/
const void* EwBspFlashReaderProc( const void* aAddress )
{
  int i;
  void* tmp;

  /* split given address in block number and offset */
  unsigned int block = (unsigned int)aAddress &~ ( FLASH_READER_BLOCK_SIZE - 1 );
  unsigned int offset = (unsigned int)aAddress & ( FLASH_READER_BLOCK_SIZE - 1 );

  /* search for already cached content and return immediate the address */
  for ( i = 0; i < 4; i++ )
  {
    if ( block == FlashAddr[ i ])
      return CacheAddr[ i ] + offset;
  }

  /* forget the oldest cache line, shift the entries and prepare the newest */
  tmp = CacheAddr[ 3 ];
  for ( i = 3; i > 0; i-- )
  {
    FlashAddr[ i ] = FlashAddr[ i - 1 ];
    CacheAddr[ i ] = CacheAddr[ i - 1 ];
  }
  CacheAddr[ 0 ] = tmp;
  FlashAddr[ 0 ] = block;

  /* load the requested flash block into the free cache line */
  SpiFlashDriver_ReadDataDma( block, (uint8_t*)CacheAddr[ 0 ], FLASH_READER_BLOCK_SIZE );
  while( SpiFlashDriver_ReceiveActive() == 1 )
    ;

  return CacheAddr[ 0 ] + offset;
}

#endif

/*******************************************************************************
* FUNCTION:
*   EwBspFlashReaderInit
*
* DESCRIPTION:
*   The function EwBspFlashReaderInit() is used to initialize a flash driver in
*   order to access an external flash that is not in the CPU address range.
*   Furthermore, a callback function is registered that is called by Graphics
*   Engine or Runtime Environment when a new block of data has to be read.
*
* ARGUMENTS:
*   aStartAddress - Address that is used by the linker as start address for the
*     external flash. The address cannot be accessed directly by CPU.
*   aSize         - Size of the flash memory area in bytes.
*
* RETURN VALUE:
*   Returns 1 if successful, 0 otherwise.
*
*******************************************************************************/
int EwBspFlashReaderInit( void* aStartAddress, int aSize )
{
#if EW_USE_EXTERNAL_FLASH == 1

  if (((unsigned int)aStartAddress % FLASH_READER_BLOCK_SIZE ) || ( aSize % FLASH_READER_BLOCK_SIZE ))
  {
    EwPrint( "EwBspFlashReaderInit: Invalid address or size!\n" );
    return 0;
  }

  /* intialize SPI driver to access external flash device */
  SpiFlashDriver_PeripherieInit();

  /* initalize the flash addresses and the pointers to the cache lines */
  FlashAddr[ 0 ] = 0;
  FlashAddr[ 1 ] = 0;
  FlashAddr[ 2 ] = 0;
  FlashAddr[ 3 ] = 0;
  CacheAddr[ 0 ] = (void*)(((unsigned int)Cache + 3 ) &~ 3);
  CacheAddr[ 1 ] = CacheAddr[ 0 ] + FLASH_READER_BLOCK_SIZE;
  CacheAddr[ 2 ] = CacheAddr[ 1 ] + FLASH_READER_BLOCK_SIZE;
  CacheAddr[ 3 ] = CacheAddr[ 2 ] + FLASH_READER_BLOCK_SIZE;

  /* register the callback for the flash loader */
  EwRegisterFlashAreaReader( EwBspFlashReaderProc, aStartAddress, aStartAddress + aSize, FLASH_READER_BLOCK_SIZE );

#endif
  return 1;
}


/* msy */

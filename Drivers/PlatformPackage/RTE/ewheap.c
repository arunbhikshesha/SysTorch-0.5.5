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
*   The module 'ewheap.c' contains the implementation of a heap memory manager
*   optimized for Embedded Wizard UI applications.
*
* SUBROUTINES:
*   EwInitHeap
*   EwDoneHeap
*   EwAddHeapMemoryPool
*   EwGetHeapInfo
*   EwDumpHeap
*   EwVerifyHeap
*   EwAllocHeapBlock
*   EwFreeHeapBlock
*   EwIsHeapPtr
*
*******************************************************************************/

#include "ewrte.h"


/* The following structure represents the begin of a free memory block. All
   free blocks build a double-chained list. The value 'Size' determines the 
   size of the entire block (incl. struct XMemoryBlock) in bytes. The values
   'Next' and 'Prev' build the chained list. */
typedef struct _XMemoryBlock
{
  long                  Size;
  struct _XMemoryBlock* Next;
  struct _XMemoryBlock* Prev;
} XMemoryBlock;


/* The following structure stores information for a single memory pool. Every
   memory pool manages a dedicated memory area. The start and the end of this 
   area are found in the both variables 'Start' and 'Guard'. The memory area 
   is divided in memory blocks. The blocks which are not in use are managed as
   a double chained list referred by the variables 'FirstFree' and 'LastFree'.

   The heap manager tries to manage small and large objects separately reducing
   so the probability for heap fragmentation. Accordingly allocations with size
   less than the value of the global variable 'LargeObjectSize' are considered 
   as 'small object'. Such objects are allocated from the begin of the list of 
   free memory blocks. In turn all other objects are allocated from the end of 
   the list. The variable 'Separator' addresses the border between the small and
   large objects.

   The variables 'TotalSize', 'FreeSize', 'NoOfFreeBlocks', 'NoOfUsedBlocks',
   'SmallAllocCounter', 'LargeAllocCounter' and 'FreeCounter' track the pool
   usage.

   The here implemented heap manager can maintain several memory pools. These
   are stored within a double chained list. The variables 'Next' and 'Prevc'
   build this chain. */
typedef struct _XMemoryPool
{
  struct _XMemoryPool*  Next;
  struct _XMemoryPool*  Prev;
  long                  TotalSize;
  long                  FreeSize;
  long                  NoOfFreeBlocks;
  long                  NoOfUsedBlocks;
  XMemoryBlock*         Start;
  XMemoryBlock*         Guard;
  XMemoryBlock*         FirstFree;
  XMemoryBlock*         LastFree;
  XMemoryBlock*         Separator;
  unsigned long         SmallAllocCounter;
  unsigned long         LargeAllocCounter;
  unsigned long         FreeCounter;
} XMemoryPool;


/* Internally used defines. */
#define UNIT_SIZE                  ( sizeof( void* ))
#define MIN_POOL_SIZE              1024
#define MIN_FREE_BLOCK_SIZE        ( sizeof( XMemoryBlock ) + UNIT_SIZE )
#define LARGE_OBJECT_SIZE          128
#define STATUS_BLOCK_USED          1
#define STATUS_BLOCK_PREV_FREE     2
#define STATUS_BLOCK_ANY           3


/* Helper macro to get the size of a block regardless of the status bits 
   also stored within the 'Size' field. */
#define BLOCK_SIZE( aBlock )                                                  \
  ((aBlock)->Size & ~STATUS_BLOCK_ANY )

/* Helper macro to get the next following block from the size of the actual
   block. */
#define NEXT_BLOCK( aBlock )                                                  \
  ((XMemoryBlock*)((char*)(aBlock) + BLOCK_SIZE( aBlock )))


/* The list containing all memory pools. */
static XMemoryPool* FirstPool = 0;
static XMemoryPool* LastPool  = 0;

/* The variable 'LargeObjectSize' stores the size of objects considered as large
   and long-lasting. Such objects are allocated from the end of the list of free
   memory blocks. In turn objects with size less than 'LargeObjectSize' are 
   allocated from the begin of the list. */
static long LargeObjectSize = LARGE_OBJECT_SIZE;


/*******************************************************************************
* FUNCTION:
*   EwInitHeap
*
* DESCRIPTION:
*   The function EwInitHeap() initializes the heap manager. Thereupon memory
*   pools can be added by using the function EwAddHeapMemoryPool(). If there
*   was heap manager initialized previously, the associated information is
*   discarded with this invocation.
*
* ARGUMENTS:
*   aLargeObjectSize - Determines the size of objects considered as large and
*     long-lasting. Such objects are allocated from the end of the list of free
*     memory blocks. All other objects are allocated from the begin of the list.
*     This reduces the probability for heap fragmentation. If this parameter is
*     <= 0 a default value for the LargeObjectSize setting is assumed.
*
* RETURN VALUE:
*   Returns != 0 if successful.
*
*******************************************************************************/
int EwInitHeap( int aLargeObjectSize )
{
  /* No LargeObjectSize specified - assume default value */
  if ( aLargeObjectSize <= 0 )
    aLargeObjectSize = LARGE_OBJECT_SIZE;

  LargeObjectSize = aLargeObjectSize;
  FirstPool       = 0;
  LastPool        = 0;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwDoneHeap
*
* DESCRIPTION:
*   The function EwDoneHeap() deinitializes the heap manager.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDoneHeap( void )
{
  LargeObjectSize = LARGE_OBJECT_SIZE;
  FirstPool = 0;
  LastPool  = 0;
}


/*******************************************************************************
* FUNCTION:
*   EwAddHeapMemoryPool
*
* DESCRIPTION:
*   The function EwAddHeapMemoryPool() adds to the heap manager a new memory 
*   area. From now the memory is used by subsequent EwAllocHeapBlock() function
*   invocations. Once added the memory area can't be removed again without 
*   reinitialization of the entire heap manager.
*
* ARGUMENTS:
*   aAddress - The start address of the memory area.
*   aSize    - The size of the memory area in bytes.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwAddHeapMemoryPool( void* aAddress, long aSize )
{
  unsigned long adr = (unsigned long)aAddress;
  XMemoryPool*  pool;
  
  /* Ensure the memory manager works with aligned addresses */
  if ( adr % sizeof( void* ))
  {
    adr      = UNIT_SIZE - ( adr % UNIT_SIZE );
    aAddress = (char*)aAddress + adr;
    aSize   -= (long)adr;
  }
  
  /* Also the memory size has to be aligned. Part of the memory is used for
     management data. */
  aSize = ( aSize & ~( UNIT_SIZE - 1 )) - sizeof( XMemoryPool );

  /* The memory area is too small for usage as a new memory pool.
     It doesn't make sense to use it. */
  if ( aSize < MIN_POOL_SIZE )
  {
    EwError( 334 );
    return;
  }

  pool = (XMemoryPool*)aAddress;

  /* Complete the description of the memory pool and ... */
  pool->TotalSize         = aSize;
  pool->FreeSize          = aSize - UNIT_SIZE;
  pool->NoOfFreeBlocks    = 1;
  pool->NoOfUsedBlocks    = 0;
  pool->Start             = (XMemoryBlock*)( pool + 1 );
  pool->Guard             = (XMemoryBlock*)((char*)( pool + 1 ) + aSize );
  pool->FirstFree         = pool->Start;
  pool->LastFree          = pool->Start;
  pool->Separator         = pool->Start;
  pool->SmallAllocCounter = 0;
  pool->LargeAllocCounter = 0;
  pool->FreeCounter       = 0;

  /* ... append the new pool to the end of the list of memory pools  */
  pool->Next = 0;
  pool->Prev = LastPool;
  LastPool   = pool; 
  if ( !FirstPool ) FirstPool        = pool;
  if ( pool->Prev ) pool->Prev->Next = pool;

  /* At its creation time, the just added pool consists of one large free
     block */
  pool->FirstFree->Next = 0;
  pool->FirstFree->Prev = 0;
  pool->FirstFree->Size = aSize;
  pool->Guard[-1].Prev  = pool->FirstFree;
}


/*******************************************************************************
* FUNCTION:
*   EwGetHeapInfo
*
* DESCRIPTION:
*   The function EwGetHeapInfo() returns few information reflecting the actuall
*   state of the heap manager. As such it is useful for debugging purpose and
*   tests. The functions copies the values into variables referred by the
*   function parameters. If a parameter is 0 (zero), the value is ignored.
*
* ARGUMENTS:
*   aNoOfMemoryPools   - Receives the number of managed memory pools. Memory
*     pools are added to the heap by using the function EwAddHeapMemoryPool().
*   aTotalSize         - Receives the size in bytes of the entire memory (used
*     and free).
*   aFreeSize          - Receives the size in bytes of the free memory.
*   aNoOfUsedBlocks    - Receives the number of allocated memory blocks.
*   aNoOfFreeBlocks    - Receives the number of free blocks.
*   aSmallestFreeBlock - Receives the size in bytes of the smallest free block.
*   aLargestFreeBlock  - Receives the size in bytes of the largest free block.
*   aSmallAllocCounter - Receives the total number of performed alloc operations
*     with small blocks.
*   aLargeAllocCounter - Receives the total number of performed alloc operations
*     with large blocks.
*   aFreeCounter       - Receives the total number of performed free operations.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwGetHeapInfo( long* aNoOfMemoryPools, long* aTotalSize, long* aFreeSize,
  long* aNoOfUsedBlocks, long* aNoOfFreeBlocks, long* aSmallestFreeBlock,
  long* aLargestFreeBlock, unsigned long* aSmallAllocCounter, 
  unsigned long* aLargeAllocCounter, unsigned long* aFreeCounter )
{
  long          noOfMemoryPools   = 0;
  long          totalSize         = 0;
  long          freeSize          = 0;
  long          noOfUsedBlocks    = 0;
  long          noOfFreeBlocks    = 0;
  long          smallestFreeBlock = 0x7FFFFFFF;
  long          largestFreeBlock  = 0;
  unsigned long smallAllocCounter = 0;
  unsigned long largeAllocCounter = 0;
  unsigned long freeCounter       = 0;
  XMemoryPool*  pool              = FirstPool;

  /* Traverse the list of memory pools */
  for ( ; pool; pool = pool->Next )
  {
    XMemoryBlock* block = pool->FirstFree;

    totalSize         += pool->TotalSize;
    freeSize          += pool->FreeSize;
    noOfFreeBlocks    += pool->NoOfFreeBlocks;
    noOfUsedBlocks    += pool->NoOfUsedBlocks;
    smallAllocCounter += pool->SmallAllocCounter;
    largeAllocCounter += pool->LargeAllocCounter;
    freeCounter       += pool->FreeCounter;
    noOfMemoryPools++;

    /* Should evaluate the list of free blocks looking for the smallest/largest
       block? */
    if ( aSmallestFreeBlock || aLargestFreeBlock )
      for ( ; block; block = block->Next )
      {
        if ( block->Size < smallestFreeBlock ) smallestFreeBlock = block->Size;
        if ( block->Size > largestFreeBlock  ) largestFreeBlock  = block->Size;
      }
  }

  if ( smallestFreeBlock > largestFreeBlock )
    smallestFreeBlock = 0;

  if ( aNoOfMemoryPools   ) *aNoOfMemoryPools   = noOfMemoryPools;
  if ( aTotalSize         ) *aTotalSize         = totalSize;
  if ( aFreeSize          ) *aFreeSize          = freeSize;
  if ( aNoOfUsedBlocks    ) *aNoOfUsedBlocks    = noOfUsedBlocks;
  if ( aNoOfFreeBlocks    ) *aNoOfFreeBlocks    = noOfFreeBlocks;
  if ( aSmallestFreeBlock ) *aSmallestFreeBlock = smallestFreeBlock - UNIT_SIZE;
  if ( aLargestFreeBlock  ) *aLargestFreeBlock  = largestFreeBlock  - UNIT_SIZE;
  if ( aSmallAllocCounter ) *aSmallAllocCounter = smallAllocCounter;
  if ( aLargeAllocCounter ) *aLargeAllocCounter = largeAllocCounter;
  if ( aFreeCounter       ) *aFreeCounter       = freeCounter;
}


/*******************************************************************************
* FUNCTION:
*   EwDumpHeap
*
* DESCRIPTION:
*   The function EwDumpHeap() exists for debugging purpose. It evaluates the
*   contents of all memory pools managed actually be the heap manager and
*   prints the associated information as well as the existing blocks.
*
* ARGUMENTS:
*   aDetailed - If != 0, the functions prints a list of all memory blocks.
*     If == 0, only heap overview is printed.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDumpHeap( int aDetailed )
{
  XMemoryPool* pool = FirstPool;
  int          i    = 0;

  EwPrint( "\nHEAP DUMP BEGIN. Small/Large objects size threshold: %d bytes", 
           LargeObjectSize );

  /* Traverse the list of existing memory pools */
  for ( ; pool; pool = pool->Next, i++ )
  {
    XMemoryBlock* block = pool->Start;
    
    EwPrint( "\nPOOL No. %d\n", i );
    EwPrint( "  TotalSize:          %d\n", pool->TotalSize );
    EwPrint( "  FreeSize:           %d\n", pool->FreeSize );
    EwPrint( "  NoOfFreeBlocks:     %d\n", pool->NoOfFreeBlocks );
    EwPrint( "  NoOfUsedBlocks:     %d\n", pool->NoOfUsedBlocks );
    EwPrint( "  Start:              %p\n", pool->Start );
    EwPrint( "  Guard:              %p\n", pool->Guard );
    EwPrint( "  FirstFree:          %p\n", pool->FirstFree );
    EwPrint( "  LastFree:           %p\n", pool->LastFree );
    EwPrint( "  Small/Large border: %p\n", pool->Separator );
    EwPrint( "  SmallAllocCounter:  %u\n", pool->SmallAllocCounter );
    EwPrint( "  LargeAllocCounter:  %u\n", pool->LargeAllocCounter );
    EwPrint( "  FreeCounter:        %u\n", pool->FreeCounter );

    /* Traverse the list of blocks within the pool */
    if ( aDetailed )
      for ( ; block < pool->Guard; block = NEXT_BLOCK( block ))
        EwPrint( "\n  %p %s %d bytes %s", block,
              ( block->Size & STATUS_BLOCK_USED )? "USED" : "FREE",
              (int)( BLOCK_SIZE( block )),
              ( block->Size & STATUS_BLOCK_PREV_FREE )? "(PREV_FREE)" : "" );
  }

  EwPrint( "\nHEAP DUMP END.\n" );
}


/*******************************************************************************
* FUNCTION:
*   EwVerifyHeap
*
* DESCRIPTION:
*   The function EwVerifyHeap() exists for debugging purpose. It evaluates the
*   contents of all memory pools managed actually be the heap manager with the
*   aim to verify whether the heap is coherent. In case the verification were
*   successful, the function returns != 0. Otherwise an adequate messages is
*   reported and the function returns 0.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns != 0 if the heap is coherent. In case of an error, the value 0 is
*   returned.
*
*******************************************************************************/
int EwVerifyHeap( void )
{
  XMemoryPool* pool = FirstPool;
  int          ok   = 1;

  /* Traverse the list of existing memory pools */
  for ( ; pool; pool = pool->Next )
  {
    XMemoryBlock* block          = pool->Start;
    XMemoryBlock* nextFree       = pool->FirstFree;
    XMemoryBlock* blockP         = 0;
    long          freeSize       = 0;
    long          noOfFreeBlocks = 0;
    long          noOfUsedBlocks = 0;
    int           error          = 0;
    
    /* 1. Is this the first pool within the list? */
    if ( !pool->Prev && ( FirstPool != pool ))
      error = 1;

    /* 2. Is this the last pool within the list? */
    else if ( !pool->Next && ( LastPool != pool ))
      error = 2;

    /* 3. Does the pool build a correct chain with the next pool? */
    else if ( pool->Next && ( pool->Next->Prev != pool ))
      error = 3;

    /* 4. Pool has to have correct total size */
    else if (( pool->TotalSize <= MIN_POOL_SIZE ) || 
             ( pool->TotalSize & ( UNIT_SIZE - 1 )))
      error = 4;

    /* 5: The start of the pool memory has to be valid */
    else if ((XMemoryBlock*)( pool + 1 ) != pool->Start )
      error = 5;
    
    /* 6: The end of the pool memory has to be valid */
    else if ((XMemoryBlock*)((char*)pool->Start + pool->TotalSize ) != 
              pool->Guard )
      error = 6;

    /* Traverse the list of blocks */
    for ( ; ( block != pool->Guard ) && !error; blockP = block, 
              block = NEXT_BLOCK( block ))
    {
      /* 7: The block has to address a correct memory */
      if (( block < pool->Start ) || ( block >= pool->Guard ) ||
          ((unsigned long)block & ( UNIT_SIZE - 1 )))
        error = 7;
      
      /* 8: The block has to have valid size, which is multiple of a unit */
      else if (( BLOCK_SIZE( block ) <= 0 ) || 
               ( BLOCK_SIZE( block ) & ( UNIT_SIZE - 1 )))
        error = 8;

      /* 9: The block size have to lie within the pool memory area */
      else if ( NEXT_BLOCK( block ) > pool->Guard )
        error = 9;

      /* 10. The block has to have a valid status flags */
      else if (( block->Size & 3 ) == STATUS_BLOCK_PREV_FREE )
        error = 10;

      /* 11. Blocks marked with 'prev free' has to follow a free block */
      else if (( block->Size & STATUS_BLOCK_PREV_FREE ) && 
               ( !blockP || ( blockP->Size & STATUS_BLOCK_USED )))
        error = 11;

      /* 12. Used block not marked with 'prev free' has to follow a
             another used block */
      else if (( block->Size & STATUS_BLOCK_USED ) && 
              !( block->Size & STATUS_BLOCK_PREV_FREE ) &&
                 blockP && !( blockP->Size & STATUS_BLOCK_USED ))
        error = 12;

      /* 13. A block marked as 'free' has to be part of the chain of free 
         blocks */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && ( nextFree != block ))
        error = 13;

      /* 14. A block marked as 'free' has to be far away from the guard */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && 
              (( block + 1 ) >= pool->Guard ))
        error = 14;

      /* 15. The 'next' link has to be a forward address pointer within the 
         pool memory */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && block->Next &&
                 (( block->Next <= ( block + 1 )) || 
                  ( block->Next >= ( pool->Guard - 1 )) || 
                  ((long)block->Next & ( UNIT_SIZE - 1 ))))
        error = 15;
      
      /* 16. ... or it has to be the last free block in the chain */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && !block->Next &&
                 ( pool->LastFree != block ))
        error = 16;

      /* 17. Is the 'next' block also free */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && block->Next &&
                 ( block->Next->Size & STATUS_BLOCK_USED ))
        error = 17;
      
      /* 18. The 'next' free block has to point back to the current free block */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && block->Next &&
                 ( block->Next->Prev != block ))
        error = 18;
      
      /* 19. The free block has a preceding blockm if it is not the first one */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && 
                (( block->Prev == 0 ) != ( pool->FirstFree == block )))
        error = 19;
      
      /* 20: The free blocks are always merged together */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && NEXT_BLOCK( block ) &&
                 ( NEXT_BLOCK( block ) != pool->Guard ) && 
                !( NEXT_BLOCK( block )->Size & STATUS_BLOCK_USED ))
        error = 20;

      /* 21: Within the regular memory area every free block manages at its end
         a link to itself */
      else if ( !( block->Size & STATUS_BLOCK_USED ) && 
              ( NEXT_BLOCK( block ) - 1 )->Prev != block )
        error = 21;

      /* Count used/free blocks and track the free memory size */
      if ( !( block->Size & STATUS_BLOCK_USED ))
      {
        freeSize += BLOCK_SIZE( block ) - UNIT_SIZE;
        noOfFreeBlocks++;
      }
      else
        noOfUsedBlocks++;

      /* The next expected free block in the chain */
      if ( !error & ( block == nextFree ))
        nextFree = block->Next;
    }

    /* 22. The number of blocks has to be correct */
    if ( !error && (( noOfFreeBlocks != pool->NoOfFreeBlocks ) ||
       ( noOfUsedBlocks != pool->NoOfUsedBlocks )))
      error = 22;

    /* 23. The cummulated free size has to be correct */
    if ( !error && ( freeSize != pool->FreeSize ))
      error = 23;

    /* 24. The estimated left border for large objects is valid */
    if ( !error && (( pool->Separator < pool->Start ) || 
       ( pool->Separator >= pool->Guard )))
      error = 24;

    /* Print the error message */
    if ( error )
      EwErrorPD( 335, pool, error );

    /* Any errors in the pool detected? */
    ok &= !error;
  }

  return ok;
}


/* The following helper function traverses the list of free blocks looking
   for a block with size of at least aSize bytes. The search operation starts
   with the block aBlock. The function returns the found block or 0 if there
   is no block matching the search condition. */
static XMemoryBlock* FindNextFirstFit( long aSize, XMemoryBlock* aBlock )
{
  while ( aBlock && ( aBlock->Size < aSize ))
    aBlock = aBlock->Next;

  return aBlock;
}


/* The following helper function traverses the list of free blocks looking
   for a block with size of at least aSize bytes. The search operation starts
   with the block aBlock. The function returns the found block or 0 if there
   is no block matching the search condition. */
static XMemoryBlock* FindPrevFirstFit( long aSize, XMemoryBlock* aBlock )
{
  while ( aBlock && ( aBlock->Size < aSize ))
    aBlock = aBlock->Prev;

  return aBlock;
}


/* The following helper function traverses the list of free blocks looking
   for a block with optimal size of at aSize bytes. The search operation 
   starts with the block aBlock. The function returns the found block or 0
   if there is no block matching the search condition. Also, the function
   stops if the actual block lies in front of aStop and returns this block. */
static XMemoryBlock* FindPrevBestFit( long aSize, XMemoryBlock* aBlock,
  XMemoryBlock* aStop )
{
  XMemoryBlock* foundBlock = 0;
  long          foundSize  = 0x7FFFFFFFl;

  for ( ; aBlock > aStop; aBlock = aBlock->Prev )
    if (( aBlock->Size >= aSize ) && ( aBlock->Size < foundSize ))
    {
      foundBlock = aBlock;
      foundSize  = aBlock->Size;
    }

  return foundBlock? foundBlock : aBlock;
}


/*******************************************************************************
* FUNCTION:
*   EwAllocHeapBlock
*
* DESCRIPTION:
*   The function EwAllocHeapBlock() tries to allocate memory block with the 
*   given number of bytes. Once not needed anymore, the memory should be 
*   released by using the function EwFreeHeapBlock().
*
* ARGUMENTS:
*   aSize - Size of the memory to allocate in bytes.
*
* RETURN VALUE:
*   Returns a pointer to the allocated memory or 0 (zero) if there is not 
*   enough free memory on the heap.
*
*******************************************************************************/
void* EwAllocHeapBlock( int aSize )
{
  XMemoryPool*  pool   = 0;
  XMemoryBlock* block  = 0;
  XMemoryBlock* blockW = 0;
  XMemoryBlock* blockN;
  XMemoryBlock* blockP;
  int           isLarge;

  /* Nothing to alloc */
  if ( aSize <= 0 )
    return 0;

  /* Too large to alloc */
  if ( aSize >= ( 1 << 28 ))
    return 0;

  /* No memory pool existing. Use EwInitHeap() and EwAddHeapMemoryPool() first! */
  if ( !FirstPool )
  {
    EwError( 322 );
    return 0;
  }

  /* Align the memory size to the next complete unit */
  if ( aSize % UNIT_SIZE )
    aSize += UNIT_SIZE - ( aSize % UNIT_SIZE );

  /* Take in account the 'Size' field at the begin of every used block. */
  aSize += UNIT_SIZE;
  
  /* The allocated block has to be as big as the smallest free block */
  if ( aSize < MIN_FREE_BLOCK_SIZE )
    aSize = MIN_FREE_BLOCK_SIZE;

  /* Clasify the alloc request. Large or small object? */
  isLarge = ( aSize >= LargeObjectSize );

  /* Trying to allocate a large object? Then start with the last pool.
     The pools are searched starting with the end of the pool memory. */
  if ( isLarge )
    for ( pool = LastPool, block = 0; pool; pool = pool->Prev )
    {
      /* Staying in the large area, try to find the best fitting block */
      block = FindPrevBestFit( aSize, pool->LastFree, pool->Separator );

      /* Not found. Look in the small objects area for the first fitting 
         block */
      if ( block && ( block <= pool->Separator ))
        block = FindPrevFirstFit( aSize, block );

      if ( block )
        break;
    }
  
  /* Trying to allocate a small object? Then start with the first pool.
     The pools are searched starting with the begin of the pool memory. */
  if ( !isLarge )
    for ( pool = FirstPool, block = 0; pool; pool = pool->Next )
      if (( block = FindNextFirstFit( aSize, pool->FirstFree )) != 0 )
        break;

  /* No pool has enough memory for this alloc request. */
  if ( !block || !pool )
    return 0;

  /* At first assume, the entire block will be used. Thus, if there is 
     another block following it, remove its 'prev free' status. */
  if (( blockN = NEXT_BLOCK( block )) < pool->Guard )
    blockN->Size &= ~STATUS_BLOCK_PREV_FREE;

  /* Remove the complete block from the list of free block */
  if (( blockN = block->Next ) != 0 ) blockN->Prev    = block->Prev;
  else                                pool->LastFree  = block->Prev;
  if (( blockP = block->Prev ) != 0 ) blockP->Next    = block->Next;
  else                                pool->FirstFree = block->Next;

  /* Adapt the statistic information */
  pool->FreeSize -= block->Size - UNIT_SIZE;
  pool->NoOfFreeBlocks--;

  /* Not entire block is needed - the waste at the block's end will 
     return to the list of free blocks. */
  if ((( block->Size - aSize ) >= MIN_FREE_BLOCK_SIZE ) && !isLarge )
  {
    blockW       = (XMemoryBlock*)((char*)block + aSize );
    blockW->Size = block->Size - aSize;
    block->Size  = aSize;
    block->Next  = 0;
    block->Prev  = 0;
  }

  /* Not entire block is needed - the waste at the block's begin will 
     return to the list of free blocks. */
  if ((( block->Size - aSize ) >= MIN_FREE_BLOCK_SIZE ) && isLarge )
  {
    blockW        = block;
    block         = (XMemoryBlock*)((char*)block + block->Size - aSize );
    block->Size   = aSize;
    blockW->Size -= aSize;
  }

  /* Add the waste block back to the list containing the free blocks */
  if ( blockW )
  {
    blockW->Prev = blockP;
    blockW->Next = blockN;
    if ( blockP ) blockP->Next = blockW; else pool->FirstFree = blockW;
    if ( blockN ) blockN->Prev = blockW; else pool->LastFree  = blockW;

    /* Every free block maintains at its end a link to itself.  */
    blockN = NEXT_BLOCK( blockW );
    blockN[-1].Prev = blockW;

    /* Also the next following block has to be marked as 'prev free'. */
    if ( blockN < pool->Guard )
      blockN->Size |= STATUS_BLOCK_PREV_FREE;

    /* Adapt the statistic information */
    pool->FreeSize += blockW->Size - UNIT_SIZE;
    pool->NoOfFreeBlocks++;
  }

  /* Mark the found block as allocated and ... */
  block->Size |= STATUS_BLOCK_USED;

  /* ... eventually as following a free block */
  if ( blockW && ( blockW < block ))
    block->Size |= STATUS_BLOCK_PREV_FREE;
    
  /* Adapt the statistic information */
  if ( isLarge ) pool->LargeAllocCounter++;
  else           pool->SmallAllocCounter++;
  pool->NoOfUsedBlocks++;

  /* Adjust the border separating the small/large objects. Every time a new 
     small object is allocated, the border is moved */
  if ( !isLarge && ( block > pool->Separator ))
    pool->Separator = block;

  /* ... in turn, if large objects are allocated the border is adjusted to be
     between the previous border and the just allocated object */
  if (  isLarge && ( block < pool->Separator ))
    pool->Separator = block + (( pool->Separator - block ) >> 1 );

  /* The pointer to the first user data byte within the allocated memory 
     block. */
  return (char*)block + UNIT_SIZE;
}


/*******************************************************************************
* FUNCTION:
*   EwFreeHeapBlock
*
* DESCRIPTION:
*   The function EwFreeHeapBlock() releases the memory allocated by a preceding
*   call to the method EwAllocHeapBlock().
*
* ARGUMENTS:
*   aMemory - Pointer to the memory block to release.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeHeapBlock( void* aMemory )
{
  XMemoryPool*  pool  = FirstPool;
  XMemoryBlock* block = (XMemoryBlock*)((char*)aMemory - UNIT_SIZE );

  /* Nothing to free or the pointer is not valid */
  if ( !aMemory )
    return;

  /* Nothing to free or the pointer is not valid */
  if ((unsigned long)aMemory % UNIT_SIZE )
  {
    EwErrorPD( 323, aMemory, 0 );
    return;
  }

  /* Traverse the list of existing memory pools trying to find a pool containing
     the specified memory block */
  for ( ; pool; pool = pool->Next )
  {
    XMemoryBlock* blockN;
    XMemoryBlock* blockP;

    /* Does the memory belong to this pool? If not continue with next pool. */
    if (( block < pool->Start ) || ( block >= pool->Guard ))
      continue;

    /* Is the block already freed? */
    if ( !( block->Size & STATUS_BLOCK_USED ))
    {
      EwErrorPD( 324, aMemory, 0 );
      return;
    }

    /* Knowing the size of the block get the address of the next following 
       block. Similarly, if the precing blockm is known as free, get its 
       address too */
    blockN = NEXT_BLOCK( block );
    blockP = ( block->Size & STATUS_BLOCK_PREV_FREE )? block[-1].Prev : 0;

    /* The last block in the pool? */
    if ( blockN == pool->Guard )
      blockN = 0;

    /* Is the estimated address of the next block valid? */
    if ( blockN && (( blockN <= block ) || ( blockN > pool->Guard )))
    {
      EwErrorPD( 325, aMemory, 0 );
      return;
    }

    /* Is the next block valid? (It can has the status 'prev free' */
    if ( blockN && ( blockN->Size & STATUS_BLOCK_PREV_FREE ))
    {
      EwErrorPD( 326, aMemory, 0 );
      return;
    }

    /* Is the estimated address of the prev block valid? */
    if ( blockP && (( blockP >= block ) || ( blockP < pool->Start ) ||
       ((unsigned long)blockP % UNIT_SIZE )))
    {
      EwErrorPD( 327, aMemory, 0 );
      return;
    }
    
    /* Is the prev block really free and its size is valid? */
    if ( blockP && (( blockP->Size & STATUS_BLOCK_ANY ) ||  
       ( NEXT_BLOCK( blockP ) != block )))
    {
      EwErrorPD( 328, aMemory, 0 );
      return;
    }

    /* Releasing a block enclosed between two free blocks? Accordingly two
       free and one occupied blocks are merged together */
    if ( blockP && blockN && !( blockN->Size & STATUS_BLOCK_USED ))
    {
      /* Adapt the statistic information */
      pool->FreeSize += BLOCK_SIZE( block ) + UNIT_SIZE;
      pool->NoOfFreeBlocks--;
      pool->NoOfUsedBlocks--;
      pool->FreeCounter++;

      /* Merge the blocks together */
      blockP->Next = blockN->Next;
      if ( blockN->Next ) blockN->Next->Prev = blockP;
      else                pool->LastFree     = blockP;

      /* The resulting size of the merged block. Also, the last entry in every
         free block has to point to the origin of the block */
      blockP->Size  += BLOCK_SIZE( block ) + blockN->Size;
      NEXT_BLOCK( blockP )[-1].Prev = blockP;

      /* Discard info associated with the merged blocks */
      blockN->Size   = 0; blockN->Next = 0; blockN->Prev = 0;
      block->Size    = 0; block->Next  = 0; block->Prev  = 0;
      block[-1].Prev = 0;
    }

    /* The block can be merged with the preceding block. */
    else if ( blockP )
    {
      /* Adapt the statistic information */
      pool->FreeSize += BLOCK_SIZE( block );
      pool->NoOfUsedBlocks--;
      pool->FreeCounter++;

      /* The resulting size of the merged block. Also, the last entry in every
         free block has to point to the origin of the block */
      blockP->Size += BLOCK_SIZE( block );
      NEXT_BLOCK( blockP )[-1].Prev = blockP;

      /* Discard info associated with the merged blocks */
      block->Size    = 0; block->Next = 0; block->Prev = 0;
      block[-1].Prev = 0;

      /* The next following block has now the status 'prev free' */
      if ( blockN )
        blockN->Size |= STATUS_BLOCK_PREV_FREE;
    }

    /* The block can be merged with the next following block */
    else if ( blockN && !( blockN->Size & STATUS_BLOCK_USED ))
    {
      /* Adapt the statistic information */
      pool->FreeSize += BLOCK_SIZE( block );
      pool->NoOfUsedBlocks--;
      pool->FreeCounter++;

      /* Merge the blocks together */
      if ( blockN->Prev ) blockN->Prev->Next = block;
      else                pool->FirstFree    = block;
      if ( blockN->Next ) blockN->Next->Prev = block;
      else                pool->LastFree     = block;
      block->Next = blockN->Next;
      block->Prev = blockN->Prev;

      /* The resulting size of the merged block. Also, the last entry in every
         free block has to point to the origin of the block */
      block->Size = blockN->Size + BLOCK_SIZE( block );
      NEXT_BLOCK( block )[-1].Prev = block;

      /* Discard info associated with the merged blocks */
      blockN->Size = 0; blockN->Next = 0; blockN->Prev = 0;
    }

    /* No merge possible. New free block is added to the list. Doing
       this expects however that we know the right position within the
       list. */
    else
    {
      /* Traverse the following blocks trying to find an empty one */
      while ( blockN && ( blockN->Size & STATUS_BLOCK_USED ))
      {
        blockP = blockN;
        blockN = NEXT_BLOCK( blockN );

        /* The last block in the pool? */
        if ( blockN == pool->Guard )
          blockN = 0;

        /* Is the estimated address of the next block valid? */
        if ( blockN && (( blockN <= blockP ) || ( blockN > pool->Guard )))
        {
          EwErrorPD( 329, aMemory, 0 );
          return;
        }

        /* Is the next block valid? (It can has the status 'prev free' */
        if ( blockN && ( blockN->Size & STATUS_BLOCK_PREV_FREE ))
        {
          EwErrorPD( 330, aMemory, 0 );
          return;
        }
      }

      /* Get the address of the free block preceding our block */
      blockP = blockN? blockN->Prev : pool->LastFree;

      /* Is the estimated address of the prev block valid? */
      if ( blockP && (( blockP >= block ) || ( blockP < pool->Start ) ||
         ((unsigned long)blockP % UNIT_SIZE )))
      {
        EwErrorPD( 331, aMemory, 0 );
        return;
      }
    
      /* Is the prev block really free and its size is valid? */
      if ( blockP && (( blockP->Size & STATUS_BLOCK_ANY ) ||  
         ( NEXT_BLOCK( blockP ) >= block )))
      {
        EwErrorPD( 332, aMemory, 0 );
        return;
      }

      /* Insert the new block after the found prev block? */
      if ( blockP )
      {
        block->Next  = blockP->Next;
        block->Prev  = blockP;
        blockP->Next = block;
 
        if ( block->Next ) block->Next->Prev = block;
        else               pool->LastFree    = block;
      }

      /* Insert the new block before the found next block? */
      else if ( blockN )
      {
        block->Next  = blockN;
        block->Prev  = blockN->Prev;
        blockN->Prev = block;

        if ( block->Prev ) block->Prev->Next = block;
        else               pool->FirstFree   = block;
      }

      /* No prev/next free block found. Verify that the entire pool is used. */
      else if ( pool->FirstFree || pool->LastFree )
      {
        EwErrorPD( 333, aMemory, 0 );
        return;
      }

      /* No prev/next free block found. */
      else
      {
        block->Next = 0;
        block->Prev = 0;
        pool->FirstFree = block;
        pool->LastFree  = block;
      }

      /* The block is not used anymore */
      block->Size &= ~STATUS_BLOCK_USED;

      /* The last entry in every free block has to point to the origin of 
         the block */
      blockN = NEXT_BLOCK( block );
      blockN[-1].Prev = block;

      /* Also, the next following block has to have the stattus 'prev free' */
      if ( blockN < pool->Guard )
        blockN->Size |= STATUS_BLOCK_PREV_FREE;

      /* Adapt the statistic information */
      pool->FreeSize += block->Size - UNIT_SIZE;
      pool->NoOfUsedBlocks--;
      pool->NoOfFreeBlocks++;
      pool->FreeCounter++;
    }
  }
}


/*******************************************************************************
* FUNCTION:
*   EwIsHeapPtr
*
* DESCRIPTION:
*   The function EwIsHeapPtr() verifies whether the given pointer aPtr refers a
*   memory area controlled by the heap manager (see EwAllocHeapBlock(), 
*   EwFreeHeapBlock()).
*
* ARGUMENTS:
*   aPtr - Address to test.
*
* RETURN VALUE:
*   EwIsHeapPtr() returns != 0 if the given pointer aPtr refers a memory area 
*   used by the heap manager. Otherwise 0 is returned.
*
*******************************************************************************/
int EwIsHeapPtr( void* aMemory )
{
  XMemoryPool* pool = FirstPool;

  /* Traverse the list of existing memory pools */
  for ( ; pool; pool = pool->Next )
    if (( aMemory >= (void*)pool->Start ) && ( aMemory < (void*)pool->Guard ))
      return 1;

  return 0;
}


/* pba */

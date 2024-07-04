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
*   This is an internal module of the Embedded Wizard Graphics Engine EWGFX. It
*   implements the core functionality of the engine:
*
*   > Surface and font glyph management. Creation and destroying of surfaces
*     and glyphs. Cache management.
*
*   > Drawing packages - so called 'issues' and the enclosed drawing 'tasks'.
*     An issue provides storage for drawing operations. This builds the base 
*     for the optimization of the drawing process. (See ewgfxtasks.h)
*
*   > State machines to control the execution of pending issues.
*
* SUBROUTINES:
*   EwInitGfxCore
*   EwDoneGfxCore
*   EwCreateSurface
*   EwCreateConstSurface
*   EwFindSurface
*   EwFreeSurface
*   EwLockSurface
*   EwUnlockSurface
*   EwCreateGlyph
*   EwFindGlyph
*   EwFreeGlyph
*   EwDiscardGlyph
*   EwLockGlyphCache
*   EwUnlockGlyphCache
*   EwInitIssue
*   EwDoneIssue
*   EwAllocTask
*   EwAllocTasks
*   EwAllocTaskData
*   EwFreeTaskData
*   EwFlushTasks
*   EwWaitForSurface
*
*******************************************************************************/

#include "ewrte.h"
#include "ewgfxdriver.h"
#include "ewgfxcore.h"
#include "ewgfxtasks.h"
#include "ewextgfx.h"
#include "ewgfxdefs.h"


/* Helper macro to simply exchange two integer values. */
#define EXCHANGE_VALUES( aValue1, aValue2 )                                    \
{                                                                              \
  int tmp = aValue1;                                                           \
  aValue1 = aValue2;                                                           \
  aValue2 = tmp;                                                               \
}


/* Pointer to the first and last surface within the surface cache. */
XSurface* EwSurfaceCacheHead      = 0;
XSurface* EwSurfaceCacheTail      = 0;
int       EwSurfaceCacheSize      = 0;
int       EwMaxSurfaceCacheSize   = EW_MAX_SURFACE_CACHE_SIZE;


/* A second surface cache used during prototyping only. It maintains surfaces
   discarded from the 'normal' cache to prevent the surfaces from being loaded
   permanently during prototyping. This cache is configured with 32 MB. It is
   not used in the target system! */
#ifdef EW_PROTOTYPING
  XSurface* EwRevivalSurfaceCacheHead    = 0;
  XSurface* EwRevivalSurfaceCacheTail    = 0;
  int       EwRevivalSurfaceCacheSize    = 0;
  int       EwMaxRevivalSurfaceCacheSize = 0x2000000;
#endif


/* Pointer to the newest and oldest glyph within the glyph cache and to the
   global glyph cache surface. */
XGlyph*   EwGlyphCacheHead        = 0;
XGlyph*   EwGlyphCacheTail        = 0;
XGlyph**  EwGlyphCacheHeads       = 0; /* Hash sorted cache lists */
XGlyph**  EwGlyphCacheTails       = 0;
XSurface* EwGlyphCacheSurface     = 0;
int       EwMaxGlyphSurfaceWidth  = EW_MAX_GLYPH_SURFACE_WIDTH;
int       EwMaxGlyphSurfaceHeight = EW_MAX_GLYPH_SURFACE_HEIGHT;

/* Following variables exist for profiling purpose only. They track the size
   of the biggest glyph used during the runtime of the application. */
int       EwMaxGlyphWidth         = 0;
int       EwMaxGlyphHeight        = 0;
int       EwNoOfGlyphs            = 0;

/* The variable EwMaxIssueTasks controls how much memory should the Graphics 
   Engine reserve as buffer to store pending drawing operations. */
int        EwMaxIssueTasks        = EW_MAX_ISSUE_TASKS;
static int IssueSize;
static int MinIssueGap;


/* Glyph cache surface allocation map as a bitfield:
   Bit '1' does correspond to an occupied surface cell.
   Bit '0' does correspond to an unused surface cell. */
unsigned int* EwGlyphCacheSurfaceMap = 0;


/* The following lists maintain all currently existing issues. The membership
   to a particular list determines indirectly the state of the affected issue.
   Following states exist:
   
   Used     - Determines all issues, which are in use but still not flushed.

   Pending  - Determines all issues, which have been flushed for execution of
              their pending drawing tasks. For all issues the tasks have been
              evaluated with the objective to detect and eliminate superfluous
              operations. The issues are just waiting for a free slot to the
              underlying graphics subsystem.

   Running  - Determines the (single) issue, which is currently performed by
              the graphics subsystem. This is the unique slot to the underlying
              graphics subsystem. In contrast to other lists, 'Running' may
              maintain only a single issue at the same time.

   Finished - Determines all issues, whose tasks have already been processed
              by the graphics subsystem. The issues are still waiting for the
              post-processing, the release of involved resources, etc.

   Ready    - Determines all issues, which are not in use. These issues can be
              reused immediately without the necessity to alloc memory. */
XIssues EwIssuesUsed      = { 0, 0 };
XIssues EwIssuesPending   = { 0, 0 };
XIssues EwIssuesRunning   = { 0, 0 };
XIssues EwIssuesFinished  = { 0, 0 };
XIssues EwIssuesReady     = { 0, 0 };


/* Memory usage profiler and debugging mode */
extern int EwResourcesMemory;
extern int EwResourcesMemoryPeak;
extern int EwObjectsMemory;
extern int EwStringsMemory;
extern int EwMemoryPeak;
extern int EwPrintGfxTasks;


/* The following variable is used during prototyping only to estimate the
   memory usage of surfaces. */
#ifdef EW_PROTOTYPING
  extern int EwSurfaceMemoryPeak;
#endif


/* Values describing the glyph surface configuration. These values are derived
   from EW_MAX_GLYPH_SURFACE_WIDTH and EW_MAX_GLYPH_SURFACE_HEIGHT. */
static int GlyphCacheCellSize;
static int NoOfGlyphCacheCols;
static int NoOfGlyphCacheRows;
static int NoOfGlyphCacheLists;


/* This function is invoked when the surface cache overflows for each surface
   which although being actually owned could be restored again. In the function
   the owner of the surface has to clear or references to the given surface as
   it were freed. In such case the function has to return != 0. If the owner
   don't want to discard the surface, the function has to return 0. aUrgency
   parameter can assume values between 0 .. 2 and determines the 'urgency'
   of the operation. */
extern int EwCanDiscardSurface( XSurface* aSurface, int aUrgency );


/* The following helper function stores the passed surface at the begin of the
   global surface cache. */
static void CacheSurface( XSurface* aSurface );


/* The following helper function removes the passed surface from the global
   surface cache. */
static void UncacheSurface( XSurface* aSurface );


/* Following functions manage a second surface cache used during prototyping
   to maintain discarded surfaces which eventually may become needed again. */
#ifdef EW_PROTOTYPING
  static void CacheRevivalSurface( XSurface* aSurface );
  static void UncacheRevivalSurface( XSurface* aSurface );
#endif


/* The following helper function has the job to discard the given surface. This
   operation will cause the release of all resources occupied by the surface. */
static void DiscardSurface( XSurface* aSurface );


/* The following helper function stores the passed glyph at the begin of the
   global glyph cache. */
static void CacheGlyph( XGlyph* aGlyph );


/* The following helper function removes the passed glyph from the global glyph
   cache. */
static void UncacheGlyph( XGlyph* aGlyph );


/* The following helper function has the job to discard the given glyph. This
   operation will cause the relase of all resources occupied by the glyph. */
static void DiscardGlyph( XGlyph* aGlyph );


/* The following helper function searches within the glyph cache allocation map
   for a free area to accommodate a glyph with the given size. If successful,
   the function returns the origin of the area in the arguments aRow and 
   aColumn and returns != 0. */
static int FindFreeGlyphCacheArea( int aNoOfColumns, int aNoOfRows,
  int* aColumn, int* aRow );


/* The following helper function marks the given area within the glyph cache
   allocation map as occupied. */
static void ReserveGlyphCacheArea( int aColumn, int aRow, int aNoOfColumns, 
  int aNoOfRows );


/* The following helper function marks the given area within the glyph cache
   allocation map as unused. */
static void ReleaseGlyphCacheArea( int aColumn, int aRow, int aNoOfColumns, 
  int aNoOfRows );


/* The following helper function has the job to enqueue the passed issue within
   the given issues list. */
static void EnqueueIssue( XIssue* aIssue, XIssues* aList );


/* The following helper function builds the counterpart to the EnqueueIssue().
   It dequeues the issue from its issues list. */
static void DequeueIssue( XIssue* aIssue );


/* The following helper function has the job to discard the memory occupied by
   the issue. */
static void DiscardIssue( XIssue* aIssue );


/* The following helper function drives the state machine of the engine. The
   function starts new issues and cleans finished issues. */
static void ProcessIssues( void );


/* The following helper function flushes pending issues and releases unused
   surface, glyphs, etc. The function tries to release max. the given number
   of surfaces and/or glyphs. */
static int FlushAndClean( int aSurfaces, int aGlyphs );


/* The following heper function verifies whether the memory size of all surfaces
   currently stored in the surface cache exceeds the predefined treshold. In
   this case, the function tries to discard unused surfaces. If the parameter
   aForce != 0 all unused surfaces are freed. The function returns != 0 if it
   could at least free one surface.*/
static int CleanSurfaceCache( int aForce );


/* Performance counter for performance investigation and debugging only */
EW_PERF_COUNTER( EwGfxAdjustSurfaceSize, Platform_Integration_API )
EW_PERF_COUNTER( EwGfxCreateSurface,     Platform_Integration_API )
EW_PERF_COUNTER( EwGfxCreateConstSurface,Platform_Integration_API )
EW_PERF_COUNTER( EwGfxGetSurfaceMemSize, Platform_Integration_API )
EW_PERF_COUNTER( EwGfxLockSurface,       Platform_Integration_API )
EW_PERF_COUNTER( EwGfxUnlockSurface,     Platform_Integration_API )
EW_PERF_COUNTER( EwGfxDestroySurface,    Platform_Integration_API )


/*******************************************************************************
* FUNCTION:
*   EwInitGfxCore
*
* DESCRIPTION:
*   The function EwInitGfxCore() initializes the Graphics Engine core module.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   If successful, returns != 0.
*
*******************************************************************************/
int EwInitGfxCore( void )
{
  static const XIssues NoIssues  = { 0, 0 };
  int                  sizeOfMap;
  int                  area;

  /* First clear all pointers */
  EwSurfaceCacheHead     = 0;
  EwSurfaceCacheTail     = 0;
  EwSurfaceCacheSize     = 0;
  EwGlyphCacheHead       = 0;
  EwGlyphCacheTail       = 0;
  EwGlyphCacheHeads      = 0;
  EwGlyphCacheTails      = 0;
  EwGlyphCacheSurface    = 0;
  EwGlyphCacheSurfaceMap = 0;
  NoOfGlyphCacheLists    = 0;
  EwMaxGlyphWidth        = 0;
  EwMaxGlyphHeight       = 0;
  EwNoOfGlyphs           = 0;
  EwIssuesUsed           = NoIssues;
  EwIssuesPending        = NoIssues;
  EwIssuesRunning        = NoIssues;
  EwIssuesFinished       = NoIssues;
  EwIssuesReady          = NoIssues;

  /* Prepare performance counters */
  EwAddPerfCounter( EwGfxCreateSurface      );
  EwAddPerfCounter( EwGfxCreateConstSurface );
  EwAddPerfCounter( EwGfxDestroySurface     );
  EwAddPerfCounter( EwGfxAdjustSurfaceSize  );
  EwAddPerfCounter( EwGfxGetSurfaceMemSize  );
  EwAddPerfCounter( EwGfxLockSurface        );
  EwAddPerfCounter( EwGfxUnlockSurface      );

  /* Respect configuration limits */
  if ( EwMaxIssueTasks < 1    ) EwMaxIssueTasks = 1;
  if ( EwMaxIssueTasks > 8192 ) EwMaxIssueTasks = 8192;
  if ( EwMaxGlyphSurfaceWidth  < 32 ) EwMaxGlyphSurfaceWidth  = 32;
  if ( EwMaxGlyphSurfaceHeight < 32 ) EwMaxGlyphSurfaceHeight = 32;

  /* From the max. number of tasks calculate the size of an issue buffer */
  IssueSize   = ( EwMaxIssueTasks * 64 ) + 512;
  MinIssueGap =   IssueSize / 16;

  /* Limit the gap to not become too large with a big issue buffer */
  if ( MinIssueGap > 8192 )
    MinIssueGap = 8192;

  /* Ensure the gap is large enough for at least one drawing operation.
     Actually the biggest one ist 76 Bytes */
  if ( MinIssueGap < 128 )
    MinIssueGap = 128;

  /* Derive the optimal granularity for the glyph cache surface in the
     horizontal direction. */
  if      ( EwMaxGlyphSurfaceHeight <= 512  ) NoOfGlyphCacheRows = 32;
  else if ( EwMaxGlyphSurfaceHeight <= 1024 ) NoOfGlyphCacheRows = 64;
  else if ( EwMaxGlyphSurfaceHeight <= 2048 ) NoOfGlyphCacheRows = 128;
  else                                        NoOfGlyphCacheRows = 256;

  /* The granularity determines the size of a single glyph surface cell
     and the resulting number of columns within the cache */
  GlyphCacheCellSize = ( EwMaxGlyphSurfaceHeight + NoOfGlyphCacheRows - 1 ) /
                         NoOfGlyphCacheRows;
  NoOfGlyphCacheCols = EwMaxGlyphSurfaceWidth  / GlyphCacheCellSize;
  sizeOfMap          = ( NoOfGlyphCacheCols * NoOfGlyphCacheRows ) / 8;

  /* Adjust the size of the surface to be multiple of the estimated cell */
  EwMaxGlyphSurfaceHeight = GlyphCacheCellSize * NoOfGlyphCacheRows;
  EwMaxGlyphSurfaceWidth  = GlyphCacheCellSize * NoOfGlyphCacheCols;

  /* By taking in account the area of the glyph cache surface, ... */
  area = EwMaxGlyphSurfaceWidth * EwMaxGlyphSurfaceHeight;

  /* ... estimate a number of lists to sort the cached glyphs ... The
     bigger the area, the more glyphs can be retained in the cache.
     The more glyphs, more lists are needed to avoid long searching 
     operations. The number of the list is a hash extracted from the
     character code by using a mask */
  if      ( area >= 0x80000 ) NoOfGlyphCacheLists = 256;
  else if ( area >= 0x40000 ) NoOfGlyphCacheLists = 128;
  else if ( area >= 0x20000 ) NoOfGlyphCacheLists = 64;
  else if ( area >= 0x10000 ) NoOfGlyphCacheLists = 32;
  else if ( area >= 0x08000 ) NoOfGlyphCacheLists = 16;
  else if ( area >= 0x04000 ) NoOfGlyphCacheLists = 8;
  else if ( area >= 0x02000 ) NoOfGlyphCacheLists = 4;
  else                        NoOfGlyphCacheLists = 2;

  /* Set of up to 256 lists to store glyphs sorted by their character code */
  EwGlyphCacheHeads = EwAlloc( sizeof( XGlyph* ) * NoOfGlyphCacheLists );
  EwGlyphCacheTails = EwAlloc( sizeof( XGlyph* ) * NoOfGlyphCacheLists );

  /* Get memory for the glyph cache surface allocation map and create this
     surface. */
  EwGlyphCacheSurfaceMap = EwAlloc( sizeOfMap );
  EwGlyphCacheSurface    = EwCreateSurface( EW_PIXEL_FORMAT_ALPHA8, 
    NoOfGlyphCacheCols * GlyphCacheCellSize,
    NoOfGlyphCacheRows * GlyphCacheCellSize, 0, 0, 0 );

  /* Could not create the surface or the memory allocation is failed ... */
  if ( !EwGlyphCacheHeads || !EwGlyphCacheTails || !EwGlyphCacheSurfaceMap ||
       !EwGlyphCacheSurface )
  {
    EwDoneGfxCore();
    return 0;
  }

  /* The glyph cache surface is not considered as a regular 'surface'. */
  UncacheSurface( EwGlyphCacheSurface ); 

  /* Ignore the memory usage increment generated by the glyph cache surface.
     During prototyping we track only the memory used by regular surfaces. */
  #ifdef EW_PROTOTYPING
    EwSurfaceMemoryPeak = 0;
  #endif

  /* Reset the allocation map and glyph cache lists */
  EwZero( EwGlyphCacheSurfaceMap, sizeOfMap );
  EwZero( EwGlyphCacheHeads, sizeof( XGlyph* ) * NoOfGlyphCacheLists );
  EwZero( EwGlyphCacheTails, sizeof( XGlyph* ) * NoOfGlyphCacheLists );

  /* Track the RAM usage */
  EwResourcesMemory += sizeof( XGlyph* ) * NoOfGlyphCacheLists * 2;
  EwResourcesMemory += sizeOfMap;

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* Successfully initialized */
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwDoneGfxCore
*
* DESCRIPTION:
*   The function EwDoneGfxCore() deinitializes the Graphics Engine core module.
*   If necessary, the function discards all existing surfaces and frees the
*   used resources.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDoneGfxCore( void )
{
  /* First at all, ensure that all flushed issues are done */
  while ( EwIssuesPending.Head || EwIssuesRunning.Head  || EwIssuesFinished.Head )
    ProcessIssues();

  /* The glyph cache surface, although a surface, is considered as special kind
     of surfaces. As such it is maintained separately outside of the surface 
     cache. Before we can destroy all surfaces put the glyph cache surface back
     to the surface cache. */
  if ( EwGlyphCacheSurface )
    CacheSurface( EwGlyphCacheSurface );

  /* Now, no other concurrent tasks are running. Discard all existing glyphs,
     surfaces, issues */
  while ( EwGlyphCacheHead      ) DiscardGlyph  ( EwGlyphCacheHead      );

  /* During prototyping we manage unused surfaces in a second large cache.
     Before shutting down the Graphics Enginen move all surfaces from this
     cache to the regular cache */
  #ifdef EW_PROTOTYPING
    while ( EwRevivalSurfaceCacheHead )
    {
      XSurface* tmp = EwRevivalSurfaceCacheHead;
      UncacheRevivalSurface( EwRevivalSurfaceCacheHead );
      CacheSurface( tmp );
    }
  #endif

  while ( EwSurfaceCacheHead    ) DiscardSurface( EwSurfaceCacheHead    );
  while ( EwIssuesUsed.Head     ) DiscardIssue  ( EwIssuesUsed.Head     );
  while ( EwIssuesPending.Head  ) DiscardIssue  ( EwIssuesPending.Head  );
  while ( EwIssuesRunning.Head  ) DiscardIssue  ( EwIssuesRunning.Head  );
  while ( EwIssuesFinished.Head ) DiscardIssue  ( EwIssuesFinished.Head );
  while ( EwIssuesReady.Head    ) DiscardIssue  ( EwIssuesReady.Head    );

  /* Also release the internal glyph surface allocation map and other lists */
  if ( EwGlyphCacheSurfaceMap ) EwFree( EwGlyphCacheSurfaceMap );
  if ( EwGlyphCacheHeads      ) EwFree( EwGlyphCacheHeads      );
  if ( EwGlyphCacheTails      ) EwFree( EwGlyphCacheTails      );

  /* Dissolve the references to the freed resources */
  EwGlyphCacheSurfaceMap = 0;
  EwGlyphCacheHeads      = 0;
  EwGlyphCacheTails      = 0;
  EwGlyphCacheSurface    = 0;

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XGlyph* ) * NoOfGlyphCacheLists * 2;
  EwResourcesMemory -= ( NoOfGlyphCacheCols * NoOfGlyphCacheRows ) / 8;
}


/*******************************************************************************
* FUNCTION:
*   EwCreateSurface
*
* DESCRIPTION:
*   The function EwCreateSurface() creates a new surface with the given size and
*   pixel format. The additional tag values serve for identification purpose and
*   simplifies the reuse of previously freed surfaces -> see the EwFindSurface()
*   function.
*
*   In order to create a new surface, the function forwards the request to the
*   underlying graphics subsystem. If the request could not be handled by the
*   subsystem due to a resource deficit, the function tries to flush the caches
*   and forces the processing of pending drawing operations until the subsystem
*   can handle the request sucessfully. If all the adopted measures didn't help
*   to handle the request, the function fails and returns null.
*
*   The newly created surface passes into the ownership of the caller of this
*   function. In order to release the surface, EwFreeSurface() should be used.
*
* ARGUMENTS:
*   aFormat - Pixel format of the desired surface. (See EW_PIXEL_FORMAT_XXX).
*   aWidth,
*   aHeight - Size of the surface in pixel.
*   aOwner  - The owner in context of which the surface will be stored.
*   aTag1,
*   aTag2   - Unique identification for the surface.
*
* RETURN VALUE:
*   If successful, EwCreateSurface() returns the new surface otherwise null is
*   returned.
*
*******************************************************************************/
XSurface* EwCreateSurface( int aFormat, int aWidth, int aHeight, void* aOwner, 
  unsigned long aTag1, unsigned long aTag2 )
{
  XSurface*     surface = 0;
  unsigned long handle  = 0;
  int           memSize = 0;

  /* It is not possible to create screen surfaces (frame buffers) dynamically
     at the runtime. */
  if ( aFormat == EW_PIXEL_FORMAT_SCREEN )
    return 0;

  /* Every invocation of this API should drive the state machine and discard
     unused surfaces */
  ProcessIssues();
  CleanSurfaceCache( 0 );

  /* If surfaces are stored in rotated orientation, exchange the coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* Depending on the target system, the real size of a surface may result
     larger than the requested size. This to know may be useful later, when
     unused surfaces are searched in the cache */
  EwStartPerfCounter( EwGfxAdjustSurfaceSize );
  {
    #ifdef EwGfxAdjustNativeSurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_NATIVE )
        EwGfxAdjustNativeSurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustAlpha8SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )
        EwGfxAdjustAlpha8SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustIndex8SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_INDEX8 )
        EwGfxAdjustIndex8SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustRGB565SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_RGB565 )
        EwGfxAdjustRGB565SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif
  }
  EwStopPerfCounter( EwGfxAdjustSurfaceSize );

  /* First, get memory for the surface structure. If not possible, try to free
     the still unused resources and repeat */
  do
    surface = EwAlloc( sizeof( XSurface ));
  while ( !surface && ( FlushAndClean( 1, 1 ) || EwImmediateReclaimMemory( 37 )));

  /* Out of memory */
  if ( !surface )
  {
    EwError( 37 );
    return 0;
  }

  EwZero( surface, sizeof( XSurface ));

  /* In the second step, ask the underlying subsystem for a 'real' surface.
     If failed, try to free unused resources and repeat */
  do
  {
    EwStartPerfCounter( EwGfxCreateSurface );
    {
      if ( aFormat == EW_PIXEL_FORMAT_NATIVE )
        #ifdef EwGfxCreateNativeSurface
          handle = EwGfxCreateNativeSurface( aFormat, aWidth, aHeight );
        #else
          handle = EwCreateNativeSurface( aWidth, aHeight );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )
        #ifdef EwGfxCreateAlpha8Surface
          handle = EwGfxCreateAlpha8Surface( aFormat, aWidth, aHeight );
        #else
          handle = EwCreateAlpha8Surface( aWidth, aHeight );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_INDEX8 )
        #ifdef EwGfxCreateIndex8Surface
          handle = EwGfxCreateIndex8Surface( aFormat, aWidth, aHeight );
        #else
          handle = EwCreateIndex8Surface( aWidth, aHeight );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_RGB565 )
        #ifdef EwGfxCreateRGB565Surface
          handle = EwGfxCreateRGB565Surface( aFormat, aWidth, aHeight );
        #else
          handle = EwCreateRGB565Surface( aWidth, aHeight );
        #endif
    }
    EwStopPerfCounter( EwGfxCreateSurface );
  }
  while ( !handle && FlushAndClean( 1, 0 ));

  /* No video resources available for the surface. */
  if ( !handle )
  {
    EwFree( surface );
    return 0;
  }
  
  /* Determine the memory consuption of the surface with the given size */
  EwStartPerfCounter( EwGfxGetSurfaceMemSize );
  {
    if ( aFormat == EW_PIXEL_FORMAT_NATIVE )
      #ifdef EwGfxGetNativeSurfaceMemSize
        memSize = EwGfxGetNativeSurfaceMemSize( aFormat, aWidth, aHeight );
      #else
        memSize = EwGetNativeSurfaceMemSize( aWidth, aHeight );
      #endif

    if ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )
      #ifdef EwGfxGetAlpha8SurfaceMemSize
        memSize = EwGfxGetAlpha8SurfaceMemSize( aFormat, aWidth, aHeight );
      #else
        memSize = EwGetAlpha8SurfaceMemSize( aWidth, aHeight );
      #endif

    if ( aFormat == EW_PIXEL_FORMAT_INDEX8 )
      #ifdef EwGfxGetIndex8SurfaceMemSize
        memSize = EwGfxGetIndex8SurfaceMemSize( aFormat, aWidth, aHeight );
      #else
        memSize = EwGetIndex8SurfaceMemSize( aWidth, aHeight );
      #endif

    if ( aFormat == EW_PIXEL_FORMAT_RGB565 )
      #ifdef EwGfxGetRGB565SurfaceMemSize
        memSize = EwGfxGetRGB565SurfaceMemSize( aFormat, aWidth, aHeight );
      #else
        memSize = EwGetRGB565SurfaceMemSize( aWidth, aHeight );
      #endif
  }
  EwStopPerfCounter( EwGfxGetSurfaceMemSize );

  /* If surfaces are stored in rotated orientation, restore the original
     coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* Complete the creation and ... */
  surface->Tag1    = aTag1;
  surface->Tag2    = aTag2;
  surface->Format  = (short)aFormat;
  surface->Width   = (short)aWidth;
  surface->Height  = (short)aHeight;
  surface->MemSize = memSize;
  surface->Handle  = handle;

  /* Does it make sense to retain the surface in cache after being freed? */
  #ifdef EW_CACHE_OFFSCREEN_SURFACES  
    surface->DoCache = 1;
  #else  
    surface->DoCache = aTag1 || aTag2;
  #endif

  /* Pass the new surface to the surface cache */
  CacheSurface( surface );

  /* The new surface passes into the ownership of the caller */
  surface->Owner = aOwner;
  surface->Owned++;
  surface->Used++;

  /* May be due to this surface creation, the max. cache is exceeded. Discard
     unused surfaces. Avoid that the just created surface is discarded too.
     Sign this surface as if had a second owner. */
  surface->Owned++;
  surface->Used++;
  CleanSurfaceCache( 0 );
  surface->Used--;
  surface->Owned--;

  /* Track the RAM usage */
  EwResourcesMemory += sizeof( XSurface );

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* During prototyping track the memory usage of surfaces */
  #ifdef EW_PROTOTYPING
    if ( EwSurfaceCacheSize > EwSurfaceMemoryPeak )
      EwSurfaceMemoryPeak = EwSurfaceCacheSize;
  #endif

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "CreateSurface( %s, %d, %d ) --> 0x%p\n", 
               ( aFormat == EW_PIXEL_FORMAT_NATIVE )? "NATIVE" :
               ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )? "ALPHA8" :
               ( aFormat == EW_PIXEL_FORMAT_RGB565 )? "RGB565" : "INDEX8",
               aWidth, aHeight, surface );
  #endif

  return surface;
}


/*******************************************************************************
* FUNCTION:
*   EwCreateConstSurface
*
* DESCRIPTION:
*   The function EwCreateConstSurface() creates a new surface with the given 
*   size, pixel format and pixel information. Important here is the fact that
*   the function doesn't reserve any memory to store the pixel information.
*   Instead it, the surface associates the pixel data passed in the parameter
*   aMemory.
*
*   The additional tag values serve for identification purpose and simplifies
*   the reuse of previously freed surfaces -> see the EwFindSurface() function.
*
*   In order to create a new surface, the function forwards the request to the
*   underlying graphics subsystem. If the request could not be handled by the
*   subsystem due to a resource deficit, the function tries to flush the caches
*   and forces the processing of pending drawing operations until the subsystem
*   can handle the request sucessfully. If all the adopted measures didn't help
*   to handle the request, the function fails and returns null.
*
*   The newly created surface passes into the ownership of the caller of this
*   function. In order to release the surface, EwFreeSurface() should be used.
*
* ARGUMENTS:
*   aFormat - Pixel format of the desired surface. (See EW_PIXEL_FORMAT_XXX).
*   aWidth,
*   aHeight - Size of the surface in pixel.
*   aOwner  - The owner in context of which the surface will be stored.
*   aTag1,
*   aTag2   - Unique identification for the surface.
*   aMemory - Structure to pass the ROM address information.
*
* RETURN VALUE:
*   If successful, EwCreateSurface() returns the new surface otherwise null is
*   returned.
*
*******************************************************************************/
XSurface* EwCreateConstSurface( int aFormat, int aWidth, int aHeight,
  void* aOwner, unsigned long aTag1, unsigned long aTag2, 
  XSurfaceMemory* aMemory )
{
  XSurface*     surface = 0;
  unsigned long handle  = 0;

  /* It is not possible to create screen surfaces (frame buffers) dynamically
     at the runtime. */
  if ( aFormat == EW_PIXEL_FORMAT_SCREEN )
    return 0;

  /* Every invocation of this API should drive the state machine and discard
     unused surfaces */
  ProcessIssues();
  CleanSurfaceCache( 0 );

  /* If surfaces are stored in rotated orientation, exchange the coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* First, get memory for the surface structure. If not possible, try to free
     the still unused resources and repeat */
  do
    surface = EwAlloc( sizeof( XSurface ));
  while ( !surface && ( FlushAndClean( 1, 1 ) || EwImmediateReclaimMemory( 38 )));

  /* Out of memory */
  if ( !surface )
  {
    EwError( 38 );
    return 0;
  }

  EwZero( surface, sizeof( XSurface ));

  /* In the second step, ask the underlying subsystem for a 'real' surface.
     If failed, try to free unused resources and repeat */
  do
  {
    EwStartPerfCounter( EwGfxCreateConstSurface );
    {
      if ( aFormat == EW_PIXEL_FORMAT_NATIVE )
        #ifdef EwGfxCreateNativeSurface
          #ifdef EwGfxCreateConstNativeSurface
            handle = EwGfxCreateConstNativeSurface( aFormat, aWidth, aHeight, 
                                                    aMemory );
          #else
          {
            EwError( 203 );
            EwPanic();
            break;
          }
          #endif
        #else
          handle = EwCreateConstNativeSurface( aWidth, aHeight, aMemory );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )
        #ifdef EwGfxCreateAlpha8Surface
          #ifdef EwGfxCreateConstAlpha8Surface
            handle = EwGfxCreateConstAlpha8Surface( aFormat, aWidth, aHeight, 
                                                    aMemory );
          #else
          {
            EwError( 204 );
            EwPanic();
            break;
          }
          #endif
        #else
          handle = EwCreateConstAlpha8Surface( aWidth, aHeight, aMemory );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_INDEX8 )
        #ifdef EwGfxCreateIndex8Surface
          #ifdef EwGfxCreateConstIndex8Surface
            handle = EwGfxCreateConstIndex8Surface( aFormat, aWidth, aHeight,
                                                    aMemory );
          #else
          {
            EwError( 205 );
            EwPanic();
            break;
          }
          #endif
        #else
          handle = EwCreateConstIndex8Surface( aWidth, aHeight, aMemory );
        #endif

      if ( aFormat == EW_PIXEL_FORMAT_RGB565 )
        #ifdef EwGfxCreateRGB565Surface
          #ifdef EwGfxCreateConstRGB565Surface
            handle = EwGfxCreateConstRGB565Surface( aFormat, aWidth, aHeight,
                                                    aMemory );
          #else
          {
            EwError( 206 );
            EwPanic();
            break;
          }
          #endif
        #else
          handle = EwCreateConstRGB565Surface( aWidth, aHeight, aMemory );
        #endif
    }
    EwStopPerfCounter( EwGfxCreateConstSurface );
  }
  while ( !handle && FlushAndClean( 1, 0 ));

  /* No video resources available for the surface. */
  if ( !handle )
  {
    EwFree( surface );
    return 0;
  }
  
  /* If surfaces are stored in rotated orientation, restore the original
     coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* Complete the creation and ... */
  surface->Tag1    = aTag1;
  surface->Tag2    = aTag2;
  surface->Format  = (short)aFormat;
  surface->Width   = (short)aWidth;
  surface->Height  = (short)aHeight;
  surface->MemSize = 0;
  surface->Handle  = handle;
  surface->DoCache = aTag1 || aTag2;

  /* Pass the new surface to the surface cache */
  CacheSurface( surface );

  /* The new surface passes into the ownership of the caller */
  surface->Owner = aOwner;
  surface->Owned++;
  surface->Used++;

  /* May be due to this surface creation, the max. cache is exceeded. Discard
     unused surfaces. Avoid that the just created surface is discarded too.
     Sign this surface as if had a second owner. */
  surface->Owned++;
  surface->Used++;
  CleanSurfaceCache( 0 );
  surface->Used--;
  surface->Owned--;

  /* Track the RAM usage */
  EwResourcesMemory += sizeof( XSurface );

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* During prototyping track the memory usage of surfaces */
  #ifdef EW_PROTOTYPING
    if ( EwSurfaceCacheSize > EwSurfaceMemoryPeak )
      EwSurfaceMemoryPeak = EwSurfaceCacheSize;
  #endif

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "CreateConstSurface( %s, %d, %d ) --> 0x%p\n", 
               ( aFormat == EW_PIXEL_FORMAT_NATIVE )? "NATIVE" :
               ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )? "ALPHA8" :
               ( aFormat == EW_PIXEL_FORMAT_RGB565 )? "RGB565" : "INDEX8",
               aWidth, aHeight, surface );
  #endif

  return surface;
}


/*******************************************************************************
* FUNCTION:
*   EwFindSurface
*
* DESCRIPTION:
*   The function EwFindSurface() searches the cache for an unused surface with
*   the given parameters and if successfull, reactivates it. If no surface
*   matches these parameters, the function fails and returns null.
*   
*   The found surface passes to the ownership of the caller of this function.
*   In order to release the surface, EwFreeSurface() should be used.
*
* ARGUMENTS:
*   aFormat - Pixel format of the desired surface. (See EW_PIXEL_FORMAT_XXX).
*   aWidth,
*   aHeight - Size of the surface in pixel.
*   aOwner  - The owner in context of which the surface will be stored after
*     it has been found.
*   aTag1,
*   aTag2   - Unique identification for the surface.
*
* RETURN VALUE:
*   If successful, EwFindSurface() returns the found surface otherwise null is
*   returned.
*
*******************************************************************************/
XSurface* EwFindSurface( int aFormat, int aWidth, int aHeight, void* aOwner, 
  unsigned long aTag1, unsigned long aTag2 )
{
  #ifdef EW_PROTOTYPING
    XSurface* surface = EwRevivalSurfaceCacheHead;
  #else
    XSurface* surface = EwSurfaceCacheHead;
  #endif
  XSurface* used = 0;

  /* Screen surfaces will never be cached. */
  if ( aFormat == EW_PIXEL_FORMAT_SCREEN )
    return 0;

  /* Every invocation of this API should drive the state machine */
  ProcessIssues();

  /* If surfaces are stored in rotated orientation, exchange the coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* Depending on the target system, the real size of a surface may result
     larger than the requested size */
  EwStartPerfCounter( EwGfxAdjustSurfaceSize );
  {
    #ifdef EwGfxAdjustNativeSurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_NATIVE )
        EwGfxAdjustNativeSurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustAlpha8SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_ALPHA8 )
        EwGfxAdjustAlpha8SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustIndex8SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_INDEX8 )
        EwGfxAdjustIndex8SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif

    #ifdef EwGfxAdjustRGB565SurfaceSize
      if ( aFormat == EW_PIXEL_FORMAT_RGB565 )
        EwGfxAdjustRGB565SurfaceSize( aFormat, aWidth, aHeight, &aWidth,
          &aHeight );
    #endif
  }
  EwStopPerfCounter( EwGfxAdjustSurfaceSize );

  /* If surfaces are stored in rotated orientation, restore the original
     coordinates. */
  #if ( EW_SURFACE_ROTATION == 90 ) || ( EW_SURFACE_ROTATION == 270 )
    EXCHANGE_VALUES( aWidth, aHeight );
  #endif

  /* Search the cache for an adequate surface. The best hit would be an unused
     surface (not involved anymore in any pending operations). If not available,
     settle for an used but not owned surface. */
  for ( ;; )
  {
    /* Ignore all owned surfaces. Skip over all surfaces not matching the given
       attributes */
    while ( surface &&
            (
              ( surface->Owned  >  0       ) ||
              ( surface->Tag1   != aTag1   ) ||
              ( surface->Tag2   != aTag2   ) ||
              ( surface->Format != aFormat ) ||
              ( surface->Width  != aWidth  ) ||
              ( surface->Height != aHeight )
            )
          )
      surface = surface->Older;

    /* All surfaces in the cache have been evaluated or we have the best hit ->
       the unused surface */
    if ( !surface || !surface->Used )
      break;

    /* The found surface is still in use but there is no owner anymore of the
       surface -> the surface is just involved in drawing operations and can be
       assigned to a new onwer - remember it as the 'second quality' surface */
    if ( surface )
      used = surface;

    /* Continue the search operation for the best hit */
    surface = surface->Older;
  }

  /* No unused surface found - continue with the 'second quality' surface */
  if ( !surface )
    surface = used;

  /* Nothing found? */
  if ( !surface )
    return 0;

  /* Place the found surface  */
  #ifdef EW_PROTOTYPING
    UncacheRevivalSurface( surface );
  #else
    UncacheSurface( surface );
  #endif

  /* ... at the begin of the cache to optimize the next search operation. */
  CacheSurface( surface );

  /* The new surface passes into the ownership of the caller */
  surface->Owner = aOwner;
  surface->Owned++;
  surface->Used++;

  /* During prototyping track the memory usage of surfaces */
  #ifdef EW_PROTOTYPING
    if ( EwSurfaceCacheSize > EwSurfaceMemoryPeak )
      EwSurfaceMemoryPeak = EwSurfaceCacheSize;
  #endif

  return surface;
}


/*******************************************************************************
* FUNCTION:
*   EwFreeSurface
*
* DESCRIPTION:
*   The function EwFreeSurface() decrements the usage counter of the surface. If
*   not involved in any other drawing operations the surface is considered as an
*   unused surface.
*
*   Freed surfaces do remain in the cache until they are reactivated or due to a
*   resource deficit displaced from the video memory.
*
* ARGUMENTS:
*   aSurface - Surface to free.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeSurface( XSurface* aSurface )
{
  if ( aSurface->Owned <= 0 )
    return;

  aSurface->Owner = 0;
  aSurface->Owned--;
  aSurface->Used--;

  /* Sort the surface in the cache, so old unused surfaces can be detected and
     freed during CleanSurfaceCache() */
  UncacheSurface( aSurface );
  CacheSurface  ( aSurface );

  /* The surface is not intended to remain in the surface cache after being
     freed. Release the associated resources now. */
  if ( !aSurface->Used && !aSurface->DoCache )
    DiscardSurface( aSurface );

  /* Discard surfaces which have been freed in the meantime */
  CleanSurfaceCache( 0 );
}


/*******************************************************************************
* FUNCTION:
*   EwLockSurface
*
* DESCRIPTION:
*   The function EwLockSurface() has the job to lock an area of the surface and
*   to provide the caller with a pointer to the surface memory. In this manner
*   the surface memory can be read and modified. After finishing the memory
*   access, the surface should be unlocked again by calling EwUnlockSurface().
*
*   Depending on the specified mode, the function may force the Graphics Engine
*   to flush pending tasks and to wait for the surface if there are any pending
*   tasks affecting the surface. To avoid this kind of synchronization, the flag
*   EW_LOCK_DONT_WAIT should be set in the aMode parameter. Regardless of this
*   additional flag, the underlying gryphics subsystem will always perform the
*   necessary synchronisation steps in order to avoid any interferences with
*   hardware executed drawing operations.
*   
*   The function fills the passed lock structure with the lock information. Here
*   the caller will find a pointer to the pixel memory and the distance between
*   two pixel rows. The memory returned by the function is not guaranteed to be
*   the real video memory of the surface. If necessary, the function will handle
*   the access by using a shadow memory area. Therefore to limit the effort of
*   memory copy operations, the desired surface area and the access mode should
*   be specified.
*
*   If there was not possible to lock the surface, the function fails and 
*   returns zero.
*
* ARGUMENTS:
*   aSurface - Surface to obtain the direct memory access.
*   aX, aY,
*   aWidth,
*   aHeight  - Limitation for the memory access to an area.
*   aIndex,
*   aCount   - Range of CLUT entries to lock. This is for Index8 surfaces only.
*   aMode    - Desired access mode to the surface memory (see EW_LOCK_XXX).
*   aLock    - Pointer to a structure where the lock information will be stored.
*
* RETURN VALUE:
*   If successful, the function returns != 0, otherwise 0 ist returned.
*
*******************************************************************************/
int EwLockSurface( XSurface* aSurface, int aX, int aY, int aWidth, int aHeight,
  int aIndex, int aCount, unsigned short aMode, XSurfaceLock* aLock )
{
  unsigned long   handle     = aSurface->Handle;
  unsigned long   lockHandle = 0;
  XSurfaceMemory* mem        = &aLock->Memory;

  /* Convert the coordinates of the desired area by taking in account how the
     pixel are stored in the surface. */
  #if EW_SURFACE_ROTATION == 90
  {
    int tmp = aY;

    EXCHANGE_VALUES( aWidth, aHeight );
    aY = aX;
    aX = aSurface->Height - tmp - aWidth;
  }
  #endif

  #if EW_SURFACE_ROTATION == 180
    aX = aSurface->Width  - aX - aWidth;
    aY = aSurface->Height - aY - aHeight;
  #endif

  #if EW_SURFACE_ROTATION == 270
  {
    int tmp = aX;

    EXCHANGE_VALUES( aWidth, aHeight );
    aX = aY;
    aY = aSurface->Width - tmp - aHeight;
  }
  #endif

  /* From now the surface can't be discarded. This is achieved by simulating the
     were owned by a second owner */
  aSurface->Owned++;
  aSurface->Used++;

  /* Synchronize the lock operation with other drawings operations? */
  if ( !( aMode & EW_LOCK_DONT_WAIT ))
    EwWaitForSurface( aSurface,
                      aMode & ( EW_LOCK_PIXEL_WRITE | EW_LOCK_CLUT_WRITE ));

  /* Try to lock the surface and its clut (if any) */
  EwStartPerfCounter( EwGfxLockSurface );
  {
    #ifdef EW_USE_PIXEL_FORMAT_SCREEN
      if ( aSurface->Format == EW_PIXEL_FORMAT_SCREEN )
        #ifdef EwGfxLockScreenSurface
          lockHandle = EwGfxLockScreenSurface( handle, aX, aY, aWidth, aHeight, 0,
                         0, aMode & EW_LOCK_PIXEL_READ, aMode & EW_LOCK_PIXEL_WRITE,
                         0, 0, mem );
        #else
          lockHandle = EwGetScreenSurfaceMemory( handle, aX, aY, 0, 
                                                 aMode & EW_LOCK_PIXEL_WRITE, mem );
        #endif
    #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_NATIVE )
      #ifdef EwGfxLockNativeSurface
        lockHandle = EwGfxLockNativeSurface( handle, aX, aY, aWidth, aHeight, 0,
                       0, aMode & EW_LOCK_PIXEL_READ, aMode & EW_LOCK_PIXEL_WRITE,
                       0, 0, mem );
      #else
        lockHandle = EwGetNativeSurfaceMemory( handle, aX, aY, 0, 
                                               aMode & EW_LOCK_PIXEL_WRITE, mem );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_ALPHA8 )
      #ifdef EwGfxLockAlpha8Surface
        lockHandle = EwGfxLockAlpha8Surface( handle, aX, aY, aWidth, aHeight, 0,
                       0, aMode & EW_LOCK_PIXEL_READ, aMode & EW_LOCK_PIXEL_WRITE,
                       0, 0, mem );
      #else
        lockHandle = EwGetAlpha8SurfaceMemory( handle, aX, aY, 0, 
                                               aMode & EW_LOCK_PIXEL_WRITE, mem );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_INDEX8 )
      #ifdef EwGfxLockIndex8Surface
        lockHandle = EwGfxLockIndex8Surface( handle, aX, aY, aWidth, aHeight, 
                       aIndex, aCount, aMode & EW_LOCK_PIXEL_READ, 
                       aMode & EW_LOCK_PIXEL_WRITE, aMode & EW_LOCK_CLUT_READ,
                       aMode & EW_LOCK_CLUT_WRITE, mem );
      #else
        lockHandle = EwGetIndex8SurfaceMemory( handle, aX, aY, aIndex,
                                               ( aMode & EW_LOCK_PIXEL_WRITE ) ||
                                               ( aMode & EW_LOCK_CLUT_WRITE  ), mem );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_RGB565 )
      #ifdef EwGfxLockRGB565Surface
        lockHandle = EwGfxLockRGB565Surface( handle, aX, aY, aWidth, aHeight, 0,
                       0, aMode & EW_LOCK_PIXEL_READ, aMode & EW_LOCK_PIXEL_WRITE,
                       0, 0, mem );
      #else
        lockHandle = EwGetRGB565SurfaceMemory( handle, aX, aY, 0, 
                                               aMode & EW_LOCK_PIXEL_WRITE, mem );
      #endif
  }
  EwStopPerfCounter( EwGfxLockSurface );

  /* Could lock the surface? */
  if ( !lockHandle )
  {
    aSurface->Owned--;
    aSurface->Used--;
    return 0;
  }

  /* Store the entire lock information */
  aLock->X      = (short)aX;
  aLock->Y      = (short)aY;
  aLock->Width  = (short)aWidth;
  aLock->Height = (short)aHeight;
  aLock->Index  = (short)aIndex;
  aLock->Count  = (short)aCount;
  aLock->Mode   = aMode;
  aLock->Handle = lockHandle;

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "LockSurface( 0x%p )\n", aSurface );
  #endif

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwUnlockSurface
*
* DESCRIPTION:
*   The function EwUnlockSurface() provides a counterpart of EwLockSurface().
*   When called, the function releases the given lock and updates the affected
*   surface. If necessary, the made modifications are transfered to the video
*   memory of the surface.
*
* ARGUMENTS:
*   aSurface - Surface to terminate the direct memory access.
*   aLock    - Lock descriptor initialized by the previous EwLockSurface() call.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwUnlockSurface( XSurface* aSurface, XSurfaceLock* aLock )
{
  EW_UNUSED_ARG ( aSurface );

  /* The surface is not locked */
  if ( aSurface->Owned < 2 )
    return;

  /* Unlock the surface and its clut (if any) */
  EwStartPerfCounter( EwGfxUnlockSurface );
  {
    #ifdef EW_USE_PIXEL_FORMAT_SCREEN
      #ifdef EwGfxLockScreenSurface
        if ( aSurface->Format == EW_PIXEL_FORMAT_SCREEN )
          EwGfxUnlockScreenSurface( aSurface->Handle, aLock->Handle, aLock->X, 
            aLock->Y, aLock->Width, aLock->Height, 0, 0, 
            aLock->Mode & EW_LOCK_PIXEL_WRITE, 0 );
      #endif
    #endif

    #ifdef EwGfxLockNativeSurface
      if ( aSurface->Format == EW_PIXEL_FORMAT_NATIVE )
        EwGfxUnlockNativeSurface( aSurface->Handle, aLock->Handle, aLock->X, 
          aLock->Y, aLock->Width, aLock->Height, 0, 0, 
          aLock->Mode & EW_LOCK_PIXEL_WRITE, 0 );
    #endif

    #ifdef EwGfxLockAlpha8Surface
      if ( aSurface->Format == EW_PIXEL_FORMAT_ALPHA8 )
        EwGfxUnlockAlpha8Surface( aSurface->Handle, aLock->Handle, aLock->X,
          aLock->Y, aLock->Width, aLock->Height, 0, 0,
          aLock->Mode & EW_LOCK_PIXEL_WRITE, 0 );
    #endif

    #ifdef EwGfxLockIndex8Surface
      if ( aSurface->Format == EW_PIXEL_FORMAT_INDEX8 )
        EwGfxUnlockIndex8Surface( aSurface->Handle, aLock->Handle, aLock->X,
          aLock->Y, aLock->Width, aLock->Height, aLock->Index, aLock->Count,
          aLock->Mode & EW_LOCK_PIXEL_WRITE, aLock->Mode & EW_LOCK_CLUT_WRITE );
    #endif

    #ifdef EwGfxLockRGB565Surface
      if ( aSurface->Format == EW_PIXEL_FORMAT_RGB565 )
        EwGfxUnlockRGB565Surface( aSurface->Handle, aLock->Handle, aLock->X,
          aLock->Y, aLock->Width, aLock->Height, 0, 0,
          aLock->Mode & EW_LOCK_PIXEL_WRITE, 0 );
    #endif
  }
  EwStopPerfCounter( EwGfxUnlockSurface );

  /* Invalidate the lock. From now the surface is not locked any more. */
  aLock->Handle = 0;
  aSurface->Owned--;
  aSurface->Used--;

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "UnlockSurface( 0x%p )\n", aSurface );
  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwCreateGlyph
*
* DESCRIPTION:
*   The function EwCreateGlyph() creates a new glyph with the given size. The
*   both additional font and charcode values serve for identification purpose
*   and simplify the reuse of already existing glyphs.
*
*   In order to create a new glyph, EwCreateGlyph() reserves an area within the
*   global glyph cache surface. If there is no space available in this surface,
*   the function forces the processing of still pending drawing operations and
*   flushes the glyph cache. If all the adopted measures didn't help to attend
*   the allocation, the function fails and returns null.
*
*   The newly created glyph passes into the ownership of the caller. In order
*   to release the glyph, EwFreeGlyph() should be used.
*
* ARGUMENTS:
*   aWidth,
*   aHeight   - Size of the glyph in pixel.
*   aFont,
*   aCharCode - Unique identification for the glyph.
*
* RETURN VALUE:
*   If successful, EwCreateGlyph() returns the new glyph otherwise null is
*   returned.
*
*******************************************************************************/
XGlyph* EwCreateGlyph( int aWidth, int aHeight, unsigned long aFont,
  unsigned long aCharCode )
{
  /* Calculate the size of an area to alloc within the glyph cache surface.
     Should the function reserve space for an additionaly one-pixel wide border
     around the glyph? */
  #ifdef EW_BORDER_AROUND_GLYPHS
    int rows = ( aHeight + GlyphCacheCellSize + 1 ) / GlyphCacheCellSize;
    int cols = ( aWidth  + GlyphCacheCellSize + 1 ) / GlyphCacheCellSize;
  #else
    int rows = ( aHeight + GlyphCacheCellSize - 1 ) / GlyphCacheCellSize;
    int cols = ( aWidth  + GlyphCacheCellSize - 1 ) / GlyphCacheCellSize;
  #endif

  XGlyph* glyph = 0;
  int     ok;
  int     row = 0;
  int     col = 0;

  /* Every invocation of this API should drive the state machine */
  ProcessIssues();

  /* The glyph is too large for the glyph cache surface. Even if the entire
     surface were empty, the allocation can not be satisfied */
  if (( rows > NoOfGlyphCacheRows ) || ( cols > NoOfGlyphCacheCols ))
    return 0;

  do
  {
    /* First scan the glyph cache surface for an area large enough to store the
       glyph pixel data. If not possible, try to free the still unused resources
       and repeat */
    do
      ok = FindFreeGlyphCacheArea( cols, rows, &col, &row );
    while ( !ok && FlushAndClean( 0, 1 ));

    /* No space available in the glyph cache surface - all cached glyphs are in
       use. */
    if ( !ok )
      return 0;

    /* Ok, there is space free for the glyph pixel data. Get memory for the glyph
       structure. If not possible, try to free the still unused resources and
       repeat */
    do
      glyph = EwAlloc( sizeof( XGlyph ));
    while ( !glyph && FlushAndClean( 1, 1 ));
  }
  while ( !glyph && EwImmediateReclaimMemory( 39 ));

  /* Out of memory */
  if ( !glyph )
  {
    EwError( 39 );
    return 0;
  }

  EwZero( glyph, sizeof( XGlyph ));

  /* Commit the found area for the glyph */
  ReserveGlyphCacheArea( col, row, cols, rows );

  /* Initialize the new glyph */
  glyph->Font     = aFont;
  glyph->CharCode = aCharCode;
  glyph->Width    = (short)aWidth;
  glyph->Height   = (short)aHeight;
  glyph->CacheX   = (short)( col * GlyphCacheCellSize );
  glyph->CacheY   = (short)( row * GlyphCacheCellSize );

  /* Pass the new glyph to the glyph cache */
  CacheGlyph( glyph );

  /* The new glyph passes into the ownership of the caller */
  glyph->Used++;

  /* Track the biggest glyph */
  if ( aWidth  > EwMaxGlyphWidth  ) EwMaxGlyphWidth  = aWidth;
  if ( aHeight > EwMaxGlyphHeight ) EwMaxGlyphHeight = aHeight;
  EwNoOfGlyphs++;

  /* Track the RAM usage */
  EwResourcesMemory += sizeof( XGlyph );

  /* Also track the max. memory pressure */
  if ( EwResourcesMemory > EwResourcesMemoryPeak )
    EwResourcesMemoryPeak = EwResourcesMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      if (( aCharCode >= 0x20 ) && ( aCharCode <= 0x7F ))
        EwPrint( "CreateGlyph( '%c' (0x%04X), %d, %d ) --> 0x%p\n",
                 aCharCode, aCharCode, aWidth, aHeight,
                 glyph );
      else
        EwPrint( "CreateGlyph( '\\x%04X', %d, %d ) --> 0x%p\n",
                 aCharCode, aWidth, aHeight, glyph );
  #endif

  return glyph;
}


/*******************************************************************************
* FUNCTION:
*   EwFindGlyph
*
* DESCRIPTION:
*   The function EwFindGlyph() searches the glyph cache for a glyph matching
*   the given font and charcode values. If no glyph could be found, the function
*   fails and returns null.
*   
*   The found glyph passes to the ownership of the caller of this function. In
*   order to release the glyph, EwFreeGlyph() should be used.
*
* ARGUMENTS:
*   aFont,
*   aCharCode - Unique identification for the glyph.
*
* RETURN VALUE:
*   If successful, EwFindGlyph() returns the found glyph otherwise null is
*   returned.
*
*******************************************************************************/
XGlyph* EwFindGlyph( unsigned long aFont, unsigned long aCharCode )
{
  /* Depending on the glyph code - select the appropriate hash list */
  XGlyph* glyph = EwGlyphCacheHeads[ aCharCode & ( NoOfGlyphCacheLists - 1 )];

  /* Every invocation of this API should drive the state machine */
  ProcessIssues();

  /* Search the cache for the desired glyph */
  while ( glyph && (( glyph->Font     != aFont ) || 
                    ( glyph->CharCode != aCharCode )))
    glyph = glyph->Next;

  /* Not found? Nothing to do */
  if ( !glyph )
    return 0;

  /* Place the found glyph at the begin of the cache to optimize the next 
     search operation */
  UncacheGlyph( glyph );
  CacheGlyph  ( glyph );

  /* The new glyph passes into the ownership of the caller */
  glyph->Used++;

  return glyph;
}


/*******************************************************************************
* FUNCTION:
*   EwFreeGlyph
*
* DESCRIPTION:
*   The function EwFreeGlyph() decrements the usage counter of the glyph. If not
*   involved in any other drawing operations the glyph is considered as unused.
*
*   Freed glyphs do remain in the cache until they are used again or due to a
*   resource deficit displaced from the video memory.
*
* ARGUMENTS:
*   aGlyph - Glyph to free.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeGlyph( XGlyph* aGlyph )
{
  aGlyph->Used--;
}


/*******************************************************************************
* FUNCTION:
*   EwDiscardGlyph
*
* DESCRIPTION:
*   The function EwDiscardGlyph() forces the affected glyph to be removed from
*   the glyph cache if the glyph is not iun use actually. Should the glyph be
*   used, nothing happens.
*
* ARGUMENTS:
*   aGlyph - Glyph to eventually discard.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDiscardGlyph( XGlyph* aGlyph )
{
  if ( !aGlyph->Used )
    DiscardGlyph( aGlyph );
}


/*******************************************************************************
* FUNCTION:
*   EwLockGlyphCache
*
* DESCRIPTION:
*   The function EwLockGlyphCache() provides a direct access to the pixel memory
*   of the given glyph. In this manner the memory can be read and modified. When
*   finishing the memory access, the glyph should be unlocked again by using the
*   function EwUnlockGlyphCache().
*
*   Because all glyphs are stored within the glyph cache surface, the request 
*   will be passed to the EwLockSurface() function, which in turn will lock the
*   glyph area within the surface.
*
*   Depending on the specified mode, the function may force the Graphics Engine
*   to flush pending tasks and to wait for the glyph cache surface if there are
*   pending tasks affecting the surface. To avoid this kind of synchronization,
*   the flag EW_LOCK_DONT_WAIT should be set in the aMode parameter. Regardless
*   of this additional flag, the underlying gryphics subsystem always performs
*   the necessary synchronisation steps in order to avoid any interferences with
*   hardware executed drawing operations.
*   
*   The function fills the passed lock structure with the lock information. Here
*   the caller will find a pointer to the pixel memory and the distance between
*   two pixel rows. The memory returned by the function is not guaranteed to be
*   the real video memory of the surface. If necessary, the function will handle
*   the access by using a shadow memory area. Therefore to limit the effort of
*   memory copy operations, the desired access mode should be specified.
*
*   If there was not possible to lock the glyph, the function fails and returns
*   zero.
*
* ARGUMENTS:
*   aGlyph - Glyph to obtain the direct memory access.
*   aMode  - Desired access mode to the glyph pixel memory (see EW_LOCK_XXX).
*   aLock  - Pointer to a structure where the lock information will be stored.
*
* RETURN VALUE:
*   If successful, the function returns != 0, otherwise 0 is returned.
*
*******************************************************************************/
int EwLockGlyphCache( XGlyph* aGlyph, unsigned short aMode, XSurfaceLock* aLock )
{
  /* The origin of the glyph within the glyph cache surface */
  int x = aGlyph->CacheX;
  int y = aGlyph->CacheY;

  /* Lock the affected area of the glyph cache surface. If additional one-pixel
     wide border exists around the glyph -> also take in account this area! */
  #ifdef EW_BORDER_AROUND_GLYPHS
    if ( !EwLockSurface( EwGlyphCacheSurface, x, y, aGlyph->Width + 2,
           aGlyph->Height + 2, 0, 0, aMode, aLock ))
      return 0;

    /* Although the lock encloses the border area -> the lock should refer
       to the glyph's first pixel - otherwise the higher level functions will
       fail. The border can be addressed as row[-1], col[-1], etc. */
    aLock->Memory.Pixel1 = (char*)aLock->Memory.Pixel1 +
                           aLock->Memory.Pitch1Y + aLock->Memory.Pitch1X;

    return 1;
  #else
    return EwLockSurface( EwGlyphCacheSurface, x, y, aGlyph->Width, 
      aGlyph->Height, 0, 0, aMode, aLock );
  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwUnlockGlyphCache
*
* DESCRIPTION:
*   The function EwUnlockGlyphCache() provides a counterpart to the function
*   EwLockGlyphCache(). When called, the function releases the given lock and
*   updates the global glyph cache surface. If necessary, the made modifications
*   are transfered to the video memory of the surface.
*
* ARGUMENTS:
*   aGlyph - Glyph to terminate the direct memory access.
*   aLock  - Lock descriptor initialized by the previous call to the function
*     EwLockGlyphCache().
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwUnlockGlyphCache( XGlyph* aGlyph, XSurfaceLock* aLock )
{
  EW_UNUSED_ARG ( aGlyph );

  if ( !aLock )
    return;

  /* Although the lock encloses the border area -> the lock should refer
     to the glyph's first pixel - otherwise the higher level functions will
     fail. The border can be addressed as row[-1], col[-1], etc. */
  #ifdef EW_BORDER_AROUND_GLYPHS
    aLock->Memory.Pixel1 = (char*)aLock->Memory.Pixel1 -
                           aLock->Memory.Pitch1Y - aLock->Memory.Pitch1X;
  #endif

  EwUnlockSurface( EwGlyphCacheSurface, aLock );
}


/*******************************************************************************
* FUNCTION:
*   EwInitIssue
*
* DESCRIPTION:
*  The function EwInitIssue() introduces the preparation of a new drawing task
*  for the specified surface. The surface will remain in this state until the
*  function EwDoneIssue() has been invoked.
*
*  EwInitIssue() ensures, that there is a valid issue attached to the surface.
*  If necessary, new empty issue will be created. If no memory is available for
*  new issues, the function forces the processing of pending drawing operations
*  and flushes the caches.
*
* ARGUMENTS:
*   aSurface - Surface to introduce the preparation of drawing tasks.
*
* RETURN VALUE:
*   If successful, EwInitIssue() returns the desired issue otherwise null is
*   returned.
*
*******************************************************************************/
XIssue* EwInitIssue( XSurface* aSurface )
{
  XIssue* issue = aSurface->Tasks;

  /* Every invocation of this API should drive the state machine */
  ProcessIssues();

  /* There is already an issue attached to the surface - use it */
  if ( issue )
  {
    /* For profiling purpose remember the memory usage in the issue buffer just
       before new issue is prepared. */
    issue->Usage = (int)(( issue->UpperEnd - issue->UpperHeap ) + 
                         ( issue->LowerHeap - (char*)( issue + 1 )));
    return issue;
  }

  /* Try to reuse an old or to create a new issue. If not possible, try to free
     the still unused resources and repeat */
  do
  {
    /* Any old unused issues? Reuse */
    if ( EwIssuesReady.Head )
    {
      issue = EwIssuesReady.Head;
      DequeueIssue( issue );
    }

    /* If not, create a new one */
    else
    {
      issue = EwAlloc( sizeof( XIssue ) + IssueSize );

      /* Track the RAM usage */
      if ( issue )
      {
        issue->Peak        = 0;
        EwResourcesMemory += sizeof( XIssue ) + IssueSize;

        /* Also track the max. memory pressure */
        if ( EwResourcesMemory > EwResourcesMemoryPeak )
          EwResourcesMemoryPeak = EwResourcesMemory;

        if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
          EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
      }
    }
  }
  while ( !issue && ( FlushAndClean( 1, 1 ) || EwImmediateReclaimMemory( 40 )));

  /* Out of memory */
  if ( !issue )
  {
    EwError( 40 );
    return 0;
  }

  /* Initialize the issue and the both heaps - empty for now */
  issue->Surface   = aSurface;
  issue->LowerHeap = (char*)( issue + 1 );
  issue->UpperHeap = issue->LowerHeap + IssueSize;
  issue->UpperEnd  = issue->UpperHeap;
  issue->List      = 0;
  issue->Next      = 0;
  issue->Prev      = 0;
  issue->Tasks     = 0;
  issue->Usage     = 0;

  /* Attach the issue to the surface and ... */
  aSurface->Tasks = issue;

  /* ... change the state of the issue to 'used' */
  EnqueueIssue( issue, &EwIssuesUsed );

  return issue;
}


/*******************************************************************************
* FUNCTION:
*   EwDoneIssue
*
* DESCRIPTION:
*  The function EwDoneIssue() completes the preparation of drawing tasks for the
*  specified surface.
*
*  EwDoneIssue() can cause the issue to be flushed if the issue memory has been
*  exhausted by the previously prepared tasks.
*
* ARGUMENTS:
*   aSurface - Surface to complete the issue preparation.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDoneIssue( XSurface* aSurface )
{
  XIssue*  issue      = aSurface->Tasks;
  int      gapSpace   = (int)( issue->UpperHeap - issue->LowerHeap );
  int      tasksSpace = (int)( issue->UpperEnd  - issue->UpperHeap );
  int      noOfTasks  = tasksSpace / sizeof( XTask );
  int      peak;

  /* Don't consume the entire free memory. A small area should remain reserved
     for the future optimization pass. We assume 50 % of the existing tasks
     will be duplicated durring this pass. */
  gapSpace -= tasksSpace / 2;

  /* For profiling purpose track the memory usage in the issue buffer */
  peak = (int)(( issue->UpperEnd - issue->UpperHeap ) + 
               ( issue->LowerHeap - (char*)( issue + 1 )) - issue->Usage );

  /* Track the max. usage for the biggest issue */
  if ( peak > issue->Peak )
    issue->Peak = peak;

  /* Flush the issue if there is not enough space for new tasks or the max.
     number of tasks for a single has been reached */
  if (( gapSpace < MinIssueGap ) || ( noOfTasks >= EwMaxIssueTasks ))
    EwFlushTasks( aSurface );
}


/*******************************************************************************
* FUNCTION:
*   EwAllocTask
*
* DESCRIPTION:
*  The function EwAllocTask() has the job to reserve memory for a new drawing
*  task and to incorporate it into the list of pending tasks of the specified
*  issue. The memory reservation will take place on the upper heap area of the
*  issue.
*
*  If the parameter aCopyOf refers to an other task, the function initializes
*  the new task with a copy of it and enqueues the task immediately after the
*  original task. If there is no task to make a copy of it, the new task will
*  remain uninitialized at the end of the list with pending tasks.
*
*  If there is no memory to attend the task allocation, the function fails and
*  returns null. 
*
* ARGUMENTS:
*   aIssue  - Issue to attend the memory allocation.
*   aCopyOf - Optional task to copy into the new task.
*
* RETURN VALUE:
*   If succesful, EwAllocTask() returns a pointer to the new task otherwise
*   null is returned.
*
*******************************************************************************/
XTask* EwAllocTask( XIssue* aIssue, XTask* aCopyOf )
{
  int    gapSpace = (int)( aIssue->UpperHeap - aIssue->LowerHeap );
  XTask* task;

  /* Enough space for a new task? */
  if ( gapSpace < (int)( sizeof( XTask )))
    return 0;

  /* Get memory for the new task */
  aIssue->UpperHeap -= sizeof( XTask );
  task = (XTask*)aIssue->UpperHeap;

  /* Initialize the task with a copy of the other task and place the copy behind
     this origin task */
  if ( aCopyOf )
  {
    *task         = *aCopyOf;
    task->Next    = aCopyOf->Next;
    aCopyOf->Next = task;
  }

  /* Enqueue the task at the begin of the list of tasks */
  else
  {
    task->Next    = aIssue->Tasks;
    aIssue->Tasks = task;
  }

  return task;
}


/*******************************************************************************
* FUNCTION:
*   EwAllocTasks
*
* DESCRIPTION:
*  The function EwAllocTasks() has the job to reserve memory for a set of new
*  drawing tasks and to incorporate them into the list of pending tasks of the
*  specified issue. The memory reservation will take place on the upper heap
*  area of the issue.
*
*  If the parameter aCopyOf refers to an other task, the function initializes
*  the new tasks with a copy of it and enqueues the tasks immediately after the
*  original task. If there is no task to make a copy of it, the new tasks will
*  remain uninitialized at the end of the list with pending tasks.
*
*  If there is no memory to attend the task allocation, the function fails and
*  returns null. 
*
* ARGUMENTS:
*   aIssue     - Issue to attend the memory allocation.
*   aNoOfTasks - Number to tasks to allocate.
*   aCopyOf    - Optional task to copy into the new task.
*
* RETURN VALUE:
*   If succesful, EwAllocTask() returns a pointer to the first task otherwise
*   null is returned.
*
*******************************************************************************/
XTask* EwAllocTasks( XIssue* aIssue, int aNoOfTasks, XTask* aCopyOf )
{
  int    gapSpace = (int)( aIssue->UpperHeap - aIssue->LowerHeap );
  int    size     = sizeof( XTask ) * aNoOfTasks;
  XTask* task     = 0;

  /* Enough space for a new task? */
  if (( aNoOfTasks <= 0 ) || ( gapSpace < size ))
    return 0;

  /* Reserve the remaining tasks */
  while ( aNoOfTasks-- > 0 )
    task = EwAllocTask( aIssue, aCopyOf );

  return task;
}


/*******************************************************************************
* FUNCTION:
*   EwAllocTaskData
*
* DESCRIPTION:
*  The function EwAllocTaskData() has the job to reserve a memory area for the
*  parameters of a drawing task. The memory reservation will take place on the
*  lower heap area of the specified issue.
*
*  If there is no memory available to attend the allocation, the function fails
*  and returns null.
*
* ARGUMENTS:
*   aIssue - Issue to attend the memory allocation.
*   aSize  - Size of the memory to reserve.
*
* RETURN VALUE:
*   If succesful, EwAllocTaskData() returns a pointer to the reserved memory
*   area otherwise null is returned.
*
*******************************************************************************/
void* EwAllocTaskData( XIssue* aIssue, int aSize )
{
  int    gapSize = (int)( aIssue->UpperHeap - aIssue->LowerHeap );
  void*  ptr     = aIssue->LowerHeap;
  int    peak;

  /* Enough space for the alloc operation? */
  if ( gapSize < ( aSize + (int)( sizeof( XTask ))))
    return 0;

  /* Get memory for the new data block */
  aIssue->LowerHeap += aSize;

  /* For profiling purpose track the memory usage in the issue buffer */
  peak = (int)(( aIssue->UpperEnd - aIssue->UpperHeap ) + 
               ( aIssue->LowerHeap - (char*)( aIssue + 1 )) - aIssue->Usage );

  /* Track the max. usage for the biggest issue */
  if ( peak > aIssue->Peak )
    aIssue->Peak = peak;

  return ptr;
}


/*******************************************************************************
* FUNCTION:
*   EwFreeTaskData
*
* DESCRIPTION:
*  The function EwFreeTaskData() has the job to shrink the memory area reserved
*  by the preceding invocation of the EwAllocTaskData() call.
*
* ARGUMENTS:
*   aIssue - Issue to attend the memory allocation.
*   aPtr   - Address specifying the new end of actually occupied memory area.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwFreeTaskData( XIssue* aIssue, void* aPtr )
{
  if (((char*)aPtr < aIssue->LowerHeap ) && ((char*)aPtr >= (char*)( aIssue + 1 )))
    aIssue->LowerHeap = aPtr;
}


/*******************************************************************************
* FUNCTION:
*   EwFlushTasks
*
* DESCRIPTION:
*  The function EwFlushTasks() has the job to complete and flush the currently
*  existing issue of the specified surface. If there is no open issue, nothing
*  is done and the function returns to the caller.
*
*  If 0 is passed for aSurface, the next available issue is flushed.
*
* ARGUMENTS:
*   aSurface - Surface to flush the pending tasks.
*
* RETURN VALUE:
*   Returns != 0, if an issue could be flushed.
*
*******************************************************************************/
int EwFlushTasks( XSurface* aSurface )
{
  XIssue* issue = aSurface? aSurface->Tasks : EwIssuesUsed.Tail;

#if defined EW_ELIMINATE_TASKS && defined EW_SUPPORT_GFX_TASK_TRACING
  int noOfTasks1 = 0;
  int noOfTasks2 = 0;
  int copyArea1  = 0;
  int copyArea2  = 0;
  int blendArea1 = 0;
  int blendArea2 = 0;
#endif

  /* No tasks to flush for this surface */
  if ( !issue || !issue->Surface )
    return 0;

  aSurface = issue->Surface;

  /* An attempt to flush drawing tasks for a surface, which is waiting for use
     as source in other drawing operations. Flush all other drawing operations
     in order to avoid the task order inversion */
  if ( aSurface->Used > ( aSurface->Owned + aSurface->Pending ))
  {
    /* Hide the surface tasks for now */
    DequeueIssue( issue );

    /* Process all pending and prepared tasks until the surface is not involved
       in any other drawing operations */
    for ( ;; )
    {
      ProcessIssues();

      /* Repeat as long as the surface remains involved in other tasks */
      if (( aSurface->Used <= ( aSurface->Owned + aSurface->Pending )) ||
            !EwIssuesUsed.Tail )
        break;

      /* Force the processing of still not completed tasks. This ensures the
         correct task extecution order */
      EwFlushTasks( EwIssuesUsed.Tail->Surface );
    }

    /* Now the surface tasks can be processed */
    EnqueueIssue( issue, &EwIssuesUsed );
  }

  /* For statistic purpose remember the current tasks impact before the 
     elimination took place */
  #if defined EW_ELIMINATE_TASKS && defined EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwCountTasks( issue, &noOfTasks1, &copyArea1, &blendArea1 );
  #endif

  /* Evaluate the list of pending tasks with the objective to eliminate and
     reduce the amount of drawing operations */
  #ifdef EW_ELIMINATE_TASKS
    EwEliminateTasks( issue );
  #endif

  /* For statistic purpose remember the tasks impact after the elimination
     took place */
  #if defined EW_ELIMINATE_TASKS && defined EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
    {
      EwCountTasks( issue, &noOfTasks2, &copyArea2, &blendArea2 );

      EwPrint( "ELIMINATE TASKS: %d(%+d) Overwrite: %d(%+0.2f%%) Blend: %d(%+0.2f%%)\n",
        noOfTasks2, noOfTasks2 - noOfTasks1, copyArea2,
        copyArea1? 
          ((float)( copyArea2  - copyArea1  ) / (float)copyArea1  ) * 100.0 :
          0.0,
        blendArea2,
        blendArea1?
          ((float)( blendArea2 - blendArea1 ) / (float)blendArea1 ) * 100.0 :
          0.0 );
    }
  #endif

  /* Change the order of tasks with the objective to put software emulated
     tasks together */
  #ifdef EW_REORDER_TASKS
    EwReorderTasks( issue );
  #else
    EwReverseTasks( issue );
  #endif

  /* Change the state of the issue to 'pending' - the issue waits for a free
     slot to the graphics subsystem */
  DequeueIssue( issue );
  EnqueueIssue( issue, &EwIssuesPending );

  /* Detach the issue from the surface but mark the surface as waiting for
     completion */
  aSurface->Tasks = 0;
  aSurface->Pending++;
  aSurface->Used++;

  /* Push the processing of issues */
  ProcessIssues();

  /* Release surfaces freed in the meantime */
  CleanSurfaceCache( 0 );

  /* An issue could be flushed */
  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwWaitForSurface
*
* DESCRIPTION:
*  The function EwWaitForSurface() ensures, that the surface is not involved in
*  any pending or running drawing operations. If necessary the function flushes
*  issues and waits until they are terminated. The last argument describes the
*  impact of the wait operation.
*
* ARGUMENTS:
*   aSurface     - Surface to flush and finish its pending drawing operations.
*   aUntilUnused - If != 0, the function also waits until other operations are
*     done, which involve the surface as a source. If aUntilUnused == 0, only
*     the drawing operations affecting the surface as a destination are flushed
*     and finished.
*
* RETURN VALUE:
*   Returns != 0, if issues could be flushed.
*
*******************************************************************************/
int EwWaitForSurface( XSurface* aSurface, int aUntilUnused )
{
  /* Ensure the previously prepared but still not flushed issues of this
     surface are ready for the execution*/
  int result = EwFlushTasks( aSurface );

  /* Repeat as long as there are any interferences possible with other drawing
     operations */
  for ( ;; )
  {
    /* There are any drawing operations waiting for this surface or the surface
       is used as a source by other drawing operations and the caller has the
       intention to modify this surface content */
    while ( aSurface->Pending ||
            (
                aUntilUnused &&
              ( aSurface->Used > aSurface->Owned ) &&
              (
                 EwIssuesPending.Tail || 
                 EwIssuesRunning.Tail || 
                 EwIssuesFinished.Tail 
              )
            )
          )
    {
      /* Drive the graphics engine state machine */
      ProcessIssues();

      /* If an issue is running - wait for its termination */
      #ifdef EwGfxWaitForThread
        if ( EwIssuesRunning.Tail )
          EwGfxWaitForThread( 10 );
      #endif
    }

    /* No interferences with other operations */
    if (( aSurface->Used <= aSurface->Owned ) || !aUntilUnused || 
          !EwIssuesUsed.Tail )
      break;

    /* The surface is used by other still not flushed drawing operations - force
       the next issue to be flushed */
    result |= EwFlushTasks( EwIssuesUsed.Tail->Surface );
  }

  return result;
}


/*******************************************************************************
* FUNCTION:
*   EwCleanSurfaceCache
*
* DESCRIPTION:
*  The function EwCleanSurfaceCache() frees all actually cached but not used
*  surfaces in order to get more free memory.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns != 0, if at least one surface could be freed.
*
*******************************************************************************/
int EwCleanSurfaceCache( void )
{
  return CleanSurfaceCache( 1 );
}


/* Masks for quick evaluation and modification of the glyph cache surface
   allocation map. */
static const unsigned int RowMasks[] =
{
  0x00000000,
  0x00000001, 0x00000003, 0x00000007, 0x0000000F,
  0x0000001F, 0x0000003F, 0x0000007F, 0x000000FF,
  0x000001FF, 0x000003FF, 0x000007FF, 0x00000FFF,
  0x00001FFF, 0x00003FFF, 0x00007FFF, 0x0000FFFF,
  0x0001FFFF, 0x0003FFFF, 0x0007FFFF, 0x000FFFFF,
  0x001FFFFF, 0x003FFFFF, 0x007FFFFF, 0x00FFFFFF,
  0x01FFFFFF, 0x03FFFFFF, 0x07FFFFFF, 0x0FFFFFFF,
  0x1FFFFFFF, 0x3FFFFFFF, 0x7FFFFFFF, 0xFFFFFFFF,
};


/* The following helper function stores the passed surface at the begin of the
   global surface cache. */
static void CacheSurface( XSurface* aSurface )
{
  aSurface->Older = EwSurfaceCacheHead;
  aSurface->Newer = 0;

  if ( EwSurfaceCacheHead )
    EwSurfaceCacheHead->Newer = aSurface;
  else
    EwSurfaceCacheTail = aSurface;

  EwSurfaceCacheHead  = aSurface;
  EwSurfaceCacheSize += aSurface->MemSize;
}


/* The following helper function removes the passed surface from the global
   surface cache. */
static void UncacheSurface( XSurface* aSurface )
{
  if ( aSurface->Older )
    aSurface->Older->Newer = aSurface->Newer;
  else
    EwSurfaceCacheTail     = aSurface->Newer;

  if ( aSurface->Newer )
    aSurface->Newer->Older = aSurface->Older;
  else
    EwSurfaceCacheHead     = aSurface->Older;

  aSurface->Older = 0;
  aSurface->Newer = 0;

  EwSurfaceCacheSize -= aSurface->MemSize;
}


#ifdef EW_PROTOTYPING
  /* The following helper function stores the passed surface at the begin of the
     second surface cache. */
  static void CacheRevivalSurface( XSurface* aSurface )
  {
    aSurface->Older = EwRevivalSurfaceCacheHead;
    aSurface->Newer = 0;

    if ( EwRevivalSurfaceCacheHead )
      EwRevivalSurfaceCacheHead->Newer = aSurface;
    else
      EwRevivalSurfaceCacheTail = aSurface;

    EwRevivalSurfaceCacheHead  = aSurface;
    EwRevivalSurfaceCacheSize += aSurface->MemSize;
  }

  /* The following helper function removes the passed surface from the second
     surface cache. */
  static void UncacheRevivalSurface( XSurface* aSurface )
  {
    if ( aSurface->Older )
      aSurface->Older->Newer    = aSurface->Newer;
    else
      EwRevivalSurfaceCacheTail = aSurface->Newer;

    if ( aSurface->Newer )
      aSurface->Newer->Older    = aSurface->Older;
    else
      EwRevivalSurfaceCacheHead = aSurface->Older;

    aSurface->Older = 0;
    aSurface->Newer = 0;

    EwRevivalSurfaceCacheSize -= aSurface->MemSize;
  }
#endif


/* The following helper function has the job to discard the given surface. This
   operation will cause the release of all resources occupied by the surface. */
static void DiscardSurface( XSurface* aSurface )
{
  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "DiscardSurface( 0x%p, %d, %d )\n", aSurface, aSurface->Width,
                aSurface->Height );
  #endif

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XSurface );

  /* Remove the surface from the cache */
  UncacheSurface( aSurface );

  /* There are some drawing tasks prepared for this surface. Detach them from
     the surface first! */
  if ( aSurface->Tasks )
  {
    DequeueIssue( aSurface->Tasks );
    EnqueueIssue( aSurface->Tasks, &EwIssuesReady );
    aSurface->Tasks = 0;
  }

  /* Pass the invocation to the underlying gryphics subsystem. */
  EwStartPerfCounter( EwGfxDestroySurface );
  {
    if ( aSurface->Format == EW_PIXEL_FORMAT_NATIVE )
      #ifdef EwGfxCreateNativeSurface
        EwGfxDestroyNativeSurface( aSurface->Handle );
      #else
        EwDestroyNativeSurface ( aSurface->Handle );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_ALPHA8 )
      #ifdef EwGfxCreateAlpha8Surface
        EwGfxDestroyAlpha8Surface( aSurface->Handle );
      #else
        EwDestroyAlpha8Surface ( aSurface->Handle );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_INDEX8 )
      #ifdef EwGfxCreateIndex8Surface
        EwGfxDestroyIndex8Surface( aSurface->Handle );
      #else
        EwDestroyIndex8Surface ( aSurface->Handle );
      #endif

    if ( aSurface->Format == EW_PIXEL_FORMAT_RGB565 )
      #ifdef EwGfxCreateRGB565Surface
        EwGfxDestroyRGB565Surface( aSurface->Handle );
      #else
        EwDestroyRGB565Surface ( aSurface->Handle );
      #endif
  }
  EwStopPerfCounter( EwGfxDestroySurface );

  /* Free also the descriptor structor of the surface */
  EwFree( aSurface );
}


/* The following helper function stores the passed glyph at the begin of the
   global glyph cache. */
static void CacheGlyph( XGlyph* aGlyph )
{
  /* Depending on the glyph code - select the appropriate hash list */
  int      sel      = aGlyph->CharCode & ( NoOfGlyphCacheLists - 1 );
  XGlyph** listHead = EwGlyphCacheHeads + sel;
  XGlyph** listTail = EwGlyphCacheTails + sel;

  /* First place the glyph in the global cache list */
  aGlyph->Older = EwGlyphCacheHead;
  aGlyph->Newer = 0;

  if ( EwGlyphCacheHead )
    EwGlyphCacheHead->Newer = aGlyph;
  else
    EwGlyphCacheTail = aGlyph;

  EwGlyphCacheHead = aGlyph;

  /* and then in a hash list */
  aGlyph->Next = *listHead;
  aGlyph->Prev = 0;

  if ( *listHead )
    (*listHead)->Prev = aGlyph;
  else
    (*listTail) = aGlyph;

  (*listHead) = aGlyph;
}


/* The following helper function removes the passed glyph from the global glyph
   cache. */
static void UncacheGlyph( XGlyph* aGlyph )
{
  /* Depending on the glyph code - select the appropriate hash list */
  int      sel      = aGlyph->CharCode & ( NoOfGlyphCacheLists - 1 );
  XGlyph** listHead = EwGlyphCacheHeads + sel;
  XGlyph** listTail = EwGlyphCacheTails + sel;

  /* First remove the glyph from the global cache list */
  if ( aGlyph->Older )
    aGlyph->Older->Newer = aGlyph->Newer;
  else
    EwGlyphCacheTail = aGlyph->Newer;

  if ( aGlyph->Newer )
    aGlyph->Newer->Older = aGlyph->Older;
  else
    EwGlyphCacheHead = aGlyph->Older;

  /* and then from the hash list */
  if ( aGlyph->Next )
    aGlyph->Next->Prev = aGlyph->Prev;
  else
    (*listTail) = aGlyph->Prev;

  if ( aGlyph->Prev )
    aGlyph->Prev->Next = aGlyph->Next;
  else
    (*listHead) = aGlyph->Next;

  aGlyph->Older = 0;
  aGlyph->Newer = 0;
  aGlyph->Next  = 0;
  aGlyph->Prev  = 0;
}


/* The following helper function has the job to discard the given glyph. This
   operation will cause the release of all resources occupied by the glyph. */
static void DiscardGlyph( XGlyph* aGlyph )
{
  /* Calculate the size of the area occupied by the glyph ... */
  int row  = aGlyph->CacheY / GlyphCacheCellSize;
  int col  = aGlyph->CacheX / GlyphCacheCellSize;

  /* ... and take in account an additional one-pixel wide border around the
     glyph */
  #ifdef EW_BORDER_AROUND_GLYPHS
    int rows = ( aGlyph->Height + GlyphCacheCellSize + 1 ) / GlyphCacheCellSize;
    int cols = ( aGlyph->Width  + GlyphCacheCellSize + 1 ) / GlyphCacheCellSize;
  #else
    int rows = ( aGlyph->Height + GlyphCacheCellSize - 1 ) / GlyphCacheCellSize;
    int cols = ( aGlyph->Width  + GlyphCacheCellSize - 1 ) / GlyphCacheCellSize;
  #endif

  /* Track the operations */
  #ifdef EW_SUPPORT_GFX_TASK_TRACING
    if ( EwPrintGfxTasks )
      EwPrint( "DiscardGlyph( 0x%p, %d, %d )\n", aGlyph, aGlyph->Width,
                aGlyph->Height );
  #endif

  /* Remove the surface from the cache and release the area within the surface */
  UncacheGlyph( aGlyph );
  ReleaseGlyphCacheArea( col, row, cols, rows );

  /* Also free the internal copy of glyph pixel data (if any) */
  if ( aGlyph->Pixel )
  {
    EwFree( aGlyph->Pixel );

    /* Track the RAM usage */
    EwResourcesMemory -= ( aGlyph->Width * aGlyph->Height ) + 1;
  }

  aGlyph->Pixel = 0;

  /* And don't forget to free the memory occupied by the glyph structure */
  EwFree( aGlyph );

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XGlyph );
  EwNoOfGlyphs--;
}


/* The following helper function searches within the glyph cache allocation map
   for a free area to accommodate a glyph with the given size. If successful,
   the function returns the origin of the area in the arguments aRow and 
   aColumn and returns != 0. */
static int FindFreeGlyphCacheArea( int aNoOfColumns, int aNoOfRows,
  int* aColumn, int* aRow )
{
  int wpc = NoOfGlyphCacheRows / 32;
  int r, c;

  /* Repeat until all allocation map rows and columns have been evaluated */
  for ( c = 0; c <= ( NoOfGlyphCacheCols - aNoOfColumns ); c++ )
    for ( r = 0; r < ( NoOfGlyphCacheRows / 32 ); r++ )
    {
      unsigned int mask  = RowMasks[ aNoOfRows ];
      unsigned int word  = EwGlyphCacheSurfaceMap[ c * wpc + r ];
      int          count = 32 - aNoOfRows;
      int          found;

   /* This is a workaround for the IAR Embedded Workbench compiler 
      (iccarm.exe, V7.80.4.12462/W32 for ARM) according to IAR support (14.08.2017)
      This workaround avoids the graphical errors and crashes observed, 
      if iccarm.exe is used with highes optimisation */
#ifdef __ICCARM__
      volatile int cc;
#else      
      int cc;
#endif

      /* Within the column search for a continuous serie of free tiles. */
      do
      {
        while ( count && ( word & mask ))
        {
          mask <<= 1;
          count--;
        }

        /* Could found a column sequence of free tiles? Then ... */
        found = !( word & mask );

        /* ... ensure the following cilumns are also empty to accommodate the
           entire glyph - search columns-wise */
        for ( cc = c + 1; found && ( cc < ( c + aNoOfColumns )); cc++ )
          found = !( EwGlyphCacheSurfaceMap[ cc * wpc + r ] & mask );

        if ( !found )
        {
          mask <<= 1;
          count--;
        }
      }
      while ( !found && ( count > 0 ));

      /* The area could be found */
      if ( found )
      {
        *aColumn = c;
        *aRow    = ( r * 32 ) + 32 - aNoOfRows - count;

        return 1;
      }
    }

  /* The evaluation is done without any success */
  return 0;
}


/* The following helper function marks the given area within the glyph cache
   allocation map as occupied. */
static void ReserveGlyphCacheArea( int aColumn, int aRow, int aNoOfColumns, 
  int aNoOfRows )
{
  unsigned int mask = RowMasks[ aNoOfRows ] << ( aRow % 32 );
  int          r    = aRow / 32;

  for ( ; aNoOfColumns > 0; aColumn++, aNoOfColumns-- )
    EwGlyphCacheSurfaceMap[( aColumn * NoOfGlyphCacheRows / 32 ) + r ] |= mask;
}


/* The following helper function marks the given area within the glyph cache
   allocation map as unused. */
static void ReleaseGlyphCacheArea( int aColumn, int aRow, int aNoOfColumns, 
  int aNoOfRows )
{
  unsigned int mask = ~( RowMasks[ aNoOfRows ] << ( aRow % 32 ));
  int          r    = aRow / 32;

  for ( ; aNoOfColumns > 0; aColumn++, aNoOfColumns-- )
    EwGlyphCacheSurfaceMap[( aColumn * NoOfGlyphCacheRows / 32 ) + r ] &= mask;
}


/* The following helper function has the job to enqueue the passed issue within
   the given issues list. */
static void EnqueueIssue( XIssue* aIssue, XIssues* aList )
{
  aIssue->Next = aList->Head;
  aIssue->Prev = 0;

  if ( aList->Head )
    aList->Head->Prev = aIssue;
  else
    aList->Tail = aIssue;

  aList->Head  = aIssue;
  aIssue->List = aList;
}


/* The following helper function builds the counterpart to the EnqueueIssue().
   It dequeues the issue from its issues list. */
static void DequeueIssue( XIssue* aIssue )
{
  /* Not belonging to a list - should never occure. Just to satisfy the static
     analyzer */
  if ( !aIssue->List )
    return;

  if ( aIssue->Next )
    aIssue->Next->Prev = aIssue->Prev;
  else
    aIssue->List->Tail = aIssue->Prev;

  if ( aIssue->Prev )
    aIssue->Prev->Next = aIssue->Next;
  else
    aIssue->List->Head = aIssue->Next;

  aIssue->Next = 0;
  aIssue->Prev = 0;
  aIssue->List = 0;
}


/* The following helper function has the job to discard the memory occupied by
   the issue. */
static void DiscardIssue( XIssue* aIssue )
{
  DequeueIssue( aIssue );
  EwFree( aIssue );

  /* Track the RAM usage */
  EwResourcesMemory -= sizeof( XIssue ) + IssueSize;
}


/* The following helper function drives the state machine of the engine. The
   function starts new issues and cleans finished issues. */
static void ProcessIssues( void )
{
  /* Drive the state machine until no transitions take place */
  for ( ;; )
  {
    /* Any issue waiting for a free slot to the underlying graphics 
       subsystem? If the slot is free - launch the issue. */
    if ( EwIssuesPending.Tail && !EwIssuesRunning.Tail )
    {
      XIssue* issue = EwIssuesPending.Tail;

      /* Place the issue into the slot */
      DequeueIssue( issue );
      EnqueueIssue( issue, &EwIssuesRunning );

      /* Perform the execution of the issue - if possible let run it in a
         separate thread */
      #ifdef EwGfxLaunchThread
        EwGfxLaunchThread( EwExecuteTasks, issue );
      #else
        EwExecuteTasks( issue );
      #endif
    }

    /* Any completed issues still occupying the slot to the underlying 
       graphics subsystem - move the issues to the finished queue */
    else if (
              EwIssuesRunning.Tail

              #ifdef EwGfxWaitForThread
                && EwGfxWaitForThread( 0 )
              #endif
            )
    {
      XIssue* issue = EwIssuesRunning.Tail;

      /* Place the issue into the finished queue */
      DequeueIssue( issue );
      EnqueueIssue( issue, &EwIssuesFinished );
    }

    /* Any finished issues there? Release all resources used by them and
       recycle it */
    else if ( EwIssuesFinished.Tail )
    {
      XIssue* issue = EwIssuesFinished.Tail;

      /* Release resources involved in drawing tasks of the issue */
      EwCompleteTasks( issue );

      /* Release the affected destination surface */
      issue->Surface->Pending--;
      issue->Surface->Used--;

      /* Issue can be recycled for the next time */
      DequeueIssue( issue );
      EnqueueIssue( issue, &EwIssuesReady );
    }

    else
      return;
  }
}


/* The following helper function flushes pending issues and releases unused
   surface, glyphs, etc. */
static int FlushAndClean( int aSurfaces, int aGlyphs )
{
  XSurface* surface = EwSurfaceCacheTail;
  XGlyph*   glyph   = EwGlyphCacheTail;
  int       count   = aSurfaces + aGlyphs;
  int       urgency = 0;

  /* First at all, ensure that all flushed issues are done */
  while ( EwIssuesPending.Tail || EwIssuesRunning.Tail  || EwIssuesFinished.Tail )
  {
    /* Drive the graphics engine state machine */
    ProcessIssues();

    /* If an issue is running currently - wait for its termination */
    #ifdef EwGfxWaitForThread
      if ( EwIssuesRunning.Tail )
        EwGfxWaitForThread( 10 );
    #endif
  }

  /* No more issues running or waiting for execution - start destroying unused
     glyphs */
  while (( aGlyphs > 0 ) && glyph )
  {
    XGlyph* tmp = glyph;

    glyph = glyph->Newer;

    /* Discard unused glyphs only */
    if ( !tmp->Used )
    {
      DiscardGlyph( tmp );
      aGlyphs--;
    }
  }
  
  /* Continue with unused surfaces */
  while (( aSurfaces > 0 ) && surface )
  {
    XSurface* tmp = surface;

    surface = surface->Newer;

    /* Discard unused surfaces only */
    if ( !tmp->Used )
    {
      DiscardSurface( tmp );
      aSurfaces--;
    }
  }
  
  /* Still surfaces to release? Try to discard surfaces, which can be restored */
  for ( ; ( aSurfaces > 0 ) && ( urgency < 3 ); urgency++ )
    for ( surface = EwSurfaceCacheTail; ( aSurfaces > 0 ) && surface; )
    {
      XSurface* tmp = surface;

      surface = surface->Newer;

      /* Can discard and restore this surface again? Note, surfaces which are 
         involved in a drawing operation or locked are protected from being
         discarded. Similarly, surfaces containing constant (DirectAccess) data,
         are not discarded. */
      if ( tmp->Owner && ( tmp->Tag1 || tmp->Tag2 ) && ( tmp->MemSize > 0 ) &&
         ( tmp->Used == 1 ) && ( tmp->Owned == 1 ) &&
           EwCanDiscardSurface( tmp, urgency ))
      {
        tmp->Used--;
        tmp->Owned--;
        tmp->Owner = 0;
        aSurfaces--;

        /* During prototyping discarded surfaces are not discarded. Instead they
           are moved to a second 'revival' cache. */
        #ifdef EW_PROTOTYPING
          UncacheSurface( tmp );
          CacheRevivalSurface( tmp );
        #else
          DiscardSurface( tmp );
        #endif
      }
    }
  
  /* Some surfaces or glyphs have been destroyed */
  return count - aSurfaces - aGlyphs;
}


/* The following heper function verifies whether the memory size of all surfaces
   currently stored in the surface cache exceeds the predefined treshold. In
   this case, the function tries to discard unused surfaces. If the parameter
   aForce != 0 all unused surfaces are freed. The function returns != 0 if it
   could at least free one surface.*/
static int CleanSurfaceCache( int aForce )
{
  XSurface* surface = EwSurfaceCacheTail;
  int       count   = 0;
  int       urgency = 0;

  /* If the cache appears to be full, try to discard any surface which is not in
     use anymore. Even surfaces containing loaded resources can be removed here.
     Start the discarding with oldest unused surface. */
  while (( aForce || ( EwSurfaceCacheSize > EwMaxSurfaceCacheSize )) && surface )
  {
    XSurface* tmp = surface;

    surface = surface->Newer;

    /* Discard unused surfaces only */
    if ( !tmp->Used )
    {
      DiscardSurface( tmp );
      count++;
    }
  }

  /* The cache is still full. In such case try to discard surfaces, which are
     not involved in any drawing operations and able to be restored if needed */
  for ( ; ( aForce || ( EwSurfaceCacheSize > EwMaxSurfaceCacheSize )) &&
          ( urgency < 3 ); urgency++ )
    for ( surface = EwSurfaceCacheTail; ( aForce || 
        ( EwSurfaceCacheSize > EwMaxSurfaceCacheSize )) && surface; )
    {
      XSurface* tmp = surface;

      surface = surface->Newer;

      /* Can discard and restore this surface again? Note, surfaces which are 
         involved in a drawing operation or locked are protected from being
         discarded. Similarly, surfaces containing constant (DirectAccess) data,
         are not discarded. */
      if ( tmp->Owner && ( tmp->Tag1 || tmp->Tag2 ) && ( tmp->MemSize > 0 ) &&
         ( tmp->Used == 1 ) && ( tmp->Owned == 1 ) && 
           EwCanDiscardSurface( tmp, urgency ))
      {
        tmp->Used--;
        tmp->Owned--;
        tmp->Owner = 0;
        count++;

        /* During prototyping discarded surfaces are not discarded. Instead they
           are moved to a second 'revival' cache. */
        #ifdef EW_PROTOTYPING
          UncacheSurface( tmp );
          CacheRevivalSurface( tmp );
        #else
          DiscardSurface( tmp );
        #endif
      }
    }

  /* During prototyping we manage a second cache to retain discarded surfaces. In
     such manner we avoid the surfaces to be recreated when they are used again.
     Avoid that this second cache overflows. */
  #ifdef EW_PROTOTYPING
    surface = EwRevivalSurfaceCacheTail;

    /* The cache is full. Discard superflous surfaces. */
    while (( EwRevivalSurfaceCacheSize > EwMaxRevivalSurfaceCacheSize ) && surface )
    {
      XSurface* tmp = surface;
      surface = surface->Newer;

      /* Move the surface back to the 'normal' cache and then discard it. */
      UncacheRevivalSurface( tmp );
      CacheSurface( tmp );
      DiscardSurface( tmp );
    }
  #endif

  return count;
}


/* pba */

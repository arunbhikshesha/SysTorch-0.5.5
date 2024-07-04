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
*   The module 'ewobject.c' implements an environment to run Embedded Wizard 
*   Objects. The Embedded Wizard Objects are the key technology that allows the 
*   user to develop object oriented software using the ANSI 'C' programming 
*   language.
*
*   'ewobject.c' contains a set of functions, data structures and macros 
*   necessary to describe, create and manage Embedded Wizard Objects. From the 
*   users point of view the Embedded Wizard Objects are comparable to the C++ 
*   objects, except C++ features like multiple inheritance, abstract methods, 
*   operator overloading, nested classes, structured exceptions handling.
*
*   Similary to C++ every Embedded Wizard Object is associated to the class the 
*   object has been created from. Using the information stored in the 'class'
*   EWRTE can create new object instances and manage all existing object 
*   instances.
*
*   Every Embedded Wizard object contains its own set of data fields. These data
*   fields will store the state of the object itself and the owner of the object
*   can access these data fields similary to access a member of an ANSI 'C' 
*   structure. An additional set of methods defined in the objects class allows
*   the owner to execute code (to call ANSI 'C' functions) in context of the 
*   object.
*
*   To save ROM and to promote the reusability of classes and methods, EWRTE
*   supports a single inheritance between classes. Each class can inherite all
*   data fields and methods from another class. It allows the user to extend
*   new classes with new data fields and new methods. If necessary the inherited
*   methods can be overloaded by an other implementation. The overloading of 
*   inherited methods and the fact, that all methods are virtual, makes the 
*   definition of polymorh classes possible.
*
*   Objects which are not used any more will be detected and disposed by the
*   EWRTE own Garbage Collector. The Garbage Collector knows about all existing
*   objects and it knows about relations between these objects so it can decide 
*   which objects are not used anymore, so that they can be disposed withou 
*   any risk.
*
* SUBROUTINES:
*   EwSetStackBaseAddress
*   EwNewObject
*   EwAttachObjectVariant
*   EwDetachObjectVariant
*   EwGetAutoObject
*   EwCastObject
*   EwCastClass
*   EwClassOf
*   EwLockObject
*   EwUnlockObject
*   EwReclaimMemory
*   EwImmediateReclaimMemory
*   EwTestImmediateReclaimMemory
*   EwReconstructObjects
*   EwPrintProfilerStatistic
*
*******************************************************************************/

#include "ewrte.h"


/* Undo the macro overloading of following functions */
#undef EwNewObjectIndirect
#undef EwCastObject
#undef EwCastClass
#undef EwLockObject
#undef EwUnlockObject
#undef EwGetAutoObject


/* The structure XRootSetItem stores a pointer to an object currently locked.
   Such objects are not disposed by Garbage Collection */
typedef struct _XRootSetItem
{
  struct _XRootSetItem* Next;
  struct _XRootSetItem* Prev;
  XObject               Object;
} XRootSetItem;


/* The structure XObjectFrame is used to record all existing Chora instances
   created dynamically (NOT the embedded within another instances!) within a
   single chained list. XObjectFrame prefixes the dynamically created objects. */
typedef struct _XObjectFrame
{
  struct _XObjectFrame* Next;
} XObjectFrame;


/* RootSet contains a list with currently locked objects. Locked objects belong
   to the root set -> so the objects and all depending objects will keep alive 
   and they will not be disposed by the Garbage Collector */
static XRootSetItem* RootSetHead = 0;

/* Objects points to the chain of existing objects. All existing objects will 
   be stored in this chain */
static XObjectFrame* Objects = 0;

#ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
  /* Stores a pointer to the CPU stack memory. It used during garbage collection
     to traverse the stack content and so detect objects/strings being in use. */
  static void** StackBaseAddress = 0;
#endif

/* This variable is != 0 if Garbage Collection is actually running */
static int ReclaimMemoryLock = 0;


/* This function will be used internally when the Garbage Collector sweeps all
   memory blocks which are not in use anymore. The function returns != 0 if 
   there was at least one string released. */
extern int EwDisposeStrings( int aClearCache );

/* Marks all pending signals as reachable */
extern XObject EwMarkSignals( XObject aMarkList );

/* Removes all pending signals */
extern void EwDiscardSignals( void );

/* Removes unused observers */
extern void EwDisposeObservers( void );

/* Removes unused signals */
extern void EwDisposeSignals( void );

/* Frees memory used internally by the decompress routines */
extern void EwReleaseDecompressBuffers( void );

/* Verifies whether aString points to an existing string which can be marked
   during the garbage collection. If successfull returns the correct string 
   to mark - this can be a first string within the same block of string
   constants. Otherwise 0 ist returned. */
extern XString EwCanMarkString( XString aString );

/* The following function frees all actually cached but not used surfaces in
   order to get more free memory. Returns != 0, if at least one surface could
   be freed. */
extern int EwCleanSurfaceCache( void );

/* Currently active styles set */
extern unsigned int EwStylesSet; 


/* The global VMT of the class XObject. Since the XObject is a fundamental 
   part of the Embedded Wizard Objects the VMT will be defined globaly here */
const struct _vmt_XObject __vmt_XObject =
{
  0x434C4153,
  "XObject",
  0,              /* XObject is the root -> there is no other base classes */
  0,
  sizeof( struct _obj_XObject ),
  { 0, 0, 0, 0, 0 },
  XObject__Init,
  XObject__ReInit,
  XObject__Done
};


/* The structure XProfilerEntry stores the number of instances of the given
   class Class. XProfilerEntry is used for debugging only. It helps to find
   out how much memory is occupied by the diverse classes. */
typedef struct  _XProfilerEntry
{
  struct _XProfilerEntry*    Next;
  struct _XProfilerEntry*    Prev;
  const struct _vmt_XObject* Class;
  int                        Count;
} XProfilerEntry;


/* ProfilerChain points to the begin of a double chained XProfilerEntry list. 
   The list stores the statistic information about the existing objects. */
static XProfilerEntry* ProfilerChain = 0;

/* This variable stores the number of bytes occupied by the profiler itself */
static int ProfilerMemory = 0;

/* These global variables store the number of bytes occupied by Chora objects */
int EwObjectsMemory     = 0;
int EwObjectsMemoryPeak = 0;
int EwNoOfObjects       = 0;

/* These global variables store the number of bytes occupied by strings */
int EwStringsMemory     = 0;
int EwStringsMemoryPeak = 0;
int EwNoOfStrings       = 0;

/* This global variable stores the number of bytes occupied by resources 
   like bitmaps and fonts. The variable EwOffScreenMemory contains the size
   of memory used for off-screen (frame-)buffers */
int EwResourcesMemory     = 0;
int EwResourcesMemoryPeak = 0;
int EwResourcesEvalMemory = 0;
int EwOffScreenMemory     = 0;
int EwOffScreenMemoryPeak = 0;

/* This global variable stores the max. number of bytes occupied in total */
int EwMemoryPeak = 0;


/* This counter tracks how many surfaces have been discarded since the
   preceding memory usage statistic. */
extern int EwNoOfDiscardedSurfaces;
extern int EwDiscardedSurfacesMemory;
extern int EwMaxSurfaceCacheSize;


/* Flag added to string magic numbers to sign the string as marked by garbage
   collection. Such strings are not disposed. */
#define EW_STRING_MARK_FLAG      0x8000


/* The function FindProfilerEntry() searches in the ProfilerChain for the entry
   which describes the given class and the source code location. If the given 
   entry does not exists, the function returns 0. */
static XProfilerEntry* FindProfilerEntry( const struct _vmt_XObject* aClass )
{
  XProfilerEntry* entry = ProfilerChain;

  /* Scan the list for the desired entry */
  while ( entry && ( entry->Class != aClass ))
    entry = entry->Next;

  /* Not found? */
  if ( !entry )
    return 0;

  /* Move the found entry to the beginning of the list - In this manner the
     often used entries are found much more quickly */
  if ( entry->Prev )
  {
    /* Remove the entry from its current position in the chain */
    entry->Prev->Next = entry->Next;

    if ( entry->Next )
      entry->Next->Prev = entry->Prev;

    /* Insert the entry at the beginning of the chain */
    entry->Next         = ProfilerChain;
    entry->Prev         = 0;
    ProfilerChain->Prev = entry;
    ProfilerChain       = entry;
  }

  return entry;
}


/* The function BuildProfilerChain() traverses the list of existing objects
   in order to build a list with the number of existing objects and the source 
   code location where these objects have been created. */
static void BuildProfilerChain( void )
{
  XObjectFrame* frame = Objects;

  /* Traverse the list of existing objects */
  while ( frame )
  {
    /* Already an entry for this instance? */
    XObject         object = (XObject)( frame + 1 );
    XProfilerEntry* entry  = FindProfilerEntry( object->_.VMT );

    /* If the entry still not exists in the ProfilerChain - create it now! */
    if ( !entry )
    {
      /* Reserve memory for the new entry and initialize it */
      entry = EwAlloc( sizeof( XProfilerEntry ));

      /* Out of memory? */
      if ( !entry )
      {
        EwError( 13 );
        EwPanic();
        return;
      }

      entry->Class = object->_.VMT;
      entry->Count = 0;

      /* Insert the new entry at the beginning of the ProfilerChain */
      entry->Next  = ProfilerChain;
      entry->Prev  = 0;

      if ( ProfilerChain )
        ProfilerChain->Prev = entry;

      ProfilerChain   = entry;
      ProfilerMemory += sizeof( XProfilerEntry );
    }

    /* Count the object ... */
    entry->Count++;

    /* Continue with the next object */
    frame = frame->Next;
  }
}


/* The function FreeProfilerChain() destroys the previously created profiler 
   chain and sets the ProfilerChain variable to zero */
static void FreeProfilerChain( void )
{
  XProfilerEntry* entry = ProfilerChain;

  /* Repeat until all entries are destroyed */
  while ( entry )
  {
    XProfilerEntry* tmp = entry;
    entry = entry->Next;
    EwFree( tmp );

    ProfilerMemory -= sizeof( XProfilerEntry );
  }

  /* Done! */
  ProfilerChain = 0;
}


/* The function SortProfilerChain() sorts the entries within the ProfilerChain
   by their 'Count' value. */
static void SortProfilerChain( void )
{
  int done;

  /* Repeat until all entries are sorted */
  do
  {
    XProfilerEntry* entry = ProfilerChain;
    done = 1;

    /* Traverse the whole chain */
    while ( entry )
    {
      /* Is the entry misplaced? */
      if ( entry->Prev && ( entry->Prev->Count < entry->Count ))
      {
        /* Remove the entry from its current position within the chain */
        entry->Prev->Next = entry->Next;

        if ( entry->Next )
          entry->Next->Prev = entry->Prev;

        entry->Next = entry->Prev;
        entry->Prev = entry->Next->Prev;
        entry->Next->Prev = entry;

        if ( entry->Prev )
          entry->Prev->Next = entry;
        else
          ProfilerChain = entry;

        /* The order within the chain has been modified - Repeat one more! */
        done = 0;
      }

      /* Continue with the next entry */
      entry = entry->Next;
    }
  }
  while ( !done );
}


/* The following structure describes an instance of an auto object. Several
   structures build a single chained list, which serves as the registry for
   existing auto objects */
typedef struct _XAutoObjectEntry
{
  struct _XAutoObjectEntry* Next;
  const XAutoObject*        Origin;
  XObject                   Object;
} XAutoObjectEntry;


/* AutoObjects points to the origin of the single chained list of existing
   auto objects. Each time a new auto object is instantiated, it is registered
   within this list. The next time, the auto object is accessed, the exitsing
   instance can be found in the list and used immediately. */
static XAutoObjectEntry* AutoObjects = 0;


/* The following function inserts the given entry in front of the list of
   existing auto objects. */
static void AddAutoObject( XAutoObjectEntry* aEntry )
{
  aEntry->Next = AutoObjects;
  AutoObjects  = aEntry;
}


/* The following function searches in the list of existing auto objects for
   an object created from the description aAutoObject. If a corresponding
   entry could be found, the function removes it from the list and returns
   it. If the entry could not be found, the function returns 0. */
static XAutoObjectEntry* FindAutoObject( const XAutoObject* aAutoObject )
{
  XAutoObjectEntry** entry = &AutoObjects;

  /* Look for the entry, registered for the object aAutoObject */
  while ( *entry )
  {
    /* The desired entry found? */
    if ((*entry)->Origin == aAutoObject )
    {
      XAutoObjectEntry* tmp = *entry;
      *entry = tmp->Next;

      return tmp;
    }  

    /* Continue with the next entry in the chain */
    else
      entry = &((*entry)->Next );
  }

  return 0;
}


/* Removes entries for auto objects, which are about to be disposed by the 
   Garbage Collector */
static void DisposeAutoObjects( void )
{
  XAutoObjectEntry** entry = &AutoObjects;
  XAutoObjectEntry*  tmp;

  /* Look for entries, which refer to unused objects */
  while ( *entry )
  {
    /* Verify, wheter the affected object has been marked by the Garbage
       Collector - If not, remove the entry */
    if ((*entry)->Object && !(*entry)->Object->_.Mark )
    {
      tmp    = *entry;
      *entry = tmp->Next;
      EwFree( tmp );

      /* Track the RAM usage */
      EwObjectsMemory -= sizeof( XAutoObjectEntry );
    }  

    /* Continue with the next entry in the chain */
    else
      entry = &((*entry)->Next );
  }
}


/* Verifies whether aObject points to an existing object which can be marked
   during the garbage collection. If successfull returns the most superior
   object containing aObject. Otherwise 0 ist returned. */
#ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
  static XObject CanMarkObject( XObject aObject )
  {
    XObjectFrame* frame  = Objects;
    XObject       object = 0;

    /* Search the list of existing objects for the one containing aObject */
    while ( 
            frame && 
            (
              ( aObject < ( object = (XObject)( frame + 1 ))) ||
              ( aObject >= (XObject)((char*)object + object->_.VMT->_Size ))
            )
          )
      frame = frame->Next;

    /* Could find a memory area containing the given address. Return the main
       object */
    return frame? object : 0;
  }
#endif


/*******************************************************************************
* METHOD:
*   XObject._Init
*
* DESCRIPTION:
*   The method _Init() will be invoked to initialize the newly created object 
*   instance 'this'. The invoking of the method gives the object a chance to 
*   initialize all user defined data fields and to prepare internal structures 
*   necessary to keep the object alive.
*
*   Because of the inheritance of classes it is necessary to invoke all _Init()
*   methods of all inherited classes to complete the initialization. For this
*   reason the _Init() method has to take care to invoke the inherited _Init() 
*   method before it can begin with the initialization.
*
*   If the object 'this' does aggregate other object instances, _Init() has to
*   take care to initialize these aggregated objects properly before _Init()
*   returns to the caller. In the same way _Init() has to reserve memory the 
*   object needs temporary (for example to store a string).
*   
* ARGUMENTS:
*   _this - Pointer to the object needs the initialization.
*   aLink - Pointer to an object which aggregates (embeds) 'this' object. 
*     If 'this' object is not aggregated: aLink == NULL.
*   aArg  - An optional 32 bit argument. The _Init() method passes aArg to the
*     constructor of the class and allows the programmer to access any data in
*     the body of the constructor.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void XObject__Init( XObject _this, XObject aLink, XHandle aArg )
{
  EW_UNUSED_ARG( aArg );

  _this->_.VMT  = &__vmt_XObject;
  _this->_.Link = aLink;
}


/*******************************************************************************
* METHOD:
*   XObject._ReInit
*
* DESCRIPTION:
*   The method _ReInit() will be invoked to re-initialize the object instance
*   this. The invoking of the method gets the object a chance to re-evaluate
*   all multilingual initialization expressions after the language has been
*   changed.
*
*   Because of the inheritance of classes it is necessary to invoke all 
*   _ReInit() methods of all inherited classes to complete the re-initialization
*   For this reason the _ReInit() method has to take care to invoke the 
*   inherited _ReInit() method before it can begin with the re-initialization.
*
*   If the object this does aggregate other object instances, _ReInit() has to
*   take care to re-initialize these aggregated objects properly before 
*   _ReInit() returns to the caller.
*   
* ARGUMENTS:
*   _this  - Pointer to the object needs the re-initialization.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void XObject__ReInit( XObject _this )
{
  EW_UNUSED_ARG( _this );

  /* Nothing to do in the XObject class. _ReInit() has to be overriden */
}


/*******************************************************************************
* METHOD:
*   XObject._Done
*
* DESCRIPTION:
*   The method _Done() will be invoked by the Garbage Collector to give the 
*   object 'this' a chance to deinitialize itself before the Garbage Collector 
*   frees the memory occupied by the object.
*
*   If the object 'this' aggregates other object instances, _Done() has to
*   take care to deinitialize these aggregated objects properly before _Done()
*   returns to the caller. In the same way _Done() has to free all the memory 
*   the object has reserved temporarily (for example to store a buffer).
*
*   Because of the inheritance of classes it is necessary to invoke all _Done()
*   methods of all inherited classes to complete the deinitialization. For this
*   reason the _Done() method has to take care to invoke the inherited _Done()
*   method.
*
*   The Garbage Collector guarantees to deinitialize only objects, which are not
*   in use anymore (garbage).
*
* ARGUMENTS:
*   _this - Pointer to the object to deinitialize.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void XObject__Done( XObject _this )
{
  EW_UNUSED_ARG( _this );

  /* Nothing to do in the XObject class. _Done() has to be overriden */
}


/*******************************************************************************
* FUNCTION:
*   EwSetStackBaseAddress
*
* DESCRIPTION:
*   The function EwSetStackBaseAddress() remembers the given value as start
*   address of the CPU stack used by Embedded Wizard application. Knowing this
*   the Garbage Collector can be started while the application is executed and
*   can evaluate stack contents in order to mark objects/strings stored on it
*   actually.
*
* ARGUMENTS:
*   aStackBaseAddress - Base address of the CPU stack used by Embedded Wizard
*     application.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwSetStackBaseAddress( void* aStackBaseAddress )
{
  #ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
    StackBaseAddress = (void**)aStackBaseAddress;
  #else
    EW_UNUSED_ARG( aStackBaseAddress );
    EwError( 314 );
  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwNewObject
*
* DESCRIPTION:
*   The function EwNewObject() implements a feature similar to the 'new' 
*   operator known from the programming language C++ and JAVA. EwNewObject() 
*   determinates the size of an object of the given class aClass, reserves the 
*   memory for the object and initializes the object properly.
*
*   The memory reservation will be done by the function EwAlloc(). If there is 
*   no memory available to create a new object of the desired aClass 
*   EwNewObject() will cause an error by calling the EwPanic() function.
*
*   After the memory could be reserved EwNewObject() invokes the _Init() 
*   method of the new object, so the object will get a chance to initialize 
*   itself. 
*
*   Note, that if there is no relationship between the newly created object 
*   and other objects stored in the root set, the object will be disposed by 
*   the next garbage colection. See EwReclaimMemory() for more details about 
*   the Garbage Collector and the root set.
*
* ARGUMENTS:
*   aClass - The name of the desired class to create a new object instance.
*   aArg   - An optional 32 bit argument. The EwNewObject() method passes aArg 
*     to the constructor of the class and allows the programmer to access any 
*     data in the body of the constructor.
*   aFile  - aFile stores the module name, where the object has been created.
*     aFile is for debugging purpose only.
*   aLine  - aLine stores the line number, at this the object has been created.
*     aLine is for debugging purpose only.
*
* RETURN VALUE:
*   EwNewObject() returns a pointer to the newly created object. 
*
*******************************************************************************/
XObject EwNewObjectIndirect( const void* aClass, XHandle aArg )
{
  const struct _vmt_XObject* vmt = aClass;
  XObjectFrame*              frame;
  XObject                    obj;

  /* Plausibility check */
  if ( !aClass )
  {
    EwError( 309 );
    EwPanic();
    return 0;
  }

  /* The specified 'aClass' is not a class */
  if ( vmt->_MagicNo != 0x434C4153 )
  {
    EwError( 312 );
    EwPanic();
    return 0;
  }

  /* Try to allocate the memory for the new object */
  do
    obj = (XObject)(( frame = EwAlloc( vmt->_Size + sizeof( XObjectFrame ))) + 1 );
  while ( !frame && EwImmediateReclaimMemory( 10 ));

  /* Allocation failed? */
  if ( !frame )
  {
    EwError( 10 );
    EwPanic();
    return 0;
  }

  /* Clear the reserved memory area */
  EwZero( obj, vmt->_Size );

  /* Before the object is initialized -> insert this object into the global chain */
  frame->Next = Objects;
  Objects     = frame;

  /* Initialize the new object */
  vmt->_Init( obj, 0, aArg );

  /* Track memory usage */
  EwObjectsMemory += vmt->_Size + sizeof( XObjectFrame );
  EwNoOfObjects++;

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return obj;
}


/*******************************************************************************
* FUNCTION:
*   EwAttachObjectVariant
*
* DESCRIPTION:
*   The function EwAttachObjectVariant() creates an object variant and attaches
*   it to the origin object aThis. At first the function verifies, whether the
*   class of the origin object has been overriden by a class variant. Depending
*   on the found class variant, a new object variant is created and initialized.
*   At the end, the pointer to the created object variant is assigned to the 
*   '_vthis' field of the origin object -> the object variant is attached to it.
*
*   For this purpose the function evaluates the list of variants defined by the
*   macros EW_DEFINE_CLASS_VARIANTS (see above). If the affected class has not 
*   been overriden by a class variant, or no variants in the list do correspond
*   to the currently active styles - the function returns to the caller and no
*   object variant is created.
*
*   The memory reservation will be done by the function EwAlloc(). If there is 
*   no memory available to create a new object variant, EwAttachObjectVariant()
*   will cause an error by calling the EwPanic() function.
*
*   After the memory could be reserved, the _Init() method of the new object
*   variant is called in order to give the object a chance to initialize itself
*   properly. The both argument aLink and aArg are then passed to the _Init()
*   method.
*
*   If the origin object is disposed, the attached object variants are also 
*   destroyed by calling the function EwDetachObjectVariant().
*
* ARGUMENTS:
*   aThis - Pointer to the object to attach an object variant.
*   aLink - Pointer to an object which aggregates (embed) aThis object. If 
*     aThis object is not aggregated: aLink == NULL.
*   aArg  - An optional 32 bit argument passed from the _Init() method of aThis
*     object.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwAttachObjectVariant( XObject aThis, XObject aLink, XHandle aArg )
{
  const struct _vmt_XObject* vvmt     = 0;
  const XClassVariant*       variants = aThis->_.VMT->_SubVariants;

  /* Evaluate the list of variants of the class of aThis object */
  while ( variants && variants->Variant )
  {
    while ( variants->Style && !( variants->Style & EwStylesSet ))
      variants++;

    /* A class variant found? Continue the evaluation in the list of sub-
       variants of the found class variant. */
    if ( variants->Variant )
    {
      vvmt     = variants->Variant;
      variants = vvmt->_SubVariants;
    }
  }

  /* Only, if there was a class variant found */
  if ( vvmt )
  {
    /* Determine the _vthis pointer of aThis object. The pointer lies at the 
       end of the super class. The size of the super class is stored in its 
       VMT. */
    int      vthisOfs = aThis->_.VMT->_Ancestor->_Size;
    XObject* vthisPtr = (XObject*)(void*)((char*)aThis + vthisOfs );
    XObject  obj;

    /* Reserve memory for the object variant. The size is stored in the VMT
       of the variant class */
    do
      obj = EwAlloc( vvmt->_Size );
    while ( !obj && EwImmediateReclaimMemory( 11 ));

    /* Allocation failed? */
    if ( !obj )
    {
      EwError( 11 );
      EwPanic();
      return;
    }

    /* Clear the reserved memory area */
    EwZero( obj, vvmt->_Size );

    /* Object variant is ready - attach it to the origin object ... */
    *vthisPtr = obj;

    /* ... and initialize it */
    vvmt->_Init( aThis, aLink, aArg );

    /* Track the RAM usage */
    EwObjectsMemory += vvmt->_Size;

    /* Also track the max. memory pressure */
    if ( EwObjectsMemory > EwObjectsMemoryPeak )
      EwObjectsMemoryPeak = EwObjectsMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwDetachObjectVariant
*
* DESCRIPTION:
*   The function EwDetachObjectVariant() detaches and destroys the object 
*   variant, which was previously attached to the origin object aThis by the 
*   function EwAttachObjectVariant().
*
*   The origin object aThis stores its object variant in the field '_vthis'. The
*   attached object variant is deinitialized by calling its _Done() Methode. 
*   After this, the memory occupied by the object variant is freed by EwFree().
*
* ARGUMENTS:
*   aThis - Pointer to the object to detach its object variant.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDetachObjectVariant( XObject aThis )
{
  /* Determine the _vthis pointer of aThis object. The pointer lies at the end 
     of the super class. The size of the super class is stored in its VMT. */
  int      vthisOfs = aThis->_.VMT->_Ancestor->_Size;
  XObject* vthisPtr = (XObject*)(void*)((char*)aThis + vthisOfs );

  /* Is there an object variant attached to the object? */
  if ( *vthisPtr )
  {
    /* Track the RAM usage */
    EwObjectsMemory -= (*vthisPtr)->_.VMT->_Size;

    /* Deinitialize the object variant ... */
    (*vthisPtr)->_.VMT->_Done( aThis );

    /* ... and free the memory occupied by the variant */
    EwFree( *vthisPtr );
    *vthisPtr = 0;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwGetAutoObject
*
* DESCRIPTION:
*   The function EwGetAutoObject() will be called from the automatic generated
*   'C' code in order to access an auto object identified by aAutoObject. The
*   function verifies whether the required object is already instantiated and 
*   if it's true, the existing instance is returned immediately. If no instance
*   of the desired auto object could be found, EwGetAutoObject() prepares a new
*   object of the class aClass and initializes it properly. The created object
*   will remains in memory as long as the object is in use. Objects, which are
*   not in use any more are automatically reclaimed by the Garbage Collector.
*
* ARGUMENTS:
*   aAutoObject - Pointer to the description of the auto object in the ROM code.
*   aClass      - The class of the auto object.
*   aFile       - aFile stores the module name, where the auto object has been
*     created. aFile is for debugging purpose only.
*   aLine       - aLine stores the line number, at this the auto object has 
*     been created. aLine is for debugging purpose only.
*
* RETURN VALUE:
*   Returns a pointer to the auto object.
*
*******************************************************************************/
XObject EwGetAutoObject( const XVariant* aAutoObject, const void* aClass )
{
  const XAutoObject* autoObject = 0;
  XAutoObjectEntry*  entry      = 0;

  /* No autoobject to find */
  if ( !aAutoObject )
    return 0;

  /* Traverse the derivation hierarchy of the variants and look for the one,
     which fits to the currently active styles */
  while ( aAutoObject )
  {
    const XSubVariant* subVariant = aAutoObject->SubVariants;

    /* Remember the found auto object */
    autoObject = aAutoObject->Content;

    /* If the variant does own sub variants, verify whether one of them does 
       fit to the currently active styles */
    if ( subVariant )
    {
      /* Repeat until all sub variants are evaluated or the evaluated sub variant
         does fit to the currently active styles set */
      while ( subVariant->Style && !( subVariant->Style & EwStylesSet ))
        subVariant++;

      /* No sub variant does fit to the styles set - Forget it! */
      if ( !subVariant->Variant )
        subVariant = 0;
    }

    /* A sub variant could been found - continue the evaluation with this sub
       variant */
    if ( subVariant )
      aAutoObject = subVariant->Variant;
    else
      aAutoObject = 0;
  }

  /* First look for the object in the list of existing auto objects. May be
     the object has been already created? Note, if an entry could be found,
     the function always removes it from the list. Don't forget to insert
     the entry back to the list!!! */
  entry = FindAutoObject( autoObject );

  /* An entry for the auto object could be found. Now verify, whether the 
     auto object has completed its initialization. As long as the auto object 
     performs the initialization, the 'entry->Object' remains zero. This avoid
     recursive accesses to an auto object, which is performing its initialization */
  if ( entry && !entry->Object )
  {
    EwErrorPD( 313, autoObject, 0 );
    EwPanic();
    return 0;
  }

  /* If the object still not exists, prepare a new entry */
  if ( !entry )
  {
    do
      entry = EwAlloc( sizeof( XAutoObjectEntry ));
    while ( !entry && EwImmediateReclaimMemory( 14 ));

    /* Out of memory? */
    if ( !entry )
    {
      EwError( 14 );
      EwPanic();
      return 0;
    }

    /* The entry remains 'empty' (Object == 0) till the object has been 
       completed its initialization. It is important in order to recognize 
       recursive accesses to the affected object during its initialization. */
    entry->Next   = 0;
    entry->Object = 0;
    entry->Origin = autoObject;

    /* Track the RAM usage */
    EwObjectsMemory += sizeof( XAutoObjectEntry );

    /* Also track the max. memory pressure */
    if ( EwObjectsMemory > EwObjectsMemoryPeak )
      EwObjectsMemoryPeak = EwObjectsMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  /* Insert the entry into the list and arrange it in front of all other 
     entries. In this manner, the next the entry will be found much more
     faster */
  AddAutoObject( entry );

  /* Create an instance of the auto object, if it not exists */
  if ( !entry->Object )
  {
    XObject object = EwNewObjectIndirect( aClass, 0 );

    /* Perform the objects initialization */
    autoObject->Init( object );

    /* Now, the object is ready for use. Complete the entry */
    entry->Object = object;
  }

  return entry->Object;
}


/*******************************************************************************
* FUNCTION:
*   EwCastObject
*
* DESCRIPTION:
*   The function EwCastObject() implements a feature known from the 
*   programming language C++ and called 'dynamic_cast<>()'. EwCastObject() 
*   verifies whether it is possible to cast the given object aObject to class 
*   aClass and returns the unchanged pointer aObject if the cast is allowed. 
*   In the case the cast is not legal EwCastObject() returns NULL.
*
*   The legal cast assumes that the class of the given object is derived from 
*   aClass or it is aClass itself. To determinate whether one class is derived
*   from another EwCastObject() evaluates the inheritance tree starting at the 
*   class of aObject. During the evaluation EwCastObject() will compare each 
*   found base class with aClass until the classes are the same or no other 
*   base classes exist.
*
* ARGUMENTS:
*   aObject - Pointer to the object which should be casted to aClass.
*   aClass  - The name of the desired class the object should be cast to.
*
* RETURN VALUE:
*   EwCastObject() returns aObject if there is a possibility to cast aObject to
*   aClass. If not, or if aObject is already NULL EwCastObject() returns NULL.
*
*******************************************************************************/
void* EwCastObject( XObject aObject, const void* aClass )
{
  const struct _vmt_XObject* vmt;

  /* Plausibility check */
  if ( !aClass )
  {
    EwError( 310 );
    EwPanic();
    return 0;
  }

  if ( !aObject )
    return 0;

  /* Get the pointer to the virtual method table associated to aObject */
  vmt = (const struct _vmt_XObject*)aObject->_.VMT;

  /* Traverse the inheritance tree. Is there a base class aClass? */
  while ( vmt && ( aClass != vmt ))
    vmt = vmt->_Ancestor;

  /* If there is no base class aClass -> aObject can not be casted to aClass */
  if ( !vmt )
    aObject = 0;

  return aObject;
}


/*******************************************************************************
* FUNCTION:
*   EwCastClass
*
* DESCRIPTION:
*   The function EwCastClass() works similar to the function EwCastObject()
*   with the difference, that EwCastClass() is always applied to Chora classes
*   whereas EwCastObject() to instances of the classes. 
*
*   EwCastClass() verifies whether it is possible to cast the given class aClass
*   to the class aDesiredClass and returns the unchanged pointer aClass if the
*   cast is allowed. In the case the cast is not legal EwCastClass() returns 
*   NULL.
*
*   The legal cast assumes that the class aClass is derived from aDesiredClass
*   or it is aDesiredClass itself. To determine whether one class is derived
*   from another EwCastClass() evaluates the inheritance tree starting at the 
*   class aClass. During the evaluation EwCastClass() will compare each found 
*   base class with aDesiredClass until the classes are the same or the root of
*   the inheritance tree has been reached.
*
* ARGUMENTS:
*   aClass        - Pointer to the class which should be casted to 
*     aDesiredClass.
*   aDesiredClass - The name of the desired class the object should be cast to.
*
* RETURN VALUE:
*   EwCastClass() returns aClass if there is a possibility to cast aClass to
*   aDesiredClass. If not, or if aClass is already NULL EwCastClass() returns
*   NULL.
*
*******************************************************************************/
XClass EwCastClass( XClass aClass, const void* aDesiredClass )
{
  const struct _vmt_XObject* vmt = (const struct _vmt_XObject*)aClass;

  /* Cast to generic class? Each class can be casted to the generic class. */
  if ( !aDesiredClass )
    return aClass;

  /* No class to cast? */
  if ( !aClass )
    return 0;

  /* Traverse the inheritance tree. Is there a base class aDesiredClass? */
  while ( vmt && ( aDesiredClass != vmt ))
    vmt = vmt->_Ancestor;

  /* If there is no base class -> aClass can not be casted to aDesiredClass */
  if ( !vmt )
    aClass = 0;

  return aClass;
}


/*******************************************************************************
* FUNCTION:
*   EwClassOf
*
* DESCRIPTION:
*   The function EwClassOf() determinates the class of an object given in the
*   argument aObject. The function returns a pointer to a global data structure
*   with the description of the class. If aObject == NULL, the function returns
*   NULL.
*
* ARGUMENTS:
*   aObject - Pointer to the object to determinate the class.
*
* RETURN VALUE:
*   EwClassOf() returns a pointer to the class structure or NULL, if aObject 
*   is NULL.
*
*******************************************************************************/
XClass EwClassOf( XObject aObject )
{
  if ( !aObject )
    return 0;

  return aObject->_.VMT;
}


/*******************************************************************************
* FUNCTION:
*   EwLockObject
*
* DESCRIPTION:
*   The function EwLockObject() adds the given object aObject to the global 
*   root set. Objects which are stored in the root set will be kept alive 
*   together with other objects depending from them.
*
*   Objects which are not stored in the root set and which are not in a 
*   relationship to another object stored in the root set are garbage and will
*   be disposed by the next garbage collection.
*
*   The size of the root set is limited to 16 entries. A try to lock 17 objects
*   will cause EwPanic() call.
*
*   If the passed argument aObject is NULL, EwLockObject() returns immediately.
*
* ARGUMENTS:
*   aObject - Pointer to the object which should be added to the root set.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwLockObject( XObject aObject )
{
  XRootSetItem* item;

  /* Nothing to do */
  if ( !aObject )
    return;

  /* Reserve memory for a new rootset item */
  do
    item = EwAlloc( sizeof( XRootSetItem ));
  while ( !item && EwImmediateReclaimMemory( 12 ));

  /* no memory available? */
  if ( !item )
  {
    EwError( 12 );
    EwPanic();
    return;
  }

  /* configure the new item */
  item->Object = aObject;

  /* insert the item into the set */
  item->Next = RootSetHead;
  item->Prev = 0;

  if ( RootSetHead )
    RootSetHead->Prev = item;

  RootSetHead = item;

  /* Track the RAM usage */
  EwObjectsMemory += sizeof( XRootSetItem );

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
}


/*******************************************************************************
* FUNCTION:
*   EwUnlockObject
*
* DESCRIPTION:
*   The function EwUnlockObject() removes the object aObject from the root set.
*   The object should be added previously to the root set by calling the 
*   function EwLockObject(). If it's not, EwUnlockObject() will cause EwPanic()
*   call.
*
*   After the object has been removed from the root set and if there is no other
*   relationship between the object and other objects still stored in the root 
*   set, the object will be disposed by the next garbage collection.
*   
*   If the passed argument aObject is NULL, EwUnlockObject() returns 
*   immediately.
*
* ARGUMENTS:
*   aObject - Pointer to the object which should be removed from the root set.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwUnlockObject( XObject aObject )
{
  XRootSetItem* item = RootSetHead;

  /* Nothing to do */
  if ( !aObject )
    return;

  /* Search the root set for an entry referring the object */
  while ( item && ( item->Object != aObject ))
    item = item->Next;

  /* If no entry found ... */
  if ( !item )
  {
    EwErrorPD( 311, aObject, 0 );
    EwPanic();
    return;
  }

  /* remove the item from the list */
  if ( item->Next )
    item->Next->Prev = item->Prev;

  if ( item->Prev )
    item->Prev->Next = item->Next;
  else
    RootSetHead = item->Next;

  /* release the rootset item */
  item->Next   = 0;
  item->Prev   = 0;
  item->Object = 0;
  EwFree( item );

  /* Track the RAM usage */
  EwObjectsMemory -= sizeof( XRootSetItem );
}


/* The function ReclaimMemory() perfomes the Garbage Collection in order to 
   dispose memory occuped by objects and strings which are not in use any more.
   The Garbage Collector implements the two phases algorithmus 'Mark and Sweep'. 

   In the first phase the function traverses the graph of object relationships
   and marks each reachable object and string. The task starts at the root set.
   Additionally the function evaluates values stored on the CPU stack lying
   in the range aCPUStackStart and aCPUStackEnd. Values refering potentially
   an object/string cause thus the affected objects and strings to be marked.

   In the second phase the function deinitializes and frees objects and strings
   which have not been marked during the first phase. In case of objects the 
   associated _Done() method of every affected object will be called, so the
   object will get a chance to shut down properly. Finally the memory used by
   the object or string is released.

   the function returns != 0 if there was at least one object/(string 
   released. */
static int ReclaimMemory( void** aCPUStackStart, void** aCPUStackEnd, 
  int aClearCaches )
{
  XRootSetItem*  rootSet = RootSetHead;
  XObjectFrame** objects = &Objects;
  XObject        first   = (XObject)1;
  XObject        current = 0;
  XObject        last    = 0;
  int            count   = 0;

  #ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
    int          none    = !aCPUStackStart && !aCPUStackEnd;
  #else
    int          none    = 1;
    EW_UNUSED_ARG( aCPUStackStart );
    EW_UNUSED_ARG( aCPUStackEnd );
  #endif

  /* Garbage collection is actually active */
  if ( ReclaimMemoryLock )
  {
    EwError( 316 );
    return 0;
  }

  /* The garbage collection starts now */
  ReclaimMemoryLock++;

  /* During the first phase collect all root set objects in the mark list */
  for ( ; rootSet; rootSet = rootSet->Next )
  {
    XObject object = rootSet->Object;
    
    /* If the object is already marked - ignore */
    if ( object->_.Mark )
      continue;

    /* Append the object to the end of the mark list */
    if ( last ) ( last = last->_.Mark = object )->_.Mark = (XObject)1;
    else        ( first = last = object        )->_.Mark = (XObject)1;

    /* At least one object in use */
    none = 0;
  }

  #ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
    /* The mark phase can also involve entries stored on the CPU stack. These
       values, however, are not guarantee to be valid pointers to objects or 
       strings. Therefore the function has to evaluate them carefully. */
    for ( ; aCPUStackStart && ( aCPUStackStart < aCPUStackEnd ); aCPUStackStart++ )
      if ( *aCPUStackStart && EwIsMemory( *aCPUStackStart ))
      {
        XChar*  string = (XChar*)*aCPUStackStart;
        XObject object = (XObject)*aCPUStackStart;

        /* Assuming aCPUStackStart points to a string -> does the string really
           exist and it is not yet marked? Then mark it now. */
        if (
             (( string = EwCanMarkString( string )) != 0 ) &&
             !( string[-1] & EW_STRING_MARK_FLAG )
           )
        {
          string[-1] |= EW_STRING_MARK_FLAG;
          none = 0;
        }

        /* Assuming aCPUStackStart points to an object -> does the object really
           exist and it is not yet marked? Then mark it now. */
        else if (
                  (( object = CanMarkObject( object )) != 0 ) &&
                  !object->_.Mark
                )
        {
          if ( last ) ( last = last->_.Mark = object )->_.Mark = (XObject)1;
          else        ( first = last = object        )->_.Mark = (XObject)1;

          /* At least one object in use */
          none = 0;
        }
      }
  #endif

  /* In case of the system shutdown - discard all pending signals now! */
  if ( none )
    EwDiscardSignals();

  /* Traverse the list with already marked objects and collect all other objects
     reachable from them */
  for ( current = first; current != (XObject)1; )
  {
    /* Evaluate every marked object */
    for ( ; current != (XObject)1; current = current->_.Mark )
    {
      XObject                    link  = current->_.Link;
      const struct _vmt_XObject* vmt   = current->_.GCT;
      unsigned int               mno   = vmt? vmt->_MagicNo : 0;
      char*                      start = (char*)current;

      /* If the object is embedded inside another object, mark the superior object */
      if ( link && !link->_.Mark )
        ( last = last->_.Mark = link )->_.Mark = (XObject)1;

      /* Process GC information provided for every of the object's basis classes.
         Note, if processing a variant, stop as soon as the VMT of the original 
         class has been reached */
      for ( ; vmt && ( vmt->_MagicNo == mno ); vmt = vmt->_Ancestor )
      {
        const int*           gcInfo   = vmt->_GCInfo;
        const XClassVariant* variants = vmt->_SubVariants;

        /* Does the class manage variants? Locate the corresponding variant object
           and mark it if necessary */
        if (( vmt->_MagicNo == 0x434C4153 ) && variants && variants->Variant )
        {
          /* Determine the _vthis pointer of the object. The pointer lies at the 
             end of the super class. */
          XObject vthis = *(XObject*)(void*)( start + vmt->_Ancestor->_Size );

          /* If the variant object is not yet marked, collect it */
          if ( vthis && !vthis->_.Mark )
            ( last = last->_.Mark = vthis )->_.Mark = (XObject)1;
        }

        /* Does the class contain data members storing simple pointers to other
           objects? */
        if ( gcInfo[0] && ( gcInfo[0] != gcInfo[1]))
        {
          int      endOfs = gcInfo[1]? gcInfo[1] : vmt->_Size;
          XObject* ptrs   = (XObject*)(void*)( start + gcInfo[0]);
          XObject* end    = (XObject*)(void*)( start + endOfs );

          /* Evaluate all affected data members and collected not yet marked objects */
          for ( ; ptrs < end; ptrs++ )
            if ( *ptrs && !(*ptrs)->_.Mark )
              ( last = last->_.Mark = *ptrs )->_.Mark = (XObject)1;
        }

        /* Does the class contain data members storing slots of other objects? */
        if ( gcInfo[1] && ( gcInfo[1] != gcInfo[2]))
        {
          int    endOfs = gcInfo[2]? gcInfo[2] : vmt->_Size;
          XSlot* slots  = (XSlot*)(void*)( start + gcInfo[1]);
          XSlot* end    = (XSlot*)(void*)( start + endOfs );

          /* Evaluate all affected data members and collected not yet marked objects */
          for ( ; slots < end; slots++ )
            if ( slots->Object && !((XObject)slots->Object)->_.Mark )
              ( last = last->_.Mark = (XObject)slots->Object )->_.Mark = (XObject)1;
        }

        /* Does the class contain data members storing references to properties? */
        if ( gcInfo[2] && ( gcInfo[2] != gcInfo[3]))
        {
          int   endOfs = gcInfo[3]? gcInfo[3] : vmt->_Size;
          XRef* refs   = (XRef*)(void*)( start + gcInfo[2]);
          XRef* end    = (XRef*)(void*)( start + endOfs );

          /* Evaluate all affected data members and collected not yet marked objects */
          for ( ; refs < end; refs++ )
            if ( refs->Object && !((XObject)refs->Object)->_.Mark )
              ( last = last->_.Mark = (XObject)refs->Object )->_.Mark = (XObject)1;
        }

        /* Does the class contain embedded objects? */
        if ( gcInfo[3] && ( gcInfo[3] != gcInfo[4]))
        {
          int   endOfs = gcInfo[4]? gcInfo[4] : vmt->_Size;
          char* objs   = start + gcInfo[3];
          char* end    = start + endOfs;

          /* Evaluate all embedded objects */
          while (( objs < end ) && ((XObject)objs)->_.GCT )
          {
            XObject obj = (XObject)objs;

            /* Mark the object if necessary */
            if ( !obj->_.Mark )
              ( last = last->_.Mark = obj )->_.Mark = (XObject)1;

            /* This object is actually performing its initialization. All other
               objects following it are till now not available. Stop. */
            if ( obj->_.GCT != obj->_.VMT )
              break;

            /* The next following embedded object */
            objs += obj->_.VMT->_Size;
          }
        }

        /* Does the class contain data members storing strings? */
        if ( gcInfo[4] && ( gcInfo[4] != gcInfo[5]))
        {
          int      endOfs = gcInfo[5]? gcInfo[5] : vmt->_Size;
          XString* strs   = (XString*)(void*)( start + gcInfo[4]);
          XString* end    = (XString*)(void*)( start + endOfs );

          /* Evaluate all affected data members and mark the strings are needed */
          for ( ; strs < end; strs++ )
            if ( *strs && !((*strs)[-1] & EW_STRING_MARK_FLAG ))
              (*strs)[-1] |= EW_STRING_MARK_FLAG;
        }
      }
    }

    /* All collected objects have been evaluated. Now verify whether there are
       dependencies from pending signals preventing other objects from being
       reclaimed. Collect them to. */
    current = last;
    last    = EwMarkSignals( last );

    /* No more objects collected */
    if ( current == last )
      break;
  }

  /* Phase 2: dispose observers, signals and auto objects, which belong to unused 
     objects */
  EwDisposeObservers();
  EwDisposeSignals();
  DisposeAutoObjects();

  /* Traverse the list of all existing objects. Objects, which have not been 
     marked are discarded now */
  while ( *objects )
  {
    XObjectFrame* frame  = *objects;
    XObject       object = (XObject)( frame + 1 );

    /* If the object has not been marked ... */
    if ( !object->_.Mark )
    {
      /* Take the object from the global list of existing objects */
      *objects = frame->Next;

      /* Track memory usage */
      EwObjectsMemory -= object->_.VMT->_Size + sizeof( XObjectFrame );
      EwNoOfObjects--;

      /* Deinitialize the object and free the memory occupied by it */
      object->_.VMT->_Done( object );
      EwFree( frame );

      /* An object was released */
      count++;
    }

    /* The object has been marked -> let it alive and continue with the next 
       object */
    else
      objects = &frame->Next;
  }
  
  /* Reset the mark state of all marked objects (incl. all embedded objects and
     variants) */
  while ( first && ( first != (XObject)1 ))
  {
    XObject object = first;
    first = first->_.Mark;
    object->_.Mark = 0;
  }

  /* Dispose all unmarked (unused) strings */
  count += EwDisposeStrings( none || aClearCaches );

  /* Similarly discard all unused surfaces */
  if ( aClearCaches )
    count += EwCleanSurfaceCache();

  /* Similarly release memory used for the decompress buffers */
  if ( none )
    EwReleaseDecompressBuffers();

  /* The garbage collection is finished */
  ReclaimMemoryLock--;

  return count;
}


/* The following functions are used internally during immediate garbage 
   collection. */
#ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
  /* The following definition is essential to avoid function inlining. If
     necessary enhance the definitions. */
  #if defined _MSC_VER
    #define EW_NO_INLINE __declspec(noinline)
  #elif defined __CC_ARM
    #define EW_NO_INLINE __attribute__((noinline))
  #elif defined __GNUC__
    #define EW_NO_INLINE __attribute__((noinline))
  #elif defined __CLANG__
    #define EW_NO_INLINE __attribute__((noinline))
  #elif defined __ICCARM__
    #define EW_NO_INLINE 
  #else
    #error "Missing 'no-inline' adaptation for the actual compiler."
  #endif

  EW_NO_INLINE static int ImmediateReclaimMemoryIntern2( void* aStackAddress )
  {
    void*  dummy    = 0;
    void** startPtr = &dummy;
    void** endPtr   = (void**)aStackAddress;

    EW_UNUSED_ARG( aStackAddress );

    /* Determine the stack area to evaluate during the immediate garbage 
       collection. Note, depending on the CPU architecture and compiler
       the stack may grows down or up. */
    if ( startPtr < StackBaseAddress )
      endPtr   = (void**)((unsigned long)StackBaseAddress & 
                           ~( sizeof( void* ) - 1 ));
    else
    {
      if ( startPtr > endPtr )
        endPtr = startPtr;

      startPtr = (void**)((unsigned long)StackBaseAddress & 
                           ~( sizeof( void* ) - 1 ));
    }

    /* Trigger the immediate garbage collection */
    return ReclaimMemory( startPtr, endPtr, 1 );
  }

  EW_NO_INLINE static int ImmediateReclaimMemoryIntern1( void )
  {
    void* regs[32];

    /* Save CPU register on the stack memory regs. */
    EwSaveRegister( &regs );

    /* Continue in the 'ewobject.c' module */
    return ImmediateReclaimMemoryIntern2( regs );
  }
#endif


/*******************************************************************************
* FUNCTION:
*   EwReclaimMemory
*
* DESCRIPTION:
*   The function EwReclaimMemory() invokes the Garbage Collector in order to 
*   dispose memory occuped by objects which are not used any more. The Garbage 
*   Collector implements the two phases algorithmus 'Mark and Sweep'. 
*
*   In the first phase EwReclaimMemory() distinguishes between live objects and 
*   the garbage. This is done by traversing the graph of object relationships. 
*   All reached objects will be marked. The traversing starts at the root set.
*   The user can add and remove objects to or from the root set by calling the
*   function EwLockObject() and EwUnlockObject(). As long as an object still
*   keeps in the root set, the Garbage Collector can not dispose the objects 
*   and all other objects standing in the relationship to the object.
*
*   In the second phase EwReclaimMemory() deinitializes and frees objects which
*   have not been marked during the first phase. The _Done() method of every 
*   affected object will be called, so the object will get a chance to shut down
*   properly. In the second step the EwFree() function will be called, so the 
*   memory occuped by the object will be released.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   The function returns != 0 if at least one Chora object or string has been 
*   released.
*
*******************************************************************************/
int EwReclaimMemory( void )
{
  return ReclaimMemory( 0, 0, 0 );
}


/*******************************************************************************
* FUNCTION:
*   EwImmediateReclaimMemory
*
* DESCRIPTION:
*   The function EwImmediateReclaimMemory() implements a second version of the
*   above EwReclaimMemory() function. While EwReclaimMemory() function can be
*   used when the GUI application is not executing any code only, this second
*   version may be called even during the execution of the GUI application.
*
*   This is so-called immediate grabage collection.
*
*   In order to not oversee any Chora objects nor strings stored actually in
*   local variables of the just interrupted GUI application, the function
*   EwImmediateReclaimMemory() evaluates the contents of the CPU stack as well
*   as the values of CPU register. As such the function needs to know the base
*   address of the CPU stack. This has to be provided at the start of the GUI 
*   application via an invocation of the function EwSetStackBaseAddress().
*
* ARGUMENTS:
*   aErrorCode - Error code identifying the orginal memory alloc operation 
*     which is failed causing the application to start the immediate garbage
*     collection. The number is used only to print a debug message.
*
* RETURN VALUE:
*   The function returns != 0 if at least one Chora object or string has been 
*   released.
*
*******************************************************************************/
int EwImmediateReclaimMemory( int aErrorCode )
{
  #ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
    int result;

    /* No stack information available. Starting garbage collection at this time
       could cause used objects and strings being released unexpectedly. */
    if ( !StackBaseAddress )
    {
      EwError( 315 );
      return 0;
    }

    /* Report that the system has not enough memory and an immediate
       garbage collection will start */
    if ( aErrorCode )
    {
      EwPrint( "Start immediate garbage collection due to out of memory "
               "error %d.\n", aErrorCode );
      EwPrint( "Memory usage before: %d bytes (Chora objects: %d, Strings: %d, "
               "Graphics Engine objects: %d)\n",
               EwObjectsMemory + EwStringsMemory + EwResourcesMemory,
               EwObjectsMemory, EwStringsMemory, EwResourcesMemory );
    }

    result = ImmediateReclaimMemoryIntern1();

    if ( aErrorCode )
      EwPrint( "Memory usage after:  %d bytes (Chora objects: %d, Strings: %d, "
               "Graphics Engine objects: %d)\n",
               EwObjectsMemory + EwStringsMemory + EwResourcesMemory,
               EwObjectsMemory, EwStringsMemory, EwResourcesMemory );

    return result;
  #else
    EW_UNUSED_ARG( aErrorCode );
    return 0;
  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwTestImmediateReclaimMemory
*
* DESCRIPTION:
*   The function EwTestImmediateReclaimMemory() implements a simple routine to
*   verify the function of immediate garbage collection. The implementation
*   creates few string objects and stores some of them in local variables. Then
*   the garbage collection is started via EwImmediateReclaimMemory(). After the
*   operation the function verifies whether the objects referenced by local
*   variable still exist and the object not referenced anymore are released.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   If the test is working the function returns 1. If some of needed objects
*   have been unexpectedly released, the function return 0 (bad error). If
*   some unused objects have not been released the function returns 2 (this
*   may occur).
*
*******************************************************************************/
int EwTestImmediateReclaimMemory( void )
{
  #ifdef EW_USE_IMMEDIATE_GARBAGE_COLLECTION
    XString str1 = EwNewStringAnsi( "abc" );
    XString str2 = EwNewStringAnsi( "abc" );
    XString str3 = EwNewStringAnsi( "abc" );
    XString str4 = EwNewStringAnsi( "abc" );
    XString str5 = EwNewStringAnsi( "abc" );
    XString str6 = EwNewStringAnsi( "abc" );
    XString str7 = EwNewStringAnsi( "abc" );
    XString str8 = EwNewStringAnsi( "abc" );
    XString str9 = EwNewStringAnsi( "abc" );
    XString strA = EwNewStringAnsi( "abc" );
    XString strB = EwNewStringAnsi( "abc" );
    XString strC = EwNewStringAnsi( "abc" );
    XString strD = EwNewStringAnsi( "abc" );
    XString strE = EwNewStringAnsi( "abc" );
    XString strF = EwNewStringAnsi( "abc" ) + 1; /* Simulate a possible pointer */
    XString strG = EwNewStringAnsi( "abc" ) + 2; /* ... inside a string */
    int     notAll = 0;

    /* None of the objects may be released */
    if ( EwImmediateReclaimMemory(0))
      return 0;

    /* Test whether all strings are still valid */
    if ( !EwCanMarkString( str1 )) return 0;
    if ( !EwCanMarkString( str2 )) return 0;
    if ( !EwCanMarkString( str3 )) return 0;
    if ( !EwCanMarkString( str4 )) return 0;
    if ( !EwCanMarkString( str5 )) return 0;
    if ( !EwCanMarkString( str6 )) return 0;
    if ( !EwCanMarkString( str7 )) return 0;
    if ( !EwCanMarkString( str8 )) return 0;
    if ( !EwCanMarkString( str9 )) return 0;
    if ( !EwCanMarkString( strA )) return 0;
    if ( !EwCanMarkString( strB )) return 0;
    if ( !EwCanMarkString( strC )) return 0;
    if ( !EwCanMarkString( strD )) return 0;
    if ( !EwCanMarkString( strE )) return 0;
    if ( !EwCanMarkString( strF )) return 0;
    if ( !EwCanMarkString( strG )) return 0;

    /* Now replace some of the strings */
    str1 = EwNewStringAnsi( "abc" );
    str3 = EwNewStringAnsi( "abc" );
    str6 = EwNewStringAnsi( "abc" );
    str9 = EwNewStringAnsi( "abc" );
    strC = EwNewStringAnsi( "abc" );
    strE = EwNewStringAnsi( "abc" );
    strG = EwNewStringAnsi( "abc" ) + 2; /* ... inside a string */

    notAll |= !EwImmediateReclaimMemory(0);
    
    /* Test whether some of the strings are still valid */
    if ( !EwCanMarkString( str1 )) return 0;
    if ( !EwCanMarkString( str2 )) return 0;
    if ( !EwCanMarkString( str3 )) return 0;
    if ( !EwCanMarkString( str4 )) return 0;
    if ( !EwCanMarkString( str5 )) return 0;
    if ( !EwCanMarkString( str6 )) return 0;
    if ( !EwCanMarkString( str7 )) return 0;
    if ( !EwCanMarkString( str8 )) return 0;
    if ( !EwCanMarkString( str9 )) return 0;
    if ( !EwCanMarkString( strA )) return 0;
    if ( !EwCanMarkString( strB )) return 0;
    if ( !EwCanMarkString( strC )) return 0;
    if ( !EwCanMarkString( strD )) return 0;
    if ( !EwCanMarkString( strE )) return 0;
    if ( !EwCanMarkString( strF )) return 0;
    if ( !EwCanMarkString( strG )) return 0;

    return notAll? 2 : 1;
  #else
    return 0;
  #endif
}


/*******************************************************************************
* FUNCTION:
*   EwReconstructObjects
*
* DESCRIPTION:
*   The function EwReconstructObjects() forces all currently existing objects
*   to execute their _ReInit() methods. The re-construction allows the objects
*   to update themselfe after the language has been changed.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwReconstructObjects( void )
{
  XObjectFrame* frame = Objects;

  /* Evaluate all objects */
  while ( frame )
  {
    XObject object = (XObject)( frame + 1 );

    /* Force the _ReInit() call */
    object->_.VMT->_ReInit( object );

    /* Continue with the next object */
    frame = frame->Next;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwPrintProfilerStatistic
*
* DESCRIPTION:
*   The function EwPrintProfilerStatistic() evaluates the list of existing
*   objects and prints out a statistic information with the amount of memory 
*   allocated by objects, strings and resources. Additionally, the current
*   total memory allocation and the maximum peak value is reported.
*   If the argument aDetailed is set to non-zero, a detailed object report is 
*   printed with all existing objects and their classes.
*
* ARGUMENTS:
*   aDetailed - Flag to switch on the detailed report.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwPrintProfilerStatistic( int aDetailed )
{
  XProfilerEntry* entry;

  /* Should display a list with classes and the number of corresponding 
     instances? */
  if ( aDetailed )
  {
    /* At first create anew profiler chain */
    BuildProfilerChain();
    SortProfilerChain();

    /* Start with the first entry */
    entry = ProfilerChain;

    /* Repeat until all entries are evaluated */
    while ( entry )
    {
      /* Print the detailed information about the entry */
      EwPrint( "%8d object(s) of class '%s' (each %d bytes large)\n", 
               entry->Count, entry->Class->_Name, entry->Class->_Size );

      /* Continue with the next entry */
      entry = entry->Next;
    }
  }

  /* At the end print the summarization */
  EwPrint( "\n" );
  EwPrint( "%8d bytes occupied by %d Chora objects (peak %d bytes)\n",
           EwObjectsMemory, EwNoOfObjects, EwObjectsMemoryPeak );
  EwPrint( "%8d bytes occupied by %d strings (peak %d bytes)\n", EwStringsMemory,
           EwNoOfStrings, EwStringsMemoryPeak );
  EwPrint( "%8d bytes occupied by Graphics Engine objects (without any framebuffers)"
           " (peak %d bytes)\n", EwResourcesMemory - EwOffScreenMemory,
           EwResourcesMemoryPeak - EwOffScreenMemoryPeak );

  if ( EwNoOfDiscardedSurfaces > 0 )
  {
    EwPrint( "%8d discarded surface(s) with totally %d bytes due to surface cache overflow\n         "
             "(surface cache is configured with EW_MAX_SURFACE_CACHE_SIZE=%d)\n",
             EwNoOfDiscardedSurfaces, EwDiscardedSurfacesMemory, EwMaxSurfaceCacheSize );
    EwNoOfDiscardedSurfaces   = 0;
    EwDiscardedSurfacesMemory = 0;
  }

  if ( EwResourcesEvalMemory > 0 )
    EwPrint( "%8d bytes occupied by 'watermark' images (used only in evaluation"
             " version)\n", EwResourcesEvalMemory );

  if ( EwOffScreenMemory > 0 )
    EwPrint( "%8d bytes occupied by the off-screen buffer (used as off-screen framebuffer)\n",
             EwOffScreenMemory );

  if ( ProfilerMemory > 0 )
    EwPrint( "%8d bytes occupied by the profiler itself\n", ProfilerMemory );

  EwPrint( "TOTAL : %d bytes, TOTAL PEAK : %d bytes\n", ProfilerMemory + 
           EwObjectsMemory + EwStringsMemory + EwResourcesMemory + 
           EwResourcesEvalMemory, EwMemoryPeak + ProfilerMemory + 
           EwResourcesEvalMemory );

  EwPrint( "\n" );

  /* Finally release the memory used eventually for collecting profiler info */
  FreeProfilerChain();
}


/* pba, msy */

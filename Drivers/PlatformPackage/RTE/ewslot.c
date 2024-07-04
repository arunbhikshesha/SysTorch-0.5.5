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
*   The module 'ewslot.c' contains a set of EWRTE functions purpose-built for 
*   operations with the Chora slot datatypes. Slots are very usefull to build a
*   sender and responder (slot) relationship. The sender will send signals to a
*   slot, so the owner of the slot (the responder) can process them.
*
*   Because an access to a slot causes a call to the slots own method, it is 
*   necessary that a single slot has to store one pointer to the method and one 
*   to the object itself, where the slot is defined in.
*
* SUBROUTINES:
*   EwNewSlot
*   EwCompSlot
*   EwIsSlotNull
*   EwSignal
*   EwPostSignal
*   EwIdleSignal
*   EwProcessSignals
*   EwAnyPendingSignals
*   EwAttachObjObserver
*   EwAttachRefObserver
*   EwAttachObserver
*   EwDetachObjObserver
*   EwDetachRefObserver
*   EwDetachObserver
*   EwNotifyObjObservers
*   EwNotifyRefObservers
*   EwNotifyObservers
*
*******************************************************************************/

#include "ewrte.h"


/* Undo the macro overloading of following functions */
#undef EwNewSlot


/* EwNullSlot stores a constant 'null' slot, so it is possible to use it
   immediatelly in assignments or comparations */
const XSlot EwNullSlot = { 0, 0 };


/* The following code is used for debbuging only. */
extern int EwObjectsMemory;
extern int EwObjectsMemoryPeak;
extern int EwStringsMemory;
extern int EwResourcesMemory;
extern int EwMemoryPeak;


/* The following structure is used internally for storing of a pending signal.
   The pending signals are managed in a single linked chain. */
struct XPendingSignal
{
  struct XPendingSignal* Next;
  XSlot                  Slot;
  XObject                Sender;
  int                    Processed;
};


/* The following structure is used internally for storing a registered observer.
   The observers are managed in a single linked chain. */
struct XObserver
{
  struct XObserver* Next;
  XSlot             Slot;
  XObject           Object;
  XRefGetSetProc    Property;
  XUInt32           Id;
};


/* The both variables points to the begin of the corresponding list of pending
   signals. New signals are added to a list by using the AddSignal() function.
   The signals are delivered when the ProcessSignals() function is called. */
static struct XPendingSignal* PostSignals = 0;
static struct XPendingSignal* IdleSignals = 0;


/* The following variables store the origin of lists of registered observers.
   New observers are added to the begin of the list by using the functions
   EwAttachObjObserver() and EwAttachRefObserver(). */
static struct XObserver* Observers    = 0; /* global observers   */
static struct XObserver* ObjObservers = 0; /* object observers   */
static struct XObserver* RefObservers = 0; /* reference observer */


/* This internal function is called, when a new signal aSlot should be stored
   for a delayed delivery. The pending signals are delivered, when the function
   ProcessSignals() is called.
   The new signal is stored at the end of the list aList. If the signal to the
   slot is already registered within the list, the affected XPendingSignal object
   is moved to the end of the list. If the affected signal is already delivered,
   so its 'Processed' flag is set true, the function call failes and returns 0.
   Otherwise != 0 is returned. */
static int AddSignal( struct XPendingSignal** aList, XSlot aSlot, 
  XObject aSender )
{
  struct XPendingSignal** list = aList;
  struct XPendingSignal*  signal;

  /* Step1: Try to find the already registered signal */
  while ( *aList && (((*aList)->Slot.SlotProc != aSlot.SlotProc ) ||
          ((*aList)->Slot.Object != aSlot.Object )))
    aList = &((*aList)->Next );

  /* If the found signal is already delivered, discard the new signal. Otherwise
     endless processing will be possible */
  if ( *aList && (*aList)->Processed )
    return 0;

  /* Remove the found signal from the list (it will be appended at the end of
     the list */
  if ( *aList )
  {
    signal = *aList;
    *aList = signal->Next;
    signal->Next = 0;
  }

  /* Or, if the affected signal is still not stored in the list - create and 
     initialize a new XPendingSignal object */
  else
  {
    /* Try to allocate memory for thge new signal. Please note: in the case, the
       allocation fails, we will eventually trigger the Garbage Collection. This,
       can cause the actual found end of the list with pending signals to be 
       cleared. Thus for the later list append operation we need to start at the
       begin of the list again. */
    do
      if (( signal = EwAlloc( sizeof( struct XPendingSignal ))) == 0 )
        aList = list;
    while ( !signal && EwImmediateReclaimMemory( 4 ));

    /* Out of memory? */
    if ( !signal )
    {
      EwError( 4 );
      EwPanic();
      return 0;
    }

    signal->Next      = 0;
    signal->Processed = 0;
    signal->Slot      = aSlot;

    /* Track the RAM usage */
    EwObjectsMemory += sizeof( struct XPendingSignal );

    /* Also track the max. memory pressure */
    if ( EwObjectsMemory > EwObjectsMemoryPeak )
      EwObjectsMemoryPeak = EwObjectsMemory;

    if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
      EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;
  }

  /* Step2: Look for the end of the list and ... */
  while ( *aList )
    aList = &((*aList)->Next );

  /* ... store the signal there */
  *aList = signal;

  /* Update the lates sender of the signal */
  signal->Sender = aSender;

  /* Successfully */
  return 1;
}


/* This internal function has the job to send signals to all slots registered
   within the list aList. */
static void ProcessSignals( struct XPendingSignal** aList )
{
  /* Repeat until all signals are delivered */
  while ( *aList )
  {
    /* Get the current signal. */
    struct XPendingSignal* signal = *aList;

    /* Before we call the slot method, we sign this signal as 'delivered'.
       This is important to avoid endless signals */
    signal->Processed = 1;

    /* Now, the signal may be delivered */
    signal->Slot.SlotProc( signal->Slot.Object, signal->Sender );

    /* Skip to the next signal ... */
    *aList = (*aList)->Next;

    /* Track the RAM usage */
    EwObjectsMemory -= sizeof( struct XPendingSignal );

    /* ... and remove the processed signal now */
    EwFree( signal );
  }
}


/* This function removes all pending signals from the given list aList. */
static void ClearSignals( struct XPendingSignal** aList )
{
  /* Repeat until all signals are removed from the list */
  while ( *aList )
  {
    /* Remember the current signal and ... */
    struct XPendingSignal* signal = *aList;

    /* ... continue with the next signal */
    *aList = (*aList)->Next;

    /* Track the RAM usage */
    EwObjectsMemory -= sizeof( struct XPendingSignal );

    /* Remove the currenty signal */
    EwFree( signal );
  }
}


/* This function removes all unused observers from the given list. */
static void DisposeObservers( struct XObserver** aList )
{
  struct XObserver** obs = aList;
  struct XObserver*  tmp;

  /* Look for observers, which are attached to unused objects */
  while ( *obs )
  {
    /* Verify, wheter the affected objects has been marked by the Garbage
       Collector - If not, remove the observer */
    if (
         !((XObject)((*obs)->Slot.Object ))->_.Mark ||
         ((*obs)->Object && !(*obs)->Object->_.Mark )
       )
    {
      /* Track the RAM usage */
      EwObjectsMemory -= sizeof( struct XObserver );

      tmp  = *obs;
      *obs = tmp->Next;
      EwFree( tmp );
    }  

    /* Continue with the next observer in the chain */
    else
      obs = &((*obs)->Next );
  }
}


/* This function removes all unused signals from the given list. */
static void DisposeSignals( struct XPendingSignal** aList )
{
  struct XPendingSignal** sig = aList;
  struct XPendingSignal*  tmp;

  /* Look for signals, which are directed to unused objects */
  while ( *sig )
  {
    /* Verify, wheter the affected objects has been marked by the Garbage
       Collector - If not, remove the signal */
    if (
         (
           !((XObject)((*sig)->Slot.Object ))->_.Mark ||
           ((*sig)->Sender && !(*sig)->Sender->_.Mark )
         ) &&
         !(*sig)->Processed
       )
    {
      /* Track the RAM usage */
      EwObjectsMemory -= sizeof( struct XPendingSignal );

      tmp  = *sig;
      *sig = tmp->Next;
      EwFree( tmp );
    }  

    /* Continue with the next signal in the chain */
    else
      sig = &((*sig)->Next );
  }
}


/* The function EwMarkSignals() will be called internally in order to mark the 
   sender objects of all currently pending signals as reachable. This function
   is used by the garbage collection only. This, however, occurs only when the
   target object is marked and the sender not. In other words, if the target
   object is unused, the sender and the signal can be removed.

   The marked objects are appended to the end of a list specified in aMarkList
   parameter. The function returns the last appended object or aMarkList if no
   objects needed to be marked. */
XObject EwMarkSignals( XObject aMarkList )
{
  struct XPendingSignal** list = &PostSignals;

  /* Repeat until all signals are evaluated */
  while ( *list )
  {
    if ((*list)->Sender && !(*list)->Sender->_.Mark &&
       ((XObject)(*list)->Slot.Object)->_.Mark )
      ( aMarkList = aMarkList->_.Mark = (*list)->Sender )->_.Mark = (XObject)1;

    /* Continue with the next signal */
    else
      list = &((*list)->Next );
  }

  list = &IdleSignals;

  /* Repeat until all signals are evaluated */
  while ( *list )
  {
    if ((*list)->Sender && !(*list)->Sender->_.Mark &&
       ((XObject)(*list)->Slot.Object)->_.Mark )
      ( aMarkList = aMarkList->_.Mark = (*list)->Sender )->_.Mark = (XObject)1;

    /* Continue with the next signal */
    else
      list = &((*list)->Next );
  }

  return aMarkList;
}


/* This function EwDiscardSignals() will be called internally in order to remove
   all pending signals. */
void EwDiscardSignals( void )
{
  ClearSignals( &PostSignals );
  ClearSignals( &IdleSignals );
}


/* Removes unused observers */
void EwDisposeObservers( void )
{
  DisposeObservers( &Observers    );
  DisposeObservers( &ObjObservers );
  DisposeObservers( &RefObservers );
}


/* Removes unused signals */
void EwDisposeSignals( void )
{
  DisposeSignals( &PostSignals );
  DisposeSignals( &IdleSignals );
}


/*******************************************************************************
* FUNCTION:
*   EwNewSlot
*
* DESCRIPTION:
*   The function EwNewSlot() initializes a new XSlot structure with the given
*   arguments aObject, aSlotProc and returns the structure.
*
* ARGUMENTS:
*   aObject   - Pointer to an object which contains the slot.
*   aSlotProc - Pointer to the slots own method.
*
* RETURN VALUE:
*   Returns an initialized XSlot structure, describing a reference to a slot.
*
*******************************************************************************/
XSlot EwNewSlot( void* aObject, XSlotProc aSlotProc )
{
  XSlot s;

  s.Object   = aObject;
  s.SlotProc = aSlotProc;

  return s;
}


/*******************************************************************************
* FUNCTION:
*   EwCompSlot
*
* DESCRIPTION:
*   The function EwCompSlot() compares the given slots aSlot1 and aSlot2 and 
*   returns a value == 0 if the slots are identical. Otherwise the function 
*   returns a value != 0.
*
*   EwCompSlot() implements the Chora instant operators: 'slot == slot' and
*   'slot != slot'.
*
* ARGUMENTS:
*   aSlot1 - The first slot.
*   aSlot2 - The second slot.
*
* RETURN VALUE:
*   Returns zero if both slots are equal.
*
*******************************************************************************/
int EwCompSlot( XSlot aSlot1, XSlot aSlot2 )
{
  if ( aSlot1.Object   != aSlot2.Object   ) return 1;
  if ( aSlot1.SlotProc != aSlot2.SlotProc ) return 1;

  return 0;
}


/*******************************************************************************
* FUNCTION:
*   EwIsSlotNull
*
* DESCRIPTION:
*   The function EwIsSlotNull() returns != 0 if the given slot aSlot does not
*   refer any valid slot (it is NULL). Otherwise the function returns 0.
*
* ARGUMENTS:
*   aSlot - Slot to verify.
*
* RETURN VALUE:
*   Returns != 0 if the slot is NULL.
*
*******************************************************************************/
XBool EwIsSlotNull( XSlot aSlot )
{
  return !aSlot.Object && !aSlot.SlotProc;
}


/*******************************************************************************
* FUNCTION:
*   EwSignal
*
* DESCRIPTION:
*   The function EwSignal() will be called in order to deliver a signal to a
*   slot aSlot. In response to the delivery the function EwSignal() calls slots
*   own method SlotProc.
*
* ARGUMENTS:
*   aSlot   - Slot to send a signal.
*   aSender - Who is the sender of the signal?
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwSignal( XSlot aSlot, XObject aSender )
{
  /* Don't signal 'null' slots */
  if ( aSlot.Object )
    aSlot.SlotProc( aSlot.Object, aSender );
}


/*******************************************************************************
* FUNCTION:
*   EwPostSignal
*
* DESCRIPTION:
*   The function EwPostSignal() will be called in order to store a signal for a
*   delayed delivery. The affected slot aSlot will receive the signal as soon as
*   possible, but always before the screen update is performed.
*
* ARGUMENTS:
*   aSlot   - Slot to receive a signal.
*   aSender - Who is the sender of the signal?
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwPostSignal( XSlot aSlot, XObject aSender )
{
  /* Don't signal 'null' slots */
  if ( aSlot.Object )
    if ( !AddSignal( &PostSignals, aSlot, aSender ))
      EwError( 308 );
}


/*******************************************************************************
* FUNCTION:
*   EwIdleSignal
*
* DESCRIPTION:
*   The function EwIdleSignal() will be called in order to store a signal for a
*   delayed delivery. The affected slot aSlot will receive the signal as soon as
*   possible, but always after the screen update is performed.
*
* ARGUMENTS:
*   aSlot   - Slot to receive a signal.
*   aSender - Who is the sender of the signal?
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwIdleSignal( XSlot aSlot, XObject aSender )
{
  /* Don't signal 'null' slots */
  if ( aSlot.Object )
    AddSignal( &IdleSignals, aSlot, aSender );
}


/*******************************************************************************
* FUNCTION:
*   EwProcessSignals
*
* DESCRIPTION:
*   The function EwProcessSignals() will be called in order to deliver the 
*   signals, which are pending at the moment. The function returns a value, 
*   which indicates whether signals have been delivered or not. If no signals
*   were pending, the function returns 0. In the case, at least one signal has
*   been delivered, != 0 is returned.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   If no signals were delivered, the function returns 0. Otherwise != 0 is
*   returned.
*
*******************************************************************************/
int EwProcessSignals( void )
{
  int anySignals = ( PostSignals != 0 );

  /* At first the 'posted' signals are delivered and the list is cleared */
  ProcessSignals( &PostSignals );
  ClearSignals( &PostSignals );

  /* Next time, when EwProcessSignals() is called, the currently pending 'idle' 
     signals will be delivered. */
  PostSignals = IdleSignals;
  IdleSignals = 0;

  return anySignals;
}


/*******************************************************************************
* FUNCTION:
*   EwAnyPendingSignals
*
* DESCRIPTION:
*   The function EwAnyPendingSignals() returns a value != 0, if there are any
*   signals pending for the delivery.
*
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   The function returns a value != 0, if there are signals pending for the
*   delivery.
*
*******************************************************************************/
int EwAnyPendingSignals( void )
{
  return ( PostSignals != 0 ) || ( IdleSignals != 0 );
}


/*******************************************************************************
* FUNCTION:
*   EwAttachObjObserver
*
* DESCRIPTION:
*   The function EwAttachObjObserver() implements the statement 'attachobserver'
*   of the Chora language. The function registers a slot method aSlot with the 
*   object aObject. As soon the object has been triggered by the function
*   EwNotifyObjObservers(), all registered slot methods will receive a signal
*   automatically.
*
*   In order to deregister an observer, the function EwDetachObjObserver() 
*   should be used. Please note, that the Garbage Collector will automatically
*   detach an observer from the object, if the affected object or the observer
*   itself is about to be disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be attached as an observer to the
*     object aObject. If aSlot is NULL, the function returns immediately.
*   aObject - The destination object for the registration. If this parameter
*     is NULL, the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers of one and the same object (aObject). This is a
*     kind of subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be registered.
*   If an identic registration already exists, the function simply returns 0.
*
*******************************************************************************/
int EwAttachObjObserver( XSlot aSlot, XObject aObject, XUInt32 aId )
{
  struct XObserver* obs = ObjObservers;

  /* No observer to register */
  if ( !aSlot.Object || !aObject )
    return 0;

  /* Ensure, that the observer is not registered yet */
  while ( obs && 
          (
            ( obs->Slot.Object != aSlot.Object ) || 
            ( obs->Slot.SlotProc != aSlot.SlotProc ) ||
            ( obs->Object != aObject ) ||
              obs->Property || 
            ( obs->Id != aId )
          )
        )
    obs = obs->Next;

  /* If the observer is already registered ... */
  if ( obs )
    return 0;

  /* Ok, the observer is not registered. Create a new XObserver */
  do
    obs = EwAlloc( sizeof( struct XObserver ));
  while ( !obs && EwImmediateReclaimMemory( 5 ));

  /* Out of memory? */
  if ( !obs )
  {
    EwError( 5 );
    EwPanic();
    return 0;
  }

  /* Prepare the new observer and insert it into the Observers chain */
  obs->Slot     = aSlot;
  obs->Object   = aObject;
  obs->Property = 0;
  obs->Id       = aId;
  obs->Next     = ObjObservers;
  ObjObservers  = obs;

  /* Track the RAM usage */
  EwObjectsMemory += sizeof( struct XObserver );

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwAttachRefObserver
*
* DESCRIPTION:
*   The function EwAttachRefObserver() implements the statement 'attachobserver'
*   of the Chora language. The function registers a slot method aSlot with the 
*   property referred by aRef. As soon the property has been triggered by the 
*   function EwNotifyRefObservers(), all registered slot methods will receive a
*   signal automatically.
*
*   In order to deregister an observer, the function EwDetachRefObserver() 
*   should be used. Please note, that the Garbage Collector will automatically
*   detach an observer from the property, if the object, the property belongs
*   to or the observer itself is about to be disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be attached as an observer to the
*     property referred by aRef. If aSlot is NULL, the function returns 
*     immediately.
*   aRef    - The reference to the destination property for the registration.
*     If this parameter is NULL, the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers of one and the same property referred by aRef. This
*     is a kind of subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be registered.
*   If an identic registration already exists, the function simply returns 0.
*
*******************************************************************************/
int EwAttachRefObserver( XSlot aSlot, XRef aRef, XUInt32 aId )
{
  struct XObserver* obs = RefObservers;

  /* No observer to register */
  if ( !aSlot.Object || !aRef.Object )
    return 0;

  /* Ensure, that the observer is not registered yet */
  while ( obs && 
          (
            ( obs->Slot.Object != aSlot.Object ) || 
            ( obs->Slot.SlotProc != aSlot.SlotProc ) ||
            ( obs->Object != aRef.Object ) ||
            ( obs->Property != aRef.OnGet ) || 
            ( obs->Id != aId )
          )
        )
    obs = obs->Next;

  /* If the observer is already registered ... */
  if ( obs )
    return 0;

  /* Ok, the observer is not registered. Create a new XObserver */
  do
    obs = EwAlloc( sizeof( struct XObserver ));
  while ( !obs && EwImmediateReclaimMemory( 6 ));

  /* Out of memory? */
  if ( !obs )
  {
    EwError( 6 );
    EwPanic();
    return 0;
  }

  /* Prepare the new observer and insert it into the Observers chain */
  obs->Slot     = aSlot;
  obs->Object   = aRef.Object;
  obs->Property = aRef.OnGet;
  obs->Id       = aId;
  obs->Next     = RefObservers;
  RefObservers  = obs;

  /* Track the RAM usage */
  EwObjectsMemory += sizeof( struct XObserver );

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwAttachObserver
*
* DESCRIPTION:
*   The function EwAttachObserver() implements the statement 'attachobserver'
*   of the Chora language. The function registers a slot method aSlot with the
*   global notifier. As soon the the global notifier has been triggered by the
*   function EwNotifyObservers(), all registered slot methods will receive a 
*   signal automatically.
*
*   In order to deregister an observer, the function EwDetachObserver() should 
*   be used. Please note, that the Garbage Collector will automatically detach 
*   an observer from the notifier, if the affected observer is about to be 
*   disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be attached as an observer to the
*     global notifier. If aSlot is NULL, the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers. This is a kind of subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be registered.
*   If an identic registration already exists, the function simply returns 0.
*
*******************************************************************************/
int EwAttachObserver( XSlot aSlot, XUInt32 aId )
{
  struct XObserver* obs = Observers;

  /* No observer to register */
  if ( !aSlot.Object )
    return 0;

  /* Ensure, that the observer is not registered yet */
  while ( obs && 
          (
            ( obs->Slot.Object != aSlot.Object ) || 
            ( obs->Slot.SlotProc != aSlot.SlotProc ) ||
              obs->Object ||
              obs->Property || 
            ( obs->Id != aId )
          )
        )
    obs = obs->Next;

  /* If the observer is already registered ... */
  if ( obs )
    return 0;

  /* Ok, the observer is not registered. Create a new XObserver */
  do
    obs = EwAlloc( sizeof( struct XObserver ));
  while ( !obs && EwImmediateReclaimMemory( 7 ));

  /* Out of memory? */
  if ( !obs )
  {
    EwError( 7 );
    EwPanic();
    return 0;
  }

  /* Prepare the new observer and insert it into the Observers chain */
  obs->Slot     = aSlot;
  obs->Object   = 0;
  obs->Property = 0;
  obs->Id       = aId;
  obs->Next     = Observers;
  Observers     = obs;

  /* Track the RAM usage */
  EwObjectsMemory += sizeof( struct XObserver );

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwDetachObjObserver
*
* DESCRIPTION:
*   The function EwDetachObjObserver() implements the statement 'detachobserver'
*   of the Chora language. The function deregisters a slot method aSlot from the 
*   object aObject. This function is the counterpart of EwAttachObjObserver().
*
*   Please note, that the Garbage Collector will automatically detach an 
*   observer from the object, if the affected object or the observer itself is 
*   about to be disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be detached from the object aObject.
*     If aSlot is NULL, the function returns immediately.
*   aObject - The affected object for the deregistration. If aObject is NULL, 
*     the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers of one and the same object (aObject). This is a
*     kind of subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be deregistered.
*   If no corresponding registration exists, the function simply returns 0.
*
*******************************************************************************/
int EwDetachObjObserver( XSlot aSlot, XObject aObject, XUInt32 aId )
{
  struct XObserver** obs = &ObjObservers;
  struct XObserver*  tmp;

  /* No observer to deregister */
  if ( !aSlot.Object || !aObject )
    return 0;

  /* Search in the chain for the affected observer */
  while ( *obs && 
          (
            ((*obs)->Slot.Object != aSlot.Object ) || 
            ((*obs)->Slot.SlotProc != aSlot.SlotProc ) ||
            ((*obs)->Object != aObject ) ||
             (*obs)->Property || 
            ((*obs)->Id != aId )
          )
        )
    obs = &((*obs)->Next );

  /* The desired observer could not be found - it has not been registered ... */
  if ( !*obs )
    return 0;

  /* Ok, the observer could be found. First remove the observer from the chain */
  tmp  = *obs;
  *obs = tmp->Next;

  /* Track the RAM usage */
  EwObjectsMemory -= sizeof( struct XObserver );

  /* ... then free the memory occupied by the XObserver structure */
  EwFree( tmp );

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwDetachRefObserver
*
* DESCRIPTION:
*   The function EwDetachRefObserver() implements the statement 'detachobserver'
*   of the Chora language. The function deregisters a slot method aSlot from the 
*   property referred by aRef. This function is the counterpart of the function
*   EwAttachRefObserver().
*
*   Please note, that the Garbage Collector will automatically detach an 
*   observer from the property, if the object, the property belongs to or the 
*   observer itself is about to be disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be detached from the property referred
*     by aRef. If aSlot is NULL, the function returns immediately.
*   aRef    - The reference to the affected property for the deregistration.
*     If aRef is NULL, the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers of one and the same property. This is a kind of 
*     subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be deregistered.
*   If no corresponding registration exists, the function simply returns 0.
*
*******************************************************************************/
int EwDetachRefObserver( XSlot aSlot, XRef aRef, XUInt32 aId )
{
  struct XObserver** obs = &RefObservers;
  struct XObserver*  tmp;

  /* No observer to deregister */
  if ( !aSlot.Object || !aRef.Object )
    return 0;

  /* Search in the chain for the affected observer */
  while ( *obs && 
          (
            ((*obs)->Slot.Object != aSlot.Object ) || 
            ((*obs)->Slot.SlotProc != aSlot.SlotProc ) ||
            ((*obs)->Object != aRef.Object ) ||
            ((*obs)->Property != aRef.OnGet ) ||
            ((*obs)->Id != aId )
          )
        )
    obs = &((*obs)->Next );

  /* The desired observer could not be found - it has not been registered ... */
  if ( !*obs )
    return 0;

  /* Ok, the observer could be found. First remove the observer from the chain */
  tmp  = *obs;
  *obs = tmp->Next;

  /* Track the RAM usage */
  EwObjectsMemory -= sizeof( struct XObserver );

  /* ... then free the memory occupied by the XObserver structure */
  EwFree( tmp );

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwDetachObserver
*
* DESCRIPTION:
*   The function EwDetachObserver() implements the statement 'detachobserver'
*   of the Chora language. The function deregisters a slot method aSlot from the 
*   global notifier. This function is the counterpart of EwAttachObserver().
*
*   Please note, that the Garbage Collector will automatically detach an 
*   observer from the notifier, if the affected observer itself is about to be 
*   disposed.
*
* ARGUMENTS:
*   aSlot   - The slot method, which will be detached from the object aObject.
*     If aSlot is NULL, the function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers. This is a kind of subdivision.
*
* RETURN VALUE:
*   The function returns a value != 0, if the observer could be deregistered.
*   If no corresponding registration exists, the function simply returns 0.
*
*******************************************************************************/
int EwDetachObserver( XSlot aSlot, XUInt32 aId )
{
  struct XObserver** obs = &Observers;
  struct XObserver*  tmp;

  /* No observer to deregister */
  if ( !aSlot.Object )
    return 0;

  /* Search in the chain for the affected observer */
  while ( *obs && 
          (
            ((*obs)->Slot.Object != aSlot.Object ) || 
            ((*obs)->Slot.SlotProc != aSlot.SlotProc ) ||
             (*obs)->Object ||
             (*obs)->Property || 
            ((*obs)->Id != aId )
          )
        )
    obs = &((*obs)->Next );

  /* The desired observer could not be found - it has not been registered ... */
  if ( !*obs )
    return 0;

  /* Ok, the observer could be found. First remove the observer from the chain */
  tmp  = *obs;
  *obs = tmp->Next;

  /* Track the RAM usage */
  EwObjectsMemory -= sizeof( struct XObserver );

  /* ... then free the memory occupied by the XObserver structure */
  EwFree( tmp );

  return 1;
}


/*******************************************************************************
* FUNCTION:
*   EwNotifyObjObservers
*
* DESCRIPTION:
*   The function EwNotifyObjObservers() implements the Chora statement 
*   'notifyobservers'. The function posts signals for each observer, which 
*   has been previously registered with the given object aObject.
*
*   In order to register an observer the function EwAttachObjObserver() should
*   be used.
*
* ARGUMENTS:
*   aObject - The object to trigger the observers. If aObject is NULL, the 
*     function returns immediately.
*   aId     - This is an additional ID, which distinguish between different
*     groups of observers of one and the same object (aObject). This is a
*     kind of subdivision.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwNotifyObjObservers( XObject aObject, XUInt32 aId )
{
  struct XObserver* obs = ObjObservers;

  /* Nothing to do */
  if ( !aObject )
    return;

  /* Look for observers registered with the given object and id */
  while ( obs )
  {
    /* If the condition is fulfilled, the observer should receive a signal */
    if (
         ( obs->Object == aObject ) &&
           !obs->Property && 
         ( obs->Id == aId )
       )
      EwPostSignal( obs->Slot, 0 );

    /* Continue with the next observer in the chain */
    obs = obs->Next;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwNotifyRefObservers
*
* DESCRIPTION:
*   The function EwNotifyRefObservers() implements the Chora statement 
*   'notifyobservers'. The function posts signals for each observer, which 
*   has been previously registered with the property referred by aRef.
*
*   In order to register an observer the function EwAttachRefObserver() should
*   be used.
*
* ARGUMENTS:
*   aRef - The reference to the property to trigger the observers. If Ref is 
*     NULL, the function returns immediately.
*   aId  - This is an additional ID, which distinguish between different
*     groups of observers of one and the same property. This is a kind of 
*     subdivision.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwNotifyRefObservers( XRef aRef, XUInt32 aId )
{
  struct XObserver* obs = RefObservers;

  if ( !aRef.Object )
    return;

  /* Look for observers registered with the given property and id */
  while ( obs )
  {
    /* If the condition is fulfilled, the observer should receive a signal */
    if (
         ( obs->Object == aRef.Object ) &&
         ( obs->Property == aRef.OnGet ) &&
         ( obs->Id == aId )
       )
      EwPostSignal( obs->Slot, 0 );

    /* Continue with the next observer in the chain */
    obs = obs->Next;
  }
}


/*******************************************************************************
* FUNCTION:
*   EwNotifyObservers
*
* DESCRIPTION:
*   The function EwNotifyObservers() implements the statement 'notifyobservers'
*   of the Chora language. The function posts signals for each observer, which 
*   has been previously registered with the global notifier.
*
*   In order to register an observer the function EwAttachObserver() should be 
*   used.
*
* ARGUMENTS:
*   aId     - This is an ID, which distinguish between different groups of 
*     observers. This is a kind of subdivision.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwNotifyObservers( XUInt32 aId )
{
  struct XObserver* obs = Observers;

  /* Look for observers registered with the given object and id */
  while ( obs )
  {
    /* If the condition is fulfilled, the observer should receive a signal */
    if (
         !obs->Object &&
         !obs->Property && 
         ( obs->Id == aId )
       )
      EwPostSignal( obs->Slot, 0 );

    /* Continue with the next observer in the chain */
    obs = obs->Next;
  }
}


/* pba, msy */

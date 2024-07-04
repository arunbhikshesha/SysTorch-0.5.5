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
*   The module 'ewtimer.c' contains a set of EWRTE functions purpose-built for 
*   timer operations. 
*
* SUBROUTINES:
*   EwCreateTimer
*   EwDestroyTimer
*   EwStartTimer
*   EwResetTimer
*   EwProcessTimers
*   EwNextTimerExpiration
*
*******************************************************************************/

#include "ewrte.h"


/* Timers points to the begin of the global timers list. */
static XTimer* Timers = 0;


/* The following code is used for debbuging only.  */
extern int EwObjectsMemory;
extern int EwObjectsMemoryPeak;
extern int EwStringsMemory;
extern int EwResourcesMemory;
extern int EwMemoryPeak;


/* This helper function returns the tick counter as a 64 bit number. */
static XTicks GetClockTicks( void )
{
  static XTicks  current = { 0, 0 };
  XUInt32        ticks   = (XUInt32)EwGetTicks();

  /* Check for the 32 bit counter overflow */
  if ( ticks < current.Lo )
    current.Hi++;

  current.Lo = ticks;
  return current;
}


/* This helper function adds the aDelta value to the 64 bit tick counter aTick.
   For this purpose the 64 bit arithmetic is used. */
static XTicks AddTicks( XTicks aTicks, XInt32 aDelta )
{
  XUInt32 lo = aTicks.Lo + aDelta;

  /* Convert signed doubleword to quadword by extending the most significant
     bit. */
  if ( aDelta < 0 )
    aTicks.Hi--;

  /* Addition with carry? */
  if ( lo < aTicks.Lo )
    aTicks.Hi++;

  aTicks.Lo = lo;
  return aTicks;
}


/* This helper function subtracts the aTicks2 ticks counter from aTicks1 and
   returns the delta as 32 bit signed number. The cast to 32 bit can cause a 
   data loss! */
static XInt32 SubTicks( XTicks aTicks1, XTicks aTicks2 )
{
  return (XInt32)( aTicks1.Lo - aTicks2.Lo );
}


/* This helper function compares the both 64 bit tick counters and returns one
   of the following values:
   -1 : aTicks1 <  aTicks2
    0 : aTicks1 == aTicks2
   +1 : aTicks1 >  aTicks2 */
static int CmpTicks( XTicks aTicks1, XTicks aTicks2 )
{
  XUInt32 result = aTicks1.Hi - aTicks2.Hi;

  if ( !result )
    result = aTicks1.Lo - aTicks2.Lo;

  return (int)result;
}


/*******************************************************************************
* FUNCTION:
*   EwCreateTimer
*
* DESCRIPTION:
*   The function EwCreateTimer() initializes an new timer and inserts it into 
*   the global timer list Timers.
*   
* ARGUMENTS:
*   aProc - Entry point of an user defined function. The function will be each
*     time the timer is expired. The function will be called in context of the
*     function EwProcessTimers() only.
*   aArg  - Optional 32 bit argument to pass in the timer procedure aProc.
*
* RETURN VALUE:
*   Returns a pointer to a new timer.
*
*******************************************************************************/
XTimer* EwCreateTimer( XTimerProc aProc, XHandle aArg )
{
  /* Reserve memory for the new timer */
  XTimer* timer;

  do
    timer = EwAlloc( sizeof( XTimer ));
  while ( !timer && EwImmediateReclaimMemory( 1 ));

  /* Failed? */
  if ( !timer )
  {
    EwError( 1 );
    EwPanic();
    return 0;
  }

  /* Initialize the new timer and ... */
  timer->Proc         = aProc;
  timer->Arg          = aArg;
  timer->Enabled      = 0;
  timer->Ticks.Lo     = 0;
  timer->Ticks.Hi     = 0;
  timer->InitialTime  = 0;
  timer->RepeatTime   = 0;

  /* Append the new timer to the global timers list. */
  timer->Next = Timers;
  Timers = timer;

  /* Track the RAM usage */
  EwObjectsMemory += sizeof( XTimer );

  /* Also track the max. memory pressure */
  if ( EwObjectsMemory > EwObjectsMemoryPeak )
    EwObjectsMemoryPeak = EwObjectsMemory;

  if (( EwObjectsMemory + EwStringsMemory + EwResourcesMemory ) > EwMemoryPeak )
    EwMemoryPeak = EwObjectsMemory + EwStringsMemory + EwResourcesMemory;

  return timer;
}


/*******************************************************************************
* FUNCTION:
*   EwDestroyTimer
*
* DESCRIPTION:
*   The function EwDestroyTimer() removes the timer from the timers list and
*   frees the memory reserved for the timer.
*   
* ARGUMENTS:
*   aTimer - Pointer to the timer to destroy.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwDestroyTimer( XTimer* aTimer )
{
  XTimer* tmp = Timers;

  /* At first stop the timer if it is running now */
  EwResetTimer( aTimer );

  /* If the timer lies at the begin of the list ... */
  if ( Timers == aTimer )
    Timers = aTimer->Next;

  /* ... or try to find it in the list */ 
  else
  {
    while ( tmp && ( tmp->Next != aTimer ))
      tmp = tmp->Next;

    /* And remove it from the list */
    if ( tmp )
      tmp->Next = aTimer->Next;

    /* Oups, the timer has not been found within the list -> This is an internal
       error! */
    else
    {
      EwErrorPD( 301, aTimer, 0 );
      EwPanic();
      return;
    }
  }

  /* Track the RAM usage */
  EwObjectsMemory -= sizeof( XTimer );

  /* Finished, we can free the memory reserved for the timer */
  EwFree( aTimer );
}


/*******************************************************************************
* FUNCTION:
*   EwStartTimer
*
* DESCRIPTION:
*   The function EwStartTimer() starts the timer aTimer with the given number
*   of milliseconds aDelay. If the timer is already started, the function
*   reinitializes the timer with a new expiration time.
*   
* ARGUMENTS:
*   aTimer       - Pointer to the timer to start.
*   aInitialTime - The number of milliseconds for the first time expiration.
*   aRepeatTime  - The number of milliseconds between two timer expirations.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwStartTimer( XTimer* aTimer, XInt32 aInitialTime, XInt32 aRepeatTime )
{
  /* At first stop the running timer */
  EwResetTimer( aTimer );

  if ( aInitialTime < 0 )
    aInitialTime = 0;

  if ( aRepeatTime < 0 )
    aRepeatTime = 0;

  if ( !aInitialTime )
    aInitialTime = aRepeatTime;

  /* The timer still keep deactivated */
  if ( !aInitialTime )
    return;

  /* Initialize the timer */
  aTimer->InitialTime = aInitialTime;
  aTimer->RepeatTime  = aRepeatTime;

  /* Calculate the first timer expiration and start it */
  aTimer->Ticks   = AddTicks( GetClockTicks(), aInitialTime );
  aTimer->Enabled = 1;
}


/*******************************************************************************
* FUNCTION:
*   EwResetTimer
*
* DESCRIPTION:
*   The function EwResetTimer() stops the given timer aTimer.
*   
* ARGUMENTS:
*   aTimer - Pointer to the timer to be stopped.
*
* RETURN VALUE:
*   None
*
*******************************************************************************/
void EwResetTimer( XTimer* aTimer )
{
  /* Is the timer running? */
  if ( !aTimer->Enabled )
    return;

  /* Stop the running timer */
  aTimer->Enabled = 0;

  /* The timer has been deactivated */
  aTimer->InitialTime  = 0;
  aTimer->RepeatTime   = 0;
  aTimer->Ticks.Lo     = 0;
  aTimer->Ticks.Hi     = 0;
}


/*******************************************************************************
* FUNCTION:
*   EwProcessTimers
*
* DESCRIPTION:
*   The function EwProcessTimers() should be called in order to process all
*   pending timer events. EwProcessTimers() traverses the global timers list
*   and invokes all timer procedures assigned to timers which are expired.
*   After all pending timer events are evaluated, EwProcessTimers() clears
*   all pending flags.
*
*   The function returns a value != 0, if at least one timer has expired.
*   
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   If at least one timer is expired, the function returns a value != 0. 
*   If no timers are expired, 0 is returned.
*
*******************************************************************************/
int EwProcessTimers( void )
{
  XTimer* tmp     = Timers;
  XTicks  ticks   = GetClockTicks();
  int     expired = 0;

  /* Traverse the timers stored in the global list and search for expired 
     timers */
  while ( tmp )
  {
    /* Is the timer expired? */
    if ( tmp->Enabled && ( CmpTicks( ticks, tmp->Ticks ) >= 0 ))
    {
      /* Should the timer be restarted? */
      if ( tmp->RepeatTime > 0 )
      {
        tmp->Ticks = AddTicks( tmp->Ticks, tmp->RepeatTime );

        /* If the system is too slow, some timer expirations are lost */
        if ( CmpTicks( ticks, tmp->Ticks ) >= 0 )
        {
          XInt32 delta = SubTicks( ticks, tmp->Ticks ) % tmp->RepeatTime;
          tmp->Ticks   = AddTicks( ticks, -delta );
          tmp->Ticks   = AddTicks( tmp->Ticks, tmp->RepeatTime );
        }
      }

      /* This was a single shot timer. So after the first expiration the job is 
         done and we can reset this timer. */
      else
      {
        tmp->Enabled      = 0;
        tmp->InitialTime  = 0;
        tmp->RepeatTime   = 0;
        tmp->Ticks.Lo     = 0;
        tmp->Ticks.Hi     = 0;
      }

      /* The timer is expired - call the registered callback function */
      tmp->Proc( tmp->Arg );
      expired++;
    } 

    /* Continue with next */
    tmp = tmp->Next;
  }

  return expired;
}


/*******************************************************************************
* FUNCTION:
*   EwNextTimerExpiration
*
* DESCRIPTION:
*   The function EwNextTimerExpiration() evaluates the list of pending timers
*   in order to find out the timer which should expire next time. The function
*   returns the number of miliseconds till the expiration of the found timer.
*
*   Please note: Because of the multitasking environment, the returned delay
*   value can become very inprecise! For this reason you should never use the 
*   determined number of miliseconds as a parameter for a 'sleep()' or similar 
*   function. This can cause the timers to expire very unprecise.
*   
* ARGUMENTS:
*   None
*
* RETURN VALUE:
*   Returns the delay in milliseconds for the next timer expiration.
*
*******************************************************************************/
int EwNextTimerExpiration( void )
{
  XTimer* tmp   = Timers;
  XTicks  ticks = GetClockTicks();
  XInt32  delta = 0x7FFFFFFF;

  /* Traverse the timers stored in the global list and search for the timer
     which should expire the next time */
  while ( tmp )
  {
    if ( tmp->Enabled && ( SubTicks( tmp->Ticks, ticks ) < delta ))
      delta = SubTicks( tmp->Ticks, ticks );

    /* Continue with next */
    tmp = tmp->Next;
  }

  /* The found timer is already behind schedule! */
  if ( delta < 0 )
    delta = 0;

  return delta;
}


/* pba, msy */

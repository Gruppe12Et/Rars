/**
 * OS.CPP - all non-graphic platform dependent code is in here:
 *
 * This version is for DOS on a PC and uses Borland functions.
 * History
 *  ver. 0.46 3/25/95 (of RARS)
 *  ver. 0.6b 5/8/95 b for beta
 *  ver. 0.61 May 26
 *  ver  0.70 by Maido Remm (-sr flag)
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <dir.h>
#include <dos.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include "track.h"
#include "os.h"

//--------------------------------------------------------------------------
//                            G L O B A L S
//--------------------------------------------------------------------------

static int no_waiting = 0;   // get_ch() doesn't wait if this is set

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Gets an operator input from the keyboard or mouse button.
 * Waits, polling, if there is no data available.
 *
 * @return  The key pressed
 */
int get_ch()
{
  static int got = ' ';

  if(no_waiting)        // we don't wait for data if this is set
  {
    if(kbhit())
    {
      if(getch() == ESC)    // We will respond to the escape key
      {
        got = ESC;
      }
    }
    return got;
  }
  got = getch();
  if(got)             // The PC returns 0 first for some keys, then an ID byte
  {
    return(got);
  }
  else
  {
    return(256 * getch());    // we will return the ID times 256
  }
}

/**
 * Returns 0 only if there is no data available
 * for get_ch().  If kb_hit() returns non-zero, that
 * means that get_ch() will return data immediately.
 */
int kb_hit()
{
   return kbhit();
}

/**
 * This routine must monitor a hardware periodic signal in order to
 * return at the next "clock tick".  The period of the clock must be
 * assigned to delta_time in OS.H.  This version uses the IBM PC clock
 * and delta_time is accordingly set at .0549 sec.  For other platforms, 
 * delta_time should be between about .05 and .1 second.
 * This routine must be called once with a non-zero argument to initialize it.
 * After that it is called with no argument, or with zero.
 */
void one_tick(int initialize = 0)
{
  static struct dostime_t t;        // used by Borland _dos_gettime() function
  static unsigned char pre_sec;
  static unsigned char pre_h;

  if(initialize)  
  {
    _dos_gettime(&t);
    pre_sec = t.second;
    pre_h   = t.hsecond;
    return;
  }
  // wait for next tick of PC clock
  do 
  {
    _dos_gettime(&t);
  } 
  while(pre_sec == t.second && pre_h == t.hsecond);
  pre_sec = t.second;
  pre_h   = t.hsecond;
}

/**
 * Gets a long integer "randomly" from system clock.
 * This version returns the clock value in (1/20) sec.
 *
 * @return a random number
 */
long pick_random()
{
  struct dostime_t t;

  _dos_gettime(&t);
  return(t.hsecond/5 + 20 * t.second + 1200 * (long)t.minute);
}

/**
 * Returns the number of available 1 K RAM blocks
 *
 * @return memory available [KB]
 */
int RAM_query()
{
   // This will use up to 64 K bytes of RAM; it doesn't de-allocate them
   // Use this only just before exiting the program.
   for(int rami=0; rami<64; rami++)  {     // we don't consider more than 64
      char *ptr;
      ptr = new char[1024];                // try to allocate 1 K of RAM
      if(!ptr)                             // if unsucessful, 
     break;                               // leave the loop & return
   }
   return rami;
}

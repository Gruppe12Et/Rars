/**
 * OS.CPP - all non-graphic platform dependent code is in here:
 *
 * This version is for UNIX platforms, not X specific.
 * History
 *  ver. 0.46 3/25/95 (of RARS)
 *  ver. 0.6b 5/8/95 by Randy Saint (saint@phoenix.net)
 *  ver. 0.61 May 26
 *  ver  0.70 by Maido Remm (-sr flag)
 *  ver. 0.71 (-q, -p modified)
 *  ver. 0.72 (-l)
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <time.h>
#include "os.h"
#include "globals.h"
#include "draw.h"
#include "misc.h"
#include "race_manager.h"

// UNIX specific include files
#include <sys/time.h>

//--------------------------------------------------------------------------
//                            E X T E R N S
//--------------------------------------------------------------------------

int getch();                // getch defined in gi.cpp
int kbhit();

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

static int no_waiting = 0;   // GetCh() doesn't wait if this is set

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

double random(int rand_max)
{
  if (rand_max < RAND_MAX)
        return((double)(rand()%rand_max));
  else
        return(0.0);
}

/**
 * Gets an operator input from the keyboard or mouse button.
 * Waits, polling, if there is no data available.
 *
 * @return  The key pressed
 */
int Os::GetCh()       // Gets an operator input from the keyboard or mouse button.
{                  // Waits, polling, if there is no data available.
  static int got = ' ';
  
  if(no_waiting)       // we don't wait for data if this is set
  {
    if(KbHit())
    {
      if(getch() == ESC)    // We will respond only to the escape key
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
 * for GetCh().  If kb_hit() returns non-zero, that
 * means that GetCh() will return data immediately
 */
int Os::KbHit()
{
  if( !draw.m_bDisplay )
  {
    return(0);
  }
  else
  {
    return(kbhit());   // Have to use the X input...
  }
}

/**
 * This routine is so that the KB is polled much less frequently when 
 * we are trying to speed up the simulation, as indicated by the no_display 
 * and real_speed global variables. 
 *
 * @return   Returns non_zero when kb should be polled
 */
static int do_kb()
{ 
  static int kb_count = 0;
 
  ++kb_count; 
  if(!draw.m_bDisplay)
  {
    if(kb_count >= 500) 
    {
      kb_count = 0; 
      return 1; 
    } 
    else 
    {
      return 0;
    }
  }
  else if(draw.m_iFastDisplay)
  {
    if(kb_count >= 10) 
    {
      kb_count = 0; 
      return 1; 
    }
    else 
    {
      return 0; 
    }
  }
  // This is when there is a display and the speed is realistic 
  kb_count = 0; 
  return 1; 
}

/**
 * Returns 0 if it is ok
 * Return 1 if ESC has been pressed -> race has to be stopped
 */
int RaceManager::Keyboard()
{
  int c;
  if( !draw.m_bDisplay )
  {
    return 0;
  }

  // This section is for keyboard check and appropriate response:
  if(do_kb())   // check the keyboard rarely if trying to go fast
  {
    if(os.KbHit())    // respond to the keyboard if its hit
    {
      if((c=os.GetCh()) == ESC)     // ESC key will end the race.
      {
        return(1);
      }
      else if( draw.m_bDisplay )
      {
        if(c == UP)   // up and down arrow designate next car.
        {
          if (draw.m_iCarShown < 0 || draw.m_iCarShown == race_data.m_aCarInPos[0])
          {
            draw.m_iCarShown = -1;   // if was first car,
            draw.Instruments();      // IP disappears
            draw.Leaders(0,race_data.m_aCarInPos); // restore name color
          }
          else                      // jump to previous in race_data.m_aCarInPos
          {
            draw.m_iCarShown = race_data.m_aCarInPos[race_data.m_aPosOfCar[draw.m_iCarShown]-1];
            draw.InitNewData();
          }
        }
        else if(c == DOWN)
        {
          if (draw.m_iCarShown < 0)        // if IP wasn't up,
          {
            draw.m_iCarShown = race_data.m_aCarInPos[0];    // show leader
            draw.Leaders(0,race_data.m_aCarInPos);      // to change name color
          }
          else if(draw.m_iCarShown == race_data.m_aCarInPos[args.m_iNumCar-1]) // if was last car
            draw.m_iCarShown = race_data.m_aCarInPos[args.m_iNumCar-1];  // keep this car on IP
          else
          {
            draw.m_iCarShown = race_data.m_aCarInPos[race_data.m_aPosOfCar[draw.m_iCarShown]+1];
            draw.InitNewData();
          }
        }
        else if ( c == 'S' ) // S cycles between skidmarks/no skidmarks
        {
          draw.m_bDrawTrajectory = !draw.m_bDrawTrajectory;
        }
        else if ( c == 's' )
        {
          draw.m_iFastDisplay =  0;
        }
        else if ( c == 'f' || c == 'F' )
        {
          draw.m_iFastDisplay =  4;
        }
        else if ( c == 'a' || c == 'A' )
        {
          draw.m_iFastDisplay = -1;
        }
        else if ( c == 'd') // cycle leaderboard mode up
        {
          if( ++draw.m_iLeaderboardMode==8 )
            draw.m_iLeaderboardMode = 0;
          draw.PrintLeaderboardHeader();
          draw.InitNewData();
        }
        else if ( c == 'D') // cycle data-mode down
        {
          if( --draw.m_iLeaderboardMode<0 )
            draw.m_iLeaderboardMode = 7;
          draw.PrintLeaderboardHeader();
          draw.InitNewData();
        }
        else if( c == 'r' || c == 'R' )
        {
          m_oInstantReplay.m_iMode = INSTANT_REWIND;
        }
        else if( c == 'e' || c == 'E' )
        {
          m_oInstantReplay.Stop();
        }
        else if( c == 'p' || c == 'P' )
        {
          if( m_oInstantReplay.m_iMode==INSTANT_STOP && m_oInstantReplay.m_iMode!=INSTANT_RECORD )
          {
            m_oInstantReplay.m_iMode = INSTANT_REPLAY;
          }
          else
          {
            m_oInstantReplay.Stop();
          }
        }
      }
    }
  }

  // Instant Replay messages
  static int iLastMode = -1;
  if( m_oInstantReplay.m_iMode!=iLastMode )
  {
    if( m_oInstantReplay.m_iMode==INSTANT_STOP )
    {
      os.ShowInitMessage( "Simulation paused - press 'P'" );
    }
    else if( m_oInstantReplay.m_iMode!=INSTANT_RECORD )
    {
      os.ShowInitMessage( "Instant replay" );
    }
    else
    {
      draw.ScoreBoard( race_data.stage, 0 );
    }
    iLastMode = m_oInstantReplay.m_iMode;
  }

  return(0);
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
void Os::OneTick( int initialize=0 )
{
  if (initialize)
    return;

#ifdef __CYGWIN__
  struct timeval timeout;
  timeout.tv_sec = 0;                          // Wait for 0 seconds...
  timeout.tv_usec = (long)(1000.0*delta_time); // ... and some fractions
  select(1,0,0,0,&timeout);
#else
  struct timespec ts;
  ts.tv_sec = 0;
  ts.tv_nsec = 2000000;
  nanosleep(&ts, 0);
#endif
}

/**
 * Gets a long integer "randomly" from system clock.
 * This version returns the clock value in sec
 *
 * @return a random number
 */
long Os::PickRandom()
{
  return(time(0) % 32000L);
}

/** 
 * Returns the number of available 1 K RAM blocks
 */
int Os::RamQuery()
{
  int rami;
  // This will use up to 64 K bytes of RAM; it doesn't de-allocate them
  // Use this only just before exiting the program.
  for(rami=0; rami<64; rami++)         // we don't consider more than 64
  {
    char *ptr;
    ptr = new char[1024];              // try to allocate 1 K of RAM
    if(!ptr)                           // if unsucessful,
    {
      break;                           // leave the loop & return
    }
  }
  return rami;
}
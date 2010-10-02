/**
 * OS.CPP - all non-graphic platform dependent code is in here:
 *
 * This version is for VISUAL C++
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

#include <time.h>
#include "race_manager.h"
#include "g_global.h"

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Gets an operator input from the keyboard or mouse button.
 * Waits, polling, if there is no data available.
 *
 * @return  The key pressed
 */
int Os::GetCh()
{
  int key = g_iLastKey;
  g_iLastKey = 0;
  return key;
}

/**
 * Return true if keyboard was hitted
 */
int Os::KbHit()
{
  return 0;
}


/**
 * Returns 0 if it is ok
 * Return 1 if ESC has been pressed -> race has to be stopped
 */
int RaceManager::Keyboard()
{
  if( g_iLastKey==ESC )
  {
    return 1;
  }
  else if ( g_iLastKey!=0 )
  {
    int c = os.GetCh();
    if( draw.m_bDisplay )          // F and S keys will speed up/slow down.
    {
      if(c == UP)            // up and down arrow designate next car.
      {
        int pos = g_ViewManager->GetFollowCar();
        pos = race_data.m_aCarInPos[ max( race_data.m_aPosOfCar[pos]-1, 0 ) ];
        g_ViewManager->FollowCar( pos );
        g_ViewManager->m_iFollowMode = FOLLOW_MANUAL;
        g_ViewManager->m_iFollowNobodyKey = c;
      }
      else if(c == DOWN)
      {
        int pos = g_ViewManager->GetFollowCar();
        pos = race_data.m_aCarInPos[ min( race_data.m_aPosOfCar[pos]+1, args.m_iNumCar-1 ) ];
        g_ViewManager->FollowCar( pos );
        g_ViewManager->m_iFollowMode = FOLLOW_MANUAL;
        g_ViewManager->m_iFollowNobodyKey = c;
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
          g_ViewManager->m_iRealSpeed = REAL_SPEED_MEDIUM;
          m_oInstantReplay.m_iMode = INSTANT_REPLAY;
        }
        else
        {
          m_oInstantReplay.Stop();
        }
      }
      else if( c == 's' || c == 'S' )
      {
        g_ViewManager->m_iRealSpeed = REAL_SPEED_SLOW;
        if( m_oInstantReplay.m_iMode!=INSTANT_RECORD )
        {
          m_oInstantReplay.m_iMode = INSTANT_REPLAY;
        }
      }
      else if( c == 'd' || c == 'D' )
      {
        g_ViewManager->m_iRealSpeed = REAL_SPEED_MEDIUM;
        if( m_oInstantReplay.m_iMode!=INSTANT_RECORD )
        {
          m_oInstantReplay.m_iMode = INSTANT_REPLAY;
        }
      }
      else if( c == 'f' || c == 'F' )
      {
        g_ViewManager->m_iRealSpeed = REAL_SPEED_FAST;
        if( m_oInstantReplay.m_iMode!=INSTANT_RECORD )
        {
          m_oInstantReplay.m_iMode = INSTANT_REPLAY;
        }
      }
      else if(c == '+' )
      {
        g_ViewManager->IncZoom();
      }
      else if(c == '-' )
      {
        g_ViewManager->DecZoom();
      }
      else if( c == 'v' || c == 'V' )
      {
        g_ViewManager->m_bChangeView = TRUE;
      }
      else if( c == 't' || c == 'T' )
      {
        draw.m_bDrawTrajectory = !draw.m_bDrawTrajectory;
      }
      else if(c == 'o' || c == 'O' )  // any Overtaking
      {
        g_ViewManager->m_iFollowMode = FOLLOW_ANY_OVERTAKING;
      }
      else if(c == 'p' || c == 'P' )  // for Position only
      {
        g_ViewManager->m_iFollowMode = FOLLOW_FOR_POSITION;
      }
      else if(c == 'i' || c == 'I' )  // Nobody
      {
        g_ViewManager->m_iFollowMode = FOLLOW_NOBODY;
      }
      else if(c == 'u' || c == 'U' )  // Update manually
      {
        g_ViewManager->m_iFollowMode = FOLLOW_MANUAL;
        g_ViewManager->FollowCar( g_ViewManager->MostInterestingCar() );
      }
      else if( c==ARROW_UP || c==ARROW_DOWN || c==ARROW_LEFT || c==ARROW_RIGHT || c==ROTATE_UP || c==ROTATE_DOWN || c==ROTATE_LEFT || c==ROTATE_RIGHT || c==ROTATE_RESET ) // Key in Free Mode
      {
        g_ViewManager->m_iFollowNobodyKey = c;
      }
      else if(c == 'n' || c == 'N' )  // show driver names
      {
        g_ViewManager->m_bShowNames = !g_ViewManager->m_bShowNames;
      }
    }
  }
  return 0;
}

/**
 * This routine must monitor a hardware periodic signal in order to
 * return at the next "clock tick".  The period of the clock must be
 * assigned to delta_time in OS.H.  This version uses the IBM PC clock
 * and delta_time is accordingly set at .0549 sec.  For other platforms, 
 * delta_time should be between about .05 and .1 second.
 * This routine must be called once with a non-zero argument to initialize it.
 * After that it is called with no argument, or with zero.
 *
 * Used only in X11. In Window/KDE, the speed is determined by a timer.
 */
void Os::OneTick(int /*initialize = 0*/)
{
}

/**
 * Gets a long integer "randomly" from system clock.
 *
 * @return a random number
 */
long Os::PickRandom()
{                       // This version returns the clock value in (1/20) sec.
  clock_t cl= clock();
  return( (long)cl );
}

/**
 * Returns the number of available 1 K RAM blocks
 *
 * @return memory available [KB]
 */
int Os::RamQuery()
{
  return 0;
}

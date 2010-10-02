/*
 * MAIN.CPP - Main program
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@skynet.be> 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */ 

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------
 
#include "car.h"
#include "os.h"
#include "track.h"
#include "race_manager.h"
#include "report.h"
#include "draw.h"

////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////
 
/**
 * MAIN FUNCTION
 * args are interpreted by Args.GetArgs() in ARGS.CPP
 */
int main(int argc, char* argv[])
{
  RaceManager Rars;
 
  Rars.ArgsInit( argc, argv );
 
  for( args.m_iCurrentTrack=0; args.m_iCurrentTrack<args.m_iNumTrack; args.m_iCurrentTrack++)
  {
    Rars.AllInit();
 
    // Practice:
    if( args.m_bPractice )
    {
      Rars.RaceInit(0);
      while(Rars.RaceLoop()) {};
      Rars.RaceClose(0);
      // Continue showing end of practice until a key is hit.
      if(draw.m_bDisplay) draw.PressKey(0);
    }
 
    /// Qualification loop:
    if( args.m_bQual )
    {
      for( int ql=0; ql<args.m_iNumQualSession; ql++ )   // For every round of qual.:
      {
        Rars.QualInit(ql);
        if( draw.m_bDisplay && draw.PressKey(ql)==ESC )
        {
          break;
        }

        for( int q=0; q<args.m_iNumCar; q++ ) // for each car:
        {
          Rars.QualInitLoop(q);
          while(Rars.QualLoop(q)){};   // repeat qualifying with every car
        }
        Rars.QualClose(ql);
 
        // Continue showing end of qualifications until a key is hit.
        if(draw.m_bDisplay && draw.PressKey(ql) == ESC)
        {
          break;
        }
      }
    }
    /// End of qualifications
 
 
    // Main loop - Once through here for every complete race session
    for(long ml=0; ml<args.m_iNumRace; ml++ )
    {
      Rars.RaceInit(ml);
      while(Rars.RaceLoop()) {};
      Rars.RaceClose(ml);
 
      // Continue showing end of race until a key is hit.
      if(draw.m_bDisplay && draw.PressKey(ml) == ESC)
      {
        break;
      }
    }
    Rars.AllClose();
  }
  Rars.ArgsClose();
  return (0);
}    


/**
 * RaceManager.h - RaceManager and Settings
 *
 * Definition of class RaceManager which controls a race, and class Settings which contains 'mode'-variables
 *
 * History
 *  ver. 0.84 Created 03.07.2001
 *
 * @author    Carsten Kjaer
 * @see       RaceManager.cpp for method definitions
 * @version   0.84
 */

#ifndef __RARSCORE_RACEMANAGER_H
#define __RARSCORE_RACEMANAGER_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifdef WIN32
#include "stdafx.h"
#endif

#include <iostream>
#include "report.h"
#include "movie.h"
#include "instant_replay.h"
#include "timer.h"

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * The Racemanager manage the play of a race 
 */
class RaceManager
{
private:
  double        m_fPanelLastTime; // Last update of Instrument Panel
  int           m_iEndExtraTime;  // Number of loops after all cars are either finished or out
  Report        m_oReport;        // Report object used to write .out files
  Movie *       m_oMovie;         // Movie object used to read/write .xy .ang files
  InstantReplay m_oInstantReplay; // InstantReplay used to stop, rewind and replay the race
  Timer         RobotTimer;    // Measures the time used by the robots

  void ArrangeCars();
  void Sortem();
  int  Keyboard();
  int  NormalRaceLoop();
  void CommonInit( Stage stage, int rl);

public:
  RaceManager()
  {
    m_fPanelLastTime = 0.0;
  }
  ~RaceManager();

  void ArgsInit( int argc, char* argv[] );
  void AllInit();

  //Qualifications
  void QualInit(int ql);
  int  QualInitLoop(int q);
  int  QualLoop(int q);
  void QualEvaluateResults();
  void QualClose(int ql);

  // Races
  void RaceInit(long ml);
  int  RaceLoop();
  void RaceClose(long ml);

  void AllClose();
  void ArgsClose();

  friend class CDlgTrackEditor;
};

#endif // __RARSCORE_RACEMANAGER_H


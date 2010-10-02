/**
 * draw.h - the interface to the systemindependent graphic functions in draw.cpp
 *
 * History
 *  ver. 0.84 Created 04.07.2001
 *
 * @author    Carsten Kjaer
 * @see       draw.cpp for method definitions
 * @version   0.84
 */

#ifndef __RARSCORE_DRAW_H
#define __RARSCORE_DRAW_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "globals.h"

//--------------------------------------------------------------------------
//                           F O R W A R D
//--------------------------------------------------------------------------

class InstantReplay;

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Draw
 */
class Draw
{
private:
  void DrawCar(double, double, double, int, int);
public:
  bool m_aNewData[MAX_CARS];   // (X11) Data to update only changes lines in Leaderboard
  int  m_iCarShown;            // (X11) The number of the car we follow in the display
  int  m_iFastDisplay;         // (X11) set for fast or ultra-fast, clear for realistic
  int  m_iLeaderboardMode;     // (X11) What to display in the leaderboard (TODO should be an enum)
  bool m_bDrawTrajectory;      // If true draw trajectory
  bool m_bDisplay;             // If true then show graphics

  void GraphSetup( InstantReplay * ir );
  void ResumeNormalDisplay();
  void RefreshFinishLine();

  // X11
  void Instruments();          // updates the instrument panel
  void DrawCar( Car * );
  int  PressKey(long);
  void ScoreBoard(Stage, long);
  void DrawTrack();            // replaces drawpath() in ver.0.71
  void Lapper(long);

  // X11 - Leaderboard
  void InitNewData();
  void CalcNewData( int old_order[] );
  void UpdateLeaderboard();
  void PrintLeaderboardHeader();
  void Leaders(int, int*);     // update leaderboard for i-th  car

  Draw()
  {
    m_iCarShown = -1;
    m_iFastDisplay = 0;
    m_iLeaderboardMode = 0;
    m_bDrawTrajectory = false;
    m_bDisplay = true;
  }
};

extern Draw draw;

#endif


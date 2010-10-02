// DRAW.CPP - the system-independent graphics portion of RARS 0.39
// (was GRAPHICS.CPP) - by Mitchell E. Timin, State College, PA    
// See GI.H, CAR.H & TRACK.H for class and structure declarations.    
// GI.CPP is the system-dependent graphics portion of RARS.
// ver. 0.1 release January 12, 1995
// ver. 0.2 1/23/95
// ver. 0.3 2/7/95
// ver. 0.39 3/6/95
// ver. 0.45 3/21/95
// ver. 0.50 4/5/95
// ver. 0.6b 5/8/95 b for beta
// ver. 0.61 May 26
// ver. 0.70 Nov 97
// ver. 0.71 Jan 98

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                            G L O B A L
//--------------------------------------------------------------------------

Draw draw;

//--------------------------------------------------------------------------
//                         F U N C T I O N S
//--------------------------------------------------------------------------

void Draw::RefreshFinishLine()   // re-draw the finish line:
{
  if( m_bDisplay  )
  {
    g_ViewManager->Refresh();
  }
}


// Initializes graphics system, draws track, fills in colored regions:
void Draw::GraphSetup( InstantReplay * ir )
{
  if( m_bDisplay )
  {
    Palette.Init();
    g_ViewManager = new TViewManager( ir );
    g_ViewManager->InitAllViews();
  }
}

void Draw::ResumeNormalDisplay()
{
  if( m_bDisplay )
  {
    delete g_ViewManager;
  }
}

//////////////// COMPATIBILITY WITH XWindows /////////////////////

void Draw::DrawTrack() {}
void Draw::DrawCar(Car * /*car*/) {}
void Draw::DrawCar(double /*x*/, double /*y*/, double /*ang*/, int /*nose*/, int /*tail*/) {}
void Draw::Lapper(long /*lap*/) {}
void Draw::ScoreBoard(Stage /*stage*/, long /*rl*/) {}
int  Draw::PressKey(long /*rl*/) { return 0; }
void Draw::Instruments() {}
void Draw::PrintLeaderboardHeader() {}
void Draw::Leaders(int /*i*/, int* /*order*/) {}
void Draw::InitNewData() {}
void Draw::CalcNewData( int /*old_order*/[] ) {}
void Draw::UpdateLeaderboard() {}

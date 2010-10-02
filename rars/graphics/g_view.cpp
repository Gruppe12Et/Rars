
//--------------------------------------------------------------------------
//
//    FILE: G_VIEW.CPP (portable)
//
//    TView
//      - This class is a virtual class for all the views that belows
//        to the "ViewManager"
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                            Class TView
//--------------------------------------------------------------------------

/**
 * Constructor
 *
 * @param x : size of the view
 * @param y : size of the view
 */
TView::TView( int x, int  y, bool bVirtual ) : TLowGraphic( x, y, bVirtual )
{
  m_iFollowCar = g_ViewManager->GetFollowCar();
  m_OptionShowBoard = 1;
  m_TopX = 0;
  m_TopY = 0;
  m_SpeedX = 0;
  m_SpeedY = 0;
}

/**
 * Destructor
 */
TView::~TView()
{
}

/**
 * Virtual function (used in child classes for modifying their bitmap)
 */
void TView::Refresh() {}

/**
 * Change the car to follow
 * 
 * @param car_nr : the new_car to follow in the view
 */
void TView::FollowCar( int car_nr )
{
  if( m_iFollowCar<0 || m_iFollowCar>=args.m_iNumCar ) return;
  m_iFollowCar = car_nr;
}

/**
 * Calculate the optimum values of m_ScaleXy, top_xy, m_CenterXy
 * so that the view can contain the full track.
 * The calculation are based on trackin and trackout
 */
void TView::FullScreenScale()
{
  // calculates m_CenterXy, top_xy, m_ScaleXy with min and max
  m_CenterX = (currentTrack->m_fXMax+currentTrack->m_fXMin)*0.5;
  m_CenterY = (currentTrack->m_fYMax+currentTrack->m_fYMin)*0.5;
  m_ScaleX = min( (float)m_SizeX/(currentTrack->m_fXMax-currentTrack->m_fXMin), (float)m_SizeY/(currentTrack->m_fYMax-currentTrack->m_fYMin) )*0.95;
  m_ScaleY= - m_ScaleX;
  m_TopX = m_CenterX - m_SizeX/(2*m_ScaleX);
  m_TopY = m_CenterY - m_SizeY/(2*m_ScaleY);
}

//__ TView::DrawBorder _____________________________________________________
//
// Draws a border to a segment
//__________________________________________________________________________

void TView::DrawBorder(Int2D v[])
{
  DrawLine(v[0], v[3], COLOR_WHITE);
  DrawLine(v[1], v[2], COLOR_WHITE);
}


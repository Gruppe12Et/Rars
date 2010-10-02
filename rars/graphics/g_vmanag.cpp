
//--------------------------------------------------------------------------
//
//    FILE: G_VMANAG.CPP (portable)
//
//    TViewManager
//      - This class shares the screen in views. It manages the views,
//        updates their contents and their windows on the screen.
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifdef WIN32
  #include <stdafx.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include "g_global.h"
#include "../misc.h"

//--------------------------------------------------------------------------
//                         V A R I A B L E S
//--------------------------------------------------------------------------

TPalette Palette;

/**
 * Constructor
 */
TViewManager::TViewManager( InstantReplay * ir )
{
  char s[256];

  // init attributes
  m_Initialized = TRUE;
  m_ZoomDelta = 1.0;
  m_ImageCpt = 0;
  m_iViewMode = g_oRarsIni.m_iViewMode;
  m_iFollowMode = g_oRarsIni.m_iFollowMode;
  m_iFollowCar = 0;  // can not be done yet.
  m_iSizeOffX = -1; 
  m_iSizeOffY = -1;
  m_oInstantReplay = ir;
  m_bChangeView = false;
  m_iRealSpeed = REAL_SPEED_MEDIUM;
  m_i3DS = g_oRarsIni.m_i3DS;
  m_bShowNames = (g_oRarsIni.m_iShowNames != 0);
  m_fFps = 0;

  // init screen
  InitScreen();

  // load the textures of cars
  int i=0;
  while( drivers[i]!=NULL ) 
  {
    sprintf( s, "bitmap/%s", drivers[i]->getBitmapName2D() );
    m_aBitmapCar[i].PCX_Read( s );
    i++;
  }

  // init all the views
  m_Views = new TView *[10];
  m_ViewNb = 0;
}

/**
 * Destructor
 */
TViewManager::~TViewManager()
{
  // close screen
  CloseScreen();
  m_Initialized = FALSE;

  // close all the views
  DeleteAllViews();
  free( m_Views );
}

/**
 * Reset the ViewManager at the begin of a race
 */
void TViewManager::Reset()
{
  m_ImageCpt = 0;
}

/**
 * Delete all the views
 */
void TViewManager::DeleteAllViews()
{
  for(int i=0; i<m_ViewNb; i++ ) 
  {
    delete m_Views[i];
  }
  m_ViewNb = 0;
}

/**
 * Add a view in the "m_Views" array
 *
 * @param v             the new view to add
 */
void TViewManager::AddView( TView * v )
{
  m_Views[m_ViewNb] = v;
  m_ViewNb++;
  if( m_ViewNb>=10 ) error("TViewManager::AddView: too much m_Views");
}

/**
 * Find distance between cars in feet
 *
 * @param i car i
 * @param j car j
 * @return distance between cars
 */

double TViewManager::distance(int i,int j)
{
  double len = get_track_description().length;
  switch( m_iFollowMode )
  {
  default:
  case FOLLOW_FOR_POSITION:
    // for position
    return fabs( race_data.cars[i]->Laps*len+race_data.cars[i]->Distance 
               - race_data.cars[j]->Laps*len-race_data.cars[j]->Distance );
    break;

  case FOLLOW_ANY_OVERTAKING:
    // closest
    return min( fabs( race_data.cars[i]->Distance 
                    - race_data.cars[j]->Distance ),
                len
              - fabs( race_data.cars[i]->Distance 
                    - race_data.cars[i]->Distance )
              );
  }

}

/**
 * Find car closest to another car or off the track
 */

int TViewManager::MostInterestingCar()
{
  int mi=race_data.m_aCarInPos[0];
  double min_d=distance(0,1);

  for( int n=0,i=mi; n<args.m_iNumCar; i=race_data.m_aCarInPos[++n])
  {
    if( race_data.cars[i]->Out != 1  // not out of the race
      && race_data.cars[i]->Out != 3  // did qualify
    )
    {
      if( race_data.cars[i]->Offroad
        && race_data.cars[i]->Out != 2 // do not follow the whole pit stop...
      )
      {
        return i; // ...only see how cars get into and out of pits
      }
      for( int j=0; j<args.m_iNumCar; j++)
      {
        if( i!=j  // not itself
          && race_data.cars[j]->Out != 1 // not out of the race
          && race_data.cars[i]->Bestlap_speed >= race_data.cars[j]->Bestlap_speed // follow the fastest
          && min_d > distance(i,j) // is it the closest ?
        )
        {
           min_d = distance(i,j);
           mi = i;
        }
      }
    }
  }
  return mi;
}

/**
 * Refresh the screen: calculate the views, copy them to the screen
 */
void TViewManager::Refresh()
{
  // image counter
  m_ImageCpt ++;

  // Follow chosen car
  if( m_ImageCpt==1 && m_iFollowMode==FOLLOW_MANUAL && g_RaceStage!=QUALIF )
  {
    FollowCar( find_name( g_oRarsIni.m_sFollowCar ) );
  }

  // super_fast_mode
  if( m_iRealSpeed==REAL_SPEED_FAST ) 
  {
    // display one image every xx ticks
    if( m_ImageCpt%m_FastModeParam ) return;
  }
  else if( m_iRealSpeed==REAL_SPEED_SLOW )
  {
#ifdef WIN32
    Sleep( 300 );
#else
    clock_t goal= clock()+200000;
    while( goal > clock() );
#endif
  }

  // zoom
  m_ZoomDelta = m_ZoomDelta*0.95+0.05;

  // BestView
  static int time_count=40;
  if( (m_iFollowMode==FOLLOW_FOR_POSITION || m_iFollowMode==FOLLOW_ANY_OVERTAKING) && --time_count == 0 )
  {
    time_count = 40;
    FollowCar( MostInterestingCar() );
  }

  for( int i=0; i<m_ViewNb; i++ ) 
  {
    m_Views[i]->Refresh();
    CopyViewToScreen(i);
  }
}

/**
 * Change the car to follow in all the views
 *
 * @param car_nr        the number of the car to follow
 */
void TViewManager::FollowCar( int car_nr )
{
  if( car_nr<0 || car_nr>=args.m_iNumCar ) return;
  m_iFollowCar = car_nr;
  for( int i=0; i<m_ViewNb; i++ ) 
  {
    m_Views[i]->FollowCar( car_nr );
  }
}

/**
 * Give the value of the attribute 'm_FollowCar'
 * 
 * @return              m_FollowCar
 */
int TViewManager::GetFollowCar()
{
  return m_iFollowCar;
}

/**
 * Increase the zoom of all views with zoom. The views with zoom
 * have to check the value of m_ZoomDelta
 */
void TViewManager::IncZoom()
{
  m_ZoomDelta = 0.95;
}

/**
 * Decrease the zoom of all views with zoom. The views with zoom
 *  have to check the value of m_ZoomDelta
 */
void TViewManager::DecZoom()
{
  m_ZoomDelta = 1.05;
}

/**
 * Set m_FastModeParam
 * 
 * @param param         new value for m_FastModeParam
 */
void TViewManager::SetFastModeParam( int param )
{
  m_FastModeParam = param;
}

/**
 * Set m_FastModeParam
 *
 * @return         view mode
 */
int TViewManager::GetViewMode( )
{
  return m_iViewMode;
}

/**
 * Get the type of opengl type view
 * !! can only be called if( IS_MODE_OPENGL(m_iViewMode) )
 *
 * @return         type view mode
 */
TView3D * TViewManager::GetOpenGlView()
{
  return (TView3D *)m_Views[0];
}


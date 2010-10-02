//--------------------------------------------------------------------------
//
//    FILE: G_VIEWTE.CPP (portable)
//
//    TView3D
//      - This is the track editor view.
//
//    Version       Author          Date
//      0.1      Marc Gueury     18/09/2001
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
  #include <stdafx.h>
  #include <GL/glaux.h>
#else
  // KDE for the font
  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include "g_global.h"
#include "g_tracked.h"
#include "g_track3d.h"

#include "../misc.h"

// #define GLUT_TEST
#ifdef GLUT_TEST
  #include <glut.h>
#endif

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Constructor
 *
 * @param x, y       : size of the view
 * @param offx, offy : offset of the view
 */
TViewTrackEditor::TViewTrackEditor( int x, int y, int offx, int offy )
: TView3D() 
{
  Init( x, y, offx, offy, true );
  m_Zoom = 0.01;
  ModelWheelRear = NULL;
  ModelWheelFront = NULL;
  ModelCarBody = NULL;
  ModelHelmet = NULL;
  ModelInstantReplay = NULL;
  ResetRotation();
}

double TViewTrackEditor::MaxDist()
{
  return 20000.0;
}

void TViewTrackEditor::DrawAll()
{
  // starting
  if( m_bInit ) 
  {
    if( g_ViewManager->m_i3DS!=0 )
    {
      track3D.Load();
    }
    m_bInit = false;
  }

  MoveWithKeys();

  // refresh the bitmap
  // g_GL_CWnd.SetFocus();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix ();  // master view

  // zoom
  m_Zoom *= g_ViewManager->m_ZoomDelta;
  m_Zoom = min( max( m_Zoom, 0.0005 ), 0.1 );

  glTranslatef (0.0f, 0.0f, -1500.0f*100.0f*float(m_Zoom) );
  glRotatef ( float(m_fAngleX),1.0f,0.0f,0.0f);
  glRotatef ( float(m_fAngleZ),0.0f,0.0f,1.0f);
  glTranslatef (-GLfloat(m_CenterX), -GLfloat(m_CenterY), 0.0f );

  // Calculate the cone of view (Fustrum)
  ExtractFrustum();

  // Track
  if( currentTrack!=NULL )
  {
    if( g_TrackEditor.m_iViewMode==TRACKED_VIEW_MODE_2D )
    {
      track3D.DrawRoad( false );
    }
    else if( g_TrackEditor.m_iViewMode==TRACKED_VIEW_MODE_3DS )
    {
      GL_EnableLight();
      track3D.DrawRoad3D();
    }
    else // TRACKED_VIEW_MODE_ONE_SEG
    {
      // track3D.DrawRoadSegment( g_TrackEditor.m_iCurrentSeg );
    }
  }

  glPopMatrix(); 
  glFinish();

  GL_SwapWidget();
  g_bToRefresh = TRUE;
}

/**
 * Center and zoom such that all the track can be seen
 */
void TViewTrackEditor::FullScreenScale()
{
  TView::FullScreenScale();

  // Angle = 45 deg -> dist
  double dist = max( currentTrack->m_fXMax-currentTrack->m_fXMin, currentTrack->m_fYMax-currentTrack->m_fYMin ) * 1.4;
  // dist = 1500.0f*100.0f*float(m_Zoom) 
  m_Zoom = dist / (1500.0f*100.0f);
}
/**
 * VC_LOWGR.CPP (VC++ specific)
 *              (not portable)
 * 
 * This file gathers together all the functions of the classes that
 * are not portable:
 * - TPalette
 * - TViewManager
 * - TLowGraphic
 * 
 * Version       Author          Date
 * 0.1      Marc Gueury     05 /08 /96
 * 
 * 
 * Bug: There is a very annoying bug on Win98 that disallow to use
 *      CreateDIBSection with a negative biHeight. 
 *      It works only sometimes depending on Windows mood.
 * WA:  The work around used here is to slip the bitmap at the hand.
 *      And Windows will split it again...
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdarg.h>
#include <qimage.h>
#include <qcolor.h>
#include <qdir.h>
#include <qdatetime.h>
#include <qlistbox.h>
#include "g_global.h"
#include "g_tracked.h"
#include "../kde/kde_rars.h"
#include "../kde/kde_initcar.h"

//--------------------------------------------------------------------------
//                            E X T E R N
//--------------------------------------------------------------------------

extern int no_display;
extern int qual_cars, qual_sess, qual_laps, race_lap_count;
extern int qual_run_flag;    // qualification mode (see os.cpp)

// main.cpp
extern RACESTAGE g_RaceStage;
extern TPalette Palette;
extern bool g_bRefresh;
extern KdeRars * g_kde_rars;
extern KdeInitCar * g_dlgInitCar;

//--------------------------------------------------------------------------
//                            G L O B A L
//--------------------------------------------------------------------------

bool                g_bToRefresh;

static int          g_iViewCpt = 0;
QImage              g_aImage[3];
TView3D *           g_View3D = NULL;

//**************************************************************************
//                             C L A S S
//                          T P A L E T T E
//**************************************************************************

/**
 * Set the palette.
 */
void TPalette::Set()
{
  // set the 16 defined colors
  for( int cpt=0; cpt<3; cpt++ )
  {
    for( int i=0; i<=m_ColorNb; i++ )
    {
      // g_aImage[cpt]->setColor( i, qRgb( m_Color[i].r, m_Color[i].g, m_Color[i].b) );
    }
  }
}

//**************************************************************************
//                             C L A S S
//                            TLowGraphic
//**************************************************************************

/**
 * Memory : Allocate some memory for the bitmap
 * Direct Access: indicate the adress of the screen (see: screen_size_x)
 */
void TLowGraphic::MallocBitmap()
{
  //// The function CreateDIBSection has a random behavior on Win98 only
  //// when the biHeight has a negative value.
  ////
  g_aImage[g_iViewCpt].reset();
  if( g_aImage[g_iViewCpt].create( m_SizeX, m_SizeY, 8 ))
  {
    m_Bitmap = (char *)( g_aImage[g_iViewCpt].scanLine(0)+0 );
    g_aImage[g_iViewCpt].setNumColors(Palette.m_ColorNb+1);
    for( int i=0; i<=Palette.m_ColorNb; i++ )
    {
      g_aImage[g_iViewCpt].setColor( i, qRgb( Palette.m_Color[i].r*4, Palette.m_Color[i].g*4, Palette.m_Color[i].b*4) );
    }
  }
  g_iViewCpt++;
}

/**
 * Memory : Free the memory of the bitmap
 * Direct Access: -
 */
void TLowGraphic::FreeBitmap()
{
}



//**************************************************************************
//                             C L A S S
//                            TViewManager
//**************************************************************************

/**
 * Initialize all the views
 */
void TViewManager::InitAllViews()
{
  g_iViewCpt = 0;
  g_View3D = NULL;

  if( m_iViewMode==MODE_CLASSIC )
  {
    TViewClassical * view = new TViewClassical(624,432);
    view->m_OptionShowBoard = 1;
    AddView( view );
  }
  else if( m_iViewMode==MODE_OPENGL_FULLSCREEN )
  {
    g_View3D = new TView3D(624,432,0,0,0);
    g_View3D->m_OptionShowBoard = 1;
    AddView( g_View3D );
  }
  else if( m_iViewMode==MODE_OPENGL_WINDOW )
  {
    g_View3D = new TView3D(372,250,8,8,0);
    AddView( g_View3D );
    AddView( new TViewClassical(220,250) );
    AddView( new TViewBoard(600,160) );
  }
  else if( m_iViewMode==MODE_TELEMETRY )
  {
    AddView( new TViewTelemetry(624,432,0) );
  }
  else
  {
    AddView( new TView2D(372,250,0) );
    AddView( new TViewClassical(220,250) );
    AddView( new TViewBoard(600,160) );
  }
  SetFastModeParam( 15 );
}

/**
 * Resize all the views
 *
 * @param x             (in) size x of the window
 * @param y             (in) size y of the window
 */
void TViewManager::ResizeAllViews( int x, int y )
{
  g_iViewCpt = 0;

  if( m_iViewMode==MODE_CLASSIC )
  {
    m_Views[0]->Resize( 624+x,432+y );
  }
  else if( m_iViewMode==MODE_OPENGL_FULLSCREEN )
  {
    g_iViewCpt++;
    // delete m_Views[0];
    // Crash that I can't explain -> 8
    // m_Views[0] = new TView3D(640+x,480+y,0,0,0);
  }
  else if( m_iViewMode==MODE_OPENGL_WINDOW )
  {
    // delete m_Views[0];
    // m_Views[0] = new TView3D(372+x,250+y,8,8,0);
    g_iViewCpt++;
    m_Views[1]->Resize( 220  ,250+y );
    m_Views[2]->Resize( 600+x,160 );
  }
  else if( m_iViewMode==MODE_TELEMETRY )
  {
    m_Views[0]->Resize( 624+x,432+y );
  }
  else
  {
    m_Views[0]->Resize( 372+x,250+y );
    m_Views[1]->Resize( 220  ,250+y );
    m_Views[2]->Resize( 600+x,160 );
  }
}

/**
 * Initialize the screen
 */
void TViewManager::InitScreen()
{
}

/**
 * Set the normal screen mode
 */
void TViewManager::CloseScreen()
{
//  pDlg->bitmap.DeleteObject();
}

/**
 * Copy the view 'view_nr' to the screen 
 *
 * @param view_nr       (in) the view to copy
 */
void TViewManager::CopyViewToScreen( int view_nr )
{
  if( view_nr == 2
      || m_iViewMode==MODE_CLASSIC
      || m_iViewMode==MODE_OPENGL_FULLSCREEN
      || m_iViewMode==MODE_TELEMETRY )
  {
    g_bToRefresh = true;
  }


  if( view_nr==0 )
  {
    static int iFrames = 0;
    static QTime t;

    if( iFrames==0 )
    {
      t.start();
    }
    else if( t.elapsed()>=1000 )
    {
      m_fFps = (m_fFps+iFrames)/2;
      t.start();
      iFrames = 0;
    }
    iFrames ++;
  }
}

/**
 * Create the Widget Renderer of OpenGL
 *
 * @param x         (in) size x of the widget
 * @param y         (in) size y
 * @param offx      (in) pos x
 * @param offy      (in) posy
 */
void GL_InitWidget(int, int, int, int)
{
}

void GL_DestroyWidget()
{
}

void GL_SwapWidget()
{
}
//---------------------------------------------------------------------------
// RarsIni
//---------------------------------------------------------------------------

/**
 * Save the maximize property of the window in Rars.ini
 */
void RarsIni::SaveWindowSettings()
{
  // It does not work and it is documented !
  // It returns always 0
  // m_iMaximized = g_kde_rars->isMaximized();
}

//---------------------------------------------------------------------------
// Os
//---------------------------------------------------------------------------

/**
 * Add a message in the initilisation window
 */
void Os::ShowInitMessage( const char *format, ... )
{
  char s[1024];
  va_list argList;

  va_start(argList, format);
  vsprintf( s, format, argList);
  va_end(argList);

  if( draw.m_bDisplay )
  {
    g_dlgInitCar->ListMessage->insertItem( s );
    kde_app->processEvents();
  }
}

/**
 * Create a new directory
 */
void Os::CreateDirectory( const char * dir )
{
  QDir d;
  d.mkdir( dir );
}

//---------------------------------------------------------------------------
// TrackEditor
//---------------------------------------------------------------------------

/**
 * Call the real refresh method
 */
void TrackEditor::Refresh( bool bRebuildList )
{
  // TODO
}

/**
 * Get and Set the data inside the TrackEditor structure on the screen
 */
void TrackEditor::UpdateData( bool bGetData )
{
  // TODO
}

/**
 * Change the current edited properties when clicking on a list
 */
void TrackEditor::EditProperty()
{
  // TODO
}

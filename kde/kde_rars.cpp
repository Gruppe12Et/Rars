/*
 * KDE_RARS.CPP - Main Window of RARS in KDE 3
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *  ver. 0.90 Feb 03 - KDE3
 *
 * @author    Marc Gueury <mgueury@synet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <kstdaccel.h>
#include <kiconloader.h>
#include <kmenubar.h>
#include <kstatusbar.h>
#include <kapp.h>
#include <kaction.h>
#include <qdialog.h>
#include <qimage.h>
#include <qlabel.h>
#include <qpainter.h>
#include <qwidget.h>
#include <qtimer.h>
#include <qlistbox.h>
#include "../graphics/g_global.h"
#include "../graphics/g_global.h"

#include "kde_rars.h"
#include "kde_initcar.h"

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

extern QImage g_aImage[];
extern TView3D * g_View3D;

extern void GL_Initialize( int x, int y );

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

////
//// Class MyOpenGL
////
// This class can cause a problem when compiling with QT without OpenGL
// In that case, comment the following define (QT_OPENGL)
// Or recompile yourself QT (www.trolltech.com)

#define QT_OPENGL
#ifdef QT_OPENGL
class MyOpenGL : public QGLWidget
{
public:
  MyOpenGL( QWidget *parent, const char *name ): QGLWidget(parent,name)
  {
  }

protected:
  void initializeGL()
  {
  }

  void resizeGL( int w, int h )
  {
    if( g_View3D )
    {
      g_View3D->Init( w, h, 0, 0, true );

    }
  }

  void paintGL()
  {
    if( g_View3D)
    {
      g_View3D->DrawAll();
    }
  }
};
#else // QT_OPENGL
class MyOpenGL : public QWidget
{
public:
  MyOpenGL( QWidget *parent, const char *name ): QWidget(parent,name)
  {
  }
};
#endif // QT_OPENGL

//--------------------------------------------------------------------------
//                            G L O B A L
//--------------------------------------------------------------------------

long g_iCptMain = 0;
KdeInitCar * g_dlgInitCar;

//--------------------------------------------------------------------------
//                            Class MyImage
//--------------------------------------------------------------------------

MyImage::MyImage( int image_id, QWidget * parent ):QWidget(parent)
{
  m_iImageId = image_id;
  setBackgroundMode( NoBackground );
}

void MyImage::paintEvent(QPaintEvent *)
{
  QPainter paint(this);
  paint.drawImage(0, 0, g_aImage[m_iImageId], 0, 0, g_aImage[m_iImageId].width(), g_aImage[m_iImageId].height());
}

//--------------------------------------------------------------------------
//                            Class KdeRars
//--------------------------------------------------------------------------

/**
 * Constructor: construct the main window
 */
KdeRars::KdeRars( const char * name ): KMainWindow( 0, name )
{
  m_offx = 0;
  m_offy = 0;
  
  setPlainCaption("Rars");

  KAction * quit = new KAction("&Quit", QIconSet( BarIcon("exit") ),
                     KStdAccel::quit(), kapp, SLOT(closeAllWindows()), this);
  KAction * changeView = new KAction("&Change View", 
                         Key_V, this, SLOT( slotChangeView() ), this );
  KAction * viewClassic = new KAction("Mode: &Classic", 0, this, SLOT( slotViewClassic() ), this );
  KAction * viewFullScreen = new KAction("Mode: OpenGL &Full screen", 0, this, SLOT( slotViewFullScreen() ), this );
  KAction * viewWindow = new KAction("Mode: OpenGL &Window", 0, this, SLOT( slotViewWindow() ), this );
  KAction * viewTelemetry = new KAction("Mode: &Telemetry", 0, this, SLOT( slotViewTelemetry() ), this );
  KAction * viewZoom = new KAction("Mode: &Zoom", 0, this, SLOT( slotViewZoom() ), this );
  KAction * viewTrajectory = new KAction("View/Hide &Trajectory", 
                         Key_T, this, SLOT( slotViewTrajectory() ), this );
  KAction * viewDriverNames = new KAction("Show &Names", 
                         Key_N, this, SLOT( slotViewDriverNames() ), this );
  KAction * carNext = new KAction("&Next Car", QIconSet( BarIcon("up")),
                         Key_PageUp, this, SLOT( slotCarNext() ), this );
  KAction * carPrevious = new KAction("&Previous Car", QIconSet( BarIcon("down")),
                         Key_PageDown, this, SLOT( slotCarPrevious() ), this );
  KAction * speedRewind = new KAction("&Rewind", QIconSet( BarIcon("player_rew")),
                         Key_R, this, SLOT( slotSpeedRewind() ), this );
  KAction * speedStop = new KAction("Stop", QIconSet( BarIcon("player_stop")),
                         Key_E, this, SLOT( slotSpeedStop() ), this );
  KAction * speedSlow = new KAction("&Slow", QIconSet( BarIcon("player_end")),
                         Key_S, this, SLOT( slotSpeedSlow() ), this );
  KAction * speedMedium = new KAction("&Medium", QIconSet( BarIcon("player_play")),
                         Key_D, this, SLOT( slotSpeedMedium() ), this );
  KAction * speedFast = new KAction("&Fast", QIconSet( BarIcon("player_fwd")),
                         Key_F, this, SLOT( slotSpeedFast() ), this );
  KAction * followCarManual = new KAction("Manual &Update", QIconSet( BarIcon("misc")),
                         Key_U, this, SLOT( slotFollowCarManual() ), this );
  KAction * followCarNobody = new KAction("Nobody", QIconSet( BarIcon("misc")),
                         Key_I, this, SLOT( slotFollowCarNobody() ), this );
  KAction * followCarOvertaking = new KAction("Any &Overtaking", QIconSet( BarIcon("misc")),
                         Key_O, this, SLOT( slotFollowCarOvertaking() ), this );
  KAction * followCarPosition = new KAction("For &Position", QIconSet( BarIcon("misc")),
                         Key_P, this, SLOT( slotFollowCarPosition() ), this );
  // Shift because bug in KDE 2.2 ('+' only does give shift+'+' ??)
  KAction * zoomIn = new KAction("Zoom &In", QIconSet( BarIcon("viewmag+")),
                         SHIFT+Key_Plus, this, SLOT( slotZoomIn() ), this );
  KAction * zoomOut = new KAction("Zoom &Out", QIconSet( BarIcon("viewmag-")),
                         SHIFT+Key_Minus, this, SLOT( slotZoomOut() ), this );
  KAction * settingsSave = new KAction("Save Settings", 0, this, SLOT( slotSettingsSave() ), this );
  KAction * moveUp = new KAction("Up", QIconSet( BarIcon("up")),
                         Key_Up, this, SLOT( slotMoveUp() ), this );
  KAction * moveDown = new KAction("Down", QIconSet( BarIcon("down")),
                         Key_Down, this, SLOT( slotMoveDown() ), this );
  KAction * moveLeft = new KAction("Left", QIconSet( BarIcon("back")),
                         Key_Left, this, SLOT( slotMoveLeft() ), this );
  KAction * moveRight = new KAction("Right", QIconSet( BarIcon("forward")),
                         Key_Right, this, SLOT( slotMoveRight() ), this );

  QPopupMenu * menuFile = new QPopupMenu;
  quit->plug(menuFile);
  menuBar()->insertItem("&File", menuFile );

  QPopupMenu * menuView = new QPopupMenu;
  changeView->plug(menuView);
  menuView->insertSeparator();
  viewClassic->plug(menuView);
  viewFullScreen->plug(menuView);
  viewWindow->plug(menuView);
  viewTelemetry->plug(menuView);
  viewZoom->plug(menuView);
  menuView->insertSeparator();
  zoomIn->plug(menuView);
  zoomOut->plug(menuView);
  menuView->insertSeparator();
  viewTrajectory->plug(menuView);
  viewDriverNames->plug(menuView);
  menuView->insertSeparator();
  QPopupMenu * menuMove = new QPopupMenu;
  moveUp->plug(menuMove);
  moveDown->plug(menuMove);
  moveLeft->plug(menuMove);
  moveRight->plug(menuMove);
  menuView->insertItem("Move", menuMove );

  menuBar()->insertItem("&View", menuView );

  QPopupMenu * menuCar = new QPopupMenu;
  carNext->plug(menuCar);
  carPrevious->plug(menuCar);
  menuBar()->insertItem("&Car", menuCar );

  // insert menuFollowCar as submenu of menuCar
  menuCar->insertSeparator();
  QPopupMenu * menuFollowCar = new QPopupMenu;
  followCarManual->plug(menuFollowCar);
  followCarNobody->plug(menuFollowCar);
  followCarPosition->plug(menuFollowCar);
  followCarOvertaking->plug(menuFollowCar);
  menuCar->insertItem("Follow &Car", menuFollowCar );

  QPopupMenu * menuSpeed = new QPopupMenu;
  speedRewind->plug(menuSpeed);
  speedStop->plug(menuSpeed);
  menuSpeed->insertSeparator();
  speedSlow->plug(menuSpeed);
  speedMedium->plug(menuSpeed);
  speedFast->plug(menuSpeed);
  menuBar()->insertItem("&Speed", menuSpeed );

  QPopupMenu * menuSettings = new QPopupMenu;
  settingsSave->plug(menuSettings);
  menuBar()->insertItem("Settings", menuSettings );

  speedRewind->plug(toolBar());
  speedStop->plug(toolBar());
  speedSlow->plug(toolBar());
  speedMedium->plug(toolBar());
  speedFast->plug(toolBar());
  carNext->plug(toolBar());
  carPrevious->plug(toolBar());
  zoomIn->plug(toolBar());
  zoomOut->plug(toolBar());

  statusBar()->message( "Ready!");

  main_part1();
  main_part2();

  this->setGeometry(0, 0, 624,510);
  widget_view = new QWidget( this );
  setCentralWidget(widget_view);
  InitDialog();
  // this->showMaximized();

  // create and setup a timer
  timer = new QTimer( this, "Timer" );
  connect( timer, SIGNAL( timeout() ), this, SLOT( slotTimeOut() ) );
  timer->start( 5, false );
}

/**
 * Slot: Next Car
 */
void KdeRars::slotCarNext()
{
  statusBar()->message( "Car: Next", 1000);
  g_iLastKey = UP;
}

/**
 * Slot: Previous Car
 */
void KdeRars::slotCarPrevious()
{
  statusBar()->message( "Car: Previous", 1000);
  g_iLastKey = DOWN;
}

/**
 * Slot: Speed rewind
 */
void KdeRars::slotSpeedRewind()
{
  statusBar()->message( "Speed: rewind", 1000);
  g_iLastKey = 'R';
}

/**
 * Slot: Speed slow
 */
void KdeRars::slotSpeedStop()
{
  statusBar()->message( "Speed: stop", 1000);
  g_iLastKey = 'E';
}

/**
 * Slot: Speed slow
 */
void KdeRars::slotSpeedSlow()
{
  statusBar()->message( "Speed: slow", 1000);
  g_iLastKey = 'S';
}

/**
 * Slot: Speed medium
 */
void KdeRars::slotSpeedMedium()
{
  statusBar()->message( "Speed: medium", 1000);
  g_iLastKey = 'D';
}

/**
 * Slot: Speed fast
 */
void KdeRars::slotSpeedFast()
{
  statusBar()->message( "Speed: fast", 1000);
  g_iLastKey = 'F';
}

/**
 * Slot: Change view in OpenGL mode
 */
void KdeRars::slotChangeView()
{
  statusBar()->message( "View: Change View", 1000);
  g_iLastKey = 'V';
}

/**
 * Slot: view in Zoom mode
 */
void KdeRars::slotViewClassic()
{
  statusBar()->message( "View: Mode Classic", 1000);
  SetOpenGLMode( MODE_CLASSIC );
}

/**
 * Slot: view to OpenGL Fullscreen Mode
 */
void KdeRars::slotViewFullScreen()
{
  statusBar()->message( "View: Mode Full screen", 1000);
  SetOpenGLMode( MODE_OPENGL_FULLSCREEN );
}

/**
 * Slot: view in Telemetry mode
 */
void KdeRars::slotViewTelemetry()
{
  statusBar()->message( "View: Mode Telemetry", 1000);
  SetOpenGLMode( MODE_TELEMETRY );
}

/**
 * Slot: view to OpenGL Window Mode
 */
void KdeRars::slotViewWindow()
{
  statusBar()->message( "View: Mode Window", 1000);
  SetOpenGLMode( MODE_OPENGL_WINDOW );
}

/**
 * Slot: view in Zoom mode
 */
void KdeRars::slotViewZoom()
{
  statusBar()->message( "View: Mode Zoom", 1000);
  SetOpenGLMode( MODE_ZOOM );
}

/**
 * Slot: view Trajectory
 */
void KdeRars::slotViewTrajectory()
{
  statusBar()->message( "View/Hide Trajectory", 1000);
  g_iLastKey = 'T';
}

/**
 * Slot: view driver names
 */
void KdeRars::slotViewDriverNames()
{
  statusBar()->message( "Show names", 1000);
  g_iLastKey = 'N';
}

/**
 * Slot: Follow Car Manual
 */
void KdeRars::slotFollowCarManual()
{
  statusBar()->message( "Follow Car: Manual Update", 1000);
  g_iLastKey = 'U';
}

/**
 * Slot: Follow Car Nobody
 */
void KdeRars::slotFollowCarNobody()
{
  statusBar()->message( "Follow Car: Nobody", 1000);
  g_iLastKey = 'I';
}

/**
 * Slot: Follow Car Overtaking
 */
void KdeRars::slotFollowCarOvertaking()
{
  statusBar()->message( "Follow Car: Any Overtaking", 1000);
  g_iLastKey = 'O';
}

/**
 * Slot: Follow Car Position
 */
void KdeRars::slotFollowCarPosition()
{
  statusBar()->message( "Follow Car: For Position", 1000);
  g_iLastKey = 'P';
}

/**
 * Slot: Zoom In
 */
void KdeRars::slotZoomIn()
{
  statusBar()->message( "Zoom: Zoom In", 1000);
  g_iLastKey = '+';
}

/**
 * Slot: Zoom out
 */
void KdeRars::slotZoomOut()
{
  statusBar()->message( "Zoom: Zoom Out", 1000);
  g_iLastKey = '-';
}

/**
 * Slot: Save Settings
 */
void KdeRars::slotSettingsSave()
{
  g_oRarsIni.SaveSettings();
  // Maybe show what was saved
}

/**
 * Slot: Move up, down, left, right,
 */
void KdeRars::slotMoveUp()
{
  g_iLastKey = ARROW_UP;
}
void KdeRars::slotMoveDown()
{
  g_iLastKey = ARROW_DOWN;
}
void KdeRars::slotMoveLeft()
{
  g_iLastKey = ARROW_LEFT;
}
void KdeRars::slotMoveRight()
{
  g_iLastKey = ARROW_RIGHT;
}

/**
 * Slot: Called by the timer 20 times per second
 */
void KdeRars::slotTimeOut()
{
  // The bActive trick is to allow the use of app.processEvents
  // in ShowInitMessage
  static bool bActive = false;
  if( bActive ) return;
  bActive = true;

  if( g_iCptMain==0 )
  {
     g_dlgInitCar = new KdeInitCar();
     g_dlgInitCar->ListMessage->clear();
     g_dlgInitCar->show();
  }
  else if( g_iCptMain==3 )
  {
     g_dlgInitCar->hide();
     delete g_dlgInitCar;
  }
  else
  {
    g_bToRefresh = false;
    while( !g_bToRefresh)
    {
      if( !main_part2() )
      {
        timer->stop();
        main_part3();
        statusBar()->message( "Race Finished !", 30000);
        return;
      }
    }

    int iViewMode = g_ViewManager->GetViewMode();
    if( iViewMode==MODE_OPENGL_FULLSCREEN )
    {
      image_opengl->update();
    }
    else if( iViewMode==MODE_CLASSIC || iViewMode==MODE_TELEMETRY )
    {
      image[0]->update();
    }
    else
    {
      if( iViewMode==MODE_OPENGL_WINDOW )
      {
        image_opengl->update();
      }
      else
      {
        image[0]->update();
      }
      image[1]->update();
      image[2]->update();
    }
  }
  g_iCptMain++;

  bActive = false;
}

/**
 * Called when the windows resizes
 */
void KdeRars::resizeEvent ( QResizeEvent * )
{
  int w = widget_view->width();
  int h = widget_view->height();
  m_offx = max( 0, w-624 );
  m_offy = max( 0, h-432 );  // and not 510 ??
  m_offx = m_offx-(m_offx%8);

  // <to_remove>
  g_ViewManager->DeleteAllViews();
  g_ViewManager->InitAllViews();
  // </to_remove>

  g_ViewManager->ResizeAllViews( m_offx, m_offy );
  SetImageGeometry();
}

void KdeRars::SetOpenGLMode( int mode )
{
//  timer->stop();
  g_ViewManager->DeleteAllViews();
  g_ViewManager->m_iViewMode = mode;
  g_ViewManager->InitAllViews();
  g_ViewManager->ResizeAllViews( m_offx, m_offy );
  SetImageGeometry();
//  timer->start( 50, false );
}

void KdeRars::InitDialog()
{
  image_opengl = new MyOpenGL(widget_view, "OpenGL");
  image[0] = new MyImage(0, widget_view);
  image[1] = new MyImage(1, widget_view);
  image[2] = new MyImage(2, widget_view);

  SetImageGeometry();
}

void KdeRars::SetImageGeometry()
{
  int iViewMode = g_ViewManager->GetViewMode();
  if( iViewMode==MODE_OPENGL_FULLSCREEN )
  {
    // wa problem resize
    image_opengl->hide();
    this->removeChild( image_opengl );
    delete image_opengl;
    image_opengl = new MyOpenGL(widget_view, "OpenGL");
    image_opengl->setGeometry( 0, 0, 624+m_offx, 432+m_offy );
    image_opengl->show();
    image[0]->hide();
    image[1]->hide();
    image[2]->hide();
  }
  else if( iViewMode==MODE_CLASSIC || iViewMode==MODE_TELEMETRY )
  {
    image[0]->setGeometry( 0, 0, 624+m_offx, 432+m_offy );
    image[0]->show();
    image_opengl->hide();
    image[1]->hide();
    image[2]->hide();
  }
  else
  {
    if( iViewMode==MODE_OPENGL_WINDOW )
    {
      // wa problem resize
      image_opengl->hide();
      this->removeChild( image_opengl );
      delete image_opengl;
      image_opengl = new MyOpenGL(widget_view, "OpenGL");
      image_opengl->setGeometry( 8, 8, 372+m_offx, 250+m_offy );
      image_opengl->show();
      image[0]->hide();
    }
    else
    {
      image[0]->setGeometry( 8, 8, 372+m_offx, 250+m_offy );
      image[0]->show();
      image_opengl->hide();
    }
    image[1]->setGeometry( 388+m_offx, 8, 220, 250+m_offy );
    image[2]->setGeometry( 8, 266+m_offy, 600+m_offx, 160 );
    image[1]->show();
    image[2]->show();
  }
}

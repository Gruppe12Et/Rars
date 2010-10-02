//--------------------------------------------------------------------------
//
//    FILE: G_VIEW3D.CPP (portable)
//
//    TView3D
//      - This is the OpenGL view.
//
//    Version       Author          Date
//      0.1      Marc Gueury     12 /10 /97
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
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include "g_global.h"
#include "../3ds/model_3ds.h"
#include "g_track3d.h"

#include "../misc.h"

// #define GLUT_TEST
#ifdef GLUT_TEST
  #include <glut.h>
#endif

//--------------------------------------------------------------------------
//                           D E F I N E S
//--------------------------------------------------------------------------

#define FX_SCALE(a) float( a )
#define FY_SCALE(a) float( a )

GLTexture * t_road;
GLTexture * t_sky;
GLTexture * t_grass;
GLTexture * t_red_brick;
GLTexture * t_pitstop;

TTrack3D track3D;

// Display Lists
#define LIST_ROAD             1

#ifdef WIN32
  LARGE_INTEGER oldCount, newCount;
  LARGE_INTEGER frequency;
  int framecount = 0;
  double cum_fps = 0;
  double fps;
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
#ifdef WIN32
TView3D::TView3D( int x, int y, int offx, int offy, int d_m_FollowCar ) : TView( 0, 0 )
{
  Init( x, y, offx, offy, true );
#else
TView3D::TView3D( int, int, int, int, int d_m_FollowCar ) : TView( 0, 0 )
{
#endif
  m_dFollowCar = d_m_FollowCar;
  m_OptionShowBoard = 0;
}

/**
 * Cosntructor used by the Track Editor
 */
TView3D::TView3D() : TView( 0, 0 )
{}

void TView3D::Init( int x, int y, int offx, int offy, bool bFirstTime )
{
  #ifdef WIN32
    QueryPerformanceFrequency(&frequency);
  #endif

  m_SizeX = x;
  m_SizeY = y;

  GL_InitWidget( x, y, offx, offy );
  opengl.Init( x, y, MaxDist() );

  if( bFirstTime )
  {
    // Read the textures
    t_road      = GLTexture::GetLoadedTexture( "bitmap/", "road2.tga", false );
    t_sky       = GLTexture::GetLoadedTexture( "bitmap/", "sky.tga", true );
    t_grass     = GLTexture::GetLoadedTexture( "bitmap/", "grass.tga", true );
    t_red_brick = GLTexture::GetLoadedTexture( "bitmap/", "red_brick.tga", true );
    t_pitstop   = GLTexture::GetLoadedTexture( "bitmap/", "pitstop.tga", true );
  }
  else
  {
    // Rebind the texture to OpenGL
    GLTexture::RebindLoadedTexture();
  }

  opengl.BuildFont();                    // build the font

  // Init the variables
  m_CenterX = 0;
  m_CenterY = 0;
  m_CenterZ = 0;
  m_Ang = 0;
  m_CamX = 0;
  m_CamY = 0;
  m_FollowCarTime = 0;
  m_Zoom = g_oRarsIni.m_fZoom;
  m_iTypeView = g_oRarsIni.m_iViewOpenGL;
  m_TrackBitmap = NULL;
  m_b3DS  = true;
  m_bInit = true;
}

/**
 * Destructor
 */
TView3D::~TView3D()
{
  Destroy();
  GLTexture::DeleteLoadedTexture();
  opengl.Destroy();
}

/**
 * Distance maximum of a object
 */
double TView3D::MaxDist()
{
  return 5000.0;
}

void TView3D::ResetRotation()
{
  m_fAngleX = -30.0;
  m_fAngleZ = 0.0;
  m_fSpeedAngleX = m_fSpeedAngleZ = 0.0;
}

void TView3D::Destroy()
{
  // all lists should be deleted
  glDeleteLists( LIST_ROAD, 6 );

  if( m_TrackBitmap )
  {
    delete m_TrackBitmap;
    m_TrackBitmap = NULL;
  }
  opengl.KillFont();
  GL_DestroyWidget();

  if( g_ViewManager->m_i3DS!=0 )
  {
    track3D.Destroy();
  }
  GLTexture::UnbindLoadedTexture();
  model_loader.Reset();
}

/**
 * Resize the bitmap and the related parameters
 * 

 * @param x             the new size of the graphic 
 * @param y             the new size of the graphic (view)
 */
void TView3D::Resize( int x, int y, int offx, int offy )
{
  TLowGraphic::Resize( x, y );

  // GL_DestroyWidget();
  Destroy();
  Init( x, y, offx, offy, false );
}

/**
 * Refresh
 *
 * Refresh the bitmap
 */
void TView3D::Refresh()
{
#ifdef WIN32
  DrawAll();
#endif
}

void TView3D::DrawAll()
{
  double banking;

  // Change view 
  if( g_ViewManager->m_bChangeView )
  {
    m_Zoom = 0.01;
    g_ViewManager->m_bChangeView  = FALSE;
    m_iTypeView++;
    if( m_iTypeView>7 ) m_iTypeView=0;
  }

  // starting
  if( m_bInit ) 
  {
    m_bInit = false;

    // Creation des listes OpenGL
    GL_CreateList();

    m_CamX = ( m_CenterX = race_data.cars[m_iFollowCar]->x ) + 200;
    m_CamY =   m_CenterY = race_data.cars[m_iFollowCar]->y;
    m_CenterZ = 100;

    if( g_ViewManager->m_i3DS!=0 )
    {
      track3D.Load();
    }
    m_TrackBitmap = new TTrackBitmap( 160, 160 );
  } 

  // the user is changing 'm_FollowCar' ?
  if( g_ViewManager->m_iFollowMode==FOLLOW_NOBODY )
  {
    MoveWithKeys();
  }
  else
  {
    if( m_FollowCarTime == 0 ) 
    {  
      float frac = ( g_ViewManager->m_iRealSpeed==REAL_SPEED_FAST )?0.99f:0.7f;
      m_CenterX = m_CenterX*(1-frac) + race_data.cars[m_iFollowCar]->x*frac;
      m_CenterY = m_CenterY*(1-frac) + race_data.cars[m_iFollowCar]->y*frac;
      m_CenterZ = m_CenterZ*(1-frac) + track3D.CarHeight( m_iFollowCar, 0, banking )*frac;
    }
    else 
    {
      double aux = (0.05*(80-m_FollowCarTime)+0.95*m_FollowCarTime)/80.0;
      m_CenterX = m_CenterX*aux + race_data.cars[m_iFollowCar]->x*(1.0-aux);
      m_CenterY = m_CenterY*aux + race_data.cars[m_iFollowCar]->y*(1.0-aux);
      m_CenterZ = m_CenterZ*aux + track3D.CarHeight( m_iFollowCar, 0, banking )*(1.0-aux);
      m_FollowCarTime--;
    }
  }

  {
    double a = race_data.cars[m_iFollowCar]->ang + race_data.cars[m_iFollowCar]->alpha*2;//JCer
    double b = a+2*PI;
    if( fabs(m_Ang-a)>fabs(m_Ang-b) ) a = b;
    b = a-2*PI;
    if( fabs(m_Ang-a)>fabs(m_Ang-b) ) a = b;
    m_Ang = g_ViewManager->m_iRealSpeed == REAL_SPEED_FAST 
          ? (m_Ang*0.4 + a*0.6)  // fast
          : (m_Ang*0.9 + a*0.1); // slow
    if( m_Ang>2*PI ) m_Ang -=2*PI;
    if( m_Ang<0    ) m_Ang +=2*PI;
  }

  // zoom
  m_Zoom *= g_ViewManager->m_ZoomDelta;
  m_Zoom = min( max( m_Zoom, 0.0005 ), 0.02 );

  // refresh the bitmap
  // g_GL_CWnd.SetFocus();
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  // glClear(GL_DEPTH_BUFFER_BIT);
  glPushMatrix ();  // master view


  m_fAngleZ = 0.0;
  switch( m_iTypeView )
  {
    case 0:
    { // fixed angle
      glTranslatef (0.0f, 0.0f, -200.0f*100.f*float(m_Zoom) );
      glRotatef (-80.0f,1.0f,0.0f,0.0f);
      break;
    } 
    case 1:
    { // above
      // glTranslatef (0.0f, 0.0f, -1000.0f*100.f*float(m_Zoom) );
      glTranslatef (0.0f, 0.0f, -50.0f*100.f*float(m_Zoom) );
      glRotatef (-30.0f,1.0f,0.0f,0.0f);
      break;
    } 
    case 2:
    { // above and behind
      glTranslatef (0.0f, 0.0f, -150.0f*100.f*float(m_Zoom) );
      glRotatef (-75.0f,1.0f,0.0f,0.0f);
      m_fAngleZ = 90.0-m_Ang*180.0/PI; 
      glRotatef( float(m_fAngleZ), 0.0f, 0.0f, 1.0f );
      break;
    }  
    case 3:
    { // close and behind
      glTranslatef (0.0f, -6.5f*100.f*float(m_Zoom), -10.0f*100.f*float(m_Zoom) ); 
      glRotatef (-75.0f,1.0f,0.0f,0.0f); 
      m_fAngleZ = 90.0-m_Ang*180.0/PI; 
      glRotatef( float(m_fAngleZ), 0.0f, 0.0f, 1.0f );
      break;
    } 
    case 4:
    { // inside
      glTranslatef (0.0f, -3.75f*float(m_Zoom*100), -2.0f ); 
      glRotatef (-90.0f,1.0f,0.0f,0.0f); 
      m_fAngleZ = 90.0-m_Ang*180.0/PI; 
      glRotatef( float(m_fAngleZ), 0.0f, 0.0f, 1.0f );
      break;
    } 
    case 5:
    { // Helicopter
      static int s_ang = 0;
      if( ++s_ang==360 ) s_ang=0;
      if( fabs(cos( s_ang * PI / 180. )) < cos(30.*PI/180) )
        s_ang+=5;
      glTranslatef (0.0f, 0.0f, -200.0f*100.f*float(m_Zoom) );
      glRotatef (-75.0f,1.0f,0.0f,0.0f);
      m_fAngleZ = 90.0+s_ang-m_Ang*180.0/PI; 
      glRotatef( float(m_fAngleZ), 0.0f, 0.0f, 1.0f );
      break;
    }
    case 6:
    { // Ground cameras
      long double z_ang, dx, dy, distance, r;
      static long double Height=90;
      int max_d, extra_height;
        
      // change parameers depending on smulation speed
      if( g_ViewManager->m_iRealSpeed!=REAL_SPEED_FAST ) // slow
      { 
        max_d = 500;  extra_height = r_rand()%91; 
      }
      else // fast
      { 
        max_d = 1000; extra_height = 90; Height = 95;
      }
        
      dx = m_CenterX - m_CamX;
      dy = m_CenterY - m_CamY;

      // calculate radius on the xy plane
      r = sqrt( dx*dx + dy*dy );
      // calculate distance in 3D
      distance = sqrt(r*r+Height*Height);
      
      if( distance > max_d ) // too far
      { 
          // calculate new camera position
          Height = 10+extra_height;

          // put camera in front of followed car
          dx = -race_data.cars[m_iFollowCar]->xdot*10;
          dy = -race_data.cars[m_iFollowCar]->ydot*10;

          // calculate radius on the xy plane
          r = sqrt( dx*dx + dy*dy );
          // calculate distance in 3D
          distance = sqrt(r*r+Height*Height);
          
          if( distance > max_d*0.75)
          { // it is too far too
            double factor = sqrt(max_d*max_d*0.75*0.75-Height*Height)/r;
            dx *= factor;
            dy *= factor;
          }

          m_CenterX = race_data.cars[m_iFollowCar]->x;
          m_CenterY = race_data.cars[m_iFollowCar]->y;
          m_CamX = m_CenterX-dx;
          m_CamY = m_CenterY-dy;
      }
        
      z_ang = atan2( dy,dx ) - PI/2.;
      glTranslatef(0.0f, 0.0f, -float(distance*m_Zoom*50));
      m_fAngleZ = -z_ang*180.0/PI; 
      glRotatef( float(m_fAngleZ), 0.0f, 0.0f, 1.0f );
      glRotatef( -float(atan2(r,Height)*180./PI),float(dy),float(-dx),0.0f);
      break;
    }
    case 7:
    {
      glTranslatef (0.0f, 0.0f, -400.0f*100.f*float(m_Zoom) );
      glRotatef (-50.0f,1.0f,0.0f,0.0f);
      break;
    }
  }
  glTranslatef (-GLfloat(m_CenterX), -GLfloat(m_CenterY), -GLfloat(m_CenterZ) );

  // Calculate the cone of view (Fustrum)
  ExtractFrustum();

  glEnable(GL_TEXTURE_2D);

  if( g_ViewManager->m_i3DS!=0 )
  {
    track3D.DrawSky( m_CenterX, m_CenterY );
    GL_EnableLight();
    track3D.Draw();
  }
  else
  {
    // The sky
    if( m_iTypeView!=1 && m_iTypeView!=7 ) 
    {
      glBindTexture( GL_TEXTURE_2D, t_sky->m_iOpenglId );
      glBegin (GL_QUADS);
      glTexCoord2f(0.0,0.0);
      glVertex3f( -20000.0f, -20000.0f, 100.0f);
      glTexCoord2f(8.0,0.0);
      glVertex3f( +20000.0f, -20000.0f, 100.0f);
      glTexCoord2f(8.0,8.0);
      glVertex3f( +20000.0f, +20000.0f, 100.0f);
      glTexCoord2f(0.0,8.0);
      glVertex3f( -20000.0f, +20000.0f, 100.0f);
      glEnd ();
    }

    // The grass
    glBindTexture( GL_TEXTURE_2D, t_grass->m_iOpenglId );
    glBegin (GL_QUADS);
    glTexCoord2f(0.0,0.0);
    glVertex3f( -10000.0f, -10000.0f, -5.0f);
    glTexCoord2f(8.0,0.0);
    glVertex3f( +10000.0f, -10000.0f, -5.0f);
    glTexCoord2f(8.0,8.0);
    glVertex3f( +10000.0f, +10000.0f, -5.0f);
    glTexCoord2f(0.0,8.0);
    glVertex3f( -10000.0f, +10000.0f, -5.0f);
    glEnd ();

    // The road
    glCallList( LIST_ROAD );
  }

  #ifdef WIN32
    QueryPerformanceCounter(&oldCount);
  #endif
  
  DrawCars();

  #ifdef WIN32
	  QueryPerformanceCounter(&newCount);
  #endif

  // glDisable(GL_CULL_FACE);
  glDisable(GL_LIGHT0);
  glDisable(GL_LIGHTING);
  glDisable(GL_TEXTURE_2D);

  ////
  //// Trajectory of the current car
  ////
  if( draw.m_bDrawTrajectory )
  {
    if( args.m_iNumCar<=3 )
    {
      for( int i=0; i<args.m_iNumCar; i++ )
      {
        DrawTrajectory( i );
      }
    }
    else
    {
      DrawTrajectory( m_iFollowCar );
    }
  }
  glPopMatrix ();   // pop master view

  if( g_ViewManager->m_ImageCpt!=1 && m_OptionShowBoard )
  {
    DrawBoard();
  }

  ////
  //// Instant Replay
  ////
  if( g_ViewManager->m_oInstantReplay->m_iMode!=INSTANT_RECORD )
  {
    // draw a turning R if InstantReplay is activated
    static GLfloat rot = 0;
    glPushMatrix ();  

    static GLfloat mat_replay1[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
    static GLfloat mat_replay2[4]  = { 0.7f, 0.7f, 0.7f, 1.0f };
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_replay1 );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_replay2 );
    glColor4f( 1, 1, 1, 1 );

    glTranslatef (+16.0f, 11.0f, -30.0f );
    glRotatef (+15.0f,1.0f,0.0f,0.0f);
    glRotatef (rot,0.0f,1.0f,0.0f);
    glShadeModel(GL_SMOOTH);
    glEnable(GL_LIGHTING);
    glEnable(GL_LIGHT0);
    ModelInstantReplay->Draw();
    glDisable(GL_LIGHT0);
    glDisable(GL_LIGHTING);
    glPopMatrix (); 

    rot += 5.0;
    if( rot>360.0 ) rot=-360.0;
  }

  // glFinish();

  #ifdef WIN32
	  double cur_fps = (double)frequency.QuadPart/(double)(newCount.QuadPart - oldCount.QuadPart);
	  cum_fps += cur_fps;
	  framecount++;
    if(framecount > 30)
    {
      fps = cum_fps / 30.f;
	    framecount = 0;
	    cum_fps = 0;
    }
  #endif

  GL_SwapWidget();
  g_bToRefresh = TRUE;
}

/**
 * EnableLight
 * 
 * Enable the light
 */
void TView3D::GL_EnableLight()
{
  static GLfloat LightDiffuse[]= { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat LightAmbient[]= { 0.5f, 0.5f, 0.5f, 1.0f };
  static GLfloat LightSpec[]   = { 1.0f, 1.0f, 1.0f, 1.0f };
  static GLfloat LightPos[]    = { 0.0f, -10000.0,10000.0,1.0};
  static GLfloat mat_light[]   = { 1.0f, 1.0f, 1.0f, 1.0f }; // Color of a lighted face
  static GLfloat mat_shadow[]  = { 0.6f, 0.6f, 0.6f, 1.0f }; // Color of a shadowed face

  glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_light );
  glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_shadow );
  glLightfv(GL_LIGHT0,GL_DIFFUSE, LightDiffuse);
  glLightfv(GL_LIGHT0, GL_AMBIENT, LightAmbient);  
  glLightfv(GL_LIGHT0,GL_SPECULAR,LightSpec);
  glLightfv(GL_LIGHT0,GL_POSITION,LightPos);

  glEnable(GL_LIGHTING);
  glEnable(GL_LIGHT0);
}

/**
 * FollowCar
 * 
 * Change the car to follow
 * @param car_nr : the new_car to follow in the view
 */
void TView3D::FollowCar( int car_nr )
{
  if( m_iFollowCar<0 || m_iFollowCar>=args.m_iNumCar || m_iFollowCar==car_nr )
    return;
  m_iFollowCar = car_nr + m_dFollowCar;
  m_FollowCarTime = (g_ViewManager->m_iRealSpeed==REAL_SPEED_FAST) ? 40:80;
}

/**
 * Draw all the cars
 */
void TView3D::DrawCars()
{
  for( int i=0; i<args.m_iNumCar; i++ )  // for each car:
  {       
    if( m_b3DS /*&& dist<300.0*/ )
    {
      GL_DrawCar3DS( i );
    }
    else
    {
      GL_DrawCar(i);
    }
  }
}

/**
 * Draw the board (if option_show_board==1)
 */
void TView3D::DrawBoard()
{
  Car * car = race_data.cars[m_iFollowCar];
  Driver * driver = drivers[m_iFollowCar];

  float aspect = float(m_SizeX)/m_SizeY;
  float ux = 0.008f*aspect;
  float uy = 0.01f;

  glPushMatrix();  
  glColor4f( 0.8f, 0.8f, 0, 1 );

  glTranslatef (-50*ux, 35*uy, -1 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iBigFont, "%d", race_data.m_aPosOfCar[m_iFollowCar]+1 );
  opengl.glPrint( opengl.m_iSmallFont, " th" );
  glTranslatef (85*ux, 0, 0 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Lap " );
  opengl.glPrint( opengl.m_iBigFont, " %d/%d", car->Laps, args.m_iNumLap );
  glTranslatef (0, -4.5f*uy, 0 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Best" );
  opengl.glPrint( opengl.m_iBigFont, " %.2f", car->bestlap_speed * MPH_FPS );
  glTranslatef (0, -4.5f*uy, 0 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Last" );
  opengl.glPrint( opengl.m_iBigFont, " %.2f", car->lastlap_speed * MPH_FPS );
  glPopMatrix();  

  glPushMatrix();  
  glTranslatef (-2*ux, -32*uy, -1 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "%s", driver->getName() );
  glTranslatef (-6*ux, -4*uy, 0 );
  glRasterPos2f(0, 0);
  double v = vec_mag(car->xdot, car->ydot);
  opengl.glPrint( opengl.m_iBigFont, "%7.2f", v*MPH_FPS );
  opengl.glPrint( opengl.m_iSmallFont, " MPH" );
  glPopMatrix();  
  
  glPushMatrix();  
  glTranslatef (-50*ux, -30*uy, -1 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Damage: %lu", car->damage );
  glTranslatef (0, -3*uy, 0 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Fuel: %.2f", car->fuel );
  glTranslatef (0, -3*uy, 0  );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "%.2f fps", g_ViewManager->m_fFps );
  glPopMatrix();

  /*
  glPushMatrix();  
  glTranslatef (-50*ux, -33*uy, -1 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "%.2f fps", g_ViewManager->m_fFps );
  glTranslatef (0, -3*uy, 0 );
  glRasterPos2f(0, 0);
  opengl.glPrint( opengl.m_iSmallFont, "Fill Fps : %.2f", fps );
  glPopMatrix();
  */

  glPushMatrix();  
  glColor4f( 0.9f, 0.9f, 0.9f, 1 );
  glTranslatef (-50*ux, 12*uy, -1 );
  m_TrackBitmap->Draw();
  glPopMatrix();  

  glColor4f( 1, 1, 1, 1 );
}

/**
 * Draw one car with the 3DS model
 */
void TView3D::GL_DrawCar3DS( int i )
{
  double banking1, banking2;
  float x = float(race_data.cars[i]->X),     // and not ->x (->X is the previous similation step coherent with seg_id and to_end)
        y = float(race_data.cars[i]->Y),
        z_front = float( track3D.CarHeight(i, -CARLEN/2, banking1) ),
        z_back  = float( track3D.CarHeight(i, CARLEN/2, banking2) ),
        z = (z_front+z_back)/2,
        ang = float(race_data.cars[i]->ang*180.0/PI);
  double banking = ( banking1 + banking2 ) / 2;

  // Store the z value to draw the trajectory
  race_data.cars[i]->Z = z; 

  double dist= SphereInFrustum( x, y, z, float(CARLEN+CARWID) );
  if( dist==0 ) // dist==0 -> car not in fustrum
  {
    return;
  }

  int color1 = race_data.cars[i]->nose_color;
  int color2 = ( race_data.cars[i]->collision_draw
    ? FLASH_COLOR
    :  race_data.cars[i]->tail_color
    );

  float lin_acc = float(race_data.cars[i]->get_lin_acc()),
        lat_acc = float(race_data.cars[i]->get_lat_acc()),
        alpha   = float(race_data.cars[i]->get_alpha());
  
  #define H_CAR1 2.5f
  #define H_CAR2 4.5f
  #define H_WMID 1.0f
  #define H_WTOP 3.0f
  #define W_WFWD 1.5f
  #define W_WRER 2.5f
  TColor * pColor;
  
  glPushMatrix ();  
  glTranslatef (x, y, z );
  double ang_with_road = -track3D.CarAngWithRoad(i)*180/PI;
  glRotatef( float(ang-ang_with_road), 0, 0, 1 );
  glRotatef( float(banking), 1, 0, 0 );
  glRotatef( float(ang_with_road), 0, 0, 1 );
  float z_ang = (float)( atan2( z_back-z_front, CARLEN )*180.0/PI );
  glRotatef( z_ang, 0, 1, 0 );

  ////
  //// Color
  ////
  pColor = &Palette.m_Color[color1];
  GLfloat mat_color1[4] = { pColor->fr, pColor->fg, pColor->fb, 1.0f };
  pColor = &Palette.m_Color[color2];
  GLfloat mat_color2[4] = { pColor->fr, pColor->fg, pColor->fb, 1.0f };
  GLfloat mat_dark_color2[4] = { pColor->fr/2, pColor->fg/2, pColor->fb/2, 1.0f };

  static GLfloat mat_wheel[4]  = { 0.1f, 0.1f, 0.1f, 1.0f };
  static GLfloat mat_gray[4]   = { 0.1f, 0.1f, 0.1f, 1.0f };
  static GLfloat mat_black[4]  = { 0.0f, 0.0f, 0.0f, 1.0f };
  static GLfloat mat_white[4]  = { 1.0f, 1.0f, 1.0f, 1.0f };
  // static GLfloat mat_light_gray[4]  = { 0.7f, 0.7f, 0.7f, 1.0f };
  static GLfloat high_shininess[] = { 20.0 };
  static GLfloat low_shininess[] = { 10.0 };

  if( race_data.cars[i]->out == 0 )
  {
    ////
    //// draw acceleration Vector
    ////
    float x,y;
    x=float(lat_acc*CARLEN*2);
    y=float(lin_acc*CARLEN*2);
    glBegin (GL_LINE_STRIP);
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_color2 );
    glVertex3f( GLfloat(+0), GLfloat(-0), H_CAR2);
    glVertex3f( GLfloat(+y), GLfloat(+x), H_CAR2);
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_color1 );
    glVertex3f( GLfloat(+y), GLfloat(+x), GLfloat(+0.2));
    glVertex3f( GLfloat(+0), GLfloat(-0), GLfloat(+0.2));
    glEnd();
  }
  glRotatef (alpha*180.f/3.1416f,0.0f,0.0f,1.0f);

  ////
  //// Roues, Ruedas, Wheels
  ////
  // The wheels have a high shininess
  glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_white);
  glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);

  if( m_aFrontWheel[i].m_pModel==NULL )
  {
    // FOR NEXT VERSION, replace that by a common wheel mechanism
    glDisable(GL_TEXTURE_2D);
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_wheel );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_gray );

    float px = -GLfloat(CARWID/2)-0.5f+W_WRER/2;
    float py = GLfloat(-CARLEN/3)+H_WTOP/2;
    glPushMatrix ();  
    glTranslatef (py, px, 0.0f );
    ModelWheelRear->Draw();
    glPopMatrix ();  

    glPushMatrix ();  
    glTranslatef (py, -px, 0.0f );
    ModelWheelRear->Draw();
    glPopMatrix ();  

    px = -GLfloat(CARWID/2)-0.5f+W_WFWD/2;
    glPushMatrix ();  
    glTranslatef (-py, +px, 0.0f );
    ModelWheelFront->Draw();
    glPopMatrix ();  

    glPushMatrix ();  
    glTranslatef (-py, -px, 0.0f );
    ModelWheelFront->Draw();
    glPopMatrix ();  
  }
  else
  {
    // Custom model
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_white );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_gray );

    GL_DrawWheel3DS( &(m_aFrontWheel[i]), i, x, y ); 
    GL_DrawWheel3DS( &(m_aBackWheel[i]),  i, x, y ); 
  }

  // addition by JCer
  // push car's suspention
  { 
    glRotatef (-float(atan(1./CARLEN)*180/3.1415926*lin_acc),float(sin(alpha)),float(cos(alpha)),0.0f);
    glRotatef ( float(atan(1./CARWID)*180/3.1415926*lat_acc),float(cos(alpha)),float(sin(alpha)),0.0f);
  };

  ////
  //// Draw the car
  ////
  if( m_aModelBody[i]==NULL )
  {
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, low_shininess);
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_color2 );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_dark_color2 );

    ModelCarBody->Draw();

    // Draw the helmet
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_color1 );
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_black );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
    
    glTranslatef (0, 0, -0.6f );
    ModelHelmet->Draw();
  }
  else
  {
    // Custom model
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT, mat_gray );
    glMaterialfv( GL_FRONT_AND_BACK, GL_DIFFUSE, mat_white );
    glMaterialfv( GL_FRONT_AND_BACK, GL_SPECULAR, mat_white);
    glMaterialfv( GL_FRONT_AND_BACK, GL_SHININESS, high_shininess);
    // m_aModelBody[i]->DrawList( false );
    // m_aModelBody[i]->DrawList( true );
    m_aModelBody[i]->Draw();
  }

  #ifdef GLUT_TEST
    glTranslatef (0, 0, 20.0f );
    glutSolidSphere(2.0, 20, 20);
  #endif

  if (g_ViewManager->m_bShowNames)
  {
    glMaterialfv( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE, mat_color2 );
    GL_DrawCarName( i );
  }

  glPopMatrix ();  
}

/**
 * Draw a 3DS Wheel
 */
void TView3D::GL_DrawWheel3DS( TWheel3D * wheel, int i, float x, float y )
{
  float dx = wheel->m_fLastX - x;
  float dy = wheel->m_fLastY - y;
  float dist = (float)sqrt( dx*dx + dy*dy );
  float dang = (dist/wheel->m_fPerimeter)*360.0f;
  if( dang> 22.0f ) dang = 22.0f;
  wheel->m_fAng += dang; 
  if( wheel->m_fAng>360.0f || wheel->m_fAng<0.0f )
  {
    wheel->m_fAng -= (int)(wheel->m_fAng/360.0f)*360.0f;
  }
  wheel->m_fLastX = x;
  wheel->m_fLastY = y;

  glPushMatrix ();  
  glTranslatef( wheel->m_fCenterX, wheel->m_fCenterY, wheel->m_fCenterZ );
  glRotatef( wheel->m_fAng, 0.0f, 1.0f, 0.0f );
  wheel->m_pModel->Draw();
  glPopMatrix ();  

  glPushMatrix ();  
  glTranslatef( wheel->m_fCenterX, -wheel->m_fCenterY, wheel->m_fCenterZ );
  glRotatef( 180.0f, 0.0f, 0.0f, 1.0f );
  glRotatef( -wheel->m_fAng, 0.0f, 1.0f, 0.0f );
  wheel->m_pModel->Draw();
  // wheel->m_pModel->DrawList( false );
  glPopMatrix ();  
}

/**
 * Draw one car without the 3DS model
 */
void TView3D::GL_DrawCar( int i )
{
  double banking;
  float x = float(race_data.cars[i]->x),
        y = float(race_data.cars[i]->y),
        z = float( track3D.CarHeight(i, 0, banking) ),
        ang = float(race_data.cars[i]->ang*180.0/PI);

  // Store the z value to draw the trajectory
  race_data.cars[i]->Z = z; 

  double dist= SphereInFrustum( x, y, z, float(CARLEN+CARWID) );
  if( dist==0 ) // dist==0 -> car not in fustrum
  {
    return;
  }
 
  int   color1 =   race_data.cars[i]->nose_color,
        color2 =   race_data.cars[i]->tail_color;
  float lin_acc = float(race_data.cars[i]->get_lin_acc()),
        lat_acc = float(race_data.cars[i]->get_lat_acc()),
        alpha = float(race_data.cars[i]->get_alpha());

  #define H_CAR1 2.5f
  #define H_CAR2 4.5f
  #define H_WMID 1.0f
  #define H_WTOP 3.0f
  #define W_WFWD 1.5f
  #define W_WRER 2.5f
  TColor * pColor;

  glPushMatrix ();  
  glTranslatef (x, y, z );
  glRotatef (ang,0.0f,0.0f,1.0f);

  // print the name of the driver above the car
  if (g_ViewManager->m_bShowNames)
  {
    pColor = &Palette.m_Color[color1];
    glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
    GL_DrawCarName( i );
  }

  if( race_data.cars[i]->out == 0 )
  {
    ////
    //// draw acceleration Vector
    ////
    float x,y;
    x=float(lat_acc*CARLEN*2);
    y=float(lin_acc*CARLEN*2);
    glBegin (GL_LINE_STRIP);
    pColor = &Palette.m_Color[color2];
    glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
    glVertex3f( GLfloat(+0), GLfloat(-0), H_CAR2);
    glVertex3f( GLfloat(+y), GLfloat(+x), H_CAR2);
    pColor = &Palette.m_Color[color1];
    glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
    glVertex3f( GLfloat(+y), GLfloat(+x), GLfloat(+0.2));
    glVertex3f( GLfloat(+0), GLfloat(-0), GLfloat(+0.2));
    glEnd ();
  } 
  glRotatef(alpha*180.f/float(PI),0.0f,0.0f,1.0f);

  ////
  //// Roues, Ruedas, Wheels
  ////
  float px = GLfloat(CARWID/2)+0.5f;

  pColor = &Palette.m_Color[
             race_data.cars[i]->collision_draw 
             ? FLASH_COLOR
             : oBLACK ];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);

  // Rear Right
  glBegin (GL_QUAD_STRIP); // around
  glVertex3f( GLfloat(-CARLEN/3)       , -px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , -px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px+W_WRER, 0.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px+W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , -px+W_WRER, 0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , -px+W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , -px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , -px,        H_WTOP);
  glEnd();
  glBegin (GL_QUADS); // top
  glVertex3f( GLfloat(-CARLEN/3)       , -px+W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px+W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, -px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , -px,        H_WTOP);
  glEnd ();
  // Rear Left
  glBegin (GL_QUAD_STRIP); // around
  glVertex3f( GLfloat(-CARLEN/3)       , +px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , +px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px-W_WRER, 0.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px-W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , +px-W_WRER, 0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , +px-W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , +px,        0.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , +px,        H_WTOP);
  glEnd ();
  glBegin (GL_QUADS); // top
  glVertex3f( GLfloat(-CARLEN/3)       , +px-W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px-W_WRER, H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, +px,        H_WTOP);
  glVertex3f( GLfloat(-CARLEN/3)       , +px,        H_WTOP);
  glEnd ();
  // Front Right
  glTranslatef (GLfloat(+CARLEN/3)-H_WTOP*0.9f*0.5f, -px+W_WFWD, 0.0f );
  glRotatef (alpha*180.f/float(PI),0.0f,0.0f,1.0f);
  glBegin (GL_QUAD_STRIP); // around
  glVertex3f( +H_WTOP*0.9f*0.5f             , -W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , -W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, -W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, -W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , -W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , -W_WFWD,    H_WTOP*0.9f);
  glEnd ();
  glBegin (GL_QUADS); // top
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, -W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , -W_WFWD,    H_WTOP*0.9f);
  glEnd ();
  // Front Left
  glRotatef (-alpha*180.f/float(PI),0.0f,0.0f,1.0f);
  glTranslatef (0.0f, 2*(+px-W_WFWD), 0.0f );
  glRotatef (alpha*180.f/float(PI),0.0f,0.0f,1.0f);
  glBegin (GL_QUAD_STRIP); // around
  glVertex3f( +H_WTOP*0.9f*0.5f             , +W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , +W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, +W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, +W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , +W_WFWD,    0.0f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , +W_WFWD,    H_WTOP*0.9f);
  glEnd ();
  glBegin (GL_QUADS); // top
  glVertex3f( +H_WTOP*0.9F*0.5f             ,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f,  0.0f,      H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f -H_WTOP*0.9f, +W_WFWD,    H_WTOP*0.9f);
  glVertex3f( +H_WTOP*0.9F*0.5f             , +W_WFWD,    H_WTOP*0.9f);
  glEnd ();
  glRotatef (-alpha*180.f/float(PI),0.0f,0.0f,1.0f);
  glTranslatef (GLfloat(-CARLEN/3)+H_WTOP*0.9f*0.5f, -px+W_WFWD, 0.0f );

  // addition by JCer
  // push car's suspention
  { glRotatef (-float(atan(1./CARLEN)*180/3.1415926*lin_acc),float(sin(alpha)),float(cos(alpha)),0.0f);
    glRotatef ( float(atan(1./CARWID)*180/3.1415926*lat_acc),float(cos(alpha)),float(sin(alpha)),0.0f);
  };

  ////
  //// nose
  ////
  glBegin (GL_TRIANGLE_FAN);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glNormal3f( 0.0f, 0.0f, 1.0f );
  glVertex3f( GLfloat(+CARLEN/2)+1,  GLfloat(+0),      H_WMID+1);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+0       )       , GLfloat(-CARWID/5), H_WMID);
  glVertex3f( GLfloat(+0       )       , GLfloat(-CARWID/5), H_CAR1+.5);
  glVertex3f( GLfloat(+0       )       , GLfloat(+CARWID/5), H_CAR1+.5);
  glVertex3f( GLfloat(+0       )       , GLfloat(+CARWID/5), H_WMID);
  glVertex3f( GLfloat(+0       )       , GLfloat(-CARWID/5), H_WMID);
  glEnd ();

  ////
  //// wings
  ////
  // front
  glBegin (GL_QUADS);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+CARLEN/2), -W_WFWD+px, H_WMID);
  glVertex3f( GLfloat(+CARLEN/2), +W_WFWD-px, H_WMID);
  glVertex3f( GLfloat(+CARLEN/3), +W_WFWD-px, H_WMID+.5f);
  glVertex3f( GLfloat(+CARLEN/3), -W_WFWD+px, H_WMID+.5f);

  // rear
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3), +W_WRER-px, H_CAR2-.5f);
  glVertex3f( GLfloat(-CARLEN/3), -W_WRER+px, H_CAR2-.5f);
  glVertex3f( GLfloat(-CARLEN/2), -W_WRER+px, H_CAR2);
  glVertex3f( GLfloat(-CARLEN/2), +W_WRER-px, H_CAR2);

  // lateral rear wings
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/2), -W_WRER+px, H_CAR1);
  glVertex3f( GLfloat(-CARLEN/3), -W_WRER+px, H_CAR1);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3), -W_WRER+px, H_CAR2);
  glVertex3f( GLfloat(-CARLEN/2), -W_WRER+px, H_CAR2);

  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/2), +W_WRER-px, H_CAR1);
  glVertex3f( GLfloat(-CARLEN/3), +W_WRER-px, H_CAR1);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3), +W_WRER-px, H_CAR2);
  glVertex3f( GLfloat(-CARLEN/2), +W_WRER-px, H_CAR2);
  glEnd ();

  ////
  //// Radiators
  ////
  glBegin (GL_QUAD_STRIP);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(-CARWID/2), H_CAR1+.5f);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(-CARWID/2), H_WMID);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(-CARWID/2), H_CAR1+.5f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP+1, GLfloat(-CARWID/2+1), H_WMID);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP+1, GLfloat(+CARWID/2-1), H_WMID);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(+CARWID/2), H_CAR1+.5f);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(+CARWID/2), H_WMID);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(+CARWID/2), H_CAR1+.5f);
  glEnd();

  glBegin (GL_POLYGON);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP, GLfloat(+0), H_CAR1);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(-CARWID/2), H_CAR1+.5f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(-CARWID/2), H_CAR1);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(-CARWID/4), H_CAR1);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3)       , GLfloat(-CARWID/4), H_CAR1);

  glVertex3f( GLfloat(-CARLEN/3)       , GLfloat(+CARWID/4), H_CAR1);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(+CARWID/4), H_CAR1);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(+CARWID/2), H_CAR1);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(+CARWID/2), H_CAR1+.5f);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(-CARWID/2), H_WMID);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(-CARWID/2), H_WMID);
  glVertex3f( GLfloat(-CARLEN/3)+H_WTOP, GLfloat(+CARWID/2), H_WMID);
  glVertex3f( GLfloat(+CARLEN/3)-H_WTOP-2, GLfloat(+CARWID/2), H_WMID);
  glEnd();

  ////
  //// Air intake
  ////
  glBegin(GL_TRIANGLE_STRIP);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/2 ), GLfloat(-0    ), H_WMID);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+0        ), GLfloat(-CARWID/5 ), H_CAR1);
  glVertex3f( GLfloat(-CARLEN/2 ), GLfloat(-0    ), H_CAR1);

  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(+0        ), GLfloat(-CARWID/20), H_CAR2+H_WMID);
  glVertex3f( GLfloat(-CARLEN/10), GLfloat(+0    ), H_CAR2+H_WMID);
  glVertex3f( GLfloat(+0        ), GLfloat(+CARWID/20), H_CAR2+H_WMID);
  pColor = &Palette.m_Color[color2];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  
  glVertex3f( GLfloat(-CARLEN/2 ), GLfloat(+0    ), H_CAR1);
  glVertex3f( GLfloat(+0        ), GLfloat(+CARWID/5 ), H_CAR1);
  pColor = &Palette.m_Color[color1];
  glColor4f ( pColor->fr, pColor->fg, pColor->fb, 1.0f);
  glVertex3f( GLfloat(-CARLEN/2 ), GLfloat(+0    ), H_WMID);
  glEnd();

  glPopMatrix();
}

/**
 * Print the name of the driver above the car.
 *
 * @param car : the id of the car
 */
void TView3D::GL_DrawCarName( int i )
{
  // float NameLen=2.0f * (float)m_Zoom/0.01f * strlen(race_data.cars[i]->driver->getName());
  glPushMatrix ();  
  glTranslatef( 0.0f, 0.0f, 20.0f);
  glRasterPos2f(0.0f, 0.0f);
  opengl.glPrint( opengl.m_iSmallFont, race_data.cars[i]->driver->getName());  // print GL text to the screen
  glPopMatrix ();  
}

/**
 * Draw the trajectory of a car.
 *
 * @param car : the id of the car
 */
void TView3D::DrawTrajectory( int car )
{
  const GLfloat TRAJECTORY_HEIGHT = 1.0f;
  InstantReplay * ir = g_ViewManager->m_oInstantReplay;

  if( car==m_iFollowCar )
  {
    glColor4f( 1.0f, 1.0f, 1.0f, 1.0f );
  }
  else
  {
    glColor4f( 0.3f, 0.3f, 1.0f, 1.0f );
  }
  glBegin( GL_LINE_STRIP );

  // Follow the buffer in the reverse order
  for( int i=ir->m_iCurrentPos-1; i>=0; i-- )
  {
    glVertex3d( ir->m_aData[i].X[car], ir->m_aData[i].Y[car], ir->m_aData[i].Z[car]+TRAJECTORY_HEIGHT );
  }

  if( ir->m_iNumData==INSTANT_BUFFER_SIZE )
  {
    for( int i=INSTANT_BUFFER_SIZE-1; i>=ir->m_iCurrentPos; i-- )
    {
      glVertex3d( ir->m_aData[i].X[car], ir->m_aData[i].Y[car], ir->m_aData[i].Z[car]+TRAJECTORY_HEIGHT );
    }
  }
  glEnd();
}

/**
 * Load a 3DS file and render it in an openGL list.
 *
 * @param list_id  : opengl id of the list
 * @param filename : name of the 3ds file
 */
void TView3D::Load3DSWheel( TWheel3D * wheel, const char * filename )
{
  bool bNew;
  Model_3DS * model = model_loader.GetModel( bNew, filename, 0.1f, 90, true );
  if( bNew )
  {
    model->CalculateSphere();
    model->Recenter();
  }
  wheel->m_fCenterX = -model->m_fCenterY;
  wheel->m_fCenterY = model->m_fCenterX;
  wheel->m_fCenterZ = model->m_fCenterZ;
  wheel->m_fPerimeter = (float )((model->m_fMaxZ-model->m_fMinZ)*PI); // 2.0f/2.0f 
  wheel->m_pModel = model;
}

/**
 * Create all the OpenGL lists to draw the race
 */
void TView3D::GL_CreateList()
{
  // problem this procedure is executed each time the window is resized...
  glNewList( LIST_ROAD, GL_COMPILE );
  track3D.DrawRoad( true );
  glEndList();
  
  ModelCarBody = model_loader.GetModel( "3ds/car_body.3ds", 0.1f, 90, false );
  ModelWheelRear = model_loader.GetModel( "3ds/wheel_rear.3ds", 0.1f, 90, false );
  ModelWheelFront = model_loader.GetModel( "3ds/wheel_front.3ds", 0.1f, 90, false );
  ModelHelmet = model_loader.GetModel( "3ds/helmet.3ds", 0.1f, 90, false );
  ModelInstantReplay = model_loader.GetModel( "3ds/instant_replay.3ds", 0.1f, 90, false );

  // Load custom model of cars
  char s[256];
  for( int i=0; i<args.m_iNumCar; i++ )  // for each car:
  {       
    const char * sModel = race_data.cars[i]->driver->getModel3D();
    if( sModel==NULL )
    {
      m_aModelBody[i] = NULL;
      m_aFrontWheel[i].m_pModel = NULL;
      m_aBackWheel[i].m_pModel = NULL;
    }
    else
    {
      sprintf( s, "3ds/%s/car_body.3ds", sModel );
      m_aModelBody[i] = model_loader.GetModel( s, 0.1f, 90, true );;
      m_aModelBody[i]->SetEnvMap( true );
      
      sprintf( s, "3ds/%s/front_right_wheel.3ds", sModel );
      Load3DSWheel( &(m_aFrontWheel[i]), s );

      sprintf( s, "3ds/%s/back_right_wheel.3ds", sModel );
      Load3DSWheel( &(m_aBackWheel[i]), s );
    }
  }

  // Build the font
  opengl.BuildFont();
}

/**
 * Extract the fustrum from the OpenGL view.
 */
void TView3D::ExtractFrustum()
{
   float   proj[16];
   float   modl[16];
   float   clip[16];
   float   t;

   /* Get the current PROJECTION matrix from OpenGL */
   glGetFloatv( GL_PROJECTION_MATRIX, proj );

   /* Get the current MODELVIEW matrix from OpenGL */
   glGetFloatv( GL_MODELVIEW_MATRIX, modl );

   /* Combine the two matrices (multiply projection by modelview) */
   clip[ 0] = modl[ 0] * proj[ 0] + modl[ 1] * proj[ 4] + modl[ 2] * proj[ 8] + modl[ 3] * proj[12];
   clip[ 1] = modl[ 0] * proj[ 1] + modl[ 1] * proj[ 5] + modl[ 2] * proj[ 9] + modl[ 3] * proj[13];
   clip[ 2] = modl[ 0] * proj[ 2] + modl[ 1] * proj[ 6] + modl[ 2] * proj[10] + modl[ 3] * proj[14];
   clip[ 3] = modl[ 0] * proj[ 3] + modl[ 1] * proj[ 7] + modl[ 2] * proj[11] + modl[ 3] * proj[15];

   clip[ 4] = modl[ 4] * proj[ 0] + modl[ 5] * proj[ 4] + modl[ 6] * proj[ 8] + modl[ 7] * proj[12];
   clip[ 5] = modl[ 4] * proj[ 1] + modl[ 5] * proj[ 5] + modl[ 6] * proj[ 9] + modl[ 7] * proj[13];
   clip[ 6] = modl[ 4] * proj[ 2] + modl[ 5] * proj[ 6] + modl[ 6] * proj[10] + modl[ 7] * proj[14];
   clip[ 7] = modl[ 4] * proj[ 3] + modl[ 5] * proj[ 7] + modl[ 6] * proj[11] + modl[ 7] * proj[15];

   clip[ 8] = modl[ 8] * proj[ 0] + modl[ 9] * proj[ 4] + modl[10] * proj[ 8] + modl[11] * proj[12];
   clip[ 9] = modl[ 8] * proj[ 1] + modl[ 9] * proj[ 5] + modl[10] * proj[ 9] + modl[11] * proj[13];
   clip[10] = modl[ 8] * proj[ 2] + modl[ 9] * proj[ 6] + modl[10] * proj[10] + modl[11] * proj[14];
   clip[11] = modl[ 8] * proj[ 3] + modl[ 9] * proj[ 7] + modl[10] * proj[11] + modl[11] * proj[15];

   clip[12] = modl[12] * proj[ 0] + modl[13] * proj[ 4] + modl[14] * proj[ 8] + modl[15] * proj[12];
   clip[13] = modl[12] * proj[ 1] + modl[13] * proj[ 5] + modl[14] * proj[ 9] + modl[15] * proj[13];
   clip[14] = modl[12] * proj[ 2] + modl[13] * proj[ 6] + modl[14] * proj[10] + modl[15] * proj[14];
   clip[15] = modl[12] * proj[ 3] + modl[13] * proj[ 7] + modl[14] * proj[11] + modl[15] * proj[15];

   /* Extract the numbers for the RIGHT plane */
   m_aFrustum[0][0] = clip[ 3] - clip[ 0];
   m_aFrustum[0][1] = clip[ 7] - clip[ 4];
   m_aFrustum[0][2] = clip[11] - clip[ 8];
   m_aFrustum[0][3] = clip[15] - clip[12];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[0][0] * m_aFrustum[0][0] + m_aFrustum[0][1] * m_aFrustum[0][1] + m_aFrustum[0][2] * m_aFrustum[0][2] ) );
   m_aFrustum[0][0] /= t;
   m_aFrustum[0][1] /= t;
   m_aFrustum[0][2] /= t;
   m_aFrustum[0][3] /= t;

   /* Extract the numbers for the LEFT plane */
   m_aFrustum[1][0] = clip[ 3] + clip[ 0];
   m_aFrustum[1][1] = clip[ 7] + clip[ 4];
   m_aFrustum[1][2] = clip[11] + clip[ 8];
   m_aFrustum[1][3] = clip[15] + clip[12];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[1][0] * m_aFrustum[1][0] + m_aFrustum[1][1] * m_aFrustum[1][1] + m_aFrustum[1][2] * m_aFrustum[1][2] ) );
   m_aFrustum[1][0] /= t;
   m_aFrustum[1][1] /= t;
   m_aFrustum[1][2] /= t;
   m_aFrustum[1][3] /= t;

   /* Extract the BOTTOM plane */
   m_aFrustum[2][0] = clip[ 3] + clip[ 1];
   m_aFrustum[2][1] = clip[ 7] + clip[ 5];
   m_aFrustum[2][2] = clip[11] + clip[ 9];
   m_aFrustum[2][3] = clip[15] + clip[13];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[2][0] * m_aFrustum[2][0] + m_aFrustum[2][1] * m_aFrustum[2][1] + m_aFrustum[2][2] * m_aFrustum[2][2] ) );
   m_aFrustum[2][0] /= t;
   m_aFrustum[2][1] /= t;
   m_aFrustum[2][2] /= t;
   m_aFrustum[2][3] /= t;

   /* Extract the TOP plane */
   m_aFrustum[3][0] = clip[ 3] - clip[ 1];
   m_aFrustum[3][1] = clip[ 7] - clip[ 5];
   m_aFrustum[3][2] = clip[11] - clip[ 9];
   m_aFrustum[3][3] = clip[15] - clip[13];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[3][0] * m_aFrustum[3][0] + m_aFrustum[3][1] * m_aFrustum[3][1] + m_aFrustum[3][2] * m_aFrustum[3][2] ) );
   m_aFrustum[3][0] /= t;
   m_aFrustum[3][1] /= t;
   m_aFrustum[3][2] /= t;
   m_aFrustum[3][3] /= t;

   /* Extract the FAR plane */
   m_aFrustum[4][0] = clip[ 3] - clip[ 2];
   m_aFrustum[4][1] = clip[ 7] - clip[ 6];
   m_aFrustum[4][2] = clip[11] - clip[10];
   m_aFrustum[4][3] = clip[15] - clip[14];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[4][0] * m_aFrustum[4][0] + m_aFrustum[4][1] * m_aFrustum[4][1] + m_aFrustum[4][2] * m_aFrustum[4][2] ) );
   m_aFrustum[4][0] /= t;
   m_aFrustum[4][1] /= t;
   m_aFrustum[4][2] /= t;
   m_aFrustum[4][3] /= t;

   /* Extract the NEAR plane */
   m_aFrustum[5][0] = clip[ 3] + clip[ 2];
   m_aFrustum[5][1] = clip[ 7] + clip[ 6];
   m_aFrustum[5][2] = clip[11] + clip[10];
   m_aFrustum[5][3] = clip[15] + clip[14];

   /* Normalize the result */
   t = float( sqrt( m_aFrustum[5][0] * m_aFrustum[5][0] + m_aFrustum[5][1] * m_aFrustum[5][1] + m_aFrustum[5][2] * m_aFrustum[5][2] ) );
   m_aFrustum[5][0] /= t;
   m_aFrustum[5][1] /= t;
   m_aFrustum[5][2] /= t;
   m_aFrustum[5][3] /= t;
}

/**
 * SphereInFrustum
 *
 * Return 0 is the sphere is not in the cone view
 * else return the distance from the camera.
 */
float TView3D::SphereInFrustum( float x, float y, float z, float radius )
{
   int p;
   float d;

   for( p = 0; p < 6; p++ )
   {
      d = m_aFrustum[p][0] * x + m_aFrustum[p][1] * y + m_aFrustum[p][2] * z + m_aFrustum[p][3];
      if( d <= -radius )
         return 0;
   }
   return d + radius;
}

/**
 * Move with the keys 
 */
void TView3D::MoveWithKeys()
{
  const double SPEED_KEY = 5.0;
  const double SPEED_ANGLE = 2.0;
  int c = g_ViewManager->m_iFollowNobodyKey;
  double ang = -m_fAngleZ/180.0*PI;
  if( c==ARROW_UP )
  {
    m_SpeedX -= SPEED_KEY*sin(ang);
    m_SpeedY += SPEED_KEY*cos(ang);
    // m_SpeedY += SPEED_KEY;
  }
  else if( c==ARROW_DOWN )
  {
    m_SpeedX += SPEED_KEY*sin(ang);
    m_SpeedY -= SPEED_KEY*cos(ang);
    // m_SpeedY -= SPEED_KEY;
  }
  else if( c==ARROW_LEFT )
  {
    m_SpeedX -= SPEED_KEY*cos(ang);
    m_SpeedY -= SPEED_KEY*sin(ang);
    // m_SpeedX -= SPEED_KEY;
  }
  else if( c==ARROW_RIGHT )
  {
    m_SpeedX += SPEED_KEY*cos(ang);
    m_SpeedY += SPEED_KEY*sin(ang);
    // m_SpeedX += SPEED_KEY;
  }
  else if( c==ROTATE_UP )
  {
    m_fSpeedAngleX += SPEED_ANGLE;
  }
  else if( c==ROTATE_DOWN )
  {
    m_fSpeedAngleX -= SPEED_ANGLE;
  }
  else if( c==ROTATE_LEFT )
  {
    m_fSpeedAngleZ -= SPEED_ANGLE;
  }
  else if( c==ROTATE_RIGHT )
  {
    m_fSpeedAngleZ += SPEED_ANGLE;
  }
  else if( c==ROTATE_RESET )
  {
    ResetRotation();
    FullScreenScale();
  }

  m_CenterX += m_SpeedX;
  m_CenterY += m_SpeedY;
  m_fAngleX  += m_fSpeedAngleX;
  m_fAngleZ  += m_fSpeedAngleZ;

  m_SpeedX *= 0.9; 
  m_SpeedY *= 0.9; 
  m_fSpeedAngleX *= 0.9; 
  m_fSpeedAngleZ *= 0.9; 
  g_ViewManager->m_iFollowNobodyKey = 0; // clear key
}


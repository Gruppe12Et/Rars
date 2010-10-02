
//--------------------------------------------------------------------------
//
//    FILE: G_VIEW2D.CPP (portable)
//
//    TView2D
//      - This is the view with a zoom. The grass uses a pattern.
//        The cars uses their textures.
//
//    Version       Author          Date
//      0.2      Jussi Pajala    03 /20 /00 Corrected car rotation bug
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                           D E F I N E S
//--------------------------------------------------------------------------

#define X_SCALE(a) ( (int)( ((a)-m_TopX)*m_ScaleX ))
#define Y_SCALE(a) ( (int)( ((a)-m_TopY)*m_ScaleY ))

void Bulle2DrawPath( Driver * driver, TView * vcl, double m_TopX, double m_TopY, double m_ScaleX, double m_ScaleY );


//__ TView2D::TView2D ______________________________________________________
//
// Constructor
// In : int x, y : size of the view
//__________________________________________________________________________

TView2D::TView2D( int x, int y, int d_m_FollowCar ) : TView( x, y )
{
  // init the variables
  m_ScaleX = 0.5;
  m_ScaleY= - m_ScaleX;
  m_CenterX = m_SizeX/(2*m_ScaleX);
  m_CenterY = m_SizeY/(2*m_ScaleY);
  m_FollowCarTime = 0;
  m_dFollowCar = d_m_FollowCar;
  m_Zoom = 0.02;
  m_OptionShowBoard = 0;

  // load the bitmap of the grass
  m_BitmapGrass.PCX_Read( "bitmap/grass8x8.pcx" );
}

//__ TView2D::~TView2D _____________________________________________________
//
// Destructor
//__________________________________________________________________________

TView2D::~TView2D()
{
}

//__ TView2D::Refresh ______________________________________________________
//
// Refresh the bitmap
//__________________________________________________________________________

void TView2D::Refresh()
{
    // the user is changing 'm_FollowCar' ?
  if( g_ViewManager->m_iFollowMode==FOLLOW_NOBODY )
  {
    const double SPEED_KEY = 5.0;
    int c = g_ViewManager->m_iFollowNobodyKey;
    if( c==ARROW_UP )
    {
      m_SpeedY += SPEED_KEY;
    }
    else if( c==ARROW_DOWN )
    {
      m_SpeedY -= SPEED_KEY;
    }
    else if( c==ARROW_LEFT )
    {
      m_SpeedX -= SPEED_KEY;
    }
    else if( c==ARROW_RIGHT )
    {
      m_SpeedX += SPEED_KEY;
    }
    m_CenterX += m_SpeedX;
    m_CenterY += m_SpeedY;

    m_SpeedX *= 0.9; 
    m_SpeedY *= 0.9;
    g_ViewManager->m_iFollowNobodyKey = 0; // clear key
  }
  else
  {
    // the user is changing 'm_FollowCar' ?
    if( m_FollowCarTime == 0 ) 
    {
      m_CenterX = m_CenterX*0.3 + race_data.cars[m_iFollowCar]->x*0.7;
      m_CenterY = m_CenterY*0.3 + race_data.cars[m_iFollowCar]->y*0.7;
    }
    else 
    {
      double aux = (1.0*(40-m_FollowCarTime)+0.95*m_FollowCarTime)/40.0;
      m_ScaleX = m_ScaleX * 0.98;
      aux = (0.3*(40-m_FollowCarTime)+0.95*m_FollowCarTime)/40.0;
      m_CenterX = m_CenterX*aux + race_data.cars[m_iFollowCar]->x*(1.0-aux);
      m_CenterY = m_CenterY*aux + race_data.cars[m_iFollowCar]->y*(1.0-aux);
      m_FollowCarTime--;
    }
  }

  // zoom
  m_Zoom /= g_ViewManager->m_ZoomDelta;
  m_Zoom = min( max( m_Zoom, 0.001 ), 0.04 );
  m_ScaleX = m_ScaleX*0.98+m_Zoom;
  m_ScaleY= - m_ScaleX;
  m_TopX = m_CenterX - m_SizeX/(2*m_ScaleX);
  m_TopY = m_CenterY - m_SizeY/(2*m_ScaleY);

  // refresh the bitmap
  ClearBitmapTexture( m_TopX*m_ScaleX, m_TopY*m_ScaleY, 1/(m_ScaleX*40.0), &m_BitmapGrass );
  DrawRoad();
  DrawCars();
  if( g_ViewManager->m_ImageCpt!=1 && m_OptionShowBoard )
  {
    DrawBoard();
  }
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

  // Show a R in the screen of the InstantReplay is activated
  if( g_ViewManager->m_oInstantReplay->m_iMode!=INSTANT_RECORD )
  {
    DrawString( "R", m_SizeX-15, 10 );
  }


  // Bulle2DrawPath( drivers[0], this, m_TopX, m_TopY, m_ScaleX, m_ScaleY );
}

//__ TView2D::FollowCar ____________________________________________________
//
// Change the car to follow
// In : car_nr : the new_car to follow in the view
//__________________________________________________________________________

void TView2D::FollowCar( int car_nr )
{
   if( m_iFollowCar<0 || m_iFollowCar>=args.m_iNumCar || m_iFollowCar==car_nr )
      return;
   m_iFollowCar = car_nr + m_dFollowCar;
   m_FollowCarTime = 40;
}

//__ TView2D::DrawRoad ______________________________________________________
//
// Draw the road
//__________________________________________________________________________

void TView2D::DrawRoad()
{
  Int2D v[4];
  int i, j;

  segment *lftwall = currentTrack->get_track_description().lftwall;
  segment *rgtwall = currentTrack->get_track_description().rgtwall;

  for(i=currentTrack->m_iNumSegment-1; i>=0; i--) 
  {                 
    // for each segment:
    if( lftwall[i].radius==0.0 ) 
    {
      // straigth
      v[0].x=X_SCALE( lftwall[i].end_x );
      v[0].y=Y_SCALE( lftwall[i].end_y );
      v[1].x=X_SCALE( rgtwall[i].end_x );
      v[1].y=Y_SCALE( rgtwall[i].end_y );
      v[2].x=X_SCALE( rgtwall[i].beg_x );
      v[2].y=Y_SCALE( rgtwall[i].beg_y );
      v[3].x=X_SCALE( lftwall[i].beg_x );
      v[3].y=Y_SCALE( lftwall[i].beg_y );
      DrawPoly( v, 4, ROAD_COLOR );
      DrawBorder(v);
    }
    else
    {
      // curve
      double step_size = 10.0/max( fabs(lftwall[i].radius), fabs(rgtwall[i].radius) );
      double fstep = lftwall[i].length/step_size;
      int nb_step = (int)fstep;
      step_size = lftwall[i].length/nb_step;

      v[2].x=X_SCALE( rgtwall[i].beg_x );
      v[2].y=Y_SCALE( rgtwall[i].beg_y );
      v[3].x=X_SCALE( lftwall[i].beg_x );
      v[3].y=Y_SCALE( lftwall[i].beg_y );

      double ang = lftwall[i].beg_ang;
      double cenx = lftwall[i].cen_x;
      double ceny = lftwall[i].cen_y;
      if(lftwall[i].radius>0.0) 
      {
        for( j=0; j<nb_step; j++) 
        {
          ang+=step_size;
          if(ang > 2.0 * PI) ang -= 2.0 * PI;
          v[0].x= X_SCALE( cenx+lftwall[i].radius*sin(ang) );
          v[0].y= Y_SCALE( ceny-lftwall[i].radius*cos(ang) );
          v[1].x= X_SCALE( cenx+rgtwall[i].radius*sin(ang) );
          v[1].y= Y_SCALE( ceny-rgtwall[i].radius*cos(ang) );
          DrawPoly( v, 4, ROAD_COLOR );
          DrawBorder(v);
          v[2]=v[1];
          v[3]=v[0];
        }
      }
      else 
      {
        for( j=0; j<nb_step; j++) 
        {
          ang-=step_size;
          if(ang < 0.0) ang += 2.0 * PI;
          v[0].x= X_SCALE( cenx+lftwall[i].radius*sin(ang) );
          v[0].y= Y_SCALE( ceny-lftwall[i].radius*cos(ang) );
          v[1].x= X_SCALE( cenx+rgtwall[i].radius*sin(ang) );
          v[1].y= Y_SCALE( ceny-rgtwall[i].radius*cos(ang) );
          DrawPoly( v, 4, ROAD_COLOR );
          DrawBorder(v);
          v[2]=v[1];
          v[3]=v[0];
        }
      }
      v[0].x=X_SCALE( lftwall[i].end_x );
      v[0].y=Y_SCALE( lftwall[i].end_y );
      v[1].x=X_SCALE( rgtwall[i].end_x );
      v[1].y=Y_SCALE( rgtwall[i].end_y );
      DrawPoly( v, 4, ROAD_COLOR );
      DrawBorder(v);
    }
  }
  // starting line
  v[0].x = X_SCALE( currentTrack->finish_rx ); v[0].y = Y_SCALE( currentTrack->finish_ry );
  v[1].x = X_SCALE( currentTrack->finish_lx ); v[1].y = Y_SCALE( currentTrack->finish_ly );
  double dir_x = -(currentTrack->finish_ry - currentTrack->finish_ly)/4;
  double dir_y = (currentTrack->finish_rx - currentTrack->finish_lx)/4;
  v[2].x = X_SCALE( currentTrack->finish_lx + dir_x);v[2].y = Y_SCALE( currentTrack->finish_ly + dir_y);
  v[3].x = X_SCALE( currentTrack->finish_rx + dir_x);v[3].y = Y_SCALE( currentTrack->finish_ry + dir_y);
  DrawPoly( v, 4, START_COLOR );
}


//__ TView2D::DrawCars _____________________________________________________
//
// Draw all the cars
//__________________________________________________________________________

void TView2D::DrawCars()
{
  double x, y;      // coordinates of center of car
  double ang;       // orientation angle of car, wrt x-axis, radians
  Int2D v[4];

  double sine, cosine, dx, dy;
  int i;

  double S_CARLEN = CARLEN*m_ScaleX;
  double S_CARWID = CARWID*m_ScaleX;

  for( i=0; i<args.m_iNumCar; i++ )           // for each car:
  {
    x = (race_data.cars[i]->x-m_TopX)*m_ScaleX;
    y = (race_data.cars[i]->y-m_TopY)*m_ScaleY;
    /* CHANGED 0.2: steering angle used to be shown in previous versions, like this:*/
    ang = race_data.cars[i]->ang + race_data.cars[i]->alpha;  /* added alpha here! */
    sine = sin(ang);    cosine = cos(ang);
    /* CHANGED 0.2: these two lines were added */
    double xx = sine*S_CARWID;  
    double yy = cosine*S_CARWID;
    x += cosine * S_CARLEN/2 + sine * S_CARWID/2;    // left front corner coords
    y += cosine * S_CARWID/2 - sine * S_CARLEN/2;
    dx = -cosine*S_CARLEN;
    dy = +sine*S_CARLEN;
    /* CHANGED 0.2:next two lines */
    v[0].x = (int)(x - xx);       v[0].y =(int)(  y - yy );
    v[3].x = (int)(x + dx - xx);  v[3].y =(int)( y+dy - yy );
    x+= sine*S_CARWID;
    y+= cosine*S_CARWID;
    /* CHANGED 0.2: again two lines below */
    v[1].x = (int)(x - xx);       v[1].y = (int)( y - yy);
    v[2].x = (int)( x + dx - xx); v[2].y = (int)( y+dy - yy);
    if( race_data.cars[i]->collision_draw )
      DrawPoly( v, 4, FLASH_COLOR );
    else
      DrawPolyTexture( v, 4, -cosine/m_ScaleX, sine/m_ScaleX, &(g_ViewManager->m_aBitmapCar[i]) );
  }

  // print the name of the driver above the car, after all cars were drawn
  if ( g_ViewManager->m_bShowNames )
  {
    for( i=0; i<args.m_iNumCar; i++ )           // for each car:
    {
      // print only if the name will be visible on screen
      y = (race_data.cars[i]->y-m_TopY)*m_ScaleY - 20.0;
      if ( y>0 && y+10.0<m_SizeY )
      {
        double NameLen = 4.0*strlen(drivers[i]->getName());
        x = (race_data.cars[i]->x-m_TopX)*m_ScaleX;
        if ( x-NameLen>0 && x+NameLen<m_SizeX )
        {
          int color;      // color of driver's name
          char s[10];

          if( race_data.cars[i]->collision_draw )
            color = FLASH_COLOR;
          else
            color = drivers[i]->getTailColor();

          sprintf( s, "%s", drivers[i]->getName() );
          DrawString(s, (int)(x-NameLen), (int)y, color);
        }
      }
    }
  }
}

//__ TView3D::DrawTrajectory  ______________________________________________
//
// Draw the trajectory of the current car.
//__________________________________________________________________________

void TView2D::DrawTrajectory( int car )
{
  InstantReplay * ir = g_ViewManager->m_oInstantReplay;
  int color = car==m_iFollowCar ? COLOR_WHITE:COLOR_BLUE;

  if( ir->m_iNumData>1 )
  {
    int start = ir->m_iCurrentPos-1;
    int x_last = X_SCALE(ir->m_aData[start].x[car]);
    int y_last = Y_SCALE(ir->m_aData[start].y[car]);
    int x, y;

    for( int i=ir->m_iCurrentPos-2; i>=0; i-- )
    {
      x = X_SCALE(ir->m_aData[i].x[car]);
      y = Y_SCALE(ir->m_aData[i].y[car]);

      DrawLine( x_last, y_last, x, y, color );

      x_last = x;
      y_last = y;
    }

    if( ir->m_iNumData==INSTANT_BUFFER_SIZE )
    {
      for( int i=INSTANT_BUFFER_SIZE-1; i>=ir->m_iCurrentPos; i-- )
      {
        x = X_SCALE(ir->m_aData[i].x[car]);
        y = Y_SCALE(ir->m_aData[i].y[car]);

        DrawLine( x_last, y_last, x, y, color );

        x_last = x;
        y_last = y;
      }
    }
  }
}

//__ TView2D::DrawBoard ____________________________________________________
//
// Draw the board (if option_show_board==1)
//__________________________________________________________________________

void TView2D::DrawBoard()
{
  char s[64];
  int y;

  sprintf( s, "%ld", g_ViewManager->m_ImageCpt );
  DrawString( s, m_SizeX-48, 1 );

  y = m_SizeY-40;

  DrawString( drivers[m_iFollowCar]->getName(), 1, y );
  y += 8;

  sprintf( s, "position %d", race_data.m_aPosOfCar[m_iFollowCar]+1 );
  DrawString( s, 3, y );
  y += 8;

  sprintf( s, "lap %ld/%ld", race_data.cars[m_iFollowCar]->laps, args.m_iNumLap );
  DrawString( s, 3, y );
  y += 8;

  sprintf( s, "damage %ld", race_data.cars[m_iFollowCar]->damage );
  DrawString( s, 3, y );
  y += 8;

  sprintf( s, "speed %6.2f", race_data.cars[m_iFollowCar]->speed_avg*MPH_FPS );
  DrawStringOpaque( s, 3, y );
  y += 8;
}

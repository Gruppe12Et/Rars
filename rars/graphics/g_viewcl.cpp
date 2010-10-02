
//--------------------------------------------------------------------------
//
//    FILE: G_VIEWCL.CPP (portable)
//
//    TViewClassical
//      - Clasical view of rars (except the boards that are not complete)
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
//                           D E F I N E S
//--------------------------------------------------------------------------

#define ERASE_CAR 0
#define DRAW_CAR  1
#define X_SCALE(a) ( (int)( ((a)-m_TopX)*m_ScaleX ))
#define Y_SCALE(a) ( (int)( ((a)-m_TopY)*m_ScaleY ))

//__ TViewClassical::TViewClassical ________________________________________
//
// Constructor
// In : int x, y : size of the view
//__________________________________________________________________________

TViewClassical::TViewClassical( int x, int y, bool bVirtual ) : TView( x, y, bVirtual )
{
   FullScreenScale();
   m_OptionFlash = 1;
   m_OptionShowBoard = 0;

   m_BitmapGrass.PCX_Read( "bitmap/grass8x8.pcx" );
   ClearBitmapTexture( m_TopX*m_ScaleX, m_TopY*m_ScaleY, 1/(m_ScaleX*40.0), &m_BitmapGrass );

   DrawRoad();
}

//__ TViewClassical::~TViewClassical _______________________________________
//
// Destructor
//__________________________________________________________________________

TViewClassical::~TViewClassical()
{
}

//__ TViewBoard::Resize ___________________________________________________
//
// Resize the bitmap and the related parameters
// In : int x, y : the new size of the graphic (view)
//__________________________________________________________________________

void TViewClassical::Resize( int x, int y )
{
   TLowGraphic::Resize( x, y );
   FullScreenScale();
   ClearBitmapTexture( m_TopX*m_ScaleX, m_TopY*m_ScaleY, 1/(m_ScaleX*40.0), &m_BitmapGrass );
   DrawRoad();
}

//__ TViewClassical::Refresh _______________________________________________
//
// Refresh the view
//__________________________________________________________________________

void TViewClassical::Refresh()
{
  int i;

  DrawStart();
  if( m_OptionShowBoard )
     DrawBoard();
  for( i=0; i<args.m_iNumCar; i++ ) {
     DrawCars( ERASE_CAR, i );
     DrawCars( DRAW_CAR, i );
   }
}

//__ TViewClassical::DrawRoad ______________________________________________
//
// Draw the road
//__________________________________________________________________________

void TViewClassical::DrawRoad()
{
   Int2D v[4];
   int i, j;

   segment *lftwall = currentTrack->get_track_description().lftwall;
   segment *rgtwall = currentTrack->get_track_description().rgtwall;

   for(i=0; i<currentTrack->m_iNumSegment; i++) 
   {                 // for each segment:
      if( lftwall[i].radius==0.0 ) 
      {
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
	     int step = (int)( lftwall[i].length*5.0 );
	     v[2].x=X_SCALE( rgtwall[i].beg_x );
	     v[2].y=Y_SCALE( rgtwall[i].beg_y );
	     v[3].x=X_SCALE( lftwall[i].beg_x );
	     v[3].y=Y_SCALE( lftwall[i].beg_y );

	     double ang = lftwall[i].beg_ang;
	     double cenx = lftwall[i].cen_x;
	     double ceny = lftwall[i].cen_y;
         if(lftwall[i].radius>0.0) 
         {
            for( j=0; j<step; j++) 
            {
	           ang+=0.2;
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
            for( j=0; j<step; j++) 
            {
	           ang-=0.2;
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

   // copy the position of the starting line
   m_StartPoly[0] = v[0];
   m_StartPoly[1] = v[1];
   m_StartPoly[2] = v[2];
   m_StartPoly[3] = v[3];
}
//__ TViewClassical::DrawStart _____________________________________________
//
// Draw the starting line
//__________________________________________________________________________

void TViewClassical::DrawStart()
{
   DrawPoly( m_StartPoly, 4, START_COLOR );
}

//__ TViewClassical::DrawCars ______________________________________________
//
// Draw one car
// In : int state: DRAW_CAR or ERASE_CAR
//      int car_nr : the number of the car
//__________________________________________________________________________

void TViewClassical::DrawCars( int state, int car_nr )
{
   double x, y;           // coordinates of center of car
   double ang;            // orientation angle of car, wrt x-axis, radians
   Int2D v[4];

   double xx, yy;
   double sine, cosine, dx, dy;

   double S_CARLEN = CARLEN*m_ScaleX;
   double S_CARWID = CARWID*m_ScaleX;

   int i = car_nr;
   if( state==DRAW_CAR ) {
      x = race_data.cars[i]->prex2 = race_data.cars[i]->x;
      y = race_data.cars[i]->prey2 = race_data.cars[i]->y;
      ang = race_data.cars[i]->prang2 = race_data.cars[i]->ang;
   } else {
      x = race_data.cars[i]->prex2;
      y = race_data.cars[i]->prey2;
      ang = race_data.cars[i]->prang2;
   }
   x = (x-m_TopX)*m_ScaleX;
   y = (y-m_TopY)*m_ScaleY;
   sine = sin(ang);    cosine = cos(ang);
   x += cosine * S_CARLEN/2 + sine * S_CARWID/2;    // left front corner coords
   y += cosine * S_CARWID/2 - sine * S_CARLEN/2;
   dx = -cosine*S_CARLEN;
   dy = +sine*S_CARLEN;
   v[0].x = (int) x; v[0].y =(int)  y;
   v[3].x = (int)( x+dx ); v[3].y =(int)( y+dy );
   xx = x + sine*S_CARWID;
   yy = y + cosine*S_CARWID;
   v[1].x = (int) xx; v[1].y = (int) yy;
   v[2].x = (int)( xx+dx ); v[2].y =(int)( yy+dy );
   if( state == DRAW_CAR ) {
      if( race_data.cars[i]->collision_draw )
         DrawPoly( v, 4, FLASH_COLOR );
      else {
         if( m_OptionFlash && i==m_iFollowCar )
            DrawPoly( v, 4, g_ViewManager->m_ImageCpt&0x0f );
         else {
            DrawPoly( v, 4, race_data.cars[i]->nose_color );
            dx *=0.3; dy*=0.3;
            v[2].x = (int)( xx+dx ); v[2].y =(int)( yy+dy );
            v[3].x = (int)( x+dx ); v[3].y =(int)( y+dy );
            DrawPoly( v, 4, race_data.cars[i]->tail_color );
         }
      }
   } else {
      DrawPoly( v, 4, ROAD_COLOR );
      dx *=0.3; dy*=0.3;
      v[2].x = (int)( xx+dx ); v[2].y =(int)( yy+dy );
      v[3].x = (int)( x+dx ); v[3].y =(int)( y+dy );
      DrawPoly( v, 4, ROAD_COLOR );
//            DrawLine( v[0], v[1], ROAD_COLOR );
   }
}

//__ TViewClassical::DrawBoard _____________________________________________
//
// Draw the board (if option_show_board==1)
//__________________________________________________________________________

void TViewClassical::DrawBoard()
{
   char s[64];
   int y;

   sprintf( s, "%ld", (long)g_ViewManager->m_ImageCpt );
   DrawStringOpaque( s, m_SizeX-48, 1 );

   y = m_SizeY-40;

   sprintf( s, "%s        ", drivers[m_iFollowCar]->getName() );
   s[8]='\0';
   DrawStringOpaque( s, 1, y );
   y += 8;

   sprintf( s, "position %d ", race_data.m_aPosOfCar[m_iFollowCar]+1 );
   DrawStringOpaque( s, 3, y );
   y += 8;

   sprintf( s, "lap %d/%d ", (int)race_data.cars[m_iFollowCar]->laps, (int)args.m_iNumLap );
   DrawStringOpaque( s, 3, y );
   y += 8;

   sprintf( s, "damage %ld    ", race_data.cars[m_iFollowCar]->damage );
   s[12]='\0';
   DrawStringOpaque( s, 3, y );
   y += 8;

   sprintf( s, "speed %6.2f", race_data.cars[m_iFollowCar]->speed_avg*MPH_FPS );
   DrawStringOpaque( s, 3, y );
   y += 8;
}

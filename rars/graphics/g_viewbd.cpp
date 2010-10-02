
/**
 * FILE: G_VIEWBD.CPP (portable)
 *
 * TViewBoard
 * - This is the view with only a board
 * 
 * History
 *  ver. 0.1  Aug 96 - Creation
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76 
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "g_global.h"

#define SPD(x,y) sqrt((x)*(x)+(y)*(y))
#define BOARD_Y 20
//--------------------------------------------------------------------------
//                            Class TViewBoard
//--------------------------------------------------------------------------


/**
 * Constructor
 *
 * @param x             size of the view
 * @param y             size of the view
 */
TViewBoard::TViewBoard( int x, int y ) : TView( x, y )
{
  m_iShowCarsBehind = 3;
  ScreenInit();
}

/**
 * Destructor
 */
TViewBoard::~TViewBoard()
{
}

/**
 * Init the screen
 */
void TViewBoard::ScreenInit()
{
   char s[64];//, track_name[16], * psz;

   iMaxCarsShown = (int)((m_SizeY-BOARD_Y-16)/8);  // 8 pixel for each line, 2 free lines on bottom

   if(m_SizeX > 960)
     m_bShowMoreInfos = true;
   else
     m_bShowMoreInfos = false;
   ClearBitmap( COLOR_LT_GRAY );

   if(m_bShowMoreInfos)
   {
     DrawStringOpaque(
       "              time     cur     time to      Laps Laps  cur   speed   best   last pit pit   pit    cur  cur  late proj. CPU", 
       1, 1, 
       oWHITE, oLIGHTGRAY );
     DrawStringOpaque(
       " Drivers     to leader pos   next   prev   d+tog Lead speed   avg.    Lap    Lap num lap  time damage fuel  m/gl laps  time", 
       1, 10, 
       oWHITE, oLIGHTGRAY );
   }
   else
   {
     DrawStringOpaque(
       "              time     cur     time to      Laps   cur    best   last pit  cur     cur     CPU", 
       1, 1, 
       oWHITE, oLIGHTGRAY );
     DrawStringOpaque(
       " Drivers     to leader pos   next   prev   d+tog  speed   Lap    Lap  num damage   fuel    time", 
       1, 10, 
       oWHITE, oLIGHTGRAY );
   }

   /*
   psz = strchr(currentTrack->m_sFileName,'/');
   if( psz==NULL ) 
   {
     psz=currentTrack->m_sFileName;
   }
   else 
   { 
     psz++;
   }
   for(i=0; i<8; i++) 
   {
     if(psz[i] == '.' || psz[i] == 0) 
     {
       break;
     }
     else 
     {
       track_name[i] = psz[i];
     }
   }
   track_name[i] = 0;
   */
   sprintf( s, 
     "Track       Length      Race Fastest Lap          BestView mode");
   DrawStringOpaque(s, 1, m_SizeY-16, oWHITE );
   sprintf( s, "%-8s   %7.2f", currentTrack->m_sShortName , currentTrack->get_track_description().length/5280.0 );
   DrawStringOpaque(s, 1, m_SizeY-8 );
}

/**
 * Resize the bitmap and the related parameters
 * 

 * @param x             the new size of the graphic 
 * @param y             the new size of the graphic (view)
 */
void TViewBoard::Resize( int x, int y )
{
  TLowGraphic::Resize( x, y );
  ScreenInit();
}

/**
 * Refresh the view
 */
void TViewBoard::Refresh()
{
  int x, y, i, pos;
  char s[256];
  char tmp[200];
  Int2D v[4];
  double fBestLapSpeed = 0.0;
  int iBestLapCar = 0,
      iFirstCarShown,           // first car shown on the board
      iLastCarShown;            // last car shown on the board

  // leader board
  x = 1; y = BOARD_Y;

  // set iFirstCarShown to see the follow car and min. (iShowCarsBehind) cars behind the car on ViewBoard
  iFirstCarShown = race_data.m_aPosOfCar[m_iFollowCar]-(iMaxCarsShown-m_iShowCarsBehind-1);
  // iFirstCarShown stays same if viewing last (iShowCarsBehind+1) cars, to avoid empty lines when enough cars available
  if(iFirstCarShown > args.m_iNumCar-iMaxCarsShown)
    iFirstCarShown = args.m_iNumCar-iMaxCarsShown;
  // iFirstCarShown should not be less then 0
  if(iFirstCarShown < 0)
    iFirstCarShown = 0;

  iLastCarShown = min(iFirstCarShown+iMaxCarsShown-1,args.m_iNumCar-1);
  for( i=iFirstCarShown ; i<=iLastCarShown; i++ )
  {
    pos = race_data.m_aCarInPos[i];

    int color;
    if( race_data.cars[pos]->out == 1 ) 
      color = oMAGENTA;
    else if( race_data.cars[pos]->out == 2 ) 
      color = oGREEN;
    else if( race_data.cars[pos]->offroad ) 
      color = oWHITE;
    else if( pos == m_iFollowCar )
      color = oYELLOW;
    else if (race_data.cars[pos]->collision_draw) 
      color = oRED;
    else
      color = oBLACK;


    if( pos==m_iFollowCar )
      strcpy(s, ">");
    else 
      strcpy(s, " ");
    
    sprintf(tmp, "  %-9s", drivers[pos]->getName() );
    strcat( s, tmp );

    // Time
    if (race_data.cars[pos]->out == 1) {
      sprintf( tmp, "   out    ");
    } else if (race_data.cars[pos]->out == 2) {
      sprintf( tmp, "   pit    ");
    } else if (i == 0) {
      format_time(race_data.cars[pos]->last_crossing, tmp);
    } else if (race_data.cars[pos]->laps < (race_data.cars[race_data.m_aCarInPos[0]]->laps - 1)) {
      sprintf(tmp, "%2ld lap(s) ", race_data.cars[race_data.m_aCarInPos[0]]->laps - 1 - race_data.cars[pos]->laps);
    } else if (race_data.cars[pos]->laps == race_data.cars[race_data.m_aCarInPos[0]]->laps) {
      format_time(race_data.cars[pos]->last_crossing - race_data.cars[race_data.m_aCarInPos[0]]->last_crossing, tmp);
    } else {
      format_time(race_data.cars[pos]->last_crossing - 
                  race_data.cars[race_data.m_aCarInPos[0]]->last_crossing +
                  race_data.cars[race_data.m_aCarInPos[0]]->lap_time, tmp);
    }

    strcat(s, tmp);

    if(m_bShowMoreInfos)
    {
      sprintf( tmp, "%4d%7.2f%7.2f%4ld+%03ld%4ld%7.2f%7.2f%7.2f%7.2f%3d%4ld%7.2f%6lu%7.2f%5.2f%4ld%7.2f"
        , i+1
        , race_data.cars[pos]->Behind_next
        , race_data.cars[pos]->Ahead_next
        , race_data.cars[pos]->Laps
        , args.m_iNumLap-race_data.cars[pos]->Laps
        , race_data.cars[pos]->Laps_lead
        , SPD(race_data.cars[pos]->xdot, race_data.cars[pos]->ydot) * MPH_FPS
        , race_data.cars[pos]->speed_avg * MPH_FPS
        , race_data.cars[pos]->bestlap_speed * MPH_FPS
        , race_data.cars[pos]->lastlap_speed * MPH_FPS
        , race_data.cars[pos]->Pit_stops
        , race_data.cars[pos]->Last_pit_visit
        , race_data.cars[pos]->Total_pit_time
        , race_data.cars[pos]->damage
        , race_data.cars[pos]->fuel
        , race_data.cars[pos]->fuel_mileage
        , race_data.cars[pos]->projected_laps
        , race_data.cars[pos]->RobotTime / race_data.m_fElapsedTime
      );
    }
    else
    {
      sprintf( tmp, "%4d%7.2f%7.2f%4ld+%03ld%7.2f%7.2f%7.2f%3d%8lu%8.2f%7.2f"
        , i+1
        , race_data.cars[pos]->Behind_next
        , race_data.cars[pos]->Ahead_next
        , race_data.cars[pos]->Laps
        , args.m_iNumLap-race_data.cars[pos]->Laps
        , SPD(race_data.cars[pos]->xdot, race_data.cars[pos]->ydot) * MPH_FPS
        , race_data.cars[pos]->bestlap_speed * MPH_FPS
        , race_data.cars[pos]->lastlap_speed * MPH_FPS
        , race_data.cars[pos]->Pit_stops
        , race_data.cars[pos]->damage
        , race_data.cars[pos]->fuel
        , race_data.cars[pos]->RobotTime / race_data.m_fElapsedTime
      );
    }

    strcat(s, tmp);

    DrawStringOpaque(s, x, y, color );

    v[0].x = x+20; v[0].y = y+2;
    v[1].x = x+20; v[1].y = y+6;
    v[2].x = x+10; v[2].y = y+6;
    v[3].x = x+10; v[3].y = y+2;
    DrawPolyTexture( v, 4, -2.0, 0.0, &(g_ViewManager->m_aBitmapCar[pos]) );
    y += 8;
      
    if( fBestLapSpeed<=race_data.cars[pos]->bestlap_speed)
    {
      iBestLapCar   = pos;
      fBestLapSpeed = race_data.cars[pos]->bestlap_speed;
    }
  }

  // fastest lap + BestView mode
  x=1; y=m_SizeY-8;
  char *mode[] = { "manual Update","any Overtaking", "for Position", "Nobody" };
  sprintf( s, "%10s %6.2f mi/h    %-18s", drivers[iBestLapCar]->getName(), race_data.cars[iBestLapCar]->bestlap_speed * MPH_FPS, mode[g_ViewManager->m_iFollowMode] );
  DrawStringOpaque(s, x+190, y );

  // fps
  sprintf( s, "%7.2f fps", g_ViewManager->m_fFps );
  DrawStringOpaque( s, m_SizeX-88, y );
}

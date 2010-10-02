/**
 * DRAW.CPP - the system-independent graphics portion of RARS 0.39
 *
 * (was GRAPHICS.CPP)
 * See GI.H, CAR.H & TRACK.H for class and structure declarations.    
 * GI.CPP is the system-dependent graphics portion of RARS.
 *
 * History
 *  ver. 0.1 release January 12, 1995
 *  ver. 0.2 1/23/95
 *  ver. 0.3 2/7/95
 *  ver. 0.39 3/6/95
 *  ver. 0.45 3/21/95
 *  ver. 0.50 4/5/95
 *  ver. 0.6b 5/8/95 b for beta
 *  ver. 0.61 May 26
 *  ver. 0.70 Nov 97
 *  ver. 0.71 Jan 98
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include "car.h"
#include "track.h"
#include "os.h"
#include "gi.h"
#include "draw.h"
#include "instant_replay.h"

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

////
//// Variables
////
extern unsigned int maxx, maxy;        // screen/window width in pixels
extern Gi gi;                          // Graphics Interface object

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

static int graphics_initialised = 0;   // set to true once the graphics
                                       // stuff is all okay
static unsigned int out_draw = 1000;

static fastest_lap race_record;
static fastest_lap track_record;
static double spacing;                 // see leaders()
Draw draw;

//--------------------------------------------------------------------------
//                          F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Draws a thin border around the entire screen
 */
static void Border()
{
  gi.SetColor(RAIL_COLOR);
  gi.DrawLine(0.0, 0.0, currentTrack->m_fXMax, 0.0);
  gi.DrawLine(currentTrack->m_fXMax, 0.0, currentTrack->m_fXMax, currentTrack->m_fYMax);
  gi.DrawLine(currentTrack->m_fXMax, currentTrack->m_fYMax, 0.0, currentTrack->m_fYMax);
  gi.DrawLine(0.0, currentTrack->m_fYMax, 0.0, 0.0);
}

/**
 * Color the track
 */
static void PaveTrack()
{
  int i, x, y;

  int NSEG = get_track_description().NSEG;
  segment *lftwall = get_track_description().lftwall;
  segment *rgtwall = get_track_description().rgtwall;

  gi.SetFillColor(TRACK_COLOR);
  for(i=0; i < NSEG; i++)
  {
    x = (int)((lftwall[i].beg_x + rgtwall[i].beg_x) / 2);
    y = (int)((lftwall[i].beg_y + rgtwall[i].beg_y) / 2);
    gi.FloodFill(x, y);
  }
}

/**
 * Extend line by one pixel
 */
static void Extend(double &x0, double &y0, double &x1, double &y1)
{
 double dx = x1 - x0;
 double dy = y1 - y0;
 double dist = sqrt(dx * dx + dy * dy);
 if (dist > 0)
 {
  dx /= dist;
  dy /= dist;
  x0 -= dx * SCALE;
  y0 -= dy * SCALE;
  x1 += dx * SCALE;
  y1 += dy * SCALE;
 }
}

/**
 * Get the fastest lap
 *
 * @return    a structure with the circuit, the driver and the laptime
 */
static fastest_lap get_race_fastest_lap()
{
  return race_data.m_oLapRecord;
}

//--------------------------------------------------------------------------
//                            Class Draw
//--------------------------------------------------------------------------

/**
 * Re-draw the finish line
 */
void Draw::RefreshFinishLine()
{
  if( m_bDisplay )
  {
    gi.SetColor(TEXT_COLOR);
    gi.DrawLine(currentTrack->finish_rx, currentTrack->finish_ry, currentTrack->finish_lx, currentTrack->finish_ly);
    gi.CheckWindowEvents();
  }
}

/**
 * Initializes graphics system, fills in colored regions:
 */
void Draw::GraphSetup( InstantReplay * /*ir*/ )
{
  gi.InitializeGraphics();

  // determine the distance in feet that each pixel will represent:
  SCALE = currentTrack->m_fXMax / (double)maxx;        // Either m_fXMax or m_fYMax will
  if(currentTrack->m_fYMax / (double)maxy > SCALE)     // determine the SCALE; The
  {
    SCALE = currentTrack->m_fYMax / (double)maxy;      // SCALE will be the smallest that
    currentTrack->m_fXMax = maxx * SCALE;              // will show both m_fXMax & m_fYMax.
  }
  else
  {
    currentTrack->m_fYMax = maxy * SCALE;
  }
  graphics_initialised = 1;
  
  // paint the whole screen green:
  gi.SetFillColor(FIELD_COLOR);
  gi.Rectangle(0.0, currentTrack->m_fYMax, currentTrack->m_fXMax, 0.0);
  Border();                          // draw border at screen boundary
}

/**
 * Resume the graphic display
 */
void Draw::ResumeNormalDisplay()
{
  gi.ResumeNormalDisplay();
}

/**
 * Draw track:
 */
void Draw::DrawTrack()
{
  int i;

  int NSEG = get_track_description().NSEG;
  segment *lftwall = get_track_description().lftwall;
  segment *rgtwall = get_track_description().rgtwall;

  // draw outer and inner track boundary:
  gi.SetColor(RAIL_COLOR);
  // Left wall:

  segment * old_left = lftwall + NSEG - 1;
   
  for(i=0; i < NSEG; i++)         // for each segment:
  {
    gi.DrawLine(old_left->end_x, old_left->end_y,
              lftwall[i].beg_x, lftwall[i].beg_y);

    if(!lftwall[i].radius)   // is this a straightaway?
    { 
      double x0 = lftwall[i].beg_x;
      double y0 = lftwall[i].beg_y;
      double x1 = lftwall[i].end_x;
      double y1 = lftwall[i].end_y;
      Extend(x0, y0, x1, y1);
      gi.DrawLine(x0, y0, x1, y1);
    }
    else           // curve
    {
      gi.DrawArc(lftwall[i].radius, lftwall[i].cen_x, lftwall[i].cen_y,
               lftwall[i].beg_ang, lftwall[i].length); // draw the arc
    }

    old_left = &lftwall[i];
  }
   
  // Right wall:

  segment * old_right = rgtwall + NSEG - 1;
   
  for(i=0; i < NSEG; i++)    // for each segment:
  {
    gi.DrawLine(old_right->end_x, old_right->end_y,
              rgtwall[i].beg_x, rgtwall[i].beg_y);

    if(!rgtwall[i].radius)  // is this a straightaway?
    {
      double x0 = rgtwall[i].beg_x;
      double y0 = rgtwall[i].beg_y;
      double x1 = rgtwall[i].end_x;
      double y1 = rgtwall[i].end_y;
      Extend(x0, y0, x1, y1);
      gi.DrawLine(x0, y0, x1, y1);
    }
    else           // curve
    {
      gi.DrawArc(rgtwall[i].radius, rgtwall[i].cen_x, rgtwall[i].cen_y,
               rgtwall[i].beg_ang, rgtwall[i].length); // draw the arc
    }

    old_right = &rgtwall[i];
  }
   
  // pave the track:
  PaveTrack();
  RefreshFinishLine();

  // do any stuff that may be necessary after the track is drawn
  gi.AfterGraphSetup();
}

/////////////////////////////////////////////////////////////////////////////
//                            CAR DRAWING
/////////////////////////////////////////////////////////////////////////////

void Draw::DrawCar(Car * car)
{
  if(car->out)    // rarely redraw a car that is out of the race.
  {
    if(++out_draw >= 100)
    {
      out_draw = 0; // show car
    }
    if(out_draw || car->s.stage == QUALIFYING)
    {
      return; // don't show car
    }
  }

  // erase old car
  if( m_bDrawTrajectory && (m_iCarShown == car->which) )
  {
    DrawCar(car->prex, car->prey, car->prang, SKID_COLOR, SKID_COLOR);
  }
  else
  {
    DrawCar(car->prex, car->prey, car->prang, TRACK_COLOR, TRACK_COLOR);
  }

  car->prang = car->ang+car->alpha;
  if( car->collision_draw & 1 )
  {
    // draw car in different colors to indicate collision
    DrawCar(car->x, car->y, car->prang, COLLISION, COLLISION);
  }
  else
  {
    DrawCar(car->x, car->y, car->prang, car->nose_color, car->tail_color); // draw new one
  }
  car->prex = car->x;
  car->prey = car->y; // save these to erase car next time
}

/**
 * Draw a little car on the screen, at given position and orientation,
 * and with the given colors.  (to erase the car, call it with track_color)
 *
 * @param x             coordinates of center of car
 * @param y             coordinates of center of car
 * @param ang           orientation angle of car, wrt x-axis, radians
 * @param nose          color of front portion
 * @param tail          color of rear portion
 */
void Draw::DrawCar(double x, double y, double ang, int nose, int tail)
{
  double xx, yy, endx, endy;
  double sine, cosine, dx, dy;    
  int i;    
  double LEN = CARLEN * 1.0;
  double WID = CARWID * 1.0;
    
  sine = sin(ang);    cosine = cos(ang);    
  xx = x + cosine * LEN/2 - sine * WID/2;    // left front corner coords    
  yy = y + cosine * WID/2 + sine * LEN/2;
  x = xx;  y = yy;                                 // save the above values
  dx = 0.3333 * WID * sine;
  dy = -.3333 * WID * cosine;    
  // below we draw four parallel lines to form the body of the car:
  gi.SetColor(tail);
  for(i=0; i<=3; i++)
  {
    endx = xx - LEN * cosine;
    endy = yy - LEN * sine;    
    gi.DrawLine(xx, yy, endx, endy);
    if(i == 3)
    {
      break;
    }
    xx += dx;
    yy += dy;
  }    
  // now four short lines of the nose color to decorate the front:
  gi.SetColor(nose);
  xx = x;  yy = y;             // restore x and y to left front of car    
  
  for(i=0; i<=3; i++)
  {
    endx = xx + WID * sine;    
    endy = yy - WID * cosine;    
    gi.DrawLine(xx, yy, endx, endy);
    if(i == 3)
    {
      break;
    }
    xx += dy;
    yy -= dx;
  }
}

/////////////////////////////////////////////////////////////////////////////
//               SCOREBOARD, LAP COUNT, TRACK name and length
/////////////////////////////////////////////////////////////////////////////

/**
 * Updates lap count and race fastest lap, if necessary
 *
 * @param new_lap       ?
 */
void Draw::Lapper(long new_lap)
{
  char stringa[] = "          ";
  int L;

  if (new_lap && race_data.stage != QUALIFYING)
  {
    gi.SetFillColor(FIELD_COLOR);
    gi.Rectangle(currentTrack->m_fScoreBoardX - 10 * CHR_WID, currentTrack->m_fScoreBoardY,
              currentTrack->m_fScoreBoardX - 5.2 * CHR_WID, currentTrack->m_fScoreBoardY - 1.2 * CHR_HGT);
    gi.SetColor(TEXT_COLOR);
    sprintf(stringa, "%ld", new_lap);
    L = strlen(stringa);
    gi.TextOutput(currentTrack->m_fScoreBoardX - 6*CHR_WID - L*CHR_WID, currentTrack->m_fScoreBoardY, stringa);
    gi.TextOutput(currentTrack->m_fScoreBoardX - 6*CHR_WID, currentTrack->m_fScoreBoardY, "/");
  }
  // Update current race best lap, if necessary:
  if(race_record.speed < get_race_fastest_lap().speed)
  {
    race_record = get_race_fastest_lap(); // get new record
    // This bar erases the previous race record:    
    gi.SetFillColor(FIELD_COLOR);
    gi.Rectangle(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 2.2*CHR_HGT,
              currentTrack->m_fMessageX + 30*CHR_WID,currentTrack->m_fMessageY + 1.2*CHR_HGT);
    gi.SetColor(TEXT_COLOR);  // write new record
    gi.TextOutput(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 2.2*CHR_HGT, " race record is        by");
    sprintf(stringa, "%.2f", race_record.speed * MPH_FPS);   
    gi.TextOutput(currentTrack->m_fMessageX + 14 * CHR_WID, currentTrack->m_fMessageY + 2.2*CHR_HGT, stringa);
    gi.TextOutput(currentTrack->m_fMessageX + 23 * CHR_WID, currentTrack->m_fMessageY + 2.2*CHR_HGT,
                  race_record.rob_name );
  }

  gi.CheckWindowEvents();
}

/**
 * Put up the scoreboard:
 *
 * @param stage
 * @param rl
 */
void Draw::ScoreBoard(Stage stage, long rl)
{
  int i;
  double length = get_track_description().length;    
  double XS = currentTrack->m_fScoreBoardX;
  double YS = currentTrack->m_fScoreBoardY;
  char string[] = "0123456789ABCD";    
  char stringa[] = "          ";
  int kount;    
   
  // Print track name and length...
  gi.SetColor(TEXT_COLOR);
  gi.TextOutput(currentTrack->m_fMessageX, currentTrack->m_fMessageY, "track is          length      mi");
  for(i=0; i<8; i++)
  {
    if(currentTrack->m_sFileName[i] == '.' || currentTrack->m_sFileName[i] == 0)
    {
      break;
    }
    else
    {
      string[i] = currentTrack->m_sFileName[i];
    }
  }
  string[i] = 0;    
  gi.SetColor(IP_NAME_COLOR);

  gi.TextOutput(currentTrack->m_fMessageX + 8 * CHR_WID, currentTrack->m_fMessageY, string);
  sprintf(string, "%.2f", length/5280.0);   
  gi.TextOutput(currentTrack->m_fMessageX + 22.5 * CHR_WID, currentTrack->m_fMessageY, string);
   
  // Get current track and race record:
  race_record.speed = 0.0; 
  track_record = get_track_description().record;
  // This bar erases the previous track record:    
  gi.SetFillColor(FIELD_COLOR);
  gi.Rectangle(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 2.2*CHR_HGT,
            currentTrack->m_fMessageX + 30*CHR_WID,currentTrack->m_fMessageY + 0.1*CHR_HGT);
  // Print track record:
  gi.SetColor(TEXT_COLOR);
  gi.TextOutput(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 1.1*CHR_HGT, "track record is        by");
  sprintf(string, "%.2f", track_record.speed * MPH_FPS);   
  gi.TextOutput(currentTrack->m_fMessageX + 14 * CHR_WID, currentTrack->m_fMessageY + 1.1*CHR_HGT, string);
  gi.TextOutput(currentTrack->m_fMessageX + 23 * CHR_WID, currentTrack->m_fMessageY + 1.1*CHR_HGT,
              track_record.rob_name );    
   
  // Draw background for leaderboard car pictures:
  spacing = 1.15 * CHR_HGT;   // for the leader board only    
  // these rectangles are for the leader board car pictures:    
  kount = args.m_iNumCar < LEADERS_A + LEADERS_B + 1 ?
          args.m_iNumCar : LEADERS_A + LEADERS_B + 1;
  // first erase the old board, if any:    
  gi.SetFillColor(FIELD_COLOR);
  gi.Rectangle(currentTrack->m_fLeaderBoardX - 1.9 * CARLEN, currentTrack->m_fLeaderBoardY - CHR_HGT,
            currentTrack->m_fLeaderBoardX + 19*CHR_WID, currentTrack->m_fLeaderBoardY - (kount + .9) * spacing);
  gi.SetFillColor(TRACK_COLOR);   // rectangular background for car pictures:
  gi.Rectangle(currentTrack->m_fLeaderBoardX - 1.9 * CARLEN, currentTrack->m_fLeaderBoardY - CHR_HGT,
            currentTrack->m_fLeaderBoardX -.5 * CARLEN, currentTrack->m_fLeaderBoardY - (kount + .9) * spacing);
   
  // erase and print either "Practice" or "Race" and race number:    
  gi.SetFillColor(FIELD_COLOR);
  gi.SetColor(TEXT_COLOR);
  gi.Rectangle(XS - 9 * CHR_WID, YS + 1.5 * CHR_HGT,
            XS + 1 * CHR_WID, YS - 1.5 * CHR_HGT);    
  if (race_data.stage == PRACTICE)
  {
    gi.TextOutput(XS - 9 * CHR_WID, YS + 1.5 * CHR_HGT, "Practice");
  }
  else if (race_data.stage == RACING)
  {
    gi.TextOutput(XS - 9 * CHR_WID, YS + 1.5 * CHR_HGT, "Race");
  }
  else if (race_data.stage == QUALIFYING)
  {
    gi.TextOutput(XS - 9 * CHR_WID, YS + 1.5 * CHR_HGT, "Qual");
  }
   
  //Print race or qual session number:
  if (stage != PRACTICE)
  { 
    sprintf(string, "%ld", rl+1);
    gi.TextOutput(XS-5*CHR_WID, YS+1.5*CHR_HGT, string);
  }
   
  // Print total number of laps on scoreboard:
  if (stage == PRACTICE)
  {
    sprintf(stringa, "%d", args.m_iNumPracticeLap);
  }
  else if (stage == QUALIFYING)
  {
    sprintf(stringa, "%d", args.m_iNumQualLap);
  }
  else if (stage == RACING)
  {
    sprintf(stringa, "%ld", args.m_iNumLap);
  }

  gi.TextOutput(XS - 5*CHR_WID, YS, stringa);
  int L = strlen(stringa);
  gi.TextOutput(XS - 4.5*CHR_WID + L*CHR_WID, YS, "laps");
}

/**
 * Put up the scoreboard:
 *
 * @param stage
 * @param rl
 */
int Draw::PressKey(long rl)
{
  gi.SetFillColor(FIELD_COLOR);
  gi.Rectangle(currentTrack->m_fMessageX, currentTrack->m_fMessageY +  2.2*CHR_HGT,
               currentTrack->m_fMessageX + 31*CHR_WID,currentTrack->m_fMessageY + 0.1*CHR_HGT);
  gi.SetColor(oRED);
  gi.TextOutput(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 1.1*CHR_HGT, "Press a key to continue");
  int key = os.GetCh();
  ScoreBoard( race_data.stage, rl );
  gi.CheckWindowEvents();
  return key;
}

/**
 * Print header of the leaderboard:
 *
 * @param dat           ?
 */
void Draw::PrintLeaderboardHeader()
{
  // erase old text:  
  gi.SetFillColor(FIELD_COLOR);
  gi.Rectangle(currentTrack->m_fLeaderBoardX - 2*CHR_WID, currentTrack->m_fLeaderBoardY + CHR_HGT,
            currentTrack->m_fLeaderBoardX + 19*CHR_WID, currentTrack->m_fLeaderBoardY - CHR_HGT);
   
  // print new header:
  gi.SetColor(TEXT_COLOR);
  if( m_iLeaderboardMode==0 )
  {
    if(race_data.stage == QUALIFYING)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 9.5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT,    "mph");
      gi.TextOutput(currentTrack->m_fLeaderBoardX - CHR_WID, currentTrack->m_fLeaderBoardY, "RESULTS:  best  laps");
    }
    else
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 11.5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT,   "mph");
      gi.TextOutput(currentTrack->m_fLeaderBoardX - CHR_WID, currentTrack->m_fLeaderBoardY, " SPEED:   best  last");
    }
  }
  else if( m_iLeaderboardMode==1 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 11.5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT,    "mph");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - CHR_WID, currentTrack->m_fLeaderBoardY, " SPEED:    avg   max");
  }
  else if( m_iLeaderboardMode==2 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX - 2*CHR_WID, currentTrack->m_fLeaderBoardY, "STANDINGS: Laps  Lead");
  }
  else if( m_iLeaderboardMode==3 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 11.5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT,       "sec     ");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - 2*CHR_WID, currentTrack->m_fLeaderBoardY, "NEXT_CAR: ahead  behind");
  }
  else if( m_iLeaderboardMode==4 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT, "total last time");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - 2*CHR_WID, currentTrack->m_fLeaderBoardY, "PITTING:stops stop in");
  }
  else if( m_iLeaderboardMode==5 )
  {
    //     gi.TextOutput(currentTrack->m_fLeaderBoardX + 13*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT,            "");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - 1.5*CHR_WID, currentTrack->m_fLeaderBoardY, " DAMAGE: damage  fuel");
  }
  else if( m_iLeaderboardMode==6 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 8*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT, "proj  mi/lb");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - CHR_WID, currentTrack->m_fLeaderBoardY,   " FUEL:    laps  usage");
  }
  else      //if( m_iLeaderboardMode==7 )
  {
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 8.5*CHR_WID, currentTrack->m_fLeaderBoardY+CHR_HGT, "    starting");
    gi.TextOutput(currentTrack->m_fLeaderBoardX - CHR_WID, currentTrack->m_fLeaderBoardY, "QUAL:    q_speed pos");
  }
}

/////////////////////////////////////////////////////////////////////////////
//                           LEADERBOARD
/////////////////////////////////////////////////////////////////////////////
    
/**
 * Update the leader board for i-th car:     
 *
 * @param i             number of the car
 * @param order         order of the car
 */
void Draw::Leaders(int i, int* order)
{
  char string[] = "0123456789";    
  double Y;    
  int shown_total;
   
  shown_total = LEADERS_A + LEADERS_B + 1;
  if( shown_total>args.m_iNumCar )  // decide how many are shown
  {
    shown_total = args.m_iNumCar;
  }
   
  if (m_iCarShown == -1 && i >= shown_total) // nobody is selected
  {
    return;
  }
  if (m_iCarShown > -1)
  {
    if((race_data.m_aPosOfCar[m_iCarShown] <= LEADERS_A && i >= shown_total) ||
       (race_data.m_aPosOfCar[m_iCarShown] > LEADERS_A && i > race_data.m_aPosOfCar[m_iCarShown]+LEADERS_B) ||
       (i < (race_data.m_aPosOfCar[m_iCarShown] - LEADERS_A)))
    {
      return; //don't show upper or lower part of the board, if necessary 
    }
  }
  Y = currentTrack->m_fLeaderBoardY - spacing * (i + 1);
   
  // Shift board up if necessary
  if (m_iCarShown > -1)
  { 
    if (race_data.m_aPosOfCar[m_iCarShown] >= LEADERS_A &&
        race_data.m_aPosOfCar[m_iCarShown] < args.m_iNumCar-2 ) // backmarkers
    {
      Y += spacing * (race_data.m_aPosOfCar[m_iCarShown] - LEADERS_A);
    }
    else if (race_data.m_aPosOfCar[m_iCarShown] == args.m_iNumCar-2 && args.m_iNumCar > shown_total)  // next to last car
    {
      Y += spacing * (race_data.m_aPosOfCar[m_iCarShown] - LEADERS_A - 1 );
    }
    else if (race_data.m_aPosOfCar[m_iCarShown] == args.m_iNumCar-1 && args.m_iNumCar > shown_total)  // the very last car
    {
      Y += spacing * (race_data.m_aPosOfCar[m_iCarShown] - LEADERS_A - 2);
    }
  }
   
  // Erase old name and text:    
  gi.SetFillColor(FIELD_COLOR);             // The infield color
  gi.Rectangle(currentTrack->m_fLeaderBoardX, Y, currentTrack->m_fLeaderBoardX+19*CHR_WID, Y - CHR_HGT);
  gi.Rectangle(currentTrack->m_fLeaderBoardX - 1.9*CARLEN - 2*CHR_WID, Y,   // erase position
            currentTrack->m_fLeaderBoardX - 1.9*CARLEN, Y - CHR_HGT);
   
  // Print position of m_iCarShown car:
  if(m_iCarShown == order[i])
  {
    gi.SetColor(TEXT_COLOR);
    sprintf(string, "%d", race_data.m_aPosOfCar[m_iCarShown]+1);
    gi.TextOutput(currentTrack->m_fLeaderBoardX- 1.9*CARLEN- 2*CHR_WID, Y, string);
  }   
     
  // Choose color for car name:
  gi.SetColor(TEXT_COLOR);    // normal racers
  if (m_iCarShown == order[i])
  {
    gi.SetColor(IP_NAME_COLOR);  // car instruments shown on IP
  }
  else if(race_data.cars[order[i]]->On_pit_lane)
  {
    gi.SetColor(PIT_COLOR);  // car on pit lane shown red
  }
  else if (race_data.cars[order[i]]->Out)
  {
    gi.SetColor(OUT_COLOR);  //out of race shown gray
  }
  if( race_data.stage==QUALIFYING && args.m_iQualMode==QUAL_FASTEST_LAP &&
     race_data.cars[order[i]]->Q_bestlap >= race_data.cars[order[0]]->Q_bestlap / Q_CUTOFF)
  {
    gi.SetColor(TEXT_COLOR); // qualifyier :-)
  }
  else if( race_data.stage==QUALIFYING && args.m_iQualMode==QUAL_AVERAGE_SPEED &&
          race_data.cars[order[i]]->Q_avgspeed >= race_data.cars[order[0]]->Q_avgspeed / Q_CUTOFF)
  {
    gi.SetColor(TEXT_COLOR); // qualifyier :-)
  }
   
  // Driver name:
  gi.TextOutput(currentTrack->m_fLeaderBoardX, Y, drivers[order[i]]->getName());
   
  // Car's picture:
  DrawCar(currentTrack->m_fLeaderBoardX-1.2*CARLEN, Y - 0.5*CHR_HGT , 0.0,
          drivers[order[i]]->getNoseColor(), drivers[order[i]]->getTailColor());    

  // Now write data:
  // Data color:
  if(race_data.cars[order[i]]->Out == 1 || race_data.cars[order[i]]->Out == 3)
  {
    gi.SetColor(OUT_COLOR);
  }
  else if(race_data.cars[order[i]]->On_pit_lane)
  {
    gi.SetColor(PIT_COLOR);  // car on pit lane shown red
  }
  else  
  {
    gi.SetColor(TEXT_COLOR);
  }
   
  if( m_iLeaderboardMode==0 )
  {
    // the fastest lap speed:    
    sprintf(string, "%.2f", race_data.cars[order[i]]->Bestlap_speed * MPH_FPS);
    if( race_data.stage==QUALIFYING && args.m_iQualMode==QUAL_AVERAGE_SPEED ) // qual. for average speed
    {
      sprintf(string, "%.2f", race_data.cars[order[i]]->Q_avgspeed * MPH_FPS);
    }
    gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
    if( race_data.stage==QUALIFYING ) // print laps
    {
      sprintf(string, "%ld", race_data.cars[order[i]]->Laps);
    }
    else // print the speed of last lap:    
    {
      sprintf(string, "%.2f", race_data.cars[order[i]]->Lastlap_speed * MPH_FPS);
    }
    gi.TextOutput(currentTrack->m_fLeaderBoardX+13.5*CHR_WID, Y, string);
  }
  else if( m_iLeaderboardMode==1 )
  {
    // the average speed:    
    sprintf(string, "%.2f", race_data.cars[order[i]]->Speed_avg * MPH_FPS);
    gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
    // the maximum speed:    
    sprintf(string, "%.2f", race_data.cars[order[i]]->Speed_max * MPH_FPS);
    gi.TextOutput(currentTrack->m_fLeaderBoardX+13.5*CHR_WID, Y, string);
  }
  else if( m_iLeaderboardMode==2 )
  {
     // Distance behind leader:
    if (race_data.cars[order[i]]->Out == 1)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 9 * CHR_WID, Y, "out");
    }
    else if (race_data.cars[order[i]]->Out == 2)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 8.5*CHR_WID, Y, "pits");
    }
    else if (race_data.cars[order[i]]->Out == 3)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 9 * CHR_WID, Y, "DNQ");
    }
    else if (!race_data.cars[order[i]]->Behind_leader) // Leading car
    { 
      sprintf(string, "%ld", race_data.cars[order[i]]->Laps);
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 7.5 * CHR_WID, Y, string);
      gi.TextOutput(currentTrack->m_fLeaderBoardX + (7.5 + strlen(string)) * CHR_WID, Y, "laps");
    } 
    else if (race_data.cars[order[i]]->Behind_leader < 0) // Car more than lap down
    {
      sprintf(string, "%ld", long(race_data.cars[order[i]]->Behind_leader));
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 8.5 * CHR_WID, Y, string);
      gi.TextOutput(currentTrack->m_fLeaderBoardX + (8.5+strlen(string)) * CHR_WID, Y, "L");
    } 
    else                                // Car on the lead lap
    {
      sprintf(string, "%.1f", race_data.cars[order[i]]->Behind_leader);
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 8.5*CHR_WID, Y, string);
    }   
    // laps lead:
    sprintf(string, "%ld", race_data.cars[order[i]]->Laps_lead);
    gi.TextOutput(currentTrack->m_fLeaderBoardX + 14.5*CHR_WID, Y, string);
  }
  else if( m_iLeaderboardMode==3 )
  {
    if (race_data.cars[order[i]]->Out == 1)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 11 * CHR_WID, Y, "out");
    }
    else if (race_data.cars[order[i]]->Out == 2)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX +  9 * CHR_WID, Y, "pitting");
    }
    else if (race_data.cars[order[i]]->Out == 3)
    {
      gi.TextOutput(currentTrack->m_fLeaderBoardX + 11 * CHR_WID, Y, "DNQ");
    }
    else  // car is running:
    {
      // the car ahead:    
      sprintf(string, "%.1f", race_data.cars[order[i]]->Behind_next);
      gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
      // the car behind:    
      if (race_data.cars[order[i]]->Ahead_next == 999) // Car was just passed
      {
        gi.TextOutput(currentTrack->m_fLeaderBoardX + 13 * CHR_WID, Y, "");
      }
      else if (race_data.cars[order[i]]->Ahead_next < 0) // Car more than lap down
      { 
        sprintf(string, "%ld", long(race_data.cars[order[i]]->Ahead_next));
        gi.TextOutput(currentTrack->m_fLeaderBoardX + 13.5 * CHR_WID, Y, string);
        gi.TextOutput(currentTrack->m_fLeaderBoardX + (13.5+strlen(string)) * CHR_WID, Y, "L");
      } 
      else                                   // Car on the same lap
      {
        sprintf(string, "%.1f", race_data.cars[order[i]]->Ahead_next);
        gi.TextOutput(currentTrack->m_fLeaderBoardX+13.5*CHR_WID, Y, string);
      }   
    }
  }
  else if( m_iLeaderboardMode==4 )
  {
    // the total number of pit visits:    
    sprintf(string, "%d", race_data.cars[order[i]]->Pit_stops);
    gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
    // the last pit visit:    
    sprintf(string, "%ld", race_data.cars[order[i]]->Last_pit_visit);
    gi.TextOutput(currentTrack->m_fLeaderBoardX+10.5*CHR_WID, Y, string);
    // time spent in pits:
    sprintf(string, "%ld", long(race_data.cars[order[i]]->Total_pit_time));
    gi.TextOutput(currentTrack->m_fLeaderBoardX+14.5*CHR_WID, Y, string);
  }
  else if( m_iLeaderboardMode==5 )
  {
    // the damage:    
    sprintf(string, "%ld", race_data.cars[order[i]]->get_damage());
    gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
    // approx. fuel(lb):
    sprintf(string, "%.1f", race_data.cars[order[i]]->get_fuel());
    gi.TextOutput(currentTrack->m_fLeaderBoardX+13.5*CHR_WID, Y, string);
  }
  else if( m_iLeaderboardMode==6 )
  {
    // projected laps:
    sprintf(string, "%ld", race_data.cars[order[i]]->get_proj_laps());
    gi.TextOutput(currentTrack->m_fLeaderBoardX+8.5*CHR_WID, Y, string);
    // fuel efficiency:
    sprintf(string, "%.2f", race_data.cars[order[i]]->get_fuel_mileage());
    gi.TextOutput(currentTrack->m_fLeaderBoardX+13.5*CHR_WID, Y, string);
  }
  else //if(m_iLeaderboardMode == 7)
  {
    // qualification result:
    if( args.m_iQualMode==QUAL_AVERAGE_SPEED )
    {
      sprintf(string, "%.2f", race_data.cars[order[i]]->Q_avgspeed * MPH_FPS);
    }
    else
    {
      sprintf(string, "%.2f", race_data.cars[order[i]]->Q_bestlap * MPH_FPS);
      gi.TextOutput(currentTrack->m_fLeaderBoardX+7.5*CHR_WID, Y, string);
      // starting position:
      sprintf(string, "%d", race_data.cars[order[i]]->Started + 1);
      gi.TextOutput(currentTrack->m_fLeaderBoardX+14.5*CHR_WID, Y, string);
    }
  }
}

/////////////////////////////////////////////////////////////////////////////
//                         INSTRUMENT PANEL
/////////////////////////////////////////////////////////////////////////////

/**
 * Instrument Panel for car i    
 *
 * @param i             number of the car
 */
void Draw::Instruments()
{
  char out[16];    
  static int ip_up = 0;        // set when the descriptions are printed    
  static int iwas = -1;        // previous i

  if(m_iCarShown < 0 || m_iCarShown >= args.m_iNumCar)    // Erase IP if i is invalid.
  { 
    ip_up = 0;                  
    gi.SetFillColor(FIELD_COLOR);
    gi.Rectangle(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY,currentTrack->m_fInstPanelX + 17*CHR_WID, currentTrack->m_fInstPanelY - 8*CHR_HGT);

    return;    
  }    
    
  if(!ip_up)                  // First call   
  { 
    gi.SetColor(TEXT_COLOR);
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 1 * CHR_HGT, "slip");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 2 * CHR_HGT, "speed");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 3 * CHR_HGT, "lateral g's");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 4 * CHR_HGT, "in-line g's");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 5 * CHR_HGT, "skid angle");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 6 * CHR_HGT, "damage");
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY - 7 * CHR_HGT, "fuel");

    ip_up = 1;    
  }     

  Driver * driver = drivers[m_iCarShown];
  Car * car = race_data.cars[m_iCarShown];

  if (m_iCarShown != iwas) // print new name only if new car marked
  { 
    gi.SetFillColor(FIELD_COLOR);     // The infield color
    gi.Rectangle(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY, currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - CHR_HGT); // erase name
   
    gi.SetColor(IP_NAME_COLOR); // print new name
    gi.TextOutput(currentTrack->m_fInstPanelX, currentTrack->m_fInstPanelY, (char *)(driver->getName()) );
    gi.TextOutput(currentTrack->m_fInstPanelX + (strlen(driver->getName())-.5) * CHR_WID, currentTrack->m_fInstPanelY, "'s data:");
  } 
 
  gi.SetColor(IP_NUM_COLOR);
  if (!m_iFastDisplay)
  {
    gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - CHR_HGT,
              currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 2 * CHR_HGT);
    sprintf(out, "%4.1f",(car->get_vc() - car->get_speed()) * MPH_FPS);
    gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - CHR_HGT, out);

    gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 2 * CHR_HGT,
              currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 3 * CHR_HGT);
    sprintf(out, "%4.1f", car->get_speed() * MPH_FPS);
    gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 2 * CHR_HGT, out);
    
    gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 3 * CHR_HGT,
              currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 4 * CHR_HGT);
    sprintf(out, "%4.2f", car->get_lat_acc());
    gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 3 * CHR_HGT, out);
    
    gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 4 * CHR_HGT,
              currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 5 * CHR_HGT);
    sprintf(out, "%4.2f", car->get_lin_acc());
    gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 4 * CHR_HGT, out);
    
    gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 5 * CHR_HGT,
              currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 6 * CHR_HGT);
    sprintf(out, "%4.2f", car->get_alpha() * DEGPRAD);
    gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 5 * CHR_HGT, out);
  } 
  gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 6 * CHR_HGT,
            currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 7 * CHR_HGT);
  sprintf(out, "%ld", car->get_damage());
  gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 6 * CHR_HGT, out);
  
  gi.Rectangle(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 7 * CHR_HGT,
            currentTrack->m_fInstPanelX + 17 * CHR_WID, currentTrack->m_fInstPanelY - 8 * CHR_HGT);
  sprintf(out, "%4.1f", car->get_fuel());
  gi.TextOutput(currentTrack->m_fInstPanelX + 11 * CHR_WID, currentTrack->m_fInstPanelY - 7 * CHR_HGT, out);
   
  gi.CheckWindowEvents();
}

/**
 * See what has changed and flag it
 */
void Draw::CalcNewData( int old_order[] )
{
  for(int i=0; i<args.m_iNumCar; i++)
  {
    if (old_order[i] != race_data.m_aCarInPos[i])
    {
      if (race_data.m_aCarInPos[i] == m_iCarShown && race_data.m_aPosOfCar[m_iCarShown] >= LEADERS_A)
      {
        for(i=0; i<args.m_iNumCar; i++) // update all if designated car passed
        {
          m_aNewData[race_data.m_aCarInPos[i]] = true;
        }
      }
      else // update only one line:
      {
        m_aNewData[race_data.m_aCarInPos[i]] = true;
      }
    }
  }
}

/**
 * Initialize draw.m_aNewData[] array
 */
void Draw::InitNewData()
{
  for(int i=0; i<MAX_CARS; i++)
  {
    m_aNewData[i] = true;   // calls leaders() at start
  }
}

/**
 * For any line flagged by draw.m_aNewData[], update that line on Leaderboard
 */
void Draw::UpdateLeaderboard()
{
  // For any line flagged by draw.m_aNewData[], update that
  // line on Leaderboard:
  for(int i=0; i<args.m_iNumCar; i++)
  {
    if(m_aNewData[race_data.m_aCarInPos[i]])
    {
      m_aNewData[race_data.m_aCarInPos[i]] = false;          // reset flag
      if( m_bDisplay )
      {
        Leaders(i, race_data.m_aCarInPos);   // Write data to leaderboard
      }
    }
  }
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
    gi.SetFillColor(FIELD_COLOR);
    gi.Rectangle(currentTrack->m_fMessageX, currentTrack->m_fMessageY +  2.2*CHR_HGT,
                 currentTrack->m_fMessageX + 31*CHR_WID,currentTrack->m_fMessageY + 0.1*CHR_HGT);
    gi.SetColor(oRED);
    gi.TextOutput(currentTrack->m_fMessageX, currentTrack->m_fMessageY + 1.1*CHR_HGT, s);
    gi.CheckWindowEvents();
  }
  else
  {
    printf( "%s\n", s );
  }
}



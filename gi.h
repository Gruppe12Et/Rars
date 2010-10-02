/**
 * GI.H - declarations for system dependent graphics portion of RARS
 *
 * This is part of version 0.50 of RARS (Robot Auto Racing Simulation)
 *
 * History
 *  ver. 0.1 release January 12, 1995
 *  ver. 0.39 3/6/95
 *  ver. 0.45 3/21/95 
 *  ver. 0.50 4/5/95 
 *  ver. 0.67 1997/10/27
 *  ver. 0.70  Dec.1997
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef __RARSCORE_GI_H
#define __RARSCORE_GI_H

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

// Colors
enum                         // Borland uses these color names pre-fixed by EGA_
{
   oBLACK,                   // dark colors
   oBLUE,
   oGREEN,
   oCYAN,
   oRED,
   oMAGENTA,
   oBROWN,
   oLIGHTGRAY,
   oDARKGRAY,                // light colors 
   oLIGHTBLUE,
   oLIGHTGREEN,
   oLIGHTCYAN,
   oLIGHTRED,
   oLIGHTMAGENTA,
   oYELLOW,
   oWHITE
};

const int TRACK_COLOR = oDARKGRAY;     // The color of the race course
const int FIELD_COLOR = oGREEN;        // The fields around and inside the track
const int TEXT_COLOR  = oBLACK;        // most text is this color
const int RAIL_COLOR  = oWHITE;        // The walls of the track
const int IP_NAME_COLOR  = oWHITE;     // Driver's name on instrument panel
const int IP_NUM_COLOR  = oYELLOW;     // numerical readout of instrument panel
const int OUT_COLOR = oLIGHTBLUE;      // name of driver who is out of race
const int PIT_COLOR = oRED;            // name of driver who is on pit lane
const int SKID_COLOR = oBLACK;         // car's trajectory drawn with this color
const int COLLISION = oLIGHTRED;       // car that has taken damage after collision 

const int LEADERS_A = 5;               // how many shown on leaderboard ahead you?
const int LEADERS_B = 2;               // how many shown on leaderboard behind you?

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

////
//// Variables
////
extern double SCALE;         // defined in GI.CPP; feet per pixel
extern double CHR_HGT;       // character height feet
extern double CHR_WID;       // character width in feet
extern double SCALE;         // feet per pixel
//extern double X_MAX, Y_MAX;  // screen extent in feet

////
//// Functions
////
extern double random(int);

/**
 * Gi Graphics Interface (System dependent functions )
 * Used only in draw.cpp
 */
class Gi
{
public:
  void InitializeGraphics();
  void AfterGraphSetup();
  void ResumeNormalDisplay();

  void CheckWindowEvents();

  void DrawLine(double, double, double, double);
  void DrawCircle(double,double,double);
  void DrawArc(double, double, double, double, double);
  void Rectangle(double, double, double, double);
  void FloodFill(double, double);
  void TextOutput(double, double, const char *);

  void SetColor(int);
  void SetFillColor(int);
};

#endif


// This is part of version 0.60 of RARS (Robot Auto Racing Simulation)      
// M. Timin, Feb. 1995      
// ver. 0.39 3/6/95      
// ver. 0.45 3/21/95       
// ver. 0.50 4/5/95       
// ver. 0.6b May 8, 1995  b for beta      
// ver. 0.61 May 26      
// ver. 0.70 Dec 97

#include <math.h>
const double PPI = 3.14159265;
const double DEGPRAD = 360.0/(2.0 * PPI);   // degrees per radian
      
struct colors {int nose, tail; };   // to hold the colors of one car      
      
enum {          // Borland uses these color names pre-fixed by EGA_      
   oBLACK,           /* dark colors */      
   oBLUE,      
   oGREEN,      
   oCYAN,      
   oRED,      
   oMAGENTA,      
   oBROWN,      
   oLIGHTGRAY,      
   oDARKGRAY,        /* light colors */      
   oLIGHTBLUE,      
   oLIGHTGREEN,      
   oLIGHTCYAN,      
   oLIGHTRED,      
   oLIGHTMAGENTA,      
   oYELLOW,      
   oWHITE      
};      
      
const int TRACK_COLOR = oLIGHTGRAY; // The color of the race course      
const int FIELD_COLOR = oGREEN;     // The fields around and inside the track      
const int TEXT_COLOR  = oBLACK;     // most text is this color      
const int RAIL_COLOR  = oWHITE;     // The walls of the track      
const int IP_NAME_COLOR  = oWHITE;  // Driver's name on instrument panel      
const int IP_NUM_COLOR  = oYELLOW;  // numerical readout of instrument panel      
const int OUT_COLOR = oLIGHTBLUE;   // name of driver who is out of race
const int PIT_COLOR = oRED;         // name of driver who is on pit lane
const int SKID_COLOR = oDARKGRAY;   // car's trajectory drawn with this color
const int COLLISION = oLIGHTRED;    // car that has taken damage after collision 

const int MAX_LEADERS = 10; // how many shown on leaderboard?
      
extern double SCALE;     // defined in GI.CPP; feet per pixel      
extern double CHR_HGT;   // character height feet      
extern double CHR_WID;   // character width in feet      
extern double X_MAX, Y_MAX;   // screen extent in feet      
      
// The functions defined in GI.CPP:      
void draw_line(double, double, double, double);      
void draw_arc(double, double, double, double, double);      
void draw_circle(double,double,double);      
void draw_arc1(double,double,double,double,double);      
void rectangle(double, double, double, double);      
void set_color(int);      
void set_fill_color(int);      
void flood_fill(double, double);      
void text_output(double, double, char *);      
void initialize_graphics();
void after_graph_setup();


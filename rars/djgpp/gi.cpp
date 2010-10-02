// GI.CPP - System Dependent Graphics Functions - D. Eleveld, October 1998
// This is the DJGPP/Allegro version
// version 0.1 October 11,1998
   
#include <allegro.h>
#include <stdlib.h>   
#include "gi.h"   
#include "misc.h"

const int CHR_HGT_PIX = 10;  // height in pixels of row of text
const int CHR_WID_PIX = 9;  // width in pixels of char of text   
   
double SCALE;  // feet per pixel   
double CHR_HGT;   // character height feet   
double CHR_WID;   // character width in feet   
   
static int maxx, maxy;            //screen limits, pixels   
   
// Translation table between our colors and Allegro's
static int tran_table[] = {             // identifiers from graphics.h
      0,        // our_color 0
      0,         // our_color 1
      0,       // our_color 2
      0,         // our_color 3
      0,          // our_color 4
      0,      // our_color 5
      0,        // our_color 6
      0,    // our_color 7
      0,     // our_color 8
      0,    // our_color 9
      0,   // our_color 10
      0,    // our_color 11
      0,     // our_color 12
      0, // our_color 13
      0,       // our_color 14
      0         // our_color 15
      };


void Draw::ResumeNormalDisplay()   // This just terminates graphics mode.
{   
   set_gfx_mode(GFX_TEXT, 80, 20, 0, 0);
}   
   
// Feet to Pixel converters:   
inline int xcon(double x)        // convert x coordinate to pixels   
{   
   return round(x / SCALE);   
}   
   
inline int ycon(double y)        // convert y coordinate to pixels   
{   
   return maxy - round(y / SCALE);   
}   
   
// returns degrees of arc length to cover 2 pixels, given radius in feet   
inline int ex_func(double radius)   
{   
   double feet_needed, radians_needed;   
   
   feet_needed = 2.0 * SCALE;   
   radians_needed = feet_needed / radius;   
   return 1 + int(radians_needed * DEGPRAD);   
}   
   
// Called before drawing to set the color that will be used.
static int _color;

void set_color(int our_color)   // our_color can range from 0 through 15
{
   _color = tran_table[our_color];
}

// Draw a straight line in the current drawing color from x0, y0   
// to x1, y1.  Those are feet; global SCALE is used to compute pixels.   
void draw_line(double x0, double y0, double x1, double y1)   
{   
   line(screen, xcon(x0), ycon(y0), xcon(x1), ycon(y1), _color);
}   
   
// Added by Maido Remm for debugging purposes:   
void draw_circle(double x, double y, double rad)   
{   
  circle(screen, xcon(x), ycon(y), xcon(rad), _color);
}   
   
// Added by Maido Remm for debugging purposes:   
void draw_arc1(double x, double y, double beg_ang, double end_ang, double rad)
{   
//   arc(xcon(x), ycon(y), int(beg_ang), int(end_ang), xcon(rad));
}   
   
   
// called before flood_fill() and rectangle() to determine the fill color:
static int _fill_color;

void set_fill_color(int our_color)
{
   _fill_color = tran_table[our_color];
}

// Draw a filled rectangle in the current FILL color.   
// All values are in feet; global SCALE is used to compute pixels.   
// The Borland bar() function is used.   
void rectangle(double ulx,  // upper left corner x coordinate   
               double uly,  // upper left corner y coordinate   
               double lrx,  // lower right corner x coordinate   
               double lry)  // lower right corner y coordinate   
{   
   rectfill(screen, xcon(ulx), ycon(uly), xcon(lrx), ycon(lry), _fill_color);
}   
   
   
void flood_fill(double X, double Y)   
{   
   floodfill(screen, xcon(X), ycon(Y), _fill_color);
}   
   
void text_output(double X, double Y, char* source)   
{   
   text_mode(-1);
   textout(screen, font, source, xcon(X), ycon(Y), tran_table[15]);
}   
   
// Initializes the graphics sub-system, making it ready to draw.   
// Also determines the number of pixels on the screen and uses   
// that to set the SCALE global variable.   
void initialize_graphics()   
{   
   allegro_init();
//   install_timer();

   set_color_depth(8);
   if (set_gfx_mode(GFX_VESA1, 1024, 768, 0, 0)<0)
     if (set_gfx_mode(GFX_AUTODETECT, 640, 480, 0, 0)<0)
        exit(1);

   // Load a nice palette
   PALETTE pal;
   generate_332_palette(pal);
   set_palette(pal);
   RGB black = {0,0,0};
   set_color(0,&black);

   // find out the size of the screen
   maxy = SCREEN_H;
   maxx = SCREEN_W;

   // There RGB values are probably wrong and as 
   // I am colorblind, I cannot properly correct them by myself (D.E.)
   tran_table[0] = makecol(0,0,0);        // Black
   tran_table[1] = makecol(0,0,255);      // Blue
   tran_table[2] = makecol(0,192,0);      // Green
   tran_table[3] = makecol(255,0,255);    // Cyan
   tran_table[4] = makecol(255,0,0);      // Red
   tran_table[5] = makecol(255,255,0);    // Magenta
   tran_table[6] = makecol(128,128,128);   // Brown
   tran_table[7] = makecol(150,150,150);  // Light grey
   tran_table[8] = makecol(110,110,110);     // Dark grey
   tran_table[9] = makecol(128,128,255);  // Light blue
   tran_table[10] = makecol(128,255,128); // Light green
   tran_table[11] = rand();               // Light cyan
   tran_table[12] = makecol(255,128,255); // Light red
   tran_table[13] = rand();               // Light magenta
   tran_table[14] = makecol(0,255,255);   // Yelloy
   tran_table[15] = makecol(255,255,255); // White

// End of system dependent code.
// ------------------------------------------------------------------------
// The rest of this function is portable:
   // determine the distance in feet that each pixel will represent:
   SCALE = X_MAX / (double)maxx;          // Either X_MAX or Y_MAX will
   if(Y_MAX / (double)maxy > SCALE) {     // determine the SCALE; The 
      SCALE = Y_MAX / (double)maxy;       // SCALE will be the smallest that
      X_MAX = maxx * SCALE;               // will show both X_MAX & Y_MAX.
   }
   else
      Y_MAX = maxy * SCALE;
   CHR_HGT = CHR_HGT_PIX * SCALE;      // compute character height in feet
   CHR_WID = CHR_WID_PIX * SCALE;      // compute character width in feet
}

void after_graph_setup()
{
}

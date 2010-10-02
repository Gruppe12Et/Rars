// GI.CPP - System Dependent Graphics Functions - M. Timin, February 1995
// This is the Borland version - requires EGAVGA.BGI to be present at runtime
// version 0.4 March 15, 1995
// ver. 0.46 3/25/95
// ver. 0.6b 5/8/95 b for beta
// ver. 0.61 May 26

#include <graphics.h>    // Borland's in this version
#include <stdlib.h>
#include "gi.h"
#include "misc.h"

const int CHR_HGT_PIX = 10;  // height in pixels of row of text
const int CHR_WID_PIX = 9;  // width in pixels of char of text

double SCALE;  // feet per pixel
double CHR_HGT;   // character height feet
double CHR_WID;   // character width in feet

static int maxx, maxy;            //screen limits, pixels

// This equates our colors with Borland's:
static EGA_COLORS tran_table[] = {             // identifiers from graphics.h
      EGA_BLACK,        // our_color 0
      EGA_BLUE,         // our_color 1
      EGA_GREEN,        // our_color 2
      EGA_CYAN,         // our_color 3
      EGA_RED,          // our_color 4
      EGA_MAGENTA,      // our_color 5
      EGA_BROWN,        // our_color 6
      EGA_LIGHTGRAY,    // our_color 7
      EGA_DARKGRAY,     // our_color 8
      EGA_LIGHTBLUE,    // our_color 9
      EGA_LIGHTGREEN,   // our_color 10
      EGA_LIGHTCYAN,    // our_color 11
      EGA_LIGHTRED,     // our_color 12
      EGA_LIGHTMAGENTA, // our_color 13
      EGA_YELLOW,       // our_color 14
      EGA_WHITE  };     // our_color 15

void Draw::ResumeNormalDisplay() // This just terminates graphics mode.
{
    closegraph();
}

// Feet to Pixel converters:
inline int xcon(double x)        // convert x coordinate to pixels
{
   return iround(x / SCALE);
}

inline int ycon(double y)        // convert y coordinate to pixels
{
   return maxy - iround(y / SCALE);
}

// returns degrees of arc length to cover 2 pixels, given radius in feet
inline int ex_func(double radius)
{
   double feet_needed, radians_needed;

   feet_needed = 2.0 * SCALE;
   radians_needed = feet_needed / radius;
   return 1 + int(radians_needed * DEGPRAD);
}

// Draw a straight line in the current drawing color from x0, y0
// to x1, y1.  Those are feet; global SCALE is used to compute pixels.
// The Borland line() function is used.
void draw_line(double x0, double y0, double x1, double y1)
{
   line(xcon(x0), ycon(y0), xcon(x1), ycon(y1));
}

// Added for debugging purposes:
void draw_circle(double x, double y, double rad)
{
   circle(xcon(x), ycon(y), xcon(rad));
}

/* new routine: draw_arc() routine that uses draw_line() to draw the arc */
#define LINESEG_LENGTH 20 /* this constant can be used to tweak the precision */
void draw_arc(double radius, double center_x,
              double center_y, double start_angle,
              double length)
{
	double a;
	double stepsize;
	double x1, y1, x2, y2;

	/* convert a right turn so it is consistent with the left turn */
	if (radius < 0.0)
	{
		radius = -radius;

		start_angle = start_angle - length - PI;
		while (start_angle < 0.0)
		{
			start_angle += (2 * PI);
		}
	}

        /* calculate the starting point */
	x1 = center_x + radius * sin(start_angle);
	y1 = center_y - radius * cos(start_angle);

	/* determine the step size */
	stepsize = LINESEG_LENGTH * SCALE / (2.0 * PI * radius);

        /* draw lines over the length from there, adapting the number of
         * steps to the length */
	for (a = stepsize; a < length; a += stepsize)
	{
		/* calculate the end point of this line */
		x2 = center_x + radius * sin(start_angle + a);
		y2 = center_y - radius * cos(start_angle + a);

		/* draw the line */
		draw_line(x1, y1, x2, y2);

                /* make the end point the new starting point for the
                 * next line */
		x1 = x2;
		y1 = y2;
	}

	/* calculate the end point of the arc */
	x2 = center_x + radius * sin(start_angle + length);
	y2 = center_y - radius * cos(start_angle + length);

	/* draw the last line */
	draw_line(x1, y1, x2, y2);
}

// Added by Maido Remm for debugging purposes:
void draw_arc1(double x, double y, double beg_ang, double end_ang, double
rad)
{
   arc(xcon(x), ycon(y), int(beg_ang), int(end_ang), xcon(rad));
}

// Draw a filled rectangle in the current FILL color.
// All values are in feet; global SCALE is used to compute pixels.
// The Borland bar() function is used.
void rectangle(double ulx,  // upper left corner x coordinate
               double uly,  // upper left corner y coordinate
               double lrx,  // lower right corner x coordinate
               double lry)  // lower right corner y coordinate
{
   bar(xcon(ulx), ycon(uly), xcon(lrx), ycon(lry));
}

// Called before drawing to set the color that will be used.
// This calls Borland setcolor();
void set_color(int our_color)   // our_color can range from 0 through 15
{
   setcolor(tran_table[our_color]);
}

// called before flood_fill() and rectangle() to determine the fill color:
void set_fill_color(int our_color)
{
   setfillstyle(SOLID_FILL, tran_table[our_color]);
}

void flood_fill(double X, double Y)
{
   floodfill(xcon(X), ycon(Y), getmaxcolor());
}

void text_output(double X, double Y, char* source)
{
    outtextxy(xcon(X), ycon(Y), source);
}

// Initializes the graphics sub-system, making it ready to draw.
// Also determines the number of pixels on the screen and uses
// that to set the SCALE global variable.
void initialize_graphics()
{
    // setup graphics, request auto detection */
   int gdriver = DETECT, gmode, errorcode;
   /* initialize graphics and local variables */
   initgraph(&gdriver, &gmode, "");
   errorcode = graphresult();
   if (errorcode != grOk)
      exit(1);                 // This path only if an error detected

   // find out the size of the screen
   maxy = getmaxy();
   maxx = getmaxx();

   setcolor(getmaxcolor());
   setlinestyle(SOLID_LINE, 1, 1);
   if(graphresult() != grOk)
      exit(1);                 // This path only if an error detected
}

void after_graph_setup()
{
}

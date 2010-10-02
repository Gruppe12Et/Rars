/**
 * GI.CPP - System Dependent Graphics Functions - M. Timin, February 1995
 * This is the UNIX version
 *
 * Ported to UNIX by Lars R. Clausen (elascurn@daimi.aau.dk)
 * This version is for UNIX platforms, not X specific.
 * History
 * Upgraded to version 0.61 by Randy Saint (saint@phoenix.net)
 * Upgraded to version 0.67 by Henning Klaskala (bm321465@muenchen.org)
 *
 * @author    Lars R. Clausen
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.82
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <iostream>
#include <fstream>
#include <unistd.h>
#include <assert.h>

using namespace std;

// X11 include files
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xatom.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>
#include <X11/keysymdef.h>

#include "car.h"
#include "os.h"
#include "misc.h"
#include "gi.h"
#include "draw.h"

//--------------------------------------------------------------------------
//                            D E F I N E S
//--------------------------------------------------------------------------

#ifdef DEBUG_GRAPHICS
#define TRACE(s)                        \
    fprintf(stderr, "%s:%d %s\n", __FILE__, __LINE__, s)
#else
#define TRACE(s)
#endif

#define MAX_COLOR 16

//--------------------------------------------------------------------------
//                              T Y P E S
//--------------------------------------------------------------------------

typedef enum {dir_up = -1, dir_down = 1} dir_type;

//--------------------------------------------------------------------------
//                            E X T E R N S
//--------------------------------------------------------------------------

Display *XOpenDisplay();

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

Gi gi;

// X stuff
static Window win = 0;
static Display *dpy = 0;
static GC gc = 0;
static GC fill_gc = 0;       // fill_gc is used for Rectangle and Floodfill,
                             // where fill-color == foreground-color
static XFontStruct *fnt = 0;
static Pixmap pixmap = 0;
static Drawable drawable = 0; 

static int CHR_HGT_PIX;             // height in pixels of row of text
static int CHR_WID_PIX;             // width in pixels of char of text
double SCALE;                // feet per pixel
double CHR_HGT;              // character height feet
double CHR_WID;              // character width in feet

unsigned int maxx, maxy;     //screen limits, pixels

int tran_table[MAX_COLOR];
XColor color_table[MAX_COLOR];
char *color_names[MAX_COLOR] =
{ "black", "blue", "green3", "cyan", "red3", "magenta3", "brown", "lightgray", "gray",  "lightblue", "green1", "lightcyan", "red1", "magenta1", "yellow", "white"};

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Feet to Pixel converters: convert x coordinate to pixels
 */
inline int xcon(double x)
{
  return iround(x / SCALE);
}

/**
 * Feet to Pixel converters: convert y coordinate to pixels
 */
inline int ycon(double y)
{
  return maxy - iround(y / SCALE);
}

/**
 * Returns degrees of arc length to cover 2 pixels, given radius in feet
 */
inline int ex_func(double radius)
{
  double feet_needed, radians_needed;

  feet_needed = 2.0 * SCALE;
  radians_needed = feet_needed / radius;
  return 1 + int(radians_needed * DEGPRAD);
}

static XImage *xwin2image()
{
  return(XGetImage(dpy, drawable, 0, 0, maxx, maxy, AllPlanes, ZPixmap));
}

static XImage *point2image( int x, int y )
{
  return(XGetImage(dpy, drawable, x, y, 1, 1, AllPlanes, ZPixmap));
}

static unsigned int get_width()
{
  Window root;
  int x,y;
  unsigned int width,height,border,depth;

  XGetGeometry(dpy,win,&root,&x,&y,&width,&height,&border,&depth);
  return width;
}

static unsigned int get_height()
{
  Window root;
  int x,y;
  unsigned int width,height,border,depth;

  XGetGeometry(dpy,win,&root,&x,&y,&width,&height,&border,&depth);
  return height;
}

/**
 * Initializes fonts to fit screen.
 * Should look like 9x10 font on 640x400
 */
static void init_fonts()
{
  char font_buf[1000];
  int font_height = 12; //15*get_height()/800;
  int font_width = 8;

  sprintf(font_buf, "-*-lucidatypewriter-bold-r-*-*-%d-*-*-*-*-*-*-*",font_height);

  if (!(fnt = XLoadQueryFont(dpy, font_buf)))
  {
    cout << "Couldn't load font " << font_buf << ". ";
    cout << "Trying " << font_width << "x" << font_height << "." << endl;
    sprintf(font_buf, "%dx%d", font_width, font_height);
    if (!(fnt = XLoadQueryFont(dpy, font_buf)))
    {
      cout << "Couldn't load font " << font_buf << ". Using fixed." << endl;
      if (!(fnt = XLoadQueryFont(dpy, "fixed")))
      {
        cout << "What's this? Not even a 'fixed' font???" << endl;
        exit(1);
      }
    }
  }

  font_width = fnt->max_bounds.width;
  font_height = fnt->max_bounds.ascent+fnt->max_bounds.descent;

  CHR_HGT_PIX = font_height;
  CHR_WID_PIX = int(font_width*1.2);
}

/**
 * Does no checking for correct visuals (yet...)
 * Doesn't think about screens, either
 */
static void create_colors()
{
  Colormap cmap = DefaultColormap(dpy, 0);
  int i;
  XColor xcol,dummy;

  for (i=0; i< MAX_COLOR; i++)
  {
    if (!XAllocNamedColor(dpy, cmap, color_names[i], &xcol, &dummy))
    {
      cout << "Can't alloc color " << color_names[i] << ". Using Black.\n";
      tran_table[i] = BlackPixel(dpy, 0);
    }
    else
    {
      tran_table[i] = xcol.pixel;
    }
  }
}

/**
 * Part of the floodfill ??
 */
static int fill_line(XImage *image, int x, int y, int width, unsigned int area_color, unsigned int fill_color, int minx, int maxx, dir_type direction)
{
  register int left, right, i;

  XPutPixel(image, x, y, fill_color);
  for( left=x-1; (XGetPixel(image, left, y) == area_color) && (left >= 0);left--)
  {
    XPutPixel(image, left, y, fill_color);
  }
  left++;

  for (right = x+1; (XGetPixel(image, right, y) == area_color) && (right < width); right++)
  {
    XPutPixel(image, right, y, fill_color);
  }

  XDrawLine(dpy, drawable, fill_gc, left, y, right-1, y);

  for (i = left; i < right; i++)
  {
    if (XGetPixel(image, i, y+direction) == area_color)
    {
      /* Last one because we know there is one other pixel */
      i += fill_line(image, i, y+direction, width, area_color, fill_color, left, right, direction) + 1;
    }
  }

  if (left < minx-1) /* Fill other dir */
  {
    for (i = left; i < minx-1; i++)
    {
	    if (XGetPixel(image, i, y-direction) == area_color)
      {
        /* Last one because we know there is one other pixel */
	      i += fill_line(image, i, y-direction, width, area_color, fill_color, left, right, (direction==dir_down?dir_up:dir_down)) + 1;
      }
    }
  }

  if (right > maxx) /* Fill other dir */
  {
    for (i = maxx+1; i < right; i++)
    {
	    if (XGetPixel(image, i, y-direction) == area_color)
      {
        /* Last one because we know there is one other pixel */
	      i += fill_line(image, i, y-direction, width, area_color, fill_color, left, right, (direction==dir_down?dir_up:dir_down)) + 1;
      }
    }
  }
  return(right-x);
}

/**
 * Help function for Gi::FloodFill
 */
static void floodfill(int x, int y, int color)
{
  /* Not nice - should be more general */
  unsigned int width = maxx;
  XImage *winimage;
  XImage *pointimage;
  int area_color;

  // return; // Uncomment this to avoid floodfill

  pointimage = point2image( x, y );
  area_color = XGetPixel(pointimage, 0, 0);
  XDestroyImage(pointimage);
  if (color == area_color) return;

  winimage = xwin2image();
  fill_line(winimage, x, y, width, area_color, color, x, x, dir_down);
  XDestroyImage(winimage);
}

//--------------------------------------------------------------------------
//                             Keyboard
//--------------------------------------------------------------------------

int getch()
{
  if (dpy)
  {
    char buffer[100];
    XEvent xev;
    KeySym keysym;
    XComposeStatus compose;

    XFlush(dpy);
    // Wait for the next event
    // XWindowEvent(dpy, win, KeyPressMask, &xev);
    while(!XCheckWindowEvent(dpy, win, KeyPressMask, &xev))
    {
      gi.CheckWindowEvents();
    }
    // This can only be KeyPress because of the mask
    XLookupString((XKeyEvent *)&xev, buffer, sizeof(buffer), &keysym, &compose);
    switch (keysym)
    {
      case XK_Escape:
        return(ESC);
      case XK_Up:
        return(UP);
      case XK_Down:
        return(DOWN);
      default:
        return(keysym);
    }
  }
  else
  {
    return 0;
  }
}

int kbhit()
{
  XEvent xev;

  if (XCheckWindowEvent(dpy, win, KeyPressMask, &xev))
  {
    XPutBackEvent(dpy, &xev);
    return(1);
  }
  return(0);
}

//--------------------------------------------------------------------------
//                            Class Gi
//--------------------------------------------------------------------------

/**
 * Draw a straight line in the current drawing color from x0, y0
 * to x1, y1.  Those are feet; global SCALE is used to compute pixels.
 */
void Gi::DrawLine(double x0, double y0, double x1, double y1)
{
  XDrawLine(dpy, drawable, gc, xcon(x0), ycon(y0), xcon(x1), ycon(y1));
}

/**
 * Draw a filled rectangle in the current FILL color.
 * All values are in feet; global SCALE is used to compute pixels.
 *
 * @param ulx    upper left corner x coordinate
 * @param uly    upper left corner y coordinate
 * @param lrx    lower right corner x coordinate
 * @param lry    lower right corner y coordinate
 */
void Gi::Rectangle(double ulx,
                   double uly,
                   double lrx,
                   double lry)
{
  // XFillRectangle uses foreground color, but we want to fill with the
  // fill_color == background color. Therefor uses fill_gc
  XFillRectangle(dpy, drawable, fill_gc, xcon(ulx), ycon(uly),
		 xcon(lrx)-xcon(ulx), ycon(lry)-ycon(uly));
}

void Gi::DrawArc(double radius, double center_x, double center_y,
                double start_angle, double length)
{
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

  double dangle1 = (start_angle - PI/2) * 180 * 64 / PI;
  double dangle2 = (length) * 180 * 64 / PI;
 
  int angle1, angle2;
 
  assert(radius >= 0);
 
  angle1 = iround(dangle1);
  angle2 = iround(dangle2);
 
  int x = xcon(center_x-fabs(radius));
  int y = ycon(center_y+fabs(radius));
  int width = xcon(fabs(radius*2));
  int height = xcon(fabs(radius*2));
#if 0
  cout << "Drawing an arc inside the rectangle [" << x << "," << y;
  cout << "], width (" << width << ") height (" << height;
  cout << ") from (" << angle1/64 << ") length (" << angle2/64 <<")\n";
#endif
 
  XDrawArc(dpy, drawable, gc, x, y,
           width, height, angle1, angle2);
  //getch();
} 

void Gi::FloodFill(double X, double Y)
{
  XGCValues gcv;

  XGetGCValues(dpy, fill_gc, GCForeground, &gcv);

  floodfill(xcon(X), ycon(Y), gcv.foreground);
}

void Gi::TextOutput(double X, double Y, const char * source)
{
  XTextItem thistext;

  thistext.chars = (char *)source;
  thistext.nchars = strlen(source);
  thistext.delta = 0;
  thistext.font = fnt->fid;

  XDrawText(dpy, drawable, gc, xcon(X), ycon(Y)+fnt->ascent, &thistext, 1);
}

/**
 * Called before drawing to set the color that will be used.
 *
 * @param our_color  our_color can range from 0 through 15
 */
void Gi::SetColor(int our_color)
{
  XGCValues gcv;

  gcv.foreground = tran_table[our_color];
  XChangeGC(dpy, gc, GCForeground, &gcv);
}

/**
 * Called before flood_fill() and rectangle() to determine the fill color:
 */
void Gi::SetFillColor(int our_color)
{
  XGCValues gcv;

  gcv.foreground = tran_table[our_color];
  XChangeGC(dpy, fill_gc, GCForeground, &gcv);
}

/**
 * Check if new X11 event are arrived
 */
void Gi::CheckWindowEvents()
{
  int i;
  XEvent xev;
  // extern STAGE stage;
  //  extern int* order;
  int redrawn=0;
 
  while (XCheckWindowEvent(dpy, win, ExposureMask, &xev))
  {
    switch (xev.type)
    {
     case Expose:
       if (xev.xexpose.count > 0 || redrawn) // More exposures coming up:)
       {
         continue;
       }
 
       // We have requested backingstore, but if none such, and we have a
       // copy of the track, use that.
       // Redraw the whole she-bang :(
       XCopyArea(dpy, pixmap, win, gc, 0, 0, maxx, maxy, 0, 0); 
       draw.ScoreBoard(race_data.stage,0);
       draw.PrintLeaderboardHeader();
       for( i=0; i<args.m_iNumCar; i++ ) // Would like to know how many really...
       {
         draw.Leaders(i, race_data.m_aCarInPos);
       }
       // The cars will arrive shortly:)
       redrawn = 1;
       break;
     case KeyPress:
     case ButtonPress:
     default:
       // cout << "Unknown event " << xev.type << ".\n";
       break;
    }
  }
}

/**
 * Initializes the graphics sub-system, making it ready to draw.
 * Also determines the number of pixels on the screen and uses
 * that to set the SCALE global variable.
 */
void Gi::InitializeGraphics()
{
  XSetWindowAttributes xswa;
  XGCValues gcv;
 
  if (dpy == NULL)
  {
    /* Open access to local display */
    if ((dpy = XOpenDisplay (NULL)) == NULL)
    {
      printf ("XOpenDisplay returned NULL\n"); 
      exit (1); 
    }
  
    // Get some colors
    create_colors();
  
    /* Get size of root window. (Will be used to limit size of RARS window) */
    {
      Window w;
      int x,y;
      unsigned b,d;
    
      XGetGeometry(dpy,DefaultRootWindow(dpy),&w,&x,&y,&maxx,&maxy,&b,&d);
    }
    /* Make RARS window smaller than root window (need space for borders etc.) */
    maxx-=64;
    maxy-=48;
    // Don't make the RARS window larger than 960x720. We don't know the size of
    // the area visible on the monitor. It might be smaller than the root window.
    if (maxx>960)
        maxx=960;
    if (maxy>700)
        maxy=700;
    /* Aspect ratio of RARS window should be 4/3 */
    if (maxx>maxy*4/3)
        maxx=maxy*4/3;
    if (maxy>maxx*3/4)
        maxy=maxx*3/4;
    // uncomment this if you prefer to hardcode the size of the RARS window:
    // maxx=960;
    // maxy=720;
    fprintf( stderr, "XWindow size: maxx %d, maxy %d\n", maxx, maxy );
  
    /* These three used in XCreateWindow */
    /* Set the events you want */
    xswa.event_mask = VisibilityChangeMask|ExposureMask|KeyPressMask|ButtonPressMask;
    /* Set the background colors */
    xswa.background_pixel = tran_table[FIELD_COLOR];
    /* Set to store when overlapped? */
    xswa.backing_store = Always;
    /* Make window */
    win = XCreateWindow (dpy, DefaultRootWindow (dpy), 
                         0, 10, maxx, maxy, 2, 0, 
                         InputOutput, CopyFromParent, 
                         CWEventMask | CWBackPixel | CWBackingStore, 
                         & xswa);
    /* Set name of window */
    XStoreName (dpy, win, "RARS");
  
    /* Make a graphics context */
    gcv.function = GXcopy;
    gcv.plane_mask = AllPlanes;
    /* Set foreground color */
    gcv.foreground = BlackPixel(dpy,DefaultScreen(dpy));
    /* Set background color */
    gcv.background = WhitePixel(dpy,DefaultScreen(dpy));
    /* Set width of lines */
    gcv.line_width = 0;
    /* The edge of the track needs CapButt, but it's faster with CapNotLast. */
    gcv.cap_style = CapButt;
    /* Set fill and line style to solid */
    gcv.fill_style = FillSolid;
    gcv.line_style = LineSolid;
    /* Create the GC for solid lines */
    gc = XCreateGC (dpy, win, 
                    GCFunction | GCPlaneMask | GCForeground | GCBackground | 
                    GCLineWidth | GCFillStyle | GCLineStyle | GCCapStyle, 
                    & gcv);
    fill_gc = XCreateGC (dpy, win, 
                    GCFunction | GCPlaneMask | GCForeground | GCBackground | 
                    GCLineWidth | GCFillStyle | GCLineStyle | GCCapStyle, 
                    & gcv);
    /* Ready the window for drawing */
    XMapWindow (dpy, win);
    /* Tell X to display the window */
    XFlush(dpy);
    init_fonts();
    maxx = get_width();
    maxy = get_height();

    /*
     * Create pixmap.
     *  The pixmap is created after the display of the windows due that some
     *  window manager decides to change the size of the window 
     */
    pixmap = XCreatePixmap(dpy, win, maxx, maxy, DefaultDepth(dpy, XDefaultScreen(dpy)));
    drawable = pixmap;
  }
  // End of system dependent code.
  // The rest of this function is portable:
  // determine the distance in feet that each pixel will represent:
  SCALE = currentTrack->m_fXMax / (double)maxx;          // Either m_fXMax or m_fYMax will
  if(currentTrack->m_fYMax / (double)maxy > SCALE)       // determine the SCALE; The
  {
    SCALE = currentTrack->m_fYMax / (double)maxy;       // SCALE will be the smallest that
    currentTrack->m_fXMax = maxx * SCALE;               // will show both m_fXMax & m_fYMax.
  }
  else
  {
    currentTrack->m_fYMax = maxy * SCALE;
  }
  CHR_HGT = CHR_HGT_PIX * SCALE;      // compute character height in feet
  CHR_WID = CHR_WID_PIX * SCALE;      // compute character width in feet
}

void Gi::AfterGraphSetup()
{
  /* Set cars to be drawn with CapNotLast to speed up display */
  //XSetLineAttributes(dpy, gc, 0, LineSolid, CapNotLast, JoinMiter);
  XCopyArea(dpy, pixmap, win, gc, 0, 0, maxx, maxy, 0, 0);
  drawable = win;
}

/**
 * This just terminates graphics mode.
 */
void Gi::ResumeNormalDisplay()
{
  TRACE("Resuming Normal Display");
  if (dpy)
  {
   XFreeGC(dpy, gc);
   XFreeGC(dpy, fill_gc);
   XDestroyWindow(dpy, win);
   XFreePixmap(dpy, pixmap);
   XCloseDisplay(dpy);
   dpy = 0;
  }
}

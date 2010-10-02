/*
 * G_OPENGL.CPP (portable)
 *
 * This file contains several utility function for OpenGL.
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *  ver. 0.90 Mar 02 - class OpenGL
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>

#ifdef WIN32
  #include <windows.h>
  #include <GL/glaux.h>
#else
  // KDE for the font
  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include "../misc.h"
#include "g_opengl.h"
#include "../3ds/model_3ds.h"

#include "GL/glext.h"
#ifdef WIN32
  #define GLH_EXT_SINGLE_FILE
  #include "GL/glh_extensions.h"
  HDC  getHDC();
#else
  int wglAllocateMemoryNV = 0;
#endif

//--------------------------------------------------------------------------
//                              G L O B A L S
//--------------------------------------------------------------------------

OpenGL opengl;

//--------------------------------------------------------------------------
// OpenGL
//--------------------------------------------------------------------------

/**
 * Initialise OpenGL
 */
void OpenGL::Init( int iSizeX, int iSizeY, double max_dist )
{
  m_SizeX = iSizeX;
  m_SizeY = iSizeY;

  glClearColor(0.0f, 0.8f, 0.0f, 1.0f);
  glClearDepth(1.0);
  glDepthFunc(GL_LESS);
  // glShadeModel(GL_SMOOTH);

  glEnable(GL_DEPTH_TEST);
  // glEnable(GL_CULL_FACE);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

  // Slow down a lot the rendering
  // glEnable(GL_POLYGON_SMOOTH);

  // glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);
    
  glMatrixMode(GL_PROJECTION);
  GLfloat aspect = (GLfloat)m_SizeX/(GLfloat)m_SizeY;
  gluPerspective(45.0, aspect, 1.0, max_dist);
  glMatrixMode(GL_MODELVIEW);

  // If not set, textures are rendered incorrectly
  // But if removed (it seems to go slightly faster)
  glPixelStorei (GL_UNPACK_ALIGNMENT, 1);

  VideoMemoryInit();

  // Load the environment mapping texture
  g_TextureEnvMap = GLTexture::GetLoadedTexture( "", "env_mapping.bmp", false );
}

void OpenGL::Destroy()
{
  VideoMemoryDestroy();
}

//--------------------------------------------------------------------------
// Video Memory allocation
//--------------------------------------------------------------------------

// The frame rate on NVIDIA is slow on Windows. Because of their 
// implementation of the glDrawElements.
// By allocation Video Memory with their own propriatery API
// it is possible to achieve nearly increase the performance
// by 300%. (The vertex are copied in the video memory to dicrease
// the copy of the points)
//
// On Linux, the same problem does not exist (even without trick)
// it goes even 10-20 % faster than the optimized Windows version...

static char * video_mem = NULL;
static char * last_mem  = NULL;
const int BUFFER_SIZE = 1000000;

void OpenGL::VideoMemoryInit()
{
  #ifdef WIN32
  const float priority = 1.f; // Video Memory


  glh_init_extensions( "GL_ARB_multitexture GL_NV_vertex_array_range" );

  if( wglAllocateMemoryNV!=0 )
  {
    float megabytes = (BUFFER_SIZE * sizeof(GL_FLOAT)/1000000.f);
    video_mem = (char *)wglAllocateMemoryNV(BUFFER_SIZE*sizeof(GLfloat), 0, 0, priority);
    last_mem  = video_mem;

    if( video_mem==NULL )  
    {
      exitOnError( "Unable to allocate %f megabytes of fast memory. Giving up.", megabytes );
    }
    glVertexArrayRangeNV(BUFFER_SIZE*sizeof(GLfloat), video_mem);
    glEnableClientState(GL_VERTEX_ARRAY_RANGE_NV);
  }
  #endif

}

void * OpenGL::VideoMemoryAlloc( long size )
{
  if( wglAllocateMemoryNV!=0 )
  {
    char * mem = last_mem;
    last_mem += size;

    if( last_mem-video_mem>(int)(BUFFER_SIZE*sizeof(GLfloat)) )
    {
      exitOnError( "Unable to allocate %d bytes.", size );
    }
    return mem;
  }
  else
  {
    return malloc( size );
  }
}

void OpenGL::VideoMemoryFree( void * p )
{
  if( wglAllocateMemoryNV==0 )
  {
    free( p );
  }
}

void OpenGL::VideoMemoryDestroy()
{
  #ifdef WIN32
  if( video_mem!=NULL )
  {
    wglFreeMemoryNV( video_mem );
  }
  video_mem = NULL;
  #endif
}

//--------------------------------------------------------------------------
// Font
//--------------------------------------------------------------------------

/**
 * Build our bitmap font
 */
void OpenGL::BuildFont()
{
#ifdef WIN32

  HDC     hDC=NULL;                     // Private GDI Device Context
  HFONT font;                           // Windows Font ID
  hDC=getHDC();                         // get initialized HDC from GL_InitWidget in vc_lowgr.cpp

  int size = 21*m_SizeX/1000;
  font = CreateFont(  -size,            // height of font
                      0,                // width of font
                      0,                // angle of escapement
                      0,                // orientation angle
                      FW_BOLD,          // font weight
                      FALSE,            // italic
                      FALSE,            // underline
                      FALSE,            // strikeout
                      ANSI_CHARSET,     // character set identifier
                      OUT_TT_PRECIS,    // output precision
                      CLIP_DEFAULT_PRECIS,// clipping precision
                      ANTIALIASED_QUALITY,// output quality
                      FF_DONTCARE|DEFAULT_PITCH,    // family and pitch
                      "Arial");         // font name
  SelectObject(hDC, font);              // selects the font we want
  m_iSmallFont = glGenLists(96);        // storage for 96 characters
  wglUseFontBitmaps(hDC, 32, 96, m_iSmallFont); // builds 96 characters starting at character 32
  DeleteObject( font );

  size = 40*m_SizeX/1000;
  font = CreateFont(  -size,            // height of font
                      0,                // width of font
                      0,                // angle of escapement
                      0,                // orientation angle
                      FW_ULTRABOLD,     // font weight
                      FALSE,            // italic
                      FALSE,            // underline
                      FALSE,            // strikeout
                      ANSI_CHARSET,     // character set identifier
                      OUT_TT_PRECIS,    // output precision
                      CLIP_DEFAULT_PRECIS,// clipping precision
                      ANTIALIASED_QUALITY,// output quality
                      FF_DONTCARE|DEFAULT_PITCH,    // family and pitch
                      "Arial");   // font name
  SelectObject(hDC, font);              // selects the font we want
  m_iBigFont = glGenLists(96);          // storage for 96 characters
  wglUseFontBitmaps(hDC, 32, 96, m_iBigFont); // builds 96 characters starting at character 32
  DeleteObject( font );

#else

  Display *dpy;
  XFontStruct *fontInfo;  // storage for our font.

  // load the font.  what fonts any of you have is going
  // to be system dependent, but on my system they are
  // in /usr/X11R6/lib/X11/fonts/*, with fonts.alias and
  // fonts.dir explaining what fonts the .pcf.gz files
  // are.  in any case, one of these 2 fonts should be
  // on your system...or you won't see any text.

  // get the current display.  This opens a second
  // connection to the display in the DISPLAY environment
  // value, and will be around only long enough to load
  // the font.
  dpy = XOpenDisplay(NULL); // default to DISPLAY env.

  fontInfo = XLoadQueryFont(dpy, "-adobe-helvetica-bold-r-normal--18-*-*-*-p-*-iso8859-1");
  if (fontInfo == NULL)
  {
    fontInfo = XLoadQueryFont(dpy, "fixed");
    if (fontInfo == NULL)
    {
      printf("no X font available?\n");
    }
  }
  // after loading this font info, this would probably be the time
  // to rotate, scale, or otherwise twink your fonts.
  m_iSmallFont = glGenLists(96);                // storage for 96 characters.
  // start at character 32 (space), get 96 characters (a few characters past z), and
  // store them starting at base.
  glXUseXFont(fontInfo->fid, 32, 96, m_iSmallFont);
  // free that font's info now that we've got the
  // display lists.
  XFreeFont(dpy, fontInfo);

  fontInfo = XLoadQueryFont(dpy, "-adobe-helvetica-bold-r-normal--34-*-*-*-p-*-iso8859-1");
  if (fontInfo == NULL)
  {
    fontInfo = XLoadQueryFont(dpy, "fixed");
    if (fontInfo == NULL)
    {
      printf("no X font available?\n");
    }
  }
  m_iBigFont = glGenLists(96);                // storage for 96 characters.
  glXUseXFont(fontInfo->fid, 32, 96, m_iBigFont);
  XFreeFont(dpy, fontInfo);

  // close down the 2nd display connection.
  XCloseDisplay(dpy);

#endif // WIN32
}

/**
 * Delete the font
 */
void OpenGL::KillFont()
{
  glDeleteLists(m_iSmallFont, 96);            // delete all 96 characters
  glDeleteLists(m_iBigFont, 96);              // delete all 96 characters
}

/**
 * Custom GL "Print" routine
 *
 * @param similar to printf function
 */
void OpenGL::glPrint( int base, const char *fmt, ...)         // Custom GL "Print" routine
{
  char    text[256];                // holds our string
  va_list   ap;                   // pointer to list of arguments

  if (fmt == NULL)                  // if there's no text
    return;                     // do nothing

  va_start(ap, fmt);                  // parses the string for variables
  vsprintf(text, fmt, ap);            // and converts symbols to actual numbers
  va_end(ap);                     // results are stored in text

  glPushAttrib(GL_LIST_BIT);              // pushes the display list bits
  glListBase(base - 32);                  // sets the base character to 32
  glCallLists(strlen(text), GL_UNSIGNED_BYTE, text);  // draws the display list text
  glPopAttrib();                    // pops the display list bits
}


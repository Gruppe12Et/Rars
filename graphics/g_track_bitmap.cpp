/**
 * G_TRACK_BITMAP.CPP (portable)
 *
 * In 3D, this draw the bitmap of the track in the upper left on the screen
 * It uses TViewClassical to construct the bitmap
 *
 * History
 *  ver. 0.90 Creation
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#ifdef WIN32
  #include <stdafx.h>
  #include <GL/glaux.h>
#else
  // KDE for the font
  #include <X11/X.h>
  #include <X11/Xlib.h>
  #include <GL/glx.h>
#endif
#include <GL/gl.h>
#include <GL/glu.h>

#include "g_global.h"

//--------------------------------------------------------------------------
//                        Class TTrackBitmap
//--------------------------------------------------------------------------

/**
 * Constructor
 *
 * @param x, y : size of the view
 */
TTrackBitmap::TTrackBitmap( int x, int y ) : TViewClassical( x, y, true )
{
  FullScreenScale();

  // 1) Use TViewClassical to draw the track in 1 byte per pixel
  ClearBitmap(0);
  DrawRoad();
  FlipY();

  // 2) Get a bitmap 1 bit per pixel from the bitmap
  bitmap = GetOneBitPerPixel(0);
}

/**
 * Destructor
 */
TTrackBitmap::~TTrackBitmap()
{
  delete bitmap;
}

/**
 * Draw the bitmap of the track in OpenGL
 */
void TTrackBitmap::Draw()
{
  glRasterPos2i (0, 0);
  glBitmap (m_SizeX, m_SizeY, 0.0, 0.0, 0.0, 0.0, bitmap);
}
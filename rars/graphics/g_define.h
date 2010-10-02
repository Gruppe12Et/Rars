
//--------------------------------------------------------------------------
//
//    FILE: G_DEFINE.H (portable)
//
//    This file contains DEFINEs that are widely used.
//    ( + see INCLUDE )
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

#ifndef _g_define_H
#define _g_define_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

// Microsoft Visual C++
#ifdef WIN32
  #include "vc_defin.h"
#else
// Linux KDE
  #include "kde_defi.h"
#endif

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

   // these macros are already defined in some compilers
#ifndef max
   #define max(x,y)        (((x)<(y))?(y):(x))        // return maximum
   #define min(x,y)        (((x)>(y))?(y):(x))        // return minimum
#endif


#ifndef boolean
   #define boolean  int
   #define TRUE  1
   #define FALSE 0
#endif

   #define mod(x,y)        (((x)<(y))?(x):(x-y))      // simple mod

//--------------------------------------------------------------------------
   // carz.cpp : real_speed
   #define REAL_SPEED_SLOW   1
   #define REAL_SPEED_MEDIUM 0
   #define REAL_SPEED_FAST   2

   #define MODE_CLASSIC              0
   #define MODE_OPENGL_FULLSCREEN    1
   #define MODE_OPENGL_WINDOW        2
   #define MODE_TELEMETRY            3
   #define MODE_ZOOM                 4
   #define MODE_TRACK_EDITOR         5

   #define FOLLOW_MANUAL             0
   #define FOLLOW_ANY_OVERTAKING     1
   #define FOLLOW_FOR_POSITION       2
   #define FOLLOW_NOBODY             3

   #define IS_MODE_OPENGL(iMode)     (iMode==MODE_OPENGL_WINDOW||iMode==MODE_OPENGL_FULLSCREEN||iMode==MODE_TRACK_EDITOR)

#endif // _g_define_H



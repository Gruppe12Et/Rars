
//--------------------------------------------------------------------------
//
//    FILE: G_GLOBAL.H (portable)
//
//    This file contains definitions of EXTERN variables or functions.
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

#ifndef _G_GLOBAL_H
#define _G_GLOBAL_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include "g_define.h"
#include "../car.h"
#include "../track.h"
#include "../os.h"
#include "../draw.h"
#include "../instant_replay.h"
#include "g_main.h"
#include "g_view.h"
#include "g_rars_ini.h"
#include "g_opengl.h"

#ifndef max
#define max(x,y) ((x) > (y) ? (x) : (y))
#endif
#ifndef min
#define min(x,y) ((x) < (y) ? (x) : (y))
#endif
#ifndef sqr
#define sqr(x)   ((x)*(x))
#endif

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

extern TViewManager * g_ViewManager;
extern TPalette Palette;
extern bool g_bToRefresh;         // boolean used for the refresh function
extern RarsIni g_oRarsIni;
extern int g_iLastKey;
extern RACESTAGE g_RaceStage;

//--------------------------------------------------------------------------
//                         F U N C T I O N S
//--------------------------------------------------------------------------

// File: g_divers.cpp
void error( const char * error_text);
long time_get( void );
void fgetw ( char * s, FILE * f );
void format_time(double sec, char *buf);

// in kde or windows
bool gui_window_is_maximized();
void gui_window_set_maximized(bool bMaximized);

// port dependant functions
void GL_InitWidget(int x, int y, int offx, int offy);
void GL_DestroyWidget();
void GL_SwapWidget();

#endif // _G_GLOBAL_H


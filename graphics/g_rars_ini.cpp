/**
 * RARS_INI.CPP - Struture fo the initialization of the 
 *                starting gialog box 
 *
 * History
 *  ver. 0.xx Mar,96 - creation
 *  ver  0.76 Oct 00 - ccdoc
 *
 * This file follows a C++ Coding Standard : see help.htm
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include "g_define.h"
#include "car.h"
#include "g_global.h"

//--------------------------------------------------------------------------
//                             G L O B A L S
//--------------------------------------------------------------------------

RarsIni g_oRarsIni;

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Constructor
 *
 * Set the default values
 */
RarsIni::RarsIni()
{
  m_iLaps = 80;
  strcpy( m_sTrackName, "\0" );
  m_iViewMode = MODE_ZOOM;
  m_iViewOpenGL = 0;
  m_fZoom = 0.01;
  strcpy( m_sDrivers, "\0" );
  m_iFollowMode = FOLLOW_FOR_POSITION;
  m_iMaximized = 0;
  m_iRandom = 0;
  m_iSurface = 1;
  m_i3DS = 1;
  m_iShowNames = 1;

  // Load rars.ini
  load();
}

/**
 * Load the init file "rars.ini"
 */
void RarsIni::load()
{
  FILE * in;
  char * p;
  char line[10000], sKey[128], s[128];

  if( (in=fopen( "rars.ini", "r" )) == NULL ) 
  {
    chdir( ".." );
    if( (in=fopen( "rars.ini", "r" )) == NULL ) 
    {
      chdir( ".." );
      in=fopen( "rars.ini", "r" );
      if( (in=fopen( "rars.ini", "r" )) == NULL )
      {
        chdir( "/usr/share/rars" );
        in=fopen( "rars.ini", "r" );
      }
    }
  }
  
  if( in!=NULL ) 
  {
    while( !feof(in) ) 
    {
      if( fgets( line, 10000, in ) >0 )
      {
        switch( line[0] ) 
        {
          case '#': // comment
            break;
          default:
            p = strchr( line, '=' );
            if( p==NULL ) break;
            strncpy( sKey, line, p-(line) );
            sKey[p-(line)]=0;
            if( strcmp( sKey, "laps" )==0 ) 
            {
              sscanf(p+1,"%d", &m_iLaps );
            }
            else if( strcmp( sKey, "random" )==0 ) 
            {
              sscanf(p+1,"%d", &m_iRandom );
            }
            else if( strcmp( sKey, "surface" )==0 ) 
            {
              sscanf(p+1,"%d", &m_iSurface );
            }
            else if( strcmp( sKey, "track" )==0 )
            {
              sscanf(p+1,"%s", (char *)&m_sTrackName );
            }
            else if( strcmp( sKey, "view_mode" )==0 )
            {
              sscanf(p+1,"%s", (char *)&s );
              if( strcmp( s, "classic" )==0 )
              {
                m_iViewMode = MODE_CLASSIC;
              }
              else if( strcmp( s, "fullscreen" )==0 )
              {
                m_iViewMode = MODE_OPENGL_FULLSCREEN;
              } 
              else if( strcmp( s, "window" )==0 ) 
              {
                m_iViewMode = MODE_OPENGL_WINDOW;
              }
              else if( strcmp( s, "telemetry" )==0 ) 
              {
                m_iViewMode = MODE_TELEMETRY;
              }
              else
              {
                m_iViewMode = MODE_ZOOM;
              }
            }
            else if( strcmp( sKey, "drivers" )==0 )
            {
              sscanf(p+1,"%[^\n]s", (char *)&m_sDrivers );    //read all characters until next line
            }
            else if( strcmp( sKey, "follow_car" )==0 )
            {
              sscanf(p+1,"%s", (char *)&s );
              if( strcmp( s, "<position>" )==0 )
              {
                m_iFollowMode = FOLLOW_FOR_POSITION;
              }
              else if( strcmp( s, "<any>" )==0 )
              {
                m_iFollowMode = FOLLOW_ANY_OVERTAKING;
              }
              else
              {
                m_iFollowMode = FOLLOW_MANUAL;
               strcpy( m_sFollowCar, s );
              }
            }
            else if( strcmp( sKey, "maximized" )==0 )
            {
              sscanf(p+1,"%d", &m_iMaximized );
            }
            else if( strcmp( sKey, "view_opengl" )==0 )
            {
              sscanf(p+1,"%d", &m_iViewOpenGL );
            }
            else if( strcmp( sKey, "zoom" )==0 )
            {
              sscanf(p+1,"%lf", &m_fZoom );
            }
            else if( strcmp( sKey, "3ds" )==0 )
            {
              sscanf(p+1,"%d", &m_i3DS );
            }
            else if( strcmp( sKey, "show_names" )==0 )
            {
              sscanf(p+1,"%d", &m_iShowNames );
            }
            break;
        } 
      }  
    }   
    fclose( in );
  }  
}

/**
 * Write the init file "rars.ini"
 */
/**
  Syntax:

    track=abc.trk
    laps=100
    random=0
    surface=1
    view_mode=window | classic | fullscreen | telemetry | zoom
    view_opengl=7
    zoom=0.010000
    follow_car=<any> | <position> | car name
    maximized=1
    3ds=1
    show_names=1
    drivers=Djoefe,J++,K1999,Mafanja,O1,OscCar2
*/
void RarsIni::save()
{
  FILE * out;
  const char * s = "unknown";

  if( (out=fopen( "rars.ini", "w" )) != NULL )
  {
    fprintf( out, "##\n" );
    fprintf( out, "## Rars.ini\n" );
    fprintf( out, "## - Configuration file\n" );
    fprintf( out, "##\n" );
    fprintf( out, "track=%s\n", m_sTrackName );
    fprintf( out, "laps=%d\n", m_iLaps );
    fprintf( out, "random=%d\n", m_iRandom );
    fprintf( out, "surface=%d\n", m_iSurface );

    switch( m_iViewMode )
    {
      case MODE_CLASSIC:
        s = "classic"; break;
      case MODE_OPENGL_FULLSCREEN:
        s = "fullscreen"; break;
      case MODE_OPENGL_WINDOW:
        s = "window"; break;
      case MODE_TELEMETRY:
        s = "telemetry"; break;
      case MODE_ZOOM:
        s = "zoom"; break;
    }
    fprintf( out, "view_mode=%s\n", s );
    fprintf( out, "view_opengl=%d\n", m_iViewOpenGL );
    fprintf( out, "zoom=%f\n", (float)m_fZoom );

    switch( m_iFollowMode )
    {
      case FOLLOW_MANUAL:
        s = m_sFollowCar; break;
      case FOLLOW_FOR_POSITION:
        s = "<position>"; break;
      case FOLLOW_ANY_OVERTAKING:
        s = "<any>"; break;
    }
    fprintf( out, "follow_car=%s\n", s );
    fprintf( out, "maximized=%d\n", m_iMaximized );
    fprintf( out, "3ds=%d\n", m_i3DS );
    fprintf( out, "show_names=%d\n", m_iShowNames );

    if( strlen(m_sDrivers)>0 )
    {
      fprintf( out, "drivers=%s\n", m_sDrivers );
    }
    fprintf( out, "\n" );
    fclose( out );
  }
}

/**
 * Store the new data into the structure, then save it
 */
void RarsIni::save( const char * sTrackName, int iLaps, int iRandom, int iSurface, int iViewMode, int iFollowMode, const char * sFollowCar, const char * sDrivers )
{
  m_iLaps = iLaps;
  m_iRandom = iRandom;
  m_iSurface = iSurface;
  m_iViewMode = iViewMode;
  m_iFollowMode = iFollowMode;

  strcpy( m_sTrackName, sTrackName );
  strcpy( m_sFollowCar, sFollowCar );
  strcpy( m_sDrivers, sDrivers );

  save();
}

/**
 * Store current state of Rars the structure, then save it
 */
void RarsIni::SaveSettings()
{
  int pos = g_ViewManager->GetFollowCar();
  strcpy( m_sFollowCar, drivers[pos]->getName() );
  m_iFollowMode = g_ViewManager->m_iFollowMode;
  m_iViewMode = g_ViewManager->GetViewMode();
  if( IS_MODE_OPENGL(m_iViewMode) )
  {
    TView3D * view3d = g_ViewManager->GetOpenGlView();
    m_iViewOpenGL = view3d->m_iTypeView;
    m_fZoom = view3d->m_Zoom;
  }
  m_iShowNames = (g_ViewManager->m_bShowNames ? 1 : 0);
  SaveWindowSettings();

  save();
}


/**
 * Select the boolean bSelected in the drivers
 */
void RarsIni::DriverSelect()
{
  char sDriverName[64];
  int len = strlen( m_sDrivers );
  int car, iBegin = 0;

  for( car=0; car<MAX_CARS; car++ )
  {
    m_aDriverSelected[car] = false;
  }

  if( len>0 )
  {
    for( int i=0; i<len; i++ )
    {
      if( i==len-1 || m_sDrivers[i+1]==',' )
      {
        strncpy( sDriverName, m_sDrivers+iBegin, i-iBegin+1 );
        sDriverName[i-iBegin+1] = 0;
        car = 0;
        while( drivers[car]!=NULL )
        { 
          if( strcmp(sDriverName, drivers[car]->getName())==0 )
          {
             m_aDriverSelected[car] = true;
             break;
          }
          car ++;
        }
        iBegin = i+2;
      }
    }
  }
}

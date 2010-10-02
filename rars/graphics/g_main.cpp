/*
 * G_MAIN.CPP - Main program
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@skynet.be> 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */ 

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------
 
#include <string.h>
#include "../race_manager.h"
#include "../misc.h"
#include "g_global.h"

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

// arguments
char *        g_argv[50];
int           g_argc;

char argv0[20],  argv1[20],  argv2[20],
     argv3[20],  argv4[20],  argv5[20],
     argv6[20],  argv7[20],  argv8[20],
     argv9[20];

// holds the driver names during a movie playback
char MovieDriverNames[MAX_CARS][32];

// race
static int    g_iQualSes;
static int    g_iQualCar;
RaceManager   g_RaceManager;
RACESTAGE     g_RaceStage;

// ViewManager
TViewManager * g_ViewManager = NULL;

// Keyboard
int g_iLastKey;

////////////////////////////////////////////////////////////////////////////
// Main
////////////////////////////////////////////////////////////////////////////
 
/**
 * MAIN FUNCTION Splitted for GUI OS Windows/KDE
 * Part 1/3 : Before the race
 */
void main_part1()
{
  g_RaceManager.ArgsInit( g_argc, g_argv );
  g_RaceManager.AllInit();
  g_RaceStage = BEGIN;

  // No display -> run all the loops and exit
  if( !draw.m_bDisplay )
  {
    while( main_part2() ) {};
    main_part3();
    exit( 0 );
  }
}

/**
 * MAIN FUNCTION Splitted for GUI OS Windows/KDE
 * Part 2/3 : During the race
 */
bool main_part2()
{
  switch(g_RaceStage) 
  {
    case BEGIN:
      if (args.m_bQual) 
      {
        g_iQualSes = 0;
        g_iQualCar = 0;
        g_RaceManager.QualInit( g_iQualSes );
        g_ViewManager->FollowCar( g_RaceManager.QualInitLoop(g_iQualCar) );
        g_RaceStage = QUALIF;
      }
      else 
      {
        g_RaceStage = START_RACE;
      }
      break;
    case START_RACE:
      g_RaceManager.RaceInit(0);
      if( g_ViewManager!=NULL )
      {
        g_ViewManager->FollowCar( race_data.m_aCarInPos[0] );
        g_ViewManager->Reset();
        g_ViewManager->Refresh();
      }
      g_RaceStage = INIT_CARS;
      break;
    case QUALIF:
      if (!g_RaceManager.QualLoop(g_iQualCar)) 
      {
        g_iQualCar++;
        if( g_iQualCar==args.m_iNumCar ) 
        {
          g_RaceManager.QualClose(g_iQualSes);
          g_iQualCar = 0;
          g_iQualSes++;
          if (g_iQualSes == args.m_iNumQualSession) 
          {
            g_RaceStage = START_RACE;
            break;
          } 
          g_RaceManager.QualInit(g_iQualSes);
        }
        g_ViewManager->FollowCar( g_RaceManager.QualInitLoop(g_iQualCar) );
      }
      break;
    case INIT_CARS:
      {
        if (!g_RaceManager.RaceLoop()) 
        {
          g_RaceManager.RaceClose(0);
          g_RaceStage  = END;
          return FALSE;
        }
        g_RaceStage = INIT_CARS2;
      }
      break;
    case INIT_CARS2:
      // The initialising car dialog is removed during this step
      g_RaceStage = INRACE;
      break; //
    case INRACE:
      if (!g_RaceManager.RaceLoop()) 
      {
        g_RaceManager.RaceClose(0);
        g_RaceStage  = END;
        return FALSE;
      }
      break;
    case END:
      break;
  }  
  return TRUE;
}

/**
 * MAIN FUNCTION Splitted for GUI OS Windows/KDE
 * Part 3/3 : After the race
 */
void main_part3()
{
  if( g_RaceStage  == INRACE ) 
  {
    g_RaceManager.RaceClose(0);
  }
  g_RaceManager.AllClose();
}

/**
 * Build the arguments
 */
void build_args(
  int iCars,
  int iLaps,
  int iRandom,
  int iSurface,
  const char * sTrackName,
  const char * sMovie,
  const char * sOpenGL,
  int iRadioQualif,
  int iQualifLaps,
  int iQualifNumber,
  const char * sFollowCar,
  const char ** aDriver
)
{
  char s[512];
  char sDrivers[512];

  strcpy( argv0, "rars" );
  sprintf( argv1, "%d", iCars );
  sprintf( argv2, "%d", iLaps );
  strcpy( argv3, sTrackName );
  sprintf( argv4, "-s%d", iSurface );

  g_argv[0] = argv0;
  g_argv[1] = argv1;
  g_argv[2] = argv2;
  g_argv[3] = argv3;
  g_argv[4] = argv4;
  g_argc = 5;

  if( iRandom>0 )
  {
    sprintf( s, "-nr%d", iRandom );
    strcpy( argv5, s );
    g_argv[g_argc++] = argv5;
  }

  if( strcmp( sMovie, "<Record>")==0 )
  {
    // record movie
    strcpy(argv6, "-mrmovie");
    g_argv[g_argc++] = argv6;
  }
  else if( strcmp( sMovie, "<None>")!=0 )
  {
    // replay movie
    sprintf( s, "-mpmovies/%s", sMovie );
    strcpy(argv6, s);
    g_argv[g_argc++] = argv6;


    // Workaround for the movie playback bug
    // load the driver names from the movie file
    char MovieFile[100];
    strcpy(s, sMovie);
    char *tmp = strchr(s, '.');
    tmp[0] = '\0';
    sprintf(MovieFile, "movies/%s", s);

    Movie movie(MovieFile);

    // create dummy drivers for a replay
    int i;
    bool allDeleted = false;
    for( i=0; i<MAX_CARS; i++ )
    {
      if( !drivers[i] )
        allDeleted = true;
      if( !allDeleted )
        delete drivers[i];
      drivers[i] = new ReplayDriver();
    }
    movie.ReplayInit(&iCars, drivers);

    aDriver = (const char**) new char**[ iCars ];
    ReplayDriver* Dummy;
    for( i=0; i<iCars; i++ )
    {
      aDriver[i] = drivers[i]->getName();
      Dummy = (ReplayDriver*) drivers[i];
      Dummy->set2DBitmap();
    }
    // iCars was modified
    sprintf( argv1, "%d", iCars );

  }
  else if (iRadioQualif > 0)
  {
    // Movie disabled
    // qualif
    sprintf( s, "-ql%d", iQualifLaps);
    strcpy(argv6, s);
    sprintf( s, "-qs%d", iQualifNumber);
    strcpy(argv7, s);
    sprintf( s, "-qc%d", iCars);
    strcpy(argv8, s);
    sprintf( s, "-qf%d", iRadioQualif);
    strcpy(argv9, s);

    g_argv[g_argc++] = argv6;
    g_argv[g_argc++] = argv7;
    g_argv[g_argc++] = argv8;
    g_argv[g_argc++] = argv9;
  }

  strcpy( sDrivers, "\0" );
  if( iCars>0 )
  {
    g_argv[g_argc++] = "-d";
    g_argv[g_argc++] = (char *)aDriver[0];
    strcat( sDrivers, aDriver[0] );

    for( int i=1; i<iCars; i++ )
    {
      g_argv[g_argc++] = (char *)aDriver[i];
      strcat( sDrivers, "," );
      strcat( sDrivers, aDriver[i] );
    }
  }

  int iOpenGL = MODE_ZOOM;
  if( strcmp( sOpenGL, "Classic" )==0 )
  {
    iOpenGL = MODE_CLASSIC;
  }
  else if( strcmp( sOpenGL, "OpenGL Full Screen" )==0 )
  {
    iOpenGL = MODE_OPENGL_FULLSCREEN;
  }
  else if( strcmp( sOpenGL, "OpenGL Window" )==0 )
  {
    iOpenGL = MODE_OPENGL_WINDOW;
  }
  else if( strcmp( sOpenGL, "Telemetry" )==0 )
  {
    iOpenGL = MODE_TELEMETRY;
  }

  int iFollowMode = FOLLOW_MANUAL;
  if( strcmp( sFollowCar, "<For position>")==0 )
  {
    iFollowMode = FOLLOW_FOR_POSITION;
  }
  else if( strcmp( sFollowCar, "<Any overtaking>")==0 )
  {
    iFollowMode = FOLLOW_ANY_OVERTAKING;
  }

  // Save the parameter in the ini file.
  if( strcmp( sMovie, "<None>")==0 || strcmp( sMovie, "<Record>")==0 )
  {
    g_oRarsIni.save( sTrackName, iLaps, iRandom, iSurface, iOpenGL, iFollowMode, sFollowCar, sDrivers );
  }

  delete [] aDriver;
}


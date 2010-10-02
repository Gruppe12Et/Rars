/*
 * RaceManager.cpp - Definition of methods in class RaceManager
 * 
 * History
 *  ver. 0.84 03.07.2001 created
 *
 * @author    Carsten Kjaer
 * @see:      RaceManager.h
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <ostream.h>
#include <string.h>

#include "race_manager.h"

#include "car.h"
#include "misc.h"
#include "track.h"
#include "movie.h"
#include "os.h"
#include "draw.h"

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

Os       os;
Args     args;
RaceData race_data;

//--------------------------------------------------------------------------
//                          F U N C T I O N S
//--------------------------------------------------------------------------


/**
 * Destructor
 */
RaceManager::~RaceManager()
{
  // Free the drivers array
  int i=0;
  while( drivers[i]!=NULL )
  {
    delete drivers[i];
    drivers[i] = NULL;
    i++;
  }

  // Free the track
  if( currentTrack!=NULL )
  {
    delete currentTrack;
    currentTrack = NULL;
  }
}

/**
 * General initialization: only one time
 * See main()
 *
 * @param argc          (in) from main()
 * @param argv          (in) from main()
 */
void RaceManager::ArgsInit( int argc, char* argv[] )
{
  // Set the track, car_count, lap_count, and various options:
  args.GetArgs(argc, argv);
  
  // Initialize the global currentTrack
  currentTrack = new Track(args.m_aTracks[args.m_iCurrentTrack]);

  // Init deprecated variables
  deprecated_init_globals();

  // For compatibility reason with the old get_names
  int i=0;
  while( drivers[i]!=NULL )
  {
    drivers[i]->init(i);
    i++;
  }

  // Initialize the random variable generator:
  randomizer();
  
  // Write initial race data to report file:
  m_oReport.WriteBegin();
}

/**
 * General close: only one time
 * See main()
 */
void RaceManager::ArgsClose()
{
  m_oReport.WriteRam();
}

/**
 * Initialize a race (before each circuit)
 * See main()
 */
void RaceManager::AllInit()
{
  // No qualifications with replay:
  if( args.m_iMovieMode==MOVIE_PLAYBACK && args.m_bQual )
  {
    exitOnError("Qualification mode is not allowed while replaying movie!\nDisable qualifying!");
  }
  
  // Replace currentTrack with new
  delete currentTrack;
  currentTrack = new Track(args.m_aTracks[args.m_iCurrentTrack]);
  currentTrack->readTrackFastestLap();
  
  // Initialize graphics and draw the background:
  if(draw.m_bDisplay)
  {
    draw.GraphSetup( &m_oInstantReplay );
  }

  // Create the car objects:
  int i=0;
  while( drivers[i]!=NULL )
  {
    race_data.cars[i] = new Car(i);
    i++;
  }
}

/**
 * Initialize a race (before each session:practice, qualification or race )
 * See main()
 *
 * @param rl            (in) race counter
 */
void RaceManager::RaceInit(long rl)
{
  int i;
  long j;
  
  race_data.m_iNumCarFinished = 0;  // incremented by each car that finishes the race
  race_data.m_iNumCarOut = 0;   // incremented by each car that crashes
  race_data.m_fElapsedTime = 0.0;
  m_iEndExtraTime = 50;
  m_fPanelLastTime = 0.0;

  // Common init code to RaceInit and QualInit
  if(args.m_bPractice)
  {
    CommonInit( PRACTICE, rl );
  }
  else
  {
    CommonInit( RACING, rl );
  }

  // Open the data files for reading or writing movie (if necessary):
  if( args.m_iMovieMode!=MOVIE_NORMAL )
  {
    m_oMovie = new Movie( args.m_sMovieName );
  }
  // Initialize movie
  // + write or read race_data.m_aStartPos[] array
  // + write or read stage of racing
  if( args.m_iMovieMode==MOVIE_PLAYBACK )
  {
    m_oMovie->ReplayInit( &args.m_iNumCar, drivers);
    m_oMovie->ReplayRacers( args.m_iNumCar, race_data.m_aStartPos );
    m_oMovie->ReplayStage( &race_data.stage );
  }
  else if( args.m_iMovieMode==MOVIE_RECORD )
  {
    m_oMovie->RecordInit( args.m_iNumCar, drivers);
    m_oMovie->RecordRacers( args.m_iNumCar, race_data.m_aStartPos );
    m_oMovie->RecordStage( race_data.stage );
  }

  if( race_data.stage==RACING )
  {
    // race_data.lapFinishingTimes stores lap finishing times for all cars and all laps
    race_data.m_aLapFinishingTimes = new double *[args.m_iNumCar]; // setup the array:
    for (i=0;i<args.m_iNumCar;i++)
    {
      race_data.m_aLapFinishingTimes[i] = new double[args.m_iNumLap+1];
    }
    
    // Initialize race_data.lapFinishingTimes:
    for (i=0;i<args.m_iNumCar;i++)
    {
      for (j=0;j<args.m_iNumLap+1;j++)
      {
        race_data.m_aLapFinishingTimes[i][j] = 0.0;
      }
    }
  }
    
  // puts cars on starting grid, initialize their variables:
  ArrangeCars(); 
}

////////////////////////////////////////////////////////////////////////////
// Race loop functions
////////////////////////////////////////////////////////////////////////////


/**
 * Main loop of a race
 * See main()
 */
int RaceManager::RaceLoop()
{
  if( m_oInstantReplay.m_iMode==INSTANT_RECORD || m_oInstantReplay.m_iMode==INSTANT_INIT || !draw.m_bDisplay )
  {
    int res = NormalRaceLoop();
    m_oInstantReplay.Record();
    return res;
  }
  else
  { 
    m_oInstantReplay.Replay();
    // Exist only in Gui mode
    Keyboard();
    draw.RefreshFinishLine();
    return 1; // always continue
  }

}

/**
 * Real main loop of a race
 * called when the InstantReplay is not activated
 */
int RaceManager::NormalRaceLoop()
{
  int i;
  rel_state rel_state_vec[NEARBY_CARS];// contains 5 relative state vectors
  int tick_count = 0;                  // for dynamic simulation-speed variation

  //////// observe & control:
  for(i=0; i<args.m_iNumCar; i++)           // for each car:
  {
    race_data.cars[i]->Observe();      // compute its local situation
  }
  if( args.m_iMovieMode!=MOVIE_PLAYBACK ) // not possible if in replay mode
  {
    for(i=0; i<args.m_iNumCar; i++)         // for each car:
    { 
      race_data.cars[i]->CheckNearby(rel_state_vec); // report nearby cars

      if( !race_data.cars[i]->out )
      {
        RobotTimer.startTimer();
        race_data.cars[i]->Control();              // compute a control vector
        race_data.cars[i]->RobotTime += RobotTimer.stopTimer();
      }
      else
        race_data.cars[i]->Control();              // compute a control vector
    }
  }

  //////// move_car, check collisions:
  for(i=0; i<args.m_iNumCar; i++)             // for each car:
  {
    if( args.m_iMovieMode==MOVIE_PLAYBACK ) // imitate move_car()
      race_data.cars[i]->ReplayMovie( m_oMovie );
    else
      race_data.cars[i]->MoveCar();      // update state of car
  } 
  if( args.m_iMovieMode!=MOVIE_PLAYBACK )
  {
    for(i=0; i<args.m_iNumCar; i++)           // for each car:
    {
      race_data.cars[i]->CheckCollisions();   // check for collisions
      if( args.m_iMovieMode==MOVIE_RECORD )     // store some movie data
        race_data.cars[i]->RecordMovie( m_oMovie );
    }
  }

  if(draw.m_bDisplay && draw.m_iFastDisplay > -1)
  {
    draw.RefreshFinishLine();
    for(i=0; i<args.m_iNumCar; i++)                // for each car:
    {
      race_data.cars[i]->DrawCar();     // update screen image of car
    }
  }

  race_data.m_fElapsedTime += delta_time;  // Advance the simulated time.

  // Sort cars:
  Sortem();                                   // maintains the race_data.m_aCarInPos[] and race_data.m_aPosOfCar[] arrays

  draw.UpdateLeaderboard();

  // This section is required to update Instrument Panel:
  if(draw.m_bDisplay && draw.m_iCarShown >= 0)   // only if some driver is selected
  {
    if(race_data.m_fElapsedTime - m_fPanelLastTime > .5)// every tenth cycle
    {
      draw.Instruments();         // refresh data
      m_fPanelLastTime = race_data.m_fElapsedTime;     // reset counter
    }
  }

  if( Keyboard() )
  {
    return(0); // ESC key will end the race.
  }

  // check for race over:
  if ((race_data.m_iNumCarFinished + race_data.m_iNumCarOut >= args.m_iNumCar) && (--m_iEndExtraTime < 0))
  {
    return(0);
  }
  // also quit if lead car completes lap_count + 1 laps:
  if(race_data.cars[race_data.m_aCarInPos[0]]->Laps >= (race_data.stage==PRACTICE ? args.m_iNumPracticeLap : args.m_iNumLap) + 1)
  {
    return(0);
  }

  // movie
  if( args.m_iMovieMode==MOVIE_PLAYBACK ) // quit if there is no more data
  {
    if( m_oMovie->isReplayFinished() )
    {
      return(0);
    }
  }

  // This is where (maybe) we slow the race down to realistic speed:
  if( draw.m_iFastDisplay != -1 && draw.m_bDisplay )
  {
    if (++tick_count >= draw.m_iFastDisplay)
    {
      os.OneTick(0);     // wait here till the next clock tick
      tick_count = 0;
    }
  }

  return(1);                           // repeats race loop as long as (1) is returned
}

////////////////////////////////////////////////////////////////////////////
// Race closing functions
////////////////////////////////////////////////////////////////////////////

/**
 * Close a race (before each session:practice, qualification or race )
 * See main()
 *
 * @param rl            (in) race counter
 */
void RaceManager::RaceClose(long rl)
{
  if( race_data.stage==RACING ) 
  {
    m_oReport.WriteResults(rl+1, race_data.m_aCarInPos, race_data.cars);
    for (int i=0;i<args.m_iNumCar;i++)
    {
      delete [] race_data.m_aLapFinishingTimes[i];
    }
    delete [] race_data.m_aLapFinishingTimes;
  }
  if( race_data.stage==PRACTICE )
  {
    args.m_bPractice = 0;
  }
  else
  {
    currentTrack->writeTrackFastestLap(race_data.m_oLapRecord); // writes track record to file
  }
  
  // movie
  if( args.m_iMovieMode!=MOVIE_NORMAL )
  {
    delete m_oMovie;
  }
}

/**
 * Close all (executed only once)
 * See main()
 */
void RaceManager::AllClose()
{
  m_oReport.WriteFinal();

  // clean up, end graphics, back to normal
  int i=0;
  while( drivers[i]!=NULL )
  {
    delete race_data.cars[i];
    i++;
  }
  draw.ResumeNormalDisplay();
}

////////////////////////////////////////////////////////////////////////////
// Qualification functions
////////////////////////////////////////////////////////////////////////////

/**
 * Initialize a qualification session(for all cars)
 * See main()
 *
 * @param ql            (in) qualification session counter
 */
void RaceManager::QualInit(int ql)
{
  int i;
  draw.m_iLeaderboardMode = 0;
  
  // Common init code to RaceInit and QualInit
  CommonInit( QUALIFYING, ql );
    
  if( ql==0 )
  {
    for(i=0; i<args.m_iNumCar; i++)
    {
      race_data.cars[race_data.m_aStartPos[i]]->PutCar(0,0,0); //initializes car data
    }
  }
}

/**
 * Initialize a qualification session(for one cars)
 * See main()
 *
 * @param q             (in) qualification of the car "q"
 */
int RaceManager::QualInitLoop(int q)
{
  race_data.m_iNumCarFinished = 0;      //reset values
  race_data.m_iNumCarOut = 0;
  race_data.m_fElapsedTime = 0.0;
  m_fPanelLastTime = 0.0;
  m_iEndExtraTime = 220;
  
  // Draw track boundaries and pave track:
  if(draw.m_bDisplay)
  {
    draw.DrawTrack();
  }
  
  // Put the qualifying car on starting position:
  race_data.cars[race_data.m_aStartPos[q]]->PutCar((currentTrack->m_fLftStartX + currentTrack->m_fRgtStartX)/2,(currentTrack->m_fLftStartY + currentTrack->m_fRgtStartY)/2, currentTrack->m_fStartAng);
  
  // If data is watched, jump automatically to next car's data:
  if (draw.m_iCarShown >= 0)
  {
    draw.m_iCarShown = race_data.m_aStartPos[q];
  }

  return race_data.m_aStartPos[q];
}

/**
 * Main loop during the qualifications (for one cars)
 * See main()
 *
 * @param q             (in) qualification of the car "q"
 */
int RaceManager::QualLoop(int q)
{
  rel_state rel_state_vec[NEARBY_CARS];
  
  //////// observe & control:
  race_data.cars[race_data.m_aStartPos[q]]->Observe();          // compute its local situation
  race_data.cars[race_data.m_aStartPos[q]]->CheckNearby(rel_state_vec);
  race_data.cars[race_data.m_aStartPos[q]]->Control();          // compute a control vector
  race_data.cars[race_data.m_aStartPos[q]]->MoveCar();          // update state of car  
  
  if(draw.m_bDisplay && draw.m_iFastDisplay > -1)
  {
    draw.RefreshFinishLine();
    race_data.cars[race_data.m_aStartPos[q]]->DrawCar();        // update screen image of car
  }
  
  race_data.m_fElapsedTime += delta_time;            // Advance the simulated time.
  
  // Sort cars:
  // qsortem is called by observe() only at SF lane
  
  draw.UpdateLeaderboard();

  // Update the instrument panel:
  if(draw.m_bDisplay && draw.m_iCarShown >= 0)
  {
    if(race_data.m_fElapsedTime - m_fPanelLastTime > .25)  // every fifth cycle
    {
      draw.Instruments();
      m_fPanelLastTime = race_data.m_fElapsedTime;
    }
  }
  
  if( Keyboard() )
  {
    return(0); // ESC key will end the race.
  }

  // check for race over:
  if ((race_data.m_iNumCarFinished || race_data.m_iNumCarOut) && (--m_iEndExtraTime < 0))
  {
    return(0);
  }

  // This is where (maybe) we slow the race down to realistic speed:
  if( !draw.m_iFastDisplay && draw.m_bDisplay )
  {
    os.OneTick(0);                     // wait here till the next clock tick
  }

  return(1);
}

/**
 * Takes out cars that were too slow in qualifications
 */
void RaceManager::QualEvaluateResults()
{
  int i;
  for (i=0;i<args.m_iNumCar;i++)
  {
    if((args.m_iQualMode==QUAL_FASTEST_LAP && (race_data.cars[i]->q_bestlap <
      race_data.cars[race_data.m_aCarInPos[0]]->q_bestlap/Q_CUTOFF))
      || (args.m_iQualMode==QUAL_AVERAGE_SPEED && (race_data.cars[i]->q_avgspeed <
      race_data.cars[race_data.m_aCarInPos[0]]->q_avgspeed/Q_CUTOFF)))
    {
      race_data.cars[i]->Out = race_data.cars[i]->out = 3;  // did not qualify for race!
    }
  }

  m_oReport.WriteQualResults(race_data.m_aCarInPos, race_data.cars);

  for (i=0;i<args.m_iNumCar;i++)
  {
    if(race_data.cars[i]->out == 3) // required for sortem!
    {
      race_data.cars[i]->laps = -1;
      race_data.cars[i]->distance = -race_data.m_aPosOfCar[i]; // so they are not equal.
      race_data.cars[i]->Speed_avg = race_data.cars[i]->Speed_max = 0; // to show on leaderboard
    }
  }
}

/**
 * Close a qualification session (for all races )
 * See main()
 *
 * @param ql            (in) qualification session counter
 */
void RaceManager::QualClose(int ql)
{
  int i;
  
  if( ql==args.m_iNumQualSession-1 )  // last qual attempt
  {
    race_data.QSortem();              // Final sorting
    
    QualEvaluateResults();
    currentTrack->writeTrackFastestLap(race_data.m_oLapRecord); // writes track record to file
    for (i=0;i<args.m_iNumCar;i++)          // make starting grid (race_data.m_aStartPos[])
    {
      race_data.m_aStartPos[i] = race_data.m_aCarInPos[i];            // according to qual results
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// Race Cars Management Position,Initialisation...
////////////////////////////////////////////////////////////////////////////

/**
 * Arrange cars on starting positions
 */
void RaceManager::ArrangeCars()
{
  double start_pos;
  double d = 1.5*CARLEN;  // distance between cars
  double ang_toend;
  double R; // curve radius at cars position
  int i,N = currentTrack->m_iStartRows;
  double W, x = 0, y = 0, car_ang = 0;
  
  for(i=0; i<args.m_iNumCar; i++)
  {
    start_pos = currentTrack->length - (int(i/N)*d+i*d) - CARLEN/2;
    
    if (currentTrack->lftwall[1].radius < 0) // first car is starting inside next curve
    {
      W = currentTrack->width*((i%N)+1)/(N+1); // how far from right wall
    }
    else
    {
      W = currentTrack->width - (currentTrack->width*((i%N)+1)/(N+1));
    }
    
    //find starting segment for given car:
    for(int k=0; k<currentTrack->NSEG; k++)
    {
      if (start_pos > currentTrack->seg_dist[k])  // car is on segment k+1!
      {
        race_data.cars[race_data.m_aStartPos[i]]->seg_id = (k+1)%currentTrack->NSEG; // initialize seg_id!
        if(currentTrack->lftwall[(k+1)%currentTrack->NSEG].radius>0)      // in left curve
        {
          ang_toend = 2*(currentTrack->seg_dist[k+1]-start_pos)/
            (currentTrack->lftwall[k+1].radius+currentTrack->rgtwall[k+1].radius);
          R = currentTrack->rgtwall[k+1].radius - W;
          x = currentTrack->lftwall[k+1].cen_x - R*sin(ang_toend-currentTrack->lftwall[k+1].end_ang);
          y = currentTrack->lftwall[k+1].cen_y - R*cos(ang_toend-currentTrack->lftwall[k+1].end_ang);
          car_ang = currentTrack->lftwall[k+1].end_ang - ang_toend;
        }
        else if(currentTrack->lftwall[(k+1)%currentTrack->NSEG].radius<0) // in right curve
        {
          ang_toend = -2*(currentTrack->seg_dist[k+1]-start_pos)/
                      (currentTrack->lftwall[k+1].radius+currentTrack->rgtwall[k+1].radius);
          R = fabs(currentTrack->rgtwall[k+1].radius) + W;
          x = currentTrack->lftwall[k+1].cen_x - R*sin(ang_toend+currentTrack->lftwall[k+1].end_ang);
          y = currentTrack->lftwall[k+1].cen_y + R*cos(ang_toend+currentTrack->lftwall[k+1].end_ang);
          car_ang = ang_toend + currentTrack->lftwall[k+1].end_ang;
        }
        else // on straight
        {
          x = currentTrack->rgtwall[(k+1)%currentTrack->NSEG].beg_x +
            (start_pos - currentTrack->seg_dist[k]) * cos(currentTrack->lftwall[(k+1)%currentTrack->NSEG].beg_ang)
            - W * sin(currentTrack->lftwall[(k+1)%currentTrack->NSEG].beg_ang);
          y = currentTrack->rgtwall[(k+1)%currentTrack->NSEG].beg_y +
            (start_pos - currentTrack->seg_dist[k]) * sin(currentTrack->lftwall[(k+1)%currentTrack->NSEG].beg_ang)
            + W * cos(currentTrack->lftwall[(k+1)%currentTrack->NSEG].beg_ang);
          car_ang = currentTrack->rgtwall[(k+1)%currentTrack->NSEG].beg_ang;
        }
        continue;
      }
    }
    // Put cars on computed starting positions and initialize 
    // their variables. Save starting position
    // Mark cars that did not qualify for the race 
    if (race_data.cars[race_data.m_aStartPos[i]]->Out != 3)
    {
      race_data.cars[race_data.m_aStartPos[i]]->PutCar(x,y,car_ang);
    }
    else
    {
      ++race_data.m_iNumCarOut;
    }
    race_data.cars[race_data.m_aStartPos[i]]->started = race_data.cars[race_data.m_aStartPos[i]]->Started = i;
  }
}

/**
 * Sort the array race_data.m_aCarInPos[] array which has the position of each car.
 */
void RaceManager::Sortem()
{
  int i, temp;
  int old_order[MAX_CARS];
  
  for(i=0; i<args.m_iNumCar; i++)     // copy first part of race_data.m_aCarInPos[] array
  {
    old_order[i] = race_data.m_aCarInPos[i];
  }
  
  for(i=0; i<args.m_iNumCar-1; i++)
  {
    if(race_data.cars[race_data.m_aCarInPos[i]]->Out != 3 && !race_data.cars[race_data.m_aCarInPos[i]]->Done)
    {
      // the while loop below does not usually repeat:
      while(race_data.cars[race_data.m_aCarInPos[i+1]]->Farther(race_data.cars[race_data.m_aCarInPos[i]]))
      {
        // When a car passes another, we swap positions:
        temp = race_data.m_aCarInPos[i];
        race_data.m_aCarInPos[i] = race_data.m_aCarInPos[i+1];
        race_data.m_aPosOfCar[race_data.m_aCarInPos[i+1]] = i;
        race_data.m_aCarInPos[i+1] = temp;
        race_data.m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
        {
          --i;
        }
      }
    }
  }
    
  if(draw.m_bDisplay)
  {
    draw.CalcNewData( old_order );
  }
}

/**
 * CommonInit, common part to RaceInit and QualInit 
 *
 * @param rl            (in) race/qualif counter
 */
void RaceManager::CommonInit( Stage stage, int rl )
{
  race_data.stage = stage;

  race_data.m_oLapRecord.speed = 0.0;
  
  // (X11) Draw track boundaries and pave track:
  if(draw.m_bDisplay)
  {
    draw.DrawTrack();
  }

  // When zero cars are requested, we only show the track.
  if(args.m_iNumCar==0)
  {
    if(draw.m_bDisplay) 
    {
      os.GetCh();
      draw.ResumeNormalDisplay();
    }
    exitOnError("Zero cars were requested.");
  }

  // InstantReplay
  m_oInstantReplay.Init();

  // Initialize the clock (X11)
  os.OneTick(1);

  if( stage!=QUALIFYING || rl==0 )
  {
    // Decide on the starting positions:
    if( stage==QUALIFYING || args.m_bPractice || !args.m_bQual) //qualifying sets the race_data.m_aStartPos[]
    { 
      race_data.OrderStartPos( rl );
    }

    // Initialize race_data.m_aCarInPos[] and race_data.m_aPosOfCar[]
    // race_data.m_aCarInPos[] and race_data.m_aPosOfCar[] are reverse arrays
    for( int i=0; i<args.m_iNumCar; i++)
    {
      race_data.m_aCarInPos[i] = race_data.m_aStartPos[i];// must correspond with initial positions
      race_data.m_aPosOfCar[race_data.m_aStartPos[i]] = i; // reverse array of race_data.m_aCarInPos
    }
    draw.InitNewData();
  }

  // BEGIN THE RACE!
  if(!draw.m_bDisplay)
  {
    if( race_data.stage==QUALIFYING )
    {
      cout << "Beginning qualifying session " << (rl+1);
      cout << " on " << currentTrack->m_sFileName;
    }
    else if( race_data.stage==PRACTICE )
    {
      cout << "Practice session for " << args.m_iNumPracticeLap << " laps";
      cout << " on " << currentTrack->m_sFileName;
    }
    else if( race_data.stage==RACING )
    {
      cout << "Beginning race " << (rl+1) << " on ";
      cout << currentTrack->m_sFileName;
    }
    cout << endl;
  }

  // (X11) Instrument panel and leaderboard:
  if(draw.m_bDisplay)
  {
    draw.m_iCarShown = -1;   // the instrument panel is initially inactive
    draw.Instruments();      // erase instruments from prev race
    draw.PrintLeaderboardHeader();
    draw.UpdateLeaderboard();
  }

  // Init deprecated variables
  deprecated_init_globals();
}

/**
 * MOVIE.CPP - Record and replay a movie
 *
 * There are always two functions: Record...() will record what happens, and
 * Replay...() will replay it on demand.
 *
 * If a short int is not a 16bit-integer on your machine, please correct
 * the typedef below so that a 16bit-integer type will be used.
 *
 * Here is how it works:
 * The x and y values can be interpolated nicely, so that's what I do.
 * x and y are recorded twice a second. You can change this with
 * XYPause. It determines how many times sampling will be suppressed
 * in between two samplings. Each twentieth of these samplings will
 * be a full sampling (16 bit), the others will be delta samples (8 bit).
 * This can be adjusted with FullSample. It determines how many delta
 * samples will be taken in between two full samples. To get the
 * values between the two samples, an interpolation is used.
 *
 * The angle can change rapidly, thus it's not such a good candidate
 * for interpolation. Due to this, it's recorded six times a second,
 * and it will stay constant from one sample to the next. This can
 * be changed with AnglePause. The angles are stored as 8 bit, meaning
 * fractions of 2 Pi. (1 equals 1/256 of 2 Pi).
 *
 * Since the angles and positions are highly interleaved, and since 
 * it's necessary to pre-read positions for the interpolation, I decided
 * to create two data files: movie.xy for the coordinates and
 * movie.ang for the angles.
 *
 * History
 *  ver. 0.6  May 96 
 *  ver. 0.76 Aug 98 - CCDOC
 *
 * @author    Nicole Greiber
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <iostream.h>
#include <fstream.h>
#include <stdlib.h>
#include <string.h>
#include "car.h"
#include "track.h"             // trackfile
#include "movie.h"
#include "misc.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

// so many data sets will be skipped in between two recordings
const unsigned char XYPause = 8;       // sample at 2 Hz = 18 / (XYPause+1)
const unsigned char FullSample = 9;    //how many deltas between 2 full records
const unsigned char AnglePause = 8;    // sample at 2 Hz
const double AngleScale = (2 * PI) / 256.0;

/** interpolation matrix */
const signed char BM[4][4]={{ 0, 2, 0, 0},
                            {-1, 0, 1, 0},
                            { 2,-5, 4,-1},
                            {-1, 3,-3, 1}};

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

typedef short INT16;           // put a signed 16-bit-type here

struct MovieData
{
  unsigned char NoRecord;      // used for recording and replay
  unsigned char NoAngle;       // used for recording and replay
  unsigned char FullCount;     // counts up to FullSample
  double OldX;                 // used for replay
  double OldY;                 // used for replay
};

/**
 * This struct is stored in the driver's RAM area 
 */
struct ReplayData 
{
  double x[4];                 // the four x and y values for interpolation
  double y[4];
  double t;                    // for interpolation, t is in [0.0, ..., 1.0]
  unsigned char PreloadCount;  // to handle data preloading
};

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

static MovieData MD[MAX_CARS];   // this should be in the car class

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Interpolate between 2 points x, y ??
 *
 * @param d        is the vector that gives the values for interpolation.
 * @param t        determines where to interpolate (t=0: d[1], t=1: d[2])
 * @return         ??
 */
static double Interpolate( double d[4], double t )
{
  double a[4];

  for( char i = 0; i < 4; i++ )
  {
    a[i] = BM[i][0]*d[0] + BM[i][1]*d[1] 
         + BM[i][2]*d[2] + BM[i][3]*d[3];
    a[i] *= 0.5;
  }
  return ((a[3]*t+a[2])*t+a[1])*t+a[0];  // Horner's rule
}

/**
 * Put a INT16 in the ostream
 *
 * @param s        (in) the stream
 * @param x        (in) the INT16 to put in the stream
 */
static void INT16Put(ostream& s, INT16 x)
{
  // write the upper byte, then the lower byte
  unsigned char tmp;
  tmp = ( x >> 8 ) & 0xFF;
  s.put( tmp );
  tmp = x & 0xFF;
  s.put( tmp );
}

/**
 * Get a INT16 in the ostream
 *
 * @param s        (in) the stream
 * @param x        (out) the INT16 readed from the stream
 */
static void INT16Get(istream& s, INT16& x)
{
  // read the upper byte
  unsigned char tmp;
  tmp = s.get();
  x = (tmp << 8) & 0xFF00;
  // then the lower byte
  tmp = s.get();
  x |= (tmp & 0xFF);
}

//--------------------------------------------------------------------------
//                             CLASS Car
//--------------------------------------------------------------------------

/**
 * Record the path of a car
 *
 * @param oMovie:  (in) pointer to the current movie
 */
void Car::RecordMovie( Movie * oMovie )
{
  signed char delta;

  if( MD[which].NoAngle == 100 ) // recording is starting
  {
    MD[which].NoAngle = 0;       // reset flag
    // write the full x and y values:
    INT16Put(oMovie->m_fileXY, iround(x) );
    INT16Put(oMovie->m_fileXY, iround(y) );
    MD[which].FullCount = 0;
    MD[which].OldX = x; MD[which].OldY = y;
  }
  // and now, write the angle:
  if( MD[which].NoAngle )
  {
    MD[which].NoAngle--;
  } 
  else 
  {
    MD[which].NoAngle = AnglePause;
    // the following will normalize and scale ang+alpha:
    oMovie->m_fileAng.put( (signed char)iround((ang+alpha) / AngleScale) );
  }

  if( MD[which].NoRecord ) // it's not time to sample
  {
    MD[which].NoRecord--;      // nothing else to do
  } 
  else  // write a delta set
  {  
    MD[which].NoRecord = XYPause;
    if( MD[which].FullCount == FullSample )   // time for a full sample  
    {
      MD[which].FullCount = 0;  // reset counter
      if( out )
      {
        INT16Put(oMovie->m_fileXY, iround(x) );
        INT16Put(oMovie->m_fileXY, iround(y) );
      } 
      else 
      {
        INT16Put(oMovie->m_fileXY, iround(x) );
        INT16Put(oMovie->m_fileXY, iround(y) );
        MD[which].OldX = x; MD[which].OldY = y;
      }
    } 
    else  // delta recording
    {
      MD[which].FullCount++;
      if( out ) // car is no longer in the race
      {
        oMovie->m_fileXY.put( char(0) );  // delta_x
        oMovie->m_fileXY.put( char(0) );  // delta_y
      }
      else
      {
        // otherwise, write delta_x and delta_y
        delta = iround(x - MD[which].OldX);
        oMovie->m_fileXY.put( delta );
        delta = iround(y - MD[which].OldY);
        oMovie->m_fileXY.put( delta );
        MD[which].OldX = x; MD[which].OldY = y;
      }
    }
  }
  return;
}

/**
 * Replay the path of a car
 *
 * @param oMovie:  (in) pointer to the current movie
 */
void Car::ReplayMovie( Movie * oMovie )
{
  signed char ang256, delta;
  ReplayData *RD = (ReplayData*)data_ptr;
  // get access to the robot's data area
  INT16 inttmp;

  if( starting )                        // beginning of race for this car!
  {
    starting = 0;                       // reset the flag since nobody else does this
    RD->PreloadCount = 2;               // load 2 more values into the array
    out = 0;                            // car can't be out at startup
    MD[which].FullCount = 0;            // reset counter
    INT16Get(oMovie->m_fileXY, inttmp); // read the first value: full x
    x = RD->x[0] = RD->x[1] = RD->x[2] = RD->x[3] = inttmp;
    INT16Get(oMovie->m_fileXY, inttmp); // second value: full y
    y = RD->y[0] = RD->y[1] = RD->y[2] = RD->y[3] = inttmp;
    ang256 = oMovie->m_fileAng.get();   // first angle is needed
    ang = ang256 * AngleScale;
    alpha = 0;                          // necessary because ang already contains ang+alpha
    RD->t = 0.0;                        // init the interpolation counter
    return;                             // enough for startup
  }

  if( RD->PreloadCount )
  {
    RD->PreloadCount--;   // decrease counter
    // shift the data
    RD->x[0] = RD->x[1];
    RD->x[1] = RD->x[2];
    RD->x[2] = RD->x[3];
    RD->y[0] = RD->y[1];
    RD->y[1] = RD->y[2];
    RD->y[2] = RD->y[3];

    if( MD[which].FullCount == FullSample ) // read full samples
    {
      INT16Get(oMovie->m_fileXY, inttmp);
      RD->x[3] = inttmp;
      INT16Get(oMovie->m_fileXY, inttmp);
      RD->y[3] = inttmp;
      MD[which].FullCount = 0;
    } 
    else                                    // read delta samples
    {
      delta = oMovie->m_fileXY.get();
      RD->x[3] += delta;
      delta = oMovie->m_fileXY.get();       // read the new delta_x and delta_y
      RD->y[3] += delta;
      MD[which].FullCount++;
    }
  }

  out = 0;  // driver is still running
  // interpolate and set the data for the driver
  if( ! RD->t )   // we can use "real" data
  {
    x = RD->x[1];
    y = RD->y[1];
  }     
  else  // interpolate
  {
    x = Interpolate( RD->x, RD->t );
    y = Interpolate( RD->y, RD->t );
  }

  // check whether we need a new angle from the FIFO
  if( MD[which].NoAngle )
  {
    MD[which].NoAngle--;
  } 
  else 
  {
    MD[which].NoAngle = AnglePause;
    ang256 = oMovie->m_fileAng.get();  // first angle is needed
    ang = ang256 * AngleScale;
  }

  RD->t += 1 / double( XYPause + 1 );
  if( RD->t > 0.95 )  // be aware of rounding errors
  {
    RD->t = 0.0;
    RD->PreloadCount++; // load next data set
  }
}

//--------------------------------------------------------------------------
//                             CLASS Movie
//--------------------------------------------------------------------------

/**
 * Constructor
 *
 * @param sMovieName (in) name of the movie
 */
Movie::Movie( const char * sMovieName )
{
  char sFileName[80];

  strcpy(sFileName,sMovieName);
  strcat(sFileName,".xy");
  m_fileXY.open( sFileName, ios::binary | ((args.m_iMovieMode==MOVIE_RECORD) ?
						ios::out|ios::trunc : ios::in) );
  strcpy(sFileName,sMovieName);
  strcat(sFileName,".ang");
  m_fileAng.open( sFileName, ios::binary | ((args.m_iMovieMode==MOVIE_RECORD) ?
						ios::out|ios::trunc : ios::in) );
}

/**
 * Destructor
 */
Movie::~Movie()
{
  m_fileXY.close();
  m_fileAng.close();
}


/**
 * Return the number of drivers in the movie file and their names.
 *
 * @param DriverNames (out) Names of the drivers.
 * @return number of the drivers
 */
int Movie::getDriverNames(char DriverNames[MAX_CARS][32])
{
  int count; 
  char buf[25];
  int nose, tail;

  m_fileXY >> count;
  m_fileXY >> buf;  // trackfile name

  for( char i = 0; i < count; i++ )
  {
    m_fileXY.width(24);
    m_fileXY >> buf;
    strcpy(DriverNames[i], buf);    
    m_fileXY >> nose >> tail;
  }
  return count;
}


/**
 * Write the header of the movie.
 *
 * @param car_count (in) Number of car in the race.
 * @param drivers   (in) An array with all the drivers
 */
void Movie::RecordInit( int car_count, Driver *drivers[] )
{
  track_desc track = get_track_description();
  m_fileXY << car_count << " " << track.sName << endl;
  Driver * driver;
  for( char i = 0; i < car_count; i++ )
  {
    MD[i].NoRecord = XYPause; // reset the delay array
    MD[i].NoAngle = 100;
    driver = drivers[i];
    m_fileXY << driver->getName() << " " << driver->getNoseColor()
             << " " << driver->getTailColor() << endl;
  }
}

/**
 * Read the header of the movie.
 *
 * @param car_count (out) Number of car in the race.
 * @param drivers   (out) An array with all the drivers (names only)
 */
void Movie::ReplayInit( int *car_count, Driver * drivers[] )
{
  int tmp; 
  char buf[25];
  Driver * driver;

  m_fileXY >> tmp;
  *car_count = tmp;
  m_fileXY >> buf;  // trackfile name

  for( char i = 0; i < tmp; i++ )
  {
    MD[i].NoRecord = XYPause; // reset the delay array
    MD[i].NoAngle = 0;
    driver = drivers[i];
    m_fileXY.width(24);
    m_fileXY >> buf;
    driver->setName( buf );
    int nose, tail;
    m_fileXY >> nose >> tail;
    driver->setNoseColor( nose );
    driver->setTailColor( tail );
  }
}

/**
 * Write the racers array.
 *
 * @param car_count (in) number of cars
 * @param racers    (in) array of racers
 */
void Movie::RecordRacers( int car_count, int *racers )
{
  for( int i = 0; i < car_count; i++ )
  {
    m_fileXY << racers[i] << " ";
  }
  m_fileXY << endl;
}

/**
 * Read the racers array.
 *
 * @param car_count (in) number of cars
 * @param racers    (out) array of racers
 */
void Movie::ReplayRacers( int car_count, int *racers )
{
  for( int i = 0; i < car_count; i++ )
  {
    m_fileXY >> racers[i];
  }
}

/**
 * This should be the last output before the race data
 * No CR or LF here!!
 *
 * @param stage     (in) stage
 */
void Movie::RecordStage( Stage stage )
{
  m_fileXY << (unsigned char)stage;
}

/**
 * Read the stage of the race
 *
 * @param stage     (out) stage
 */
void Movie::ReplayStage( Stage *stage )
{
  unsigned char tmp;
  m_fileXY >> tmp;
  *stage = (Stage)tmp;
}

/**
 * In replay mode, return if the movie is finished
 *
 * @return true if the movie is finished
 */
bool Movie::isReplayFinished()
{
  return( m_fileXY.eof() || m_fileAng.eof() );
}





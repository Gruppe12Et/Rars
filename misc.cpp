/**
 * MISC.CPP - miscellaneous support functions
 *
 * History
 *  ver. 0.1  May 1995 
 *  ver. 0.6b 5/8/95 b for beta 
 *  ver. 0.61 May 26 
 *  ver. 0.76 Oct 2000 - CCDOC
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------
 
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>
#include <math.h>
#include <iostream.h>
#include <stdio.h>
#include "car.h"
#include "os.h"
#include "track.h"
#include "movie.h"
#include "draw.h"

#ifdef WIN32
  #include "windows.h"
#endif

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

static const double w = .5 * CARWID;   // half-width
static const double l = .5 * CARLEN;   // half-length 

// Random Variable Generation
#define MULTIPLIER      0x015a4e35L
#define INCREMENT       1

//--------------------------------------------------------------------------
//                            G L O B A L S
//--------------------------------------------------------------------------

// Random Variable Generation
static long coreSeed = 1;
static long externalSeed = 1;

// Error handling
#ifdef WIN32
  extern void StopAllTimer();
#endif


//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Random Variable Generation
////////////////////////////////////////////////////////////////////////////

/**
 * Random Variable Generation
 * This one is for RARS core
 *
 * @return  a random number
 */
int coreRand()
{
  coreSeed = MULTIPLIER * coreSeed + INCREMENT;
  return((int)(coreSeed >> 16) & 0x7FFF);
}

/**
 * Random Variable Generation
 * Robots and other functions can use this one
 *
 * @return  a random number
 */
int r_rand()
{ 
  externalSeed = MULTIPLIER * externalSeed + INCREMENT; 
  return((int)(externalSeed >> 16) & 0x7FFF);
} 

/**
 * Command line processing in OS.CPP can either call this with input of 0,
 * or a user's entry, or not call it. 0 input means really randomize.
 * If not called at all, the default seed of 1 is used.
 *
 * @param input         (in) 0 or or user entry
 */
void seedRandomFunctions(long input)
{
  if(input)                  // This is when the user gives the seed
  {
    coreSeed = input;
  }
  else                       // this is when the user wants to randomize
  {
    coreSeed = os.PickRandom(); // get a random no. from the op. sys.
  }
  race_data.m_iInitialSeed = coreSeed;     // this won't change again.
  externalSeed = (long)coreRand();
}

/**
 * Randomly set the random variable generator, but only if rndmiz is set:
 * 
 */
void randomizer()
{
  if(args.m_bRndmiz)
  {
    seedRandomFunctions(0);
  }
  else if(args.m_iSeed)
  {
    seedRandomFunctions(args.m_iSeed);
  }
}

////////////////////////////////////////////////////////////////////////////
// Mathematics functions
////////////////////////////////////////////////////////////////////////////

/**
 * Convert double to int by rounding
 *
 * @param given   double
 * @return        rounded value
 */
int iround(double given)
{
  if( given>0.0)
  {
    return int( given+.5 );
  }
  else
  {
    return int( given-.5 );
  }
}

/**
 * Sqrt of sum of squares (used for vector magnitude)
 *
 * @param x             (in) x
 * @param y             (in) y
 * @return              magnitude
 */
double vec_mag(double x, double y)
{
  return sqrt(x * x + y * y); 
} 

////////////////////////////////////////////////////////////////////////////
// Errors functions
////////////////////////////////////////////////////////////////////////////

/*
 * Generalized error function. It can be called the same way that printf() can.
 *
 * @format format with the same meaning as in printf
 */
void exitOnError(const char *format, ...)
{
  char s[1024];

  va_list argList;
  
  va_start(argList, format);
  vsprintf( s, format, argList);
  va_end(argList);
  
  #ifdef WIN32
    StopAllTimer();
    MessageBox( NULL, s, "Exit on Error", MB_OK );
  #else
    printf("ERROR: ");
    printf( s );
    printf("\nRARS terminated!\n");
  #endif 
  
  exit(1);
}

/**
 * 
 */
void exitNormally()
{
  exit(0);
}

/*
 * Generalized warning function. It can be called the same way that printf() can.
 *
 * @format format with the same meaning as in printf
 */
void warning(const char *format, ...)
{
  char s[1024];

  va_list argList;
  
  va_start(argList, format);
  vsprintf( s, format, argList);
  va_end(argList);
  
  #ifdef WIN32
    MessageBox( NULL, s, "Warning", MB_OK );
  #else
    printf("WARNING: ");
    printf( s );
    printf( "\n" );
  #endif 
}

////////////////////////////////////////////////////////////////////////////
// IO functions
////////////////////////////////////////////////////////////////////////////

/**
 * Convert a double to a 'pretty string'
 */
const char * Double2String( double val )
{
  static char buffer[256];

  double val2 = val;
  int i;
  double fval;
  bool bFound = false;

  for( i=0; i<4; i++ )
  {
    fval = floor(val2+0.5);
    if( fabs(val2-fval)<0.001 )   
    {
      bFound = true;
      break;
    }
    val2 *= 10;
  }

  if( bFound )
  {
    if( i==0 ) 
    {
      sprintf( buffer, "%.0f" , val ); 
    }
    else if( i==1 ) 
    {
      sprintf( buffer, "%.1f" , val ); 
    }
    else if( i==2 ) 
    {
      sprintf( buffer, "%.2f" , val ); 
    }
    else if( i==3 ) 
    {
      sprintf( buffer, "%.3f" , val ); 
    }
  }
  else
  {
    sprintf( buffer, "%f" , val );
  }

  return buffer;
}

/**
 * Find this name in the drivers[] array
 *
 * @param name    (in) name of a driver
 * @return        index in drivers[]
 *                -1 is returned if name is not found
 *                else returned value will be 0 - MAX_CARS-1
 */
int find_name(char* name)
{
  int i=0, cmp;

  while( drivers[i]!=NULL )
  {
    cmp = strcmp(name, drivers[i]->getName());
    if(!cmp)
    {
      return i;
    }
    i++;
  }
  return -1;
}

/**
 * Call repeatedly to change direction by 180 degrees:
 *
 * @param v           
 * @param alpha_ptr
 * @param vc_ptr
 */
static void reverse(double v, double* alpha_ptr, double* vc_ptr) 
{ 
  if(v > 10.0)           // This is algorithm to reverse velocity vector: 
  {
    *vc_ptr = 0.0;       // if not going very slow, brake hard 
  }
  else
  {
    *vc_ptr = -15.0;     // when going slow enough, put 'er in reverse! 
  }
  *alpha_ptr = 0.0;      // don't turn. 
} 
 
/**
 * This routine analyses the parameters to determine if the car is in an 
 * abnormal situation.  If so, it returns non-zero & sets the result
 * vector so as to free the car.  If all is normal it returns zero. 
 * This is a service for the robot drivers; it is only called by them. 
 *
 * @param backward      (in) backward
 * @param v             (in) speed
 * @param vn            (in) normal speed
 * @param to_lft        (in) distance to the left of the road
 * @param to_rgt        (in) distance to the right of the road
 * @param alpha_ptr     (inout) alpha (direction)
 * @param vc_ptr        (inout) vc (speed)
 * @return              0 if OK 1 else.
 */
int stuck(int backward, double v, double vn, double to_lft, 
          double to_rgt, double* alpha_ptr, double* vc_ptr) 
{ 
  if(to_lft < 0.0)                     // If over the left wall,  
  {
    if(vn > .5 * v)                    // test for more than 30 degrees off course 
    {
      reverse(v, alpha_ptr, vc_ptr); 
      return 1; 
    } 
    else if(vn > -.5 * v && backward)  // or going well backward
    {
      reverse(v, alpha_ptr, vc_ptr); 
      return 1; 
    } 
    else if(vn < -.5 * v)              // heading away from wall, 
    {
      *alpha_ptr = .03;                // turn to left 
      *vc_ptr = (.66667 * v + 10.0);   // accelerate toward 30 fps 
      return 1; 
    } 
    else 
    {
      *alpha_ptr = -.03;               // turn to right 
      *vc_ptr = (.66667 * v + 10.0);   // accelerate toward 30 fps 
      return 1; 
    }
  }
  else if(to_rgt < 0.0)                // if over the right wall: 
  {
    if(vn < -.5 * v)                   // test for more than 30 degrees off course 
    {
      reverse(v, alpha_ptr, vc_ptr); 
      return 1; 
    } 
    else if(vn < .5 * v && backward)   // or going well backward 
    {
      reverse(v, alpha_ptr, vc_ptr); 
      return 1; 
    }
    else if(vn > .5 * v)               // heading away from wall, 
    {
      *alpha_ptr = -.03;               // turn to right 
      *vc_ptr = .66667 * v + 10.0;     // accelerate toward 30 fps 
      return 1; 
    } 
    else 
    {
      *alpha_ptr = .03;                // turn to left 
      *vc_ptr = .66667 * v + 10.0;     // accelerate toward 30 fps 
      return 1;
    }
  }
  else if(backward)
  {
    if(vn > .866 * v)                  // you are going more-or-less sideways left 
    {
      *alpha_ptr = -.03; 
      *vc_ptr = .66667 * v + 10; 
      return 1; 
    } 
    else if(vn < -.866 * v)            // you are going more-or-less sideways rt. 
    {
      *alpha_ptr = .03; 
      *vc_ptr = .66667 * v + 10; 
       return 1; 
    } 
    else
    {
      reverse(v, alpha_ptr, vc_ptr); 
      return 1; 
    }
  }
  else if(v < 15)                      // nothing wrong except you are going very slow: 
  {
    if(to_rgt > to_lft)                // you are on left side of track 
    {
      if(vn < -.7 * v)                 // and you are not heading very much to right 
      {
        *alpha_ptr = -.03; 
      }
      else 
      {
        *alpha_ptr = .03;
      }
    }
    else                               // you are on the right side, 
    {
      if(vn > .7 * v)                  // and you are not heading very much to left 
      {
        *alpha_ptr = .03; 
      }
      else 
      {
        *alpha_ptr = -.03; 
      }
    }
    *vc_ptr = .66667 * v + 10;         // acellerate moderately 
    return 1; 
  }
  return 0;                            // We get here only if all is normal. 
} 
 
/**
 * Returns true if either rear corner of the other car is within the base car. 
 * rel_x and rel_y are the coordinates of the other car in the local coor. 
 * system of the base car. 
 *
 * @param rel_x         (in) relative position x
 * @param rel_y         (in) relative position y
 * @param sine          (in) relative sin(angle)
 * @param cosine        (in) relative cos(angle)
 * @return              0 if not in 1 if collision

 */
static int other_in(double rel_x, double rel_y, double sine, double cosine) 
{ 
  double xl, yl, xr, yr;     // coordinates of rear corners, left & right 

  // locate rear left corner of other car: 
  xl = rel_x - w * cosine + l * sine; 
  yl = rel_y - l * cosine - w * sine; 
  // locate rear right corner: 
  xr = rel_x + w * cosine + l * sine; 
  yr = rel_y - l * cosine + w * sine; 
 
  // See if either corner of the other car is within the base car: 
  if(xl < w && xl > -w && yl < l)
  {
    return(1);
  }
  else if(xr < w && xr > -w && yr < l) 
  {
    return(1); 
  }
  else
  {
    return(0);
  }
}

/**
 * Returns true iff the car at rel_x, rel_y with orientation "rotation" is 
 * overlapping the base car.  rel_x, rel_y and rotation are all relative 
 * to the base car.  It is assumed that base car is "chasing" the other,  
 * hence rotation is within + or - 90 degrees and rel_y is greater than 0. 
 * Both cars are rectangles of length CARLEN and width CARWID, and rel_x
 * and rel_y refer to the center points of the rectangles. 
 *
 * @param rel_x         (in) relative position x
 * @param rel_y         (in) relative position y
 * @param rotation      (in) relative angle
 * @return              0 if not in 1 if collision
 */
int collide(double rel_x, double rel_y, double rotation) 
{ 
  double x, y; 
  double sine, cosine;            // sine and cosine of the relative rotation 
 
  sine = sin(rotation);
  cosine = cos(rotation);

  if(other_in(rel_x, rel_y, sine, cosine))
  {
    return 1;
  }
  else
  {
    x = rel_x * cosine + rel_y * sine;
    y = rel_y * cosine - rel_x * sine;
    if(other_in(x, y, -sine, cosine))
    {
      return 1;
    }
    else
    {
      return 0;
    }
  }
}

/**
 * Sorting routines that's fast when things are already in order:
 * This routine updates the race_data.m_aCarInPos[] array which has the position of each
 * car.  race_data.m_aCarInPos[0] is the ID of the leader, race_data.m_aCarInPos[1] is in second place, etc.
 */
void qsortem()
{
  int i, temp;
  
  for(i = 0; i < args.m_iNumCar-1; i++)
  {
    if( args.m_iQualMode==QUAL_FASTEST_LAP )
    {
      while(race_data.cars[race_data.m_aCarInPos[i+1]]->Q_bestlap > race_data.cars[race_data.m_aCarInPos[i]]->Q_bestlap)  
      { 
        // When a car runs faster qualifying lap, we swap positions:
        temp = race_data.m_aCarInPos[i]; 
        race_data.m_aCarInPos[i] = race_data.m_aCarInPos[i+1];
        race_data.m_aPosOfCar[race_data.m_aCarInPos[i+1]] = i;
        race_data.m_aCarInPos[i+1] = temp;
        race_data.m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
          --i;
      }
    }
    else if( args.m_iQualMode==QUAL_AVERAGE_SPEED )
    {
      while(race_data.cars[race_data.m_aCarInPos[i+1]]->Q_avgspeed > race_data.cars[race_data.m_aCarInPos[i]]->Q_avgspeed)  
      {
        // When a car gets higher average speed, we swap positions:
        temp = race_data.m_aCarInPos[i];
        race_data.m_aCarInPos[i] = race_data.m_aCarInPos[i+1];
        race_data.m_aPosOfCar[race_data.m_aCarInPos[i+1]] = i;
        race_data.m_aCarInPos[i+1] = temp;
        race_data.m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
          --i;
      }
    }
  }

  for(i = 0; i < args.m_iNumCar-1; i++)
  {
    if(race_data.cars[race_data.m_aCarInPos[i]]->Out == 3)
    {
      if(race_data.cars[race_data.m_aCarInPos[i+1]]->Out != 3) 
      { 
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
    draw.InitNewData();
  }
}

/**
 * Returns the next segment of the track 
 * (i.e. 0 yields 1, 1 yields 2, but NSEG-1 yields 0)
 *
 * @param seg           (in) a segment
 * @return              the next segment
 */
int incseg(int seg)
{
  if(++seg == currentTrack->NSEG)
  {
    seg = 0;
  }
  return seg; 
} 

/**
 * Returns the previous segment of the track 
 * (i.e. 1 yields 0, 2 yields 1, but 0 yields NSEG-1)
 *
 * @param seg           (in) a segment
 * @return              the next segment
 */
int decseg(int seg)
{
  if(--seg == -1)
  {
    seg = currentTrack->NSEG-1;
  }
  return seg;
} 

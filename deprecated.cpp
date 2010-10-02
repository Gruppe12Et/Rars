/**
 * DEPRECATED.CPP - contains the functions for compatiblity with older Rars
 *                  versions. These functions will maybe be removed in the 
 *                  future.
 *
 * History
 *  ver. 0.9  Aug 2001 Compatibility with 0.80 versions
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <string.h>

#include <car.h>
#include <deprecated.h>
#include <draw.h>

#ifdef USE_DEPRECATED

//--------------------------------------------------------------------------
//                            G L O B A L S
//--------------------------------------------------------------------------

int    car_count;
int    current_track;
bool   global_side_vision;
long   lap_count;
int    no_display;
Car *  pcar[MAX_CARS];
long   plap_count;
long   qlap_count;
int    surface;
char * trackfile[32];

static char sTrackFile[64];

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Initialise the deprecated globals
 */
void deprecated_init_globals()
{
  car_count          = args.m_iNumCar;
  global_side_vision = args.m_bGlobalSideVision;
  lap_count          = args.m_iNumLap;
  no_display         = !draw.m_bDisplay;
  for( int i=0; i<MAX_CARS; i++ )
  {
    pcar[i]= race_data.cars[i];
  }
  plap_count         = args.m_iNumPracticeLap;
  qlap_count         = args.m_iNumQualLap;
  surface            = args.m_iSurface;

  current_track      = args.m_iCurrentTrack;
  strcpy( sTrackFile, currentTrack->m_sFileName );
  trackfile[current_track] = sTrackFile;
}

/**
 * Do not use this funtion anymore
 */
double alpha_limit(double was, double request)
{
  // Function does not change the state of the object
  // This is not clean but it works.
  return race_data.cars[0]->AlphaLimit( was, request);
}

int farther(Car* car0, Car* car1)
{
  return car1->Farther( car0 );
}


#else

void deprecated_init_globals()
{
}

#endif

/**
 ** For a robot driver to identify itself, called during it initialization
 ** For compatibility reasons
 **
 ** @param name          (in) name of the robot
 *
 * Replaced by nothing.
 * The same information can be given with a 00 implementation of the driver.
 * If the driver is not OO, the name is given in the DriverOld constructor
 * called in drivers.cpp (the file with the list of the drivers)
 *
 * Action: Just remove the call and put right name in drivers.cpp
 *         or use a OO Driver and set the name in m_sName
 */
void my_name_is(const char * /*name*/)
{
} 

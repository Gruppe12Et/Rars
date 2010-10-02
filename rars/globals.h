/**
 * globals.h - Declaration of global variables
 *
 * Declares:
 *  CompetitionData
 *  Settings
 *
 * History
 * ver. 0.84: 08.07.2001: Created
 *	
 * @author    Carsten Kjaer
 * @see       RaceManager.cpp for definition of race_data and args
 * @version   0.84
 */

#ifndef __GLOBALS_H
#define __GLOBALS_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "track.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define MAX_CARS   32 // This must be greater than the number of drivers defined in drivers.cpp
#define MAX_TRACKS 32

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

class Car;          // Forward declaration (declared in car.h)

/**
 * Stage
 */
enum Stage {BEFORE, QUALIFYING, PRACTICE, RACING, FINISHED};

/**
 * Movie mode
 */
enum MovieMode {MOVIE_NORMAL, MOVIE_RECORD, MOVIE_PLAYBACK};

/**
 * Qualification mode
 */
enum QualMode {QUAL_FASTEST_LAP, QUAL_AVERAGE_SPEED};

/**
 * Args contains all arguments of the program that do not 
 * change during the race
 */
class Args
{
  private:
    void PrintHelpFile();
    void VersionReport();

  public:
    MovieMode m_iMovieMode;
    char m_sMovieName[80];
    bool m_bQual;               // If true then qualification is done according to the next 3 variables
    QualMode m_iQualMode;
    int  m_iNumQualLap;         // Number of laps in each qualification session
    int  m_iNumQualSession;     // Number of qualification sessions
    bool m_bPractice;           // If true then a practice session precedes qualification and race
    int  m_iNumPracticeLap;     // Number of practice laps allowed for each car
    long m_iRaceLength;         // Race length in miles
    bool m_bRandomMotion;       // When set, no random variable generation
    bool m_bRndmiz;             // Use random with a random seed
    long m_iSeed;               // Random seed 
    bool m_bGlobalSideVision;   // Disables side vision for all drivers if false
    int  m_iStartRows;          // How many start rows of cars in the starting line
    bool m_bKeepOrder;          // When set starting order = initial order
    long m_iNumRace;            // There will be this many races per track
    long m_iNumLap;             // There will be this many laps.
    int  m_iNumCar;             // This many cars will race
    int  m_iSurface;            // 0 is looser, 1 is harder
    char * m_aTracks[MAX_TRACKS]; // String names of track files
    int  m_iNumTrack;           // Number of tracks in trackFileNames
    int  m_iCurrentTrack;       // Index of current

    Args();
    void GetArgs(int argc, char* argv[]); // command line processing
};

/**
 * RaceData contains all information about the current race, 
 * cars, positions, finishing times etc.
 */
class RaceData
{
  private:
    void PickRandomOrder();       // Fills race_data.m_aStartPos[] with random order

  public:
    RaceData();
    ~RaceData();
    void QSortem();               // Sorts the m_aCarInPos[] array
    void OrderStartPos(int);      // Define the order of the cars

    Car ** cars;                  // Cars
    Stage stage;                  // What stage of the competition are we in
    int m_iNumCarFinished;        // Number of cars that have finished the race
    int m_iNumCarOut;             // Number of cars out of the race
    double m_fElapsedTime;        // Number of seconds since race start
    double** m_aLapFinishingTimes;// Finishing times for all laps and all cars
    fastest_lap m_oLapRecord;     //
    int m_aStartPos[MAX_CARS];    // The order of cars at the start of the race
    int m_aCarInPos[MAX_CARS];    // m_aCarInPos[i] is the index of the car (in cars) that is in position i
    int m_aPosOfCar[MAX_CARS];    // m_aPosOfCar[i] is the position of the car in cars[i]
    long m_iInitialSeed;          // Initial random seed
};

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

// Global variables - defined in RaceManager.cpp
extern Args args;
extern RaceData race_data;

#endif // __GLOBALS_H


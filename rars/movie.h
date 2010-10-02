/**
 * MOVIE.H - Record and replay a movie
 *
 * @author    Nicole Greiber
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef __RARSCORE_MOVIE_H
#define __RARSCORE_MOVIE_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <iostream>

using namespace std;

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Movie (used by RaceManager)
 */
class Movie
{
  private:
    fstream m_fileXY;           // Movie file for x/y
    fstream m_fileAng;          // Movie file for ang

  public:
    Movie( const char * sMovieName );
    ~Movie();

    int getDriverNames(char DriverNames[MAX_CARS][32]);

    void RecordInit( int car_count, Driver * drivers[] );
    void ReplayInit( int *car_count, Driver * drivers[] );

    void RecordRacers( int car_count, int * racers );
    void ReplayRacers( int car_count, int * racers );

    void RecordStage( Stage stage );
    void ReplayStage( Stage *stage );

    bool isReplayFinished();

  friend class Car;
};

#endif // __RARSCORE_MOVIE_H



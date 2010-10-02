/*
 * Timer.cpp - Definition of methods in class Timer
 * 
 * History
 *  ver. 0.90 16.08.2002 created
 *
 * @author    Eugen Treise
 * @see:      Timer.h
 * @version   0.90
 */

#include "timer.h"


Timer::Timer()
{
#ifdef WIN32
  LARGE_INTEGER RealFrequency;
  // should work with Pentium compatible processors
  if( !QueryPerformanceFrequency(&RealFrequency) )
  {
    //warning("Timer could not be initialized.");
  }
  // convert to Ticks/millisecond
  Frequency = RealFrequency.LowPart / 1000.0;
#endif // WIN32
}


void Timer::startTimer()
{
#ifdef WIN32
  QueryPerformanceCounter( &StartCount );
#else // Linux
  gettimeofday( &StartCount, NULL );
#endif // WIN32
}


double Timer::stopTimer()
{
#ifdef WIN32
  QueryPerformanceCounter( &StopCount );
  return (StopCount.LowPart - StartCount.LowPart) / Frequency;
#else // Linux
  gettimeofday( &StopCount, NULL );
  return (StopCount.tv_sec - StartCount.tv_sec) * 1000.0 + (StopCount.tv_usec - StartCount.tv_usec) / 1000.0;
#endif // WIN32

}
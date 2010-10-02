/**
 * Timer.h - Timer functions
 *
 * This class is used to measure the CPU time taken by the robot functions.
 *
 * History
 *  ver. 0.90 16.08.2002 created
 *
 * @author    Eugen Treise
 * @see       Timer.cpp for method definitions
 * @version   0.90
 */

#ifndef __TIMER_H
#define __TIMER_H

#ifdef WIN32
#include "Vc_rars.h"
#else // Linux
#include <sys/time.h>
#include <unistd.h>
#endif // WIN32


class Timer
{
private:

#ifdef WIN32
  double Frequency;   // Frequency in Ticks/millisecond.
  LARGE_INTEGER
#else // Linux
  timeval
#endif // WIN32

    StartCount, StopCount;  // These variables hold the start and stop time values.
public:
  Timer();

  // Starts the timer.
  void startTimer();
  // Stops the timer and returns the elapsed time since startTimer in milliseconds.
  double stopTimer();
};

#endif // __TIMER_H


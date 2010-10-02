/**
 * DEPRECATED.H - compatibility with old rars version
 *
 * @author    Marc Gueury
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef __RARSCORE_DEPRECATED_H
#define __RARSCORE_DEPRECATED_H

// #define USE_DEPRECATED
#ifdef USE_DEPRECATED

//--------------------------------------------------------------------------
//                          D E F I N E S
//--------------------------------------------------------------------------

// MAXCARS has been renamed MAX_CARS
#define MAXCARS MAX_CARS
// Type STAGE is renamed Stage
#define STAGE Stage 
// Value of enum STAGE(FINISH) is renamed Stage(FINISHED)
const int FINISH=FINISHED;

//--------------------------------------------------------------------------
//                          G L O B A L S
//--------------------------------------------------------------------------

extern int    car_count;
extern int    current_track;
extern bool   global_side_vision;
extern long   lap_count;
extern long   plap_count;
extern int    surface;
extern char * trackfile[32];

//--------------------------------------------------------------------------
//                         F U N C T I O N S
//--------------------------------------------------------------------------

double alpha_limit(double was, double request);

#endif

void deprecated_init_globals();
void my_name_is(const char * /*name*/);

#endif


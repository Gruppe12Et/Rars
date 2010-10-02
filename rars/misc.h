/**
 * MISC.H - declarations for the functions in MISC.CPP    
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef __RARSCORE_MISC_H
#define __RARSCORE_MISC_H

#include "car.h"

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

// Intern random functions
int coreRand();
void seedRandomFunctions(long input);
void randomizer();

// Mathematics functions
int iround(double given);
double vec_mag(double x, double y);

// Error functions
void exitOnError(const char *format, ...);
void exitNormally();
void warning(const char *format, ...);

// IO function
const char * Double2String( double val );

// Other help functions
int find_name(char* name);
int collide(double, double, double);   // Return 1 for collision, else 0
int incseg(int seg);                   // Return the next segment of the track 
int decseg(int seg);                   // Return the previous segment of the track 


#endif // __RARSCORE_MISC_H


/*
 * G_MAIN.H - Main program
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@skynet.be> 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */ 

#ifndef __G_MAIN_H
#define __G_MAIN_H

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Stage of a race
 * Used inside the GUI Os only Windows/KDE
 */
enum RACESTAGE  
{
  BEGIN, 
  START_RACE,
  INIT_CARS,
  INIT_CARS2,
  QUALIF, 
  INRACE, 
  END
};

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

////
//// Variables
////
extern char * g_argv[50];
extern int g_argc;

////
//// Functions
////
void main_part1();
bool main_part2();
void main_part3();
void build_args(
  int iCars,
  int iLaps,
  int iRandom,
  int iSurface,
  const char * sTrackName,
  const char * sMovie,
  const char * sOpenGL,
  int iRadioQualif,
  int iQualifLaps,
  int iQualifNumber,
  const char * sFollowCar,
  const char ** aDriver
);

#endif // __G_MAIN_H


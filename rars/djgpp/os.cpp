// OS.CPP - all non-graphic platform dependent code is in here:
// This version is for DOS on a PC and uses Borland functions.
// M. Timin, February, 1995
// ver. 0.46 3/25/95 (of RARS)
// ver. 0.6b 5/8/95 b for beta
// ver. 0.61 May 26
// Upgraded to version 0.70 by Maido Remm (-sr flag)
// version 0.71 (-q, -p modified)
// version 0.72 (-l)

#include <dos.h>
#include <time.h>
#include <ctype.h>
#include <conio.h>
#include <string.h>
#include <stdlib.h>
#include <iostream.h>
#include <fstream.h>
#include "track.h"
#include "os.h"
#include "car.h"
#include "movie.h"

extern car_ID drivers[];  // pointers to driver programs from DRIVERS.CPP

static int no_waiting = 0;   // get_ch() doesn't wait if this is set
static int rndmiz = 1;  // see randomizer() below:
static long seed = 0L;  // to hold a value entered by user

// Default values:
int randomotion = 1;    // the default is stochastic traction model
int practice = 0;       // practice mode
long plap_count = 100;     // default number of desired practice laps
int track_count = 0;
int global_side_vision = 1; // disables side vision for all drivers if 0
int cm_rows = 0;   // how many start rows of cars defined on command line
long qlap_count = 12;      // default number of qualifying laps
int qualifying = 0;  // mode of qualifying (1 = fastest lap, 2 = avg_speed)
int qual_sess = 1;    // How many rounds of qualifying?
long lap_count = 0;       // length of the race in laps, initially undefined
long race_length = 200;      // 200 miles 
int car_count = 12;       // how many cars in the race
long race_count = 1;      // how many races?
int real_speed = 0;    // if non-zero, PC clock will control speed of race
int no_display = 0;  // if non-zero, race will be invisible
int keep_order = 0; // if this is 0, then starting order will be random
int surface = 1; // surface type, 0 is looser, 1 is harder. (see friction())
int behind = 1; // which distance to show on IP? 0=behind leader, 1 is to car in front
char moviename[80] = "movie";

void print_help_file();       // in REPORT.CPP
int find_name(char *);        // in REPORT.CPP
void version_report();        // in REPORT.CPP
void Randomize(long);         // in CARZ.CPP

// Randomly set the random variable generator, but only if rndmiz is set:
void randomizer()             // calls Randomizer() in CARZ.CPP
{
   if(rndmiz)
	   Randomize(0);
   else if(seed)
	   Randomize(seed);
}

/* This is called once by main() to interpret the command line:
   It sets lap_count, car_count, real_speed and trackfile[], and options.
   RARS command line options:             ( - or / signifies an option)
-h or -H or -?  shows this help screen         (/ may be used in place of -)
-d  meaning  drivers (followed by space and then list of driver's names)
-D  meaning  ignore drivers (followed by space and list of names to not use)
-f  meaning  fastest that computer can compute (default is realistic)
-k  meaning  keystrokes supplied by computer, no waiting.
-l  meaning  followed by race length in miles.
-mp meaning  playback movie (can be followed by filename, eg. -mpmovie)
-mr meaning  record movie (can be followed by filename, eg. -mrmovie)
-nd meaning  no display (there is a results report written to RACE.OUT)
-nr meaning  no randomization of r.v.g.  (same initial seed every time)
	(-nr may be followed by a seed value, without a space)
-nR meaning  car motion is deterministic, and also r.v.g. not randomized
-o  meaning  order (starting order as given in driver list or as compiled)
-p  meaning  practice, followed by the number of practice laps.
-q  meaning  qualifying mode (1-fastest lap, 2-avg speed)
-ql meaning  qualifying laps, how many
-qr meaning  qualifying sessions, how many
-r  meaning  races, how many
-s  meaning  surface type, s0 = loose surface, s1 = harder surface, default 0
-sr meaning  starting rows. Default is given in track file.
-v  meaning  Just show the version and exit.
-z  meaning  disable "side vision" for all drivers
*/
void get_args(int argc, char* argv[])
{
   int cur_arg, i, n, k;
   char c;
   char *ptr;           // will point to current argument being processed
   char *tmp_ptr;       // Used for track names
   int num_count = 0;   // how many numerical arguments have been seen
   char option;         // holds the option code
   car_ID temp_drv;
   char lastext[81];   // the last text argument processed
   lastext[0] = 0;

   // process each argument in turn:

   for(cur_arg=1; cur_arg<argc; cur_arg++)  {   // once for each argument
      ptr = argv[cur_arg];
      c = *ptr;
      if(c == '-' || c == '/')  {     // is this an option request?
	 ++ptr;
	 option = *ptr;               // grab the option code
	 ++ptr;                       // point to char after code
	 switch(option)  {            // which one?
	 case '?':  case 'h': case 'H':     // H for Help
	    print_help_file();
	    exit(0);
	 case 'd':                          // d for drivers
	    // re-arrange drivers[] array according to names in command line
	    for(n=0; n<MAXCARS; n++)  {
	       ++cur_arg;
	       ptr = argv[cur_arg];
	       if((i = find_name(ptr)) < 0) {
		  strcpy(lastext, ptr);
		  --cur_arg;
		  break;
	       }
	       temp_drv = drivers[n];
	       drivers[n] = drivers[i];
	       drivers[i] = temp_drv;
	    }
	    break;
	 case 'D':                 // D for drivers to eliminate
	    // re-arrange drivers[] array according to names in command line
	    for(n=0; n<MAXCARS; n++)  {
	       ++cur_arg;
	       ptr = argv[cur_arg];
	       if((i = find_name(ptr)) < 0) {
		  strcpy(lastext, ptr);
		  --cur_arg;
		  break;
	       }
	       temp_drv = drivers[i];
	       for(k=i; k<MAXCARS-1; k++)
		  drivers[k] = drivers[k+1];
	       drivers[MAXCARS-1] = temp_drv;
	    }
	    break;
	 case 'f':                      // f for fast
	    real_speed = 0;
	    break;
	 case 'k':                      // k for keystrokes (supplied)
	    no_waiting = 1;
	    break;
	 case 'l':                      // l for race length (in miles)
	    if (isdigit(*ptr))
	       race_length = atol(ptr);
	    break;
	 case 'm':
	    if ((*ptr == 'p') || (*ptr == 'r')) {
	       // get movie filename
	       if (*(ptr+1)) {
		  strcpy(moviename,ptr+1);
		  char *p = strchr(moviename,'.');
		  if (p)
		     *p = 0;
	       }
	       RACEMODE = (*ptr == 'r'? 1 : 2);
	    }
	    break;
	 case 'n':                      // n for no or non
	    if(*ptr == 'r')  {             // nr for non-random
	       rndmiz = 0;
	       if(*(ptr+1) != ' ')         // this when -nr has a seed given
		  seed = (long)atol(ptr+1);
	    }
	    else if(*ptr == 'R') {         // nR for even more non-random
	       rndmiz = 0;
	       randomotion = 0;
	    }
	    else if(*ptr == 'd')           // nd for no display
	       no_display = 1;
	    break;
	 case 'o':                         // o for order
	    keep_order = 1;
	    break;
	 case 'p':                         // p for practice
	    practice = 1;    // practice mode
	    if(isdigit(*ptr))
	       plap_count = atol(ptr); // lap count entered without space
	    break;
	    
	 case 'q':
	    if (isdigit(*ptr))
	       qualifying = atoi(ptr);     // set another mode by 0, 1 or 2
	    else if(*ptr == '\0')  {
	       if(!qualifying) qualifying = 1; // set default if only -q
	    }
	    else if (*ptr == 'l')  {       // ql for qual lap count
	       if(*(ptr+1) != ' '){
		  qlap_count = atol(ptr+1);
		  if(!qualifying) qualifying = 1; // set default
	       }
	    }
	    else if (*ptr == 'r') {         // qr for qual sessions or
	       if(*(ptr+1) != ' ') {        // qual Rounds
		  qual_sess = atoi(ptr+1);
		  if(!qualifying) qualifying = 1; // set default
	       }
	    }
	    break;
	    
	 case 'r':                         // r for races
	    if(*ptr == '\0')  {  // a space after the - causes a new argument
	       ++cur_arg;    // this is because spaces increase the arg count
	       ptr = argv[cur_arg];
	    }
	    race_count = atol(ptr);    // race count entered without space
	    break;
	 case 's':
	    if(*ptr == 'r')               // sr for starting rows
	       cm_rows = atoi(ptr+1);
	    else
	       surface = atoi(ptr);
	    break;
	 case 'v':                         // v for version
	    version_report();    // in report.cpp
	    exit(0);
	 case 'z':                         // side vision
	    global_side_vision = 0;
	    break;
	 }   // end switch()
      }  // end if( c == '-')
      else if(c >= '0' && c <= '9') {  // is it a numerical argument?
	 if(!num_count)                   // is this the first such?
	    car_count = atoi(ptr);
	 else{
	    lap_count = atol(ptr);   // if lap_count is explicitly defined,
	    race_length = 0;         // don't use race_length value
	 }
	 ++num_count;
      }
      else   {                            // then its a text argument
	 if(track_count<MAX_TRACKS){
	    tmp_ptr=new char[16];
	    strcpy(tmp_ptr,argv[cur_arg]);
	    trackfile[track_count++]=tmp_ptr;
	 }
	 else
	    cout<<"Max "<<MAX_TRACKS<<" tracks per series.."<<endl;
      }
   } // end big for(;;) loop.
   
   // handle goofy command line input:
   if(car_count > MAXCARS)
      car_count = MAXCARS;
   if(car_count < 0)
      car_count = 0;
   if(lap_count < 0)
      lap_count = 0; // use race_length for finding lap_count
   if(!lap_count && race_length <= 0)
      race_length = 200; // negative lap_count was entered?
   if(qlap_count <= 0)
      qlap_count = 1;
   if(plap_count <= 0)
      plap_count = 1;
   if(track_count <= 0){
      tmp_ptr=new char[16];
      strcpy(tmp_ptr,"rars.trk"); //the default trackfile
      trackfile[0]=tmp_ptr;
      track_count = 1;
   }
   if(qualifying > 2 || qualifying < 0)
      qualifying = 1;         // only 0,1,2 allowed
   if(cm_rows < 0 || cm_rows > 4)
      cm_rows = 1;
   if(!strcmp(trackfile[0], "season")) // read tracks from file
      read_track_names();
   
   // check to see if a bogus driver name was entered under the -d option:
   if(!lastext[0])
      return;                         // OK if nothing was captured
   for(i=0; lastext[i]; i++) {        // OK if a number was captured
      c = lastext[i];
      if(c < '0' || c > '9')   // is it a number?
	 break;                 // leave loop if not a number
   }
   if(i == strlen(lastext))
      return;                  // This when lastext contains only numbers
}

int get_ch()       // Gets an operator input from the keyboard or mouse button.
{                  // Waits, polling, if there is no data available.
   static int got = ' ';
   
   int ret = got;

   if(no_waiting)  {      // we don't wait for data if this is set
	  if(kbhit())
	 if(getch() == ESC)    // We will respond to the escape key
		got = ESC;

      ret = got;
      return ret;
   }
   got = getch();
   if(got)             // The PC returns 0 first for some keys, then an ID byte
   {
      ret = got;
      return ret;
   }
   else
   {
      ret = 256 * getch();    // we will return the ID times 256
      return ret;
   }

}

int kb_hit()         // Returns 0 only if there is no data available
{                    // for get_ch().  If kb_hit() returns non-zero, that
   return kbhit();   // means that get_ch() will return data immediately.
}

// This routine must monitor a hardware periodic signal in order to
// return at the next "clock tick".  The period of the clock must be
// assigned to delta_time in OS.H.  This version uses the IBM PC clock
// and delta_time is accordingly set at .0549 sec.  For other platforms,
// delta_time should be between about .05 and .1 second.
// This routine must be called once with a non-zero argument to initialize it.
// After that it is called with no argument, or with zero.
void one_tick(int initialize = 0)
{
   static struct dostime_t t;        // used by Borland _dos_gettime() function
   static unsigned char pre_sec;
   static unsigned char pre_h;

   if(initialize)  {
	  _dos_gettime(&t);
      pre_sec = t.second;
      pre_h   = t.hsecond;
      return;
   }
    // wait for next tick of PC clock
   do {
      _dos_gettime(&t);
   } while(pre_sec == t.second && pre_h == t.hsecond);
   pre_sec = t.second;
   pre_h   = t.hsecond;
}

// Gets a long integer "randomly" from system clock.
long pick_random()
{                       // This version returns the clock value in (1/20) sec.
   struct dostime_t t;

   _dos_gettime(&t);
   return(t.hsecond/5 + 20 * t.second + 1200 * (long)t.minute);
}

int RAM_query()  // returns the number of available 1 K RAM blocks
{
   // This will use up to 64 K bytes of RAM; it doesn't de-allocate them
   // Use this only just before exiting the program.
   for(int rami=0; rami<64; rami++)  {     // we don't consider more than 64
      char *ptr;
      ptr = new char[1024];                // try to allocate 1 K of RAM
      if(!ptr)                             // if unsucessful, 
	 break;                               // leave the loop & return
   }
   return rami;
}

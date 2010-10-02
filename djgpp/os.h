// OS.H - by M. Timin, February 1995 - for the RARS software  
// This is part of version 0.60 of RARS (Robot Auto Racing Simulation)  
// ver. 0.39 3/6/95  
// ver. 0.45 3/21/95  
// ver. 0.50 4/5/95  
// ver. 0.6b May 8, 1995  b for beta  
// ver. 0.61 May 26  
// ver. 0.70 Nov 1997 added -sr flag  
// ver. 0.71 Jan 1998 qualifications  
// ver. 0.72 Mar 1998 (behind variable)
// Modified by Doug Eleveld to work with DJGPP/Allegro

extern int car_count;            // This many cars will race  
extern long lap_count;           // this many laps.  
extern long race_length;         // race length in miles
extern int practice;             // practice time before the race
extern long plap_count;           // the number of desired practice laps  
extern long race_count;           // There will be this many races.  
extern int real_speed;           // clear for fast, set for realistic  
extern int no_display;           // no graphics when set  
extern int randomotion;          // when set, no random variable generation  
extern int global_side_vision;   // disables side vision for all drivers if 0  
extern int keep_order;           // when set, starting order = initial order  
extern int surface;              // 0 is looser, 1 is harder  
extern int track_count;          // How many tracks overall
extern int cm_rows;   // how many start rows of cars defined on command line
extern long qlap_count;           // number of qualifying laps
extern int qualifying;// mode of qualifying (1 = fastest lap, 2 = avg_speed)
extern int qual_sess;            // How many rounds of qualifying?
extern int behind;               // which distance to show on IP? 
extern char moviename[];         // name of movie file
  
extern void get_args(int argc, char* argv[]); // command line processing  
extern void randomizer();                     // random init. for RVG
extern long pick_random();                    // get random no. from clock
extern int get_ch();                          // wait, then get KB character
extern int kb_hit();                          // true if a char is waiting
extern void one_tick(int);                    // waits till next PC clock tick  
extern int RAM_query();
  
const double delta_time = .0549;  // seconds, IBM PC clock tick period  
const int ESC  = 27;       // Escape key or other termination signal  
const int UP   = 18432;   // up arrow key
const int DOWN = 20480;   // down arrow key
const int LINES_PER_PAGE = 24;    // per screen page


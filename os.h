/**
 * OS.H - by M. Timin, February 1995 - for the RARS software
 *
 * This is for Linux version !!!!!!!!!!!
 *
 * History
 *  ver. 0.39 3/6/95
 *  ver. 0.45 3/21/95
 *  ver. 0.50 4/5/95
 *  ver. 0.6b May 8, 1995  b for beta
 *  ver. 0.61 May 26
 *  ver. 0.65 Mar 1, Randy Saint
 *  ver. 0.70 Nov 1997 (added -sr flag)
 *  ver. 0.71 Jan 1998 (qualifications)
 *  ver. 0.72 Mar 1998 (behind variable)
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef __RARSCORE_OS_H
#define __RARSCORE_OS_H
//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

const double delta_time = .0549;       // seconds, IBM PC clock tick period
const int ESC  = 27;                   // Escape key or other termination signal
const int UP   = 0x4800;               // page up key
const int DOWN         = 0x5000;       // page down key
const int ARROW_UP     = 0x5001;       // up arrow key
const int ARROW_DOWN   = 0x5002;       // down arrow key
const int ARROW_LEFT   = 0x5003;       // left arrow key
const int ARROW_RIGHT  = 0x5004;       // right arrow key
const int ROTATE_UP    = 0x5011;       // up arrow key
const int ROTATE_DOWN  = 0x5012;       // down arrow key
const int ROTATE_LEFT  = 0x5013;       // left arrow key
const int ROTATE_RIGHT = 0x5014;       // right arrow key
const int ROTATE_RESET = 0x5015;       // reset rotation
const int LINES_PER_PAGE = 24;         // per screen page
const int COLLISION_FLASH= 15;         // number of ticks to flash during collision

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Os (operating system dependent functions)
 */
class Os
{
  public:
    long PickRandom();                 // get random no. from clock
    int GetCh();                       // wait, then get KB character
    int KbHit();                       // true if a char is waiting
    void OneTick(int);                 // waits till next PC clock tick
    int RamQuery();
    void ShowInitMessage( const char * format, ... ); // see xx_lowgr.cpp
    void CreateDirectory( const char * dir );         // see xx_lowgr.cpp

};

extern Os os;

#endif // __RARSCORE_OS_H


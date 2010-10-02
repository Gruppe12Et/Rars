/**
 * ARGS.CPP - argument of the program. The argument are determined at the
 *            startup of the program and are not change during its 
 *            execution.
 *
 * History
 *  ver. 0.90 Creation
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <ctype.h>
#include <iostream>
#include <stdarg.h>
#include <stdlib.h>
#include <string.h>
#include "car.h"
#include "os.h"
#include "misc.h"
#include "draw.h"

using namespace std;

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Contructor
 */
Args::Args()
{
  // Set default values
  m_iMovieMode = MOVIE_NORMAL;
  strcpy( m_sMovieName, "movie" );
  m_bQual = false;
  m_iQualMode = QUAL_FASTEST_LAP;
  m_iNumQualLap = 7;
  m_iNumQualSession = 1;
  m_bPractice = false;
  m_iNumPracticeLap = 100;
  m_iRaceLength = 200;           // 200 miles 
  m_bRandomMotion = true;        // the default is stochastic traction model
  m_bRndmiz = true;              // Use a random random seed
  m_iSeed = 0L;                  // Value of the random seed
  m_bGlobalSideVision = true;
  m_iStartRows = 0;              // How many start rows of cars
  m_bKeepOrder = false;
  m_iNumRace = 1;  
  m_iNumLap = 0;                 // Length of the race in laps, initially undefined
  m_iNumCar = 12;                // How many cars in the race
  m_iSurface = 1;                // default surface (1 hard)
}

/**
 * This is called once by main() to interpret the command line:
 * It sets m_iNumLap, m_iNumCar, real_speed and trackfile[], and options.
 * RARS command line options:             ( - or / signifies an option)
 * -h or -H or -?  shows this help screen         (/ may be used in place of -)
 * -d  meaning  drivers (followed by space and then list of driver's names)
 * -D  meaning  ignore drivers (followed by space and list of names to not use)
 * -f  meaning  fastest that computer can compute (default is realistic)
 * -l  meaning  followed by race length in miles.
 * -mp meaning  playback movie (can be followed by filename, eg. -mpmovie)
 * -mr meaning  record movie (can be followed by filename, eg. -mrmovie)
 * -nd meaning  no display (there is a results report written to RAC.OUT)
 * -ni meaning  keystrokes supplied by computer, no waiting.
 * -nr meaning  no randomization of r.v.g.  (same initial seed every time)
 *         (-nr may be followed by a seed value, without a space)
 * -nR meaning  car motion is deterministic, and also r.v.g. not randomized
 * -o  meaning  order (starting order as given in driver list or as compiled)
 * -p  meaning  practice, followed by the number of practice laps.
 * -q  meaning  qualifying mode (1-fastest lap, 2-avg speed)
 * -ql meaning  qualifying laps, how many
 * -qr meaning  qualifying sessions, how many
 * -r  meaning  races, how many
 * -s  meaning  surface type, s0 = loose surface, s1 = harder surface, default 0
 * -sr meaning  starting rows. Default is given in track file.
 * -v  meaning  Just show the version and exit.
 * -z  meaning  disable "side vision" for all drivers
 *
 * @param argc          (in) from maim()
 * @param argv          (in) from maim()
 */
void Args::GetArgs(int argc, char* argv[])
{
  int cur_arg, i;
  char c;
  char *ptr;           // will point to current argument being processed
  char *tmp_ptr;       // Used for track names
  int num_count = 0;   // how many numerical arguments have been seen
  char option;         // holds the option code
  char lastext[81];   // the last text argument processed
  lastext[0] = 0;

  Driver * temp_drv;
  int n, k;

  // process each argument in turn:

  for(cur_arg=1; cur_arg<argc; cur_arg++) // once for each argument
  {
    ptr = argv[cur_arg];
    c = *ptr;
    if(c == '-' || c == '/')         // is this an option request?
    {
      ++ptr;
      option = *ptr;                // grab the option code
      ++ptr;                        // point to char after code
      switch(option)                // which one?
      {
        case '?':  case 'h': case 'H': // H for Help
          PrintHelpFile();
          exit(0);
        case 'd':                          // d for drivers
          // re-arrange drivers[] array according to names in command line
          for( n=0; n<MAX_CARS; n++ )  
          {
            ++cur_arg;
            if (cur_arg == argc)
            {
              break;
            }

            ptr = argv[cur_arg];
            if((i = find_name(ptr)) < 0)
            {
              //TODO: it must be possible to replay with unknown drivers.
              if( m_iMovieMode != MOVIE_PLAYBACK )
                exitOnError("Driver %s was not found.", ptr);
              /*
              strcpy(lastext, ptr);
              --cur_arg;
              break;
              */
            }
            temp_drv = drivers[n];
            drivers[n] = drivers[i];
            drivers[i] = temp_drv;
          }
          break;
        case 'D':                 // D for drivers to eliminate
          // re-arrange drivers[] array according to names in command line
          for(n=0; n<MAX_CARS; n++)
          {
            ++cur_arg;
            if (cur_arg == argc)
            {
              break;
            }
               
            ptr = argv[cur_arg];
            if((i = find_name(ptr)) < 0)
            {
              strcpy(lastext, ptr);
              --cur_arg;
              break;
            }
            temp_drv = drivers[i];
            for(k=i; k<MAX_CARS-1; k++)
            {
              drivers[k] = drivers[k+1];
            }
            drivers[MAX_CARS-1] = temp_drv;
          }
          break;
        case 'f':                      // f for fast
          if (isdigit(*ptr))
          {
            draw.m_iFastDisplay = atoi(ptr);
          } 
          else
          {
            draw.m_iFastDisplay = -1;
          }
          break;
        case 'l':                      // l for race length (in miles)
          if (isdigit(*ptr))
          {
            m_iRaceLength = atol(ptr);
          }
          break;
        case 'm':
          if ((*ptr == 'p') || (*ptr == 'r'))
          {
               // get movie filename
            if (*(ptr+1))
            {
              strcpy( m_sMovieName, ptr+1 );
              char *p = strchr( m_sMovieName,'.' );
              if( p )
              {
                *p = 0;
              }
            }
            m_iMovieMode = (*ptr == 'r'? MOVIE_RECORD : MOVIE_PLAYBACK);
          }
          break;
        case 'n':                      // n for no or non
          if(*ptr == 'r')              // nr for non-random
          {
            m_bRndmiz = false;
            if(*(ptr+1) != ' ')        // this when -nr has a seed given
            {
              m_iSeed = (long)atol(ptr+1);
            }
          } 
          else if(*ptr == 'R')         // nR for even more non-random
          {
            m_bRndmiz = false;
            m_bRandomMotion = false;
          }
          else if(*ptr == 'd')         // nd for no display
          {
            draw.m_bDisplay = false;
          }
          break;
        case 'o':                      // o for order
          m_bKeepOrder = true;
          break;
        case 'p':                      // p for practice
          m_bPractice = 1;        // practice mode
          if(isdigit(*ptr))
          {
            m_iNumPracticeLap = atol(ptr); // lap count entered without space
          }
          break;
            
        case 'q':
        {
          // Do ugly adjustment to new structure
          int qualifying = 1;
          if (isdigit(*ptr))
          {
            qualifying = atoi(ptr);// set another mode by 0, 1 or 2
          }
          else if(*ptr == '\0')
          {
            if(!qualifying) 
            {
              qualifying = 1;    // set default if only -q
            }
          }
          else if (*ptr == 'l')      // ql for qual lap count
          {
            if(*(ptr+1) != ' ')
            {
              m_iNumQualLap = atol(ptr+1);
              if(!qualifying)
              {
                qualifying = 1;// set default
              }
            }
          }
          else if (*ptr == 'r')      // qr for qual sessions or
          {
            if(*(ptr+1) != ' ')    // qual Rounds
            {
              m_iNumQualSession = atoi(ptr+1);
              if(!qualifying) 
              {
                qualifying = 1;// set default
              }
            }
          }
          // Adjust to new structure
          if (qualifying == 0) 
          {
            m_bQual = false;
          }
          else if (qualifying == 1) 
          {
            m_bQual = true;
          }
          else if (qualifying == 2) 
          {
            m_bQual = true;
            m_iQualMode = QUAL_AVERAGE_SPEED;
          } 
          break;
        } 
        case 'r':                      // r for races
          if(*ptr == '\0')             // a space after the - causes a new argument
          {
            ++cur_arg;                 // this is because spaces increase the arg count
            ptr = argv[cur_arg];
          }
          m_iNumRace = atol(ptr);      // number of races entered without space
          break;
        case 's':
          if(*ptr == 'r')              // sr for starting rows
          {
            m_iStartRows = atoi(ptr+1);
          }
          else
          {
            m_iSurface = atoi(ptr);
          }
          break;
        case 'v':                      // v for version
          VersionReport();
          exit(0);
        case 'z':                      // side vision
          m_bGlobalSideVision = false;
          break;
      }   // end switch()
    }  // end if( c == '-')
    else if(c >= '0' && c <= '9')      // is it a numerical argument?
    {
      if(!num_count)                   // is this the first such?
      {
        m_iNumCar = atoi(ptr);
      }
      else
      {
        m_iNumLap = atol(ptr);   // if lap_count is explicitly defined,
        m_iRaceLength = 0;  // don't use race_length value
      }
      ++num_count;
    }
    else                               // then its a text argument
    {
      if( m_iNumTrack<MAX_TRACKS )
      {
        // tmp_ptr=new char[16];
        // strcpy(tmp_ptr,argv[cur_arg]);
        // m_aTracks[m_iNumTrack++]=tmp_ptr;
        m_aTracks[m_iNumTrack++]=argv[cur_arg];
      }
      else
      {
        cout<<"Max "<<MAX_TRACKS<<" tracks per series.."<<endl;
      }
    }
  } // end big for(;;) loop.

  // handle goofy command line input:
  if( m_iNumCar>MAX_CARS )
  {
    m_iNumCar = MAX_CARS;
  }
  if( m_iNumCar<0 )
  {
    m_iNumCar = 0;
  }
  if( m_iNumLap<0 )
  {
    m_iNumLap = 0; // use race_length for finding lap_count
  }
  if(!m_iNumLap && m_iRaceLength<=0 )
  {
    m_iRaceLength = 200; // negative lap_count was entered?
  }
  if( m_iNumQualLap<=0 )
  {
    m_iNumQualLap=1;
  }
  if(m_iNumPracticeLap <= 0)
  {
    m_iNumPracticeLap = 1;
  }
  if( m_iNumTrack<=0 )
  {
    tmp_ptr=new char[16];
    strcpy(tmp_ptr,"rars.trk"); //the default trackfile
    m_aTracks[0]=tmp_ptr;
    m_iNumTrack = 1;
  }
  /*  if(qualifying > 2 || qualifying < 0)
  {
      qualifying = 1;         // only 0,1,2 allowed (not necessary anymore)
  }
  */
  if( m_iStartRows<0 || m_iStartRows>8 ) // starting rows
  {
    m_iStartRows = 1;
  }
  if(draw.m_iFastDisplay < -1 )
  {
    draw.m_iFastDisplay = 1;
  }
  if(!strcmp(m_aTracks[0], "season")) // read tracks from file
  {
    Track::readTrackNamesFromFile();
  }
   
  // check to see if a bogus driver name was entered under the -d option:
  if(!lastext[0])
  {
    return;                         // OK if nothing was captured
  }
  for(i=0; lastext[i]; i++)         // OK if a number was captured
  {
    c = lastext[i];
    if(c < '0' || c > '9')   // is it a number?
    {
      break;                 // leave loop if not a number
    }
  }
  if(i == (int)strlen(lastext))
  {
    return;                  // This when lastext contains only numbers
  }
}

/**
 * Prints RARS.HLP to the screen, or if the
 * file is not found, prints embedded text strings.
 */
void Args::PrintHelpFile()
{
  ifstream fin("rars.hlp");
  char c;
  int linecount = 0;
   
  if(!fin)
  {
    cout << " RARS.HLP is missing.  The command line options are:" << endl;   
    cout << "-h or -?  or /H  -  HELP " << endl;   
    cout << "-d   drivers (followed by list of drivers names)" << endl;   
    cout << "-D   ignore drivers (followed by list of drivers names)" << endl;   
    cout << "-f   fast speed (not realistic speed)" << endl;   
    cout << "-l   followed by race length in miles" << endl;
    cout << "-mp  playback movie (can be followed by filename, eg. -mpmovie)" << endl;   
    cout << "-mr  record movie (can be followed by filename, eg. -mrmovie)" << endl;   
    cout << "-nd  no graphics display" << endl;   
    cout << "-ni  non/interactive: don't wait for keystroke" << endl;   
    cout << "-nr  no randomization (always same output from r.v. generator)" << endl;   
    cout << "-nR  no randomization (car motion is deterministic)" << endl;   
    cout << "-o   order (starting order as given in driver file)" << endl;   
    cout << "-p   practice laps, how many" << endl;   
    cout << "-q   qualification mode: 1-fastest lap, 2-average speed" << endl;  
    cout << "-ql  qualification laps, how many" << endl;
    cout << "-qr  qualification sessions, how many" << endl;
    cout << "-r   races, how many" << endl;   
    cout << "-s   surface type, -s0 or -s1" << endl;   
    cout << "-sr  start rows, how many" << endl;   
    cout << "-v   just show version" << endl;   
    cout << "-z   disable side vision for all drivers" << endl;   
  }
  else
  {
    while((c = fin.get()) != EOF)
    {
      if(c == '\n')                      // a simple paging facility   
      {
        if(++linecount == LINES_PER_PAGE)
        {
          linecount = 0;   
          cout  << endl << "(any key to continue)";   
               
          os.GetCh(); // does not work in Linux !?   
          cout << endl;   
        }
        else
        {
          cout << endl;
        }
      }
      else
      {
        cout << c;
      }
    }
  }
}

/**
 *  Reports the version of rars
 */
void Args::VersionReport()
{
  cout << "This is Robot Auto Racing Series (RARS), version ";
  cout << VERSION;
  cout << endl;
}

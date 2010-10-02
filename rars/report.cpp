/**
 * REPORT.CPP - output of racing and other data to a file - M. Timin, March 1995   
 * for RARS version 0.60   
 *
 * History
 *  ver. 0.50 4/5/95   
 *  ver. 0.6b 5/8/95 b for beta   
 *  ver. 0.60 5/17/95   
 *  ver. 0.61  May 95   
 *  ver. 0.70  Dec 97
 *  ver. 0.72 Mar 98
 *  ver. 0.73 Aug 98
 *  ver. 0.8? Apr 2001 fix in INDYCAR and NASCAR scoring (Rémi)
 *
 * @author    Henning Klaskala <bm321465@muenchen.org>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.80
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdarg.h>
#include <fstream.h>
#include <string.h>
#include <iomanip.h>
#include "report.h"
#include "os.h"
#include "misc.h"

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Opens the output file, fout, using a name made from the track file.   
 * Outputs number of cars and laps, name of track file, and the robot's names.   
 * Also, the starting seed of the random variable generator.   
 */
void Report::WriteBegin()
{
  int i;   
  char base[64], name[64], name2[64];   
   
  if( args.m_iNumTrack==1 )
  {
    // make a filename like the track name, but with the .out extension:   
    strcpy( base, args.m_aTracks[0] );   
    for(i=0; i<8; i++)
    {
      if( base[i]=='.' || base[i]==0 ) 
      {
        break;
      }
    }
    base[i] =0;

    // Open the output files
    sprintf( name, "result_%s", base );
  }
  else // Multiple races:
  {
    sprintf( name, "result_season" );
  }
  sprintf( name2, "%s.html", name );
  if( (m_fhtml = fopen(name2, "w"))==0 )
  {
    // can not open file in current dir -> Unix -> open it in /tmp/
    warning( "Report.cpp: WriteBegin: current directory is not writable: writing result files in /tmp" );
    sprintf( name2, "%s", name );
    sprintf( name, "/tmp/%s", name2 );
    sprintf( name2, "%s.html", name );
    if( (m_fhtml = fopen(name2, "w"))==0 )
    {
      exitOnError( "Report.cpp: WriteBegin: Unable to create file %s", name );
    }
  }
  sprintf( name2, "%s.txt", name );
  m_fout.open( name2 );

  WriteHtml( "<HTML>\n" );
  WriteHtml( "<HEAD><style type=\"text/css\">\n" );
  WriteHtml( "<!--\n" );
  WriteHtml( ".tabTitle { background-color: #000099; font-weight: bold; color: #FFFFFF}\n" );
  WriteHtml( ".title {  font-size: 24px; font-weight: bold; color: #0000CC}\n" );
  WriteHtml( ".tableBorder {  font-size: 9px; color: #000099; background-color: #000099}\n" );
  WriteHtml( ".tabData {  font-family: \"Courier New\", Courier, mono; text-align: left}\n" );
  WriteHtml( ".red {  font-weight: bold; color: #FF0000}\n" );
  WriteHtml( "-->\n" );
  WriteHtml( "</style></HEAD>\n" );
  WriteHtml( "<BODY><CENTER>\n" );
  WriteHtml( "<span class=\"title\">" );
  Write( "RARS version %3.2f", VERSION );
  WriteHtml( "</span>" );
  WriteNewLine();
  WriteNewLine();
  Write( "Track surface type %d", args.m_iSurface );
  WriteNewLine();
  Write( "%d cars for ", args.m_iNumCar );
  if( args.m_iNumLap )
  {
    Write( "%d laps over ", args.m_iNumLap );
  }
  else
  {
    Write( "%5.2f miles over ", args.m_iRaceLength );
  }
  Write( "%d tracks with ", args.m_iNumTrack );
  Write( "%d races on each track.", args.m_iNumRace );
  WriteNewLine();
  Write( "The initial RVG seed was %d", race_data.m_iInitialSeed );
  WriteNewLine();
  if(args.m_bPractice)
  {
    Write( "%d practice laps were requested.", args.m_iNumPracticeLap );
  WriteNewLine();
  }
  WriteNewLine();
}

/**
 * Outputs final qualifying results
 *
 * @param order      (in) an array with the srating order of the cars
 * @param pcar       (in) an array with a pointer to all cars
 */
void Report::WriteQualResults(int* order, Car** pcar)
{
  int i, k;
  double ratio= 0;
 
  WriteHR( '-' );
  WriteHtml( "<span class=\"title\">" );
  Write( "Qualifying results on %s", get_track_description().sName );
  WriteHtml( "</span>" );
  WriteNewLine();
  WriteNewLine();
  Write( "%d laps for ", args.m_iNumQualLap );
  if( args.m_iQualMode==QUAL_FASTEST_LAP )
  {
    Write( "fastest LAP speed. " );
  }
  else if (args.m_iQualMode == QUAL_AVERAGE_SPEED)
  {
    Write( "fastest AVERAGE speed. " );
  }
  Write( "%d", args.m_iNumQualSession );
  if (args.m_iNumQualSession == 1)
    Write( " qualifying session." );
  else
    Write( " qualifying sessions." );
  WriteNewLine();
  Write( "Time to qualify = best time * %4.2f = ", Q_CUTOFF  );
  if(args.m_iQualMode == QUAL_AVERAGE_SPEED)
  {
    Write( "%6.2f", MPH_FPS * pcar[order[0]]->q_avgspeed / Q_CUTOFF );
  }
  else if(args.m_iQualMode == QUAL_FASTEST_LAP)
  {
    Write( "%6.2f", MPH_FPS * pcar[order[0]]->q_bestlap / Q_CUTOFF );
  }
  Write( " mph." );

  WriteNewLine();
  WriteNewLine();
  WriteHtml( "<TABLE>" );
  WriteHtml( "<TR><TD class=\"tabTitle\">Position</TD><TD class=\"tabTitle\">Driver</TD><TD class=\"tabTitle\">Time</TD></TR>" );
  for(i=0; i<args.m_iNumCar; i++) 
  {
    WriteHtml( "<TR><TD>" );
    k = order[i];   
    Write( "%2d ", i+1 );
    WriteHtml( "</TD><TD>" );
    Write( "%-9s ", drivers[k]->getName() );
    WriteHtml( "</TD><TD>" );
    if( args.m_iQualMode==QUAL_AVERAGE_SPEED )
    {
      if(pcar[k]->q_avgspeed)
        ratio = 100 * pcar[order[0]]->q_avgspeed / pcar[k]->q_avgspeed;
      else ratio = 0;
      Write( "%6.2f ", pcar[k]->q_avgspeed * MPH_FPS );
    }
    else if(args.m_iQualMode==QUAL_FASTEST_LAP)
    {
      if(pcar[k]->q_bestlap)
      {
        ratio = 100 * pcar[order[0]]->q_bestlap / pcar[k]->q_bestlap;
      }
      else
      {
        ratio = 0;
      }
      Write( "%6.2f mph", pcar[k]->q_bestlap * MPH_FPS );
    }

    if(pcar[k]->out==3)
    {
      m_fout << "DNQ";
      if(!pcar[k]-> q_avgspeed)
        Write( "%6d laps",  pcar[k]->laps );
      else
        Write( "%6d %",  ratio );
    } 
    else if(pcar[k]->damage)
    {
      Write( "%6d %", pcar[k]->damage );
    }
    WriteHtml( "</TD></TR>" );
    m_fout << endl;
  }
  WriteHtml( "</TABLE>" );
  WriteNewLine();
}

/**
 * Outputs the race results, statistics about race
 * and the accumulated points
 *
 * @param race       (in) ?
 * @param order      (in) an array with the srating order of the cars
 * @param pcar       (in) an array with a pointer to all cars
 */
void Report::WriteResults(int race, int* order, Car** pcar)
{
  int i, k;

  track_desc track = get_track_description();
  fastest_lap record = track.record;
  double length = track.length;

  WriteHR( '-' );
  WriteHtml( "<span class=\"title\">" );
  Write( "Race %d : Track %s", race + args.m_iCurrentTrack*args.m_iNumRace, track.sName );
  WriteHtml( "</span><br>" );
  WriteNewLine();
  Write( "Race length was %.2f miles, ", args.m_iNumLap*length/5280.0 );
  Write( "%.2f kilometers.", args.m_iNumLap*length/3281.0 );
  WriteNewLine();
  Write( "Track record was %.2f mph by %s", record.speed*MPH_FPS, record.rob_name );
  WriteNewLine();
  WriteNewLine();

  // Print table header:
  m_fout << " F  S  Driver      Laps Lead  Speed   Best Damage Fuel Pits Time Last Pnts";
  m_fout << endl;

  WriteHtml( "<table border=\"0\" cellpadding=\"0\" cellspacing=\"0\" bordercolor=\"#0000FF\">\n" );
  WriteHtml( "<tr>\n" );
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "</tr><tr>\n" ); 
  WriteHtml( "  <td class=\"tableBorder\">aa</td>\n" );
  WriteHtml( "  <td>\n" ); 
  WriteHtml( "<TABLE width=\"700\">\n" );
  WriteHtml( "<TR><TD class=\"tabTitle\">F</TD><TD class=\"tabTitle\">S</TD><TD class=\"tabTitle\">Driver</TD><TD class=\"tabTitle\">Laps</TD><TD class=\"tabTitle\">Lead</TD><TD class=\"tabTitle\">Speed</TD><TD class=\"tabTitle\">Best</TD><TD class=\"tabTitle\">Damage</TD><TD class=\"tabTitle\">Fuel</TD><TD class=\"tabTitle\">Pits</TD><TD class=\"tabTitle\">Time</TD><TD class=\"tabTitle\">Last</TD><TD class=\"tabTitle\">Pnts</TD></TR>" );
   
  // The places:   
  for(i=0; i<args.m_iNumCar; i++)
  {
    k = order[i];   
    if(pcar[k]->out == 3)
    {
      break;// Don't show DNQ cars in race results
    }

#ifdef F1
    // F1 style scoring:
    const int points[] = {10, 6, 4, 3, 2, 1 };   

    if(i < 6)   // Points:
    {
      m_aPoints[k] += points[i];
    }
#endif
#ifdef INDYCAR
    // Indycar style scoring: 
    const int points[] = { 20, 16, 14, 12, 10, 8, 6, 5, 4, 3, 2, 1 };   

    if(i < 12)   // Points:
    {
      m_aPoints[k] += points[i];
    }
    if(args.m_bQual && !pcar[k]->started)
    {
      m_aPoints[k] += 1;
    }
    for(int j=0;j<args.m_iNumCar;j++)
    {
      if (pcar[k]->laps_lead < pcar[j]->laps_lead)
      {
        break;
      }
      if (j==args.m_iNumCar-1)
      {
        m_aPoints[k] += 1;
      }
    }
#endif
#ifdef NASCAR
    // NASCAR style scoring: 
    const int points[] = {175,170,165,160,155,150,146,142,138,134,130,
    127,124,121,118,115,112,109,106,103,100,97,94,91,88,85,82,79,76,
    73,70,67,64,61,58,55,52,49,46,43,40,37,34,31};   

    if(i < 44)   // Points:
    {
      m_aPoints[k] += points[i];
    }
    if(pcar[k]->laps_lead)
    {
      m_aPoints[k] += 5;
    }
    for(int j=0;j<car_count;j++)
    {
      if (pcar[k]->laps_lead < pcar[j]->laps_lead)
      {
        break;
      }
      if (j==car_count-1)
      {
        m_aPoints[k] += 5;
      }
    }
#endif      
#ifdef BTCC
    // BTCC style scoring: 
    const int points[] = { 15, 12, 10, 8, 6, 5, 4, 3, 2, 1 };   

    if(i < 10)   // Points:
    {
      m_aPoints[k] += points[i];
    }
    if(args.doQual && !pcar[k]->started)
    {
      m_aPoints[k] += 1;
    }
#endif

    // Statistics:
    WriteHtml( "\n<TR>" );
    if( i==0 ) WriteHtml( "<DIV class=\"red\">" );
    WriteHtml( "<TD>" );
    Write( "%2d", i+1 );
    WriteHtml( "</TD><TD>" );
    Write( "%3d", pcar[k]->started+1 );
    WriteHtml( "</TD>" );
    if( i==0 )
    {
      WriteHtml( "<TD class=\"red\">" );
    }
    else 
    {
      WriteHtml( "<TD>" );
    }
    m_fout << "  ";
    Write( "%-9s",  drivers[k]->getName() );
    WriteHtml( "</TD><TD>" );
    // output completed laps or seconds down from leader:
    if( pcar[k]->laps>=args.m_iNumLap )
    {
      if(!i)
      {
        Write( "%7d", args.m_iNumLap );
      }
      else
      {
        Write( "%7.2f", pcar[order[0]]->last_crossing-pcar[k]->last_crossing );
      }
    }
    else
    {
      Write( "%7d", pcar[k]->laps );
    }
    WriteHtml( "</TD><TD>" );
    Write( "%5d", pcar[k]->laps_lead );
    WriteHtml( "</TD><TD>" );
    Write( "%7.2f",  pcar[k]->speed_avg * MPH_FPS );
    WriteHtml( "</TD><TD>" );
    Write( "%7.2f", pcar[k]->bestlap_speed * MPH_FPS );
    if(pcar[k]->bestlap_speed > record.speed)
    {
      Write( "!" );
    }
    else
    {
      Write( " " );
    }
    WriteHtml( "</TD><TD>" );
    Write( "%6d", pcar[k]->damage );
    WriteHtml( "</TD><TD>" );
    Write( "%5d", int(pcar[k]->fuel) );
    WriteHtml( "</TD><TD>" );
    Write( "%4d", pcar[k]->pit_stops );// number of stops
    WriteHtml( "</TD><TD>" );
    Write( "%6d", int(pcar[k]->Total_pit_time) );
    WriteHtml( "</TD><TD>" );
    Write( "%5d", pcar[k]->last_pit_visit );
    WriteHtml( "</TD><TD>" );
    Write( "%3d", m_aPoints[k] );
    WriteHtml( "</TD>" );
    WriteHtml( "</TR>" );
    m_fout << endl;
  }
  WriteHtml( "</TABLE>" );
  WriteHtml( "  </td>\n" ); 
  WriteHtml( "  <td class=\"tableBorder\">aa</td>\n" );
  WriteHtml( "</tr><tr>\n" ); 
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "  <td class=\"tableBorder\">a</td>\n" );
  WriteHtml( "</tr>\n" );
  WriteHtml( "</table>\n" );
}

/**
 * Show remaining RAM
 */
void Report::WriteRam()
{
  int iRam = os.RamQuery();   // returns number of free 1 K RAM blocks
  Write( "%d  K bytes of heap remained unused.", iRam );
  WriteNewLine();
}

/**
 * Write the final standing when rars has been started for several tracks.
 * This function is written by Oscar Gustfasson.
 */
void Report::WriteFinal()
{
  int i, tmp, changed = 1, m = args.m_iNumCar;
  int place[MAX_CARS];

  // check if a race was run at all
  if( !m_aPoints )
  {
    return;
  }

  for(i=0; i < m; i++)
  {
    place[i] = i;
  }

  while(changed)    // Lousy bubbelsort
  {
    changed = 0;
    for(i=0;i < m-1;i++)
    {
      if( m_aPoints[place[i]]<m_aPoints[place[i+1]] )
      {
        tmp = place[i];
        place[i] = place[i+1];
        place[i+1] = tmp;
        changed = 1;
      }
    }
  }
  WriteHR( '#' );
  Write( "Final standings." );
  WriteNewLine();
  WriteNewLine();
  
  for( i=0; i<m; i++ )
  {
    if( i==0 || m_aPoints[place[i]]!=m_aPoints[place[i-1]] )
    {
      // won't try the 2nd statement if i == 0
      Write( "Final place %3d ", i+1 );
    }
    else
    {
      Write( "                " );
    }
    Write( "%10s  %2d points", drivers[place[i]]->getName(),m_aPoints[place[i]]  );
    WriteNewLine();
  } 
  WriteNewLine();

  WriteHtml( "</CENTER></BODY>\n" );
  WriteHtml( "</HTML>\n" );
}

/**
 * Write the text to the both result file
 */
void Report::Write( const char * format, ... )
{
  va_list argList;
  va_start(argList, format);
  vsprintf(m_buffer, format, argList);
  va_end(argList);

  m_fout << m_buffer;
  fprintf( m_fhtml, m_buffer );
}

/**
 * Write a new line in the both files
 */
void Report::WriteNewLine()
{
  m_fout << endl;
  fprintf( m_fhtml, "<br>\n" );
}

/**
 * Write a horizontal line in the both files
 */
void Report::WriteHR( char c )
{
  for( int i=0; i<80; i++ )
  {
    m_fout << c;
  }
  m_fout << endl;
  fprintf( m_fhtml, "<hr>\n" );
}


/**
 * Write the text only to the html file
 */
void Report::WriteHtml( const char * format, ... )
{
  va_list argList;
  va_start(argList, format);
  vsprintf(m_buffer, format, argList);
  va_end(argList);

  fprintf( m_fhtml, m_buffer );
}


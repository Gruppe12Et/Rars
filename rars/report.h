/**
 * report.h - declaration report functions
 *
 * History
 *  ver. 0.84 Created 03.07.2001
 *
 * @author    Carsten Kjaer
 * @see       report.cpp for definition of functions
 * @version   0.84
 */

#ifndef __RARSCORE_REPORT_H
#define __RARSCORE_REPORT_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include "car.h"
#include <iostream>

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Report (used by RaceManager)
 */
class Report
{
  private:
    char m_buffer[512]; 
  
    int m_aPoints[MAX_CARS]; // Accumulated points, initialized:
    std::ofstream m_fout;         // Output : .out file
    FILE *   m_fhtml;        // Output : .html file

    void WriteHtml( const char * format, ... );
    void Write( const char * format, ... );
    void WriteNewLine();
    void WriteHR( char c );
  
  public:
    void WriteBegin();
    void WriteResults(int, int*, Car**);
    void WriteQualResults(int*, Car**);
    void WriteRam();
    void WriteFinal();
};

#endif




/**
 * FILE: G_DIVERS.CPP (portable)
 *
 * This file contains functions that are difficult to categorize.
 * ( error, fgetw, ... )
 *
 * History
 *  ver. 0.1  Aug 96 - Creation
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76 
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifdef WIN32
  #include <windows.h>
#endif

#include <stdio.h>
#include <stdlib.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                          F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Close what is initialized
 */
void error_close_all( void )
{
  if( g_ViewManager!=NULL && g_ViewManager->m_Initialized ) 
  {
    delete g_ViewManager;
  }
}

/**
 * Stop the program and write the error message
 * 
 * @param error_text    (in) error message
 */
void error( const char * error_text)
{
#ifdef WIN32
  MessageBox( 0,error_text,"ERROR",MB_OK);
#endif
  fprintf( stderr, error_text );
  fprintf( stderr, "\n" );

  error_close_all();
  exit(1);
}

/**
 * Read a word in a file (list of characters ended by a space or a return )
 * 
 * @param s             (inout) string where it writes the read word
 * @param f             (in) input file
 */
void fgetw ( char * s, FILE * f )
{
  int i = 0;
  int c = fgetc(f);

  while( c==' ' || c=='\n' || c=='\r' || c=='\t' ) 
  {
    c = fgetc(f);
  }
  s[i++] = (char)c;

  while( c!=' ' && c!='\n' && c!='\t' ) 
  {
    c = fgetc(f);
    s[i++] = (char) c;
  }

  ungetc( c, f );
  i--;
  s[i] = '\0';
}

/**
 * Format the time in HH:MM:SS:xxx
 *
 * @param sec           (in) time in ????
 * @param buf           (inout) time converted in string  
 */
void format_time(double sec, char *buf)
{
  const char* sign;
  if (sec < 0.0)
  {
    sec = -sec;
    sign = "-";
  } 
  else
  {
    sign = " ";
  }
  int m = (int)(sec / 60.0);
  int s = (int)(sec - 60*m);
  int c = (int)((sec - 60*m - s) * 1000.0);
  if (m)
  {
    sprintf(buf, "%s%2.2d:%2.2d:%3.3d", sign,m,s,c);
  }
  else
  {
    sprintf(buf, "   %s%2.2d:%3.3d", sign,s,c);
  }
}

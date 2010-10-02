/*
 * KDE_MAIN.CPP - Main function in KDE 2
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@synet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <kcmdlineargs.h>
#include <kaboutdata.h>
#include <klocale.h>

#include "../graphics/g_global.h"
#include "kde_rars.h"
#include "kde_start_impl.h"
//#include "../robots/bulle/kde_bulle_impl.h"

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

KApplication * kde_app;

//--------------------------------------------------------------------------
//                              M A I N
//--------------------------------------------------------------------------

int main( int argc, char* argv[] )
{
  KApplication kapplication( argc, argv, "rars" );
  kde_app = &kapplication;

  if( argc>1 )
  {
    if( argc==2 && strcmp( argv[1], "bulle" )==0 )
    {
      //KdeBulleImpl * krars = new KdeBulleImpl( &kapplication );
      //krars->show();
    }
    else
    {
      g_argc = argc;
      for( int i=0; i<argc; i++)
      {
        g_argv[i] = argv[i];
      }
      KdeRars * krars = new KdeRars();
      krars->show();
    }
  }
  else
  {
    KdeStartImpl * krars = new KdeStartImpl();
    krars->show();
  }
  return kapplication.exec();
}


/*
 * KDE_START.CPP - Start Window of RARS in KDE 2
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

#include <qcombobox.h>
#include <qdir.h>
#include <qfileinfo.h>
#include <qlined.h>
#include <qmessagebox.h>
#include <qmultilineedit.h>
#include <qradiobutton.h>
#include <qspinbox.h>
#include <qlistbox.h>
#include <unistd.h>


#include "kde_rars.h"
#include "kde_start_impl.h"
#include "../graphics/g_global.h"

//--------------------------------------------------------------------------
//                            G L O B A L S
//--------------------------------------------------------------------------

KdeRars * g_kde_rars;

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

static void FillComboWithDir( QComboBox * combo, const char * sDirectory, const char * ext )
{
  // Look in the current directory
  QDir dir(sDirectory);
  if( !dir.exists() )
  {
    fprintf( stderr, "Warning: Directory not found (%s).\n", sDirectory );
    fprintf( stderr, "Action: Run rars from his home directory\n" );
    return;
  }
  dir.setFilter( QDir::Files | QDir::NoSymLinks );
  const QFileInfoList * fileinfolist = dir.entryInfoList();
  QFileInfoListIterator it (*fileinfolist);
  QFileInfo *fi;
  while( (fi=it.current()) )
  {
    if( strcmp( fi->extension(), ext )==0 )
    {
       combo->insertItem( fi->fileName() );
    }
    ++it;
  }
}

//--------------------------------------------------------------------------
//                            Class KdeStartImpl
//--------------------------------------------------------------------------

/**
 * Constructor
 */
KdeStartImpl::KdeStartImpl()
: KdeStart()
{
  // Slot Button Start, Exit
  QObject::connect( (QObject *)PushStart, SIGNAL(clicked()), (QObject *)this, SLOT(slotStartRace()) );
  QObject::connect( (QObject *)PushExit, SIGNAL(clicked()), (QObject *)kde_app, SLOT(quit()) );

  // Slot Radio Button
  QObject::connect( (QObject *)RadioQualifRace, SIGNAL(toggled(bool)), (QObject *)this, SLOT(slotRadioToggled(bool)) );

  // Slot Combobox track
  QObject::connect( (QObject *)ComboTrack, SIGNAL(activated(int)), (QObject *)this, SLOT(slotChangeTrack(int)) );

  // Slot Button Driver SelectOne, SelectAll, RemoveOne, RemoveAll
  QObject::connect( (QObject *)PushDriverSelectOne, SIGNAL(clicked()), (QObject *)this, SLOT(slotDriverSelectOne()) );
  QObject::connect( (QObject *)PushDriverSelectAll, SIGNAL(clicked()), (QObject *)this, SLOT(slotDriverSelectAll()) );
  QObject::connect( (QObject *)PushDriverRemoveOne, SIGNAL(clicked()), (QObject *)this, SLOT(slotDriverRemoveOne()) );
  QObject::connect( (QObject *)PushDriverRemoveAll, SIGNAL(clicked()), (QObject *)this, SLOT(slotDriverRemoveAll()) );

  FillComboWithDir( ComboTrack, "tracks", "trx" );
  FillComboWithDir( ComboTrack, "tracks", "trk" );

  ComboOpenGL->insertItem("Classic");
  ComboOpenGL->insertItem("OpenGL Full Screen");
  ComboOpenGL->insertItem("OpenGL Window");
  ComboOpenGL->insertItem("Telemetry");
  ComboOpenGL->insertItem("Zoom");

  ComboMovie->insertItem("<None>");
  ComboMovie->insertItem("<Record>");
  FillComboWithDir( ComboMovie, "movies", "xy" );

  ComboSurface->insertItem("0 - Loose (Normal Car)");
  ComboSurface->insertItem("1 - Harder (Normal Car)");
  ComboSurface->insertItem("2 - Asphalt (F1 tyres)");

  ////
  //// Read the ini file and fill the value in the dlgbox
  ////
  SpinLaps->setValue( g_oRarsIni.m_iLaps );
  SpinRandom->setValue( g_oRarsIni.m_iRandom );
  ComboOpenGL->setCurrentItem( g_oRarsIni.m_iViewMode );
  for( int i=0; i<ComboTrack->count(); i++ )
  {
    if( strcmp( ComboTrack->text(i),g_oRarsIni.m_sTrackName)==0 )
    {
      ComboTrack->setCurrentItem(i);
      slotChangeTrack(i);
    }
  }

  ////
  //// Add the list of the drivers in the available or selected box
  ////
  g_oRarsIni.DriverSelect();

  ListAvailable->clear();
  ListSelected->clear();

  int i = 0;
  while( drivers[i]!=NULL )
  {
    const char * sDriverName = drivers[i]->getName();
    if( g_oRarsIni.m_aDriverSelected[i] )
    {
      ListSelected->insertItem( sDriverName );
    }
    else
    {
      ListAvailable->insertItem( sDriverName );
    }
    i++;
  }

  ListAvailable->sort();
  ListSelected->sort();

  ////
  //// Follow car
  ////
  ComboFollowCar->insertItem("<Any overtaking>");
  ComboFollowCar->insertItem("<For position>");

  i = 0;
  while( drivers[i]!=NULL )
  {
    int j;
    int nb = 2+i;
    const char * sDriver = drivers[i]->getName();

    for( j=2; j<nb; j++ )
    {
      if( strcmp( ComboFollowCar->text(j),sDriver)>0 )
      {
        ComboFollowCar->insertItem( sDriver, j );
        break;
      }
    }
    if( j==nb )
    {
      ComboFollowCar->insertItem( sDriver, nb );
    }
    i++;
  }

  if( g_oRarsIni.m_iFollowMode==FOLLOW_ANY_OVERTAKING )
  {
    ComboFollowCar->setCurrentItem(0);
  }
  else if( g_oRarsIni.m_iFollowMode==FOLLOW_FOR_POSITION )
  {
    ComboFollowCar->setCurrentItem(1);
  }
  else
  {
    for( int i=2; i<ComboFollowCar->count(); i++ )
    {
      if( strcmp( ComboFollowCar->text(i),g_oRarsIni.m_sFollowCar)==0 )
      {
        ComboFollowCar->setCurrentItem(i);
      }
    }
  }

  ComboSurface->setCurrentItem(g_oRarsIni.m_iSurface);
}


/**
 * Slot Radio Toggled
 */
void KdeStartImpl::slotRadioToggled(bool on)
{
  SpinQualifLaps->setEnabled( on );
  SpinQualifNumber->setEnabled( on );
}

/**
 * Slot : Button Start Rare
 */
void KdeStartImpl::slotStartRace()
{
  int iLaps = SpinLaps->value();
  int iRandom = SpinRandom->value();
  const char * sTrackName = ComboTrack->currentText();
  const char * sMovie = ComboMovie->currentText();
  const char * sOpenGL = ComboOpenGL->currentText();
  const char * sFollowCar = ComboFollowCar->currentText();
  int iRadioQualif = (RadioRaceRandom->isChecked()?0:1);
  int iQualifLaps = SpinQualifLaps->value();
  int iQualifNumber = SpinQualifNumber->value();
  int iNumSelectedDrivers = ListSelected->count();
  int iSurface = ComboSurface->currentItem();

  if( iNumSelectedDrivers==0 )
  {
    QMessageBox::warning( 0, "Error", "No driver has been selected." );
    return;
  }
  const char ** aDrivers = (const char**) new char**[ iNumSelectedDrivers ];
  for( int i=0; i<iNumSelectedDrivers; i++ )
  {
    aDrivers[i] = ListSelected->text( i );
  }

  build_args(
    iNumSelectedDrivers,
    iLaps,
    iRandom,
    iSurface,
    sTrackName,
    sMovie,
    sOpenGL,
    iRadioQualif,
    iQualifLaps,
    iQualifNumber,
    sFollowCar,
    aDrivers
  );

  g_kde_rars = new KdeRars;
  this->hide();
  g_kde_rars->show();
}

/**
 * Slot : Combobox - change track
 */
void KdeStartImpl::slotChangeTrack(int)
{
  int i;
  char base[64], filename[64];
  FILE * f;
  char buf[8192];

  // Make a filename like the track name, but with result_xxx.txt
  strcpy(base, ComboTrack->currentText());
  for(i=0; i<8; i++)
  {
    if(base[i] == '.' || base[i] == 0)
    {
      break;
    }
  }
  base[i] = 0;
  sprintf( filename, "result_%s.txt", base );

  memset( buf, 0, 8192 );
  if((f=fopen(filename,"rb"))!= NULL)
  {
    if( fread((char *)&buf,1,8191,f) > 0 )
    {
      fclose( f );
    }
  }
  LineLastResult->setText( buf );
}


/**
 * Slot : PushDriverSelectOne
 */
void KdeStartImpl::slotDriverSelectOne()
{
  int current = ListAvailable->currentItem();
  if( current>=0 )
  {
    ListSelected->insertItem( ListAvailable->text( current ) );
    ListAvailable->removeItem( current );
    ListAvailable->setCurrentItem( current );
  }
  ListSelected->sort();
}

/**
 * Slot : PushDriverSelectAll
 */
void KdeStartImpl::slotDriverSelectAll()
{
  int nb=ListAvailable->count();
  for( int i=0; i<nb; i++ )
  {
    ListSelected->insertItem( ListAvailable->text(i) );
  }
  ListAvailable->clear();
  ListSelected->sort();
}

/**
 * Slot : PushDriverRemoveOne
 */
void KdeStartImpl::slotDriverRemoveOne()
{
  int current = ListSelected->currentItem();
  if( current>=0 )
  {
    ListAvailable->insertItem( ListSelected->text( current ) );
    ListSelected->removeItem( current );
  }
  ListAvailable->sort();
}

/**
 * Slot : PushDriverRemoveAll
 */
void KdeStartImpl::slotDriverRemoveAll()
{
  int nb=ListSelected->count();
  for( int i=0; i<nb; i++ )
  {
    ListAvailable->insertItem( ListSelected->text(i) );
  }
  ListSelected->clear();
  ListAvailable->sort();
}
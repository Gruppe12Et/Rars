/**
 * FILE: DlgStartRace.cpp (VC++ specific)
 *
 * CDlgStartRace::CDialog
 *  - Selection of the options before the race
 *
 * Version       Author          Date
 *  0.1      Marc Gueury     14 /03 /98
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "stdafx.h"
#include <fstream.h>
#include <iostream.h>
#include <stdlib.h>
#include <direct.h>

#include "g_global.h"
#include "Vc_rars.h"
#include "dlgstart.h"
#include "dlgRars.h"
#include "movie.h"
#include "car.h"

/////////////////////////////////////////////////////////////////////////////
// CDlgStartRace dialog

CDlgStartRace::CDlgStartRace(CWnd* pParent /*=NULL*/)
    : CDialog(CDlgStartRace::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDlgStartRace)
  m_sTrackName    = _T("brazil.trk");
  m_iLaps         = 80;
  m_iRandom       = 0;
  m_LastResult    = _T("");
	m_iQualifLaps = 1;
	m_iQualifNumber = 1;
	m_iRadioQualif = 0;
	m_sOpenGL = _T("");
	m_sMovie = _T("");
	m_sFollowCar = _T("");;
	//}}AFX_DATA_INIT
}


void CDlgStartRace::DoDataExchange(CDataExchange* pDX)
{
    CDialog::DoDataExchange(pDX);
    //{{AFX_DATA_MAP(CDlgStartRace)
	DDX_Control(pDX, IDC_LAPS, m_EditLaps);
	DDX_Control(pDX, IDC_SPIN4, m_QualifLaps);
	DDX_Control(pDX, IDC_SPIN3, m_NumberQualif);
	DDX_Control(pDX, IDC_SPIN2, m_RaceLaps);
	DDX_Control(pDX, IDC_SURFACE, m_Surface);
	DDX_Control(pDX, IDC_DRIVER_AVAILABLE, m_ListAvailable);
	DDX_Control(pDX, IDC_DRIVER_SELECTED, m_ListSelected);
	DDX_Control(pDX, IDC_FOLLOW_CAR, m_ComboFollowCar);
  DDX_Control(pDX, IDC_MOVIE, m_ComboMovie);
  DDX_Control(pDX, IDC_OPEN_GL, m_ComboOpenGL);
  DDX_Control(pDX, IDC_LAST_RESULT, m_EditLastResult);
  DDX_Control(pDX, IDC_TRACK_NAME, m_ComboTrack);
  DDX_Text(pDX, IDC_TRACK_NAME, m_sTrackName);
  DDX_Text(pDX, IDC_LAPS, m_iLaps);
  DDV_MinMaxInt(pDX, m_iLaps, 1, 1000);
  DDX_Text(pDX, IDC_RANDOM, m_iRandom);
  DDX_Text(pDX, IDC_LAST_RESULT, m_LastResult);
	DDX_Text(pDX, IDC_QUALIF_LAPS, m_iQualifLaps);
	DDX_Text(pDX, IDC_QUALIF_SESSIONS, m_iQualifNumber);
	DDX_Radio(pDX, IDC_RADIO1, m_iRadioQualif);
	DDX_CBString(pDX, IDC_OPEN_GL, m_sOpenGL);
	DDX_CBString(pDX, IDC_MOVIE, m_sMovie);
	DDX_CBString(pDX, IDC_FOLLOW_CAR, m_sFollowCar);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgStartRace, CDialog)
  //{{AFX_MSG_MAP(CDlgStartRace)
  ON_BN_CLICKED(IDC_RADIO1, OnRandomRace)
  ON_BN_CLICKED(IDC_RADIO2, OnQualif)
  ON_BN_CLICKED(IDC_RADIO3, OnQualifOnly)
  ON_BN_CLICKED(IDC_RADIO4, OnUsePrevQualif)
  ON_CBN_SELCHANGE(IDC_TRACK_NAME, OnSelchangeTrackName)
	ON_BN_CLICKED(IDC_DRIVER_SELECT_ALL, OnDriverSelectAll)
	ON_BN_CLICKED(IDC_DRIVER_REMOVE_ONE, OnDriverRemoveOne)
	ON_BN_CLICKED(IDC_DRIVER_REMOVE_ALL, OnDriverRemoveAll)
	ON_BN_CLICKED(IDC_DRIVER_SELECT_ONE, OnDriverSelectOne)
	ON_LBN_DBLCLK(IDC_DRIVER_AVAILABLE, OnDblclkDriverAvailable)
	ON_LBN_DBLCLK(IDC_DRIVER_SELECTED, OnDblclkDriverSelected)
	ON_CBN_SELCHANGE(IDC_MOVIE, OnSelchangeMovie)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgStartRace message handlers

// m_QualFlag:
//      0   random starting grid for race
//      1   qualif and race
//      2   use static order
//      3   qualif only
//      4   race with previous qualif
//
BOOL CDlgStartRace::OnInitDialog() 
{
  int i;

  isMovieSelected = false;

  CDialog::OnInitDialog();
  m_hIcon = AfxGetApp()->LoadIcon(IDR_START);
  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  m_RaceLaps.SetRange(1, 1000);
  m_NumberQualif.SetRange(1, 100);
  m_QualifLaps.SetRange(1, 100);

  OnRandomRace();

  m_ComboTrack.Dir( 0, "tracks/*.tr*" );

  if( m_ComboTrack.GetCount()==0 )
  {
    // We are in a wrong directory
    // Just to be sure, go in upper directory
    chdir( ".." );
    m_ComboTrack.Dir( 0, "tracks/*.tr*" );
  }

  m_ComboMovie.Dir( 0, "movies/*.xy" );

  m_ComboTrack.SetCurSel(0);
  m_ComboOpenGL.SetCurSel(0);
  m_ComboMovie.SetCurSel(0);
    
  ////
  //// Read the ini file and fill the value in the dlgbox
  ////
  UpdateData( TRUE );
  m_iLaps   = g_oRarsIni.m_iLaps;
  m_iRandom = g_oRarsIni.m_iRandom;
  UpdateData( FALSE );
    
  m_ComboOpenGL.SetCurSel(g_oRarsIni.m_iViewMode);

  int iTrack = m_ComboTrack.FindString( 0, g_oRarsIni.m_sTrackName );
  if( iTrack!=CB_ERR ) 
  {
    m_ComboTrack.SetCurSel(iTrack);
  }

  m_Font.CreateFont(15,0,0,0,100,FALSE,FALSE,0,ANSI_CHARSET,
      OUT_DEFAULT_PRECIS,CLIP_DEFAULT_PRECIS,DEFAULT_QUALITY,
      DEFAULT_PITCH | FF_SWISS, "Courier New");
  m_EditLastResult.SetFont( &m_Font );
  OnSelchangeTrackName();

  ////
  //// Add the list of the drivers in the available or selected box
  ////
  g_oRarsIni.DriverSelect();

  ////
  //// Follow car
  ////
  m_ComboFollowCar.AddString("<Any overtaking>");
  m_ComboFollowCar.AddString("<For position>");

  ////
  //// List of cars
  ////
  i=0;
  while( drivers[i]!=NULL )
  {
    const char * sDriverName = drivers[i]->getName();
    if( g_oRarsIni.m_aDriverSelected[i] )
    {
      m_ListSelected.AddString( sDriverName );
    }
    else
    {
      m_ListAvailable.AddString( sDriverName );
    }
    m_ComboFollowCar.AddString( sDriverName );
    i++;
  }

  if( g_oRarsIni.m_iFollowMode==FOLLOW_ANY_OVERTAKING )
  {
    m_ComboFollowCar.SetCurSel(0);
  }
  else if( g_oRarsIni.m_iFollowMode==FOLLOW_FOR_POSITION )
  {
    m_ComboFollowCar.SetCurSel(1);
  }
  else
  {
    m_ComboFollowCar.SelectString( 0, g_oRarsIni.m_sFollowCar );
  }

  ////
  //// Surface
  ////
  m_Surface.SetCurSel( g_oRarsIni.m_iSurface );

  return TRUE;
}

void CDlgStartRace::OnOK() 
{
  if( !UpdateData(TRUE) )
    return;

  int iNumSelectedDrivers = 0;
  CString aStringDriver[MAX_CARS];
  const char ** aDrivers = NULL;
  // Normal race or movie record
  if( m_sMovie == "<None>" || m_sMovie == "<Record>" )
  {
    iNumSelectedDrivers = m_ListSelected.GetCount();
    if( iNumSelectedDrivers==0 )
    {
      MessageBox( "No drivers has been selected.", "Error", MB_ICONERROR );
      return;
    }

    aDrivers = (const char**) new char**[ iNumSelectedDrivers ];
    for( int i=0; i<iNumSelectedDrivers; i++ )
    {
      m_ListSelected.GetText( i, aStringDriver[i] );
      aDrivers[i] = aStringDriver[i];
    }
  }

  m_sTrackName.MakeLower();       // needed with NT 4

  int iSurface = m_Surface.GetCurSel();


  build_args(
    iNumSelectedDrivers, // use the number of selected driver as number of cars
    m_iLaps,
    m_iRandom,
    iSurface,
    m_sTrackName,
    m_sMovie,
    m_sOpenGL,
    m_iRadioQualif,
    m_iQualifLaps,
    m_iQualifNumber,
    m_sFollowCar,
    aDrivers
  );

  this->ShowWindow(SW_HIDE);
  CDlgRars oDlg;
  APP->m_pDlgRars = &oDlg;
  oDlg.DoModal();

  // ?? hangs the program
  // CDialog::OnOK();
  exit(0);
}

void CDlgStartRace::OnRandomRace() 
{
  GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(FALSE);
  GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(FALSE);
  GetDlgItem(IDC_LAPS)->EnableWindow(TRUE);   
}

void CDlgStartRace::OnQualif() 
{
  GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(TRUE);
  GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(TRUE);
  GetDlgItem(IDC_LAPS)->EnableWindow(TRUE);   
}

void CDlgStartRace::OnOrderRace() 
{
  GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(FALSE);
  GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(FALSE);
  GetDlgItem(IDC_LAPS)->EnableWindow(TRUE);   
}

void CDlgStartRace::OnQualifOnly() 
{
  GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(TRUE);
  GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(TRUE);
  GetDlgItem(IDC_LAPS)->EnableWindow(FALSE);  
}

void CDlgStartRace::OnUsePrevQualif() 
{
  GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(FALSE);
  GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(FALSE);
  GetDlgItem(IDC_LAPS)->EnableWindow(TRUE);   
  OnSelchangeTrackName();
}

void CDlgStartRace::OnSelchangeTrackName() 
{
  int i;
  char base[64], filename[64]; 
  FILE * f;
  char buf[8192];

  if( !UpdateData(TRUE) )
  {
    return;
  }
    
  // Make a filename like the track name, but with result_xxx.txt
  strcpy(base, m_sTrackName);
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
  m_LastResult = buf;
  UpdateData(FALSE);
}


void CDlgStartRace::OnDriverSelectOne() 
{
  moveOneDriver(m_ListAvailable, m_ListSelected);
}

void CDlgStartRace::OnDriverSelectAll() 
{
  moveAllDrivers(m_ListAvailable, m_ListSelected);
}

void CDlgStartRace::OnDriverRemoveOne() 
{
  moveOneDriver(m_ListSelected, m_ListAvailable);
}

void CDlgStartRace::OnDriverRemoveAll() 
{
  moveAllDrivers(m_ListSelected, m_ListAvailable);
}

void CDlgStartRace::OnDblclkDriverAvailable() 
{
	OnDriverSelectOne();
}

void CDlgStartRace::OnDblclkDriverSelected() 
{
  OnDriverRemoveOne();
}

void CDlgStartRace::moveOneDriver(CListBox& source, CListBox& target)
{
  int current = source.GetCurSel();
  if( current>=0 )
  {
    CString s;
    source.GetText( current, s );
    target.AddString( s );
    source.DeleteString( current );
    source.SetCurSel( current );

    // select the last available driver, if the last one was removed
    current = source.GetCurSel();
    int count = source.GetCount();
    if(current < 0 && count > 0)
    {
      source.SetCurSel(count-1);
    }
  }
}

void CDlgStartRace::moveAllDrivers(CListBox& source, CListBox& target)
{
  CString s;
  int nb = source.GetCount();
  for( int i=0; i<nb; i++ )
  {
    source.GetText( i, s );
    target.AddString( s );
  }
  source.ResetContent();
}

// Select the track for this movie and disable some controls when replaying a movie
void CDlgStartRace::OnSelchangeMovie() 
{
  CString Movie;
	m_ComboMovie.GetWindowText( Movie.GetBuffer(20), 20 );
	Movie.ReleaseBuffer();
	if( Movie == "<None>" || Movie == "<Record>" )
	{
		if( isMovieSelected )
		{
      SetPlaybackMode( false );
		}
	}
	else
	{
		int pos = Movie.Find(".xy");
		Movie.Delete( pos, 3 );
		if( m_ComboTrack.SelectString( -1, Movie ) == CB_ERR )
		{
      SetPlaybackMode( false );
			return;
		}

    OnSelchangeTrackName();

		if( !isMovieSelected )
		{
      SetPlaybackMode( true );
		}
	}
}

void CDlgStartRace::SetPlaybackMode(bool enable)
{
  bool disable = !enable;
  isMovieSelected = enable;

  GetDlgItem(IDC_DRIVER_AVAILABLE)->EnableWindow(disable);
  GetDlgItem(IDC_DRIVER_SELECTED)->EnableWindow(disable);
  GetDlgItem(IDC_DRIVER_SELECT_ALL)->EnableWindow(disable);
  GetDlgItem(IDC_DRIVER_REMOVE_ALL)->EnableWindow(disable);
  GetDlgItem(IDC_DRIVER_SELECT_ONE)->EnableWindow(disable);
  GetDlgItem(IDC_DRIVER_REMOVE_ONE)->EnableWindow(disable);
  GetDlgItem(IDC_RANDOM)->EnableWindow(disable);
  GetDlgItem(IDC_RADIO1)->EnableWindow(disable);
  GetDlgItem(IDC_RADIO2)->EnableWindow(disable);
  GetDlgItem(IDC_SURFACE)->EnableWindow(disable);

  char Laps[5];

  if( disable )
  {
  	m_ComboMovie.SetCurSel(0);

		itoa( m_iNumLaps, Laps, 10);
		m_EditLaps.SetWindowText( Laps );

    switch ( m_iRadioQualif )
    {
    case 0:
      OnRandomRace();
      break;
    case 1:
      OnQualif();
      break;
    case 2:
      OnQualifOnly();
      break;
    case 3:
      OnUsePrevQualif();
      break;
    default:
      break;
    }
  }
  else
  {
    GetDlgItem(IDC_QUALIF_LAPS)->EnableWindow(disable);
    GetDlgItem(IDC_QUALIF_SESSIONS)->EnableWindow(disable);

    m_EditLaps.GetWindowText( Laps, 5 );
		m_iNumLaps = atoi( Laps );
    // Set Laps to a high value, because Rars crashes, when movie has more laps then set here
    // FIXME: show the real number of laps for a movie
		m_EditLaps.SetWindowText( "300" );
  }

}

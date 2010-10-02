//--------------------------------------------------------------------------
//
//    FILE: Vc_rars.cpp (VC++ specific)
//
//    - CRarsApp::CWinApp
//         MFC application
//
//    Version       Author          Date
//      0.1      Marc Gueury     14 /03 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "g_global.h"
#include "Vc_rars.h"
#include "DlgStart.h"
#include "DlgRars.h"
#include "DlgTrackEditor.h"
#include "robots/bulle/dlgbulle.h"

/////////////////////////////////////////////////////////////////////////////
// CRarsApp

BEGIN_MESSAGE_MAP(CRarsApp, CWinApp)
    //{{AFX_MSG_MAP(CRarsApp)
    ON_COMMAND(ID_SPEED_FAST, OnSpeedFast)
    ON_COMMAND(ID_SPEED_MEDIUM, OnSpeedMedium)
    ON_COMMAND(ID_SPEED_SLOW, OnSpeedSlow)
    ON_COMMAND(ID_CAR_NEXT_CAR, OnCarNextCar)
    ON_COMMAND(ID_CAR_PRV_CAR, OnCarPrvCar)
    ON_COMMAND(ID_CAR_ANY_OVERTAKING, OnCarAnyOvertaking)
    ON_COMMAND(ID_CAR_FOR_POSITION, OnCarForPosition)
	ON_COMMAND(ID_ZOOM_IN, OnZoomIn)
	ON_COMMAND(ID_ZOOM_OUT, OnZoomOut)
	ON_COMMAND(ID_VIEW_CLASSIC, OnViewClassic)
	ON_COMMAND(ID_VIEW_OPENGL_FULLSCREEN, OnViewOpenglFullscreen)
	ON_COMMAND(ID_VIEW_OPENGL_WINDOW, OnViewOpenglWindow)
	ON_COMMAND(ID_VIEW_TELEMETRY, OnViewTelemetry)
	ON_COMMAND(ID_VIEW_ZOOM, OnViewZoom)
	ON_COMMAND(ID_SETTINGS_SAVE, OnSettingsSave)
	ON_COMMAND(ID_CAR_MANUAL, OnCarManual)
	ON_COMMAND(ID_OPENGL_VIEW, OnOpenglView)
	ON_COMMAND(ID_SPEED_REWIND, OnSpeedRewind)
	ON_COMMAND(ID_SPEED_STOP, OnSpeedStop)
	ON_COMMAND(ID_VIEW_TRAJECTORY, OnViewTrajectory)
	ON_COMMAND(ID_VIEW_DRIVERNAMES, OnViewDriverNames)
	ON_COMMAND(ID_CAR_NOBODY, OnCarNobody)
	ON_COMMAND(ID_MOVE_DOWN, OnMoveDown)
	ON_COMMAND(ID_MOVE_LEFT, OnMoveLeft)
	ON_COMMAND(ID_MOVE_RIGHT, OnMoveRight)
	ON_COMMAND(ID_MOVE_UP, OnMoveUp)
	ON_COMMAND(ID_ROTATE_DOWN, OnRotateDown)
	ON_COMMAND(ID_ROTATE_LEFT, OnRotateLeft)
	ON_COMMAND(ID_ROTATE_RIGHT, OnRotateRight)
	ON_COMMAND(ID_ROTATE_UP, OnRotateUp)
	ON_COMMAND(ID_ROTATE_RESET, OnRotateReset)
	ON_COMMAND(ID_SPEED_PAUSE, OnSpeedPause)
	//}}AFX_MSG_MAP
    ON_COMMAND(ID_HELP, CWinApp::OnHelp)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CRarsApp construction

CRarsApp::CRarsApp()
{
    // TODO: add construction code here,
    // Place all significant initialization in InitInstance
    m_pDlgRars = NULL;
}

/////////////////////////////////////////////////////////////////////////////
// The one and only CRarsApp object

CRarsApp theApp;

/////////////////////////////////////////////////////////////////////////////
// CRarsApp initialization

BOOL CRarsApp::InitInstance()
{
  // Standard initialization
  // If you are not using these features and wish to reduce the size
  //  of your final executable, you should remove from the following
  //  the specific initialization routines you do not need.

#ifdef _AFXDLL
  Enable3dControls();         // Call this when using MFC in a shared DLL
#else
  Enable3dControlsStatic();   // Call this when linking to MFC statically
#endif

  // CDlgStartRaceBulle dlg;
  if( strcmp(m_lpCmdLine, "-tracked" )==0 )
  {
    CDlgTrackEditor dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();
  }
  else
  {
    CDlgStartRace dlg;
    m_pMainWnd = &dlg;
    dlg.DoModal();
  }

  return TRUE;
}

void CRarsApp::OnSpeedPause() 
{
  g_iLastKey = 'P';
}

void CRarsApp::OnSpeedRewind() 
{
  g_iLastKey = 'R';
}

void CRarsApp::OnSpeedStop() 
{
  g_iLastKey = 'E';
}

void CRarsApp::OnSpeedFast() 
{
  g_iLastKey = 'F';
}

void CRarsApp::OnSpeedMedium() 
{
  g_iLastKey = 'D';
}

void CRarsApp::OnSpeedSlow() 
{
  g_iLastKey = 'S';
}

void CRarsApp::OnCarNextCar() 
{
  g_iLastKey = UP;
}

void CRarsApp::OnCarPrvCar() 
{
  g_iLastKey = DOWN;
}

void CRarsApp::OnCarAnyOvertaking() 
{
  g_iLastKey = 'O';
}

void CRarsApp::OnCarForPosition() 
{
  g_iLastKey = 'P';
}

void CRarsApp::OnCarNobody() 
{
  g_iLastKey = 'I';
}

void CRarsApp::OnCarManual() 
{
  g_iLastKey = 'U';
}


void CRarsApp::OnViewClassic() 
{
  m_pDlgRars->SetViewMode( MODE_CLASSIC );
}

void CRarsApp::OnViewOpenglFullscreen() 
{
  m_pDlgRars->SetViewMode( MODE_OPENGL_FULLSCREEN );
}

void CRarsApp::OnViewOpenglWindow() 
{
  m_pDlgRars->SetViewMode( MODE_OPENGL_WINDOW );
}

void CRarsApp::OnViewTelemetry() 
{
  m_pDlgRars->SetViewMode( MODE_TELEMETRY );
}

void CRarsApp::OnViewZoom() 
{
  m_pDlgRars->SetViewMode( MODE_ZOOM );
}

void CRarsApp::OnZoomIn() 
{
  g_iLastKey = '+';
}

void CRarsApp::OnViewTrajectory() 
{
  g_iLastKey = 'T';
}

void CRarsApp::OnViewDriverNames() 
{
  g_iLastKey = 'N';
}

void CRarsApp::OnZoomOut() 
{
  g_iLastKey = '-';
}

void CRarsApp::OnSettingsSave() 
{
  g_oRarsIni.SaveSettings();
}

void CRarsApp::OnOpenglView() 
{
  g_iLastKey = 'V';
}

void CRarsApp::OnMoveUp() 
{
  g_iLastKey = ARROW_UP;
}

void CRarsApp::OnMoveDown() 
{
  g_iLastKey = ARROW_DOWN;
}

void CRarsApp::OnMoveLeft() 
{
  g_iLastKey = ARROW_LEFT;
}

void CRarsApp::OnMoveRight() 
{
  g_iLastKey = ARROW_RIGHT;
}

void CRarsApp::OnRotateDown() 
{
  g_iLastKey = ROTATE_DOWN;
}

void CRarsApp::OnRotateLeft() 
{
  g_iLastKey = ROTATE_LEFT;	
}

void CRarsApp::OnRotateRight() 
{
  g_iLastKey = ROTATE_RIGHT;	
}

void CRarsApp::OnRotateUp() 
{
  g_iLastKey = ROTATE_UP;	
}

void CRarsApp::OnRotateReset() 
{
  g_iLastKey = ROTATE_RESET;	
}


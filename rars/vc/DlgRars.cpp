//--------------------------------------------------------------------------
//
//    FILE: DlgRars
//
//    - CAboutDlg :
//         About dialog
//    - CDlgRars :
//         Dialog that displays the views of the race
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "g_global.h"
#include "Vc_rars.h"
#include "DlgRars.h"

//--------------------------------------------------------------------------
//                          Class CDlgRars
//--------------------------------------------------------------------------

CDlgRars::CDlgRars(CWnd* pParent /*=NULL*/)
    : CDlgRarsCommon(CDlgRars::IDD, pParent)
{
  //{{AFX_DATA_INIT(CDlgRars)
	//}}AFX_DATA_INIT
  // Note that LoadIcon does not require a subsequent DestroyIcon in Win32

  // Value from GetClientRect( m_OrigClientRect );
  m_OrigClientRect.top = 0;
  m_OrigClientRect.bottom = 452;
  m_OrigClientRect.left= 0;
  m_OrigClientRect.right = 630;

  LPCTSTR lpTableName = MAKEINTRESOURCE( IDR_RARS_ACCELERATOR );
  m_hAccel = LoadAccelerators( theApp.m_hInstance, lpTableName );
}

void CDlgRars::DoDataExchange(CDataExchange* pDX)
{
  CDialog::DoDataExchange(pDX);
  //{{AFX_DATA_MAP(CDlgRars)
	//}}AFX_DATA_MAP
}

BEGIN_MESSAGE_MAP(CDlgRars, CDialog)
    //{{AFX_MSG_MAP(CDlgRars)
    ON_WM_SYSCOMMAND()
    ON_WM_PAINT()
    ON_WM_QUERYDRAGICON()
    ON_WM_TIMER()
    ON_WM_SIZE()
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgRars message handlers

BOOL CDlgRars::OnInitDialog()
{
  CDlgRarsCommon::OnInitDialog();
  
  if( g_oRarsIni.m_iMaximized )
  {
    ShowWindow( SW_SHOWMAXIMIZED );
  }

  vc_main_part0( this );
  vc_main_part1();
  InitDialogPart2();
  
  return TRUE;
}

void CDlgRars::OnSysCommand(UINT nID, LPARAM lParam)
{
  CDialog::OnSysCommand(nID, lParam);
}

void CDlgRars::OnPaint() 
{
  CDlgRarsCommon::OnPaint();
}

// The system calls this to obtain the cursor to display while the user drags
//  the minimized window.
HCURSOR CDlgRars::OnQueryDragIcon()
{
  return (HCURSOR) m_hIcon;
}

void CDlgRars::OnTimer(UINT nIDEvent) 
{
  Refresh();
  CDialog::OnTimer(nIDEvent);
}

void CDlgRars::Refresh()
{
  int iViewMode = g_ViewManager->GetViewMode();
  
  for( int i=0;i<5; i++ ) 
  {
    while( g_bToRefresh==FALSE ) 
    {
      if( !vc_main_part2() ) 
      {
        g_bToRefresh = FALSE;
        KillTimer( 1 );
        return; 
      }
    }
    g_bToRefresh = FALSE;
    if( iViewMode==MODE_ZOOM || iViewMode==MODE_TELEMETRY || iViewMode==MODE_CLASSIC )
    {
      m_StaticBitmap2D.Invalidate(FALSE);
      m_StaticBitmapClassic.Invalidate(FALSE);
      m_StaticBitmapBoard.Invalidate(FALSE);
      UpdateWindow();
      // break;
    }
    if( g_RaceStage==INIT_CARS )
    {
      break;
    }
  }

  if( iViewMode==MODE_OPENGL_WINDOW )
  {
    m_StaticBitmapClassic.Invalidate(FALSE);
    m_StaticBitmapBoard.Invalidate(FALSE);
    UpdateWindow();
  }
}

void CDlgRars::OnCancel() 
{
  g_bToRefresh = FALSE;
  vc_main_part3();
  CDialog::OnCancel();
}

void CDlgRars::OnOK()
{
}

void CDlgRars::OnSize(UINT nType, int cx, int cy) 
{
  CDlgRarsCommon::OnSize(nType, cx, cy);
}

BOOL CDlgRars::PreTranslateMessage(MSG* pMsg) 
{
  return CDlgRarsCommon::PreTranslateMessage(pMsg); 
}

//--------------------------------------------------------------------------
//                        Class CDlgRarsCommon
//--------------------------------------------------------------------------

/**
 * Constructor
 */
CDlgRarsCommon::CDlgRarsCommon( UINT nIDTemplate, CWnd* pParentWnd /*= NULL*/ )
: CDialog( nIDTemplate, pParentWnd )
{
  m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
  m_hBitmap2D = 0;
  m_hBitmapClassic = 0;
  m_hBitmapBoard = 0;
  m_bRectInit = FALSE;
  m_fOffX = 0;
  m_fOffY = 0;
  m_bTimerInit = FALSE;
}

/**
 * Initialisation of the Dialog box
 */
BOOL CDlgRarsCommon::OnInitDialog()
{
  CDialog::OnInitDialog();

  // Set the icon for this dialog.  The framework does this automatically
  //  when the application's main window is not a dialog
  SetIcon(m_hIcon, TRUE);         // Set big icon
  SetIcon(m_hIcon, FALSE);        // Set small icon

  VERIFY(m_StaticBitmap2D.SubclassDlgItem(IDC_BITMAP_2D, this));
  VERIFY(m_StaticBitmapClassic.SubclassDlgItem(IDC_BITMAP_CLASSIC, this));
  VERIFY(m_StaticBitmapBoard.SubclassDlgItem(IDC_BITMAP_BOARD, this));
  InitDialog( g_oRarsIni.m_iViewMode );

  ////
  //// Code for Resizing
  ////
  m_bRectInit = TRUE;
  m_StaticBitmap2D.GetWindowRect( m_OrigStaticBitmap2D ) ;
  m_StaticBitmapClassic.GetWindowRect( m_OrigStaticBitmapClassic );
  m_StaticBitmapBoard.GetWindowRect( m_OrigStaticBitmapBoard );
    
  ScreenToClient(m_OrigStaticBitmap2D);
  ScreenToClient(m_OrigStaticBitmapClassic);
  ScreenToClient(m_OrigStaticBitmapBoard);

  return TRUE;
}

/**
 * Hide, show needed items
 */
void CDlgRarsCommon::InitDialog( int iViewMode )
{
  g_bToRefresh = FALSE;

  m_StaticBitmap2D.ShowWindow( SW_SHOWNORMAL );
  m_StaticBitmapClassic.ShowWindow( SW_SHOWNORMAL );
  m_StaticBitmapBoard.ShowWindow( SW_SHOWNORMAL );

  ////
  //// Due to something I don't understand, the buttons
  //// do not work if OpenGL is enabled.
  //// This code hide them
  ////
  
  if( iViewMode==MODE_OPENGL_FULLSCREEN 
   || iViewMode==MODE_TRACK_EDITOR
   || iViewMode==MODE_OPENGL_WINDOW )
  {
    m_StaticBitmap2D.ShowWindow( SW_HIDE );
  }
  if( iViewMode==MODE_CLASSIC 
   || iViewMode==MODE_OPENGL_FULLSCREEN 
   || iViewMode==MODE_TRACK_EDITOR
   || iViewMode==MODE_TELEMETRY )
  {
    m_StaticBitmapClassic.ShowWindow( SW_HIDE );
    m_StaticBitmapBoard.ShowWindow( SW_HIDE );
  }
}

/**
 * Associate bitmaps and mfc widgets
 */
void CDlgRarsCommon::InitDialogPart2()
{
  m_StaticBitmap2D.SetBitmap( m_hBitmap2D );
  m_StaticBitmapClassic.SetBitmap( m_hBitmapClassic );
  m_StaticBitmapBoard.SetBitmap( m_hBitmapBoard );
}

/**
 * Set a new Mode
 *
 * @param mode          the new mode
 */
void CDlgRarsCommon::SetViewMode( int mode )
{
  g_ViewManager->DeleteAllViews();
  g_ViewManager->m_iViewMode = mode;
  g_ViewManager->InitAllViews(); 
  // ResizeAllViews( m_fOffX, m_fOffY );
  InitDialog( mode );
  InitDialogPart2();
}


/**
 * If you add a minimize button to your dialog, you will need the code below
 *  to draw the icon.  For MFC applications using the document/view model,
 *  this is automatically done for you by the framework.
 */
void CDlgRarsCommon::OnPaint() 
{
  if (IsIconic())
  {
    CPaintDC dc(this); // device context for painting

    SendMessage(WM_ICONERASEBKGND, (WPARAM) dc.GetSafeHdc(), 0);

    // Center icon in client rectangle
    int cxIcon = GetSystemMetrics(SM_CXICON);
    int cyIcon = GetSystemMetrics(SM_CYICON);
    CRect rect;
    GetClientRect(&rect);
    int x = (rect.Width() - cxIcon + 1) / 2;
    int y = (rect.Height() - cyIcon + 1) / 2;

    // Draw the icon
    dc.DrawIcon(x, y, m_hIcon);
  }
  else
  {
    CDialog::OnPaint();
    if( !m_bTimerInit )     
    {   // trick for NT 4.0
      m_bTimerInit = TRUE;
      SetTimer( 1, 10, 0 );
    }
  }
}

/**
 * Event received when the window is resized
 */
void CDlgRarsCommon::OnSize(UINT nType, int cx, int cy) 
{
  cx = max( cx, m_OrigClientRect.right ); 
  cy = max( cy, m_OrigClientRect.bottom ); 

  int offx = cx - m_OrigClientRect.right; 
  int offy = cy - m_OrigClientRect.bottom; 
  m_fOffX = offx = offx-offx%8;
  m_fOffY = offy = offy-offy%4;
  
  if( m_bRectInit && nType!=SIZE_MINIMIZED ) 
  {
    CRect rStaticBitmap2D       = m_OrigStaticBitmap2D,
          rStaticBitmapClassic  = m_OrigStaticBitmapClassic,
          rStaticBitmapBoard    = m_OrigStaticBitmapBoard;

    rStaticBitmap2D.InflateRect( 0, 0, offx, offy );

    rStaticBitmapClassic.OffsetRect( offx, 0 );
    rStaticBitmapClassic.InflateRect( 0, 0, 0, offy );
    rStaticBitmapBoard.OffsetRect( 0, offy );
    rStaticBitmapBoard.InflateRect( 0, 0, offx, 0 );

    m_StaticBitmap2D.MoveWindow( rStaticBitmap2D, FALSE );
    m_StaticBitmapClassic.MoveWindow( rStaticBitmapClassic, FALSE );
    m_StaticBitmapBoard.MoveWindow( rStaticBitmapBoard, FALSE );

    ResizeAllViews( offx, offy );
    m_StaticBitmap2D.SetBitmap( m_hBitmap2D );
    m_StaticBitmapClassic.SetBitmap( m_hBitmapClassic );
    m_StaticBitmapBoard.SetBitmap( m_hBitmapBoard );
  }
  CDialog::OnSize(nType, cx, cy);
    
  Invalidate( TRUE );
  UpdateWindow();
}

/**
 * Pretranslate the message to allow key accelerators
 */
BOOL CDlgRarsCommon::PreTranslateMessage(MSG* pMsg) 
{
  if(!(m_hAccel &&::TranslateAccelerator(this->m_hWnd, m_hAccel, pMsg)))
    return CDialog::PreTranslateMessage(pMsg);
  else
   return TRUE;
}

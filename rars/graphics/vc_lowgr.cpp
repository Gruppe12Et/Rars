/**
 * VC_LOWGR.CPP (VC++ specific)
 *              (not portable)
 * 
 * This file gathers together all the functions of the classes that
 * are not portable:
 * - TPalette
 * - TViewManager
 * - TLowGraphic
 * 
 * Version       Author          Date
 * 0.1      Marc Gueury     05 /08 /96
 * 
 * 
 * Bug: There is a very annoying bug on Win98 that disallow to use
 *      CreateDIBSection with a negative biHeight. 
 *      It works only sometimes depending on Windows mood.
 * WA:  The work around used here is to slip the bitmap at the hand.
 *      And Windows will split it again...
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "stdafx.h"
#include <dos.h>
#include <conio.h>
#include <direct.h>
#include "g_global.h"
#include "g_tracked.h"
#include "Vc_rars.h"
#include "DlgRars.h"
#include "DlgInitCars.h"
#include "DlgTrackEditor.h"

//--------------------------------------------------------------------------
//                            T Y P E S
//--------------------------------------------------------------------------

typedef struct 
{
  BITMAPINFOHEADER    bmiHeader;
  RGBQUAD             bmiColors[256];
} TBitmapInfo;

class VcLowGraphic
{
public:
  TLowGraphic *        m_pLowGraphic;
  char *               m_RealBitmap;
  char *               m_Bitmap;
  HBITMAP              m_hBitmap;

  void FlipY();
};


/**
 * (Windows) To calculate the fps
 */
class Ticker
{
  public:
    inline void Init(void);
    inline void Update(void);
    inline float Passed(void);

  protected:
    LARGE_INTEGER now;
    LARGE_INTEGER temp;
    float res; 
};

//--------------------------------------------------------------------------
//                            G L O B A L
//--------------------------------------------------------------------------

TBitmapInfo         g_BitmapInfo;
static int          g_iViewCpt = 0;
static VcLowGraphic g_LowGraphic[3];

// Dialog box
CDlgInitCars        g_dlgInitCars;
CDlgRarsCommon *    g_dlgRars;
bool                g_bToRefresh;

// OpenGL
CButton   g_GL_CWnd;
HDC       g_GL_hDC         = 0;     
HGLRC     g_GL_hRC         = 0;
Ticker    g_Ticker;

//--------------------------------------------------------------------------
//                              M A I N
//--------------------------------------------------------------------------

void vc_main_part0( CDlgRarsCommon * _pDlg )
{
  g_dlgRars = _pDlg;
}

void vc_main_part1()
{
  main_part1();
}

BOOL vc_main_part2()
{
  BOOL res = main_part2();
  switch(g_RaceStage) 
  {
    case INIT_CARS:
      g_dlgInitCars.DoModal();
      break;
    case INIT_CARS2:
      g_dlgInitCars.EndDialog(0);
      break;
  }  
  return res;
}

void vc_main_part3()
{
  main_part3();
}

void ResizeAllViews( int x, int y )
{
  if( g_ViewManager != NULL )
  {
    g_ViewManager->ResizeAllViews( x, y );
  }
}

//--------------------------------------------------------------------------
//                           Class Ticker
//--------------------------------------------------------------------------

inline void Ticker::Init()
{
  LARGE_INTEGER freq;

  QueryPerformanceFrequency(&freq);
  QueryPerformanceCounter(&now);
  res = (float) (1.0f / (double) freq.QuadPart);
}

inline void Ticker::Update()
{
  QueryPerformanceCounter(&now);
}

inline float Ticker::Passed()
{
  QueryPerformanceCounter(&temp);

  return (temp.QuadPart - now.QuadPart) * res * 1000.0f;
}

//--------------------------------------------------------------------------
//                           Class TPalette
//--------------------------------------------------------------------------

/**
 * Set the palette.
 */
void TPalette::Set()
{
  // set the 16 defined colors
  for( int i=0; i<=m_ColorNb; i++ ) 
  {
    g_BitmapInfo.bmiColors[i].rgbBlue  = unsigned char( 255-(63-m_Color[i].b)*4 );
    g_BitmapInfo.bmiColors[i].rgbGreen = unsigned char( 255-(63-m_Color[i].g)*4 );
    g_BitmapInfo.bmiColors[i].rgbRed   = unsigned char( 255-(63-m_Color[i].r)*4 );
  }
}

//--------------------------------------------------------------------------
//                           Class TLowGraphic
//--------------------------------------------------------------------------

/**
 * Memory : Allocate some memory for the bitmap
 * Direct Access: indicate the adress of the screen (see: screen_size_x)
 */
void TLowGraphic::MallocBitmap()
{
  //// The function CreateDIBSection has a random behavior on Win98 only
  //// when the biHeight has a negative value.
  ////

  // set the 16 defined colors
  g_BitmapInfo.bmiHeader.biWidth         = m_SizeX; 
  g_BitmapInfo.bmiHeader.biHeight        = m_SizeY; 
  g_BitmapInfo.bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
  g_BitmapInfo.bmiHeader.biSizeImage     = 0;
  g_BitmapInfo.bmiHeader.biCompression   = BI_RGB; 
  g_BitmapInfo.bmiHeader.biPlanes        = 1; 
  g_BitmapInfo.bmiHeader.biBitCount      = 8; 
  g_BitmapInfo.bmiHeader.biXPelsPerMeter = 0;
  g_BitmapInfo.bmiHeader.biYPelsPerMeter = 0; 
  g_BitmapInfo.bmiHeader.biClrUsed       = 16; 
  g_BitmapInfo.bmiHeader.biClrImportant  = 16; 
  
  HDC hdc = GetDC(NULL);
  HBITMAP hbitmap = CreateDIBSection(
                      hdc,
                      (BITMAPINFO *)&g_BitmapInfo,
                      DIB_RGB_COLORS,
                      (void **)&(m_Bitmap),
                      0, 0
  );
  ReleaseDC(NULL, hdc); 

  //// WA for bug biHeight 
  g_LowGraphic[g_iViewCpt].m_hBitmap = hbitmap;

  if( hbitmap!=0 )
  {
    if( g_iViewCpt==0 )
      g_dlgRars->m_hBitmap2D = hbitmap;
    else if( g_iViewCpt==1 )
      g_dlgRars->m_hBitmapClassic = hbitmap;
    else if( g_iViewCpt==2 )
      g_dlgRars->m_hBitmapBoard = hbitmap;

    //// WA for bug biHeight 
    g_LowGraphic[g_iViewCpt].m_pLowGraphic = this;
    g_LowGraphic[g_iViewCpt].m_RealBitmap = m_Bitmap;
    g_LowGraphic[g_iViewCpt].m_hBitmap    = hbitmap;
    m_Bitmap = new char[m_SizeX*m_SizeY];
    g_LowGraphic[g_iViewCpt].m_Bitmap     = m_Bitmap;
  } 

  g_iViewCpt++;
}

/**
 * Memory : Free the memory of the bitmap
 * Direct Access: -
 */
void TLowGraphic::FreeBitmap()
{
  //// WA for bug biHeight 
  if( m_Bitmap )
  {
    free( m_Bitmap );
    m_Bitmap = NULL;
  }
}

//--------------------------------------------------------------------------
//                         Class VcLowGraphic
//--------------------------------------------------------------------------

/**
 * WorkAround for bug biHeight 
 */
void VcLowGraphic::FlipY()
{
  uint32 * p1, *p2;

  int xnb = m_pLowGraphic->m_SizeX/4; // we copy 32 bits each time
  int ynb = m_pLowGraphic->m_SizeY;

  for( int y=0; y<ynb; y++ )
  {
    p1 = (uint32 *) m_RealBitmap + y*xnb;

    p2 = (uint32 *) m_Bitmap+ (ynb-y-1)*xnb;
    for( int x=0; x<xnb; x++ )
    {
      *p1++ = *p2++;
    }
  }
}

//--------------------------------------------------------------------------
//                         Class TViewManager
//--------------------------------------------------------------------------

/**
 * Initialize all the views
 */
void TViewManager::InitAllViews()
{
  g_iViewCpt = 0;
  m_iSizeOffX = g_dlgRars->m_fOffX;
  m_iSizeOffY = g_dlgRars->m_fOffY;
  m_iLastViewMode = m_iViewMode;

  int x = m_iSizeOffX;
  int y = m_iSizeOffY;

  if( m_iViewMode==MODE_CLASSIC )
  {
    TViewClassical * view = new TViewClassical(608+x,430+y);
    view->m_OptionShowBoard = 1;
    AddView( view );
  }
  else if( m_iViewMode==MODE_OPENGL_FULLSCREEN )
  {
    TView3D * view = new TView3D(640+x,480+y,0,0,0); 
    view->m_OptionShowBoard = 1;
    AddView( view );
  }
  else if( m_iViewMode==MODE_TRACK_EDITOR )
  {
    AddView( new TViewTrackEditor(440+x,250+y,170,10) );
  }
  else if( m_iViewMode==MODE_OPENGL_WINDOW )
  {
    AddView( new TView3D(379+x,256+y,9,8,0) );
    AddView( new TViewClassical(220,250+y) );
    AddView( new TViewBoard(600+x,160) );
  }
  else if( m_iViewMode==MODE_TELEMETRY )
  {
    AddView( new TViewTelemetry(608+x,430+y,0) );
  }
  else
  {
    AddView( new TView2D(372+x,250+y,0) );
    AddView( new TViewClassical(220,250+y) );
    AddView( new TViewBoard(600+x,160) );
  }
  SetFastModeParam( 15 );
}

/**
 * Resize all the views
 *
 * @param x             (in) size x of the window
 * @param y             (in) size y of the window
 */
void TViewManager::ResizeAllViews( int x, int y )
{
  g_iViewCpt = 0;

  if( m_iSizeOffX!=x || m_iSizeOffY!=y || m_iLastViewMode!=m_iViewMode )
  {
    m_iSizeOffX = x;
    m_iSizeOffY = y;
    m_iLastViewMode = m_iViewMode;

    if( m_iViewMode==MODE_CLASSIC )
    {
      m_Views[0]->Resize( 608+x,430+y );
    }
    else if( m_iViewMode==MODE_OPENGL_FULLSCREEN )
    {
      // Crash that I can't explain -> 8
      TView3D * view_3d = (TView3D *) m_Views[0];
      view_3d->Resize( 640+x,480+y,8,0 );
    }
    else if( m_iViewMode==MODE_TRACK_EDITOR )
    {
      // Crash that I can't explain -> 8
      TViewTrackEditor * view = (TViewTrackEditor *) m_Views[0];
      view->Resize( 440+x,250+y,170,10 );
    }
    else if( m_iViewMode==MODE_OPENGL_WINDOW )
    {
      // delete m_Views[0];
      // m_Views[0] = new TView3D(388+x,264+y,9,8,0);
      TView3D * view_3d = (TView3D *) m_Views[0];
      view_3d->Resize( 379+x,256+y,9,8 );
      m_Views[1]->Resize( 220  ,250+y );
      m_Views[2]->Resize( 600+x,160 );
    }
    else if( m_iViewMode==MODE_TELEMETRY )
    {
      m_Views[0]->Resize( 608+x,430+y );
    }
    else
    {
      m_Views[0]->Resize( 372+x,250+y );
      m_Views[1]->Resize( 220  ,250+y );
      m_Views[2]->Resize( 600+x,160 );
    }
  }
}

/**
 * Initialize the screen
 */
void TViewManager::InitScreen()
{
  // Init the timer
  g_Ticker.Init();
}

/**
 * Set the normal screen mode
 */
void TViewManager::CloseScreen()
{
//  pDlg->bitmap.DeleteObject();
}

/**
 * Copy the view 'view_nr' to the screen 
 *
 * @param view_nr       (in) the view to copy
 */
void TViewManager::CopyViewToScreen( int view_nr )
{
  if( view_nr==2
   || m_iViewMode==MODE_CLASSIC
   || m_iViewMode==MODE_TELEMETRY )
  {
    g_bToRefresh = TRUE;
  }

   //// WA for bug biHeight 
  if(g_LowGraphic[view_nr].m_hBitmap!=0 )
  {
    g_LowGraphic[view_nr].FlipY();
  }

  if( view_nr==0 )
  {
    static int iFrames = 0;
    iFrames ++;

    // is this correct?
    if( g_Ticker.Passed()>=1000 )
    {
      m_fFps = (m_fFps+iFrames)/2; 
      g_Ticker.Update();
      iFrames = 0;
    }
  }
}

//---------------------------------------------------------------------------
// OpenGL
//---------------------------------------------------------------------------

/**
 * Set the desired format renderer of OpenGL
 *
 * @param hdc       (in) handle to the graphic context
 */
BOOL GL_bSetupPixelFormat( HDC hdc )
{
  PIXELFORMATDESCRIPTOR pfd, *ppfd;
  int pixelformat;

  memset( &pfd, 0, sizeof(pfd) );
  ppfd = &pfd;

  ppfd->nSize = sizeof(PIXELFORMATDESCRIPTOR);
  ppfd->nVersion = 1;
  ppfd->dwFlags = PFD_DRAW_TO_WINDOW | PFD_GENERIC_ACCELERATED |
                  PFD_SUPPORT_OPENGL | PFD_SWAP_EXCHANGE |
                  PFD_DOUBLEBUFFER |   PFD_STEREO_DONTCARE;
  ppfd->dwLayerMask = PFD_MAIN_PLANE;
  ppfd->iPixelType = PFD_TYPE_RGBA;
  ppfd->cColorBits = 24;
  ppfd->cRedBits = 8;
  ppfd->cRedShift = 16;
  ppfd->cGreenBits = 8;
  ppfd->cGreenShift = 8;
  ppfd->cBlueBits = 8;
  ppfd->cBlueShift = 0;
  ppfd->cAlphaBits = 0;
  ppfd->cAlphaShift = 0;
  ppfd->cAccumBits = 48;
  ppfd->cAccumRedBits = 16;
  ppfd->cAccumGreenBits = 16;
  ppfd->cAccumBlueBits = 16;
  ppfd->cAccumAlphaBits = 0;
  ppfd->cDepthBits = 32;
  ppfd->cStencilBits = 0;
  ppfd->cAuxBuffers = 0;
  ppfd->iLayerType = PFD_MAIN_PLANE;
  ppfd->bReserved = 0;
  ppfd->dwLayerMask = 0;
  ppfd->dwVisibleMask = 0;
  ppfd->dwDamageMask = 0;

  pixelformat = ChoosePixelFormat(hdc, ppfd);

  if ((pixelformat = ChoosePixelFormat(hdc, ppfd)) == 0) 
  {
    MessageBox(NULL, "ChoosePixelFormat failed", "Error", MB_OK);
    return FALSE;
  }

  if (pfd.dwFlags & PFD_NEED_PALETTE) 
  {
    MessageBox(NULL, "Needs palette", "Error", MB_OK);
    return FALSE;
  }

  if (SetPixelFormat(hdc, pixelformat, ppfd) == FALSE) 
  {
    MessageBox(NULL, "SetPixelFormat failed", "Error", MB_OK);
    return FALSE;
  }

  return TRUE;
}

/**
 * Create the Widget Renderer of OpenGL
 *
 * @param x         (in) size x of the widget
 * @param y         (in) size y
 * @param offx      (in) pos x 
 * @param offy      (in) posy
 */
void GL_InitWidget(int x, int y, int offx, int offy)
{
  g_GL_CWnd.CreateEx
  (
      0, NULL, "OpenGL Rars", WS_CHILD | WS_CLIPSIBLINGS | WS_CLIPCHILDREN,
      CRect(offx, offy, x+offx, y+offy), g_dlgRars, NULL, NULL
  );

  g_GL_CWnd.ShowWindow( SW_SHOW );
  g_GL_CWnd.UpdateWindow();
  CRect rect;
  g_GL_CWnd.GetClientRect(&rect);
  g_GL_CWnd.SetFocus();

  g_GL_hDC = GetDC( g_GL_CWnd.m_hWnd );
  if( !GL_bSetupPixelFormat(g_GL_hDC) )
     PostQuitMessage (0);

  g_GL_hRC = wglCreateContext(g_GL_hDC);
  wglMakeCurrent(g_GL_hDC, g_GL_hRC);
}

void GL_DestroyWidget()
{
  g_GL_CWnd.DestroyWindow();
  wglDeleteContext( g_GL_hRC );
}

void GL_SwapWidget()
{
  SwapBuffers( g_GL_hDC );
}

HDC getHDC()
{
  return g_GL_hDC;
}

//---------------------------------------------------------------------------
// RarsIni
//---------------------------------------------------------------------------

/**
 * Save the maximize property of the window in Rars.ini
 */
void RarsIni::SaveWindowSettings()
{
  WINDOWPLACEMENT wndpl;
  if( g_dlgRars->GetWindowPlacement( &wndpl ) )
  {
    if( wndpl.showCmd==SW_SHOWMAXIMIZED )
    {
      m_iMaximized = 1;
    }
    else
    {
      m_iMaximized = 0;
    }
  }
}

/**
 * Stop all the timer (just before to show an error)
 */
void StopAllTimer()
{
  if( g_dlgRars!=NULL )
  {
    g_dlgRars->KillTimer( 1 );
  }
}


//---------------------------------------------------------------------------
// Os 
//---------------------------------------------------------------------------

/**
 * Add a message in the initilisation window
 */
void Os::ShowInitMessage( const char *format, ... )
{
  char s[1024];

  va_list argList;
  
  va_start(argList, format);
  vsprintf( s, format, argList);
  va_end(argList);
  
  g_dlgInitCars.m_ListMessage.AddString( s );
  g_dlgInitCars.UpdateWindow();
}

/**
 * Create a new directory
 */
void Os::CreateDirectory( const char * dir )
{
  _mkdir( dir );
}

//---------------------------------------------------------------------------
// TrackEditor
//---------------------------------------------------------------------------

/**
 * Call the real refresh method
 */
void TrackEditor::Refresh( bool bRebuildList )
{
  CDlgTrackEditor * dlg = (CDlgTrackEditor *) g_dlgRars;
  dlg->Refresh( bRebuildList );
}

/**
 * Get and Set the data inside the TrackEditor structure on the screen
 */
void TrackEditor::UpdateData( bool bGetData )
{
 CDlgTrackEditor * dlg = (CDlgTrackEditor *) g_dlgRars;

 if( bGetData )
 {
   dlg->m_PropValue.GetWindowText( m_sPropValue, 256 );
 }
 else
 {
   dlg->m_PropValue.SetWindowText( m_sPropValue );
 }
}

/**
 * Change the current edited properties when clicking on a list
 */
void TrackEditor::EditProperty()
{
 CDlgTrackEditor * dlg = (CDlgTrackEditor *) g_dlgRars;

 if( m_pStructProp!=NULL )
  {
    dlg->m_PropName.SetWindowText( m_pStructProp->sXmlTag );
    dlg->m_Description.SetWindowText( m_pStructProp->sDescription );
    dlg->m_PropValue.SetWindowText( m_pStructProp->GetString(m_pStructBase) );
    dlg->m_PropValue.SetFocus();
  }
}

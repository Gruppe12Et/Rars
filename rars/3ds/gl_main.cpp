/*
 * Test program for the 3DS loader
 */
#pragma comment(lib, "opengl32.lib")
#pragma comment(lib, "glu32.lib")

#include <windows.h>
#include <GL\gl.h>
#include <GL\glu.h>
#include <math.h>
#include <stdio.h>

#define FULLSCREEN                              (0)
#define WIDTH                                   (640)
#define HEIGHT                                  (480)
#define BITDEPTH                                (16)
#define START_WITH_ANIMATION                    (0)

/********************************************************************************/

#include "Model_3DS.h"

/*******************************************************************************/


/*******************************************************************************

        Main

*******************************************************************************/

/* image map structures */
struct DIB2D
{
        BITMAPINFOHEADER *Info;
        RGBQUAD *palette;
        BYTE    *bits;
};

struct GLTXTLOAD
{
        GLint format;
        GLint perpixel;
        GLint Width;
        GLint Height;
        BYTE* bits;
};


Model_3DS m;

static char szAppName[] = "3DWin";
static GLsizei glnWidth, glnHeight;
static GLdouble gldAspect;

static int face_number = 0;

static HPALETTE hPalette        = NULL;
static GLfloat nSize            = 0.0f;
static GLfloat nCol             = 0.0f;
static GLfloat nTop             = 1.0f;
static GLfloat nBottom          = 0.0f;
static int bFlag                = 1;
static dFlag                    = 1;
static aFlag                    = START_WITH_ANIMATION; // animation

LONG WINAPI MainWndProc (HWND, UINT, WPARAM, LPARAM);
BOOL CreateGLWindow(char* title, int width, int height, int bits, bool fullscreenflag);
static void SetDCPixelFormat (HWND hwnd,HDC hdc);
static BOOL ChangeResolution(LONG w, LONG h, LONG bitdepth, int mode);
static void InitializeRC();
static void Resize();
static void DrawScene (HDC hdc, float nAngleX, float nAngleY, float nAngleZ, float nDistanceX, float nDistanceY, float nDistanceZ);

#define ROT_STEP        (1.0f)
#define MOVE_STEP       (1.0f)

// Display Lists
#define LIGHT_LIST    (1)
#define POLYGON_LIST  (2)

int WINAPI WinMain (HINSTANCE hInstance, HINSTANCE hPrevInstance,
                    LPSTR lpszCmdLine, int nCmdShow)
{
        WNDCLASSEX  wndclass;
        HWND hWnd;
        MSG msg;

        /* Register the frame class */
        wndclass.cbSize        = sizeof (wndclass);
        wndclass.style         = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
        wndclass.lpfnWndProc   = (WNDPROC) MainWndProc;
        wndclass.cbClsExtra    = 0;
        wndclass.cbWndExtra    = 0;
        wndclass.hInstance     = hInstance;
        wndclass.hIcon         = 0;
        wndclass.hCursor       = LoadCursor (NULL, IDC_ARROW);
        wndclass.hbrBackground = NULL;
        wndclass.lpszMenuName  = NULL;
        wndclass.lpszClassName = szAppName;
        wndclass.hIconSm       = 0;


        if (!RegisterClassEx(&wndclass))
                return FALSE;

        if ( FULLSCREEN )
        {
      if (!ChangeResolution(WIDTH,HEIGHT,BITDEPTH, 0))
        return 0;
        }

        if ( FULLSCREEN )
        {
      hWnd = CreateWindowEx(WS_EX_APPWINDOW /*| WS_EX_TOPMOST */,
                  szAppName, szAppName,
                  WS_POPUP |  WS_CLIPSIBLINGS,
                  0, 0, WIDTH, HEIGHT,
                  NULL, NULL, hInstance, NULL);
        }
        else
        {
      LONG  h;
      LONG  w;

      h = GetSystemMetrics(SM_CYCAPTION) + GetSystemMetrics(SM_CXDLGFRAME)*2 + HEIGHT;
      w = GetSystemMetrics(SM_CXDLGFRAME)*2 + WIDTH;

      hWnd = CreateWindowEx(WS_EX_APPWINDOW,
                  szAppName, szAppName,
                  WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN | WS_CLIPSIBLINGS,
                  1, 1, w, h,
                  NULL, NULL, hInstance, NULL);
        }


        /* Make sure window was created */
        if (!hWnd)
                return FALSE;

        ShowWindow (hWnd, nCmdShow);
        UpdateWindow (hWnd);

    char title[4096];
    sprintf(title, "%s (%d faces)", szAppName, face_number);
    SetWindowText(hWnd, title);

    while (GetMessage (&msg, NULL, 0, 0))
        {
      TranslateMessage (&msg);
      DispatchMessage (&msg);
    }
    return msg.wParam;
}


LONG WINAPI MainWndProc (HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
    static HDC hdc;
    static HGLRC hrc;
    PAINTSTRUCT ps;
    static float nAngleX = 0.0f;
    static float nAngleY = 0.0f;
    static float nAngleZ = 0.0f;
    static float nDistanceX = 0.0f;
    static float nDistanceY = 0.0f;
    static float nDistanceZ = -8.0f;
    static INT nTimer;
    int n;

    switch (msg)
  {

    case WM_CREATE:
        {
      hdc = GetDC (hwnd);
            SetDCPixelFormat (hwnd,hdc);
            hrc = wglCreateContext (hdc);
            wglMakeCurrent (hdc, hrc);
      SetForegroundWindow(hwnd);            // Slightly Higher Priority
            InitializeRC ();

            nTimer = SetTimer (hwnd, 1, 50, NULL);
            return 0;
        }
        break;

        case WM_SIZE:
        {
      glnWidth = (GLsizei) LOWORD (lParam);
            glnHeight = (GLsizei) HIWORD (lParam);
      Resize();
      return 0;
        }
        break;

        case WM_PAINT:
        {
      BeginPaint (hwnd, &ps);
            DrawScene (hdc, nAngleX, nAngleY, nAngleZ, nDistanceX, nDistanceY, nDistanceZ);
            EndPaint (hwnd, &ps);
            return 0;
    }
        break;

    case WM_TIMER:
        {
      if(aFlag == 0)
        break;

      nAngleZ += 1;
            if (nAngleZ >= 360)
        nAngleZ -= 360;

      if (bFlag == 1)
        nSize += 0.05f;

      nCol += 0.01f;
            if (nSize >= nTop)
        bFlag = 0;

      if (bFlag == 0)
        nSize -= 0.05f;
      nCol -= 0.01f;

      if (nSize <= nBottom)
        bFlag = 1;

      InvalidateRect (hwnd, NULL, FALSE);

      return 0;
    }
        break;

    case WM_QUERYNEWPALETTE:
        {
      if (hPalette != NULL)
            {
        if (n = RealizePalette (hdc))
          InvalidateRect (hwnd, NULL, FALSE);
        return n;
      }
    }
        break;

        case WM_PALETTECHANGED:
                {
          if ((hPalette != NULL) && ((HWND) wParam != hwnd))
                    {
                        if (RealizePalette (hdc))
              UpdateColors (hdc);
            return 0;
          }
                }
        break;

                case WM_DESTROY:
                {
          wglMakeCurrent (NULL, NULL);
                    wglDeleteContext (hrc);
          ReleaseDC (hwnd, hdc);
                    if (hPalette != NULL)
            DeleteObject (hPalette);
          KillTimer (hwnd, nTimer);
          if ( FULLSCREEN )
            ChangeResolution(WIDTH,HEIGHT,BITDEPTH, 1);
                    PostQuitMessage (0);
          return 0;
                }
                break;

                case WM_CHAR:
                {
          char chCharCode = (TCHAR) wParam;
          if(chCharCode == 0x1B)
          {
            SendMessage(hwnd, WM_DESTROY, 0,0);
                        return 0;
                        break;
          }

                        if(chCharCode == 'l')   // line draw on/off
                                dFlag ^= 1;
                        if(chCharCode == 'a')   // animation on/off
                                aFlag ^= 1;
                        if(chCharCode == 'r')   // reset
                        {
                                nSize   = 0.0f;
                                nCol    = 0.0f;
                                nTop    = 1.0f;
                                nBottom = 0.0f;
                                bFlag   = 1;
                                dFlag   = 1;
                                aFlag   = START_WITH_ANIMATION;
                                nAngleX = 0.0f;
                                nAngleY = 0.0f;
                                nAngleZ = 0.0f;
                nDistanceX = 0.0f;
                nDistanceY = 0.0f;
                nDistanceZ = -8.0f;
                        }
                        InvalidateRect (hwnd, NULL, FALSE);
                        return 0;
                }
                break;

                case WM_KEYDOWN:
                {
                        if(aFlag == 1)
                                break;

                        char chCharCode = (TCHAR) wParam;

            int ctrl_mode = 0;
            int shift_mode = 0;

            GetAsyncKeyState(VK_CONTROL);
            if(GetAsyncKeyState(VK_CONTROL))
              ctrl_mode = 1;

            GetAsyncKeyState(VK_SHIFT);
            if(GetAsyncKeyState(VK_SHIFT))
              ctrl_mode = 2;

            if(ctrl_mode == 0 && chCharCode == 0x26) // up
                        {
                                nAngleX += ROT_STEP;
                                if (nAngleX >= 360)
                                        nAngleX -= 360;
                        }

            if(ctrl_mode == 1 && chCharCode == 0x26) // up
                        {
              nDistanceZ += MOVE_STEP;
            }

            if(ctrl_mode == 2 && chCharCode == 0x26) // up
                        {
              nDistanceY += MOVE_STEP;
            }

                        if(ctrl_mode == 0 && chCharCode == 0x28) // down
                        {
              nAngleX -= ROT_STEP;
              if (nAngleX <= 0)
                nAngleX += 360;
                        }

            if(ctrl_mode == 1 && chCharCode == 0x28) // down
                        {
              nDistanceZ -= MOVE_STEP;
            }

            if(ctrl_mode == 2 && chCharCode == 0x28) // down
                        {
              nDistanceY -= MOVE_STEP;
            }

            if(ctrl_mode == 0 && chCharCode == 0x27) // right
                        {
              nAngleZ += ROT_STEP;
                            if (nAngleZ >= 360)
                nAngleZ -= 360;
                        }

            if(ctrl_mode == 1 && chCharCode == 0x27) // right
                        {
              nDistanceX += MOVE_STEP;
            }

            if(ctrl_mode == 0 && chCharCode == 0x25) // left
                        {
              nAngleZ -= ROT_STEP;
                            if (nAngleZ <= 0)
                nAngleZ += 360;
                        }

            if(ctrl_mode == 1 && chCharCode == 0x25) // right
                        {
              nDistanceX -= MOVE_STEP;
            }

            if(chCharCode == 0x6B) // +
                        {
                                nSize += 0.05f;
                        }

            if(chCharCode == 0x6D) // -
                        {
                                nSize -= 0.05f;
                                if(nSize < 0.0)
                                        nSize = 0.0;
                        }

                        InvalidateRect (hwnd, NULL, FALSE);
                        return 0;
                }
                break;

    }
    return DefWindowProc (hwnd, msg, wParam, lParam);
}

static void Resize()
{
  if(glnHeight == 0)
    glnHeight = 1;

    glViewport (0, 0, glnWidth, glnHeight);

    glMatrixMode (GL_PROJECTION);
    glLoadIdentity ();
  gldAspect = (GLdouble) glnWidth / (GLdouble) glnHeight;
    gluPerspective (30.0,           // Field-of-view angle
                    gldAspect,      // Aspect ratio of viewing volume
                    0.1,           // Distance to near clipping plane
                    100.0);                   // Distance to far clipping plane

}

static void DrawScene (HDC hdc,
            float nAngleX,
            float nAngleY,
            float nAngleZ,
            float nDistanceX,
            float nDistanceY,
            float nDistanceZ)
{
    //
    // Clear the color and depth buffers.
    //
    glClear (GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    //
    // Define the modelview transformation.
    //
  glMatrixMode (GL_MODELVIEW);
    glLoadIdentity ();
    glTranslatef (nDistanceX, nDistanceY, nDistanceZ);
    glRotatef ( -75.0f, 1.0f, 0.0f, 0.0f);
    glRotatef (nAngleX, 1.0f, 0.0f, 0.0f);
    glRotatef (nAngleY, 0.0f, 1.0f, 0.0f);
    glRotatef (nAngleZ, 0.0f, 0.0f, 1.0f);
    glScalef (nSize+0.05f,nSize+0.05f,nSize+0.05f);
    glRotatef (30.0f, 0.0f, 0.0f, 1.0f);


  if(glIsList(POLYGON_LIST))
    glCallList(POLYGON_LIST);

//
// // If you want to show the model's normals
// m.shownormals = true;
//
// // If the model is not going to be lit then set the lit
// // variable to false. It defaults to true.
// m.lit = false;
//
// // You can disable the rendering of the model
// m.visible = false;
// 
// // You can move and rotate the model like this:
// m.rot.x = 90.0f;
// m.rot.y = 30.0f;
// m.rot.z = 0.0f;
//
// m.pos.x = 10.0f;
// m.pos.y = 0.0f;
// m.pos.z = 0.0f;
//
// // If you want to move or rotate individual objects
// m.Objects[0].rot.x = 90.0f;
// m.Objects[0].rot.y = 30.0f;
// m.Objects[0].rot.z = 0.0f;
//
// m.Objects[0].pos.x = 10.0f;
// m.Objects[0].pos.y = 0.0f;
// m.Objects[0].pos.z = 0.0f;


  glFlush();

    SwapBuffers (hdc);
};

static void InitializeRC()
{
    // glClearColor(background[0], background[1], background[2], background[0] );
  glClearDepth(1.0f);                 // Depth Buffer Setup
  glEnable (GL_DEPTH_TEST);
  glDepthFunc(GL_LEQUAL);               // The Type Of Depth Testing To Do
  glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);  // Really Nice Perspective Calculations
  glEnable (GL_NORMALIZE);
  glEnable(GL_BLEND);
  glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glShadeModel(GL_SMOOTH);
//    glShadeModel(GL_FLAT);
//    glDisable(GL_CULL_FACE);
    glEnable(GL_CULL_FACE);

  glNewList(POLYGON_LIST, GL_COMPILE);
  m.Load("porshe.3ds"); 
  m.Draw();
  glEndList();
}

static void SetDCPixelFormat (HWND hwnd,HDC hdc)
{
    HANDLE hHeap;
    int nColors, i;
    LPLOGPALETTE lpPalette;
    BYTE byRedMask, byGreenMask, byBlueMask;

    static PIXELFORMATDESCRIPTOR pfd = {
        sizeof (PIXELFORMATDESCRIPTOR),             // Size of this structure
        1,                                          // Version number
        PFD_DRAW_TO_WINDOW |                        // Flags
        PFD_SUPPORT_OPENGL |
        PFD_GENERIC_ACCELERATED|
        PFD_DOUBLEBUFFER,
        PFD_TYPE_RGBA,                              // RGBA pixel values
        24,                     // 24-bit color
        0, 0, 0, 0, 0, 0,                           // Don't care about these
        0, 0,                                       // No alpha buffer
        0, 0, 0, 0, 0,                              // No accumulation buffer
        32,                     // 32-bit depth buffer
        0,                                          // No stencil buffer
        0,                                          // No auxiliary buffers
        PFD_MAIN_PLANE,                             // Layer type
        0,                                          // Reserved (must be 0)
        0, 0, 0                                     // No layer masks
    };

    int nPixelFormat;

    nPixelFormat = ChoosePixelFormat (hdc, &pfd);
    SetPixelFormat (hdc, nPixelFormat, &pfd);

    if (pfd.dwFlags & PFD_NEED_PALETTE)
  {
        nColors = 1 << pfd.cColorBits;
        hHeap = GetProcessHeap ();

        (LPLOGPALETTE) lpPalette = (LPLOGPALETTE)HeapAlloc (hHeap, 0,
            sizeof (LOGPALETTE) + (nColors * sizeof (PALETTEENTRY)));

        lpPalette->palVersion = 0x300;
        lpPalette->palNumEntries = nColors;

        byRedMask = (1 << pfd.cRedBits) - 1;
        byGreenMask = (1 << pfd.cGreenBits) - 1;
        byBlueMask = (1 << pfd.cBlueBits) - 1;

        for (i=0; i<nColors; i++) {
            lpPalette->palPalEntry[i].peRed =
                (((i >> pfd.cRedShift) & byRedMask) * 255) / byRedMask;
            lpPalette->palPalEntry[i].peGreen =
                (((i >> pfd.cGreenShift) & byGreenMask) * 255) / byGreenMask;
            lpPalette->palPalEntry[i].peBlue =
                (((i >> pfd.cBlueShift) & byBlueMask) * 255) / byBlueMask;
            lpPalette->palPalEntry[i].peFlags = 0;
        }

        hPalette = CreatePalette (lpPalette);
        HeapFree (hHeap, 0, lpPalette);

        if (hPalette != NULL)
    {
            SelectPalette (hdc, hPalette, FALSE);
            RealizePalette (hdc);
        }
    }
}

static DEVMODE currentdevMode;
static BOOL ChangeResolution(LONG w, LONG h, LONG bitdepth, int mode)
{
  DEVMODE devMode;
  LONG   modeExist;
  LONG   modeSwitch;
  LONG   i;
  char    buf[256];

  if(mode == 1)
  {
    modeSwitch = ChangeDisplaySettings(&currentdevMode, CDS_FULLSCREEN);
    if (modeSwitch==DISP_CHANGE_SUCCESSFUL)
      return TRUE;
  }

  modeExist = EnumDisplaySettings(NULL, ENUM_CURRENT_SETTINGS, &currentdevMode);
  if (modeExist)
  {
    sprintf(buf, "Current Settings %ldx%ldx%ld-bit color \n",
              currentdevMode.dmPelsWidth,
              currentdevMode.dmPelsHeight,
              currentdevMode.dmBitsPerPel
                        );
  }

  for (i=0; ;i++)
  {
    modeExist = EnumDisplaySettings(NULL, i, &devMode);

    if ( ((LONG)devMode.dmBitsPerPel == bitdepth)
       &&((LONG)devMode.dmPelsWidth == w)
       &&((LONG)devMode.dmPelsHeight == h) )
    {
      modeSwitch = ChangeDisplaySettings(&devMode, CDS_FULLSCREEN);
          if (modeSwitch==DISP_CHANGE_SUCCESSFUL)
                  return TRUE;
    }
  }
  return 0;
}






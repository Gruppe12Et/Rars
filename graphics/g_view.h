
/**
 * FILE: G_VIEW.H (portable)
 *
 * This file contains definitions of the classes :
 * - TPalette
 * - TLowGraphic
 *   |_ TView
 *      |_ TView2D
 *      |_ TView3D
 *         |_ TViewTrackEditor
 *      |_ TViewClassical
 *         |_ TTrackBitmap
 *      |_ TViewBoard
 *      |_ TViewTelemetry
 * - TViewManager
 *
 * History
 *  ver. 0.1  Aug 96 - Creation
 *  ver. 0.76 Oct 00 - CCDOC + telemetry
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76 
 */


#ifndef _g_view_H
#define _g_view_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "g_bitmap.h"
#include "g_define.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define  COLOR_BLACK       0
#define  COLOR_BLUE        1
#define  COLOR_GREEN       2
#define  COLOR_CYAN        3
#define  COLOR_RED         4
#define  COLOR_MAGENTA     5
#define  COLOR_MID_GRAY    6
#define  COLOR_LT_GRAY     7
#define  COLOR_DARK_GRAY   8
#define  COLOR_LT_BLUE     9
#define  COLOR_LT_GREEN   10
#define  COLOR_LT_RED     11
#define  COLOR_LT_MAGENTA 12
#define  COLOR_ORANGE     13
#define  COLOR_YELLOW     14
#define  COLOR_WHITE      15
 
#define  COLOR_MAX        256
#define  GRASS_COLOR      COLOR_GREEN
#define  ROAD_COLOR       COLOR_MID_GRAY
#define  START_COLOR      COLOR_WHITE
#define  TEXT_COLOR       COLOR_BLACK
#define  FLASH_COLOR      COLOR_WHITE

//--------------------------------------------------------------------------
//                           F O R W A R D
//--------------------------------------------------------------------------

class Model_3DS;

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

typedef struct
{
  int x;
  int y;
} Int2D;

typedef struct 
{
  float x;
  float y;
} Float2D;

typedef Int2D Quad2D[4];

typedef struct
{
  int r;                      // red
  int g;                      // green
  int b;                      // blue

  float fr;                   // idem mais pour OpenGL
  float fg;                   //
  float fb;                   //
} TColor;

typedef struct
{
  Model_3DS * m_pModel;

  float m_fCenterX;
  float m_fCenterY;
  float m_fCenterZ;
  float m_fPerimeter;

  float m_fLastX;
  float m_fLastY;
  float m_fAng;
} TWheel3D;

class TPalette
{
  public:
    // attributes
    int m_ColorNb;
    TColor  m_Color[COLOR_MAX];
    // operations
    void Init();
  private:
    void Load( const char * file_name );
    void Set();
};

class TLowGraphic 
{
  // attributes
    long m_ScreenSizeX;          // width of the screen (normally=size_x)
  public:
    long m_SizeX, m_SizeY;       // size of the bitmap
    int m_MaxX, m_MaxY;          // size_x-1, size_y-1
    char * m_Bitmap;
    bool m_bVirtual;             // A virtual graphic is not associated with a control on the screen
    int m_ColorNb;
    TBitmap m_BitmapFont;

  // constructor, destructor
    TLowGraphic( int x, int y, bool bVirtual=false );
    virtual ~TLowGraphic();
  // alloc-free bitmap
    virtual void MallocBitmap();
    void FreeBitmap();
  // operations
    virtual void Resize( int x, int y );
    void ClearBitmap( int c );
    void ClearBitmapTexture( double x, double y, double d, TBitmap * t );
    void DrawLine( Int2D p1, Int2D p2, int c );
    void DrawLine( int p1_x, int p1_y, int p2_x, int p2_y, int c);
    void DrawHorzLine( int x1, int x2, int y, int c );
    void DrawVertLine( int x, int y1, int y2, int c );
    void DrawPoint( int x, int y, int c );
    boolean ClipLine( int &x1, int &y1, int &x2, int &y2 );
    void DrawPoly( Int2D v[], int v_nb, int c );
    void DrawPolyTexture( Int2D v[], int v_nb, double sine, double cosine, TBitmap * t );
    void DrawString( const char * text, int x, int y, int foreground=oBLACK );
    void DrawStringOpaque( const char * text, int x, int y, int foreground=oBLACK, int background=oLIGHTGRAY );
    void FlipY();
    unsigned char * GetOneBitPerPixel( int color );
};

class TView: public TLowGraphic 
{
  // attributes
  protected:
    int m_iFollowCar;
    double m_ScaleX, m_ScaleY;
    double m_TopX, m_TopY;
    double m_CenterX, m_CenterY, m_CenterZ;
    double m_SpeedX, m_SpeedY;

    // Draws a border to a segment
    void DrawBorder(Int2D v[]);

  public:
    int m_OptionShowBoard;

  public:
  // constructor, destructor
    TView( int x, int y, bool bVirtual=false );
    virtual ~TView();
  // operations
    virtual void Refresh();
    virtual void FollowCar( int car_nr );
    virtual void FullScreenScale();
};

class TViewClassical: public TView
{
  // attributes
  protected:
    Quad2D m_StartPoly;
    TBitmap m_BitmapGrass;
  public:
    int m_OptionFlash;

  // operations
  public:
    TViewClassical( int x, int y, bool bVirtual=false );
    virtual ~TViewClassical();
    virtual void Resize( int x, int y );
    virtual void Refresh();
    void DrawCars( int state, int car_nr );
    void DrawRoad();
    void DrawBoard();
    void DrawStart();
};

/**
 * In 3D, the bitmap of the track in the upper left on the screen
 * It uses TViewClassical to construct the bitmap
 */
class TTrackBitmap : TViewClassical
{
  // attributes
    unsigned char * bitmap;

  // operations
  public:
    TTrackBitmap( int x, int y );
    ~TTrackBitmap();
    void Draw();
};


class TViewBoard: public TView
{
  // attributes
  int iMaxCarsShown;            // how many entries can be shown on the board
  int m_iShowCarsBehind;      // how many cars are shown behind the follow car
  bool m_bShowMoreInfos;      // more infos about cars will be shown if true

  // operations
  public:
    TViewBoard( int x, int y );
    virtual ~TViewBoard();
    virtual void Resize( int x, int y );
    virtual void Refresh();
    void ScreenInit();
};

class TViewTelemetry: public TView
{
  // attributes
  const char * m_sTitle;         // title
  double       m_fMinX;          // Minumum X
  double       m_fMaxX;          // Maximum X 
  double       m_fMinY;          // Minumum Y
  double       m_fMaxY;          // Maximum Y 
  int          m_iModeScaleX;    // automatic | manual
  int          m_iModeScaleY;    // automatic | manual
  double       m_fTickStepX;     // distance between 2 ticks 
  double       m_fTickStepY;     // distance between 2 ticks 
  boolean      m_bShowTicksX;    // show tick in X   
  boolean      m_bShowTicksY;    // show tick in Y
  double       m_fChartPosX;     // Chart pos X
  double       m_fChartPosY;     // Chart pos Y
  double       m_fChartSizeX;    // Chart size X
  double       m_fChartSizeY;    // Chart size Y
  int          m_iNbPoint;       // number of points in the graph
  int          m_iChartDataNb;   // number of charts
  // operations
  public:
    TViewTelemetry( int x, int y, int car_nr );
    virtual ~TViewTelemetry();
    virtual void Resize( int x, int y );
    virtual void Refresh();
    void ScreenInit();
    void MonitorInit();
    void Monitor( int _car );
    void setScaleX( double min, double max );
    void setTickX( double step );
    void setTickY( double step );
    void setTitle( const char * sTitle );
    void autoTickY();
    double searchMin( double d[], int nb );
    double searchMax( double d[], int nb );
    int data2ChartX( double x );
    int data2ChartY( double y );
    virtual void FollowCar( int car_nr );
};

class TView2D: public TView
{
  // attributes
    int m_FollowCarTime;
    int m_dFollowCar;
    TBitmap m_BitmapGrass;
    double m_Zoom;

  // operations

  public:
    TView2D( int x, int y, int d_follow_car );
    virtual ~TView2D();
    virtual void Refresh();
    virtual void FollowCar( int car_nr );
    void DrawCars();
    void DrawRoad();
    void DrawBoard();
    void DrawTrajectory( int car );
};

class TView3D: public TView
{
  protected:
    // attributes
    int    m_FollowCarTime;
    int    m_dFollowCar;
    double m_Zoom;
    double m_Ang;
    double m_CamX, m_CamY;
    double m_fAngleX, m_fAngleZ;
    double m_fSpeedAngleX, m_fSpeedAngleZ;
    bool   m_b3DS;
    bool   m_bInit;
    int    m_iTypeView;
    float  m_aFrustum[6][4];
    Model_3DS * m_aModelBody[MAX_CARS];
    TWheel3D m_aFrontWheel[MAX_CARS];
    TWheel3D m_aBackWheel[MAX_CARS];

    Model_3DS * ModelWheelRear;
    Model_3DS * ModelWheelFront;
    Model_3DS * ModelCarBody;
    Model_3DS * ModelHelmet;
    Model_3DS * ModelInstantReplay;
    TTrackBitmap * m_TrackBitmap;

    // operations
    void ResetRotation();
    void MoveWithKeys();

  public:
    TView3D( int x, int y, int offx, int offy, int d_follow_car );
    TView3D();
    virtual ~TView3D();
    void Resize( int x, int y, int offx, int offy );
    virtual void Refresh();
    virtual void FollowCar( int car_nr );
    void DrawCars();
    void DrawBoard();
    void DrawTrajectory( int car );
    void GL_EnableLight();
    void GL_DrawCar( int i );
    void GL_DrawCar3DS( int i );
    void GL_DrawWheel3DS( TWheel3D * wheel, int i, float x, float y );
    void GL_DrawCarName( int i );
    void Load3DSWheel( TWheel3D * wheel, const char * filename );
    void GL_CreateList();
    void ExtractFrustum();
    float SphereInFrustum( float x, float y, float z, float radius );

    // Kde
    void Init( int x, int y, int offx, int offy, bool bFirstTime );
    void Destroy();
    virtual void DrawAll();
    virtual double MaxDist();

    friend class RarsIni;
};

class TViewTrackEditor: public TView3D
{
  public:
    TViewTrackEditor( int x, int y, int offx, int offy );
    virtual void DrawAll();
    virtual double MaxDist();
    virtual void FullScreenScale();
};

class TViewManager
{
  // attributes
    int m_iFollowCar;                 // old designated
    int m_ViewNb;
    TView ** m_Views;
    int m_FastModeParam;
    int m_iViewMode;
    int m_iLastViewMode;
    int m_iSizeOffX;
    int m_iSizeOffY;
  public:
    int m_iFollowMode;                // Follow view mode
    double m_ZoomDelta;               //
    long m_ImageCpt;                  // image counter
    int m_Initialized;                // initialized
    TBitmap m_aBitmapCar[MAX_CARS];   // Bitmap with the bitmap of cars
    InstantReplay* m_oInstantReplay;  // Pointer to the InstantReplay of RaceManager
    int m_iFollowNobodyKey;           // Key pressed in FollowNobody ARROW_XX
    bool m_bChangeView;               // Change view (OpenGL)
    int m_iRealSpeed;                 // Speed
    int m_i3DS;                       // if !=0, use 3DS objects for car and track
    bool m_bShowNames;                // If true then show drivers names
    float m_fFps;                     // frames per second

  // operations
  public:
    TViewManager( InstantReplay * ir );
    ~TViewManager();
    // start
    void Reset(); 
    // view
    void InitAllViews();
    void DeleteAllViews();
    void AddView( TView * v );
    void ResizeAllViews( int x, int y );
    // screen
    void InitScreen();
    void CloseScreen();
    // view <-> screen
    void Refresh();
    void CopyViewToScreen( int view_nr );
    // option
    int GetFollowCar();
    void FollowCar( int car_nr );
    double distance(int i,int j);
    int MostInterestingCar();
    void IncZoom();
    void DecZoom();
    void SetFastModeParam( int param );
    int GetViewMode();
    TView3D * GetOpenGlView();

    friend class KdeRars;
    friend class CDlgRarsCommon;
};

#endif // _g_view_H


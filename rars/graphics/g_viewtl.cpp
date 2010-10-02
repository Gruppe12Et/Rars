
/**
 * FILE: G_VIEWTL.CPP (portable)
 *
 * TViewTelemetry
 * - This is the view with a telemetry board
 * 
 * History
 *  ver. 0.76 Oct 00 - Creation
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76 
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <string.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

const double NO_VALUE = -0.0000012345789123456789;
const double DIVIDER = 10.0;
const int X_BORDER_LEFT  = 80;
const int Y_BORDER_UP    = 20;
const int X_BORDER_RIGHT = 10;
const int Y_BORDER_DOWN  = 60;

const int MANUAL    = 0;
const int AUTOMATIC = 1;

//--------------------------------------------------------------------------
//                            Class ChartData
//--------------------------------------------------------------------------

class ChartData
{
public:
  double *     data;
  int          nb_data;
  int          color;
  const char * name;
  
  int          car;
  int          pos;

  ChartData( int _car, int _nb )
  {
    car = _car;
    color = race_data.cars[car]->tail_color;
    if( color == oWHITE )
    {
      color = race_data.cars[car]->nose_color;
      if( color == oWHITE )
        color = oBLACK;
    }
    name = drivers[car]->getName();
    nb_data = _nb;
    data = new double[nb_data];
    for( int i=0; i<nb_data; i++ )
    {
      data[i]=NO_VALUE;
    }
  }

  ~ChartData()
  {
    delete [] data;
  }

  void update()
  {
    pos = (int) (race_data.cars[car]->distance / DIVIDER);
    if( pos <0 ) pos = 0;
    if( pos>=nb_data) pos= nb_data-1;
    data[pos] = function();
  }

  double function()
  {
//	double val = (race_data.cars[car]->vc -
//		sqrt(race_data.cars[car]->xdot*race_data.cars[car]->xdot +
//		     race_data.cars[car]->ydot*race_data.cars[car]->ydot)) * MPH_FPS;
    double val =
		(sqrt(race_data.cars[car]->xdot*race_data.cars[car]->xdot +
			  race_data.cars[car]->ydot*race_data.cars[car]->ydot)) * MPH_FPS;
    return max( -200.0, min( 200.0, val ));
  }

  void FollowCar( int car_nr )
  {
    if( car == car_nr )
      return;
    
    delete data;

    car = car_nr;
    color = race_data.cars[car]->tail_color;
    if( color == oWHITE )
    {
      color = race_data.cars[car]->nose_color;
      if( color == oWHITE )
        color = oBLACK;
    }
    name = drivers[car]->getName();
    data = new double[nb_data];
    for( int i=0; i<nb_data; i++ )
    {
      data[i]=NO_VALUE;
    }
  }
};

//--------------------------------------------------------------------------
//                            Class TViewTelemetry
//--------------------------------------------------------------------------

ChartData * chartData[10];

/**
 * Init the variable to monitor
 */
void TViewTelemetry::MonitorInit()
{
  if( m_iChartDataNb==0 ) 
  {
    setTitle( "SPEED" );
    // Monitor( 0, &(race_data.cars[0]->to_rgt) );
    Monitor( race_data.m_aCarInPos[0] );
    Monitor( m_iFollowCar );
  }
}

/**
 * Add a varaible in the list of monitored variables
 *
 * @param _car          id of the car
 * @param val           value of the car to monitor
 */
void TViewTelemetry::Monitor( int _car )
{
  chartData[m_iChartDataNb] = new ChartData( _car, m_iNbPoint );
  m_iChartDataNb ++;
}

/**
 * Constructor
 *
 * @param x             size of the view
 * @param y             size of the view
 */
TViewTelemetry::TViewTelemetry( int x, int y, int car_nr ) : TView( x, y )
{
  ScreenInit();
  m_iFollowCar = car_nr;
}

/**
 * Destructor
 */
TViewTelemetry::~TViewTelemetry()
{
  for( int i=0; i<m_iChartDataNb; i++ )
  {
    delete chartData[i];
  }
}

/**
 * Init the screen
 */
void TViewTelemetry::ScreenInit()
{
  char s[64], track_name[16], * psz;
  int i;

  ClearBitmap( COLOR_WHITE );

  psz = strchr(currentTrack->m_sFileName,'/');
  if( psz==NULL )
  {
    psz=currentTrack->m_sFileName;
  }
  else
  {
    psz++;
  }
  for(i=0; i<8; i++)
  {
    if(psz[i] == '.' || psz[i] == 0)
    {
      break;
    }
    else
    {
      track_name[i] = psz[i];
    }
  }
  track_name[i] = 0;
  sprintf( s, "Track: %s", track_name );
  DrawString( s, 1, 1 );
  sprintf( s, "Length: %.2f feets.", currentTrack->get_track_description().length );
  DrawString( s, 129, 1 );

  m_bShowTicksX  = false;
  m_bShowTicksY  = false;
  m_iModeScaleX  = AUTOMATIC;
  m_iModeScaleY  = AUTOMATIC;
  m_sTitle       = NULL;
  m_iChartDataNb = 0;

  m_iNbPoint = (int)( currentTrack->get_track_description().length/DIVIDER );
  setScaleX( 0, m_iNbPoint );
  m_iModeScaleY  = MANUAL;
  setTickX( 100 );
  m_fMinY = 0;
  m_fMaxY = 1;
}

/**
 * Resize the bitmap and the related parameters
 * 
 * @param x             the new size of the graphic 
 * @param y             the new size of the graphic (view)
 */
void TViewTelemetry::Resize( int x, int y )
{
  TLowGraphic::Resize( x, y );
  ScreenInit();
}

/**
 * Change followed cars
 * 
 * @param car_nr        the new followed car number
 */
void TViewTelemetry::FollowCar( int car_nr )
{
  if( m_iChartDataNb > 0 )
  {
    chartData[0]->FollowCar( car_nr );
    int next = race_data.cars[car_nr]->s.position != 0 
      ? race_data.m_aCarInPos[race_data.cars[car_nr]->s.position-1] 
      : race_data.m_aCarInPos[1]; 
    chartData[1]->FollowCar( next );
    m_iFollowCar = car_nr;
  }
}

/**
 * Refresh the view
 */
void TViewTelemetry::Refresh()
{
  char s[128];

  MonitorInit();

  m_fChartSizeX = m_SizeX - X_BORDER_LEFT - X_BORDER_RIGHT;
  m_fChartSizeY = m_SizeY - Y_BORDER_UP - Y_BORDER_DOWN;
  m_fChartPosX  = X_BORDER_LEFT;
  m_fChartPosY  = Y_BORDER_UP;

  int ix_chart_size = (int)m_fChartSizeX;
  int iy_chart_size = (int)m_fChartSizeY;
  int ix_chart_pos  = (int)m_fChartPosX;
  int iy_chart_pos  = (int)m_fChartPosY;

  // border
  ClearBitmap( COLOR_WHITE );
  // rectangle
  // g.setColor( Color.white );
  // g.fillRect( ix_chart_pos, iy_chart_pos, ix_chart_size, iy_chart_size );
  // cader
  
  // left
  DrawLine( ix_chart_pos, iy_chart_pos, ix_chart_pos, iy_chart_pos+iy_chart_size+5, COLOR_DARK_GRAY );
  // right
  DrawLine( ix_chart_pos+ix_chart_size, iy_chart_pos, ix_chart_pos+ix_chart_size, iy_chart_pos+iy_chart_size+5, COLOR_DARK_GRAY );
  // up
  DrawLine( ix_chart_pos-5, iy_chart_pos, ix_chart_pos+ix_chart_size, iy_chart_pos, COLOR_DARK_GRAY );
  // bottom
  DrawLine( ix_chart_pos-5, iy_chart_pos+iy_chart_size, ix_chart_pos+ix_chart_size, iy_chart_pos+iy_chart_size, COLOR_DARK_GRAY );

  int x_old, y_old, x_new, y_new;
  int i, v, nb;


  if( m_sTitle!=NULL )
  {
    DrawString( m_sTitle, 10, 5 );
  }

  for( v=0; v<m_iChartDataNb; v++ )
  {
    ChartData * cd = chartData[v];
    cd->update();
    double * dy = cd->data;

    // Scale Y
    double val = cd->function();
    if( val > m_fMaxY ) 
    {
      m_fMaxY = val; 
    }
    if( val < m_fMinY ) 
    {
      m_fMinY = val; 
    }
    autoTickY();

    // Original value
    x_old = data2ChartX( 0 );
    y_old = data2ChartY( dy[0] );

    // cursor
    x_new = data2ChartX( cd->pos );
    DrawLine( x_new, iy_chart_pos, x_new, iy_chart_pos+iy_chart_size, cd->color );


    for( i=1; i<m_iNbPoint; i++ )
    {
      x_new = data2ChartX( i );
      y_new = data2ChartY( dy[i] );
      if( dy[i-1]!=NO_VALUE && dy[i]!=NO_VALUE )
      {
        DrawLine( x_old, y_old, x_new, y_new, cd->color );
      }
      x_old = x_new;
      y_old = y_new;
    }
    // Name
    int ipos = m_SizeX/2 + (v-(m_iChartDataNb/2))*100;
    DrawString( cd->name, ipos+12, m_SizeY-20, cd->color );
  }
  

  if( m_bShowTicksX )
  {
    nb = (int)( (m_fMaxX-m_fMinX)/m_fTickStepX ) + 1;
    double x = (int)(m_fMinX/m_fTickStepX)*m_fTickStepX;
    for( i=0; i<nb; i++ )
    {
      x_new = data2ChartX( x );
      DrawLine( x_new, iy_chart_pos, x_new, iy_chart_pos+iy_chart_size+5, COLOR_LT_GRAY );
      sprintf( s, "%d", (int)x);
      DrawString( s, x_new-5, m_SizeY-40 );
      x += m_fTickStepX;
    }
  }

  if( m_bShowTicksY )
  {
    nb = (int)( (m_fMaxY-m_fMinY)/m_fTickStepY ) + 1;
    double y = (int)(m_fMinY/m_fTickStepY)*m_fTickStepY;
    for( i=0; i<nb; i++ )
    {
      y_new = data2ChartY( y );
      DrawLine( ix_chart_pos-5, y_new, ix_chart_pos+ix_chart_size, y_new, COLOR_LT_GRAY);
      sprintf( s, "%5.2f", y);
      DrawString( s, 20, y_new );
      y += m_fTickStepY;
    }
  }
}

/**
 * Set the scale X
 *
 * @param min           Minimun value in X
 * @param max           Maximum value in X
 */
void TViewTelemetry::setScaleX( double min, double max ) 
{
  m_fMinX = min;
  m_fMaxX = max;
  m_iModeScaleX = MANUAL;
}

/**
 * Set the step for the X ticks
 *
 * @param step          distance between 2 ticks
 */
void TViewTelemetry::setTickX( double step ) 
{
  m_fTickStepX = step;
  m_bShowTicksX = true;
}

/**
 * Set the step for the Y ticks
 *
 * @param step          distance between 2 ticks
 */
void TViewTelemetry::setTickY( double step ) 
{
  m_fTickStepY = step;
  m_bShowTicksY = true;
}


/**
 * Set the title
 *
 * @param sTitle        the title
 */
void TViewTelemetry::setTitle( const char * sTitle )
{
  m_sTitle = sTitle;
}


/**
 * Calculate the Tick Y from maxY minY
 */
void TViewTelemetry::autoTickY()
{
  double a = m_fMaxY-m_fMinY;
  double b = floor( log(a)/log(10.0) );
  double c = pow( 10, b );
  double d = a / c;
  if( d<3 ) 
  {
    c /= 10;
  } 
  else if ( d<6 )
  {
    c /= 5;
  }
  setTickY( c*2 );
}

/**
 * Search the minimum in a group of value
 *
 * @param d             a vector of value
 * @return              the minimum value
 */
double TViewTelemetry::searchMin( double d[], int nb )
{
  double r = d[0];
  for( int i=1; i<nb; i++ ) 
  {
    if( r>d[i] )
      r=d[i];
  }
  return r;
}

/**
 * Search the maximum in a group of value
 *
 * @param d             a vector of value
 * @return              the maximum value
 */
double TViewTelemetry::searchMax( double d[], int nb )
{
  double r = d[0];
  for( int i=1; i<nb; i++ )
  {
    if( r<d[i] )
      r=d[i];
  }
  return r;
}

/**
 * Convert a data in chart coordinates
 */
int TViewTelemetry::data2ChartX( double x )
{
  return (int)( m_fChartPosX+(x-m_fMinX)*m_fChartSizeX/(m_fMaxX-m_fMinX) );
}

/**
 * Convert a data in chart coordinates
 */
int TViewTelemetry::data2ChartY( double y )
{
  return (int)( m_fChartPosY+m_fChartSizeY-(y-m_fMinY)*m_fChartSizeY/(m_fMaxY-m_fMinY) );
}

//--------------------------------------------------------------------------
//
//    FILE: G_TRACK_AREA.CPP (portable)
//
//      - Common classes and code for a Track Area
//        An area is a group of point for the relief outside the track
//
//    Version       Author          Date
//      0.1      Marc Gueury     20 /03 /02
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "g_tracked.h"
#include "../misc.h"
#include "g_global.h"

//--------------------------------------------------------------------------
//                        Class GTrackListArea
//--------------------------------------------------------------------------


/**
 * Constructor
 */
GTrackListArea::GTrackListArea()
{
  m_iNumArea = 0;
}

/**
 * Constructor 
 */
GTrackListArea::GTrackListArea( GTrackListArea &lft, GTrackListArea &rgt )
{
  m_iNumArea = 0;

  // Copy the data of the left and right side
  Add( new GTrackArea() );
  Add( new GTrackArea() );

  m_aArea[0]->Copy( *lft.m_aArea[0], 0, lft.m_aArea[0]->m_iNumVertex-1 );
  m_aArea[1]->Copy( *rgt.m_aArea[0], 0, rgt.m_aArea[0]->m_iNumVertex-1 );
}

/**
 * Destructor
 */
GTrackListArea::~GTrackListArea()
{  
  Reset();
}

/**
 * Reset
 */
void GTrackListArea::Reset()
{  
  for( int i=0; i<m_iNumArea; i++ )
  {
    delete m_aArea[i];
    m_aArea[i] = NULL;
  }
  m_iNumArea = 0;
}

/**
 * Add a new area
 */
void GTrackListArea::Add( GTrackArea * a )
{
  m_aArea[m_iNumArea] = a;  
  m_iNumArea++;

  if( m_iNumArea>AREA_MAX )
  {
    exitOnError( "GTrackListArea::Add: AREA_MAX(%d) too small", AREA_MAX ); 
  }
}

/**
 * Remove an area
 */
void GTrackListArea::Remove( GTrackArea * a )
{
  int bFound = false;
  for( int i=0; i<m_iNumArea; i++ )
  {
    if( bFound )
    {
      m_aArea[i-1] = m_aArea[i];  
    }
    else
    {
      if( a==m_aArea[i] )
      {
        delete a;
        bFound = true;
      }
    }
  }
  m_iNumArea --;
}


/**
 * Split the Areas
 */
void GTrackListArea::SplitArea( GTrackListArea &lft, GTrackListArea &rgt )
{
  int iArea, iArea2;
  Vertex inter, inter2;

  ////
  //// First, remove all the duplicate points
  ////
  for( iArea=0; iArea<m_iNumArea; iArea++ )
  {
    m_aArea[iArea]->RemoveDuplicate();
  }

  ////
  //// Split the areas 
  ////
  bool bFound = true;
  while( bFound )
  {
    bFound = false;
    for( iArea=0; iArea<m_iNumArea; iArea++ )
    {
      int i, i2, j, j2;

      if( m_aArea[iArea]->FindFirstIntersection( *m_aArea[iArea], inter, i, i2, j, j2 ) )
      {
        GTrackArea * area1 = new GTrackArea();
        GTrackArea * area2 = new GTrackArea();

        area1->Copy( *m_aArea[iArea], i2, j );
        area2->Copy( *m_aArea[iArea], j2, i );

        area1->Add( inter );
        area2->Add( inter );

        area1->RemoveDuplicate();
        area2->RemoveDuplicate();

        Add( area1 );
        Add( area2 );

        Remove( m_aArea[iArea] );
        bFound = true;
        break;
      }
    }
  }

  ////
  //// Remove the areas where the outside point is in the road
  ////
  for( iArea=m_iNumArea-1; iArea>=0; iArea-- )
  {
    if( m_aArea[iArea]->IsOutsideVertexInsideRoad(lft,rgt) )
    {
      Remove( m_aArea[iArea] );
    }
  }

  ////
  //// Find the intersection between the left and rigt areas
  ////
  for( iArea=0; iArea<m_iNumArea; iArea++ )
  {
    if( m_aArea[iArea]->m_iType==AREA_TYPE_RIGHT )
    {
      for( iArea2=0; iArea2<m_iNumArea; iArea2++ )
      { 
        if( m_aArea[iArea2]->m_iType==AREA_TYPE_LEFT )
        {
          int ai, ai2, aj, aj2;
          if( m_aArea[iArea]->FindFirstIntersection( *m_aArea[iArea2], inter, ai, ai2, aj, aj2 ) )
          {
            int bi = ai, bi2=ai2, bj=aj+1, bj2=aj2;
            if( m_aArea[iArea]->FindNextIntersection( *m_aArea[iArea2], inter2, bi, bi2, bj, bj2 ) )
            {
              if( TGeom::LeftSide(m_aArea[iArea]->m_aVertex[ai], m_aArea[iArea]->m_aVertex[ai2], m_aArea[iArea2]->m_aVertex[aj])<0 )
              {
                // bad order
                int t;
                t = ai;  ai  = bi;  bi  = t;
                t = ai2; ai2 = bi2; bi2 = t;
                t = aj;  aj  = bj;  bj  = t;
                t = aj2; aj2 = bj2; bj2 = t;
                
                Vertex vt;
                vt = inter; inter = inter2; inter2 = vt;
              }
              ////
              //// Merge the 2 areas (The area merged are always external one)
              //// So, the 
              ////
              GTrackArea * area = new GTrackArea();
              area->Copy( *m_aArea[iArea], ai2, bi );
              area->Add( inter2 );
              m_aArea[iArea2]->FlipOrder();
              aj2 = m_aArea[iArea2]->m_iNumVertex-1-aj2;
              bj  = m_aArea[iArea2]->m_iNumVertex-1-bj;
              area->Copy( *m_aArea[iArea2], bj, aj2, false );
              area->Add( inter );

              Remove( m_aArea[iArea] );
              Remove( m_aArea[iArea2] );
              Add( area );
            }
          }
        }
      }
    }
  }
}

/**
 * Copy all areas to a 3DS model
 */
void GTrackListArea::CopyAll( Model_3DS & model )
{
  Vertex v;
  Vect2D t;

  model.RemoveAll();
  model.Create();
  for( int iArea=0; iArea<m_iNumArea; iArea++ )
  {
    model.AddObject();

    GTrackArea * area = m_aArea[iArea];
    for( int i=0; i<area->m_iNumVertex; i++ )
    {
      v = area->m_aVertex[i];
      t.SetXY( v.x/200.0, v.y/200.0 );

      model.aObject[iArea].AddVertex( v, t, false ); 
    }
  }
}

//--------------------------------------------------------------------------
//                           Class GTrackArea
//--------------------------------------------------------------------------

/**
 * Constructor
 */
GTrackArea::GTrackArea( int type )
{
  m_iNumVertex = 0;
  m_iType = type;
}

/**
 * Add a new point
 */
void GTrackArea::Add( Vertex &v )
{
  m_aVertex[m_iNumVertex] = v;
  m_iNumVertex++;

  if( m_iNumVertex>=AREA_VERTEX_MAX )
  {
    exitOnError( "GTrackArea::Add : AREA_VERTEX_MAX (%d) too small", AREA_VERTEX_MAX );
  }
}

/**
 * Flip the order
 */
void GTrackArea::FlipOrder()
{
  Vertex v;

  for( int i=0; i<m_iNumVertex/2; i++ )
  {
    v = m_aVertex[i];
    m_aVertex[i] = m_aVertex[m_iNumVertex-1-i];
    m_aVertex[m_iNumVertex-1-i] = v;
  }

  if( m_iType==AREA_TYPE_LEFT )
  {
    m_iType = AREA_TYPE_RIGHT;
  }
  else if( m_iType==AREA_TYPE_RIGHT )
  {
    m_iType = AREA_TYPE_LEFT;
  }
  else
  {
    exitOnError( "GTrackArea::FlipOrder: unknown m_iType (%d)", m_iType );
  }
}

/**
 * Rotate the points such that iFirst become the
 * first vertex.
 */
void GTrackArea::RotateOrder( int iFirst )
{
  int i;
  Vertex aTemp[AREA_VERTEX_MAX];

  for( i=0; i<m_iNumVertex; i++ )
  {
    aTemp[i] = m_aVertex[i];
  }

  for( i=0; i<m_iNumVertex; i++ )
  {
    int i2 = (i+iFirst)%m_iNumVertex;
    m_aVertex[i] = aTemp[i2];
  }
}

/**
 * Find the first intersection between 2 areas, or between the area and itself
 *
 * @param bSelf : intersction between the area and itelf -> true
 * @return true is an intersection is found
 */
bool GTrackArea::FindFirstIntersection( GTrackArea &other,Vertex &inter, int &i, int &i2, int &j, int &j2 )
{
  i = 0;
  j = 0;
  return FindNextIntersection( other, inter, i, i2, j, j2 );
}

/**
 * Find the next intersection between 2 areas, or between the area and itself
 *
 * @param bSelf : intersction between the area and itelf -> true
 * @return true is an intersection is found
 */
bool GTrackArea::FindNextIntersection( GTrackArea &other,Vertex &inter, int &i, int &i2, int &j, int &j2 )
{
  int istart = i;
  int jstart = j;
  bool bSelf = (this==&other);

  for( i=istart; i<m_iNumVertex; i++ )
  {
    i2 = (i+1)%m_iNumVertex;
    for( j=jstart; j<other.m_iNumVertex; j++ )
    {
      j2 = (j+1)%other.m_iNumVertex;

      if( !bSelf || (i!=j && i!=j2 && i2!=j && i2!=j2) )
      {
        if( TGeom::IntersectionSegmentSegment( m_aVertex[i], m_aVertex[i2], other.m_aVertex[j], other.m_aVertex[j2], inter ) )
        {
          return true;
        }
      }
    }
    jstart = 0;
  }
  return false;
}


/**
 * Clean the area
 */
void GTrackArea::RemoveDuplicate()
{
  int j=1;

  for( int i=1; i<m_iNumVertex; i++ )
  {
    Vertex last = m_aVertex[j-1];
    if( TGeom::Dist(m_aVertex[i], last)<0.01 )
    {
      // Duplicate point
    }
    else
    {
      if( i!=j )
      {
        m_aVertex[j] = m_aVertex[i];
      }
      j ++; 
    }
  }
}

/**
 * Copy a part of the point of src in the area
 *
 * @param beg first point
 * @param end last point
 */
void GTrackArea::Copy( GTrackArea &src, int beg, int end, bool bReset )
{
  if( bReset )
  {
    m_iType = src.m_iType;
    m_iNumVertex = 0;
  }

  int nb;
  if( end>beg )
  {
    nb = end-beg+1;
  }
  else
  {
    nb = end-beg+src.m_iNumVertex+1;
  }
  for( int pos1=0; pos1<nb; pos1++ )
  {
    int pos2 = (pos1+beg)%src.m_iNumVertex;
    m_aVertex[pos1+m_iNumVertex] = src.m_aVertex[pos2];
  }
  m_iNumVertex += nb;
}

/**
 * Calculate the sum of all the angle between the sub-segments
 * 
 * @return the sum of the angles
 */
double GTrackArea::SumAngle()
{
  if( m_iNumVertex<3 )
  {
    exitOnError( "GTrackArea::SumAngle: not enough points in area (min 3)" );
  }

  double sum_ang = 0;
  double last_x = m_aVertex[m_iNumVertex-2].x;
  double last_y = m_aVertex[m_iNumVertex-2].y;
  double last_ang;
  double x = m_aVertex[m_iNumVertex-1].x;
  double y = m_aVertex[m_iNumVertex-1].y;
  double ang = atan2( y-last_y, x-last_x );

  for( int i=0; i<m_iNumVertex; i++ )
  {
    last_ang = ang;
    last_x = x;
    last_y = y;

    x = m_aVertex[i].x;
    y = m_aVertex[i].y;
    ang = atan2( y-last_y, x-last_x );
    double dang = TGeom::AngleMinPItoPI( last_ang-ang );
    sum_ang += dang;
  }
  return sum_ang;
}

/**
 * Calculate the number of intersection between the area and a segment (a,b)
 * 
 * @return the number of intersection
 */
int GTrackArea::NumIntersection( Vertex &a, Vertex &b )
{
  Vertex inter;
  int num = 0;

  for( int i=0; i<m_iNumVertex; i++ )
  {
    int i2 = (i+1)%m_iNumVertex;
    if( TGeom::IntersectionSegmentSegment( a, b, m_aVertex[i], m_aVertex[i2], inter ) )
    {
      num ++;
    }
  }
  return num;
}

/**
 * Get the vertex outside (knowing the side of the road)
 *
 *          Left                              Right
 *            |         |             |        |
 *           x|         |             |        |x
 *            |         |             |        |
 *           
 * @return a vertex
 */
Vertex GTrackArea::GetVertexOutside(int i)
{
  Vertex v;

  Vertex &v0 = m_aVertex[i];
  Vertex &v1 = m_aVertex[(i+1)%m_iNumVertex];

  double dist = vec_mag( v1.x-v0.x, v1.y-v0.y );

  if( m_iType==AREA_TYPE_LEFT )
  {
    v.SetXYZ( v0.x - (v1.y-v0.y)/dist*1.0, v0.y + (v1.x-v0.x)/dist*1.0, 0 );
  }
  else if( m_iType==AREA_TYPE_RIGHT )
  {
    v.SetXYZ( v0.x + (v1.y-v0.y)/dist*1.0, v0.y - (v1.x-v0.x)/dist*1.0, 0 );
  }
  else
  {
    exitOnError( "GTrackArea::GetVertexOutside: unknown m_iType (%d)", m_iType );
  }
  return v;
}

/**
 * Detect if a vertex is inside the area
 *
 * @return true if the vertex is inside the area
 */
bool GTrackArea::IsInArea( Vertex &a )
{
  static Vertex b( 100000, 10000, 0); // a point far away
  int inter = NumIntersection( a, b );
  return (inter%2)==1;
}

/**
 * Detect if the area is inside the road.
 *
 * @return true if the area is inside the road
 */
bool GTrackArea::IsOutsideVertexInsideRoad( GTrackListArea &lft, GTrackListArea &rgt )
{
  Vertex a;
  // Find a point that should be outside of the road 
  // knowing the side of the Area
  int nb_test = max(20, m_iNumVertex);
  int nb_bad = 0;
  
  for( int i=0; i<nb_test; i++ )
  {
    a = GetVertexOutside(i);

    // Create a dummy area with 4 points of the sub-segments
    GTrackArea subseg;
    subseg.m_iNumVertex = 4;

    for( int iArea=0; iArea<lft.m_iNumArea; iArea++ )
    {
      GTrackArea &l = *lft.m_aArea[iArea];
      GTrackArea &r = *rgt.m_aArea[iArea];

      for( int j=1; j<l.m_iNumVertex; j++ )
      {
        subseg.m_aVertex[0] = l.m_aVertex[j-1];
        subseg.m_aVertex[1] = r.m_aVertex[j-1];
        subseg.m_aVertex[2] = r.m_aVertex[j];
        subseg.m_aVertex[3] = l.m_aVertex[j];

        if( subseg.IsInArea(a) )
        {
          nb_bad++;
          break;
        }
      }
    }
  }
  if( nb_bad*2<nb_test )
  {
    return false;
  }
  else
  {
    return true;
  }
}

/**
 * Find the vertex id of with the MaxX
 * 
 * @return the id of the vertex
 */
int GTrackArea::FindMaxX()
{
  int id = 0;
  double maxx = m_aVertex[0].x;

  for( int i=1; i<m_iNumVertex; i++ )
  {
    if( m_aVertex[i].x>maxx ) 
    {
      maxx = m_aVertex[i].x;
      id = i;
    }
  }
  return id;
}
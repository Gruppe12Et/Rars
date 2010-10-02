/**
 * FILE: G_GEOM.H (portable)
 *
 * This file contains definitions of the geometry function
 *
 * History
 *  ver. 0.1  Dec 10 - Creation
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <math.h>
#include "g_global.h"
#include "g_geom.h"

//--------------------------------------------------------------------------
//                           F U N C T I O N
//--------------------------------------------------------------------------

/**
 * Calculate the distance between a and b
 */
double TGeom::Dist( Vertex &a, Vertex &b )
{
  return sqrt( sqr(a.x-b.x)+sqr(a.y-b.y) );
}

/**
 * Calculate the distance of point p from the line a1, a2
 */
double TGeom::DistFromLine( Vertex &a1, Vertex &a2, Vertex &p )
{
  double sqr_size = sqr( a2.x-a1.x ) + sqr( a2.y-a1.y );
  if( sqr_size==0.0 )
  {
    return -1;
  }
  double u = ((p.x-a1.x)*(a2.x-a1.x)+(p.y-a1.y)*(a2.y-a1.y))/sqr_size;
  double x = a1.x + u *(a2.x-a1.x);
  double y = a1.y + u *(a2.y-a1.y);
  return sqrt( sqr(p.x-x)+sqr(p.y-y) );
}

/**
 * @param d1, d2 : extremity of a vector
 * @param p: point 
 * Return >0 if point is on the left side of the vector
 *        <0 if on the right side
 */
double TGeom::LeftSide( Vertex &a1, Vertex &a2, Vertex &p )
{
  double v1_x = a2.x-a1.x;
  double v1_y = a2.y-a1.y;
  double v2_x = p.x-a2.x;
  double v2_y = p.y-a2.y;

  return -v1_y*v2_x + v1_x*v2_y;
}

/**
 * Return the position of the point p in the segment a1, a2 ( if p==a1 -> 0, p==a2 -> 1 )
 */
double TGeom::PositionInSegment( Vertex &a1, Vertex &a2, Vertex &p )
{
  double sqr_size = sqr( a2.x-a1.x ) + sqr( a2.y-a1.y );
  if( sqr_size==0.0 )
  {
    return -1;
  }
  return ((p.x-a1.x)*(a2.x-a1.x)+(p.y-a1.y)*(a2.y-a1.y))/sqr_size;
}

//--------------------------------------------------------------------------
// Angles
//--------------------------------------------------------------------------

/**
 * Return a angle with a value from 0 to 2*PI
 */
double TGeom::Angle0to2PI( double ang )
{
  while( ang>=2*PI ) ang -= 2*PI;
  while( ang<0 ) ang += 2*PI;

  return ang;
}

/**
 * Return a angle with a value from -PI to PI
 */
double TGeom::AngleMinPItoPI( double ang )
{
  while( ang>=PI ) ang -= 2*PI;
  while( ang<-PI ) ang += 2*PI;

  return ang;
}

/**
 * Is ang between begin_ang and end_ang (begin_ang<end_ang)
 */
bool TGeom::AngleBetween( double ang, double begin_ang, double end_ang )
{
  if( end_ang>begin_ang ) 
  {
    return ang>begin_ang && ang<end_ang;
  }
  else
  {
    return (ang<2*PI&&ang>begin_ang) || (ang>=0&&ang<end_ang);
  }
}

//--------------------------------------------------------------------------
// Intersections
//--------------------------------------------------------------------------

/**
 * Intersection between 2 lines
 */
bool TGeom::IntersectionLineLine( Vertex &a1, Vertex &a2, Vertex &b1, Vertex &b2, Vertex &inter )
{
  // If b1 and b2 are on different side of the line(a1, a2) && opposite
  double x, y;
  double adx = a2.x-a1.x;
  double ady = a2.y-a1.y;
  double bdx = b2.x-b1.x;
  double bdy = b2.y-b1.y;
  if( adx==0.0 )
  {
    x = a1.x;
    y = bdy*(x-b1.x)/bdx+b1.y; 
  }
  else if( bdx==0.0 )
  {
    x = b1.x;
    y=  ady*(x-a1.x)/adx+a1.y; 
  }
  else
  {
    x = (b1.y-a1.y + a1.x*ady/adx - b1.x*bdy/bdx)/(ady/adx-bdy/bdx);
    y = ady*(x-a1.x)/adx+a1.y; 
  }
  inter.SetXYZ( x, y, 0 );
  return true;
}

/**
 * Intersection between 2 segments (=here segment in his geometrical meaning=part of a line between 2 points)
 * If one of the point of segment a, is on segment b, the intersection is not found
 *
 * @param a1        : segment a point 1
 * @param a2        : segment a point 2
 * @param b1        : segment b point 1
 * @param b2        : segment b point 2
 * @param inter     : the intersection
 * @return          : true if an intersection is found
 */
bool TGeom::IntersectionSegmentSegment( Vertex &a1, Vertex &a2, Vertex &b1, Vertex &b2, Vertex &inter )
{
  // If b1 and b2 are on different side of the line(a1, a2) && opposite
  if( LeftSide(a1,a2,b1)*LeftSide(a1,a2,b2)>=0 || LeftSide(b1,b2,a1)*LeftSide(b1,b2,a2)>=0 )
  {
    return false;
  }
  return IntersectionLineLine( a1, a2, b1, b2, inter );
}

/**
 * Intersection between 1 line and one circle 
 *
 * @param a1        : line point 1
 * @param a2        : line point 2
 * @param c1        : center of the circle
 * @param r         : radius of the circle 
 * @param inter1    : the intersection 1
 * @param inter2    : the intersection 2
 * @return          : true if an intersection is found
 */
bool TGeom::IntersectionLineCircle( Vertex &a1, Vertex &a2, Vertex &c1, double r, Vertex &inter1, Vertex &inter2 )
{          
  // Check if there is a intersection
  if( DistFromLine(a1, a2, c1)>=fabs(r) ) 
  {
    return false;
  }
  double x1, y1, x2, y2;
  double adx = a2.x-a1.x;
  if( adx==0.0 )
  {
    x1 = x2 = a1.x;
    double a = 1; 
    double b = -2*c1.y; 
    double c = sqr(a1.x-c1.x) + sqr(c1.y) - sqr(r); 
    double rac = b*b - 4*a*c;
    if( rac>=0 )
    {
      y1 = ( -b + sqrt( rac ) ) / (2*a);
      y2 = ( -b - sqrt( rac ) ) / (2*a);
    }
  }
  else
  {
    double ady = a2.y-a1.y;
    double a = 1           + sqr( ady/adx ); 
    double b = -2*c1.x     + 2*ady/adx*(a1.y-c1.y-a1.x*ady/adx); 
    double c = sqr(c1.x) + sqr(a1.y-c1.y-a1.x*ady/adx) - sqr(r); 
    double rac = b*b - 4*a*c;
    if( rac>=0 )
    {
      x1 = ( -b + sqrt( rac ) ) / (2*a);
      y1 = ady*(x1-a1.x)/adx+a1.y; 

      x2 = ( -b - sqrt( rac ) ) / (2*a);
      y2 = ady*(x2-a1.x)/adx+a1.y; 
    }
  }
  inter1.SetXYZ( x1, y1, 0 );
  inter2.SetXYZ( x2, y2, 0 );
  return true;
}

/**
 * Intersection between 1 segment and one arc of a circle (=here segment in his geometrical meaning=part of a line between 2 points)
 *
 * @param a1        : segment point 1
 * @param a2        : segment point 2
 * @param c1        : center of the circle
 * @param r         : radius of the circle 
 * @param begin_ang : begin angle of the arc
 * @param end_ang   : end angle of the arc
 * @param dir       : >0 -> arc is clockwise (begin_and<end_ang) else opposite
 * @param inter     : the intersection
 * @return          : true if an intersection is found
 */
bool TGeom::IntersectionSegmentArc( Vertex &a1, Vertex &a2, Vertex &c1, double r, double begin_ang, double end_ang, double dir, Vertex &inter )
{          
  Vertex inter1, inter2;
  
  if( !IntersectionLineCircle(a1,a2,c1,r,inter1,inter2) )
  {
    return false;
  }
  begin_ang = Angle0to2PI( begin_ang );
  end_ang = Angle0to2PI( end_ang );

  bool ok1, ok2;
  double ang1 = Angle0to2PI( atan2(inter1.y-c1.y,inter1.x-c1.x) );
  double ang2 = Angle0to2PI( atan2(inter2.y-c1.y,inter2.x-c1.x) );

  // Intersection between begin_ang and end_ang ?
  if( dir>0 ) // begin_ang<end_ang
  {
    ok1 = AngleBetween( ang1, begin_ang, end_ang );
    ok2 = AngleBetween( ang2, begin_ang, end_ang );
  }
  else // begin_ang>end_ang
  {
    ok1 = AngleBetween( ang1, end_ang, begin_ang );
    ok2 = AngleBetween( ang2, end_ang, begin_ang );
  }

  // Intersection between the 2 points of the segment
  double pos1 = PositionInSegment( a1, a2, inter1 );
  double pos2 = PositionInSegment( a1, a2, inter2 );
  ok1 = ok1 && ( pos1>0 && pos1<1.0 );
  ok2 = ok2 && ( pos2>0 && pos2<1.0 );

  if( ok1 ) 
  {
    inter = inter1;
    return true;
  }
  else if( ok2 ) 
  {
    inter = inter2;
    return true;
  }
  return false;
}


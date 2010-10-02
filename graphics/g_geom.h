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

#ifndef __G_GEOM_H
#define __G_GEOM_H

//--------------------------------------------------------------------------
//                            T Y P E S
//--------------------------------------------------------------------------

/**
 * Vertex struct to make code easier to read in places
 */
class Vertex 
{
  public: 
    float x;
    float y;
    float z;

    Vertex() {}
    Vertex( float _x, float _y, float _z )
    {
      x = _x;
      y = _y;
      z = _z;
    }

    void SetXYZ( double _x, double _y, double _z )
    {
      x = (float)_x; 
      y = (float)_y;
      z = (float)_z;
    };

    bool operator==(Vertex &v)
    {
      return v.x==x && v.y==y && v.z==z;
    }
};

/**
 * Calculate the distance of the point p from the line a1, a2
 */
class TGeom
{
  public:
    static double Dist( Vertex &a, Vertex &b );
    static double DistFromLine( Vertex &a1, Vertex &a2, Vertex &p );
    static double LeftSide( Vertex &a1, Vertex &a2, Vertex &p );
    static double PositionInSegment( Vertex &a1, Vertex &a2, Vertex &p );

    static double Angle0to2PI( double ang );
    static double AngleMinPItoPI( double ang );
    static bool AngleBetween( double ang, double begin_ang, double end_ang );

    static bool IntersectionLineLine( Vertex &a1, Vertex &a2, Vertex &b1, Vertex &b2, Vertex &inter );
    static bool IntersectionSegmentSegment( Vertex &a1, Vertex &a2, Vertex &b1, Vertex &b2, Vertex &inter );
    static bool IntersectionLineCircle( Vertex &a1, Vertex &a2, Vertex &c1, double r, Vertex &inter1, Vertex &inter2 );
    static bool IntersectionSegmentArc( Vertex &a1, Vertex &a2, Vertex &c1, double r, double begin_ang, double end_ang, double dir, Vertex &inter );
};

#endif // __G_GEOM_H


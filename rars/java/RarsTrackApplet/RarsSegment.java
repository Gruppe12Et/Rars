package RarsTrackApplet;

/**
 * The <code>RarsSegment</code> class is a strcuture that contains
 * a segment of a RarsTrack
 *
 * @author  Marc Gueury
 * @version X.X, Sep 27 2000
 */

class RarsSegment
{
   double radius;           // 0.0 means straight line, < 0.0 means right turn
   double length;           // radians if an arc, feet if straight line
   double beg_x, beg_y;     // coordinates of begining of segment
   double end_x, end_y;     // coordinates of end
   double cen_x, cen_y;     // coordinates of arc center (if arc)
   double beg_ang, end_ang; // path angles, radians, 0.0 is in x direction
}


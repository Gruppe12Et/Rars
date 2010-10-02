package RarsTrackApplet;

import java.awt.*;
import java.io.*;
import java.text.*;

/**
 * The <code>RarsTrack</code> is a paintable class that display a rars
 * track.
 *
 * @author  Marc Gueury
 * @version X.X, Sep 27 2000
 */

class RarsTrack
{
  Font font = new Font( "Dialog", 1, 14 );
  String name;

  int NSEG;              // number of track segments (see drawpath() in TRACK.CPP)
  double width;          // width of track, feet
  double length;

  // g_view.h
  double scale_x, scale_y;
  double top_x, top_y;
  double center_x, center_y;
  int max_x, max_y, min_x, min_y;

  RarsSegment trackout[];     // track defining arrays - see TRACK.CPP
  RarsSegment trackin[];

  double TRK_STRT_X;     // coordinates of where to start drawing track
  double TRK_STRT_Y;     // (feet)
  double SCORE_BOARD_X;  // These are in feet, track coordinates
  double SCORE_BOARD_Y;  // (where the scoreboard is located)
  double LDR_BRD_X;      // upper left corner of leader board, feet
  double LDR_BRD_Y;
  double LOTIX, LOTIY;   // where "Length of track... " starts
  double IP_X, IP_Y;     // Instrument Panel
  double FINISH;         // fraction of segment 0 prior to finish line
  double X_MAX, Y_MAX;   // maximum values that display must show, feet
  int    PIT_SIDE;       // 1 is right;  -1 is left
  double PIT_ENTRY;      // where cars start steering to pits
  double PITLANE_START;  // pitlane start-end denote stopping area
  double PITLANE_END;    // those four are compared to s.distance
  double PIT_EXIT;       // back to the track
  double PITLANE_SPEED;  // fps (or approx. km/h), divide by 1.8 to get mph

  /**
   * RarsCanvas : Contructor
   *
   * @param  track  the track to display
   */
  RarsTrack( String track_name )
  {
    name = track_name;
  }

  /**
   * Reads the track file
   *
   * @param data stream of the track file
   */
  public void read( TextInputStream data ) throws IOException
  {
    int i;
    String line;
    int degrees = -1;

    NSEG = data.readTextInt();
    line = data.readLine();
    X_MAX = data.readTextDouble();
    Y_MAX = data.readTextDouble();
    line = data.readLine();
    width = data.readTextDouble();
    line = data.readLine();
    TRK_STRT_X = data.readTextDouble();
    TRK_STRT_Y = data.readTextDouble();
    line = data.readLine();
    SCORE_BOARD_X = data.readTextDouble();
    SCORE_BOARD_Y = data.readTextDouble();
    line = data.readLine();
    LDR_BRD_X = data.readTextDouble();
    LDR_BRD_Y = data.readTextDouble();
    line = data.readLine();
    IP_X = data.readTextDouble();
    IP_Y = data.readTextDouble();
    line = data.readLine();
    LOTIX = data.readTextDouble();
    LOTIY = data.readTextDouble();
    line = data.readLine();
    FINISH = data.readTextDouble();
    line = data.readLine();
    PIT_SIDE = (int)data.readTextDouble();
    line = data.readLine();
    PIT_ENTRY = data.readTextDouble();
    PITLANE_START = data.readTextDouble();
    line = data.readLine();
    PITLANE_END = (int)data.readTextDouble();
    PIT_EXIT = data.readTextDouble();
    line = data.readLine();
    PITLANE_SPEED = data.readTextDouble();
    line = data.readLine();

    trackout = new RarsSegment[NSEG];   // allocate RAM for track segment arrays
    trackin = new RarsSegment[NSEG];

    for(i=0; i<NSEG; i++)
    { // read the segment data:
      trackout[i] = new RarsSegment();
      trackin[i] = new RarsSegment();
      trackout[i].radius = data.readTextDouble();
      trackout[i].length = data.readTextDouble();
      line = data.readLine();
      // this section of the loop handles (degree vs. radians):
      if(degrees == -1 && trackout[i].radius != 0.0)
        if(trackout[i].length < 5.0)
          degrees = 0;
        else
          degrees = 1;
      if(degrees == 1 && trackout[i].radius != 0.0)
        trackout[i].length *= 2.0*Math.PI/360.0;
    }

    for(i=0; i<NSEG; i++)
    { // fill in trackin[] from trackout[]
      if(trackout[i].radius == 0.0)
        trackin[i].radius = trackout[i].radius;
      else
        trackin[i].radius = trackout[i].radius - width;
      trackin[i].length = trackout[i].length;
    }
  } // read()

  /**
   * Draws the path specified by the segment array and starting
   * conditions which are given as parameters.  Also, fills in the
   * un-initialized portions of the segment array.  Returns the length.
   *
   * @param  xstart   coordinates of starting point
   * @param  ystart   coordinates of starting point
   * @param  alfstart tarting tangent angle
   * @param  track    pointer to structure that defines path
   */
  double drawpath(double xstart,
  		            double ystart,
               		double alfstart,
                  RarsSegment track[])
  {
    double length = 0;          // to accumulate total length of path
    double cenx, ceny;          // center of circle arc
    double radius;              // radius of circle arc (negative == rt. turn)
    double x, y, alf;           // position and direction of start of segment
    double newx, newy, newalf;  // and the one after that  (alf in radians)
    int i;

    x = xstart;  y = ystart;    // store starting point & direction
    alf = alfstart;

    for(i=0; i < NSEG; i++) {                 // for each segment:
      radius = track[i].radius;
      if(radius == 0.0) {                   // is this a straightaway?
  	    length += track[i].length;
        newx = x + track[i].length * Math.cos(alf);      // find end coordinates
        newy = y + track[i].length * Math.sin(alf);
        track[i].end_x = newx;   track[i].end_y = newy;    // fill in these
        track[i].beg_x = x;      track[i].beg_y = y;       // empty slots in
        track[i].beg_ang = track[i].end_ang = alf;         // the track array
        newalf = alf;                                // direction won't change
      } else if(radius > 0.0) {
        length += radius * track[i].length;
        cenx = x - radius * Math.sin(alf);  // compute center location:
        ceny = y + radius * Math.cos(alf);
        track[i].cen_x = cenx;   track[i].cen_y = ceny;  // fill empty slots
        track[i].beg_ang = alf;
        newalf = alf + track[i].length;           // compute new direction
        if(newalf > 2.0 * Math.PI)
          newalf -= 2.0 * Math.PI;
        track[i].end_ang = newalf;                // fill this empty slot
        newx = cenx + radius * Math.sin(newalf);   // location of end
        newy = ceny - radius * Math.cos(newalf);
        track[i].end_x = newx;   track[i].end_y = newy;  // fill in these
        track[i].beg_x = x;      track[i].beg_y = y;     // empty slots
      } else {
        length -= radius * track[i].length;
        cenx = x - radius * Math.sin(alf);  // compute center location:
        ceny = y + radius * Math.cos(alf);
        track[i].cen_x = cenx;   track[i].cen_y = ceny;  // fill empty slots
        track[i].beg_ang = alf;
        newalf = alf - track[i].length;           // compute new direction
        if(newalf < -2.0 * Math.PI)
          newalf += 2.0 * Math.PI;
        track[i].end_ang = newalf;                // fill this empty slot
        newx = cenx + radius * Math.sin(newalf);   // location of end
        newy = ceny - radius * Math.cos(newalf);
        track[i].end_x = newx;   track[i].end_y = newy;  // fill in these
        track[i].beg_x = x;      track[i].beg_y = y;     // empty slots
      }
      x = newx;                     // repeat with new position and direction:
      y = newy;
      alf = newalf;
    }
    // To close the circuit, draw a line from the last point back to the first.
    // This usually is not necessary, but it prevents flood fill leaking.

    return length;      // return the length of the path
  }

  /**
   * Calculate everything about the track.
   */
  void calc()
  {
    double alt_len;
    // draw outer track boundary:
    length = drawpath(TRK_STRT_X, TRK_STRT_Y, 0, trackout);
    // draw inner track boundary:
    alt_len = drawpath(TRK_STRT_X, TRK_STRT_Y+width, 0, trackin);
    if(alt_len < length)     // take length of shorter rail as track length
      length = alt_len;
    FullScreenScale();
  }

  /**
   * Find the extreme coordinates of the track to display it automatically
   */
  void FullScreenScale()
  {
    int i, j;

    // look for the min and max points of the track
    max_x = (int) trackin[0].beg_x;
    max_y = (int) trackin[0].beg_y;
    min_x = max_x;
    min_y = max_y;

    for(i=0; i<NSEG; i++)
	  {
      // for each segment:
      if( trackin[i].radius!=0.0 )
		  {
	      int step = (int)( trackin[i].length*5.0 );
	      double ang = trackin[i].beg_ang;

        for( j=0; j<step; j++ )
			  {
          max_x = (int) Math.max( max_x, trackin[i].cen_x + trackin[i].radius*Math.sin(ang) );
          max_x = (int) Math.max( max_x, trackin[i].cen_x + trackout[i].radius*Math.sin(ang) );
          max_y = (int) Math.max( max_y, trackin[i].cen_y - trackin[i].radius*Math.cos(ang) );
          max_y = (int) Math.max( max_y, trackin[i].cen_y - trackout[i].radius*Math.cos(ang) );
          min_x = (int) Math.min( min_x, trackin[i].cen_x + trackin[i].radius*Math.sin(ang) );
          min_x = (int) Math.min( min_x, trackin[i].cen_x + trackout[i].radius*Math.sin(ang) );
          min_y = (int) Math.min( min_y, trackin[i].cen_y - trackin[i].radius*Math.cos(ang) );
          min_y = (int) Math.min( min_y, trackin[i].cen_y - trackout[i].radius*Math.cos(ang) );

          if( trackin[i].radius>0.0 )
				  {
            ang+=0.2;
	          if(ang > 2.0 * Math.PI) ang -= 2.0 * Math.PI;
				  }
          else
				  {
            ang-=0.2;
            if(ang < 2.0 * Math.PI) ang += 2.0 * Math.PI;
				  }
			  }
      }
      else
		  {
        max_x = (int) Math.max( max_x, trackin[i].beg_x );
        max_x = (int) Math.max( max_x, trackout[i].beg_x );
        max_y = (int) Math.max( max_y, trackin[i].beg_y );
        max_y = (int) Math.max( max_y, trackout[i].beg_y );
        min_x = (int) Math.min( min_x, trackin[i].beg_x );
        min_x = (int) Math.min( min_x, trackout[i].beg_x );
        min_y = (int) Math.min( min_y, trackin[i].beg_y );
        min_y = (int) Math.min( min_y, trackout[i].beg_y );
		  }
	  }
  }

  /**
   * X-Scale a point from track coordinates to screen coordinates
   */
  int X_SCALE( double a)
  {
    return (int)( (a-top_x)*scale_x );
  }

  /**
   * Y-Scale a point from track coordinates to screen coordinates
   */
  int Y_SCALE( double a)
  {
    return (int)( (a-top_y)*scale_y );
  }

  /**
   * Paint the track
   *
   * @param   g  the graphics context
   * @param   r  the rectangle border
   */
  public void paint(Graphics g, Rectangle r)
  {
    Color ROAD_COLOR = Color.darkGray;
    Color START_COLOR = Color.white;

	  // calculates center_xy, top_xy, scale_xy with min and max
    scale_x = Math.min( (float)r.width/(float)(max_x-min_x), (float)r.height/(float)(max_y-min_y) )*0.95;
    scale_x *= 0.95;
    scale_y= - scale_x;
    center_x = (max_x+min_x)*0.5;
    center_y = (max_y+min_y)*0.5;
    top_x = center_x - r.width/(2*scale_x);
    top_y = center_y - r.height/(2*scale_y);

    int vx[] = new int[4];
    int vy[] = new int[4];
    int i, j;

    for(i=0; i<NSEG; i++) {                 // for each segment:
      if( trackin[i].radius==0.0 ) {
      	vx[0]=X_SCALE( trackin[i].end_x );
        vy[0]=Y_SCALE( trackin[i].end_y );
        vx[1]=X_SCALE( trackout[i].end_x );
        vy[1]=Y_SCALE( trackout[i].end_y );
        vx[2]=X_SCALE( trackout[i].beg_x );
        vy[2]=Y_SCALE( trackout[i].beg_y );
        vx[3]=X_SCALE( trackin[i].beg_x );
        vy[3]=Y_SCALE( trackin[i].beg_y );
        g.setColor( ROAD_COLOR );
        g.fillPolygon( vx, vy, 4 );
      } else {
      	int step = (int)( trackin[i].length*5.0 );
        vx[2]=X_SCALE( trackout[i].beg_x );
        vy[2]=Y_SCALE( trackout[i].beg_y );
        vx[3]=X_SCALE( trackin[i].beg_x );
        vy[3]=Y_SCALE( trackin[i].beg_y );

        double ang = trackin[i].beg_ang;
        double cenx = trackin[i].cen_x;
        double ceny = trackin[i].cen_y;
        if(trackin[i].radius>0.0) {
          for( j=0; j<step; j++) {
            ang+=0.2;
            if(ang > 2.0 * Math.PI) ang -= 2.0 * Math.PI;
            vx[0]= X_SCALE( cenx+trackin[i].radius*Math.sin(ang) );
            vy[0]= Y_SCALE( ceny-trackin[i].radius*Math.cos(ang) );
            vx[1]= X_SCALE( cenx+trackout[i].radius*Math.sin(ang) );
            vy[1]= Y_SCALE( ceny-trackout[i].radius*Math.cos(ang) );
            g.setColor( ROAD_COLOR );
            g.fillPolygon( vx, vy, 4 );
            vx[2]=vx[1];vy[2]=vy[1];
            vx[3]=vx[0];vy[3]=vy[0];
          }
        } else {
          for( j=0; j<step; j++) {
            ang-=0.2;
            if(ang < 2.0 * Math.PI) ang += 2.0 * Math.PI;
            vx[0]= X_SCALE( cenx+trackin[i].radius*Math.sin(ang) );
            vy[0]= Y_SCALE( ceny-trackin[i].radius*Math.cos(ang) );
            vx[1]= X_SCALE( cenx+trackout[i].radius*Math.sin(ang) );
            vy[1]= Y_SCALE( ceny-trackout[i].radius*Math.cos(ang) );
            g.setColor( ROAD_COLOR );
            g.fillPolygon( vx, vy, 4 );
            vx[2]=vx[1];vy[2]=vy[1];
            vx[3]=vx[0];vy[3]=vy[0];
          }
        }
        vx[0]=X_SCALE( trackin[i].end_x );
        vy[0]=Y_SCALE( trackin[i].end_y );
        vx[1]=X_SCALE( trackout[i].end_x );
        vy[1]=Y_SCALE( trackout[i].end_y );
        g.setColor( ROAD_COLOR );
        g.fillPolygon( vx, vy, 4 );
      }
      if( i==0 ) {
        // assume straightaway parallel to x-axis
        vx[2] = vx[3] = vx[2] + (int)( FINISH*(vx[0]-vx[2]) );
        vx[0] = vx[1] = vx[2] + (int)( 10.0*scale_x );
        g.setColor( START_COLOR );
        g.fillPolygon( vx, vy, 4 );
      }
    }

	  int sx = 10;
	  int sy = r.height - 28;
    g.setColor( Color.white );
    g.setFont( font );
	  g.drawString("Track: " + name, sx, sy);
    String sLength = NumberFormat.getInstance().format((length/5280.0));
	  g.drawString("Length: " + sLength + " miles" , sx, sy+14);
  }
}


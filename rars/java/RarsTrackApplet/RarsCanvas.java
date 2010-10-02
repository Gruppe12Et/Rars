package RarsTrackApplet;

import java.lang.Math;
import java.awt.*;
import java.io.*;
import java.net.*;
import java.applet.*;

/**
 * The <code>RarsCanvas</code> class is a canvas that display
 * a RarsTrack
 *
 * @author  Marc Gueury
 * @version X.X, Sep 27 2000
 */

class RarsCanvas extends Canvas
{
  int		    startAngle = 0;
  int		    endAngle   = 45;
  boolean	  filled     = false;
  RarsTrack  t;

  /**
   * RarsCanvas : Contructor
   *
   * @param  track  the track to display
   */
  public RarsCanvas( RarsTrack track )
  {
    t = track;
    setBackground( new Color( 0, 190, 0) );
  }

  /**
   * Paint the track on the canvas
   *
   * @param  g  the graphics context
   */
  public void paint(Graphics g)
  {
	  Rectangle r = this.getBounds();
	  t.paint( g, r );
  }
}



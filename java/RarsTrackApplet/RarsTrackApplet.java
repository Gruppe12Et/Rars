package RarsTrackApplet;

import java.applet.*;
import java.awt.*;
import java.io.*;
import java.lang.Math;
import java.net.*;

/**
 * The <code>RarsTrackApplet</code> class is a the applet that shows
 * a RarsTrack
 *
 * @author  Marc Gueury
 * @version X.X, Sep 27 2000
 */

public class RarsTrackApplet extends Applet
{
  String arg_track_name;

  /**
   * Initialize the applet
   */
   public void init()
  {
	  setLayout(new BorderLayout());
    try
    {
      // Read the track name from the arguments
      arg_track_name = getParameter("track_name");
      System.out.println( "Track : " + arg_track_name );

      // Open the file
      URL url = new URL( getCodeBase(), arg_track_name );
      InputStream in = url.openStream();
      TextInputStream data = new TextInputStream( new BufferedInputStream(in) );

      // Create the layout
      RarsTrack t = new RarsTrack( arg_track_name );
      t.read( data );
      t.calc();

	    RarsCanvas c = new RarsCanvas( t );
	    add("Center", c);
     }
     catch( Exception e )
     {
       e.printStackTrace( System.err );
       stop();
     }
  }
}



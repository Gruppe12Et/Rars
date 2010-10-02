package RarsTrackApplet;

import java.io.*;

/**
 * The <code>TextInputStream</code> class allows to reads direcly numbers
 * int/double from a DataInputStream
 *
 * @author  Marc Gueury
 * @version X.X, Sep 27 2000
 */

class TextInputStream extends DataInputStream
{
  /**
   * Constructor  : TextInputStream
   *
   * @param  in  Original InputStream
   */
  public TextInputStream(InputStream in)
  {
    super(in);
  }

  /**
   * Read a "int" from the stream
   *
   * @return a integer
   */
  public final int readTextInt() throws IOException
  {
    String s = "";
    char c;

    do {
      c = (char) read();
    } while( c=='\r'||c=='\n'||c=='\t'||c==' ' );

    if( (c<'0' || c>'9') && c!='.'&&c!='-' ) {
      throw new IOException();
    }

    do {
      s=s+(char)c;
      mark( 100000 );
      c = (char) read();
    } while( !(c=='\r'||c=='\n'||c=='\t'||c==' ') );
    reset();

    return Integer.valueOf(s).intValue();
  }

  /**
   * Read a "double" from the stream
   *
   * @return a integer
   */
  public final double readTextDouble() throws IOException
  {
    String s = "";
    char c;
    int i;

    do {
      c = (char) read();
      i = c;
    } while( c=='\r'||c=='\n'||c=='\t'||c==' ' );

    if( (c<'0' || c>'9') &&c!='.'&&c!='-' ) {
      throw new IOException();
    }

    do {
      s=s+(char)c;
      mark( 100000 );
      c = (char) read();
    } while( !(c=='\r'||c=='\n'||c=='\t'||c==' ') );
    reset();

    return Double.valueOf(s).doubleValue();
  }
}

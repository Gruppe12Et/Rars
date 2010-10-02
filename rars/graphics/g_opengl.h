/**
 * FILE: G_OPENGL.H (portable)
 *
 * OpenGL Help classe
 *
 * History
 *  ver. 0.1  Mar 2002 - Creation
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef OPENGL_H
#define OPENGL_H

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Class OpenGL
 */
class OpenGL
{
    int m_SizeX;
    int m_SizeY;

  public:
    unsigned int m_iSmallFont;
    unsigned int m_iBigFont;

    void Init( int iSizeX, int iSizeY, double max_dist );
    void Destroy();

    void VideoMemoryInit();
    void * VideoMemoryAlloc( long size );
    void VideoMemoryFree( void * p );
    void VideoMemoryDestroy();

    void BuildFont();
    void KillFont();
    void glPrint( int base, const char *fmt, ...);
};

extern OpenGL opengl;

#endif 


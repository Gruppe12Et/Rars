/**
 * OpenGL Texture Class
 */

#ifndef GLTEXTURE_H
#define GLTEXTURE_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#ifdef WIN32
  #include "windows.h"
  #include <GL/glaux.h>
#endif
#include <GL/gl.h>      // Header File For The OpenGL32 Library
#include <GL/glu.h>     // Header File For The GLu32 Library

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Class GLTexture
 */
class GLTexture  
{
private:
  GLubyte * m_ImageData;                         // Image data (up to 32 Bits)
  bool m_bMipmap;                                // Use Mipmap

  int LoadUncompressedTGA( FILE * file );
  int LoadCompressedTGA( FILE * file );
  int ReadBMPHeaders( FILE * file, int &info_size );
  void FindAlphaChannel();
  void CheckWidth();

  int LoadTGA( FILE * file );                    // Loads a targa file
  int LoadBMP( FILE * file );                    // Loads a bmp file
  int Load( const char * sPath, const char * sName, bool bMipmap );   // Load the texture

public:
  char m_sTextureName[128];                      // The textures name
  unsigned int m_iOpenglId;                      // OpenGL's number for the texture
  int width;                                     // Texture's width
  int height;                                    // Texture's height
  int bpp;                                       // Bits per pixel
  GLuint type;                                   // Texture type

  GLTexture();                                   // Constructor
  virtual ~GLTexture();                          // Destructor

  void Use();                                    // Binds the texture for use
  void BuildColorTexture(unsigned char r, unsigned char g, unsigned char b);
  void BindTexture();                            // Bind the texture in OpenGL

  static GLTexture * GetLoadedTexture( const char * sPath, const char * sName, bool bMipmap );
  static void RebindLoadedTexture();
  static void UnbindLoadedTexture();
  static void DeleteLoadedTexture();
};

extern GLTexture * g_TextureEnvMap;

#endif

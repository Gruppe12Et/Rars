/**
 * OpenGL Texture Class
 *
 * 01-04-01 Matthew Fairfax     Creation
 * 15-08-01 Marc Gueury         Nehe compressed targa
 *
 * GLTexture.cpp: implementation of the GLTexture class.
 * This class loads a texture file and prepares it
 * to be used in OpenGL. It can open a bitmap or a
 * targa file. 
 *
 * Usage:
 * GLTexture tex1;
 * GLTexture tex2;
 * GLTexture tex3;
 *
 * tex1.Load("texture.tga");    // Loads a targa
 * tex1.Use();                  // Binds the targa for use
 *
 * tex1.Load("texture.bmp");    // Loads a BMP
 * tex1.Use();                  // Binds the BMP for use
 *
 * // You can also build a texture with a single color and use it
 * tex3.BuildColorTexture(255, 0, 0); // Builds a solid red texture
 * tex3.Use();         // Binds the targa for use
 *
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include "../misc.h"
#include "gl_texture.h"

//--------------------------------------------------------------------------
//                           T Y P E S
//--------------------------------------------------------------------------

typedef enum TextureError
{ 
  ERR_FILE_NOT_FOUND = 1,
  ERR_READING_HEADER,
  ERR_UNKNOWN_FORMAT,
  ERR_INVALID_FORMAT,
  ERR_NOT_ENOUGH_MEMORY,
  ERR_READING_RLE_HEARDER,
  ERR_READING_DATA,
  ERR_TOO_MANY_PIXELS
};

// Format TGA
typedef struct
{
  GLubyte Header[12];                 // TGA File Header
} TGAHeader;

typedef struct
{
  GLubyte   header[6];                 // First 6 Useful Bytes From The Header
  GLuint    bytesPerPixel;             // Holds Number Of Bytes Per Pixel Used In The TGA File
  GLuint    imageSize;                 // Used To Store The Image Size When Setting Aside Ram
  GLuint    temp;                      // Temporary Variable
  GLuint    type; 
  GLuint    Height;                    //Height of Image
  GLuint    Width;                     //Width ofImage
  GLuint    Bpp;                       // Bits Per Pixel
} TGA;

// Format BMP
struct BitmapArrayHeader
{
   unsigned int     size;
   unsigned int     next_offset;
   unsigned short   x_display;
   unsigned short   y_display;
};

struct BMPFileHeader
{
   int              size;
   unsigned short   reserved1;
   unsigned short   reserved2;
   int              offset_bits;
};

struct BMPInfoHeader
{
   int              width;
   int              height;
   short            planes;
   short            bit_count;
   int              compression;
   int              image_size;
   int              x_pixels_per_meter;
   int              y_pixels_per_meter;
   int              colors_used;
   int              colors_important;
};

struct BMPInfoHeaderOld
{
   short            width;
   short            height;
   short            planes;
   short            bit_count;
};

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

BMPInfoHeader bmpInfo;           
BMPFileHeader bmpHeader;           
TGAHeader tgaheader;                   // TGA header
TGA tga;                               // TGA image data
GLubyte uTGAcompare[12] = {0,0,2, 0,0,0,0,0,0,0,0,0}; // Uncompressed TGA Header
GLubyte cTGAcompare[12] = {0,0,10,0,0,0,0,0,0,0,0,0}; // Compressed TGA Header

#define MAX_TEXTURES 1000
GLTexture * g_aLoadedTexture[MAX_TEXTURES];
int g_iNbLoadedTexture = 0;
GLTexture * g_TextureEnvMap;

//--------------------------------------------------------------------------
//                           Class GLTexture
//--------------------------------------------------------------------------

/**
 * Constructor
 */
GLTexture::GLTexture()
{
  m_ImageData = NULL;
}

/**
 * Destructor
 */
GLTexture::~GLTexture()
{
  if( m_ImageData!=NULL ) 
  {
    delete m_ImageData;
    m_ImageData = NULL;
  }
}

/**
 * Load a texture
 *
 * @param name    : name of the bitmap file (tga or bmp)
 * @param bMipMap : render the texture with MipMap
 */
int GLTexture::Load( const char * sPath, const char * sName, bool bMipmap)
{
  char sFullName[256];
  strcpy( m_sTextureName, sName );
  sprintf( sFullName, "%s%s", sPath, sName );
  
  int error = 0;
  
  FILE * file;                               // File pointer to texture file
  file = fopen(sFullName, "rb");              // Open file for reading

  if( file==NULL )        
  {
    sprintf( sFullName, "tracks/texture/%s", sName );
    file = fopen(sFullName, "rb");            
    if( file==NULL )        
    {
      return ERR_FILE_NOT_FOUND;
    }
  }

  // If the file name is like *_nmm.* -> no mipmap
  if( strstr(sName,"_nmm.") || strstr(sName,"_NMM.") ) 
  {
    bMipmap = false;
  }

  // check the file extension to see what type of texture
  if( strstr(sName,".tga") || strstr(sName,".TGA") ) 
    error = LoadTGA( file );
  else if( strstr(sName,".bmp") || strstr(sName,".BMP") ) 
    error = LoadBMP( file );
  else
    error = ERR_UNKNOWN_FORMAT;

  if( error!=0 )
  {
    return error; 
  }
  FindAlphaChannel();
  m_bMipmap = bMipmap;
  BindTexture();

  g_aLoadedTexture[g_iNbLoadedTexture++] = this;

  return 0;
}

/**
 * Bind the texture in OpenGL
 */
void GLTexture::BindTexture()
{
  // Generate the OpenGL texture id
  glGenTextures(1, &m_iOpenglId);

  // Bind this texture to its id
  glBindTexture(GL_TEXTURE_2D, m_iOpenglId);

  // Use mipmapping filter
  // glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
  if( m_bMipmap )
  {
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
  }
  else
  {
    glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR);
  }
  glTexParameterf(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // Generate the mipmaps
  gluBuild2DMipmaps(GL_TEXTURE_2D, type, width, height, type, GL_UNSIGNED_BYTE, m_ImageData);
}

void GLTexture::Use()
{
  glEnable(GL_TEXTURE_2D);                       // Enable texture mapping
  glBindTexture(GL_TEXTURE_2D, m_iOpenglId);     // Bind the texture as the current one
}

/**
 * Build a dummy texture with the given color
 */
void GLTexture::BuildColorTexture(unsigned char r, unsigned char g, unsigned char b)
{
  unsigned char data[12]; // a 2x2 texture of 24 bits
  // Store the data
  for( int i=0; i<12; i+=3 )
  {
    data[i] = r;
    data[i+1] = g;
    data[i+2] = b;
  }
  // Generate the OpenGL texture id
  glGenTextures(1, &m_iOpenglId);

  // Bind this texture to its id
  glBindTexture(GL_TEXTURE_2D, m_iOpenglId);

  glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

  // Use mipmapping filter
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_NEAREST);
  glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);

  // Generate the texture
  gluBuild2DMipmaps(GL_TEXTURE_2D, 3, 2, 2, GL_RGB, GL_UNSIGNED_BYTE, data);
}

/**
 * Load a TGA file
 */
int GLTexture::LoadTGA( FILE * file )
{
  // Read TGA signature
  if( fread(&tgaheader, sizeof(TGAHeader), 1, file)==0 )
  {
    fclose( file );
    return ERR_READING_HEADER;
  }

  // Read TGA header
  if(fread(tga.header, sizeof(tga.header), 1, file) == 0)
  {                   
    return ERR_READING_HEADER;
  } 

  width      = tga.header[1] * 256 + tga.header[0]; // Determine The TGA Width  (highbyte*256+lowbyte)
  height     = tga.header[3] * 256 + tga.header[2]; // Determine The TGA Height (highbyte*256+lowbyte)
  bpp        = tga.header[4];                    // Determine the bits per pixel
  tga.Width  = width;                            // Copy width into local structure            
  tga.Height = height;                           // Copy height into local structure
  tga.Bpp    = bpp;                              // Copy BPP into local structure

  if((width <= 0) || (height <= 0) || ((bpp != 24) && (bpp !=32)))  // Make sure all information is valid
  {
    return ERR_INVALID_FORMAT;
  }

  if( bpp==24 )                                  //If the BPP of the image is 24...
  {
    type = GL_RGB;                               // Set Image type to GL_RGB
  }
  else                                           // Else if its 32 BPP
  {
    type = GL_RGBA;                              // Set image type to GL_RGBA
  }

  tga.bytesPerPixel = (tga.Bpp / 8);             // Compute BYTES per pixel
  tga.imageSize = (tga.bytesPerPixel * tga.Width * tga.Height);   // Compute amout of memory needed to store image
  m_ImageData = new GLubyte[tga.imageSize];      // Allocate that much memory

  if( m_ImageData==NULL )                        // If it wasnt allocated correctly..
  {
    return ERR_NOT_ENOUGH_MEMORY;
  }

  // See if signature matches the predefined header of compressed or uncompresed 
  if(memcmp(uTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
  {
    return LoadUncompressedTGA( file );
  }
  else if(memcmp(cTGAcompare, &tgaheader, sizeof(tgaheader)) == 0)
  {
    return LoadCompressedTGA( file );
  }
  else
  {
    fclose(file);
    return ERR_UNKNOWN_FORMAT;
  }
}

/**
 * Load an uncompressed TGA (note, much of this code is based on NeHe's )
 */
int GLTexture::LoadUncompressedTGA( FILE * file )
{
  if(fread(m_ImageData, 1, tga.imageSize, file) != tga.imageSize)  // Attempt to read image data
  {
    delete m_ImageData;
    m_ImageData = NULL;
    return ERR_READING_DATA;
  }

  // Byte Swapping Optimized By Steve Thomas
  for(GLint cswap = 0; cswap < (int)tga.imageSize; cswap += tga.bytesPerPixel)
  {
    m_ImageData[cswap] ^= m_ImageData[cswap+2] ^=
    m_ImageData[cswap] ^= m_ImageData[cswap+2];
  }

  return 0;                                 // Return success
}

/**
 * Load COMPRESSED TGAs
 */
int GLTexture::LoadCompressedTGA( FILE * file )
{
  GLuint pixelcount = tga.Height * tga.Width;    // Nuber of pixels in the image
  GLuint currentpixel = 0;                       // Current pixel being read
  GLuint currentbyte  = 0;                       // Current byte 
  GLubyte * colorbuffer = new GLubyte[tga.bytesPerPixel]; // Storage for 1 pixel
  if( colorbuffer==NULL )
  {
    return ERR_NOT_ENOUGH_MEMORY;
  }

  do
  {
    GLubyte chunkheader = 0;                     // Storage for "chunk" header

    if(fread(&chunkheader, sizeof(GLubyte), 1, file) == 0)        // Read in the 1 byte header
    {
      delete m_ImageData;
      m_ImageData = NULL;
      return ERR_READING_RLE_HEARDER;
    }

    if(chunkheader < 128)                       // If the ehader is < 128, it means the that is the number of RAW color packets minus 1
    {                                 // that follow the header
      chunkheader++;                          // add 1 to get number of following color values
      for(short counter = 0; counter < chunkheader; counter++)    // Read RAW color values
      {
        if(fread(colorbuffer, 1, tga.bytesPerPixel, file) != tga.bytesPerPixel) // Try to read 1 pixel
        {
          delete colorbuffer;
          delete m_ImageData;
          m_ImageData = NULL;
          return ERR_READING_DATA;
        }

        // write to memory
        m_ImageData[currentbyte    ] = colorbuffer[2];           // Flip R and B vcolor values around in the process 
        m_ImageData[currentbyte + 1  ] = colorbuffer[1];
        m_ImageData[currentbyte + 2  ] = colorbuffer[0];

        if(tga.bytesPerPixel == 4)               // if its a 32 bpp image
        {
          m_ImageData[currentbyte + 3] = colorbuffer[3];       // copy the 4th byte
        }

        currentbyte += tga.bytesPerPixel;        // Increase thecurrent byte by the number of bytes per pixel
        currentpixel++;                          // Increase current pixel by 1

        if(currentpixel > pixelcount)            // Make sure we havent read too many pixels
        {
          delete colorbuffer;
          delete m_ImageData;
          m_ImageData = NULL;
          return ERR_TOO_MANY_PIXELS;
        }
      }
    }
    else                                         // chunkheader > 128 RLE data, next color reapeated chunkheader - 127 times
    {
      chunkheader -= 127;                             // Subteact 127 to get rid of the ID bit
      if( fread(colorbuffer, 1, tga.bytesPerPixel, file)!=tga.bytesPerPixel )   // Attempt to read following color values
      { 
        delete colorbuffer;
        delete m_ImageData;
        m_ImageData = NULL;
        return ERR_READING_DATA;
      }

      for( short counter=0; counter<chunkheader; counter++ )          // copy the color into the image data as many times as dictated 
      {                                     // by the header
        m_ImageData[currentbyte    ] = colorbuffer[2];         // switch R and B bytes areound while copying
        m_ImageData[currentbyte + 1  ] = colorbuffer[1];
        m_ImageData[currentbyte + 2  ] = colorbuffer[0];

        if( tga.bytesPerPixel==4 )                        // If TGA images is 32 bpp
        {
          m_ImageData[currentbyte + 3] = colorbuffer[3];       // Copy 4th byte
        }

        currentbyte += tga.bytesPerPixel;                   // Increase current byte by the number of bytes per pixel
        currentpixel++;                             // Increase pixel count by 1

        if(currentpixel > pixelcount)                     // Make sure we havent written too many pixels
        {
          delete colorbuffer;
          delete m_ImageData;
          m_ImageData = NULL;
          return ERR_TOO_MANY_PIXELS;
        }
      }
    }
  }

  while(currentpixel < pixelcount);              // Loop while there are still pixels left
  return 0;                                      // return success
}

/**
 * Read a BMP file header
 */
int GLTexture::ReadBMPHeaders( FILE * file, int &info_size )
{
   fread(&bmpHeader, 1, sizeof(BMPFileHeader), file);
   fread(&info_size, 1, sizeof(int), file);

   if (info_size == 12)
   {
      BMPInfoHeaderOld old_info;
      fread(&old_info, 1, sizeof(BMPInfoHeaderOld), file);

      bmpInfo.width = old_info.width;
      bmpInfo.height = old_info.height;
      bmpInfo.planes = old_info.planes;
      bmpInfo.bit_count = old_info.bit_count;

      bmpInfo.compression = 0;
      bmpInfo.image_size = ((bmpInfo.bit_count * bmpInfo.width + 31) / 32)
                           * 4 * bmpInfo.height;
      bmpInfo.x_pixels_per_meter = 0;
      bmpInfo.y_pixels_per_meter = 0;
      bmpInfo.colors_used = 0;
      bmpInfo.colors_important = 0;
   }
   else if (info_size >= 40)
   {
      fread(&bmpInfo, 1, sizeof(BMPInfoHeader), file);
   }
   else
   {
      return ERR_READING_HEADER;
   }
   return 0;
}

/**
 * Load a BMP image
 */
int GLTexture::LoadBMP( FILE * file )
{
   char file_type[2];
   fread(&file_type, 1, 2, file);

   if (file_type[0] != 'B' || (file_type[1] != 'A' && file_type[1] != 'M'))
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   int best_res = -1;
   long best_offset = 0;
   if (file_type[1] == 'A')
   {
      // File is an archive of bitmaps.  Find the bitmap with the most detail.

      int count = 0;
      for (;;)
      {
         BitmapArrayHeader array;
         fread(&array, 1, sizeof(BitmapArrayHeader), file);
         fread(&file_type, 1, 2, file);
         long curr_offset = ftell(file);
         if (file_type[0] == 'B' && file_type[1] == 'M')
         {
            int info_size;
            if( !ReadBMPHeaders(file, info_size) )
            {
               ++count;
               if (bmpInfo.image_size >= best_res)
               {
                  best_res = bmpInfo.image_size;
                  best_offset = curr_offset;
               }
            }
         }
         if (array.next_offset)
         {
            fseek(file, (long)(array.next_offset+2), SEEK_SET);
         }
         else
         {
            break;
         }
      }
      if (best_res == -1)
      {
         fclose(file);
         return ERR_INVALID_FORMAT;
      }
      fseek(file, best_offset, SEEK_SET);
   }

   int info_size;

   if( ReadBMPHeaders(file, info_size) )
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }

   if (bmpInfo.width <= 0)
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   if (bmpInfo.height <= 0)
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   if (bmpInfo.planes != 1)
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   if (bmpInfo.bit_count != 1 && bmpInfo.bit_count != 4 &&
       bmpInfo.bit_count != 8 && bmpInfo.bit_count != 24)
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   if (bmpInfo.compression < 0 || bmpInfo.compression > 2 ||
       (bmpInfo.compression == 1 && bmpInfo.bit_count != 8) ||
       (bmpInfo.compression == 2 && bmpInfo.bit_count != 4))
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }
   if (bmpInfo.bit_count == 1 && bmpInfo.colors_used != 0 && bmpInfo.colors_used != 2)
   {
      fclose(file);
      return ERR_INVALID_FORMAT;
   }

   if (info_size > 40)
   {
      fseek(file, info_size-40, SEEK_CUR);
   }

   unsigned int num_pixels = bmpInfo.width * bmpInfo.height * 3;
   m_ImageData = new GLubyte[num_pixels];

   char *color_map = NULL;
   if (bmpInfo.bit_count != 24)
   {
      if (!bmpInfo.colors_used)
         bmpInfo.colors_used = 1 << bmpInfo.bit_count;
      color_map = new char[bmpInfo.colors_used * 3];
      for (int i=0; i<3*bmpInfo.colors_used; i+=3)
      {
         fread(&color_map[i+2], 1, 1, file);
         fread(&color_map[i+1], 1, 1, file);
         fread(&color_map[i], 1, 1, file);
         if (info_size != 12)
            fseek(file, 1, SEEK_CUR);
      }
   }

   fseek(file, bmpHeader.offset_bits, SEEK_SET);
   if (bmpInfo.bit_count == 1)
   {
      long pad = ((bmpInfo.width + 31) / 32)*4 - ((bmpInfo.width + 7)/8);
      char *iptr = (char*)m_ImageData;
      for (int y=0; y<bmpInfo.height; ++y)
      {
         for (int x=0; x<bmpInfo.width;)
         {
            char buf;
            fread(&buf, 1, 1, file);
            for (int i=0; i<8 && x<bmpInfo.width; ++x, ++i)
            {
               if (buf & 0x80)
               {
                  *iptr = color_map[3]; ++iptr;
                  *iptr = color_map[4]; ++iptr;
                  *iptr = color_map[5]; ++iptr;
               }
               else
               {
                  *iptr = color_map[0]; ++iptr;
                  *iptr = color_map[1]; ++iptr;
                  *iptr = color_map[2]; ++iptr;
               }
               buf <<= 1;
            }
         }
         fseek(file, pad, SEEK_CUR);
      }
   }
   else if (bmpInfo.bit_count == 4)
   {
      if (bmpInfo.compression)
      {
         int x = 0, y = 0;
         char *iptr = (char*)m_ImageData;
         while (y < bmpInfo.height)
         {
            unsigned char buf;
            fread(&buf, 1, 1, file);
            if (buf == 0)
            {
               fread(&buf, 1, 1, file);
               if (buf == 0)
               {
                  x = 0; ++y;
                  iptr = (char*)m_ImageData + y*bmpInfo.width*3 + x*3;
               }
               else if (buf == 1)
               {
                  break;
               }
               else if (buf == 2)
               {
                  fread(&buf, 1, 1, file);
                  x += buf;
                  fread(&buf, 1, 1, file);
                  y += buf;
                  iptr = (char*)m_ImageData + y*bmpInfo.width*3 + x*3;
               }
               else
               {
                  unsigned char num = buf;
                  for (unsigned char i = 0; i < num; ++i)
                  {
                     fread(&buf, 1, 1, file);
                     int indx = 3*((buf >> 4) & 0x0f);
                     *iptr = color_map[indx]; ++iptr;
                     *iptr = color_map[indx+1]; ++iptr;
                     *iptr = color_map[indx+2]; ++iptr;
                     ++x;
                     if (++i < num)
                     {
                        indx = 3 * (buf & 0x0f);
                        *iptr = color_map[indx]; ++iptr;
                        *iptr = color_map[indx+1]; ++iptr;
                        *iptr = color_map[indx+2]; ++iptr;
                        ++x;
                     }
                  }
                  if (num & 0x03 == 0x01 || num & 0x03 == 0x02)
                     fseek(file, 1, SEEK_CUR);
               }
            }
            else
            {
               unsigned char num = buf;
               fread(&buf, 1, 1, file);
               int indx1 = 3*((buf >> 4) & 0x0f);
               int indx2 = 3 * (buf & 0x0f);
               for (unsigned char i = 0; i < num; ++i)
               {
                  *iptr = color_map[indx1]; ++iptr;
                  *iptr = color_map[indx1+1]; ++iptr;
                  *iptr = color_map[indx1+2]; ++iptr;
                  if (++i < num)
                  {
                     *iptr = color_map[indx2]; ++iptr;
                     *iptr = color_map[indx2+1]; ++iptr;
                     *iptr = color_map[indx2+2]; ++iptr;
                  }
               }
               x += num;
            }
         }
      }
      else
      {
         long pad = ((4*bmpInfo.width + 31) / 32)*4 - ((4*bmpInfo.width + 7)/8);
         char *iptr = (char*)m_ImageData;
         for (int y=0; y<bmpInfo.height; ++y)
         {
            for (int x=0; x<bmpInfo.width; ++x)
            {
               unsigned char buf;
               fread(&buf, 1, 1, file);
               int indx = 3*((buf >> 4) & 0x0f);
               *iptr = color_map[indx]; ++iptr;
               *iptr = color_map[indx+1]; ++iptr;
               *iptr = color_map[indx+2]; ++iptr;
               if (++x < bmpInfo.width)
               {
                  indx = 3 * (buf & 0x0f);
                  *iptr = color_map[indx]; ++iptr;
                  *iptr = color_map[indx+1]; ++iptr;
                  *iptr = color_map[indx+2]; ++iptr;
               }
            }
            fseek(file, pad, SEEK_CUR);
         }
      }
   }
   else if (bmpInfo.bit_count == 8)
   {
      if (bmpInfo.compression)
      {
         int x = 0, y = 0;
         char *iptr = (char*)m_ImageData;
         while (y < bmpInfo.height)
         {
            unsigned char buf;
            fread(&buf, 1, 1, file);
            if (buf == 0)
            {
               fread(&buf, 1, 1, file);
               if (buf == 0)
               {
                  x = 0; ++y;
                  iptr = (char*)m_ImageData + y*bmpInfo.width*3 + x*3;
               }
               else if (buf == 1)
               {
                  break;
               }
               else if (buf == 2)
               {
                  fread(&buf, 1, 1, file);
                  x += buf;
                  fread(&buf, 1, 1, file);
                  y += buf;
                  iptr = (char*)m_ImageData + y*bmpInfo.width*3 + x*3;
               }
               else
               {
                  unsigned char num = buf;
                  for (unsigned char i = 0; i < num; ++i)
                  {
                     fread(&buf, 1, 1, file);
                     int indx = 3*buf;
                     *iptr = color_map[indx]; ++iptr;
                     *iptr = color_map[indx+1]; ++iptr;
                     *iptr = color_map[indx+2]; ++iptr;
                     ++x;
                  }
                  if (num & 0x01)
                     fseek(file, 1, SEEK_CUR);
               }
            }
            else
            {
               unsigned char num = buf;
               fread(&buf, 1, 1, file);
               int indx = 3*buf;
               for (unsigned char i = 0; i < num; ++i)
               {
                  *iptr = color_map[indx]; ++iptr;
                  *iptr = color_map[indx+1]; ++iptr;
                  *iptr = color_map[indx+2]; ++iptr;
               }
               x += num;
            }
         }
      }
      else
      {
         long pad = ((bmpInfo.width + 3) / 4)*4 - bmpInfo.width;
         char *iptr = (char*)m_ImageData;
         for (int y=0; y<bmpInfo.height; ++y)
         {
            for (int x=0; x<bmpInfo.width; ++x)
            {
               unsigned char buf;
               fread(&buf, 1, 1, file);
               int indx = 3*buf;
               *iptr = color_map[indx]; ++iptr;
               *iptr = color_map[indx+1]; ++iptr;
               *iptr = color_map[indx+2]; ++iptr;
            }
            fseek(file, pad, SEEK_CUR);
         }
      }
   }
   else
   {
      long pad = ((3*bmpInfo.width + 3) / 4)*4 - bmpInfo.width*3;
      char *iptr = (char*)m_ImageData;
      for (int y=0; y<bmpInfo.height; ++y)
      {
         fread(iptr, 1, 3*bmpInfo.width, file);
         for (int x=0; x<bmpInfo.width; ++x)
         {
            unsigned char tmp = *iptr;
            *iptr = *(iptr+2);
            *(iptr+2) = tmp;
            iptr += 3;
         }
         fseek(file, pad, SEEK_CUR);
      }
   }
   width = bmpInfo.width;
   height = bmpInfo.height;
   bpp = 3;
   type = GL_RGB;

   if( color_map )
   {
     delete color_map;
   }

   fclose(file);

   return 0;
}

/**
 * Find if the texture has some Purple Color
 * If true, create an alpha channel where the purple color is transparent.
 */
void GLTexture::FindAlphaChannel()
{ 
  if( type==GL_RGB )
  {
    if( bpp!=3 )
    {
      return;
    }
    // Find the number of pure purple pixels
    int size = width*height*3;
    int nb_purple = 0;
    int src;

    for( src=0; src<size; src+=3 )
    {
      if( m_ImageData[src]==255 &&  m_ImageData[src+1]==0 && m_ImageData[src+2]==255 )
      {
        nb_purple++;
      }
    }

    // If there are more than 3 pixels, it is transparent
    if( nb_purple>=3 )
    {
      GLubyte * image = new GLubyte[width*height*4];

      int dest = 0;
      for( src=0; src<size; src+=3 )
      {
        if( m_ImageData[src]==255 &&  m_ImageData[src+1]==0 && m_ImageData[src+2]==255 )
        {
          image[dest] = 0;
          image[dest+1] = 0;
          image[dest+2] = 0;
          image[dest+3] = 0;
        }
        else
        {
          image[dest] = m_ImageData[src];
          image[dest+1] = m_ImageData[src+1];
          image[dest+2] = m_ImageData[src+2];
          image[dest+3] = 255;
        }
        dest += 4;
      }
      delete m_ImageData;
      m_ImageData = image;
      type = GL_RGBA;
      bpp = 4;
    }
  }
}

/**
 * Check if the width of the texture is mod 8
 * If not change it
 */
void GLTexture::CheckWidth()
{ 
  if( bpp!=3 || width%8==0 )
  {
    return;
  }
  // Find the number of pure purple pixels
  int h, w;
  int new_width = ((width/8)+1)*8;

  GLubyte * image = new GLubyte[new_width*height*3];

  int dest = 0;
  int src = 0;
  for( h=0; h<height; h++ )
  {
    src  = h*width*3;
    dest = h*new_width*3;
    for( w=0; w<new_width; w++ )
    {
      image[dest] = m_ImageData[src];
      image[dest+1] = m_ImageData[src+1];
      image[dest+2] = m_ImageData[src+2];
      src +=3;
      dest +=3;
    }
    for( w=width; w<new_width; w++ )
    {
      image[dest]   = 0;
      image[dest+1] = 0;
      image[dest+2] = 0;
      dest +=3;
    }
  }
  delete m_ImageData;
  m_ImageData = image;
  width = new_width;
}

/**
 * Find a texture in the list of the already loaded textures
 */
GLTexture * GLTexture::GetLoadedTexture( const char * sPath, const char * sName, bool bMipmap )
{
  for( int i=0; i<g_iNbLoadedTexture; i++ )
  {
     if( strcmp( sName, g_aLoadedTexture[i]->m_sTextureName )==0 )
     {
       return g_aLoadedTexture[i];
     }
  }

  GLTexture * tex = new GLTexture();
  int error = tex->Load( sPath, sName, bMipmap );

  if( error )
  {
    warning( "GLTexture::GetLoadedTexture: Error '%d' while loading texture '%s%s'", error, sPath, sName );
    return GetLoadedTexture( "tracks/texture/", "unknown.bmp", true );
  }

  return tex;
}

/**
 * Unbind all the texture in OpenGL
 */
void GLTexture::RebindLoadedTexture()
{
  for( int i=0; i<g_iNbLoadedTexture; i++ )
  {
    g_aLoadedTexture[i]->BindTexture();
  }
}

/**
 * Unbind all the texture in OpenGL
 */
void GLTexture::UnbindLoadedTexture()
{
  GLuint todel[MAX_TEXTURES];
  for( int i=0; i<g_iNbLoadedTexture; i++ )
  {
    todel[i] = g_aLoadedTexture[i]->m_iOpenglId;
  }
  glDeleteTextures( g_iNbLoadedTexture, todel );
}

/**
 * Delete the textures
 */
void GLTexture::DeleteLoadedTexture()
{
  for( int i=0; i<g_iNbLoadedTexture; i++ )
  {
     delete g_aLoadedTexture[i];
     g_aLoadedTexture[i] = NULL;
  }

  g_iNbLoadedTexture = 0;
}

















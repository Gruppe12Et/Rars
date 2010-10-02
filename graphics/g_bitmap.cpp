
/**
 * FILE: G_BITMAP.CPP (portable)
 *
 *  TBitmap
 *  - The bitmaps are arrays of pixels that can be loaded from files
 *  and used by TLowGraphic as data.
 *
 * 
 * History
 *  ver. 0.1  Aug 96 - Creation
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury / Belgium <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76 
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <malloc.h>
#include "g_global.h"
#include "g_bitmap.h"

//--------------------------------------------------------------------------
//                              T Y P E
//--------------------------------------------------------------------------

typedef struct  
{
  char manufacturer;
  char version;
  char encoding;
  char bits_per_pixel;
  int16 xmin,ymin;
  int16 xmax,ymax;
  int16 hres;
  int16 vres;
  char palette[48];
  char reserved;
  char colour_planes;
  int16 bytes_per_line;
  int16 palette_type;
  char filler[58];
} PCXHEAD;

//--------------------------------------------------------------------------
//                            Class TBITMAP
//--------------------------------------------------------------------------

/**
 * Constructor
 */
TBitmap::TBitmap()
{
  m_Bitmap = NULL;
}

/**
 * Destructor
 */
TBitmap::~TBitmap()
{
  if( m_Bitmap!=NULL )
  {
    delete [] m_Bitmap;
    m_Bitmap = NULL;
  }
}

/**
 * Read a pcx file
 *
 * @param sFileName      the input file
 */
void TBitmap::PCX_Read( const char * sFileName )
{
  FILE * in;
  PCXHEAD       header;
  unsigned int  bytes, bits;
  char          palette[768];
  int           i;

  // Try to open the fileentative d'ouverture du fichier 
  if((in=fopen(sFileName,"rb")) == NULL)
  {
    // Add .pcx and try again
    char s[256];
    sprintf( s, "%s.pcx", sFileName );
    if((in=fopen(s,"rb")) == NULL)
    {
      sprintf( s, "PCX_read: file not found (%s)", sFileName );
      error( s );
    }
  }
  // Read the header
  if(fread((char *)&header,1,sizeof(PCXHEAD),in)!= sizeof(PCXHEAD))
  {
    error("PCX_read: problem in the header");
  }

  // Check the signature 
  if(header.manufacturer != 0x0a || header.version != 5)
  {
    error("PCX_read: PCX format not supported");
  }

  if(header.bits_per_pixel == 1)
  {
    bits=header.colour_planes;
  }
  else
  {
    bits=header.bits_per_pixel;
  }

  if(bits != 8)
  {
    error("PCX_read: not a 256 Colors PCX");
  }

  // Find the palette 
  if(!fseek(in,-769L,SEEK_END)) 
  {
    if(fgetc(in) != 0x0c || fread(palette,1,768,in) != 768)
    {
         error("PCX_read: problem with the colors");
    }
  } 
  else
  {
    error("PCX_read: colors not found");
  }

  fseek(in,128L,SEEK_SET);

  m_iSizeX = (header.xmax-header.xmin)+1;
  m_iSizeY = (header.ymax-header.ymin)+1;
  bytes = header.bytes_per_line;

  // Allocate a big buffer 
  m_Bitmap = new char[ m_iSizeX*m_iSizeY ];
  if( m_Bitmap==NULL )
  {
    error( "PCX_read: Out of memory" );
  }

  // Uncompress the file
  for( i=0; i<m_iSizeY; i++ ) 
  {
    PCX_ReadLine( m_Bitmap+i*m_iSizeX, bytes, in );
  }

  fclose( in );
}

/**
 * Read a line of pixels in a PCX file
 *
 * @param p             the memory where the pixels will be writed
 * @param nb            the number of pixels
 * @param in            the input file
 */
void TBitmap::PCX_ReadLine( char * p, int nb, FILE * in )
{
  int n=0,c,i;

  for( i=0; i<nb; i++ )
  {
    p[i]=0;
  }

  do
  {
    c=fgetc(in) & 0xff;
    if( (c & 0xc0) == 0xc0 )
    {
      i=c & 0x3f;
      c=fgetc(in);
      while(i--)
      {
        p[n++] = char(c);
      }
    }
    else 
    {
      p[n++] = char(c);
    }
  } while(n < nb);
}

//--------------------------------------------------------------------------
//
//    FILE: G_BITMAP.H (portable)
//
//    TBitmap
//      - The bitmaps are arrays of pixels that can be loaded from files
//        and used by TLowGraphic as data.
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

#ifndef _g_bitmap_H
#define _g_bitmap_H

//--------------------------------------------------------------------------
//                             C L A S S
//--------------------------------------------------------------------------

class TBitmap 
{
private:  
  void PCX_ReadLine( char * p, int nb, FILE * in );

public:
  long m_iSizeX, m_iSizeY;
  char * m_Bitmap;

   // constructor, destructor
   TBitmap();
   virtual ~TBitmap();
   // operations
   void PCX_Read( const char * sFileName );
};

#endif // _g_bitmap_H


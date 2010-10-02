
//--------------------------------------------------------------------------
//
//    FILE: G_LOWGRA.CPP (portable)
//
//    TLowGraphic
//      - This class contains all the function needed to draw a car race.
//        The current implementation uses a bitmap that is copied on the
//        screen after its update in the memory.
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define SWAP(a,b) { temp = (a); (a) = (b); (b) = temp; }


//__ TLowGraphic::TLowGraphic ______________________________________________
//
// Constructor
// In : int x, y : size of the graphic
//      bool bVirtual : A virtual LowGraphic is not associated with a control 
//__________________________________________________________________________

TLowGraphic::TLowGraphic( int x, int  y, bool bVirtual )
{
  m_SizeX = x;  m_SizeY = y;
  m_MaxX = x-1; m_MaxY = y-1;
  m_ScreenSizeX = m_SizeX;
  m_Bitmap = NULL;
  m_bVirtual = bVirtual;

  if( m_bVirtual )
  {
    m_Bitmap = new char[m_SizeX*m_SizeY];
  }
  else
  {
    MallocBitmap();
  }
  m_BitmapFont.PCX_Read( "bitmap/font8x8.pcx" );
}

//__ TLowGraphic::~TLowGraphic _____________________________________________
//
// Destructor
//__________________________________________________________________________

TLowGraphic::~TLowGraphic()
{
  FreeBitmap();
}

//__ TLowGraphic::Resize ___________________________________________________
//
// Resize the bitmap and the related parameters
// In : int x, y : the new size of the graphic (view)
//__________________________________________________________________________

void TLowGraphic::Resize( int x, int y )
{
  FreeBitmap();
  m_SizeX = x;  m_SizeY = y;
  m_MaxX = x-1; m_MaxY = y-1;
  m_ScreenSizeX = m_SizeX;
  MallocBitmap();
}

//__ TLowGraphic::ClearBitmap ______________________________________________
//
// Fill the bitmap with the color 'c'
// In : int c: the color to clear the bitmap
//__________________________________________________________________________

void TLowGraphic::ClearBitmap( int c )
{
   uint32 * p = (uint32 *) m_Bitmap;
   uint32 c2= c+(c<<8);
   c2= c2+(c2<<16);
   for( long i=(m_SizeX*m_SizeY/4)-1; i>=0; i-- )
      *p++=c2;
}

//__ TLowGraphic::ClearBitmapTexture _______________________________________
//
// Fill the bitmap with a pattern given by a bitmap
// In : double x, y  : the texture coordonates for the pixel (0,0)
//      double d     : the step to use in the texture coordonates
//      TBitmap * t : the texture
//__________________________________________________________________________

void TLowGraphic::ClearBitmapTexture( double x, double y, double d, TBitmap * t )
{
   char * p = m_Bitmap;
   char * pt;
   int ix, ix_start, iy, id, i, j;

   d *= 4096.0;
   ix_start = (int)(x*d)&0x7fff;
   iy = (int)(y*d)&0x7fff;
   id = (int)(d)&0x7fff;
   for( i=m_SizeY; i>0; i-- ) 
   {
      ix = ix_start;
      pt = t->m_Bitmap+((iy&0x7000)>>9);
      for( j=m_SizeX; j>0; j-- ) 
      {
         *p++=*(pt+(ix>>12));
         ix+=id;
         ix&=0x7fff;
      }
      iy+=id;
      iy&=0x7fff;
      p += m_ScreenSizeX-m_SizeX;
   }
}

//__ TLowGraphic::ClipLine _________________________________________________
//
// Clip a line
// In : int x1, y1 : the first point
//      int x2, y2 : the second point of the line
// Out: boolean : FALSE == line out of the screen
//      x1, y1, x2, y2 : adjusted line to fit in the graphic
//__________________________________________________________________________

boolean TLowGraphic::ClipLine( int &x1, int &y1, int &x2, int &y2 )
{
   int temp;

   if( x1>x2 ) {
      SWAP(x1,x2);
      SWAP(y1,y2);
   }
   if( x2<0 || x1>m_MaxX ) return FALSE;
   if(y1 < y2) {
       if( y2<0 || y1>m_MaxY ) return FALSE;
       if( y1<0 ) {
       x1 += (long)(x2-x1)*(0-y1)/(y2 - y1);
       if( x1>m_MaxX ) return FALSE;
       y1 = 0;
       }
       if( y2>m_MaxY ) {
       x2 -= (long)(x2 - x1)*(y2 - m_MaxY)/(y2 - y1);
       if( x2<0 ) return FALSE;
       y2 = m_MaxY;
       }
   } else {
       if((y1 < 0) || (y2 > m_MaxY)) return FALSE;
       if(y1 > m_MaxY) {
       x1 += (long)(x2 - x1)*(y1 - m_MaxY)/(y1 - y2);
       if( x1>m_MaxX ) return FALSE;
       y1 = m_MaxY;
       }
       if(y2 < 0) {
       x2 -= (long)(x2 - x1)*(0 - y2)/(y1 - y2);
       if( x2<0 ) return FALSE;
       y2 = 0;
       }
   }
   if( x1<0 ) {
       y1 += (long)(y2 - y1)*(0 - x1)/(x2 - x1);
       x1 = 0;
   }
   if( x2>m_MaxX ) {
       y2 -= (long)(y2 - y1)*(x2 - m_MaxX)/(x2 - x1);
       x2 = m_MaxX;
   }
   return TRUE;
}

//__ TLowGraphic::DrawLine _________________________________________________
//
// Draw a line
// In : Int2D p1 : the first point
//      Int2D p2 : the second point of the line
//      int c    : the color of the line
//__________________________________________________________________________

void TLowGraphic::DrawLine( int p1_x, int p1_y, int p2_x, int p2_y, int c)
{
  Int2D p1, p2;

  p1.x = p1_x;
  p1.y = p1_y;
  p2.x = p2_x;
  p2.y = p2_y;
  DrawLine( p1, p2, c );
}

void TLowGraphic::DrawLine(Int2D p1, Int2D p2, int c)
{
   int32 i;

   int32 off_cpt, off_inc;
   char * p;

   // special case ( point, horizontal line )
   if( p1.y == p2.y ) 
   {
      DrawHorzLine( p1.x, p2.x, p1.y, c );
      return;
   }
   if( ClipLine( p1.x, p1.y, p2.x, p2.y ) ) 
   {
     // special case ( point, horizontal line )
     if( p1.y == p2.y ) 
     {
        DrawHorzLine( p1.x, p2.x, p1.y, c );
        return;
     }
     // line in the screen
      off_cpt = 0;
      p = m_Bitmap + p1.x + p1.y*m_ScreenSizeX;
      if( p2.y>p1.y ) 
      {
         if( p2.x-p1.x>p2.y-p1.y ) 
         {
            off_inc = ( (int32)(p2.y-p1.y)<<10 )/(p2.x-p1.x);
            for( i=p2.x-p1.x; i>=0; i-- ) 
            {
               *p = (char)c;
               p += 1;
               off_cpt += off_inc;
               if( off_cpt>=0x400 ) 
               {
                  p += m_ScreenSizeX;
                  off_cpt &= 0x3FF;
               }
            }
         } 
         else 
         {
            off_inc = ( (int32)(p2.x-p1.x)<<10 )/(p2.y-p1.y);
            for( i=p2.y-p1.y; i>=0; i-- ) 
            {
               *p = (char)c;
               p += m_ScreenSizeX;

               off_cpt += off_inc;
               if( off_cpt>=0x400 ) 
               {
                  p += 1;
                  off_cpt &= 0x3FF;
               }
            }
         }
      } 
      else 
      {
         if( p2.x-p1.x>p1.y-p2.y ) 
         {
            off_inc = ( (int32)(p1.y-p2.y)<<10 )/(p2.x-p1.x);
            for( i=p2.x-p1.x; i>=0; i-- ) 

            {
               *p = (char)c;
               p += 1;
               off_cpt += off_inc;
               if( off_cpt>=0x400 ) 
               {
                  p -= m_ScreenSizeX;
                  off_cpt &= 0x3FF;
               }
            }
         }
         else 
         {
            off_inc = ( (int32)(p2.x-p1.x)<<10 )/(p1.y-p2.y);
            for( i=p1.y-p2.y; i>=0; i-- ) 
            {
               *p = (char)c;
               p -= m_ScreenSizeX;
               off_cpt += off_inc;
               if( off_cpt>=0x400 ) 
               {
                  p += 1;							// bug corrected here. JCer
                  off_cpt &= 0x3FF;
               }
            }
         }
      }
   }
}

//__ TLowGraphic::DrawHorzLine _____________________________________________
//
// Draw a horizontal line
// In : int x1, x2, y : the coordonates of the line
//      int c         : the color of the line
//__________________________________________________________________________

void TLowGraphic::DrawHorzLine( int x1, int x2, int y, int c )
{
   int i, temp;
   char * p;

   if( x1>x2 ) SWAP(x1,x2);
   if( x1>m_MaxX || x2<0 ) return;
   if( x1<0 ) x1=0;
   if( x2>m_MaxX ) x2=m_MaxX;
   if( y>m_MaxY  ) return;
   if( y<0 ) return;
   p = m_Bitmap+y*m_ScreenSizeX+x1;
   for( i=x2-x1; i>=0; i-- ) *p++ = (char)c;
}

//__ TLowGraphic::DrawVertLine _____________________________________________
//
// Draw a vertical line
// In : int x, y1, y2 : the coordonates of the line
//      int c         : the color of the line
//__________________________________________________________________________

void TLowGraphic::DrawVertLine( int x, int y1, int y2, int c )
{
   int i, temp;
   char * p;

   if( x>m_MaxX  ) return;
   if( x<0 ) return;
   if( y1>y2 ) SWAP(y1,y2);
   if( y1>m_MaxY || y2<0 ) return;
   if( y1<0 ) y1=0;
   if( y2>m_MaxY ) y2=m_MaxY;
   p = m_Bitmap+y1*m_ScreenSizeX+x;
   for( i=y2-y1; i>=0; i-- ) {
      *p = (char)c;
      p+= m_ScreenSizeX;
   }
}

//__ TLowGraphic::DrawPoint _____________________________________________
//
// Draw a point
// In : int x, y      : the coordonates of the point
//      int c         : the color of the point
//__________________________________________________________________________

void TLowGraphic::DrawPoint( int x, int y, int c )
{
   char * p;

   if( x>m_MaxX  ) return;
   if( x<0 ) return;
   if( y>m_MaxY  ) return;
   if( y<0 ) return;
   p = m_Bitmap+y*m_ScreenSizeX+x;
   *p = (char)c;
}

//__ TLowGraphic::DrawPoly _________________________________________________
//
// Draw a polygon
// In : Int2D v[] : the vertexes of the polygon
//      int v_nb  : the number of vertexes
//      int c     : the color of the line
//__________________________________________________________________________

void TLowGraphic::DrawPoly(Int2D v[], int v_nb, int c)
{
   #define mod(x,y)   (((x)<(y))?(x):(x-y))
   #define rgt_add(a,b)                                             \
     temp= a; temp2= b;                                             \
     if( temp2>y_prv ) {                                            \
    x_prv = rgt[rgt_nb].x = temp;                               \
    y_prv = rgt[rgt_nb++].y = temp2;                            \
     } else if( temp>x_prv ) {                                      \
    x_prv = rgt[rgt_nb-1].x = temp;                             \
    y_prv = rgt[rgt_nb-1].y = temp2;                            \
     }
   #define lft_add(a,b)                                             \
     temp= a; temp2= b;                                             \
     if( temp2>y_prv) {                                             \
    x_prv = lft[lft_nb].x = temp;                               \
    y_prv = lft[lft_nb++].y = temp2;                            \
     } else if( temp<x_prv ) {                                      \
    x_prv = lft[lft_nb-1].x = temp;                             \
    y_prv = lft[lft_nb-1].y = temp2;                            \
     }


   #define mod(x,y)   (((x)<(y))?(x):(x-y))

   int i, nxt, temp, temp2;
   long x, y, x_prv, y_prv;
   int ymin_pos, ymin_val, xmin_val;
   int ymax_pos, ymax_val, xmax_val;

   Int2D lft[10];
   Int2D rgt[10];
   int rgt_nb, rgt_cpt, xr, xr2, yr, xr_inc;
   int lft_nb, lft_cpt, xl, xl2, yl, xl_inc;

   char * p;

   rgt_nb=1; lft_nb=1;
   xmin_val=v[0].x;
   xmax_val=v[0].x;
   ymin_pos=0; ymin_val=v[0].y;
   ymax_pos=0; ymax_val=v[0].y;
   for( i=1;i<v_nb;i++ ) {
      if( ymin_val>=v[i].y ) {
         if( ymin_val>v[i].y || v[i].x<v[ymin_pos].x  ) {
        ymin_pos=i;
        ymin_val=v[i].y;
         }
      }
      if( ymax_val<=v[i].y ) {
         if( ymax_val<v[i].y || v[i].x>v[ymax_pos].x  ) {
        ymax_pos=i;
        ymax_val=v[i].y;
         }
      }
      if( xmin_val>v[i].x ) xmin_val=v[i].x;
      if( xmax_val<v[i].x ) xmax_val=v[i].x;
   }
   if( xmax_val<0 ) return;
   if( xmin_val>=m_SizeX ) return;
   if( ymax_val<0 ) return;
   if( ymin_val>=m_SizeY ) return;
   if( ymin_val==ymax_val ) {
      DrawHorzLine( xmin_val, xmax_val, ymin_val, c );
      return;
   }

   y_prv = -10000; x_prv=-10000;
   i=ymin_pos;
   while( i!=ymax_pos ) {
      nxt=mod(i+1,v_nb);
      y = v[i].y;
      if( y>=-10000 ) {
         x= v[i].x;
         // x clipping
         if( x<0 ) {
        if( v[nxt].x>=0) {
           rgt_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           if( v[nxt].x>m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        }
         } else {
        if( x>m_MaxX ) {
           if( v[nxt].x<=m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        } else {
           rgt_add( x, y );

           if( v[nxt].x>m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        }
        if( v[nxt].x<0) {
           rgt_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
            }
         }
      }
      i= nxt;
   }
   x=v[ymax_pos].x; y=v[ymax_pos].y;
   if( x>=0 && x<=m_MaxX ) {
      rgt_add( x, y );
   }
   while( rgt[1].y<0 ) {
      if( rgt_nb>2 ) {
         rgt[1].x= rgt[1].x-(long)(rgt[2].x-rgt[1].x)*rgt[1].y/(rgt[2].y-rgt[1].y);
         rgt[1].y= 0;
         if( rgt[2].y<=0 ) {
            rgt_nb--;
            for( i=1; i<rgt_nb; i++ ) rgt[i]=rgt[i+1];
         }
      } else {
         rgt[1].y= 0;
      }
   }

   y_prv = -10000; x_prv=10000;
   i=ymin_pos;
   while( i!=ymax_pos ) {
      nxt=mod(i+v_nb-1,v_nb);
      y = v[i].y;
      if( y>=0 || v[nxt].y>=0 ) {
         x= v[i].x;
     // x clipping
     if( x>m_MaxX ) {
        if( v[nxt].x<=m_MaxX) {
           lft_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
           if( v[nxt].x<0)
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
        }
     } else {
        if( x<0 ) {
           if( v[nxt].x>=0 ) {
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           }
        } else {
           lft_add( x, y );

           if( v[nxt].x<0 ) {
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           }
        }
        if( v[nxt].x>m_MaxX ) {
           lft_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
        }
     }
      }
      i= nxt;
   }
   x=v[ymax_pos].x; y=v[ymax_pos].y;
   if( x>=0 && x<=m_MaxX ) {
      lft_add( x, y );
   }
   while( lft[1].y<0 ) {
      if( lft_nb>2 ) {
         lft[1].x= lft[1].x-(long)(lft[2].x-lft[1].x)*lft[1].y/(lft[2].y-lft[1].y);
         lft[1].y= 0;
         if( lft[2].y<=0 ) {
            lft_nb--;
            for( i=1; i<lft_nb; i++ ) lft[i]=lft[i+1];
         }
      } else {
         lft[1].y= 0;
      }
   }


   if( rgt_nb>=2 && (lft_nb<2 || lft[lft_nb-1].y<rgt[rgt_nb-1].y) ) {
      lft[lft_nb].x= 0;
      lft[lft_nb++].y= rgt[rgt_nb-1].y;
   }
   if( lft_nb>=2 && (rgt_nb<2 || lft[lft_nb-1].y>rgt[rgt_nb-1].y) ) {
      rgt[rgt_nb].x= m_MaxX;
      rgt[rgt_nb++].y= lft[lft_nb-1].y;
   }

   if( rgt_nb<=2 && lft_nb<=2 ) return;   // normal DrawHorzLine

   rgt_cpt=2;
   lft_cpt=2;

   if( rgt_nb==1 ) {
      rgt[0].x=m_MaxX;
      rgt[0].y=lft[1].y;
      rgt[1].x=m_MaxX;
      rgt[1].y=lft[lft_nb-1].y;
      rgt_nb++;
      rgt_cpt--;
   } else if( lft_nb==1 ) {
      lft[0].x=0;
      lft[0].y=rgt[1].y;
      lft[1].x=0;
      lft[1].y=rgt[rgt_nb-1].y;
      lft_nb++;
      lft_cpt--;
   } else if( lft[1].y<rgt[1].y ) {
      rgt[0].x=m_MaxX;
      rgt[0].y=lft[1].y;
      rgt_cpt--;
   } else if( lft[1].y>rgt[1].y ) {
      lft[0].x=0;
      lft[0].y=rgt[1].y;
      lft_cpt--;
   }

#define calc_xl2_yl                                               \
   xl2 = lft[lft_cpt-1].x<<6;                                     \
   yl = lft[lft_cpt].y;                                           \
   xl_inc = ((lft[lft_cpt].x<<6)-xl2)/(yl-lft[lft_cpt-1].y);
#define calc_xr2_yr                                               \
   xr2 = rgt[rgt_cpt-1].x<<6;                                     \
   yr = rgt[rgt_cpt].y;                                           \
   xr_inc = ((rgt[rgt_cpt].x<<6)-xr2)/(yr-rgt[rgt_cpt-1].y);

   y = lft[lft_cpt-1].y;
   p = m_Bitmap+y*m_ScreenSizeX;
   calc_xl2_yl;
   calc_xr2_yr;
   while( y<m_SizeY && lft_cpt!=lft_nb ) {
      if( yr<yl ) {
     rgt_cpt++;
     do {
        xl=xl2>>6; xr=xr2>>6;
        p += xl;
        for(i=xr-xl;i>=0; i--) *p++=(char)c;
        p += m_ScreenSizeX-xr-1;
        xl2 += xl_inc;
        xr2 += xr_inc;
     } while( yr>++y && y<m_SizeY );
     calc_xr2_yr;
      } else {
     lft_cpt++;
     if( lft_cpt==lft_nb ) yl++;
     do {
        xl=xl2>>6; xr=xr2>>6;
        p += xl;
        for( i=xr-xl; i>=0; i-- ) *p++=(char)c;
        p += m_ScreenSizeX-xr-1;
        xl2 += xl_inc;
        xr2 += xr_inc;
     } while( yl>++y && y<m_SizeY );
     if( yr==yl ) {
        rgt_cpt++;
        calc_xr2_yr;
     }
     if( lft_cpt!=lft_nb ) { calc_xl2_yl };
      }
   }
}

//__ TLowGraphic::DrawPolyTexture __________________________________________
//
// Draw a polygon with a rotated texture. v[0] have the coordonates 0,0 in
// the texture.
// In : Int2D v[]          : the vertexes of the polygon
//      int v_nb           : the number of vertexes
//      double sine, cosine : the steps to use in the texture coordonates
//      TBitmap * t        : the texture
//__________________________________________________________________________

void TLowGraphic::DrawPolyTexture( Int2D v[], int v_nb, double sine, double cosine, TBitmap * t )
{
   #define rgt_add(a,b)                                             \
     temp= a; temp2= b;                                             \
     if( temp2>y_prv ) {                                            \
    x_prv = rgt[rgt_nb].x = temp;                               \
    y_prv = rgt[rgt_nb++].y = temp2;                            \
     } else if( temp>x_prv ) {                                      \
    x_prv = rgt[rgt_nb-1].x = temp;                             \
    y_prv = rgt[rgt_nb-1].y = temp2;                            \
     }
   #define lft_add(a,b)                                             \
     temp= a; temp2= b;                                             \
     if( temp2>y_prv) {                                             \
    x_prv = lft[lft_nb].x = temp;                               \
    y_prv = lft[lft_nb++].y = temp2;                            \
     } else if( temp<x_prv ) {                                      \
    x_prv = lft[lft_nb-1].x = temp;                             \
    y_prv = lft[lft_nb-1].y = temp2;                            \
     }

   int i, nxt, temp, temp2;
   long x, y, x_prv, y_prv;
   int ymin_pos, ymin_val, xmin_val;
   int ymax_pos, ymax_val, xmax_val;

   Int2D lft[10];
   Int2D rgt[10];
   int rgt_nb, rgt_cpt, xr, xr2, yr, xr_inc;
   int lft_nb, lft_cpt, xl, xl2, yl, xl_inc;

   char * p;

   rgt_nb=1; lft_nb=1;
   xmin_val=v[0].x;
   xmax_val=v[0].x;
   ymin_pos=0; ymin_val=v[0].y;
   ymax_pos=0; ymax_val=v[0].y;
   for( i=1;i<v_nb;i++ ) {
      if( ymin_val>=v[i].y ) {
         if( ymin_val>v[i].y || v[i].x<v[ymin_pos].x  ) {
        ymin_pos=i;
        ymin_val=v[i].y;
         }
      }
      if( ymax_val<=v[i].y ) {
         if( ymax_val<v[i].y || v[i].x>v[ymax_pos].x  ) {
        ymax_pos=i;
        ymax_val=v[i].y;
         }
      }
      if( xmin_val>v[i].x ) xmin_val=v[i].x;
      if( xmax_val<v[i].x ) xmax_val=v[i].x;
   }
   if( xmax_val<0 ) return;
   if( xmin_val>=m_SizeX ) return;
   if( ymax_val<0 ) return;
   if( ymin_val>=m_SizeY ) return;
   if( ymin_val==ymax_val ) {
      DrawHorzLine( xmin_val, xmax_val, ymin_val, 1 );
      return;
   }

   y_prv = -10000; x_prv=-10000;
   i=ymin_pos;
   while( i!=ymax_pos ) {
      nxt=mod(i+1,v_nb);
      y = v[i].y;
      if( y>=-10000 ) {
         x= v[i].x;
         // x clipping
         if( x<0 ) {
        if( v[nxt].x>=0) {
           rgt_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           if( v[nxt].x>m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        }
         } else {
        if( x>m_MaxX ) {
           if( v[nxt].x<=m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        } else {
           rgt_add( x, y );

           if( v[nxt].x>m_MaxX) {
              rgt_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
               }
        }
        if( v[nxt].x<0) {
           rgt_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
            }
         }
      }
      i= nxt;
   }
   x=v[ymax_pos].x; y=v[ymax_pos].y;
   if( x>=0 && x<=m_MaxX ) {
      rgt_add( x, y );
   }
   while( rgt[1].y<0 ) {
      if( rgt_nb>2 ) {
         rgt[1].x= rgt[1].x-(long)(rgt[2].x-rgt[1].x)*rgt[1].y/(rgt[2].y-rgt[1].y);
         rgt[1].y= 0;
         if( rgt[2].y<=0 ) {
            rgt_nb--;
            for( i=1; i<rgt_nb; i++ ) rgt[i]=rgt[i+1];
         }
      } else {
         rgt[1].y= 0;
      }
   }

   y_prv = -10000; x_prv=10000;
   i=ymin_pos;
   while( i!=ymax_pos ) {
      nxt=mod(i+v_nb-1,v_nb);
      y = v[i].y;
      if( y>=0 || v[nxt].y>=0 ) {
         x= v[i].x;
     // x clipping
     if( x>m_MaxX ) {
        if( v[nxt].x<=m_MaxX) {
           lft_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
           if( v[nxt].x<0)
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
        }
     } else {
        if( x<0 ) {
           if( v[nxt].x>=0 ) {
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           }
        } else {
           lft_add( x, y );

           if( v[nxt].x<0 ) {
          lft_add( 0, y-x*(v[nxt].y-y)/(v[nxt].x-x) );
           }
        }
        if( v[nxt].x>m_MaxX ) {
           lft_add( m_MaxX, y-(x-m_MaxX)*(v[nxt].y-y)/(v[nxt].x-x) );
        }
     }
      }
      i= nxt;
   }
   x=v[ymax_pos].x; y=v[ymax_pos].y;
   if( x>=0 && x<=m_MaxX ) {
      lft_add( x, y );
   }
   while( lft[1].y<0 ) {
      if( lft_nb>2 ) {
         lft[1].x= lft[1].x-(long)(lft[2].x-lft[1].x)*lft[1].y/(lft[2].y-lft[1].y);
         lft[1].y= 0;
         if( lft[2].y<=0 ) {
            lft_nb--;
            for( i=1; i<lft_nb; i++ ) lft[i]=lft[i+1];
         }
      } else {
         lft[1].y= 0;
      }
   }


   if( rgt_nb>=2 && (lft_nb<2 || lft[lft_nb-1].y<rgt[rgt_nb-1].y) ) {
      lft[lft_nb].x= 0;
      lft[lft_nb++].y= rgt[rgt_nb-1].y;
   }
   if( lft_nb>=2 && (rgt_nb<2 || lft[lft_nb-1].y>rgt[rgt_nb-1].y) ) {
      rgt[rgt_nb].x= m_MaxX;
      rgt[rgt_nb++].y= lft[lft_nb-1].y;
   }

   if( rgt_nb<=2 && lft_nb<=2 ) return;   // normal DrawHorzLine

   rgt_cpt=2;
   lft_cpt=2;

   if( rgt_nb==1 ) {
      rgt[0].x=m_MaxX;
      rgt[0].y=lft[1].y;
      rgt[1].x=m_MaxX;
      rgt[1].y=lft[lft_nb-1].y;
      rgt_nb++;
      rgt_cpt--;
   } else if( lft_nb==1 ) {
      lft[0].x=0;
      lft[0].y=rgt[1].y;
      lft[1].x=0;
      lft[1].y=rgt[rgt_nb-1].y;
      lft_nb++;
      lft_cpt--;
   } else if( lft[1].y<rgt[1].y ) {
      rgt[0].x=m_MaxX;
      rgt[0].y=lft[1].y;
      rgt_cpt--;
   } else if( lft[1].y>rgt[1].y ) {
      lft[0].x=0;
      lft[0].y=rgt[1].y;
      lft_cpt--;
   }

#define calc_xl2_yl                                               \
   xl2 = lft[lft_cpt-1].x<<6;                                     \
   yl = lft[lft_cpt].y;                                           \
   xl_inc = ((lft[lft_cpt].x<<6)-xl2)/(yl-lft[lft_cpt-1].y);
#define calc_xr2_yr                                               \
   xr2 = rgt[rgt_cpt-1].x<<6;                                     \
   yr = rgt[rgt_cpt].y;                                           \
   xr_inc = ((rgt[rgt_cpt].x<<6)-xr2)/(yr-rgt[rgt_cpt-1].y);

   int16 aux1, aux2, t_x0, t_y0, t_x, t_y;
   int16 t_sin, t_cos;
   char * t_p;

   y = lft[lft_cpt-1].y;
   p = m_Bitmap+y*m_ScreenSizeX;

   t_sin = (int16)( sine*1024.0);
   t_cos = (int16)( cosine*1024.0);
   aux2  = (int16)( lft[lft_cpt-1].y-v[0].y );
   t_x0  = (int16)( 1024-aux2*t_sin );
   t_y0  = (int16)( 1024+aux2*t_cos );
   t_p   = t->m_Bitmap;

   calc_xl2_yl;
   calc_xr2_yr;
   while( y<m_SizeY && lft_cpt!=lft_nb ) {
      if( yr<yl ) {
     rgt_cpt++;
     do {
        xl=xl2>>6; xr=xr2>>6;
        p += xl;
            aux1 = (int16)( xl-v[0].x );
            t_x = (int16)( aux1*t_cos + t_x0 );
            t_y = (int16)( aux1*t_sin + t_y0 );
        for(i=xr-xl;i>=0; i--) 
        {
            *p++= t_p[(t_x>>10)+((t_y&0x7c00)>>6)];
            t_x = (int16)( t_x + t_cos );

            t_y = (int16)( t_y + t_sin );
        }
        p += m_ScreenSizeX-xr-1;
        xl2 += xl_inc;
        xr2 += xr_inc;
        t_x0 = (int16)( t_x0-t_sin );
        t_y0 = (int16)( t_y0+t_cos );
     } 
     while( yr>++y && y<m_SizeY );
     calc_xr2_yr;
   } else {
     lft_cpt++;
     if( lft_cpt==lft_nb ) yl++;
     do {
        xl=xl2>>6; xr=xr2>>6;
        p += xl;
        aux1 = (int16)( xl-v[0].x );
        t_x = (int16)( aux1*t_cos + t_x0 );
        t_y = (int16)( aux1*t_sin + t_y0 );
        for(i=xr-xl;i>=0; i--) 
        {
           *p++= t_p[(t_x>>10)+((t_y&0x7c00)>>6)];
           t_x = (int16)( t_x+t_cos );
           t_y = (int16)( t_y+t_sin );
        }
        p += m_ScreenSizeX-xr-1;
        xl2 += xl_inc;
        xr2 += xr_inc;
        t_x0 = (int16)( t_x0-t_sin );
        t_y0 = (int16)( t_y0+t_cos );
     } while( yl>++y && y<m_SizeY );
     if( yr==yl ) {
        rgt_cpt++;
        calc_xr2_yr;
     }
     if( lft_cpt!=lft_nb ) { calc_xl2_yl };
      }
   }
}

//__ TLowGraphic::DrawString ________________________________________________
//
// Write a text (the background is transparent)
// In : int x, y  : the place to write the text
//      char * t  : the text
//__________________________________________________________________________

void TLowGraphic::DrawString( const char * text, int x, int y, int foreground )
{
  int i, j;
  const char * t = text;
  char * p_font, * p_screen;
  char * p_screen_start = m_Bitmap + y*m_ScreenSizeX;

  while( *t!='\0' ) 
  {
    if( x >= 0 && x + 8 <= m_ScreenSizeX )
    {
      p_screen = p_screen_start + x;
      p_font = ( (*t-32)<<3 )+m_BitmapFont.m_Bitmap;
      for( i=0; i<8; i++ ) {
        for( j=0; j<8; j++ ) {
          if( p_font[j]==0 )
            p_screen[j]=foreground;
        }
        p_font+= m_BitmapFont.m_iSizeX;
        p_screen+= m_ScreenSizeX;
      }
    }
    x = x + 8;
    t++;
  }
}

//__ TLowGraphic::DrawStringOpaque _________________________________________
//
// Write a text (the background is not transparent)
// In : int x, y  : the place to write the text
//      char * t  : the text
//__________________________________________________________________________

void TLowGraphic::DrawStringOpaque( const char * text, int x, int y, int foreground, int background )
{
   int i, j;
   const char * t = text;
   char * p_font, * p_screen;
   char * p_screen_start = m_Bitmap + y*m_ScreenSizeX;

   while( *t!='\0' ) {
    if( x >= 0 && x + 8 <= m_ScreenSizeX )
    {
      p_screen = p_screen_start + x;
      p_font = ( (*t-32)<<3 )+m_BitmapFont.m_Bitmap;
      for( i=0; i<8; i++ ) {
       for( j=0; j<8; j++ ) {
         if( p_font[j]==0 )
         p_screen[j]=foreground;
         else
         p_screen[j]=background;
       }
       p_font+= m_BitmapFont.m_iSizeX;
       p_screen+= m_ScreenSizeX;
      }
    }
      x = x + 8;
      t++;
   }
}

/**
 * Flip the bitmap round the Y axis
 */
void TLowGraphic::FlipY()
{
  unsigned char * p1, *p2, tmp;

  for( int y=0; y<m_SizeY/2; y++ )
  {
    p1 = (unsigned char *) m_Bitmap + y*m_SizeX;
    p2 = (unsigned char *) m_Bitmap+ (m_SizeY-y-1)*m_SizeX;

    for( int x=0; x<m_SizeX; x++ )
    {
      tmp = *p1;
      *p1++ = *p2;
      *p2++ = *p1;
    }
  }
}


/**
 * Transform the bitmap to a bitmap with one bit per pixel
 *    
 * @param color: the color used for the bit=0 (else bit=1)
 */
unsigned char * TLowGraphic::GetOneBitPerPixel( int color )
{
  int sx = (m_SizeX+7)/8;
  int sy = m_SizeY;

  unsigned char * bit = new unsigned char[sx*sy]; 
  unsigned char * pbit = bit;
  
  for( int y=0; y<m_SizeY; y++ ) 
  {
    unsigned char byte=0;
    unsigned char pos=128;
    for( int x=0; x<m_SizeX; x++ ) 
    {
      char * p = m_Bitmap+y*m_SizeX+x;
      if( *p!=(char)color )
      {
        byte += pos;
      }
      pos /= 2;

      if( pos==0 )
      {
        *(pbit++) = byte;
        byte = 0;
        pos = 128;
      }
    }
    if( pos!=128 ) // Fetch the end of the line
    {
      *(pbit++) = byte;
    }
  }

  return bit;
}



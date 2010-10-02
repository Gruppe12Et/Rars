
//--------------------------------------------------------------------------
//
//    FILE: G_PALETT.CPP (portable)
//
//    TPalette
//      - This class manages the colors
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

//__ TPalette::Init ________________________________________________________
//
// Init the palette. (The colors are loaded from a file.)
//__________________________________________________________________________

void TPalette::Init()
{
   Load( "bitmap/color.pl0" );
   Set();
}

//__ TPalette::Load ________________________________________________________
//
// Load the colors in a file
// In : char * filename : the name of the file
//__________________________________________________________________________

void TPalette::Load( const char * file_name )
{
   FILE     * f;
   char     * s, * s2;
   int      i, cpt;

   if ((f = fopen( file_name ,"r"))==NULL) {
      error( "TPalette::Load: file not found\n" );
   }

   s = new char[200];
   if( s==NULL ) error("TPalette::Load: out of memory");

   // read: << COLOR: 249 >>
   fgetw( s, f );
   fscanf( f, "%d", &m_ColorNb );

   // read: << R  G  B     NB  NOM >>
   s2 = fgets( s, 200, f);
   s2 = fgets( s, 200, f);
   s2 = s2;

   for( i=0; i<=m_ColorNb; i++ ) 
   {
      // read: << 25 25 35    0   name >>
      fscanf( f, "%d %d %d %d",
	      &m_Color[i].r,
	      &m_Color[i].g,
	      &m_Color[i].b,
	      &cpt
	    );
      fgetw( s, f );
       
      m_Color[i].fr = float(m_Color[i].r)/63.0f; 
      m_Color[i].fg = float(m_Color[i].g)/63.0f; 
      m_Color[i].fb = float(m_Color[i].b)/63.0f; 
      if (cpt!=i) error("TPalette::Load: probleme de numerotation");
   }
   free( s );
   fclose( f );
}


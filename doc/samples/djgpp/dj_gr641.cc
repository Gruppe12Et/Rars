//--------------------------------------------------------------------------
//
//    FILE: DJ_LOWGR.CPP (DJGPP + LibGrx)
//          (not portable)
//
//    This file gathers together all the functions of the classes that
//    are not portable:
//       - TPalette
//       - TViewManager
//       - TLowGraphic
//
//    Version       Author          Date
//      0.1      Marc Gueury     05 /08 /96
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <dos.h>
#include <conio.h>
#include <grx.h>
#include <malloc.h>
#include "g_global.h"

//--------------------------------------------------------------------------
//                    G L O B A L   V A R I A B L E S
//--------------------------------------------------------------------------

 uchar      * GM;
 uint16     * GM2;

//**************************************************************************
//                              M A I N
//**************************************************************************

void main(int argc, char* argv[])
{
   OldMain( argc, argv );
}

//**************************************************************************
//                             C L A S S
//                          T P A L E T T E
//**************************************************************************

//__ TPalette::Set _________________________________________________________
//
// Set the palette.
//__________________________________________________________________________

void TPalette::Set()
{
   // set the 16 defined colors
   for( int i=0; i<color_nb; i++ ) {
      GrSetColor(i, 3*color[i].r, 3*color[i].g, 3*color[i].b);
   }

   // fill the other ones with black
   for( i=color_nb; i<256; i++ ) {
      GrSetColor(i, 3, 6, 9);
   }
}

//**************************************************************************
//                             C L A S S
//                            TLowGraphic
//**************************************************************************

//__ TLowGraphic::MallocBitmap _____________________________________________
//
// Memory : Allocate some memory for the bitmap
// Direct Access: indicate the adress of the screen (see: screen_size_x)
//__________________________________________________________________________

void TLowGraphic::MallocBitmap()
{
   bitmap = (char *) GM;
   screen_size_x = 1024;
}

//__ TLowGraphic::FreeBitmap _______________________________________________
//
// Memory : Free the memory of the bitmap
// Direct Access: -
//__________________________________________________________________________

void TLowGraphic::FreeBitmap()
{
}

//**************************************************************************
//                             C L A S S
//                            TViewManager
//**************************************************************************

//_ TViewManager::InitAllViews _____________________________________________
//
// Initialize all the views
//__________________________________________________________________________

void TViewManager::InitAllViews()
{
   AddView( new TViewClassical(640,480) );
   views[0]->option_show_board = 1;
   SetFastModeParam( 8 );
}

//_ TViewManager::InitScreen _______________________________________________
//
// Initialize the screen
//__________________________________________________________________________

void TViewManager::InitScreen()
{
   GrSetMode(GR_width_height_color_graphics,640,480,256);
   GM = (uchar *) 0xd0000000;
   GM2 = (uint16 *) 0xd0000000;
}

//_ TViewManager::CloseScreen ______________________________________________
//
// Set the normal screen mode
//__________________________________________________________________________

void TViewManager::CloseScreen()
{
   GrSetMode(GR_default_text);
}

//_ TViewManager::CopyViewToScreen _________________________________________
//
// Copy the view 'view_nr' to the screen
// In : int view_nr : the view to copy
//__________________________________________________________________________

void TViewManager::CopyViewToScreen( int view_nr )
{
/*
   long i, j, size_x, size_y, x, y;

   x = 0; y = 0;
   if( view_nr==1 ) { x=320; y=0; }
   if( view_nr==2 ) { x=0; y=160; }
   GrContext c1;
   size_x = views[view_nr]->size_x;
   size_y = views[view_nr]->size_y;
   GrCreateContext( size_x,
                    size_y,
                    views[view_nr]->bitmap,
                    &c1
   );
   GrBitBlt(NULL,x,y, &c1,0,0,size_x-1, size_y-1, GrWRITE );
*/
}

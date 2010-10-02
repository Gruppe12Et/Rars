//--------------------------------------------------------------------------
//
//    FILE: BC_LOWGR.CPP (BORLAND C++ 3.1)
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
#include <malloc.h>
#include "g_global.h"

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
      outportb( 0x3C8, i );
      outportb( 0x3C9, color[i].r );
      outportb( 0x3C9, color[i].g );
      outportb( 0x3C9, color[i].b );
   }

   // fill the other ones with black
   for( i=color_nb; i<256; i++ ) {
      outportb( 0x3C8, i );
      outportb( 0x3C9, 1 );
      outportb( 0x3C9, 2 );
      outportb( 0x3C9, 3 );
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
   bitmap = (char *) MK_FP( 0xA000, 0x0000 );
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

//_ init_mode ______________________________________________________________
//
// Initialize a graphic mode
// In : int val = chosen mode
//__________________________________________________________________________

void init_mode( int val )
{
   struct REGPACK reg;

  reg.r_ax = val;
  outpw(0x3d4, 0x4a0b);
  intr( 0x10, &reg );
}

//_ TViewManager::InitAllViews _____________________________________________
//
// Initialize all the views
//__________________________________________________________________________

void TWinManager::InitAllViews()
{
   AddView( new TViewClassical(320,200) );
   views[0]->option_show_board=1;
   SetFastModeParam( 8 );
}

//_ TViewManager::InitScreen _______________________________________________
//
// Initialize the screen
//__________________________________________________________________________

void TWinManager::InitScreen()
{
   init_mode( 0x13 );
}

//_ TViewManager::CloseScreen ______________________________________________
//
// Set the normal screen mode
//__________________________________________________________________________

void TWinManager::CloseScreen()
{
   init_mode( 3 );
   clrscr();
}

//_ TViewManager::CopyViewToScreen _________________________________________
//
// Copy the view 'view_nr' to the screen
// In : int view_nr : the view to copy
//__________________________________________________________________________

void TWinManager::CopyViewToScreen( int view_nr )
{
}

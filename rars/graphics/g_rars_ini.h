/**
 * RARS_INI.H - Struture fo the initialization of the 
 *              startin gialog box 
 *
 * History
 *  ver. 0.74 Mar,99 - creation
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef __RARS_INI_H
#define __RARS_INI_H

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

class RarsIni
{
private:
  void load();
  void save();

public:
  // saved data
  int    m_iLaps;                     // 80
  char   m_sTrackName[128];           // ex: monaco.trk
  int    m_iViewMode;                 // disabled
  char   m_sDrivers[512];             // List of drivers
  int    m_iFollowMode;               // Follow car mode
  char   m_sFollowCar[128];           // Car to follow
  int    m_iMaximized;                // Window maximized
  int    m_iViewOpenGL;               // In a OpenGl mode, the view used
  double m_fZoom;                     // Zoom
  int    m_iRandom;                   // Random seed
  int    m_iSurface;                  // Surface
  int    m_i3DS;                      // Use 3DS object and track
  int    m_iShowNames;                // Show driver names during the race

  // calculated data
  bool   m_aDriverSelected[MAX_CARS]; // constructed from m_sDrivers

  RarsIni();
  void save( const char * sTrackName, int iLaps, int iRandom, int iSurface, int iViewMode, int iFollowMode, const char * sFollowCar, const char * sDrivers );
  void SaveSettings();
  void SaveWindowSettings();          // Platform depent functions defined in xx_lowgr.cpp
  void DriverSelect();
};

#endif // __RARS_INI_H


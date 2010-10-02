/*
 * KDE_START.H - Start Window of RARS in KDE 2
 *
 * History
 *  ver. 0.76 Oct 00 - CCDOC
 *
 * @author    Marc Gueury <mgueury@synet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

#ifndef KDE_START_IMPL_H
#define KDE_START_IMPL_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <kapp.h>
#include "kde_start.h"

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

class KdeStartImpl : public KdeStart
{
  Q_OBJECT

public:
  KdeStartImpl();

public slots:
  virtual void slotStartRace();
  virtual void slotRadioToggled( bool on );
  virtual void slotChangeTrack(int index);
  virtual void slotDriverSelectOne();
  virtual void slotDriverSelectAll();
  virtual void slotDriverRemoveOne();
  virtual void slotDriverRemoveAll();
};

#endif


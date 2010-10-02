/**
 * JOYSTICK.CPP - joystick class
 *
 * History
 *  ver. 0.91 April 2003 - CCDOC
 *
 * @author    Eugen Treise
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.91
 */

#ifndef __JOYSTICK_H
#define __JOYSTICK_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifndef WIN32 // Linux

#include <linux/joystick.h>

#endif

//--------------------------------------------------------------------------
//                             Class Joystick
//--------------------------------------------------------------------------

// a common structure for 2 axes and 4 buttons
typedef struct
{
  int x;
  int y;
  bool button1;
  bool button2;
  bool button3;
  bool button4;
} JoystickStatus;

/* This class gives access to a joystick under Win32 and Linux.
 * 2 axes and 4 buttons are used at the moment.
 */
class Joystick
{
private:

#ifndef WIN32
  int joy_fd, num_of_axis, num_of_buttons;
  int* axis;
  char* button;
  struct js_event jse;
#endif

public:

  Joystick();
  virtual ~Joystick();

  bool init();
  bool GetStatus(JoystickStatus& js);
  void close();
};

/*
bool InitJoystick();
bool GetJoystickStatus(JoystickStatus& js);
void CloseJoystick();
*/
#endif  // __JOYSTICK_H

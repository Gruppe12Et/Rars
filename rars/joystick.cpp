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

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------


#ifdef WIN32

#include <windows.h>
#include <mmsystem.h>

#else  // Linux

// Linux version from http://lgdc.sunsite.dk/articles/19.html

#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/ioctl.h>

#endif

#include "joystick.h"

//--------------------------------------------------------------------------
//                             Class Joystick
//--------------------------------------------------------------------------

#ifdef WIN32

Joystick::Joystick()
{
}

Joystick::~Joystick()
{
}

bool Joystick::init()
{
  return (joyGetNumDevs() > 0);
}

bool Joystick::GetStatus(JoystickStatus& js)
{
  JOYINFO JoyInfo;
  bool RetValue = ( joyGetPos(JOYSTICKID1, &JoyInfo) == JOYERR_NOERROR );
  js.x = JoyInfo.wXpos - 32768;
  js.y = JoyInfo.wYpos - 32768;
  js.button1 = (JoyInfo.wButtons & JOY_BUTTON1) > 0;
  js.button2 = (JoyInfo.wButtons & JOY_BUTTON2) > 0;
  js.button3 = (JoyInfo.wButtons & JOY_BUTTON3) > 0;
  js.button4 = (JoyInfo.wButtons & JOY_BUTTON4) > 0;
  return RetValue;
}

void Joystick::close()
{
}

#else  // Linux

// Linux version from http://lgdc.sunsite.dk/articles/19.html

#define JOY_DEV "/dev/js0"

Joystick::Joystick()
{
  axis = 0;
  button = 0;
}

Joystick::~Joystick()
{
}

bool Joystick::init()
{
  
  joy_fd = open( JOY_DEV , O_RDONLY);
  if( joy_fd != -1 )
  {
    ioctl( joy_fd, JSIOCGAXES, &num_of_axis );
    ioctl( joy_fd, JSIOCGBUTTONS, &num_of_buttons );

    axis = new int[num_of_axis];
    button = new char[num_of_buttons];

    fcntl( joy_fd, F_SETFL, O_NONBLOCK );	/* use non-blocking mode */
    return true;
  }
  else
    return false;
}

bool Joystick::GetStatus(JoystickStatus& js)
{
  js.x = js.y = 0;
  js.button1 = js.button2 = js.button3 = js.button4 = false;

  /* read the joystick state */
  while( read(joy_fd, &jse, sizeof(struct js_event))>0 )
  {

    /* see what to do with the event */
    switch (jse.type & ~JS_EVENT_INIT)
    {
      case JS_EVENT_AXIS:
        axis [ jse.number ] = jse.value;
        break;
      case JS_EVENT_BUTTON:
        button [ jse.number ] = jse.value;
        break;
    }
  }
  
  js.x = axis[0];
  js.y = axis[1];
  js.button1 = button[0] > 0;
  js.button2 = button[1] > 0;
  js.button3 = button[2] > 0;
  js.button4 = button[3] > 0;
  
  return true;
}

void Joystick::close()
{
  if( axis )
    delete [] axis;
  if( button )
    delete [] button;

  ::close( joy_fd );
}

#endif  //WIN32

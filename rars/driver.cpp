/**
 * DRIVER.CPP - Object Oriented Robot
 *
 * Object Oriented robots
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include "car.h"
#include "string.h"

//--------------------------------------------------------------------------
//                            CLASS Driver
//--------------------------------------------------------------------------

/**
 * Constructor
 */
Driver::Driver()
{
  m_sModel3D = NULL;
}

const char * Driver::getBitmapName2D()
{
  return m_sBitmapName2D;
}

const char * Driver::getModel3D()
{
  return m_sModel3D;
}

const char * Driver::getName()
{
  return m_sName;
}

void Driver::setName( char * _name )
{
  strcpy( m_sName2, _name );
  m_sName = m_sName2;
}

int Driver::getNoseColor()
{
  return m_iNoseColor;
}

void Driver::setNoseColor( int color )
{
  m_iNoseColor = color;
}

int Driver::getTailColor()
{
  return m_iTailColor;
}

void Driver::setTailColor( int color )
{
  m_iTailColor = color;
}

void Driver::init(int /*ID*/) {};

con_vec Driver::drive(situation & /*s*/)
{
  con_vec result = CON_VEC_EMPTY;
  return result;
}


ReplayDriver::ReplayDriver() : Driver()
{
  m_sName = "Dummy";
  m_sAuthor = "Rars";
  m_sDescription = "Dummy Driver for Rars Replay";
  m_iNoseColor = oWHITE;	//helm color
  m_iTailColor = oWHITE;	//car color
  m_sBitmapName2D = "car_white_white";
  m_sModel3D = NULL;	// model in 3D view
  new2DName = false;
}

ReplayDriver::~ReplayDriver()
{
  if( new2DName )
    delete [] (char*)m_sBitmapName2D;
}

void ReplayDriver::set2DBitmap()
{
  // m_iTailColor is used to create the name of pcx file for the car (simple for now)
  m_sBitmapName2D = new char[100];
  char ColorName[20];
  getColorName(ColorName, m_iTailColor);
  sprintf((char*) m_sBitmapName2D, "car_%s_%s.pcx", ColorName, ColorName);
  new2DName = true;
}



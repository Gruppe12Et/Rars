/**
 * RACE_DATA.CPP - Data of the race change during its race execution.
 *
 * History
 *  ver. 0.90 Creation
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "car.h"
#include "draw.h"
#include "misc.h"

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Contructor
 */
RaceData::RaceData()
{
  cars = new Car*[MAX_CARS];
}

/**
 * Destructor
 */
RaceData::~RaceData()
{
  delete [] cars;
}

/**
 * Sorting routines that's fast when things are already in order:
 * This routine updates the m_aCarInPos[] array which has the position of each
 * car.  m_aCarInPos[0] is the ID of the leader, m_aCarInPos[1] is in second place, etc.
 */
void RaceData::QSortem()
{
  int i, temp;
  
  for(i = 0; i < args.m_iNumCar-1; i++)
  {
    if( args.m_iQualMode==QUAL_FASTEST_LAP )
    {
      while(cars[m_aCarInPos[i+1]]->Q_bestlap > cars[m_aCarInPos[i]]->Q_bestlap)  
      { 
        // When a car runs faster qualifying lap, we swap positions:
        temp = m_aCarInPos[i]; 
        m_aCarInPos[i] = m_aCarInPos[i+1];
        m_aPosOfCar[m_aCarInPos[i+1]] = i;
        m_aCarInPos[i+1] = temp;
        m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
          --i;
      }
    }
    else if( args.m_iQualMode==QUAL_AVERAGE_SPEED )
    {
      while(cars[m_aCarInPos[i+1]]->Q_avgspeed > cars[m_aCarInPos[i]]->Q_avgspeed)  
      {
        // When a car gets higher average speed, we swap positions:
        temp = m_aCarInPos[i];
        m_aCarInPos[i] = m_aCarInPos[i+1];
        m_aPosOfCar[m_aCarInPos[i+1]] = i;
        m_aCarInPos[i+1] = temp;
        m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
          --i;
      }
    }
  }

  for(i = 0; i < args.m_iNumCar-1; i++)
  {
    if(cars[m_aCarInPos[i]]->Out == 3)
    {
      if(cars[m_aCarInPos[i+1]]->Out != 3) 
      { 
        temp = m_aCarInPos[i];
        m_aCarInPos[i] = m_aCarInPos[i+1];
        m_aPosOfCar[m_aCarInPos[i+1]] = i;
        m_aCarInPos[i+1] = temp;
        m_aPosOfCar[temp] = i+1;
        if(i > 0) // check if it passed more than one car
        {
          --i;
        }
      }
    }
  }
  if(draw.m_bDisplay)
  {
    draw.InitNewData();
  }
}

/**
 * Fills m_aStartPos[] with random permutation of 0, 1, 2..., car_count-1
 */
void RaceData::PickRandomOrder()
{
  int selected[MAX_CARS];               // marks the points as they are picked
  int i, j, k, count;
  
  for(i=0; i<args.m_iNumCar; i++)      // initialize selected[] to all zeroes:
  {
    selected[i] = 0;                   // (meaning no points are picked)
  }
  
  // for each element of rptr, j is no. of alternates 
  for(j=args.m_iNumCar; j>0; j--)      // remaining.  here we pick one of them
  { 
    k = j>1 ? (coreRand()/128)%j : 0;  // k chooses among the remaining pts.
    for(i=0, count=0; i<args.m_iNumCar; i++)
    {
      if(!selected[i])                 // if not already picked,
      {
        if(count++ == k)
        { 
          selected[i] = 1;             // marks this point as taken 
          m_aStartPos[j-1] = i;             // puts it into the target
        }
      }
    }
  }
}


/**
 * Order the cars (m_aStartPos)
 *
 * @param rl :  race_count
 */
void RaceData::OrderStartPos( int rl )
{
  int i, temp;

  if(args.m_bKeepOrder)
  {
    // The same order that is in initial drivers[] array (in drivers.cpp)
    for( i=0; i<args.m_iNumCar; i++)
    {
      m_aStartPos[i] = i;
    }
  }
  else   // random starting positions
  {
    if(!(rl%2))
    {
      PickRandomOrder();
    }
    else // reverse positions for 2nd race
    {
      for(i=0; i<args.m_iNumCar/2; i++)
      {
        temp = m_aStartPos[i];
        m_aStartPos[i] = m_aStartPos[args.m_iNumCar - 1 - i];
        m_aStartPos[args.m_iNumCar - 1 - i] = temp;
      }
    }
  }
}
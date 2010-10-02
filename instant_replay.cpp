/**
 * INSTANT_REPLAY.CPP - Record the trajectory of the cars
 *
 * Record the trajectory of the car in a buffer.
 * It records in a buffer of fixed lenght. When the buffer
 * is full, it restarts at the beginning.
 *  
 * The size of the buffer is INSTANT_BUFFER_SIZE.
 *
 * This means that after INSTANT_BUFFER_SIZE steps of Rars, the buffer
 * is always full.
 *
 * History
 *  ver. 0.9  Aug 2001 
 *
 * @author    Marc Gueury
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include "car.h"
#include "os.h"
#include "instant_replay.h"

//--------------------------------------------------------------------------
//                         CLASS InstantReplay
//--------------------------------------------------------------------------

/**
 * Constructor
 */
InstantReplay::InstantReplay()
{
  Init();
}

/**
 * Init
 */
void InstantReplay::Init()
{
  m_iNumData = 0;
  m_iCurrentPos = 0;
  m_iReplayPos = 0;
  m_iMode = INSTANT_INIT; //INSTANT_RECORD;
  m_bStopNextStep = false;
}


/**
 * Called when m_iMode = INSTANT_RECORD
 */
void InstantReplay::Record()
{
  InstantData * data = &(m_aData[m_iCurrentPos]);
  for( int i=0; i<args.m_iNumCar; i++ )
  {
    Car * car = race_data.cars[i];
    data->x[i] = car->x;
    data->y[i] = car->y;
    data->X[i] = car->X;
    data->Y[i] = car->Y;
    data->Z[i] = car->Z;
    data->ang[i] = car->ang;
    data->alpha[i] = car->alpha;
    data->cen_a[i] = car->cen_a;
    data->tan_a[i] = car->tan_a;
    data->collision_draw[i] = car->collision_draw;
    data->damage[i] = car->damage;
    data->offroad[i] = car->offroad;
    data->to_end[i] = car->to_end;
    data->to_rgt[i] = car->to_rgt;
    data->seg_id[i] = car->seg_id;
  }
  // Number of data
  if( ++m_iNumData>INSTANT_BUFFER_SIZE )
  {
    m_iNumData = INSTANT_BUFFER_SIZE;
  }
  if( ++m_iCurrentPos>=INSTANT_BUFFER_SIZE )
  {
    m_iCurrentPos = 0;
  }
  m_iReplayPos = m_iCurrentPos;

  CheckStopNextStep();

  // Pause at startup
  if( m_iMode==INSTANT_INIT )
  {
     m_iMode = INSTANT_STOP;
  }
}

/**
 * Called when m_iMode != INSTANT_RECORD
 *
 * Replay, or rewind in the buffer 
 */
void InstantReplay::Replay()
{
  switch( m_iMode )
  {
  case INSTANT_REPLAY:
    if( m_iReplayPos==m_iCurrentPos ) // Case after a STOP -> REPLAY -> RECORD 
    {
      m_iMode = INSTANT_RECORD;
      return;
    }
    m_iReplayPos++;
    if( m_iReplayPos==m_iCurrentPos-1 ) // Case after a STOP -> REWIND -> REPLAY -> RECORD
    {
      m_iMode = INSTANT_RECORD;
    }
    if( m_iReplayPos>=INSTANT_BUFFER_SIZE )
    {
      m_iReplayPos = 0;
    }
    ReplayCars();
    break;

  case INSTANT_STOP:
    // Do nothing....
    break;

  case INSTANT_REWIND:
    for( int i=0; i<4; i++ )                  // 4=Rewind speed
    {
      m_iReplayPos --;
      if( m_iReplayPos<0 )                    
      {
        if( m_iNumData!=INSTANT_BUFFER_SIZE ) // Buffer not full ?
        {
          m_iReplayPos = 0;
          m_iMode = INSTANT_STOP;             
        } 
        else
        {
          m_iReplayPos = INSTANT_BUFFER_SIZE-1;
        }
      }
      if( m_iReplayPos==m_iCurrentPos )
      {
        m_iMode = INSTANT_STOP;
        m_iReplayPos++;
      }
    }
    ReplayCars();
    break;
  }

  CheckStopNextStep();
}

/**
 * Replace the cars in their positions at 
 */
void InstantReplay::ReplayCars()
{
  InstantData * data = &(m_aData[m_iReplayPos]);
  for( int i=0; i<args.m_iNumCar; i++ )
  {
    Car * car = race_data.cars[i];
    car->x = data->x[i];
    car->y = data->y[i];
    car->X = data->X[i];
    car->Y = data->Y[i];
    car->ang = data->ang[i];
    car->alpha = data->alpha[i];
    car->cen_a = data->cen_a[i];
    car->tan_a = data->tan_a[i];
    car->collision_draw = data->collision_draw[i];
    car->damage = data->damage[i];
    car->offroad = data->offroad[i];
    car->to_end = data->to_end[i];
    car->to_rgt = data->to_rgt[i];
    car->seg_id = data->seg_id[i];
  }
}

/**
 * Stop. If already stopped and just at the end of the buffer, go to the next step
 */
void InstantReplay::Stop()
{
  if( m_iMode==INSTANT_STOP )
  {
    m_bStopNextStep = true;
    if( m_iReplayPos==m_iCurrentPos ) // on the last record
    {
      m_iMode=INSTANT_RECORD;
    }
    else
    {
      m_iMode=INSTANT_REPLAY;
    }
  }
  else 
  {
    m_iMode = INSTANT_STOP; 
  }
}

/**
 * Check if it need to stop on the next step
 */
void InstantReplay::CheckStopNextStep()
{
  if( m_bStopNextStep )
  {
    m_iMode= INSTANT_STOP;
    m_bStopNextStep = false;
  }
}
/**
 * INSTANT_REPLAY.H - Record the trajectory of the cars
 *
 * @author    Marc Gueury
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef __RARSCORE_INSTANT_REPLAY_H
#define __RARSCORE_INSTANT_REPLAY_H


//--------------------------------------------------------------------------
//                          D E F I N E S
//--------------------------------------------------------------------------

#define INSTANT_BUFFER_SIZE 1000

#define INSTANT_RECORD   0
#define INSTANT_REPLAY   1
#define INSTANT_STOP     2
#define INSTANT_REWIND   3
#define INSTANT_INIT     4

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

class InstantData
{
  public:
    double x[MAX_CARS];       // private value (1 step after to_end)
    double y[MAX_CARS];
    double X[MAX_CARS];       // public value (coherent with to_end)
    double Y[MAX_CARS];       
    double Z[MAX_CARS];
    double ang[MAX_CARS];
    double alpha[MAX_CARS];
    double cen_a[MAX_CARS];
    double tan_a[MAX_CARS];
    int collision_draw[MAX_CARS];
    unsigned long damage[MAX_CARS];
    int offroad[MAX_CARS];
    double to_end[MAX_CARS];
    double to_rgt[MAX_CARS];
    int seg_id[MAX_CARS];
};

/**
 * InstantReplay (used by RaceManager)
 */
class InstantReplay
{
  private:
    int m_iNumData;                          // Number of data stored in the data                   
    int m_iCurrentPos;                       // Number of current position of the data                   
    int m_iReplayPos;                        // Position of the replay                   
    bool m_bStopNextStep;                    // Stop after the next step
    InstantData m_aData[INSTANT_BUFFER_SIZE];// Position of the robot

    void ReplayCars();                         
    void CheckStopNextStep();

  public:
    int  m_iMode;                            // INSTANT_REPLAY, RECORD, STOP, REWIND

    InstantReplay();
    void Init();
    void Record();
    void Replay();
    void Stop();

  friend class TView2D;  
  friend class TView3D;
};

#endif



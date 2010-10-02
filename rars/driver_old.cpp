/**
 * DRIVER_OLD.CPP - Wrapper for old robots
 *
 * Wrapper for old robots to Object Oriented robots
 *
 * @author    Marc Gueury <mgueury@skynet.be>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "car.h"
#include "misc.h"

//--------------------------------------------------------------------------
//                           CLASS DriverOld
//--------------------------------------------------------------------------

/**
 * Constructor fo the old type of cars
 */
DriverOld::DriverOld( robot * _rob_ptr, int nose_color, int tail_color, const char * _bitmap_name, const char * _model_3d, const char * _rob_name ) : Driver()
{
  rob_ptr = _rob_ptr;
  m_iNoseColor = nose_color;
  m_iTailColor = tail_color;
  m_sBitmapName2D = _bitmap_name;
  m_sName = (char *)_rob_name;
  m_sModel3D = _model_3d;

  // default value
  m_sAuthor = (char *)"-";
  m_sDescription  = (char *)"-";
}

/**
 * Call the original function of the boot
 * 
 * @param s             situation
 * @return              alpha, vc
 */
con_vec DriverOld::drive( situation &s )
{
  return rob_ptr( s );
}

/**
 * For compatibility, 1 call to get the name 
 * This name is not used anymore.
 */
void DriverOld::init(int ID)
{
  rel_state rel_state_vec[NEARBY_CARS];
  situation s = FillSituation(rel_state_vec);
  s.my_ID = ID;
  rob_ptr( s );
}

/**
 * Create a default situation vector to pass to control() during initializaton 
 *
 * @param rel_state_vec_ptr    (in) for the nearby structure
 * @return                     a default situation vector
 */
situation DriverOld::FillSituation(rel_state* rel_state_vec_ptr)
{ 
  situation result; 

  result.cur_rad = 0.0; 
  result.cur_len = 1.0; 
  result.to_lft =  1.0;     
  result.to_rgt =  1.0;   
  result.to_end =  1.0;     
  result.v = 20.0;
  result.vn = 0.0;         
  result.nex_len = 1.0;    
  result.nex_rad = 1.0;    
  result.after_rad = 0.0; 
  result.power_req = 1.0;  
  result.dead_ahead = 0;    
  result.backward = 0;      
  result.bestlap_speed = 0.0; 
  result.lastlap_speed = 0.0;  
  result.lap_time = 0.0;       
  result.distance = 0.0;
  result.behind_leader = 0.0;
  result.damage = 0;
  result.seg_ID = 0;
  result.laps_to_go = 0;
  result.laps_done = 0;
  result.position = 0;
  result.starting = 1;
  result.started = 0;          
  result.out_pits = 0; 
  result.side_vision = 0;       
  result.nearby = rel_state_vec_ptr; 
  for(int k=0; k<3; k++)
  {
    result.nearby[k].who = 999;
  }
 
  return result; 
} 
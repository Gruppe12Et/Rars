/**
 * CAR.H - class and structure declarations for RARS (Robot Auto Racing Sim)
 *
 * Reads the track definition file to define the track and
 * set the location of the scoreboard, leaderboard and other display items.
 *
 * History
 *  ver. 0.1 release January 12, 1995 
 *  ver. 0.2 1/23/95 
 *  ver. 0.3 2/7/95 
 *  ver. 0.39 3/6/95 
 *  ver. 0.45 3/21/95 
 *  ver. 0.50 4/5/95 
 *  ver. 0.6b May 8, 1995  b for beta 
 *  ver. 0.61 May 26 
 *  ver. 0.63b July 20
 *  ver. 0.70 Nov, 97  * physical constants moved here
 *  ver. 0.71 Jan, 98  * RaceManager class and qualifying
 *  ver. 0.72 Mar, 98  * more security on data, different scoring options
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @see       CAR.H & TRACK.H for class and structure declarations 
 * @version   0.76
 */

#ifndef __RARSCORE_CAR_H
#define __RARSCORE_CAR_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifdef min
#undef min
#endif
#ifdef max
#undef max
#endif

#include <fstream>            // movie recording
#include <math.h>
#include "memory.h"
#include "globals.h"
#include "gi.h"
#include "deprecated.h"

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

// Forward declaration
class Driver;
class Movie;

// from DRIVERS.CPP
extern Driver* drivers[];

// function to get a car out of a crash or pileup or spin-out:
int stuck(int, double, double, double, double, double*, double*);
double sqrt(double);                   // just to avoid some reads of math.h
double friction(double slip);          // export friction model
int r_rand();

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#ifndef PI
const double PI = 3.14159265;
#endif
const double DEGPRAD = 360.0/(2.0 * PI);   // degrees per radian
const double FEET_BY_METER = 3.28083332;
const double METER_BY_FEET = 0.30480061;

#ifdef random
#undef random
#endif
#ifndef RAND_MAX
#define RAND_MAX 0x7fff
#endif

// Choose ONE of the following scoring systems:
//#define F1
#define INDYCAR
//#define NASCAR
//#define BTCC

#define VERSION 0.90

const double g = 32.2;                 // acceleration due to gravity, ft./sec^2
//const int MAXCARS = 16;                // nr of drivers defined in drivers.cpp
const int NEARBY_CARS = 5;             // number of cars reported in s.nearby 
const int MAXRAND = 0x7FFF;            // maximum value returned by r_rand() 
const double Q_CUTOFF = 1.20;          // 120% cutoff for race qualification
const double MPH_FPS = 3600.0/5280.0;  // mph per fps (0.681) 
const double CARLEN = 20; // 14;       // feet; 6.6 (4.3) m 
const double CARWID = 10; //  7;       // feet; 3.3 (2.1) m 
const double PM = 1e5; //4e5;          // Power, Maximum, ca 182 (727) horsepowers
const double DRAG_CON = .0065;         // air drag, lb. per (ft/sec)^2 
const double M = 75; //42;             // mass, slugs   (about 1100 (610) kg without fuel) 
const double MAX_FUEL = 150; // 440;   // inital fuel supply, lb.,ca 68(200) kg
const double SFC = .45e-6;//1e-6;      // Fuel Consumption, lb.s/ft-lb.
            //(should take f1 car with 200 l tank through 190 mile race)
const unsigned long MAX_DAMAGE = 30000; // out of race with this damage
static const double STARTING_SPEED = 20.0;  // cars start at this speed, ft/sec
static const double REVERSE_GEAR_LIMIT = 20; // ft/sec max to allow reverse
static const int PRIV_DATA_SIZE = 4096;// size of robot's private data area
const double MYU_MAX0 = 1.0;           // maximum coeficient of friction, surface 0
const double MYU_MAX1 = 1.05;          // maximum coeficient of friction, surface 1
const double MYU_MAX2 = 1.8;           // maximum coeficient of friction, surface 2
const double SLIPPING = 2.0;           // slip that generates half_maximum of friction

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Relative position and velocity vectors of a car in front
 * These are wrt a non-rotating frame of reference centered on
 * the car behind.  The y axis is in the direction the car is pointing.
 */
struct rel_state 
{
  double rel_x;              // how far to your right is the car ahead?
  double rel_y;              // how far in front of you?
  double rel_xdot;           // how fast cutting accross from left-to-right?
  double rel_ydot;           // how fast is he getting away from you?
  double alpha;              // car's current alpha
  double to_lft;             // car's s.to_lft position
  double to_rgt;             // car's s.to_rgt position 
  double v;                  // car's velocity 
  double vn;                 // car's velocity wrt track 
  double dist;               // distance ahead on track 
  int who;                   // an identifier for that car. 
  int braking;               // car is braking
  int for_position;          // pass is for position 
  int coming_from_pits;      // flagged if car is just about to drive out   
};
 
/**
 * Control vector, steering & throttle, pit_stop orders   
 */
struct con_vec 
{
  double alpha, vc, fuel_amount;
  int request_pit, repair_amount;
}; 

#define CON_VEC_EMPTY {0.0, 0.0, 0.0, 0, 0} 

/**
 * To hold the colors of one car
 */
struct colors
{
  int nose;
  int tail;
};

/**
 * This struct is calculated by observe() and used by control() 
 */
struct situation             // a car's local situation as seen by the driver 
{
  double cur_rad;            // radius of inner track wall, 0 = straight, minus = right
  double cur_len;            // length of current track segment (angle if curve) 
  double to_lft;             // distance to left wall 
  double to_rgt;             // distance to right wall 
  double to_end;             // how far to end of current track seg. (angle or feet)
  double v;                  // the speed of the car, feet per second 
  double vn;                 // component of v perpendicular to track direction
  double nex_len;            // length of the next track segment (angle if curve) 
  double nex_rad;            // radius of inner wall of next segment (or 0) 
  double after_rad;          // radius of the segment after that one. (or 0) 
  double after_len;          // length of the segment after that one. 
  double aftaft_rad;         // radius of segment after that one. (or 0) 
  double aftaft_len;         // length of segment after that one.
  double cen_a, tan_a;       // centripetal, tangential acceleration
  double alpha, vc;          // wheel angle of attack and wheel command velocity
  double power_req;          // ratio: power requested by driver to maximum power 
  double power;              // power delivered, divided by PwrMax
  double fuel;               // lbs. of fuel remaining 
  double fuel_mileage;       // miles per lb.
  double time_count;         // elapsed time since start of race
  double start_time;         // value of time_count when first lap begins 
  double bestlap_speed;      // speed of cars best lap in that race fps 
  double lastlap_speed;      // speed of last lap, fps 
  double lap_time;           // time in seconds to complete most recent lap 
  double distance;           // distance traveled from start of first segment 
  double behind_leader;      // seconds behind leader on last SF crossing
  int dead_ahead;            // set when there is a car dead ahead, else 0 
  unsigned long damage;      // accumulated damage units (out of race 30000)
  Stage stage;               // What's happening, what stage of the competition? 
  int my_ID;                 // to tell which car object you are driving 
  int seg_ID;                // current segment number, 0 to NSEG-1 
  int laps_to_go;            // laps remaining to be completed
  int laps_done;             // laps completed 
  int out_pits;              // 1 if coming out from pits after stop - adjust your speed
  int go_pits;               // becomes 1 when main program takes over for pitting 
  int position;              // 0 means leading, 1 means 2nd place, etc. 
  int started;               // where were we on the starting grid? 
  int lap_flag;              // changes from 0 to 1 on each crossing of finish line
  int backward;              // set if cars motion is opposed to track direction
  int starting;              // if not zero, robot knows to initialize data 
  int side_vision;           // allow cars alongside in s.nearby data
  rel_state* nearby;         // relative states of three cars in front of you 
  void* data_ptr;            // pointer to driver's scratchpad RAM area 
};

/**
 * A robot is a function that takes a situation reference and returns a con_vec:
 */
typedef con_vec(robot)(situation&);

/** 
 * Old Driver structure : This structure make one car different from another 
 */
struct car_ID              
{
  robot*  rob_ptr;           // pointer to the robot "driver" function; 
  colors  paint_job;         // the nose and tail colors
  char*   bitmap_name;       // name of the bitmap to use to display the car
  char    rob_name[33];      // pointer to the first character of the name string 
};

/** 
 * New Driver structure : This structure make one car different from another 
 */
class Driver 
{
protected:
  const char *  m_sName;          // Name of the car
  char          m_sName2[32];     // Used during the function setName
  const char *  m_sAuthor;        // Name of the author
  const char *  m_sDescription;   // Some description, version, date...
  int     m_iNoseColor;           // the nose color
  int     m_iTailColor;           // the tail color
  const char *  m_sBitmapName2D;  // name of the bitmap to use to display the car (View 2D)
  const char *  m_sModel3D;       // name of the bitmap to use to display the car (View 3D)

public:
  Driver();
  virtual ~Driver() {};

  const char * getBitmapName2D();
  const char * getModel3D();
  const char * getName();
  void setName( char * _name );
  int getNoseColor();
  void setNoseColor( int color );
  int getTailColor();
  void setTailColor( int color );

  virtual void init(int ID);
  virtual con_vec drive(situation &s);
};

class DriverOld : public Driver
{
private:
  robot*  rob_ptr;
  situation FillSituation(rel_state* rel_state_vec_ptr);

public:
  DriverOld( robot * _rob_ptr, int nose_color, int tail_color, const char * _bitmap_name, const char * _model_3d, const char * _rob_name );
  virtual void init(int ID); 
  virtual con_vec drive(situation &s);
};

/**
 * A robot2 is a function to create a object of type Driver
 */
typedef Driver *(robot2)();


/**
 * Dummy driver to replay a movie
 */
class ReplayDriver : public Driver
{
private:
  // Return an available color name for a color number.
  static void getColorName(char* ColorName, int ColorNr)
  {
    const char* ColorNames[] = { "black", "blue", "green", "green", "red", "pink", "black", "gray", "gray", "blue", "green", "green", "lred", "orange", "yellow", "white" };
    strcpy(ColorName, ColorNames[ColorNr]);
  }
  
  bool new2DName;     // a new 2D file name was assigned to this driver, has to be deleted in destructor

public:
  ReplayDriver();
  virtual ~ReplayDriver();
  // Calculate the m_sBitmapName2D with m_iNoseColor and m_iTailColor
  void set2DBitmap();
};


/** 
 * Each car is an instance of this class:
 */
class Car
{
  int nose_color, tail_color;          // the car's colors
  int collision_draw;                  // draw car in different color
  int backward_count;                  // don't allow driving backwards
  double x, y, xdot, ydot, ang, adot;  // current state of car (feet, seconds)
  double prex, prey, prang;            // previous state drawn on screen 
  double pre_xdot, pre_ydot;           // previous velocity components 
  int lap_flag;                        // changes from 0 to 1 on each crossing of finish line 
  double to_end;                       // same as in s.to_end, above
  double to_rgt;                       // same as in s.to_rgt, above 
  double vn;                           // same as in s.vn, above 
  double cen_a, tan_a;                 // centripetal, tangential acceleration 
  double pre_x_a, pre_y_a;             // previous acceleration components
  double alpha, vc;                    // wheel angle of attack and wheel command velocity 
  double prev_alpha;                   // previous value of alpha
  int which;                           // which car (index into pcar[] array) 
  int offroad, veryoffroad;            // flags, set if off the track
  double power_req;                    // power requested by driver, divided by PwrMax 
  double power;                        // power delivered, divided by PwrMax
  double fuel;                         // lbs of fuel remaining 
  unsigned long damage;                // accumulated damage units (out of race 30000)
  int dead_ahead;                      // set if there is another car dead ahead 
  unsigned int init_flag;              // for use only by control program (driver)
  double start_time;                   // time instant of 1st crossing starting line 
  double last_crossing;                // time instant of most recent crossing  
  int starting;                        // if not zero, robot knows to initialize
  int repair_amount;                   // repair to do during next pitting
  double fuel_amount;                  // how much fuel to add to the tank 
  int go_pits;                         // becomes 1 when main program takes over for pitting 
  int out_pits;                        // 1 when exiting pits 
  double pit_done_time;                // moment when all done in pits
  double full_load;                    // desired tank load (fuel + fuel_amount) 
  Driver * driver;                     // pointer to the car's driver program
  void* data_ptr;                      // pointer to driver's scratchpad RAM area 
  situation s;                         // situation data
  int done;                            // completed race when set
  int out;                             // out of race when 1, in pits when 2 
  int seg_id;                          // current track segment index
  double distance;                     // how many feets travelled from SF lane 
  long laps;                           // laps completed 
  long laps_lead;                      // lead race at SF crossing   
  long last_pit_visit;                 // at end of lap # 
  int started;                         // cars position on the starting grid
  int pit_stops;                       // number of pit visits 
  int on_pit_lane;                     // approaching  or exiting pits
  int coming_from_pits;                // flagged if car is just about to drive out
  int pitting;                         // set when car is standing in pits
  double speed_avg, speed_max;         // average and maximum speed 
  double bestlap_speed, lastlap_speed; // speed of this cars best lap & last lap   
  double lap_time;                     // most recent lap time
  double q_bestlap;                    // best lap time in qualifying  
  double q_avgspeed;                   // average speed of qual attempt
  double last_fuel[3];                 // fuel amount at prev. SF crossing 
  double fuel_mileage;                 // miles/gallon at last lap
  long projected_laps;                 // how far can go with this fuel?
  double RobotTime;                    // Time in milliseconds used by the robot

  void Observe();                      // computes the car's local situation
  void CheckNearby(rel_state*);        // computes the car's local situation
  void Control();                      // the "driver" - actual code selected by cntrl[]
  void MoveCar();                      // simulates the physics of car, track, control
  void CheckCollisions();              // checks for collisions between cars
  void DrawCar();                      // erases and redraws the car
  void PutCar(double, double, double); // puts car on track
  int  Farther(Car*);                  // is the car farther than another car
  double AlphaLimit(double, double);   // Limit alpha
  void RecordMovie( Movie* );          // the void* is pointer to the data file
  void ReplayMovie( Movie* );
  double getAirResistance(int);        // calculate relative air resistance for a given car

public: 
  int Out; 
  int Done; 
  int Seg_id;                          // current track segment index
  int Started;                         // car's position on the starting grid
  int Pit_stops;                       // number of pit visits 
  int On_pit_lane;                     // approaching  or exiting pits
  int Coming_from_pits;                // flagged if car is just about to drive out
  int Pitting;                         // set when car is standing in pits
  int Offroad, Veryoffroad;            // flags, set if off the track
  int Pitstops[1000];                  // At which laps were your pitstops?
  unsigned long Damage;                // accumulated damage units (out of race 30000)
  long Laps;                           // how many laps done
  long Laps_to_go;                     // how many laps to finish
  long Laps_lead;                      // lead race at SF crossing   
  long Last_pit_visit;                 // at end of lap # 
  double X, Y;                         // Car absolute coordinates!
  double Z;                            // Value not used by the simulation (used only by G_VIEW3D) 
  double Distance;                     // how many feets travelled from SF lane 
  double Total_pit_time;               // Total time spent standing in pits
  double Speed_avg, Speed_max;         // average and maximum speed 
  double Bestlap_speed, Lastlap_speed; // speed of this cars best lap & last lap   
  double Lap_time;                     // most recent lap time
  double Last_crossing;                // time instant of most recent crossing  
  double Behind_leader;                // seconds behind leader on last SF crossing  
  double Behind_next;                  // seconds behind the car in front on last SF crossing  
  double Ahead_next;                   // seconds ahead the car behind on last SF crossing  
  double Q_bestlap;                    // best lap time in qualifying  
  double Q_avgspeed;                   // average speed of qual attempt
  double prex2, prey2, prang2;

  Car(int);                            // constructor
  ~Car();                              // destructor

  inline double get_speed() const;     // returns the car's true speed
  inline double get_speed_x() const;   // returns the car's speed in x
  inline double get_speed_y() const;   // returns the car's speed in y
  inline long get_damage() const;      // returns the car's damage
  inline double get_fuel() const;      // returns the car's fuel load
  inline double get_vc() const;        // returns the car's commanded speed
  inline double get_alpha() const;     // returns the car's skid angle
  inline double get_lat_acc() const;   // returns the car's lateral acceleration
  inline double get_lin_acc() const;   // returns the car's in-line acceleration
  inline double get_fuel_mileage() const;// returns the car's fuel mileage miles/lb
  inline long get_proj_laps() const;   // returns laps the car can go with this fuel
   
  friend class Report;
  friend class InstantReplay;
  friend class RaceManager;
  friend class Draw;
  friend class TView;
  friend class TView2D;
  friend class TView3D;
  friend class TViewClassical;
  friend class TViewBoard;
  friend class TViewTelemetry;
  friend class TTrack3D;
  friend class ChartData;

  // see deprecated.h
  friend double alpha_limit(double, double);
  friend int farther(Car*, Car*);
};

//--------------------------------------------------------------------------
//                           I N L I N E S
//--------------------------------------------------------------------------

// These eight functions are member functions of the Car class.    
// All they do is fetch private data from the car objects.    

/**
 * Get the speed of the car
 *
 * @return the speed of the car
 */
inline double Car::get_speed() const
{
  return sqrt(xdot * xdot + ydot * ydot);  
}

/**
 * Get the speed in the x direction
 *
 * @return the speed in the x direction
 */
inline double Car::get_speed_x() const
{
  return xdot;
}

/**
 * Get the speed in the y direction
 *
 * @return the speed in the y direction
 */
inline double Car::get_speed_y() const
{
  return ydot;
}

/**
 * Get the speed asked to the car
 *
 * @return the speed asked to the car
 */
inline double Car::get_vc() const
{
  return vc;
}

/**
 * Get the alpha asked to the car
 *
 * @return the alpha asked to the car
 */
inline double Car::get_alpha() const
{
  return alpha;
}

/**
 * Get the linear acceleration
 *
 * @return the linear acceleration
 */
inline double Car::get_lin_acc() const
{
  return tan_a/g;
}

/**
 * Get the lateral acceleration
 *
 * @return the lateral acceleration
 */
inline double Car::get_lat_acc() const
{
  return cen_a/g;
}

/**
 * Get the damage
 *
 * @return the damage
 */
inline long Car::get_damage() const
{
  return damage;
}

/**
 * Get the projected laps (fuel)
 *
 * @return projected laps 
 */
inline long Car::get_proj_laps() const
{
  return projected_laps;
}

/**
 * Get the fuel used per mile
 *
 * @return fuel used per mile
 */
inline double Car::get_fuel_mileage() const
{
  return fuel_mileage;
}

/**
 * Get the fuel 
 *
 * @return the fuel 
 */
inline double Car::get_fuel() const
{
  return fuel;
}
  
#endif


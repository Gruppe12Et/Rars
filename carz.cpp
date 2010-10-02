/*
 * CAR.CPP - simulated robot car racing - begin development Dec. '94
 *
 * History
 *  ver. 0.1 release January 12, 1995
 *  ver. 0.63b July95 - even better collision code, -D option, better re-fuel
 *  ver. 0.64b Oct95 - For the start, cars are placed beginning with front row
 *  ver. 0.65 Feb 96 - Additions to s.nearby structure (D. Sparks)
 *  ver. 0.67 Nov 97 - pitting,random tracks,car colors in carz.cpp
 *                      (T. Thellefsen, H. Klaskala & R. Scott)
 *  ver. 0.70 Dec 97 - tracks and cars can be placed under any angle
 *                  - pit stops for refueling and repair
 *                  - drivers moved to drivers.cpp (M.Remm)
 *  ver. 0.71 Mar98  - qualifications and new structure of main fn.
 *                  - RaceManager class (M.Gueury and M.Remm)
 *  ver. 0.72 Mar 98 - modified dead_ahead, air_drag and car's public data
 *                  - track record, multiple tracks (MG and MR)
 *  ver. 0.73 Apr 99 - Movies work again (MG)
 *  ver. 0.74 May 99 - Additional friction model, new leaderboard design (MR)
 *  ver. 0.75 Jun 00 - New timing, new bots
 *  ver. 0.76 Oct 00 - CCDOC
 *  ver. 0.81 Feb 01 - New collision code by Rémi Coulom
 *  ver  0.90 Aug 01 - Split carz.cpp = car+race_manager+setting+
 *
 * @author    Mitchell E. Timin, State College, PA
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <math.h>
#include <stdlib.h>          // exit()
#include <stdio.h>           // exit()
#include <string.h>          // strcpy, strcat, strlen
#include <iostream>        // C++ stream support
#include "car.h"             // car objects
#include "track.h"           // track structure
#include "os.h"              // command line arguments interpretation
#include "misc.h"            // vec_mag(), coreRand(), qsortem()
#include "movie.h"           // movie recording and replay stuff
#include "draw.h"

using namespace std;

//--------------------------------------------------------------------------
//                          F U N C T I O N S
//--------------------------------------------------------------------------

////////////////////////////////////////////////////////////////////////////
// Simulation of the cars
////////////////////////////////////////////////////////////////////////////

/**
 * This function is used only by zbrent().  It calculates power delivered
 * to the tires using the same formulas as in move_car().  The value
 * returned to the caller is that power minus 99.75% of maximum power.
 * The target power used by zbrent() is therefore .9975 * PM.
 * We use this instead of full power because zbrent() can err slightly
 * on either side of the target, and we should not exceed PM.
 *
 * @param vc            (in) the desired speed of the car [feet/sec]
 * @param sine          (in) sin(alpha)
 * @param cosine        (in) sin(alpha)
 * @param v             (in) the current speed of the car [feet/sec]
 * @param mass          (in) the mass of the car [unit]
 * @return              power excess
 */
static double power_excess(double vc, double sine, double cosine, double v, double mass)
{
  double Ln, Lt;             // normal and tangential components of slip vector
  double l;                  // magnitude of slip vector, ft. per sec.
  double F;                  // force on car from track, lb.
  double Fn, Ft;             // tangential and normal (to car's path) force components

  Ln = -vc * sine;   Lt = v - vc * cosine; // vector sum to compute slip vector
  l = vec_mag(Lt, Ln);                     // compute slip speed
  F = mass * g * friction(l);              // compute friction force from track
  if(l < .0001)                            // to prevent possible division by zero
  {
    Fn = Ft = 0.0;
  }
  else
  {
    Fn = -F * Ln/l;          // compute components of force vector
    Ft = -F * Lt/l;
  }
  // compute power excess over target value of .9975*PM:
  return (vc < 0.0 ? -vc : vc) * (Ft * cosine + Fn * sine) - .9975*PM;
}

/**
 * Something needed by zbrent()
 *
 * @param a             (in) a
 * @param b             (in) b
 * @return              b>0 -> +a else -a
 */
inline double SIGN(double a, double b)
{
  return b >= 0.0 ? fabs(a) : -fabs(a);
}

/**
 * This routine was adapted from the book "Numerical Recipes in C" by
 * Press, et. al.  It searches for a value of vc which causes the
 * power to be very close to the maximum available.
 * (This is Brent's method of root finding.)  x1 and x2 are values of
 * vc which bracket the root.  b represents the variable vc.
 * (See power_excess(), above.)
 *
 * @param sine          (in)
 * @param cosine        (in)
 * @param v             (in) the current speed of the car [feet/sec]
 * @param x1            (in) vc coodinate 1 (v*cosine) [feet/sec]
 * @param x2            (in) vc coodinate 2 (vc) [feet/sec]
 * @param mass          (in) the mass of the car [unit]
 * @param tol           (in)
 * @return              the max vc depending of the powerpi
 */
static double zbrent(double sine, double cosine, double v, double x1, double x2, double mass,
              double tol)
{
  const int ITMAX = 20;
  const double EPS = 1.0e-8;
  int iter;
  double a=x1, b=x2, c=x2, d=0, e=0, min1, min2;
  double fa=power_excess(a, sine, cosine, v, mass);
  double fb=power_excess(b, sine, cosine, v, mass);
  double fc,p,q,r,s,tol1,xm;
  double Ln, Lt;      // normal and tangential components of slip vector
  double l;           // magnitude of slip vector, ft. per sec.
  double F;           // force on car from track, lb.
  double Fn, Ft;      // tangential and normal (to car's path) force components

  if ((fa > 0.0 && fb > 0.0) || (fa < 0.0 && fb < 0.0))
  {
    return b;              // This should never happen.
  }
  fc=fb;
  for (iter=1;iter<=ITMAX;iter++)
  {
    if ((fb > 0.0 && fc > 0.0) || (fb < 0.0 && fc < 0.0))
    {
      c=a;
      fc=fa;
      e=d=b-a;
    }
    if (fabs(fc) < fabs(fb))
    {
      a=b;
      b=c;
      c=a;
      fa=fb;
      fb=fc;
      fc=fa;
    }
    tol1=2.0*EPS*fabs(b)+0.5*tol;
    xm=0.5*(c-b);
    if (fabs(xm) <= tol1 || fb == 0.0)
    {
      return b;
    }
    if (fabs(e) >= tol1 && fabs(fa) > fabs(fb))
    {
      s=fb/fa;
      if (a == c)
      {
        p=2.0*xm*s;
        q=1.0-s;
      }
      else
      {
        q=fa/fc;
        r=fb/fc;
        p=s*(2.0*xm*q*(q-r)-(b-a)*(r-1.0));
        q=(q-1.0)*(r-1.0)*(s-1.0);
      }
      if (p > 0.0)
      {
        q = -q;
      }
      p=fabs(p);
      min1=3.0*xm*q-fabs(tol1*q);
      min2=fabs(e*q);
      if (2.0*p < (min1 < min2 ? min1 : min2))
      {
        e=d;
        d=p/q;
      }
      else
      {
        d=xm;
        e=d;
      }
    }
    else
    {
      d=xm;
      e=d;
    }
    a=b;
    fa=fb;
    if (fabs(d) > tol1)
    {
      b += d;
    }
    else
    {
      b += SIGN(tol1,xm);
    }
    Ln = -b * sine;   Lt = v - b * cosine; // vector sum to compute slip vector
    l = vec_mag(Lt, Ln);               // compute slip speed
    F = mass * g * friction(l);        // compute friction force from track
    if(l < .0001)                      // to prevent possible division by zero
    {
      Fn = Ft = 0.0;
    }
    else
    {
      Fn = -F * Ln/l;      // compute components of force vector
      Ft = -F * Lt/l;
    }
    // compute power delivered:
    fb = (b < 0.0 ? -b : b) * (Ft * cosine + Fn * sine) - .9975*PM;
  }
  return b;
}

/**
 * This is the model of the track friction force on the tire. This force
 * provides propulsion, cornering, and braking.  Force is assumed to depend
 * only on the slip speed, rising very rapidly with small slip speed,
 * and then asymtotically approaching an upper limit.  (This is similar to
 * tires on unpaved surfaces.)  There are two models here, the global
 * variable "surface" chooses between them.
 *
 * @param               (in) given the slip speed [ft.per sec]
 * @return              returns the coef. of friction,
 */
double friction(double slip)
{
  if( args.m_iSurface==0 )           // (very loose surface)
  {
    return (MYU_MAX0 * slip)/(SLIPPING + slip);
  }
  else if( args.m_iSurface==1 )      // (harder surface, ordinary cars)
  {
    return MYU_MAX1 * (1.0 - exp(-slip/SLIPPING));
  }
  else                       // surface == 2 - (asphalt, F1 tyres)
  {
    if(slip > 20)
    {
      return MYU_MAX2 * (1.0 - exp(-slip/SLIPPING)) - .2; // skidding
    }
    else
    {
      return MYU_MAX2 * (1.0 - exp(-slip/SLIPPING));
    }
  }
}

/**
 * A version of the friction model with randomness in the "slipping" variable
 *
 * @param               (in) given the slip speed [ft.per sec]
 * @return              returns the coef. of friction,
 */
static double rfriction(double slip)
{
  double slipping;           // slip speed at which half maximum force is reached

  slipping = 1.5 + (double)coreRand()/MAXRAND;    // range is 1.5 to 2.5 fps

  if( args.m_iSurface==0 )           // (very loose surface)
  {
    return (MYU_MAX0 * slip)/(slipping + slip);
  }
  else if( args.m_iSurface==1 )      // (harder surface, ordinary cars)
  {
    return MYU_MAX1 * (1.0 - exp(-slip/slipping));
  }
  else                       // surface == 2 - (asphalt, F1 tyres)
  {
    if(slip > 20)
    {
      return MYU_MAX2 * (1.0 - exp(-slip/slipping)) - .2; // skidding
    }
    else
    {
      return MYU_MAX2 * (1.0 - exp(-slip/slipping));
    }
  }
}

////////////////////////////////////////////////////////////////////////////
// Car class
////////////////////////////////////////////////////////////////////////////

/**
 * The Car constructor
 */
Car::Car(int i)
{
  which = i;                    // Each car has it own index into race_data.cars[]
  driver = drivers[i];          // Each car has a pointer to its driver.
  nose_color = drivers[i]->getNoseColor();
  tail_color = drivers[i]->getTailColor();
  x = y = 0;   // This is for initializing of cars in qualifications
  X = Y = Z = 0;
  prex = 0;        // These 3 are initialized so that
  prey = 0;        // draw_car() will work OK the
  prang = 0.0;     // first time it is called.
  power=power_req=.9; // meaningless, but will be soon replaced by move_car()
  data_ptr = (void*)new char[PRIV_DATA_SIZE];// give the robot some RAM to use
  tan_a = 0;
  last_crossing = 0;
  s.side_vision = 0;
  collision_draw = 0;
  q_bestlap = 0;
  q_avgspeed = 0;
  to_rgt = 0;
  offroad = 0;
  RobotTime = 0.0;
}

/**
 * The Car destructor
 */
Car::~Car()
{
  // changed to use array delete
  delete [] (char *)data_ptr;
  driver->~Driver();          // execute drivers destructor
}

/**
 * The purpose of control() is to call the car's driver function, and
 * then to return the steering and throttle settings produced by
 * that routine.
 *
 * Calls the control function via the pointer that was installed in
 * the car object by the constructor
 */
void Car::Control()
{
  con_vec output;
  int i;

  if(out)
  {
    output.alpha = output.vc = 0.0;  // no action if out of race
  }
  else
  {
    s.data_ptr = data_ptr;   // the robot's data area in RAM
    s.starting = starting;   // startup signal for the robot
    s.out_pits = out_pits;   // drive going out of pitstop
    s.go_pits  = go_pits;    // driver in the pitstop

    if( starting && race_data.stage != QUALIFYING )
    {
      os.ShowInitMessage( "Initialising car: %s", driver->getName() );
    }
    output = driver->drive(s);// call the robot driver

    if (starting)            // new in version 0.70
    {
      fuel = output.fuel_amount;
      if (fuel < 1 || fuel > MAX_FUEL) // make it maximum if
      {
        fuel = MAX_FUEL;     // fuel is not initialized in robot driver
      }
      starting = 0;
    }
  }

  // slow down after crossing the finish line
  if (done && (s.v > 80))
  {
    vc = s.v * 0.95;
  }
  else
  {
    vc = output.vc;
  }
  alpha = output.alpha;     // set private vars in car object
  // limit how fast alpha can change, and its maximum value:
  alpha = AlphaLimit(prev_alpha, alpha);
  prev_alpha = alpha;

  ////
  //// PIT ENTRY CODE
  ////
  if( output.request_pit == 1 &&
      race_data.m_iNumCarFinished == 0 && // pitting is not allowed on last lap of the race:
      !go_pits && !out_pits && // not already pitting
      distance > currentTrack->m_fPitEntry && distance < currentTrack->m_fPitEntry+100)
  {
    // car that is at pit entry has asked for a pit stop
    go_pits = 1;
    last_pit_visit = laps+1;
    ++pit_stops;
    for(i=0;i<1000;i++)      // record when were pitstops
    {
      if(!Pitstops[i]){
        Pitstops[i]=laps+1;
        break;
      }
    }
    repair_amount = output.repair_amount;
    fuel_amount = output.fuel_amount;
    if (repair_amount < 0)             // exclude some bogus values
    {
      repair_amount = 0;
    }
    if (repair_amount > (long)damage)  // can't repair more than damage is
    {
      repair_amount = damage;
    }
    if (fuel_amount < 0)               // can't take fuel out of tank
    {
      fuel_amount = 0;
    }
    if (fuel_amount > MAX_FUEL - fuel)
    {
      fuel_amount = MAX_FUEL - fuel;   // no more than full tank!
    }
  }
}

/**
 * Calculate relative air density/resistance for a given car
 * @param car           (in) number of the car
 * @return              air resistance for this car
 *                        0 - min (no) air resistance
 *                        1 - max air resistance
 */
double Car::getAirResistance(int car)
{
  double airResistance;
  double distToCar, maxDist;
  double sideOffset;

  // set to max resistance
  airResistance=1.0;

  for(int i=0; i<args.m_iNumCar; i++)
  {
    if(i == which)                     // ignore oneself
      continue;

    if(race_data.cars[i]->out)                   // ignore dead cars
      continue;

    distToCar = race_data.cars[i]->distance - distance;
    maxDist = race_data.cars[i]->s.v;
    if( distToCar < CARLEN || distToCar > maxDist )             // car is to far away
      continue;

    sideOffset = fabs(to_rgt - race_data.cars[i]->to_rgt);
    if( sideOffset > CARWID )                               // car drives an other line
      continue;

    airResistance *= 1 - (1-distToCar/maxDist) * (1-sideOffset/CARWID);
  }

  return airResistance;
}


/**
 * Simulates the physics, moves the car by changing the state variables.
 * The angle "alpha" is the angle between the car's orientation angle and
 * its velocity vector.  (like angle of attack of an aircraft)
 * Cornering force depends on alpha.  (It is also thought of as a slip angle.)
 * "Slip" refers to wheel vs. track motion.
 * "vc" is the speed of the bottom of the wheel relative to the car.
 * This model is like a four-wheel drive car, since the forces are not
 * computed separately for front and rear wheels.
 */
void Car::MoveCar()
{
  double D;           // force on car from air, lb.
  double Fn, Ft;      // normal & tangential components of track force vector
  double P;           // power delivered to track, ft. lb. per sec.
  double v;           // car's speed
  double Ln, Lt;      // normal and tangential components of slip vector
  double l;           // magnitude of slip vector, ft. per sec.
  double F;           // force on car from track, lb.
  double x_a, y_a;    // accelleration components in x & y directions
  double sine, cosine, temp;
  double mass;                 // current mass of car

  v = vec_mag(xdot, ydot);             // the car's speed, feet/sec
  if(v > speed_max)                    // keep track of max speed
  {
    speed_max = v;

  }
  mass = M + fuel/g;                   // current mass of car + fuel

  // Don't allow reverse gear
  if(vc < 0.0)                         // This would be a reverse gear request
  {
    if(v > REVERSE_GEAR_LIMIT)         // if going too fast
    {
      vc = 0.0;                        // make it maximum braking
    }
  }

  ////
  //// OUT ?
  ////
  // check accumulated damage and fuel, take car out of race if necessary:

  if(damage > MAX_DAMAGE || fuel <= 0.0 || (race_data.stage == QUALIFYING && laps >= args.m_iNumQualLap))
  {
    if(offroad)
    {
      if(!out)
      {
        ++race_data.m_iNumCarOut;   // Put it out of race if too much damage.
        // Show it in different color in score- and leaderboard:
        draw.m_aNewData[which] = 1; // to activate leaders()
      }
      if(v > .001)
      {
        temp = .8 *CARLEN / v;
      }
      else
      {
          temp = 1.0;    // This should be extremely rare.
      }
      x += temp * xdot;  // get it farther from edge
      y += temp * ydot;
      xdot = ydot = 0.0;   // stop it dead.

      // if cars are sorted by avg speed, accept only
      // cars that could finish ALL laps:
      if(race_data.stage == QUALIFYING && laps < args.m_iNumQualLap && args.m_iQualMode==QUAL_AVERAGE_SPEED )
      {
        q_avgspeed = 0;
      }

      if(!out) // the race is over :-(
      {
        out = Out = 1;
      }
      return;
    }
    else   // veer off the track:
    {
      // slow down and approach the right edge of track:
      if(v > 36.0)
      {
        alpha = .2 * (CARWID-to_rgt) / currentTrack->width - vn / v;
        vc = v - 2.0;
      }
      else
      {
        alpha = .8 * (-3*CARWID-to_rgt) / currentTrack->width - vn / v;
        vc = 35.0;    // stay just below 20 mph;
      }
    }
  }  // end of take-out routine

  if(out && !pitting)  // stop here if car is permanently out of race
  {
    return;
  }

  ////
  //// PIT CODE
  ////
  // find pit position for given car:
  // set zero mark on length/2, to avoid discontinuity going over SF-lane:

  double pit_position;
  if(currentTrack->m_fPitLaneStart < currentTrack->length/2)
  {
    currentTrack->m_fPitLaneStart += currentTrack->length;
  }
  pit_position = currentTrack->m_fPitLaneStart+(currentTrack->length+ currentTrack->m_fPitLaneEnd - currentTrack->m_fPitLaneStart)
               * started/args.m_iNumCar - currentTrack->length/2;
  if (pit_position < 0)
  {
    pit_position += currentTrack->length;
  }

  double pdist = distance - currentTrack->length/2; // car's distance from pit_zero
  if(pdist < 0)
  {
    pdist += currentTrack->length;
  }

  // dist. to right or to left to allow pitting on both sides
  double target = currentTrack->m_iPitSide < 0? currentTrack->width - to_rgt:to_rgt;

  // check if pit requested, take car to the pit:
  if (go_pits == 1)   // this means that main program takes over from the driver
  {
    if (pdist < pit_position)
    {
      // slow down in two phases and approach the proper edge of track:
      vc = v > currentTrack->m_fPitLaneSpeed + 1 ? v*0.8 : currentTrack->m_fPitLaneSpeed;
      if (v > currentTrack->m_fPitLaneSpeed)
      {
        alpha = .004 * currentTrack->m_iPitSide * (0.5*CARWID - target) - vn / v;
      }
      else
      {
        alpha = .008 * currentTrack->m_iPitSide*(-3*CARWID - target) - vn / v;
        if(target < -CARWID)
        {
          if(!on_pit_lane)
          {
            draw.m_aNewData[which] = 1;       // to change color on leaderboard
          }
          on_pit_lane = On_pit_lane = 1;// disappears from s.nearby
        }
      }
    }
    else   // enter your pits.
    {
      vc = 0.0;
      if (v < 15.0)
      {
        out = Out = 2;                 // temporarily out
        if(!pitting)                   // pit stop is just starting!
        {
          pitting = 1;                 // set until standing in pits
          draw.m_aNewData[which] = 1;  // to change leaderboard
          // find time required for pit stop:
          double pit_time = 0.005*repair_amount; // repair time
          if (pit_time < 0.05*fuel_amount) // refuel takes longer?
          {
            pit_time = 0.05*fuel_amount; // refuel time
          }
          full_load = fuel + fuel_amount; // refuel up to full_load
          pit_done_time = pit_time + race_data.m_fElapsedTime; // when is stop over?
          Total_pit_time += pit_time;   // for statistics
        }
        else
        {
          if(pit_done_time < race_data.m_fElapsedTime)//pit stop over, go racing!
          {
            pitting = 0;
            draw.m_aNewData[which] = 1;       // to change leaderboard
            go_pits = 0;
            s.out_pits = out_pits = 1;
            out = Out = 0;
            damage -= repair_amount;   // set new value for damage
          }

          if(fuel < full_load-1)
          {
              fuel += delta_time/0.05; // you will see fuel coming in
          }
        }
      }
    }
  }  // end of go_pits

  if (out_pits == 1)
  {
    if(pdist <= currentTrack->m_fPitExit+currentTrack->length/2) // still on pit lane
    {
      alpha = 0.005 * currentTrack->m_iPitSide * (-3*CARWID-target) - vn / v;
      vc = v < currentTrack->m_fPitLaneSpeed ? v + 5.0:currentTrack->m_fPitLaneSpeed;

      double pitexit_length = currentTrack->m_fPitExit - currentTrack->m_fPitLaneEnd < 200? currentTrack->m_fPitExit - currentTrack->m_fPitLaneEnd : 200;
      // 200 feets or less before exit set warning flag:
      if(pdist > currentTrack->m_fPitExit - pitexit_length+currentTrack->length/2)
      {
        coming_from_pits = 1;  // this goes to s.nearby to warn
      }
      // other cars well before exit.
    }
    else if (pdist < currentTrack->m_fPitExit + 200+currentTrack->length/2) //turn from pit lane to track
    {
      if(on_pit_lane)
      {
        draw.m_aNewData[which] = 1; // to change name color back to black
      }
      on_pit_lane = On_pit_lane = 0;
      alpha = 0.01 * currentTrack->m_iPitSide*(.6*CARWID - target) - vn/v;
      vc = v+.2;//v < PITLANE_SPEED ? v + 5.0:PITLANE_SPEED;
    }
    else  // 200 feets after pit exit, car should be now on track
    {
      s.out_pits = out_pits = 0; // back to driver control
      coming_from_pits = 0;
    }
  }

  if(out) // do not continue with move_car when car is pitting
  {
    return;
  }

  sine = sin(alpha);
  cosine = cos(alpha); // alpha is angle of attack



	// air drag force (add up to 200% air drag for damage!)
  D = DRAG_CON * v * v * (2*damage+MAX_DAMAGE)/MAX_DAMAGE * getAirResistance(which);
  if(offroad && !go_pits && !out_pits) // if the car is off the track,
  {
    D += (0.6 + .008 * v) * mass * g;  // add a lot more resistance
    if(veryoffroad  && !go_pits && !out_pits)
    {
      D += 1.7 * mass * g;
    }
  }

  ////
  //// CALC POWER AND MOVE CAR
  ////

  int it = 0;  // This is a loop counter.
VC:    // maybe loop to control power (we don't permit P > PM)

  Ln = -vc * sine;
  Lt = v - vc * cosine;                // vector sum to compute slip vector
  l = vec_mag(Lt, Ln);                 // compute slip speed
  F = mass * g * friction(l);          // compute friction force from track

  if(l < .0001)                        // to prevent possible division by zero
  {
    Fn = Ft = 0.0;
  }
  else
  {
    Fn = -F * Ln/l;                    // compute components of force vector
    Ft = -F * Lt/l;
  }
  // compute power delivered:
  P = (vc < 0.0 ? -vc : vc) * (Ft * cosine + Fn * sine);

  if(!it)                              // If this is the first time through here, then:
  {
    power_req = P/PM;                  // Tell the driver how much power it requested.
    if(P > PM)                         // If the request was too high, reduce it to 100% pwr.
    {
      ++it;
      vc = zbrent(sine, cosine, v, v * cosine, vc, mass, .006);
      goto VC;
    }
  }
  power = P/PM;                        // store this value in the car object

  // put some randomness in the magnitude of the traction force, F:
  // (Ft might be set to 0.0 above, in which case F will be zero.)
  if(Ft != 0.0 && args.m_bRandomMotion)
  {
    temp = rfriction(l) * mass * g / F; // ratio of new to original force
  }
  else
  {
    temp = 1.0;
  }

  // compute centripetal and tangential acceleration components:
  cen_a = Fn * temp /  mass;
  tan_a = (Ft * temp - D) / mass;

  if(P > 0.0)
  {
    fuel -= P * SFC * delta_time;
  }

  if(offroad && !go_pits && !out_pits && !args.m_bPractice)
  {
    // If off the track the car accumulates damage.
    // damage is proportional to square of acceleration:
    // damage from grass reduced 5x in version 0.70
    damage += (unsigned long)((tan_a * tan_a + cen_a * cen_a) / 50);
  }

  if(v < .0001)                        // prevent division by zero
  {
    adot = sine = cosine = 0.0;
  }
  else
  {
    adot = cen_a / v;                  // angular velocity
    sine = ydot/v;   cosine = xdot/v;  // direction of motion
  }
  x_a = tan_a * cosine - cen_a * sine; // x & y components of acceleration
  y_a = cen_a * cosine + tan_a * sine;

  // Advance the state using the Adam's predictor formula:
  x += (1.5 * xdot - .5 * pre_xdot) * delta_time;
  y += (1.5 * ydot - .5 * pre_ydot) * delta_time;
  pre_xdot = xdot;  pre_ydot = ydot;
  xdot += (1.5 * x_a - .5 * pre_x_a) * delta_time;
  ydot += (1.5 * y_a - .5 * pre_y_a) * delta_time;
  pre_x_a = x_a;   pre_y_a = y_a;
  if(v >= .0001)
  {
    ang = atan2(ydot,xdot);            // new orientation angle
  }
}

/**
 * Rotate Corners
 */
static void RotateCorners(double *px,
                          double *py,
                          double c,
                          double s,
                          double a)
{
 double ca = cos(a);
 double sa = sin(a);

 double csum = ca * c - sa * s;
 double ssum = ca * s + sa * c;

 for (int i = 4; --i >= 0;)
 {
  double x = px[i] * csum - py[i] * ssum;
  double y = px[i] * ssum + py[i] * csum;
  px[i] = x;
  py[i] = y;
 }
}

/**
 * Get overlap of corners of Me on the rectangle of Him
 */
static double GetOverlap(double *txMe,
                         double *tyMe,
                         double *txHim,
                         double *tyHim)
{
 double Result = 0;

 for (int i = 4; --i >= 0;)
  for (int j = 4; --j >= 0;)
  {
   int k = (j + 1) & 3;
   if ((tyHim[j] - tyMe[i]) * (tyHim[k] - tyMe[i]) < 0)
   {
    double x = txHim[j] + (tyMe[i] - tyHim[j]) *
               (txHim[k] - txHim[j]) / (tyHim[k] - tyHim[j]);
    double Overlap = txMe[i] - x;
    if (Overlap > Result)
     Result = Overlap;
   }
  }

 return Result;
}

/**
 * Check this car against other cars to see if a collision occurred.
 * This routine models a semi-elastic collision in that some energy
 * is absorbed by the car in the form of damage, while the rest is
 * reflected back, causing the cars to bounce off each other. The
 * amount of energy absorbed is related to the collective speed at
 * which the cars collide.
 */
void Car::CheckCollisions()
{
  double pvec_x, pvec_y;               // pointing vector of car (velocity vec + alpha)
  double sine, cosine, separation, dx, dy;
  double dot, mag_prod, temp, v;
  double rel_xdot, rel_ydot;           // velocity relative to other car being hit
  double theta, rel_y, rel_x;
  int i, other_seg;

  // First compute pointing vector by rotating velocity vector by alpha:
  sine = sin(alpha);   cosine = cos(alpha);// alpha is angle of attack
  v = vec_mag(xdot, ydot);             // the car's speed, feet/sec
  pvec_x = xdot * cosine - ydot * sine;// components of pointing vector:
  pvec_y = xdot * sine + ydot * cosine;
  dead_ahead = 0;   // will be set if there is a car more-or-less dead ahead

  for(i=0; i<args.m_iNumCar; i++)           // check for cars nearby or bumping
  {
    if(i == which)                     // ignore oneself
    {
      continue;
    }
    if(race_data.cars[i]->out)                   // ignore dead cars
    {
      continue;
    }
    other_seg = race_data.cars[i]->seg_id;       // only consider present and next segment
    if(!(seg_id == other_seg || incseg(seg_id) == other_seg))
    {
      continue;
    }
    dx = race_data.cars[i]->x - x;               // components of vector to other car
    dy = race_data.cars[i]->y - y;
    separation = vec_mag(dx, dy);      // distance to the other car
    dot = pvec_x * dx + pvec_y * dy;
    mag_prod = separation * v;         // p_vec and v vectors are same length

    //
    // Is there a car dead ahead?  "dead ahead" means within braking distance
    // AND within a few degrees of pointing vector  (arcos(.94) == 20 deg.)
    // compute dot product, rel. position & pointing vectors:
    // ignore cars farther away than braking distance:
    //
    if(separation <= ((v - race_data.cars[i]->vn - 20)*(v + race_data.cars[i]->vn - 20) / (MYU_MAX1*g*2)) &&
       dot > .94 * mag_prod)           // test based on properties of dot product
    {
     dead_ahead = 1;                   // there is a car more-or-less dead ahead
    }

    //
    // Now check for collisions and handle shocks
    //

    // Early exit if bounding spheres do not intersect
    if (dx * dx + dy * dy > CARLEN * CARLEN + CARWID * CARWID)
     continue;

    // The faster car of the two is considered responsible for the shock
    if (xdot * xdot + ydot * ydot <
        race_data.cars[i]->xdot * race_data.cars[i]->xdot + race_data.cars[i]->ydot * race_data.cars[i]->ydot)
     continue;

    // see if cars are closing
    rel_xdot = xdot - race_data.cars[i]->xdot;
    rel_ydot = ydot - race_data.cars[i]->ydot;
    dot = rel_xdot * dx + rel_ydot * dy;

    // compute relative rotation of car in front wrt this car:
    theta = race_data.cars[i]->ang - ang + race_data.cars[i]->alpha - alpha;// relative rotation

    // Compute components of relative position vector in axis system
    // aligned with pvec_x, pvec_y:
    rel_x = (dx * pvec_y - dy * pvec_x) / v;
    rel_y = dot / v;        // based on properties of dot product

    double c_cosine,c_sine,c_x1,c_x2,c_x3,c_x4,c_y1,c_y2,e;
    const double ELASTICITY_CONSTANT = 200;
    const double COLLISION_FRICTION = 0.99;
    const double MINIMUM_DAMAGE = 10;
    const double DAMAGE_MULTIPLIER = 100;

    // calculate angle of impact
    c_cosine = dx / separation;
    c_sine = dy / separation;

    // Test whether the car rectangles overlap and how much
    double txMe[4] = {CARLEN/2, CARLEN/2, -CARLEN/2, -CARLEN/2};
    double tyMe[4] = {-CARWID/2, CARWID/2, CARWID/2, -CARWID/2};
    double txHim[4] = {CARLEN/2, CARLEN/2, -CARLEN/2, -CARLEN/2};
    double tyHim[4] = {-CARWID/2, CARWID/2, CARWID/2, -CARWID/2};

    RotateCorners(txMe, tyMe, c_cosine, -c_sine, ang + alpha);
    RotateCorners(txHim, tyHim, c_cosine, -c_sine, race_data.cars[i]->ang + race_data.cars[i]->alpha);
    {
     for (int k = 4; --k >= 0;)
      txHim[k] += separation;
    }

    double Overlap1 = GetOverlap(txMe, tyMe, txHim, tyHim);
    {
     for (int k = 4; --k >= 0;)
     {
      txMe[k] = -txMe[k];
      txHim[k] = -txHim[k];
     }
    }
    double Overlap2 = GetOverlap(txHim, tyHim, txMe, tyMe);

    double Overlap = 0;
    if (Overlap1 > Overlap2)
     Overlap = Overlap1;
    else
     Overlap = Overlap2;

    if (Overlap == 0)
     continue;

    // calculate this car's collision vector wrt impact angle
    c_x1 = xdot * c_cosine + ydot * c_sine;
    c_y1 = -xdot * c_sine + ydot * c_cosine;

    // calculate other car's collision vector wrt impact angle
    c_x2 = race_data.cars[i]->xdot * c_cosine + race_data.cars[i]->ydot * c_sine;
    c_y2 = -race_data.cars[i]->xdot * c_sine + race_data.cars[i]->ydot * c_cosine;

    // calculate the impact speed along impact vector
    temp = fabs(c_x2 - c_x1);

    // calculate coefficient of elasticity
    e = exp(-temp / ELASTICITY_CONSTANT);

    // calculate new x components, we're ignoring mass for now
    c_x3 = 0.5 * (c_x1 + c_x2 - e * (c_x1 - c_x2));
    c_x4 = 0.5 * (c_x1 + c_x2 + e * (c_x1 - c_x2));

    // calculate new velocity vector for this car
    // assume that friction slows it down a bit
    c_y1 *= COLLISION_FRICTION;
    xdot = c_x3 * c_cosine - c_y1 * c_sine;
    ydot = c_y1 * c_cosine + c_x3 * c_sine;
    v = vec_mag(xdot,ydot);

    // calculate new velocity vector for the other car
    race_data.cars[i]->xdot = c_x4 * c_cosine - c_y2 * c_sine;
    race_data.cars[i]->ydot = c_y2 * c_cosine + c_x4 * c_sine;

    // calculate new position for this car
    x -= c_cosine * Overlap / 2;
    y -= c_sine * Overlap / 2;

    // calculate new position for the other car
    race_data.cars[i]->x += c_cosine * Overlap / 2;
    race_data.cars[i]->y += c_sine * Overlap / 2;

    // I assume that the cars can absorb a certain amount
    // of punishment without taking damage, but begin
    // taking damage in larger proportion above that level
    // I also assume that a certain amount of energy is
    // dissipated in heat and vibration
    // I also assume that the rear car will take the
    // majority of the damage
    temp *= (1 - e);
    temp = (temp * temp) * DAMAGE_MULTIPLIER - MINIMUM_DAMAGE;
    if (temp > 0 && !go_pits && !out_pits && !args.m_bPractice)
    {
      damage +=(unsigned long) (.50 * temp);
      race_data.cars[i]->damage +=(unsigned long) (.25 * temp);
      collision_draw = race_data.cars[i]->collision_draw = COLLISION_FLASH | 1;
    }
  }
}

/**
 * Calls drawcar() twice, once to erase, once to draw
 */
void Car::DrawCar()
{
  draw.DrawCar( this );
  if( collision_draw )
  {
    collision_draw--;
  }
}

/**
 * Initializes most of race-specific variables; set cars coordinates; draws car.
 *
 * @param x_pos         (in) x position [feet]
 * @param y_pos         (in) x position [feet]
 * @param alf_ang       (in) angle
 */
void Car::PutCar(double x_pos, double y_pos, double alf_ang)
{
  x = x_pos;
  y = y_pos;
  ang = alf_ang;
  to_end = currentTrack->lftwall[0].length;          // not exact, but corrected by observe()
  alpha = prev_alpha = 0.0;
  pit_done_time = full_load = 0.0;
  lap_time = speed_avg = speed_max = lastlap_speed = bestlap_speed = 0.0;
  backward_count = 100;
  laps = Laps = -1;                    // to become 0 crossing the finish line at start of race
  starting = 1;
  if(race_data.stage == QUALIFYING) seg_id = 0;  //in race this is set by arrange_cars
  laps_lead = last_pit_visit = pit_stops = 0;
  pitting = go_pits = out_pits = on_pit_lane = coming_from_pits = 0;
  out = done = lap_flag = init_flag = dead_ahead = damage = 0;
  distance = Behind_leader = Behind_next = Ahead_next = 0.0;
  vc = STARTING_SPEED;
  xdot = STARTING_SPEED*cos(ang);
  ydot = STARTING_SPEED*sin(ang);
  pre_xdot = pre_ydot = 0.0;
  pre_x_a = pre_y_a = 0.0;
  tan_a = cen_a = 0.0;
  projected_laps = 0;
  fuel_mileage = 0;
  fuel = last_fuel[0] = last_fuel[1] = last_fuel[2] = MAX_FUEL;
  //car starts with full tank, can be changed by robot in start of race

  // Also initialize car's public variables:
  X = Y = Distance = Speed_max = Speed_avg = Lap_time = 0.0;
  Done = Out = Laps_lead = Seg_id = Started = Pit_stops = 0;
  Last_pit_visit = On_pit_lane = Coming_from_pits = Pitting = 0;
  Bestlap_speed = Lastlap_speed = Last_crossing = 0.0;
  Q_bestlap = Q_avgspeed = Total_pit_time = 0.0;
  Offroad = Veryoffroad = Damage = 0;

  if(draw.m_bDisplay)
  {
    DrawCar();                        // draw the new car on the screen
  }
}

/**
 * This function uses the current state of the car, and the current
 * track segment data, to compute the car's local situation as seen by
 * the driver.  (see struct situation)
 */
void Car::Observe()
{
  double rad;                          // current radius
  double dx, dy, xp, yp;
  double sine, cosine;
  double temp;
  long i;                              // how many laps down?
  int nex_seg;                         // segment ID of the next segment
  int flag = 1;                        // controls possible repetition of calculations due to
  // completion of a lap.
  if(out)                              // This gets set if the car is stuck and off the track
  {
    return;
  }

  s.v      = vec_mag(xdot, ydot);      // the actual speed
  s.dead_ahead = dead_ahead;           // copy the value set by move_car()
  s.power_req = power_req;             // copy the value set by move_car()
  s.power  = power;                    // copy the value set by move_car()
  s.fuel   = fuel;                     // copy the value set by move_car()
  s.damage = damage;                   // copy the value set by move_car()
  s.cen_a  = cen_a;                    // copy the value set by move_car()
  s.tan_a  = tan_a;                    // copy the value set by move_car()
  s.alpha  = alpha;                    // copy the value set by move_car()
  s.vc     = vc;
  s.start_time = start_time;
  s.lap_flag = 0;
  s.time_count = race_data.m_fElapsedTime;// copy the global value
  s.stage = race_data.stage;     // copy the global value
  s.position = race_data.m_aPosOfCar[which];        // current position
  s.started = started;                 // starting position
  s.my_ID = which;
  s.fuel_mileage = fuel_mileage;       // miles per lb.
  s.behind_leader = Behind_leader;

  // Copy private variables to public area:
  X = x;    Y = y;    Done = done;    Out = out;

  Seg_id = seg_id; Distance = distance;
  Pit_stops = pit_stops;    Last_pit_visit = last_pit_visit;
  Coming_from_pits = coming_from_pits; Pitting = pitting;
  Speed_avg = speed_avg;    Speed_max = speed_max;
  Lap_time = lap_time;
  Q_bestlap = q_bestlap;    Q_avgspeed = q_avgspeed;
  Last_crossing = last_crossing;    Offroad = offroad;
  Veryoffroad = veryoffroad;    Damage = damage;


  // Computations below are based on the data in lftwall[] and
  // rgtwall[].  Radii are based on the inside rail in each case.

  while(flag) // This loop repeats only when a segment boundary is crossed,
  {           // in which case it repeats once:
    // compute sine and cosine of track direction:
    sine = sin(temp = currentTrack->rgtwall[seg_id].beg_ang);
    cosine = cos(temp);

    if((nex_seg = seg_id + 1) == currentTrack->NSEG)          // which segment is next
    {
      nex_seg = 0;
    }
    s.nex_len = currentTrack->rgtwall[nex_seg].length;        // length and radius
    s.nex_rad = currentTrack->lftwall[nex_seg].radius;        // of next segment
    if(s.nex_rad < 0.0)                         // always use smaller radius
    {
      s.nex_rad = currentTrack->rgtwall[nex_seg].radius;
    }

    if(++nex_seg == currentTrack->NSEG)
    {
      nex_seg = 0;
    }
    s.after_len = currentTrack->rgtwall[nex_seg].length;      // length and radius
    s.after_rad = currentTrack->lftwall[nex_seg].radius;      // and the one after that
    if(s.after_rad < 0.0)                       // always use smaller radius
    {
      s.after_rad = currentTrack->rgtwall[nex_seg].radius;
    }

    if(++nex_seg == currentTrack->NSEG)
    {
      nex_seg = 0;
    }
    s.aftaft_len = currentTrack->rgtwall[nex_seg].length;     // length and radius
    s.aftaft_rad = currentTrack->lftwall[nex_seg].radius;     // and the one after that
    if(s.aftaft_rad < 0.0)                      // always use smaller radius
    {
      s.aftaft_rad = currentTrack->rgtwall[nex_seg].radius;
    }

    s.cur_len = currentTrack->rgtwall[seg_id].length;         // copy these two fields:
    s.cur_rad = rad = currentTrack->lftwall[seg_id].radius;   // rt. turn will use rgtwall
    if(seg_id != 0)                             // This is used to tell when the finish line
    {
      lap_flag = 0;                             // is crossed, thus completing each lap
    }
    if(rad == 0)                                // if current segment is straight,
    {
      // xp and yp locate the car with respect to the beginning of the right
      // hand wall of the straight segment.  calculate them:
      dx = x - currentTrack->rgtwall[seg_id].beg_x;
      dy = y - currentTrack->rgtwall[seg_id].beg_y;
      xp = dx * cosine + dy * sine;
      yp = dy * cosine - dx * sine;
      s.to_rgt = yp;            // fill in to_rgt and to_end:
      s.to_end = s.cur_len - xp;

      if(seg_id == 0)
      {
        if(xp > currentTrack->m_fFinish * s.cur_len && !lap_flag)
        {
          /* ################# This means the line crossing was noted: #############*/
          s.lap_flag = lap_flag = 1;

          if (!draw.m_bDisplay && race_data.stage != QUALIFYING && !(laps%10) && !s.position)
          {
            cout << ".";   // Show progress for each 10 laps
          }

          // Add 1 lap to lap count:
          ++laps;
          ++Laps;

          // Accurate line crossing time
          double xv = xdot * cosine + ydot * sine;
          double LineTime = race_data.m_fElapsedTime - (xp - currentTrack->m_fFinish * s.cur_len) / xv;

          if(laps == 0)     // record when the starting line is crossed:
          {
            start_time = last_crossing = LineTime;
          }
          else if (!done)   // after first lap update speed and car's data
          {
            Speed_avg = speed_avg = currentTrack->length * laps / race_data.m_fElapsedTime;
            lap_time = LineTime - last_crossing;
            Lastlap_speed = lastlap_speed = currentTrack->length / lap_time;
            if (lastlap_speed > bestlap_speed)
            {
              Bestlap_speed = bestlap_speed = lastlap_speed;
              if (bestlap_speed > race_data.m_oLapRecord.speed)
              {
                race_data.m_oLapRecord.speed = bestlap_speed;
                strcpy(race_data.m_oLapRecord.rob_name, drivers[which]->getName());
              }

            }
            if(race_data.stage==QUALIFYING)
            {
              if (bestlap_speed > q_bestlap)
              {
                Q_bestlap = q_bestlap =  bestlap_speed;
              }
              if(laps == args.m_iNumQualLap && speed_avg > q_avgspeed)
              {
                Q_avgspeed = q_avgspeed = speed_avg;
              }
            }
            if (s.position == 0)
            {
              ++laps_lead;
              ++Laps_lead;
            }
            last_crossing = LineTime;

            // Calcualte fuel statistics:
            if (last_fuel[2] > fuel)
            {
              if (laps == 1)
              {
                fuel_mileage = currentTrack->length/((last_fuel[0] - fuel) * 5280.0);
              }
              else if (laps == 2)
              {
                fuel_mileage = 2*currentTrack->length/((last_fuel[1] - fuel) * 5280.0);
              }
              else
              {
                fuel_mileage = 3*currentTrack->length/((last_fuel[2] - fuel) * 5280.0);
              }
            }
            projected_laps = long(fuel*fuel_mileage*5280.0/currentTrack->length);
          }

          last_fuel[2] = last_fuel[1]; // 2 laps ago
          last_fuel[1] = last_fuel[0]; // 1 lap ago
          last_fuel[0] = fuel;         // now

          if (race_data.stage == RACING)
          {
            // calculate difference with other cars
            race_data.m_aLapFinishingTimes[which][laps] = LineTime;

            // distance to leader:
            Behind_leader = LineTime - race_data.m_aLapFinishingTimes[race_data.m_aCarInPos[0]][laps];
            if (race_data.cars[race_data.m_aCarInPos[0]]->laps - laps) // more than a lap down
            {
              Behind_leader = laps - race_data.cars[race_data.m_aCarInPos[0]]->laps; //negative!
            }

            // distance to car ahead:
            if (s.position)            // this car is NOT the leader!
            {
              Behind_next = LineTime - race_data.m_aLapFinishingTimes[race_data.m_aCarInPos[race_data.m_aPosOfCar[which]-1]][laps];
            }
            else
            {
              Behind_next = 0.0;       // it is leader! :-)
            }

            // distance to car behind:
            // not for the first lap or last car!
            if (laps < 1 || race_data.m_aPosOfCar[which] == args.m_iNumCar-1)
            {
              Ahead_next = 0.0;
            }
            else
            {
              Ahead_next = race_data.m_aLapFinishingTimes[race_data.m_aCarInPos[race_data.m_aPosOfCar[which]+1]][laps-1] - race_data.m_aLapFinishingTimes[which][laps-1];
            }

            // check if other car is more than 1 lap down:
            if (Ahead_next < 0)
            {
              if (!race_data.m_aLapFinishingTimes[race_data.m_aCarInPos[race_data.m_aPosOfCar[which]+1]][laps-1])
              {
                for(i=1;i<args.m_iNumLap;i++)
                {
                  if(race_data.m_aLapFinishingTimes[race_data.m_aCarInPos[race_data.m_aPosOfCar[which]+1]][laps-i])
                  {
                    Ahead_next = double(1-i); // How many laps down?
                    break;
                  }
                }
              }
              else //  car was just passed
              {
                Ahead_next = 999;
              }
            }
          }
          else if (race_data.stage == QUALIFYING) // sort qualifiers after each lap
          {
            race_data.QSortem();
          }

          if(draw.m_bDisplay)
          {
            draw.m_aNewData[which] = 1; // updates leaderboard for that car
            if(!s.position) // leading car crosses line
            {
              draw.Lapper(laps); // updates lap count
            }
            else
            {
              draw.Lapper(0);  // updates race fastest lap if necessary
            }
          }
          ///////////// Check if car has finished race:  //////////////
          // mode to finish race when everyone has crossed the line
          // or leaders have done 1 extra lap:
          if ((race_data.stage == QUALIFYING && laps == args.m_iNumQualLap) ||
            (race_data.stage == PRACTICE && laps == args.m_iNumPracticeLap) ||
            (race_data.stage == RACING && laps == args.m_iNumLap) ||
            (race_data.stage == RACING && !done && race_data.m_iNumCarFinished))
          {
            // somebody has finished race - stop this car too!
            ++race_data.m_iNumCarFinished;
            done = 1;
          }
          ///////////// end of lap crossing ////////////////////
        }
      }

      // here we make sure we are still in the same segment:
      if(s.to_end <= 0.0)           // see if a lap has been completed,
      {
        if(++seg_id == currentTrack->NSEG)
        {
          seg_id = 0;
        }
        continue;           // repeat the loop in context of next segment
      }
      s.to_lft = currentTrack->width - yp;           // fill in to_lft & cur_rad:
      s.cur_rad = 0.0;
      s.vn      = ydot * cosine - xdot * sine;  // compute cross-track speed
      s.backward = (xdot * cosine + ydot * sine < 0.0);
    }
    else if(rad > 0.0)       // when current segment is a left turn:
    {
      dx = x - currentTrack->rgtwall[seg_id].cen_x;//compute position relative to center
      dy = y - currentTrack->rgtwall[seg_id].cen_y;
      temp = atan2(dy, dx);    //this is the current angular position
      s.to_end = currentTrack->rgtwall[seg_id].end_ang - temp - PI/2.0;//this is angle
      if(s.to_end > 1.5 * PI)
      {
          s.to_end -= 2.0 * PI;
      }
      else if(s.to_end < -.5 * PI)
      {
        s.to_end += 2.0 * PI;
      }
      if(s.to_end <= 0.0)             // Handle segment crossing:
      {
        if(++seg_id == currentTrack->NSEG)
        {
          seg_id = 0;                // going from last segment to 1st
        }
        continue;
      }
      s.to_lft = vec_mag(dx, dy) - rad;
      s.to_rgt = currentTrack->width - s.to_lft;
      s.vn = (-xdot * dx - ydot * dy)/vec_mag(dx, dy);   // a trig thing
      s.backward = (ydot * dx - xdot * dy < 0.0);
    }
    else
    {
      s.cur_rad = rad = currentTrack->rgtwall[seg_id].radius;  // rt. turn needs rgtwall
      dx = x - currentTrack->rgtwall[seg_id].cen_x;//compute position relative to center
      dy = y - currentTrack->rgtwall[seg_id].cen_y;
      temp = atan2(dy, dx);      // this is the current angular position
      s.to_end = -currentTrack->rgtwall[seg_id].end_ang + temp - PI/2.0;//this is angle
      if(s.to_end < -.5 * PI)
      {
        s.to_end += 2.0 * PI;
      }
      else if(s.to_end >= 1.5 * PI)
      {
        s.to_end -= 2.0 * PI;
      }
      if(s.to_end <= 0.0)             // Handle segment transistion:
      {
        if(++seg_id == currentTrack->NSEG)
        {
          seg_id = 0;
        }
        continue;
      }
      s.to_rgt = vec_mag(dx, dy) + rad;
      s.to_lft = currentTrack->width - s.to_rgt;
      s.vn = (xdot * dx + ydot * dy)/vec_mag(dx, dy);   // a trig thing
      s.backward = (xdot * dy - ydot * dx < 0.0);
    }

    // If we get this far, we do not repeat the big loop.
    flag = 0;
  }  // end while(flag) loop

  s.seg_ID = seg_id;
  s.lap_time = lap_time;
  s.lastlap_speed = lastlap_speed;
  s.bestlap_speed = bestlap_speed;
  s.laps_done = laps;
  s.laps_to_go = args.m_iNumLap - laps;
  offroad = (s.to_lft < 0.0 || s.to_rgt < 0.0);   // maybe set offroad flag
  veryoffroad = (s.to_lft < -currentTrack->width || s.to_rgt < -currentTrack->width); // maybe veryoffroad
  to_end = s.to_end;                   // fill in these fields in Car object;
  to_rgt = s.to_rgt;
  vn = s.vn;

   // calculate distance travelled from SF lane
  if (rad)
  {
    distance = currentTrack->seg_dist[seg_id]
             - fabs((currentTrack->lftwall[seg_id].radius+currentTrack->rgtwall[seg_id].radius) * to_end / 2);
  }
  else
  {
    distance = currentTrack->seg_dist[seg_id] - to_end;
  }

  if (distance<0)
  {
    distance += currentTrack->length;
  }
  s.distance = distance;

  if(s.backward)
  {
    if(--backward_count == 0)
    {
      damage = MAX_DAMAGE;
    }
    return;               // If going backwards, skip looking for nearby cars.
  }
  else
  {
    backward_count = 100; // Backwards driving allowed for max. 5 seconds
  }
}

/**
 * This function uses the current state of the car, and the current
 * track segment data, to relate data about nearby cars to the
 * the driver.  (see struct rel_state)
 *
 * @param rel_vec_ptr   (out) an array of rel_state
 */
void Car::CheckNearby(rel_state* rel_vec_ptr)
{
  double dx, dy;
  double sine, cosine;
  double separation, temp, dxdot, dydot, max_dist;
  int i, k, m;

  // These two arrays describe the three closest cars:
  // element 0 is for the closest, element 1 next, element two after that.
  int closest[NEARBY_CARS];
  double how_close[NEARBY_CARS];
  for (i = 0; i < NEARBY_CARS; i++)
  {
    closest[i] = 999;                  // their ID's, initially too big
    how_close[i] = 1e5;                // initially very large distances
  }

  // figure look-ahead based on braking distance
  max_dist = 0.5 * s.v * s.v / g + 100;

  // The relative vectors must be found wrt to velocity vector frame.
  // we need sine and cosine of rotation of axes:
  if(s.v > 1e-7)                       // prevent division by 0
  {
    sine = - xdot / s.v;               // direction of velocity
    cosine = ydot / s.v;               // wrt the x axis.
  }
  else
  {
    sine = -1.0;
    cosine = 0.0;
  }

  // find the closest cars in front:
  // "front", here, is direction of velocity vector, not pointing vector.
  s.nearby = rel_vec_ptr;              // the robot's pointer to his RAM area
  for(i=0; i<args.m_iNumCar; i++)           // check for cars nearby
  {
    if(i == which)                     // ignore oneself
    {
      continue;
    }
    if(race_data.cars[i]->out)                   // ignore dead cars
    {
      continue;
    }
    if(race_data.cars[i]->on_pit_lane && !coming_from_pits) // ignore cars on pit lane, but not those leaving pits
    {
      continue;
    }

    // use track distance as a sieve
    temp = race_data.cars[i]->distance - distance;

    // if car is behind us on last segment, subtract the track length
    if ((seg_id == 0) && (race_data.cars[i]->seg_id == currentTrack->NSEG-1))
    {
      temp -= currentTrack->length;
    }

    // handle car ahead past the last segment
    if (temp < -CARLEN)
    {
      temp += currentTrack->length;
    }

    // ignore cars that are way ahead or completely behind us
    if ((temp > max_dist) || (temp < -CARLEN))
    {
      continue;
    }

    // get vector to other car
    dx = race_data.cars[i]->x - x;
    dy = race_data.cars[i]->y - y;

    // now check for cars alongside, but only if the driver wants it
    temp = cosine * dy - sine * dx;
    if (temp < 0)
    {
      if (!args.m_bGlobalSideVision || !s.side_vision || (temp < -CARLEN))
      {
        continue;
      }
    }

    // calculate distance to other car
    separation = vec_mag(dx,dy);

    // is this car closer than others?
    for(k=0; k<NEARBY_CARS; k++)
    {
      if(separation < how_close[k])
      {
        for(m = NEARBY_CARS-1; m > k; m--)
        {
          closest[m] = closest[m-1];
          how_close[m] = how_close[m-1];
        }
        closest[k] = i;
        how_close[k] = separation;
        break;
      }
    }
  }
  // now compute local relative state vector for those cars:
  for(k=0; k<NEARBY_CARS; k++)
  {
    if(closest[k] > args.m_iNumCar)         // This is when there aren't enough cars
    {
      for(; k<NEARBY_CARS; k++)
      {
        s.nearby[k].who = 999;
      }
      break;
    }
    i = closest[k];
    dx = race_data.cars[i]->x - x;      // components of relative position vector
    dy = race_data.cars[i]->y - y;
    dxdot = race_data.cars[i]->xdot - xdot; // components of relative velocity
    dydot = race_data.cars[i]->ydot - ydot;
    s.nearby[k].who = i;
    s.nearby[k].rel_x = cosine * dx + sine * dy;
    s.nearby[k].rel_y = cosine * dy - sine * dx;
    s.nearby[k].rel_xdot = cosine * dxdot + sine * dydot;
    s.nearby[k].rel_ydot = cosine * dydot - sine * dxdot;
    s.nearby[k].alpha = race_data.cars[i]->alpha;
    s.nearby[k].to_lft = race_data.cars[i]->s.to_lft;
    s.nearby[k].to_rgt = race_data.cars[i]->s.to_rgt;
    s.nearby[k].v = race_data.cars[i]->s.v;
    s.nearby[k].vn = race_data.cars[i]->s.vn;
    s.nearby[k].dist = how_close[k];
    s.nearby[k].braking = (race_data.cars[i]->tan_a < -5.0);
    s.nearby[k].for_position = (laps <= race_data.cars[i]->laps);
    s.nearby[k].coming_from_pits = race_data.cars[i]->coming_from_pits;
  }
}

/**
 * limits the rate of change and maximum value of the angle of attack
 *
 * @param was       This is what alpha was
 * @param request   The robot wants this alpha
 */
double Car::AlphaLimit(double was,
                       double request)
{
  const double MAX_RATE = 3.2;  // maximum radians per second possible
  const double MAX_ALPHA= 1.0;   // maximum radians possible

  double alpha;                 // the result to return

  if(request - was > MAX_RATE * delta_time)   // want more positive alpha
  {
    alpha = was + MAX_RATE * delta_time;
  }
  else if(was - request > MAX_RATE * delta_time)  // want more negative alpha
  {
    alpha = was - MAX_RATE * delta_time;
  }
  else
  {
    alpha = request;
  }

  if(alpha > MAX_ALPHA)
  {
    alpha = MAX_ALPHA;
  }
  else if(alpha < -MAX_ALPHA)
  {
    alpha = -MAX_ALPHA;
  }

  return alpha;
}

////////////////////////////////////////////////////////////////////////////
// Sorting of the cars
////////////////////////////////////////////////////////////////////////////

/**
 * Comparison routine for determining who's ahead: (used when sorting)
 *
 * @param      (in) car0
 * @return     1 if 'this' has gone farther than car0
 */
 int Car::Farther(Car* car0)
{
  if(car0->laps > laps)
  {
    return 0;
  }
  else if(car0->laps < laps)
  {
    return 1;                // else laps are equal
  }
  else if(car0->distance > distance)
  {
    return 0;
  }
  else if(car0->distance < distance)
  {
    return 1;
  }
  else
  {
    return 0;                // returns 0 in case of total equality -> else problem in <sortem>
  }
}



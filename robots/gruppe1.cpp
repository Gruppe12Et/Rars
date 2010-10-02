/*
Gruppe1.CPP (JACKBOT.CPP )

History
- yet another RARS 'bot, by M. Timin, June '95
- Object Oriented Robot + Pitting     January 2001

This file is the seventh in our tutorial series.  It is also a working robot.  The robot function name,
and its displayed name, is "JackBot". This file may be compiled and linked just as any other RARS
driver. The main purpose of JackBot is to present and explain some code that uses the "nearby" structure
for collision avoidance and passing. It has been tested only on OVAL2.TRK, and it runs very well there.
However, this code is not symetrical with respect to left and right, and will not perform well on tracks
with right turns.

This code is largely based on the first six tutorials.  I will try to explain where and why it differs
in the comments of the code.

The file CNTRL0.CPP is still useful as a reference for some details of robot code that are not explained
below.
*/

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdlib.h>
#include <math.h>
#include "car.h"

//--------------------------------------------------------------------------
//                           D E F I N E S
//--------------------------------------------------------------------------

// parameters to tinker with:
// accelerations are in feet/second per second.
// slips are in feet/second
// distances are in feet
const double CORN_MYU   = 1.00;     // lateral g's expected when cornering
const double BRAKE_ACCEL = -33.0;   // acceleration when braking on straight
const double BRAKE_SLIP  = 6.5;     // tire slip when braking
const double BRK_CRV_ACC = -27.0;   // acceleration when braking in curve
const double BRK_CRV_SLIP = 3.5;    // tire slip for braking in curve
const double MARGIN = 10.0;         // target distance from curve's inner rail
const double MARG2 = MARGIN+10.0;   // target for entering the curve
const double ENT_SLOPE = .33;       // slope of entrance path before the curve
const double STEER_GAIN = 1.0;      // gain of steering servo
const double  DAMP_GAIN = 1.2;      // damping of steering servo
const double  BIG_SLIP = 9.0;       // affects the bias of steering servo
const double CURVE_END = 4.0;       // when you are near end of curve, widths
const double TOO_FAST = 1.02;    // a ratio to determine if speed is OK in curve
const double DELTA_LANE = 2.5;   // if collision predicted, change lane by this

//--------------------------------------------------------------------------
//                           Class Tutorial3
//--------------------------------------------------------------------------

class Gruppe1 : public Driver
{
public:

  Gruppe1()
  {
    m_sName = "Tuto4";
    m_sAuthor = "Mitchell Timin";
    m_iNoseColor = oBLUE;
    m_iTailColor = oBLUE;
    m_sBitmapName2D = "car_blue_blue";
    m_sModel3D = NULL;
  }

  double corn_spd(double radius)  // returns maximum cornering speed, fps
  {
    //  MUST NEVER CALL THIS ROUTINE WITH ZERO OR NEGATIVE ARGUMENT!
    return sqrt(radius * 32.2 * CORN_MYU);     // compute the speed
  }

  // Calculates the critical distance necessary to bring a car from speed
  // v0 to speed v1 when the braking acceleration is "a", ft per sec^2.
  // Speeds are in fps.  ("a" should be negative)
  double CritDist(double v0, double v1, double a)
  {
    double dv;

    dv = v1 - v0;
    if(dv > 0.0)          // this saves having such a test in the caller
      return(0.0);
    return (v0 + .5 * dv) * dv / a;
  }

  con_vec drive(situation &s)       // This is the robot "driver" function:
  {
    con_vec result = CON_VEC_EMPTY; // This is what is returned.
    double alpha, vc;             // components of result
    double bias;                  // added to servo's alpha result when entering curve
    double speed;                 // target speed for curve (next curve if straightaway)
    double speed_next = 0.0;      // target speed for next curve when in a curve, fps.
    double width;                 // track width, feet
    double to_end;                // distance to end of present segment in feet.
    static double lane;           // target distance from left wall, feet
    static double lane0;          // value of lane during early part of straightaway
    static int rad_was = 0;       // 0, 1, or -1 to indicate type of previous segment
    static double lane_inc = 0.0; // an adjustment to "lane", for passing

    // service routine in the host software to handle getting unstuck from
    // from crashes and pileups:
    if(stuck(s.backward, s.v,s.vn, s.to_lft,s.to_rgt, &result.alpha,&result.vc))
      return result;

    width = s.to_lft + s.to_rgt;  // compute width of track

    // This is a little trick so that the car will not try to change lanes
    // during the "dragout" at the start of the race.  We set "lane" to
    // whatever position we have been placed by the host.
    if(s.starting)                // will be true only once
      lane = lane0 = s.to_lft;    // better not to change lanes during "dragout"

    // Set "lane" during curves.  This robot sets "lane" during curves to
    // try to maintain a small fixed distance to the inner rail.
    if(s.cur_rad > 0.0)           // turning left
    {
      lane = MARGIN;
      rad_was = 1;                // set this appropriate to curve.
    }
    else if(s.cur_rad < 0.0)      // turning right
    {
      lane = width - MARGIN;
      rad_was = -1;               // set this appropriate to curve.
    }
    else                          // straightaway:
    {
      // We will let the car go down the straigtaway in whatever "lane" it
      // comes out of the turn.
      if(rad_was)                 // If we just came out of a turn, then:
      {
        lane = s.to_lft;          // set "lane" where we are now.
        if(lane < .5 * width)     // but maybe push it a little more to right?
          lane += MARG2;          // (add MARG2 if we were to left of center)
        lane0 = lane;             // save a copy of the new "lane" value.
        rad_was = 0;              // set this appropriate to straightaway.
      }
      // This is for the transition from straight to left turn.  If we are
      // in a transition zone near the end of the straight, then set lane to
      // a linear function of s.to_end.  During this zone, "lane" will change
      // from "lane0" upon entering the zone to MARG2 upon reaching the end
      // of the straightaway.  ENT_SLOPE is the change in lane per change in
      // s.to_end.
      if(s.to_end < (lane0 - MARG2) / ENT_SLOPE)
        lane = MARG2 + ENT_SLOPE * s.to_end;
    }

    // set the bias:
    // Bias is an additive term in the steering servo, so that the servo
    // doesn't have to "hunt" much for the correct alpha value.  It is an
    // estimate of the alpha value that would be found by the servo if there
    // was plenty of settling time.  It is zero for straightaways.
    // Also, for convenience, we call the corn_spd() function here.  On
    // the straightaway, we call it to find out the correct speed for the
    // corner ahead, using s.nex_rad for the radius.  In the curve we of
    // course use the radius of the curve we are in.  But also, we call it
    // for the next segment, to find out our target speed for the end of
    // the current segment, which we call speed_next.
    if(s.cur_rad == 0.0)
    {
      bias = 0.0;
      if(s.nex_rad > 0.0)
        speed = corn_spd(s.nex_rad + MARGIN);
      else if(s.nex_rad < 0.0)
        speed = corn_spd(-s.nex_rad + MARGIN);
      else
        speed = 250.0;  // This should not execute, for a normal track file
    }
    else                     // we are in a curve:
    {
      if(s.nex_rad == 0.0)
        speed_next = 250.0;
      else
        speed_next = corn_spd(fabs(s.nex_rad) + MARGIN);
      speed = corn_spd(fabs(s.cur_rad) + MARGIN + fabs(lane_inc));
      bias = (s.v*s.v/(speed*speed)) * atan(BIG_SLIP / speed);
      if(s.cur_rad < 0.0)   // bias must be negative for right turn
        bias = -bias;
    }

    // set alpha:  (This line is the complete steering servo.)
    alpha = STEER_GAIN * (s.to_lft - lane)/width - DAMP_GAIN * s.vn/s.v + bias;

    // set vc:
    if(s.cur_rad == 0.0)              // If we are on a straightaway,
    {                                          // if we are far from the end,
      if(s.to_end > CritDist(s.v, speed, BRAKE_ACCEL))
         vc = s.v + 50.0;                           // pedal to the metal!
      else                      // otherwise, adjust speed for the coming turn:
      {
         if(s.v > TOO_FAST * speed)             // if we're a little too fast,
           vc = s.v - BRAKE_SLIP;                // brake hard.
         else if(s.v < speed/TOO_FAST)         // if we're a little too slow,
           vc = 1.1 * speed;          // accelerate hard.
         else                               // if we are very close to speed,
           vc = .5 * (s.v + speed);   // approach the speed gently.
      }
    }
    else       // This is when we are in a curve:  (seek correct speed)
    {
      // calculate distance to end of curve:
      if(s.cur_rad > 0.0)
        to_end = s.to_end * (s.cur_rad + MARGIN);
      else
        to_end = -s.to_end * (s.cur_rad - MARGIN);
      // compute required braking distance and compare:
      // This is to slow us down for then next curve, if necessary:
      if(to_end <= CritDist(s.v, speed_next, BRK_CRV_ACC))
        vc = s.v - BRK_CRV_SLIP;
      // but if there is a straight, or a faster curve next, then
      // we may want to accelerate:
      else if(to_end/width < CURVE_END && speed_next > speed)
        vc = .5 * (s.v + speed_next)/cos(alpha);
      else   // normally, just calculate vc to maintain speed in corner
        vc = .5 * (s.v + speed)/cos(alpha);
    }

    // Passing and anti-collision code:
    // This code first tries to predict a collision; if no collision is
    // predicted, it does nothing.  Collision prediction is approximate, and
    // is based on linear extrapolation.  This can work because it is
    // repeated eighteen times per second of simulated time.
    // If a collision is predicted, then it gradually changes the
    // lane_inc static variable which changes alpha.
    // The hope is to steer around the car.  When no collision is
    // predicted then lane_inc is gradually brought back to zero.
    // If a crash is about to occur, medium hard braking occurs.
    double x, y, vx, vy, dot, vsqr, c_time, y_close, x_close;
    int kount;     // counts cars that are in danger of collision
    kount = 0;
    for(int i=0;i<3;i++) if (s.nearby[i].who<16)  // if there is a close car
    {
      y=s.nearby[i].rel_y;         // get forward distance (center-to-center)
      x=s.nearby[i].rel_x;         // get right distance
      vx=s.nearby[i].rel_xdot;     // get forward relative speed
      vy=s.nearby[i].rel_ydot;     // get lateral relative speed
      // if the cars are getting closer, then the dot product of the relative
      // position and velocity vectors will be negative.
      dot = x * vx + y * vy;     // compute dot product of vectors
      if(dot > -0.1)            // no action if car is not approaching.
        continue;
      vsqr = vx*vx + vy*vy;      // compute relative speed squared
      // Time to closest approach is dot product divided by speed squared:
      c_time = -dot / vsqr;     // compute time to closest approach
      if(c_time > 3.0)          // ignore if over three seconds
        continue;
      /* If the execution gets this far, it means that there is a car
      ahead of you, and getting closer, and less than 3.0 seconds
      away.  Evaluate the situation more carefully to decide if
      evasive action is warranted: */
      x_close = x + c_time * vx;      // x coord at closest approach
      y_close = y + c_time * vy;      // y coord at closest approach
      /*  Due to the length of the cars, a collision will occur if
          x changes sign while y is less than CARLEN.  This
          can happen before the center-to-center distance reaches its
          point of closest approach. */
      // check if collision would occur prior to closest approach
      // if so, reduce c_time, re-calculate x_close and y_close:
      if(x_close * x < 0.0 && y < 1.1 * CARLEN)
      {
        c_time = (fabs(x) - CARWID) / fabs(vx);
        x_close = x + c_time * vx;      // x coord at closest approach
        y_close = y + c_time * vy;      // y coord at closest approach
      }
      // Will it be a hit or a miss?
      if(fabs(x_close) > 2 * CARWID || fabs(y_close) > 1.25 * CARLEN)
        continue;            // this when a miss is predicted
      // If we get here there is a collision predicted
      ++kount;    // This counts how many cars are in the way.
      if(kount > 1 || c_time < .85)  // if more than one problem car, or if
        vc = s.v - BRK_CRV_SLIP;    // car within .85 sec of collision, brake!
      // steer to avoid the other car:
      // if there is room, we try to pass with least x deviation
      if(s.cur_rad > 0.0)
        if(x_close < 0.0 || s.to_lft < MARGIN)  // avoid scraping the inside
          lane_inc += DELTA_LANE;
        else
          lane_inc -= DELTA_LANE;
      else if(s.cur_rad < 0.0)
        if(x_close > 0.0 || s.to_rgt < MARGIN)
          lane_inc -= DELTA_LANE;
        else
          lane_inc += DELTA_LANE;
      else if(x_close < 0.0)      // on straights, pass with least x deviation
        lane_inc += DELTA_LANE;
      else
        lane_inc -= DELTA_LANE;
      if(lane_inc > .25 * width)  // limit the lane alteration to 1/4 width:
        lane_inc = .25 * width;
      else if(lane_inc < -.25 * width)
        lane_inc = -.25 * width;
    }

    // Here we gradually reduce lane_inc to zero if no collision is predicted:
    if(!kount)
      if(lane_inc > .1)
        lane_inc -= .5*DELTA_LANE;
      else if(lane_inc < -.001)
        lane_inc += .5*DELTA_LANE;

    // lane_inc represents an adjustment to the lane variable.  This is
    // accomplished by changing alpha an amount equal to that which the
    // steering servo would have done had lane actually been changed.
    result.vc = vc;   result.alpha = alpha - STEER_GAIN * lane_inc / width;

    // Pit: if the fuel is too low
    //  Fuel: full
    //  Damage: repair all
    if( s.fuel<10.0 )
    {
      result.request_pit   = 1;
      result.repair_amount = s.damage;
      result.fuel_amount = MAX_FUEL;
    }

    return result;
  }
};

Driver * getGruppe1Instance()
{
  return new Gruppe1();
}

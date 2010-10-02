/**
 * TRACKGEN.CPP - a random track generator for RARS
 *
 * Algorithm:
 * 1. Select a simple basic track (i.e. set NSEG, radii and lengths of segments)
 * 2. Replace a straight by a sequence of curves and straights
 * 3. Repeat step 2 until the track is complex enough
 * 4. If some segments overlap, generate another track (goto step 1)
 *
 * History
 *  ver. 0.70 Nov 97 
 *  ver  0.75 Sep 00 - revision
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Henning Klaskala <bm321465@muenchen.org>
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include "misc.h"
#include "track.h"
#include <iostream.h>
#include <fstream.h>

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define TRACKFILENAME "random.trk"
#ifndef PI
   #define PI 3.141592653589793
#endif
#define BASIC_TRACKS 1
#define REPLACEMENT_TYPES 4
#define LMIN 50.0               // minimum length of a straight
#define MIN_RAD_QUOT 1.3        // min. quotient of radii of consecutive curves

//--------------------------------------------------------------------------
//                              T Y P E
//--------------------------------------------------------------------------

struct segm
{
  double radius;   // radius of curves in feet (inner wall), 0 means straight
  double length;   // length of curves in radians, of straights in feet
};

//--------------------------------------------------------------------------
//                          F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Return the length of the track.
 *
 * @param NSEG   (in) number of segments
 * @param width  (in) width of the track
 * @param track  (in) array of segm(s)
 */
static double tracklength(int NSEG, double width, struct segm *track)
{
  int i;
  double l=0;

  for(i=0; i<NSEG; i++)
  {
    if (track[i].radius==0)
    {
      l+=track[i].length;
    }
    else if (track[i].radius<0)
    {
      l-=track[i].length*(track[i].radius-width/2);
    }
    else if (track[i].radius>0)
    {
      l+=track[i].length*(track[i].radius+width/2);
    }
  }
  return l;
}

/**
 * Check if the pointer that was just allocated is null.
 * If true, show an error and exit.
 *
 * @param p  .. (in) a pointer that was just allocated
 */
static void out_of_memory( void *p )
{  
  if( p==NULL )
  { 
    fprintf(stderr, "\nAllocation failed: out of memory!\n");
    exit(1);
  }
  return;
}

/**
 * Generate a random track
 *
 * @param NSEG     (??) the number of segments
 * @param width    (??) the width of the track
 * @param length   (??) the mength of the track
 * @param track    (??) an array of segm(s)
 * @param maxseg   (??) the maximum number of segments
 * @param RMIN     (??) 
 */
static void generate_track(int *NSEG, double width, double length, struct segm *track, int maxseg, double RMIN)
{
  int i=0,j,k;
  double h,l;
  
  switch (r_rand() % BASIC_TRACKS) // select a basic track
  {
  case 0: // choose a polygon shaped track as basic track
    do
    {
      do
      { 
        *NSEG=2*(2+r_rand()%3); // the polygon has 2..4 edges (2=oval)
      }
      while(*NSEG>maxseg);

      track[0].radius=0.0; // even segments are straights; odd curves
      track[0].length=300.0+3*RMIN+r_rand()%(4000/ *NSEG);
      track[1].radius=floor(RMIN+width+r_rand()%(2800/ *NSEG));
      // track[1].length=PI*4/ *NSEG;
      // for(i=2; i<*NSEG; i++)
      // {
      //   track[i].radius=track[i-2].radius;
      //   track[i].length=track[i-2].length;
      // }
      track[1].length=PI/180*(30+r_rand()%120);
      switch( *NSEG )
      {
        case 4: // oval
          track[1].radius=floor(RMIN+r_rand()%(400/ *NSEG));
          track[1].length=PI/180*(150+r_rand()%30);
          track[2].radius=0.0;
          track[2].length=track[0].length;
          track[3].radius=( track[0].length
                            +track[1].radius*tan(track[1].length/2)
                          )
                          /tan(track[1].length/2);
          track[3].length=2*PI-track[1].length;
          break;
        case 6: // triangle
          track[2].radius=track[0].radius;
          track[2].length=track[0].length;
          do
          {
            track[3].radius=floor(RMIN+r_rand()%(2800/ *NSEG));
            track[3].length=(2*PI-track[1].length)/2;
            track[4].radius=0.0;
            {
              double x1,y1,x2,y2,x,y;
              x1 = track[0].length
                 + track[1].radius*tan(track[1].length/2)
                 + track[1].radius*tan(track[1].length/2)*cos(track[1].length)
                 + track[2].length*                       cos(track[1].length)
                 + track[3].radius*tan(track[3].length/2)*cos(track[1].length)
                 + track[3].radius*tan(track[3].length/2)*cos(track[1].length+track[3].length);
              y1 = track[1].radius*tan(track[1].length/2)*sin(track[1].length)
                 + track[2].length*                       sin(track[1].length)
                 + track[3].radius*tan(track[3].length/2)*sin(track[1].length)
                 + track[3].radius*tan(track[3].length/2)*sin(track[1].length+track[3].length);

              x2 =
                 - track[3].radius*tan(track[3].length/2)
                 - track[3].radius*tan(track[3].length/2)*cos(track[3].length);
              y2 = track[3].radius*tan(track[3].length/2)*sin(track[3].length);

              x = x2-x1;
              y = y2-y1;
        
              track[4].length=sqrt(x*x+y*y) * fabs(y)/(-y);
            }
          }
          while( track[4].length < 100.0 );

          track[5].radius=track[3].radius;
          track[5].length=track[3].length;
          break;
      
        case 8: // square
          track[2].radius=0.0;
          track[2].length=300.0+3*RMIN+r_rand()%(4000/ *NSEG);
          track[3].radius=floor(RMIN+r_rand()%(2800/ *NSEG));
          track[3].length=PI-track[1].length;
          track[4].radius=0.0;
          track[4].length=track[0].length;
          track[5].radius=track[1].radius;
          track[5].length=track[1].length;
          track[6].radius=0.0;
          track[6].length=track[2].length;
          track[7].radius=track[3].radius;
          track[7].length=track[3].length;
          break;
      } 
      l=tracklength(*NSEG, width, track);
    }
    while(length>0 && l>length); // track length requested & exceeded?
  }
  
  k=10*maxseg;
  while (*NSEG<=maxseg-4 && --k>0)
  {  
    // replace a straight by a sequence of curves and straights
    double ldiff=0;   // track length change in feet
    double r0=0,r1=0,r2=0,r3=0,r4=0; // radii of new segments
    double l0=0,l1=0,l2=0,l3=0,l4=0; // length of new segments

    switch (r_rand() % REPLACEMENT_TYPES)  // replace this straight with what?
    {
      case 0:
        while(track[i=1+r_rand()%((*NSEG*2)/3)].radius != 0);  // find a straight

        if( track[i].radius != 0.0 )
        {
          break;
        }

        // replace with: curve-straight-curve180-straight-curve
      
        if (track[i].length <= 2*width+4*RMIN)
        {
          break; // straight too short to be replaced
        }
      
        h=track[i].length-width;
        do
        {
          double x1,x2,y1,y2,m1,m2,b1,b2,A,B,C,d;

          r0=-RMIN-r_rand()%(int)floor(1+h);
          r1=0;
          r3=0;
          r4=-RMIN-r_rand()%(int)floor(1+h);
          l0=PI/180*(30+r_rand()%120);
          l1 = r_rand()%400+2*width+LMIN+RMIN*6;
          l2=PI;
          l4=PI-l0;

          x2 = track[i].length - fabs(r0)*sin(l0);
          y2 = fabs(r0) - fabs(r0)*cos(l0);
          x1 = fabs(r4)*sin(l4);
          y1 = fabs(r4) - fabs(r4)*cos(l4);

          m1 = cos(   l4 )/sin(   l4 ); // 1/tan(l4)
          m2 = cos( - l0 )/sin( - l0 ); // 1/tan(-l0)

          if( fabs( m1 ) < 0.1
            || fabs( m2 ) < 0.1 )
          {
            r2 = 0.0;
            continue;
          } 

          m2 = 1/m2;
          b2 = y2 + 1/m2*x2;
          A  = -1/m2;
          B  = -1;
          C  = b2;
          d  = (A*x1+B*y1+C)/sqrt(A*A+B*B);

          l3 = l1 + d;

          // r2=RMIN+r_rand()%(int)floor(1+(h-3*RMIN+r0)/2);

          m1 = 1/m1;
          b1 = y1 - m1*x1;
          A  = m1;
          B  = -1;
          C  = b1;
          d  = (A*x2+B*y2+C)/sqrt(A*A+B*B);

          r2 = l0 > PI/2
             ?  d/2
             : -d/2;
      
        }
        while( fabs(r2) < RMIN
             || r2 > 0.0
             && r2 < RMIN + width
             || l3 < LMIN
             || l1 < LMIN );
      
        if( r2 > 0.0 )
        {
          r2 -= width;
        }
      
        // l1=-r4+r_rand()%800+2*width+LMIN+RMIN*6;
        // l3=l1-r0+r4;
      
        if( r_rand()%2 )
        {
          r0 = -r0;
          r2 = -r2;
          r4 = -r4;
        }
      
        h=r0/track[i-1].radius;
        if (h>1/MIN_RAD_QUOT && h<MIN_RAD_QUOT)
        {
          break;
        }
        h=r4/track[i+1].radius;
        if (h>1/MIN_RAD_QUOT && h<MIN_RAD_QUOT)
        {
          break;
        }
        ldiff= (r0<0.0?width-r0:r0)*l0
             + l1
             + (r2<0.0?width-r2:r2)*l2
             + l3
             + (r4<0.0?width-r4:r4)*l4
             - track[i].length;
        break;

      case 1:
      case 2:
        while(track[i=1+r_rand()%((*NSEG*2)/3)].radius != 0);  // find a straight
      
        if( track[i].radius != 0.0 )
        {
          break;
        }

        // replace with: Rcurve-straight-Lcurve-straight-Rcurve10..120
        // (symmetrical Rcurves and straights)
        if (track[i].length <= 6*RMIN+2*LMIN+2+2*width)
        {
          break; // straight too short to be replaced
        }

        r1=r3=0;
        h=track[i].length/2;
        do
        {
          l0=l4=PI/180*(30+r_rand()%70);
        }
        while(fabs(l0-PI/2) < 0.07);

        l2=2*l0;
        r0=r4=-RMIN-r_rand()%(int)floor(1-RMIN-width+h);
        h-=(width-r0)*sin(l0);
        if (l0 > PI/2)
        {
          l1=l3=width+r_rand()%750+RMIN*6;
        }
        else
        {
          l1=l3=LMIN+r_rand()%(int)floor(1+fabs((h-RMIN)/cos(l0)-LMIN));
        }
        r2=(h-l1*cos(l0))/sin(l2/2);
        h=r0/track[i-1].radius;
        if (h>1/MIN_RAD_QUOT && h<MIN_RAD_QUOT)
        {
          break;
        }
        h=r4/track[i+1].radius;
        if (h>1/MIN_RAD_QUOT && h<MIN_RAD_QUOT)
        {
          break;
        }

        if( r_rand()%2 )
        {
          r0 = -r0;
          r2 = -r2;
          r4 = -r4;
        }

        ldiff=(r2<0.0?width-r2:r2)*l2
             +2*(l3+(r4<0.0?width-r4:r4)*l4)
             -track[i].length;
        break;

      case 3:
        while(track[i=1+r_rand()%((*NSEG*2)/3)].radius == 0);  // find a curve

        if( track[i].radius == 0.0 )
        {
          break;
        }

        // Replace curve with S_curve/Straight/S_curve
        { double delta = fabs(track[i].radius)-RMIN;
		    delta<1?delta=1:0;
          r0 = r4 = +RMIN+r_rand()%(int)floor(delta);
          r1 = r3 = +RMIN+r_rand()%(int)floor(delta);
        }
        r2 = 0.0;
      
        l0 = l4 = PI/180.*30.
           + (r_rand()%100)/100.
           * (PI-PI/180.*30.-track[i].length/2.)
        ;
        l1 = l3 = l0
           + (track[i].radius!=0.0?track[i].length/2.: 0.0);
        l2 = 2.0
           * ( ( fabs(track[i].radius) + width + r0 )
           * sin(track[i].radius!=0.0?track[i].length/2.:0.0)
           - ( r0 + width + r1 )
           * sin((track[i].radius!=0.0?track[i].length/2.:0.0)+l0)
           );

        if( track[i].radius == 0.0 )
        {
          l2 += track[i].length;
        }

        if( l2 < LMIN )
        {
          break;
        }

        if( track[i].radius == 0.0 )
        {
          r_rand()%2 == 0 ? r0 = -r0 : r0;
        }
        else
        {
          r0 *= -fabs(track[i].radius)/track[i].radius;
        }

        r1 *= -fabs(r0)/r0;
        r3 *= -fabs(r0)/r0;
        r4 *=  fabs(r0)/r0;

        ldiff=2.0*(r0<0.0?width-r0:r0)*l0
             +2.0*(r1<0.0?width-r1:r1)*l1
             +l2
             -(track[i].radius != 0.0
              ? track[i].radius < 0.0
              ? width-track[i].radius
              :       track[i].radius
              : 1.0
             )
             *track[i].length;

    }
      
    if (ldiff>0       // ldiff>0 means: a replacement was found
        && (length<0   // length<0 means: we don't care about track length
        || l+ldiff<=length))  // requested track length exceeded?
    {  // replace segment i (a straight) with 5 segments
      *NSEG+=4;
      for(j=*NSEG-1; j-4>i; j--)
      {
          track[j]=track[j-4];
      }
      track[i  ].length=l0;
      track[i+1].length=l1;
      track[i+2].length=l2;
      track[i+3].length=l3;
      track[i+4].length=l4;
      track[i  ].radius=r0;
      track[i+1].radius=r1;
      track[i+2].radius=r2;
      track[i+3].radius=r3;
      track[i+4].radius=r4;
      l+=ldiff;
    }
  }

  if (length>0 && l<length)
  {  // enlarge the track up to the requested length
    double w2=width/2;
    h=(length+PI*width)/(l+PI*width);
    for(i=0; i<*NSEG; i++)
    {
      if (track[i].radius==0)
      {
        track[i].length*=h;
      }
      else if (track[i].radius>0)
      {
        track[i].radius=(track[i].radius+w2)*h-w2;
      }
      else if (track[i].radius<0)
      {
        track[i].radius=(track[i].radius-w2)*h+w2;
      }
    }
  }

  return;
}


/**
 * Check whether the track contains overlapping segments.
 *
 * Idea:  Put lots of dots on the middle of the track!
 * The dots in separate segments normally shouldn't come close.
 * If the distance is smaller than the track width, the 2 segments either
 * overlap or the segments are neighbors...
 *
 * @param NSEG     (??) the number of segments
 * @param width    (??) the width of the track
 * @param track    (??) an array of segm(s)
 * @param limit    (??) 
 */
static int overlaps(int NSEG, double width, struct segm *track, int limit )
{
  int    i, j, ii, jj;
  int    *dots;         // dots[i] is the number of dots in segment i
  double **X, **Y;      // X[i][j], Y[i][j] are coord. of j-th dot in segment i
  double x=0, y=width/2, w=0;
  double xx , yy;
  int    overlap=0;

  dots = new int[NSEG];
  X    = new double *[NSEG];
  Y    = new double *[NSEG];
  out_of_memory(Y);

  for(i=0; i<NSEG; i++)        // put dots on segment i
  {
    double r=track[i].radius;
    double l=track[i].length;
    if (r==0)
    {
      double hx, hy;
      dots[i]=(int)ceil(l/width*8);
      if (dots[i] <= 1)
      {
        dots[i]=2;
      }
      X[i] = new double[ dots[i] ];
      Y[i] = new double[ dots[i] ];
      out_of_memory(Y[i]);
      xx=x; yy=y;
      x+=l*cos(w);
      y+=l*sin(w);
      hx=(x-xx)/(dots[i]-1);
      hy=(y-yy)/(dots[i]-1);
      for(j=0; j<dots[i]; j++)
      {
        X[i][j]=xx+hx*j;
        Y[i][j]=yy+hy*j;
      }
    }
    else
    {
      double xm, ym, w0, w1, PI_2, ll, hh;
      if (r>0)
      {
        PI_2=PI/2;
        ll=l;
      }
      else
      {
        PI_2=-PI/2;
        r=-r;
        ll=-l;
      }
      r+=width/2;
      xm=x+r*cos(w+PI_2);
      ym=y+r*sin(w+PI_2);
      w0=w-PI_2;
      w1=w0+ll;
      w+=ll;

      dots[i]=(int)ceil(r*l/width*8);
      if (dots[i] <= 1)
      {
        dots[i]  = 2;
      }
      X[i] = new double[ dots[i] ];
      Y[i] = new double[ dots[i] ];
      out_of_memory(Y[i]);
      hh=(w1-w0)/(dots[i]-1);
      for(j=0; j<dots[i]; j++)
      {  
        double h=w0+hh*j;
        X[i][j]=xm+r*cos(h);
        Y[i][j]=ym+r*sin(h);
      }
      x=X[i][dots[i]-1];
      y=Y[i][dots[i]-1];
    }
  }

  // Now there are lots of dots on the middle of the track.
  // Next we check whether dots in separate segments come too close.
  for(i=0;   i<NSEG-3; i++)
  {
    for(j=i+3; j<NSEG  ; j++)
    {
      double width2=width*width*1.02;
      double width8=8/width;
      if (i<1 && j>=NSEG-3 && i<j-NSEG+3)
      {
         continue;  // segments i and j are near neighbors, dots may come close
      }

      for(ii=dots[i]-1; ii>=0; ii--)
      {
        int dmin=32767;
        for(jj=dots[j]-1; jj>=0; jj--)
        {
          double dx=X[i][ii]-X[j][jj];
          double dy=Y[i][ii]-Y[j][jj];
          double d2=dx*dx+dy*dy;
          if (d2 >= width2)
          {
            int d=(int)(sqrt(d2)*width8)-8;
            if (d<dmin) dmin=d;
            {
              jj-=d;
            }
          }
          else
          {
            ++overlap;//=1;  // segments i and j probably overlap
            //  cout << '.';
            if( overlap > limit )
            {
              goto End;
            }
          }
        } 
        ii-=dmin;
      }
    }
  }

End:
  cout << overlap << ' ';
  for(i=0; i<NSEG; i++)
  {
    free(X[i]);
    free(Y[i]);
  }
  free(X);
  free(Y);
  free(dots);

  return overlap;
}


/**
 * Write the track in random.trk
 *
 * @param NSEG     (in) the number of segments
 * @param width    (in) the width of the track
 * @param track    (in) an array of segm(s)
 */
static void write_track(int NSEG, double width, struct segm *track)
{
  int i,j;
  double x=0, xmin=0, xmax=0, X_MAX, TRK_STRT_X,
         y=0, ymin=0, ymax=0, Y_MAX, TRK_STRT_Y,
         w=0, length, h;

  length= tracklength(NSEG,width,track);

  for(i=0; i<NSEG; i++)
  {  
    double r=track[i].radius;
    double l=track[i].length;
    if (r==0)
    {
      x+=l*cos(w);
      y+=l*sin(w);
      if (x>xmax) xmax=x; 
      if (x<xmin) xmin=x; 
      if (y>ymax) ymax=y; 
      if (y<ymin) ymin=y; 
    }
    else
    {
      double xm, ym, w0, w1, PI_2, ll;
      if (r>0)
      {
        PI_2=PI/2;
        ll=l;
      }
      else
      {
        PI_2=-PI/2;
        r=-r;
        ll=-l;
      }
      r+=width/2;
      xm=x+r*cos(w+PI_2);
      ym=y+r*sin(w+PI_2);
      w0=w-PI_2;
      w1=w0+ll;
      w+=ll;

      for(j=1; j<=100; j++)
      {
        h=w0+(w1-w0)*j/100;
        x=xm+r*cos(h);
        y=ym+r*sin(h);
        if (x>xmax) xmax=x; 
        if (x<xmin) xmin=x; 
        if (y>ymax) ymax=y; 
        if (y<ymin) ymin=y; 
      }
    }
  }

  TRK_STRT_X=-xmin+width;
  TRK_STRT_Y=-ymin+width/2;
  X_MAX= xmax-xmin+width*2;
  Y_MAX= ymax-ymin+width*2;
  h=(Y_MAX*454/480-X_MAX)/3;

  if (h>0)
  {
      X_MAX=Y_MAX*4/3;
  }
  else
  {
    h=Y_MAX;
    X_MAX=X_MAX*640/454;
    Y_MAX=X_MAX/4*3;
    TRK_STRT_Y+=(Y_MAX-h)/2;
    h=0;
  } 

  FILE * f = fopen(TRACKFILENAME,"w");
  if( f==NULL )
  {
    exitOnError("trackgen.cpp: write_track: unable to create file %s", TRACKFILENAME );
  }

  fprintf(f,"%d\t\tNSEG - the number of track segments\n", NSEG);
  fprintf(f,"%d\t%d\tX_MAX, Y_MAX - maximum value of X, Y to be displayed\n", (int)X_MAX, (int)Y_MAX);
  fprintf(f,"%g\t\twidth - width of track\n\n",width);
  fprintf(f,"%d\t%d\t%g\tTRK_STRT_X, _Y, _ANG - coordinates and angle of right rail starting point\n", (int)(TRK_STRT_X+h), (int)TRK_STRT_Y, 0.0);
  fprintf(f,"%d\t%d\tSCORE_BOARD_X, _Y - coordinates of scoreboard upper left corner\n",               (int)(X_MAX*.787-h), (int)(Y_MAX*.96));
  fprintf(f,"%d\t%d\tLDR_BRD_X, _Y - coordinates of leader board upper left corner\n",                 (int)(X_MAX*.729-h), (int)(Y_MAX*.38));
  fprintf(f,"%d\t%d\tIP_X, IP_Y - coordinates of instrument panel upper left corner\n",                (int)(X_MAX*.714-h), (int)(Y_MAX*.6));
  fprintf(f,"%d\t%d\tLOTIX, LOTIY - coordinates of track length message\n",                            (int)(X_MAX*.59-h), (int)(Y_MAX*.03));
  fprintf(f,"%g\t%d\tFINISH, SR - fraction of 1st segment prior to finish line; Starting rows\n\n",.8,2);

  fprintf(f,"%d\t\tPIT_SIDE - left (inside)\n", 1);
  fprintf(f,"%d\t%d\tPIT_ENTRY, PITLANE_START - in feets from SF-line\n", (int)length-800, (int)length-300);
  fprintf(f,"%d\t%d\tPITLANE_END, PIT_EXIT - in feets from SF-line\n", 100, 300); 
  fprintf(f,"%d\t\tPITLANE_SPEED - feets per second (approx km/h)\n\n",40); 
         
  for(i=0; i<NSEG; i++) 
  {
    if (track[i].radius > 0)
    {
      fprintf(f,"%.6g\t%.7g\n", track[i].radius+width,       track[i].length/PI*180);
    }
    else if (track[i].radius < 0)
    {
      fprintf(f,"%.6g\t%.7g\n", track[i].radius,     track[i].length/PI*180);
    }
    else if (track[i].radius == 0)
    {
      fprintf(f,"0\t%.7g\n",                         track[i].length);
    }
  }
  fprintf(f,"\nRADIUS\tLENGTH  - NSEG repetitions of segment radius and length:"); 
  fprintf(f,"\n\t\t (0 radius means a straightaway with length in feet. Length"); 
  fprintf(f,"\n\t\t of curves is in degrees. Negative radius means right turn.)\n"); 
  fprintf(f,"\nTrack length: %.4g miles.\n", length/5280); 
  fclose(f);

  return;
}

/**
 * Generate random track and write it to a file
 */
void Track::generateRandomTrack()
{
  int    NSEG;                            // number of track segments
  int    maxseg=20+r_rand()%29;           // maximum number of track segments
  double width=100;   //75.0+r_rand()%26; // track width in feet
  double length=3000+r_rand()%18000;      // requested track length in feet (length<0: don't care)
  double minrad=10;                       // minimum radius of curves (inner wall)
  struct segm * track;
  int o;

  cout << length/5280. << ' '
       << width  << ' '
       << maxseg << ' ';

  track = new segm[maxseg];
  out_of_memory(track);

  do
  {
    generate_track(&NSEG,width,length,track,maxseg,minrad); // generate track
  }
  while((o=overlaps(NSEG, width, track, (int)(length*0.05) ))
         >
         (length<20.0?0.0:length)*0.01
       );      // repeat until no segments overlap

  write_track(NSEG, width, track);     // write the track to file "random.trk"

  return;
}

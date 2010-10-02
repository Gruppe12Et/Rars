/**
 * TRACK.CPP - Race Course definition  
 *
 * History
 *  ver  0.1 release January 12, 1995
 *  ver  0.2 1/23/95
 *  ver  0.3 2/7/95
 *  ver. 0.39 3/6/95
 *  ver. 0.46 3/25/95
 *  ver. 0.6b 5/8/95 b for beta 
 *  ver. 0.61 May 95 
 *  ver. 0.70 Nov 97 - added pit stop variables to track file 
 *  ver  0.72 Apr 98 - track records
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @see       CAR.H & TRACK.H for class and structure declarations 
 * @version   0.76
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include <iostream>
#include <stdlib.h>
#include <string.h>
#include <math.h>               // fabs()
#include <iomanip.h>
#include "track.h"
#include "draw.h"               // needed only for resume_text_display()
#include "os.h"
#include "misc.h"
#include "xml/xmlparse.h"       // expat Xml parser 
#include "graphics/g_define.h"  // min/max

using namespace std;

//--------------------------------------------------------------------------
//                            D E F I N E S
//--------------------------------------------------------------------------

#define XML_BUFFER_SIZE 65536

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

// Race Records
static fastest_lap records[200];    // Array of track records
static int number_of_records = 0;   // number of records in records.dat

// Global variable describing the current Track
Track * currentTrack = NULL;

// Profile with height
// The end of a profile is detected by a 0 or 1
ProfileHeight g_aProfileHeight[] =
{
  { "flat", { 0, 1 } },
  { "hill", { 0, 0.04, 0.2, 0.5, 0.8, 0.96, 0.999, 0.96, 0.8, 0.5, 0.2, 0.04, 0 } },
  { "s",    { 0, 0.04, 0.2, 0.5, 0.8, 0.96, 1 } },
  { NULL }
};

// Variables used for the Reflection
static Track g_track;
static Segment3D g_seg3D;
static SegmentSide3D g_segSide;
static Object3D g_object3D;
static double g_fDistanceScale = 1;

/**
 * List of the properties of a segment. This list is used mostly to read the
 * Xml file and for the track editor
 */

ReflectionProperty g_aTrackInfoProp[] =
{ 
  { "name",               &(g_track.m_sName),               &g_track,    T_STRING,   "Name of the track (<>finename)" },
  { "version",            &(g_track.m_iVersion),            &g_track,    T_INT,      "Version of the track" },
  { "author",             &(g_track.m_sAuthor),             &g_track,    T_STRING,   "Author of the track" },
  { "description",        &(g_track.m_sDescription),        &g_track,    T_STRING,   "Description of the track" },
  { NULL }
};

ReflectionProperty g_aTrackUnitProp[] =
{ 
  { "distance_name",      &(g_track.m_sUnitDistanceName),   &g_track,    T_STRING,   "Name of the distance unit" },
  { "distance_scale",     &(g_track.m_fUnitDistanceScale),  &g_track,    T_DOUBLE,   "Number of (Rars) feets in a distance unit" },
  { NULL }
};

ReflectionProperty g_aTrackHeaderProp[] =
{ 
  { "width",              &(g_track.width),                 &g_track,    T_DISTANCE, "Width of the track" },
  { "finish",             &(g_track.m_fFinish),             &g_track,    T_DOUBLE,   "Position of the finish line on the first segment" },
  { "start_rows",         &(g_track.m_iStartRows),          &g_track,    T_INT,      "Number of car per line at the start" },
  { "start_x",            &(g_track.m_fRgtStartX),          &g_track,    T_DISTANCE, "Position X of the right side of the first segment" },
  { "start_y",            &(g_track.m_fRgtStartY),          &g_track,    T_DISTANCE, "Position Y of the right side of the first segment" },
  { "start_ang",          &(g_track.m_fStartAng),           &g_track,    T_ANGLE,    "Angle of the first segment" },
  { NULL }
};

ReflectionProperty g_aTrackXWindowProp[] =
{ 
  { "score_board_x",      &(g_track.m_fScoreBoardX ),       &g_track,    T_DOUBLE,   "Position X of the scoreboard" },
  { "score_board_y",      &(g_track.m_fScoreBoardY ),       &g_track,    T_DOUBLE,   "Position Y of the scoreboard" },
  { "leader_board_x",     &(g_track.m_fLeaderBoardX ),      &g_track,    T_DOUBLE,   "Position X of the leader board" },
  { "leader_board_y",     &(g_track.m_fLeaderBoardY ),      &g_track,    T_DOUBLE,   "Position Y of the leader board" },
  { "inst_panel_x",       &(g_track.m_fInstPanelX ),        &g_track,    T_DOUBLE,   "Position X of the instrument panel" },
  { "inst_panel_y",       &(g_track.m_fInstPanelY ),        &g_track,    T_DOUBLE,   "Position Y of the instrument panel" },
  { "message_x",          &(g_track.m_fMessageX ),          &g_track,    T_DOUBLE,   "Position X of the messages" },
  { "message_y",          &(g_track.m_fMessageY ),          &g_track,    T_DOUBLE,   "Position Y of the messages" },
  { NULL }
};

ReflectionProperty g_aTrackPitstopProp[] =
{ 
  { "side",               &(g_track.m_iPitSide ),           &g_track,    T_INT,      "Side if the pitstop (1 is right side; left side is -1)" },
  { "entry",              &(g_track.m_fPitEntry ),          &g_track,    T_DOUBLE,   "Where car starts steering to pits" },
  { "exit",               &(g_track.m_fPitExit ),           &g_track,    T_DOUBLE,   "Where car is back on the track" },
  { "lane_start",         &(g_track.m_fPitLaneStart ),      &g_track,    T_DOUBLE,   "lane_start-end denote stopping area" },
  { "lane_end",           &(g_track.m_fPitLaneEnd ),        &g_track,    T_DOUBLE,   "lane_start-end denote stopping area" },
  { "lane_speed",         &(g_track.m_fPitLaneSpeed ),      &g_track,    T_DOUBLE,   "speed in the pitstop (feet per second)" },
  { NULL }
};

ReflectionProperty g_aSegProp[] =
{ 
  { "length",             &(g_seg3D.m_fLength),             &g_seg3D,    T_DISTANCE   , "Straight: Length of the segment" },
  { "radius",             &(g_seg3D.m_fRadius),             &g_seg3D,    T_DISTANCE   , "Curve: Radius (<0 right curve) (0=straight) (>0 left curve)" },
  { "arc",                &(g_seg3D.m_fArc),                &g_seg3D,    T_ANGLE      , "Curve: Arc of the segment" },
  { "end_z",              &(g_seg3D.m_fEndZ),               &g_seg3D,    T_DISTANCE   , "Height of the end of the segment" },
  { "end_banking",        &(g_seg3D.m_fEndBanking),         &g_seg3D,    T_ANGLE      , "Banking at the end of the segment" },
  { "name",               &(g_seg3D.m_sName),               &g_seg3D,    T_STRING     , "Name of the segment (Ex. for Spa: raidillon, la source, ...)" },
  { "hill_height",        &(g_seg3D.m_fHillHeight),         &g_seg3D,    T_DISTANCE   , "Height of a hill (profile_height=hill)" },
  { "profile_height",     &(g_seg3D.m_sProfileHeight),      &g_seg3D,    T_STRING     , "Height Profile of the segment (flat, s, hill)" },
  { "type",               &(g_seg3D.m_sType),               &g_seg3D,    T_STRING     , "Type of the segment (default, y, yinv)" },
  { "section",            &(g_seg3D.m_sSection),            &g_seg3D,    T_STRING     , "Name of the section for segment with a type equal to y or yinv" },
  { "model",              &(g_seg3D.m_sModel),              &g_seg3D,    T_STRING     , "3DS Model for special segments where we do not want to use the 3D model\ngenerated automatically by Rars (3DS filename without .3ds extension)" },
  { NULL }
};

ReflectionProperty g_aSegSideProp[] =
{ 
  { "border_size",        &(g_segSide.m_fBorderSize),       &g_segSide,  T_DISTANCE   , "Size of the border between the road and the barrier" },
  { "barrier_height",     &(g_segSide.m_fBarrierHeight),    &g_segSide,  T_DISTANCE   , "Height of the barrier" },
  { "border_tree_size",   &(g_segSide.m_fBorderTreeSize),   &g_segSide,  T_DISTANCE   , "Size of the border between the barrier and the tree" },
  { "border_tree_height", &(g_segSide.m_fBorderTreeHeight), &g_segSide,  T_DISTANCE   , "Height difference between the barrier and the tree" },
  { "show_tree",          &(g_segSide.m_bShowTree),         &g_segSide,  T_BOOL       , "Show or not the tree" },
  { "show_border_tree",   &(g_segSide.m_bShowBorderTree),   &g_segSide,  T_BOOL       , "Show or not the border tree" },
  { "texture_border",     &(g_segSide.m_sTextureBorder),    &g_segSide,  T_STRING     , "Texture used for the border" },
  { "texture_barrier",    &(g_segSide.m_sTextureBarrier),   &g_segSide,  T_STRING     , "Texture used for the barrier" },
  { "texture_border_tree",&(g_segSide.m_sTextureBorderTree),&g_segSide,  T_STRING     , "Texture used for the border between the barrier and the tree" },
  { "texture_tree",       &(g_segSide.m_sTextureTree),      &g_segSide,  T_STRING     , "Texture used for the trees" },
  { NULL }
};

ReflectionProperty g_aObject3DProp[] =
{ 
  { "model",              &(g_object3D.m_sModel),           &g_object3D, T_STRING     , "Name of the object (3ds file name without .3ds extension)" },
  { "x",                  &(g_object3D.x),                  &g_object3D, T_DISTANCE   , "Position of the object X" },
  { "y",                  &(g_object3D.y),                  &g_object3D, T_DISTANCE   , "Position of the object Y" },
  { "z",                  &(g_object3D.z),                  &g_object3D, T_DISTANCE   , "Position of the object Z" },
  { "rot_x",              &(g_object3D.rot_x),              &g_object3D, T_ANGLE      , "Rotation of the object X" },
  { "rot_y",              &(g_object3D.rot_y),              &g_object3D, T_ANGLE      , "Rotation of the object Y" },
  { "rot_z",              &(g_object3D.rot_z),              &g_object3D, T_ANGLE      , "Rotation of the object Z" },
  { NULL }
};

//--------------------------------------------------------------------------
//                           F U N C T I O N S
//--------------------------------------------------------------------------

/**
 * Dummy constructor (used by reflection only)
 */
Track::Track()
{
  Init();
}

void Track::Init()
{
  m_sName[0] = 0;
  m_sAuthor[0] = 0;
  m_sDescription[0] = 0;
  m_iVersion = 0;

  strcpy( m_sUnitDistanceName, "feet" );
  m_fUnitDistanceScale = 1;
  m_fFinish = 0;
  m_iStartRows = 2;
  m_fRgtStartX = m_fRgtStartY = m_fStartAng = 0;

  m_iPitSide = 0;
  m_fPitEntry = m_fPitExit = 0.0;
  m_fPitLaneStart = m_fPitLaneEnd = 0.0;
  m_fPitLaneSpeed = 0.0;

  m_fXMax = m_fYMax = 0;
  m_fScoreBoardX = m_fScoreBoardY = 0;
  m_fInstPanelX = m_fInstPanelY = 0;
  m_fLeaderBoardX = m_fLeaderBoardY = 0;
  m_fMessageX = m_fMessageY = 0;

  rgtwall = NULL;
  lftwall = NULL;
  NSEG = 0;
  m_iNumSegment = 0;
  m_iNumObject3D = 0;
  m_iNumSection = 0;
  m_bCopy = false;

  strcpy( m_oSky.m_sModel, "model/sky.3ds" );
}

/*
 * Default constructor for Track - load trackdata from fileName
 *
 * @ fileName: File to load track from (relative to rars/rars/track )
 */
Track::Track(char* file)
{
  Init();

  // Read the file
  strcpy(m_sFileName, file);
  chdir("tracks");

  // Generate a random track if requested and save as random.trk
  bool random = (args.m_iMovieMode != MOVIE_PLAYBACK) && (!strcmp(m_sFileName, "random") || !strcmp(m_sFileName, "random.trk") );
  if( random )
  {
    generateRandomTrack();
  }

  if( strstr(m_sFileName,".trx") || strstr(m_sFileName,".TRX") ) 
  {
    // TRX = XML Format
    LoadXml( m_sFileName );
  }
  else
  {
    // TRK = Format version 0.8x and before
    ifstream inf(m_sFileName); // open the track file for input
    static int degrees; // will become 1 for degrees, 0 for radians
    degrees = -1;
    int i, l;

    if(!inf)                // If can't open file, & if filename had no extension,
    {        
      for(i=0; i<8; i++)  // append .trk and see if that works
      {
        if(m_sFileName[i] == '\0')
        {
          break;
        }
        else if(m_sFileName[i] == '.')
        {
          if(m_sFileName[i+1] == '\0')
          {
            break;
          }
          else
          {
            chdir("..");
            exitOnError("There is no trackfile. Tried to find '%s'", m_sFileName);
          } 
        }
      }
      m_sFileName[i++] = '.';
      m_sFileName[i++] = 't';
      m_sFileName[i++] = 'r';
      m_sFileName[i++] = 'k';
      m_sFileName[i] = '\0';
      inf.open(m_sFileName);
      if(!inf)
      {
        chdir("..");
        exitOnError("There is no trackfile. Tried to find '%s'", m_sFileName);
      }
    }
    else   // insist that the file have the .trk extension
    {   
      l = strlen(m_sFileName);
      if(strcmp(&m_sFileName[l-4], ".trk") && strcmp(&m_sFileName[l-4], ".TRK") )
      {
        // draw.ResumeNormalDisplay();
        chdir(".."); // go back to main directory
        exitOnError("Track data file must have .TRK extension.  :(");
      }
    }

    char buffer[128];
    inf.get( buffer, 128, '\n' );
    sscanf( buffer, "%d", &NSEG );
    inf.ignore(120, '\n');        // ignore rest of line, up to 120 chars
    inf >> m_fXMax >> m_fYMax;
    inf.ignore(120, '\n');
    inf >> width;                 // read the header section:
    inf.ignore(120, '\n');
    inf >> m_fRgtStartX >> m_fRgtStartY >> m_fStartAng;
    inf.ignore(120, '\n');
    inf >> m_fScoreBoardX >> m_fScoreBoardY;
    inf.ignore(120, '\n');
    inf >> m_fLeaderBoardX >> m_fLeaderBoardY;
    inf.ignore(120, '\n');
    inf >> m_fInstPanelX >> m_fInstPanelY;
    inf.ignore(120, '\n');
    inf >> m_fMessageX >> m_fMessageY;
    inf.ignore(120, '\n');
    inf >> m_fFinish >> m_iStartRows;
    inf.ignore(120, '\n');
    inf >> m_iPitSide;
    inf.ignore(120, '\n');
    inf >> m_fPitEntry >> m_fPitLaneStart;
    inf.ignore(120, '\n');
    inf >> m_fPitLaneEnd >> m_fPitExit;
    inf.ignore(120, '\n');
    inf >> m_fPitLaneSpeed;
    inf.ignore(120, '\n');

    // define full name of your rars directory for Mac!

    if( args.m_iStartRows )
    {
      m_iStartRows = args.m_iStartRows;
    }
    // command line overrides track file settings for number of start rows

    // Create table rgtwall/leftwall/seg3d
    m_iNumSegment = NSEG;
    AllocSegment();

    for( i=0; i<m_iNumSegment; i++ )           // read the segment data:
    {
      inf >> m_aSeg[i].m_fRadius;
      if( m_aSeg[i].m_fRadius==0.0 )
      {
        inf >> m_aSeg[i].m_fLength;
      }
      else
      {
        inf >> m_aSeg[i].m_fArc;
      }
      inf.ignore(120, '\n');
      // this section of the loop handles (degree vs. radians):
      if(degrees == -1 && m_aSeg[i].m_fRadius != 0.0)
      {
        if(m_aSeg[i].m_fArc < 5.0)
        {
          degrees = 0;
        }
        else
        {
          degrees = 1;
        }
      }
      if(degrees == 1 && m_aSeg[i].m_fRadius != 0.0)
      {
        m_aSeg[i].m_fArc /= DEGPRAD;
      }
    }
    // Section
    m_iNumSection = 1;
    strcpy( m_aSection[0].m_sName, "track" );
    m_aSection[0].m_iFirstSeg = 0;
    m_aSection[0].m_iLastSeg = NSEG-1;
  }

  chdir(".."); // go back to main directory

  Rebuild();
}

/**
 * Destructor
 */
Track::~Track()
{
  if( rgtwall && !m_bCopy )
  {
    delete rgtwall;
    delete lftwall;
    delete m_aSeg;
    delete seg_dist;

    rgtwall = NULL;
  }
}

/**
 * Allocate segment
 */
void Track::AllocSegment()
{
  rgtwall  = new segment[m_iNumSegment];
  lftwall  = new segment[m_iNumSegment];
  m_aSeg   = new Segment3D[m_iNumSegment];
  seg_dist = new double[m_iNumSegment];
}

/**
 * Rebuild the lftwall, rgtwall from m_aSeg
 */
void Track::Rebuild()
{
  int i;

  // ShortName
  strcpy( m_sShortName, m_sFileName );
  char * p = strchr( m_sShortName, '.' );
  *p = 0;

  g_fDistanceScale = m_fUnitDistanceScale;
  NSEG = m_aSection[0].m_iLastSeg+1;

  // lftwall can be reallocated in the track editor
  if( lftwall!=NULL )
  {
    delete lftwall;
    delete seg_dist;
  }
  lftwall = new segment[m_iNumSegment];
  seg_dist = new double[m_iNumSegment];

  // Reset the remumbering (needed for AddSegment/DeleteSegment)
  for( i=0; i<m_iNumSegment; i++ )
  {
    m_aSeg[i].m_iPrvSeg = -1;
    m_aSeg[i].m_iNxtSeg = -1;
    m_aSeg[i].m_iOtherYSeg = -1;
  }
      
  for( i=0; i<m_iNumSegment; i++ )
  {
    // Fill in lftwall[] from rgtwall[]
    rgtwall[i].radius = m_aSeg[i].m_fRadius;
    if(rgtwall[i].radius == 0.0)
    {
      lftwall[i].radius = 0.0;
      rgtwall[i].length = lftwall[i].length = m_aSeg[i].m_fLength;
    }
    else
    {
      lftwall[i].radius = rgtwall[i].radius - width;
      rgtwall[i].length = lftwall[i].length = m_aSeg[i].m_fArc;
    }

    // Rebuild seg3D
    m_aSeg[i].Rebuild();

    // Previous segment
    int prv_seg = i-1;
    int nxt_seg = i+1;
    if( m_aSeg[i].m_iPrvSeg<0 )
    {
      if( i==0 )
      {
        prv_seg = NSEG-1;
        m_aSeg[NSEG-1].m_iNxtSeg = 0;
      }
      m_aSeg[i].m_iPrvSeg = prv_seg ;
    }
    if( m_aSeg[i].m_iNxtSeg<0 )
    {
      int iSection = GetSectionId( i );
      if( i!=m_aSection[iSection].m_iLastSeg )
      {
        m_aSeg[i].m_iNxtSeg = nxt_seg ;
      }
    }
    if( strcmp(m_aSeg[i].m_sType, "y" )==0 )
    {
      for( int j=0; j<m_iNumSection; j++ )
      {
        if( strcmp(m_aSeg[i].m_sSection, m_aSection[j].m_sName )==0 )
        {
          m_aSeg[i].m_iOtherYSeg = m_aSection[j].m_iFirstSeg;
          m_aSeg[m_aSection[j].m_iFirstSeg].m_iOtherYSeg = i;
          m_aSeg[m_aSection[j].m_iFirstSeg].m_iPrvSeg = prv_seg ;
        }
      }
    }
    else if( strcmp(m_aSeg[i].m_sType, "yinv" )==0 )
    {
      for( int j=0; j<m_iNumSection; j++ )
      {
        if( strcmp(m_aSeg[i].m_sSection, m_aSection[j].m_sName )==0 )
        {
          m_aSeg[i].m_iOtherYSeg = m_aSection[j].m_iLastSeg;
          m_aSeg[m_aSection[j].m_iLastSeg].m_iOtherYSeg = i;
          m_aSeg[m_aSection[j].m_iLastSeg].m_iNxtSeg = nxt_seg;
        }
      }
    }
  }

  length = 0;
  for( i=0; i<NSEG; i++ )
  {
    // Track length determined here
    if( lftwall[i].radius )
    {
      length += fabs(lftwall[i].length*(lftwall[i].radius + rgtwall[i].radius)/2);
    }
    else
    {
      length += lftwall[i].length;
    }
  }


  if(args.m_iRaceLength) // if lap count is not explicitly declared use race length 
  {
    // This is an exception in args. (an "arg" is normally known before the start of the runtime)
    // But here, the length is not known before the track is read...
    args.m_iNumLap = long(args.m_iRaceLength*5280/length) + 1; // to find lap_count
  }

  // initialize these global variables:
  from_start_to_seg1 = (1.0 - m_fFinish) * rgtwall[0].length;
  m_fLftStartX = m_fRgtStartX - width*sin(m_fStartAng); //lftwall
  m_fLftStartY = m_fRgtStartY + width*cos(m_fStartAng);
  finish_rx = m_fRgtStartX + m_fFinish * rgtwall[0].length * cos(m_fStartAng);// rgt
  finish_ry = m_fRgtStartY + m_fFinish * rgtwall[0].length * sin(m_fStartAng);
  finish_lx = finish_rx - width*sin(m_fStartAng); //lftwall
  finish_ly = finish_ry + width*cos(m_fStartAng);

  // Find additional members of segment structure from known data:
  track_setup( m_fRgtStartX, m_fRgtStartY, m_fStartAng, rgtwall );
  track_setup( m_fLftStartX, m_fLftStartY, m_fStartAng, lftwall );
  CalcMinMax();

  // Calculate distance to end of each track segment.
  // Distance and seg_dist counting starts from SF lane in ver.0.70!
  // This is required for calculating "distance" that is part of car
  // object and is used for nearby_cars in check_nearby
  // and for locating pit entry, pitlane and pit exit positions
  // and for arranging cars on their starting positions
  for( i=0; i<NSEG; i++ ) 
  {
    if( i ) // not first segment
    {
       seg_dist[i] = seg_dist[i-1]; // take old value...
    }
    else  // first segment
    {
      seg_dist[i] = -m_fFinish*lftwall[0].length;
    }
     
    if( lftwall[i].radius )   // and add length of this segment in feets!
    {
       seg_dist[i] += fabs(lftwall[i].length *
               (lftwall[i].radius + rgtwall[i].radius)/2);
    }
    else                     // and add length of this segment in feets!
    {
      seg_dist[i] += lftwall[i].length;
    }
  }

  // precalculate the track description
  strcpy( m_oTrackDesc.sName, m_sFileName ); // track filename
  m_oTrackDesc.NSEG = NSEG;
  m_oTrackDesc.width = width;
  m_oTrackDesc.length = length;           // drivers can get length from here
  m_oTrackDesc.rgtwall = rgtwall;         // replaces trackout
  m_oTrackDesc.lftwall = lftwall;         // replaces trackin
  m_oTrackDesc.trackout = rgtwall;
  m_oTrackDesc.trackin = lftwall;
  m_oTrackDesc.seg_dist = seg_dist;       // distance from SF to the end of each segment
  m_oTrackDesc.pit_side = m_iPitSide;     // car needs to know pit location
  m_oTrackDesc.pit_entry = m_fPitEntry;   // to avoid entering and exiting cars
  m_oTrackDesc.pit_exit = m_fPitExit;     // and to calculate estimated pit times
  m_oTrackDesc.pit_speed = m_fPitLaneSpeed; // mph
}

/**
 * Fills in the un-initialized portions of the segment array.
 *
 * @param xstart    (in) coordinates of starting point
 * @param ystart    (in) coordinates of starting point
 * @param alfstart  (in) starting tangent angle 
 * @param seg       (in) pointer to structure that defines path
 */
void Track::track_setup(double xstart, double ystart, double angstart, segment *seg)
{
  double cenx, ceny;          // center of circle arc
  double radius;              // radius of circle arc (negative == rt. turn)
  double x, y, ang;           // position and direction of start of segment
  double newx, newy, newang;  // and the one after that  (alf in radians)
  int i;

  seg[NSEG-1].end_x   = xstart;  
  seg[NSEG-1].end_y   = ystart;  
  seg[NSEG-1].end_ang = angstart;  // store starting point & direction

  for( i=0; i<m_iNumSegment; i++ )                 // for each segment:
  {    
    int prv = m_aSeg[i].m_iPrvSeg;
    x   = seg[prv].end_x;
    y   = seg[prv].end_y;
    ang = seg[prv].end_ang;
    radius = seg[i].radius;
    if(radius == 0.0)         // is this a straightaway?
    {                   
      newx = x + seg[i].length * cos(ang);         // find end coordinates
      newy = y + seg[i].length * sin(ang);
      newang = ang;                                // direction won't change
    }
    else 
    {
      if(radius > 0.0) 
      {
        cenx = x - radius * sin(ang);              // compute center location:
        ceny = y + radius * cos(ang);
        newang = ang + seg[i].length;              // compute new direction
        if(newang > 2.0 * PI)
        {
          newang -= 2.0 * PI;
        }
      }
      else 
      {
        cenx = x - radius * sin(ang);              // compute center location:
        ceny = y + radius * cos(ang);
        newang = ang - seg[i].length;              // compute new direction
        if(newang < -2.0 * PI)
        {
          newang += 2.0 * PI;
        }
      }
      seg[i].cen_x = cenx;   seg[i].cen_y = ceny;
      newx = cenx + radius * sin(newang);          
      newy = ceny - radius * cos(newang);
    }
    seg[i].beg_ang = ang;
    seg[i].beg_x = x;      seg[i].beg_y = y;       
    seg[i].end_ang = newang;                       
    seg[i].end_x = newx;   seg[i].end_y = newy;    
  }
}


//---------------------------------------------------------------------------
// Load XML
//---------------------------------------------------------------------------

struct XmlStack 
{
  int iDepth;
  const char * aStack[10];
};

static XmlStack xml_stack;
static Segment3D temp_seg3D[MAX_SEGMENT];   // 
static Segment3D * current_seg3D = NULL;    // Need to be static for right/left tag

/**
 * Start an element when reading a XML file
 */
static void startElement(void * userData, const char * name, const char ** atts)
{
  Object3D * current_object = NULL; 
  Section * current_section = NULL; 

  xml_stack.aStack[xml_stack.iDepth] = name;
  xml_stack.iDepth ++;

  Track * track = (Track *) userData;

  // Elements
  if( strcmp( name, "segment" )==0 )
  {      
    current_seg3D = &(temp_seg3D[track->m_iNumSegment]);

    track->m_iNumSegment++; 
    if( track->m_iNumSegment>MAX_SEGMENT )
    {
      exitOnError( "Track::LoadXml (startElement): MAX_SEGMENT too small %d", MAX_SEGMENT );
    }
    // Initialise the segment with the default values
    *current_seg3D = track->m_oDefault;
  }
  else if( strcmp( name, "default" )==0 )
  {
    current_seg3D = &(track->m_oDefault);
  }
  else if( strcmp( name, "object" )==0 )
  {
    current_object = &(track->m_aObject3D[track->m_iNumObject3D]);

    track->m_iNumObject3D++;
    if( track->m_iNumObject3D>MAX_OBJECT3D )
    {
      exitOnError( "Track::LoadXml (startElement): MAX_OBJECT3D too small %d", MAX_OBJECT3D );
    }
  }
  else if( strcmp( name, "section" )==0 )
  {
    current_section = &(track->m_aSection[track->m_iNumSection]);

    track->m_iNumSection++;
    if( track->m_iNumSection>MAX_SECTION )
    {
      exitOnError( "Track::LoadXml (startElement): MAX_SECTION too small %d", MAX_SECTION );
    }
  }

  SegmentSide3D * current_segSide3D = NULL; 
  if( strcmp( name, "right" )==0 )
  {
    current_segSide3D = &(current_seg3D->rgt);
  }
  else if( strcmp( name, "left" )==0 )
  {
    current_segSide3D = &(current_seg3D->lft);
  }
  
  // Attributes
  for( int i=0; atts[i]!=NULL; i+=2 )
  {
    const char * param_name = atts[i];
    const char * param_value = atts[i+1];
    
    if( param_value==NULL )
    {
      warning( "Track::LoadXml (startElement): param_name (%s) without param_value", param_name ); 
      continue;
    }

    if( strcmp( name, "track" )==0 )
    {
      // no attributes
    }
    else if( strcmp( name, "info" )==0 )
    {
      ReflectionProperty::ReadXml( g_aTrackInfoProp, track, param_name, param_value );
    }
    else if( strcmp( name, "unit" )==0 )
    {
      ReflectionProperty::ReadXml( g_aTrackUnitProp, track, param_name, param_value );
    }
    else if( strcmp( name, "header" )==0 )
    {
      ReflectionProperty::ReadXml( g_aTrackHeaderProp, track, param_name, param_value );
    }
    else if( strcmp( name, "xwindow" )==0 )
    {
      ReflectionProperty::ReadXml( g_aTrackXWindowProp, track, param_name, param_value );
    }
    else if( strcmp( name, "pitstop" )==0 )
    {
      ReflectionProperty::ReadXml( g_aTrackPitstopProp, track, param_name, param_value );
    }
    else if( strcmp( name, "segments" )==0 )
    {
      // no attributes
    }
    else if( strcmp( name, "section" )==0 )
    {
      if( strcmp( param_name, "name" )==0 )
      {
        strcpy( current_section->m_sName, param_value );
        current_section->m_iFirstSeg = track->m_iNumSegment;
      }
    }
    else if( strcmp( name, "segment" )==0 )
    {      
      ReflectionProperty::ReadXml( g_aSegProp, current_seg3D, param_name, param_value );
    }
    else if( strcmp( name, "right" )==0 || strcmp( name, "left" )==0 )
    {      
      ReflectionProperty::ReadXml( g_aSegSideProp, current_segSide3D , param_name, param_value );
    }
    else if( strcmp( name, "object" )==0 )
    {      
      ReflectionProperty::ReadXml( g_aObject3DProp, current_object, param_name, param_value );
    }
  }
}

static void endElement(void * userData, const char * name)
{
  Track * track = (Track *) userData;

  if( strcmp( name, "unit" )==0 )
  {
    g_fDistanceScale = track->m_fUnitDistanceScale;
  }
  else if( strcmp( name, "section" )==0 )
  {
    track->m_aSection[track->m_iNumSection-1].m_iLastSeg = track->m_iNumSegment-1;
  }
  
  // XML stack
  xml_stack.iDepth --;
}

/*
 * Load the track
 *
 * @ fileName : Track filename (relative to rars/rars/track )
 */
void Track::LoadXml( char * sFileName )
{
  int done = 0;
  char buf[XML_BUFFER_SIZE];

  FILE * in = fopen( sFileName, "r");
  if( in==NULL )
  {
    exitOnError( "Track::LoadXml: unable to open the file %s",sFileName );
  }

  int iNumRead = fread( buf, sizeof( char ), XML_BUFFER_SIZE, in );
  if( iNumRead>=XML_BUFFER_SIZE )
  {
    exitOnError( "Track::LoadXml: buffer is too small" );
  }
  else if( iNumRead==0 )
  {
    exitOnError( "Track::LoadXml: file is empty" );
  }

  fclose( in );

  XML_Parser parser = XML_ParserCreate(NULL);
  XML_SetUserData(parser, this);
  XML_SetElementHandler(parser, startElement, endElement);

  if( !XML_Parse(parser, buf, iNumRead, done) ) 
  {
    exitOnError( "LoadXML : %s at line %d",
	    XML_ErrorString(XML_GetErrorCode(parser)),
	    XML_GetCurrentLineNumber(parser)
    );
  }

  XML_ParserFree(parser);

  // Copy temp_segment in rgtwall
  AllocSegment();    
  int i;
  for( i=0; i<m_iNumSegment; i++ )
  {
    m_aSeg[i] = temp_seg3D[i];
  }
}

//---------------------------------------------------------------------------
// Save XML
//---------------------------------------------------------------------------

/*
 * Save the track
 */
int Track::SaveXml()
{
  int i, j;
  Segment3D builtin_default;  // Default of the default :builtin default of the executable
  Object3D object3D_default; 

  if( !strstr(m_sFileName,".trx") ) 
  {
    warning( "Track::Save: Track name has no .trx extension '%s'", m_sFileName );
    return 1;
  }

  chdir("tracks");
  FILE * out = fopen( m_sFileName, "w" );
  chdir(".."); // go back to main directory

  if(!out)
  {
    warning( "Track::Save: Can not create file '%s'", m_sFileName );
    return 1;
  }

  m_iVersion ++;

  fprintf( out, "<?xml version=\"1.0\"?>\n" );
  fprintf( out, "<track>\n\n" );
  fprintf( out, "  <info" );
  ReflectionProperty::WriteXml( out, g_aTrackInfoProp, this, &g_track, "\n    " );
  fprintf( out, "\n  />\n\n" );
  fprintf( out, "  <unit" );
  ReflectionProperty::WriteXml( out, g_aTrackUnitProp, this, &g_track, "\n    " );
  fprintf( out, "\n  />\n\n" );
  fprintf( out, "  <header" );
  ReflectionProperty::WriteXml( out, g_aTrackHeaderProp, this, &g_track, "\n    " );
  fprintf( out, "\n  />\n\n" );
  fprintf( out, "  <xwindow" );
  ReflectionProperty::WriteXml( out, g_aTrackXWindowProp, this, &g_track, "\n    " );
  fprintf( out, "\n  />\n\n" );
  fprintf( out, "  <pitstop" );
  ReflectionProperty::WriteXml( out, g_aTrackPitstopProp, this, &g_track, "\n    " );
  fprintf( out, "\n  />\n\n" );

  fprintf( out, "  <segments>\n\n" );

  WriteXmlSegment( out, "default", "    ", &m_oDefault, &builtin_default );
  fprintf( out, "\n" );

  for( i=0; i<m_iNumSection; i++ )
  {
    fprintf( out, "    <section name=\"%s\">\n", m_aSection[i].m_sName );
    for( j=m_aSection[i].m_iFirstSeg; j<=m_aSection[i].m_iLastSeg; j++ )
    {
      WriteXmlSegment( out, "segment", "      ", &m_aSeg[j], &m_oDefault );
    }
    fprintf( out, "    </section>\n\n" );
  }
  fprintf( out, "  </segments>\n\n" );

  fprintf( out, "  <objects>\n" );
  for( i=0; i<m_iNumObject3D; i++ )
  {
    fprintf( out, "    <object" );
    // ReflectionProperty::Store( &(m_aObject3D[i]) );
    ReflectionProperty::WriteXml( out, g_aObject3DProp, &(m_aObject3D[i]), &object3D_default, " " );
    fprintf( out, "/>\n" );
  }
  fprintf( out, "  </objects>\n\n" ); 
  fprintf( out, "</track>\n" );

  fclose( out );

  return 0;
}

/**
 * Write the XML tag of a segment
 */
void Track::WriteXmlSegment( FILE * out, const char * tag, const char * space, Segment3D * seg3D, Segment3D * default_seg )
{
  fprintf( out, "%s<%s" , space, tag );

  ReflectionProperty::WriteXml( out, g_aSegProp, seg3D, default_seg, " " );

  if( seg3D->lft.equals( &default_seg->lft ) && seg3D->rgt.equals( &default_seg->rgt ) )
  {
    fprintf( out, "/>\n" );
  }
  else
  {
    char space2[32];
    sprintf( space2, "%s  ", space );
    fprintf( out, ">\n" );
    WriteXmlSegmentSide( out, "left", space2, &(seg3D->lft), &(default_seg->lft) );
    WriteXmlSegmentSide( out, "right", space2, &(seg3D->rgt), &(default_seg->rgt) );
    fprintf( out, "%s</%s>\n", space, tag );
  }
}

/**
 * Write the XML tag of a segment3d side
 */
void Track::WriteXmlSegmentSide( FILE * out, const char * tag, const char * space, SegmentSide3D * side, SegmentSide3D * default_side )
{
  if( !side->equals(default_side) )
  {
    fprintf( out, "%s<%s" , space, tag  );

    // ReflectionProperty::Store( side );
    ReflectionProperty::WriteXml( out, g_aSegSideProp, side, default_side, " " );
    
    fprintf( out, "/>\n" );
  }
}

/*
 * Add a segment to the rgtwall 
 * The new segment is inserted at the position 'pos'
 */
void Track::AddSegment( int pos, char * section )
{
  int i;
  segment * newwall = new segment[m_iNumSegment+1]; 
  Segment3D * new3D = new Segment3D[m_iNumSegment+1]; 

  for( i=0; i<pos; i++ )
  {
    newwall[i] = rgtwall[i];
    new3D[i]   = m_aSeg[i];
  }

  for( i=pos; i<m_iNumSegment; i++ )
  {
    newwall[i+1] = rgtwall[i];
    new3D[i+1]   = m_aSeg[i];
  }

  delete rgtwall;
  delete m_aSeg;
  rgtwall = newwall;
  m_aSeg  = new3D;
  m_iNumSegment++;

  bool found = false;
  for( i=0; i<m_iNumSection; i++ )
  {
    if( found )
    {
      m_aSection[i].m_iFirstSeg++;
    }
    if( strcmp(m_aSection[i].m_sName, section)==0 )
    {
      found = true;
    }
    if( found )
    {
      m_aSection[i].m_iLastSeg++;
    }
  }

  if( !found )
  {
    strcpy( m_aSection[i].m_sName, section );
    m_aSection[i].m_iFirstSeg = pos;
    m_aSection[i].m_iLastSeg = pos;
    m_iNumSection ++;
  }
}

/*
 * Remove a segment to the rgtwall
 */
void Track::RemoveSegment( int pos )
{
  int i;
  segment * newwall = new segment[m_iNumSegment-1]; 
  Segment3D * new3D = new Segment3D[m_iNumSegment-1]; 

  for( i=0; i<pos; i++ )
  {
    newwall[i] = rgtwall[i];
    new3D[i]   = m_aSeg[i];
  }
  for( i=pos+1; i<m_iNumSegment; i++ )
  {
    newwall[i-1] = rgtwall[i];
    new3D[i-1]   = m_aSeg[i];
  }

  delete rgtwall;
  delete m_aSeg;
  rgtwall = newwall;
  m_aSeg  = new3D;
  m_iNumSegment--;

  int section = GetSectionId( pos );

  for( i=0; i<m_iNumSection; i++ )
  {
    if( i>section )
    {
      m_aSection[i].m_iFirstSeg--;
    }
    if( i>=section )
    {
      m_aSection[i].m_iLastSeg--;
    }
  }
  // Section is empty ??
  if( m_aSection[section].m_iLastSeg<m_aSection[section].m_iFirstSeg )
  {
    for( i=section; i<m_iNumSection-1; i++ )
    {
      m_aSection[i]=m_aSection[i+1];
    }
    m_iNumSection--;
  }
}

/**
 * Find a section id of a segment if not found
 */
int Track::GetSectionId( int iSeg )
{
  for( int i=0; i<m_iNumSection; i++ )
  {
    if( iSeg>=m_aSection[i].m_iFirstSeg && iSeg<=m_aSection[i].m_iLastSeg )
    {
      return i;
    }
  }
  // It should never come here.
  return -1;
}



/**
 * Read the files recordx.dat.
 *
 * Read the files recordx.dat with the best speed
 * and fill the array 'records'
 */
void Track::readTrackFastestLap()
{
  ifstream fin;
  int i; int found=0;

  // Try to read record from  the track record file:
  if( args.m_iSurface==0 )
  {
    fin.open("records0.dat");  // Open the track record file
  }  
  else if( args.m_iSurface==1 )
  {
    fin.open("records1.dat");  // Open the track record file
  }
  else if( args.m_iSurface==2 )
  {
    fin.open("records2.dat");  // Open the track record file
  }
   
  if (fin) 
  {
    fin >> number_of_records;
    for(i=0;i< number_of_records;i++)
    {
      fin >> records[i].track;
      fin >> records[i].speed;
      fin.ignore(20, '\t');
      fin.getline(records[i].rob_name, 33);
      if(!strcmp(records[i].track, m_sFileName))
      {
        strcpy(m_oRecord.track, records[i].track);
        m_oRecord.speed = records[i].speed;
        strcpy(m_oRecord.rob_name, records[i].rob_name);
        found = 1;
      }
    }
    fin.close();
    if(!found)
    {
      m_oRecord.speed = 0;
      strcpy(m_oRecord.rob_name, "Nobody");
    }
    m_oRecord.speed /= 3600.0;
    m_oRecord.speed *= 5280.0;
    m_oTrackDesc.record = m_oRecord;
     // change it to fps, for rars code
  }
  else  // No track record file :-(
  {
    strcpy(m_oRecord.rob_name, "Nobody");
    m_oRecord.speed = 0;
  }
}


/**
 * Save the file recordx.dat with the fastest lap time
 *
 * Based on the array 'record', the program add the new record and
 * store the result in the file recordx.dat
 *
 * @param rfl    (in) fastest lap time
 */
void Track::writeTrackFastestLap(fastest_lap rfl)
{
  ofstream fout;
  ifstream fin;
  int i,found;
  i = found = 0;
  fastest_lap temp;

  // Here is new track record confirmed:
  if( rfl.speed > m_oRecord.speed &&
      strcmp(m_sFileName, "random.trk") ) 
  {
    m_oRecord.speed = rfl.speed * 3600.0/5280.0;
    // convert to MPH
    strcpy( m_oRecord.rob_name, rfl.rob_name );

    // Now print alphabetically sorted track records to records.dat
    for(i=0;i< number_of_records;i++)
    {
      // update the record
      if(!strcmp(records[i].track,m_sFileName))
      {
        records[i].speed = m_oRecord.speed;
        strcpy(records[i].rob_name,m_oRecord.rob_name);
        found = 1;
      }
    }
    if(!found) // if record for this track has not been recorded, do it now
    {
      strcpy(records[i].track, m_sFileName);
      records[i].speed = m_oRecord.speed;
      strcpy(records[i].rob_name,m_oRecord.rob_name);
      ++number_of_records;
    }
    // Sort the records:
    for(i=0; i<number_of_records-1; i++)
    {
      while(strcmp(records[i].track,records[i+1].track)>0)
      {
        temp = records[i];
        records[i] = records[i+1];
        records[i+1] = temp;
        if(i>0) --i;
      }
    }

    // Now print it all out again:
    //      fout.open("records.dat");
    if( args.m_iSurface==0 )
    {
      fout.open("records0.dat");  // Open the track record file
    }
    else if( args.m_iSurface==1 )
    {
      fout.open("records1.dat");  // Open the track record file
    }
    else if( args.m_iSurface==2 )
    {
      fout.open("records2.dat");  // Open the track record file
    }

    fout << number_of_records << endl;
    for(i=0;i<number_of_records;i++)
    {
      fout.setf(ios::showpoint);
      fout << setiosflags(ios::left);
      fout << setw(14) << records[i].track;
      fout << records[i].speed << "\t";
      fout << records[i].rob_name << endl;
    }
    fout.close();
  }
}

/**
 * Read the track names from the "season" file
 */
void Track::readTrackNamesFromFile()
{
  ifstream fin;
  ofstream fout;
  int i = 0;
  char *string; 

  // Try to read record from  the track record file:
  chdir("tracks");
  fin.open("season");  // Open the track record file
   
  if (fin) 
  {
    for(i=0;i < MAX_TRACKS;i++)
    {
      string = new char[16];
  
      fin >> string;
      if(!strcmp("END",string))
      {
        break;
      }
      args.m_aTracks[i] = string;
    }
    args.m_iNumTrack = i;
    fin.close();
 
    chdir("..");
  }
  else  // No season file
  {
    cout << "Season file is missing from tracks directory" << endl;
  }
}

/**
 * Find the min and the max of the track 
 */
void Track::CalcMinMax()
{
  int i, j;

  // look for the min and max points of the track
  m_fXMin = (int) lftwall[0].beg_x;
  m_fYMin = (int) lftwall[0].beg_y;
  m_fXMax = (int) lftwall[0].beg_x;
  m_fYMax = (int) lftwall[0].beg_y;

  for( i=0; i<m_iNumSegment; i++ ) 
  {  
    // for each segment:
    if( lftwall[i].radius!=0.0 )
    {
      int step = (int)( lftwall[i].length*5.0 );
      double ang = lftwall[i].beg_ang;
       
      for( j=0; j<step; j++ )
      {
        m_fXMax = max( m_fXMax, lftwall[i].cen_x + lftwall[i].radius*sin(ang) );
        m_fXMax = (int) max( m_fXMax, lftwall[i].cen_x + rgtwall[i].radius*sin(ang) );
        m_fYMax = (int) max( m_fYMax, lftwall[i].cen_y - lftwall[i].radius*cos(ang) );
        m_fYMax = (int) max( m_fYMax, lftwall[i].cen_y - rgtwall[i].radius*cos(ang) );
        m_fXMin = (int) min( m_fXMin, lftwall[i].cen_x + lftwall[i].radius*sin(ang) );
        m_fXMin = (int) min( m_fXMin, lftwall[i].cen_x + rgtwall[i].radius*sin(ang) );
        m_fYMin = (int) min( m_fYMin, lftwall[i].cen_y - lftwall[i].radius*cos(ang) );
        m_fYMin = (int) min( m_fYMin, lftwall[i].cen_y - rgtwall[i].radius*cos(ang) );

        if( lftwall[i].radius>0.0 )
        {
          ang+=0.2;
          if(ang > 2.0 * PI) ang -= 2.0 * PI;
        }
        else 
        {
          ang-=0.2;
          if(ang < 2.0 * PI) ang += 2.0 * PI;
        }
      }
    } 
    else
    {
      m_fXMax = (int) max( m_fXMax, lftwall[i].beg_x );
      m_fXMax = (int) max( m_fXMax, rgtwall[i].beg_x );
      m_fYMax = (int) max( m_fYMax, lftwall[i].beg_y );
      m_fYMax = (int) max( m_fYMax, rgtwall[i].beg_y );
      m_fXMin = (int) min( m_fXMin, lftwall[i].beg_x );
      m_fXMin = (int) min( m_fXMin, rgtwall[i].beg_x );
      m_fYMin = (int) min( m_fYMin, lftwall[i].beg_y );
      m_fYMin = (int) min( m_fYMin, rgtwall[i].beg_y );
    }
  }
}

/**
 * Get the track description
 *
 * Get the track description. This function is used by the robots to get
 * the characteristics of the tracks.
 *
 * @return the description of the tracks
 */
track_desc Track::get_track_description() const
{
  return m_oTrackDesc;
}

/**
 * Get the track description
 *
 * Retained for compatability with drivers
 *
 * @return the description of the tracks
 */
track_desc get_track_description()
{
  return currentTrack->get_track_description();
}

//--------------------------------------------------------------------------
//                      Class Segment3D
//--------------------------------------------------------------------------

/**
 * Constructor - set the default values
 */
Segment3D::Segment3D()
{
  m_fLength = 0; 
  m_fRadius = 0; 
  m_fArc = 0; 
  m_fEndZ = 0; 
  m_fEndBanking = 0; 
  m_fHillHeight = 0;
  m_iPrvSeg = -1; 
  m_iOtherYSeg = -1; 

  m_pProfileHeight = &(g_aProfileHeight[0]); // "flat"
  strcpy( m_sName, "" );
  strcpy( m_sProfileHeight, "flat" );
  strcpy( m_sType, "default" );
  strcpy( m_sSection, "" );
  strcpy( m_sModel, "" );
}

/**
 * Calculate the data (m_pProfileHeight, ... )
 */
void Segment3D::Rebuild()
{
  int i = 0;
  while( strcmp(g_aProfileHeight[i].sName, m_sProfileHeight) )
  {
    i++;
    if( g_aProfileHeight[i].sName==NULL )
    {
      warning("Unknown profile height '%s'", m_sProfileHeight);
      i = 0;
      break;
    }
  }
  m_pProfileHeight = &(g_aProfileHeight[i]);
}

//--------------------------------------------------------------------------
//                      Class ProfileHeight
//--------------------------------------------------------------------------

int ProfileHeight::GetNbSubSeg()
{
  int i=1;
  while( aHeight[i]!=0 && aHeight[i]!=1 )
  {
    i++;
  }
  return i;
}

//--------------------------------------------------------------------------
//                      Class ReflectionProperty
//--------------------------------------------------------------------------

/**
 * Read the property and store in the global segment 
 */
void ReflectionProperty::ReadXml( ReflectionProperty a[], void * struct_base, const char * param_name, const char * param_value )
{
  int i=0;
  
  while( a[i].sXmlTag!=NULL )
  {
    if( strcmp(param_name, a[i].sXmlTag)==0 )
    {
      a[i].SetString( struct_base, param_value );
      break; // while
    }
    i++;
  }
}

/**
 * Write the xml tags
 */
void ReflectionProperty::WriteXml( FILE * out, ReflectionProperty a[], void * struct_base, void * default_base, char * space )
{
  // For all the property of the list, write them if the value is not the default one
  int i=0;
  while( a[i].sXmlTag!=NULL )
  {
    if( !a[i].IsDefault( struct_base, default_base ) )
    {
      fprintf( out, "%s%s=\"%s\"", space, a[i].sXmlTag, a[i].GetString( struct_base ) );
    }
    i++;
  }
}

/**
 * Return the pointer of the data relatif to another FullSegment or SegmentSide3D
 */
void * ReflectionProperty::GetDataPointer( void * struct_base  )
{
  // Pointer trick : char = 1 byte
  return (char *)(struct_base) + ((char *)data - (char *)(base)) ; 
}

/**
 * Get the string value of the property Is the property equal to the default value ?
 */
char * ReflectionProperty::GetString( void * struct_base )
{
  static char s[256];
  void * struct_data = GetDataPointer( struct_base );
  switch( iType )
  {
    case T_BOOL:
    {
      bool * p = (bool *)struct_data;
      if( *p ) 
      {
        sprintf( s, "true" );
      }
      else
      {
        sprintf( s, "false" );
      }
      break;
    }
    case T_DISTANCE:
    {
      double * p = (double *)struct_data;
      strcpy( s, Double2String( *p/g_fDistanceScale ) );
      break;
    }
    case T_DOUBLE:
    case T_ANGLE:
    {
      double * p = (double *)struct_data;
      strcpy( s, Double2String( *p ) );
      break;
    }
    case T_INT:
    {
      int * p = (int *)struct_data;
      sprintf( s, "%d", *p );
      break;
    }
    case T_STRING:
    {
      char * data = (char *)struct_data;
      strcpy( s, data );
      break;
    }
  }
  return s;
}

/**
 * Store the string value in the property
 */
void ReflectionProperty::SetString( void * struct_base, const char * val )
{
  void * struct_data = GetDataPointer( struct_base );

  switch( iType )
  {
    case T_BOOL:
    {
      bool * p = (bool *)struct_data;
      if( strcmp(val,"true")==0 )
      {
        *p = true;
      }
      else
      {
        *p = false;
      }
      break;
    }
    case T_DISTANCE:
    {
      double * p = (double *)struct_data;
      *p = atof( val )*g_fDistanceScale;
      break;
    }
    case T_DOUBLE:
    case T_ANGLE:
    {
      double * p = (double *)struct_data;
      *p = atof( val );
      break;
    }
    case T_INT:
    {
      int * p = (int *)struct_data;
      *p = atoi( val );
      break;
    }
    case T_STRING:
    {
      char * p = (char *)struct_data;
      strcpy( p, val );
      break;
    }
  }
}

/**
 * Store the string value in the property
 */
void ReflectionProperty::CopyValue( void * dest_base, void * src_base )
{
  void * dest_data = GetDataPointer( dest_base );
  void * src_data = GetDataPointer( src_base );

  switch( iType )
  {
    case T_BOOL:
    {
      bool * dest = (bool *)dest_data;
      bool * src  = (bool *)src_data;
      *dest = *src;
      break;
    }
    case T_DOUBLE:
    case T_DISTANCE:
    case T_ANGLE:
    {
      double * dest = (double *)dest_data;
      double * src  = (double *)src_data;
      *dest = *src;
      break;
    }
    case T_INT:
    {
      int * dest = (int *)dest_data;
      int * src  = (int *)src_data;
      *dest = *src;
      break;
    }
    case T_STRING:
    {
      char * dest = (char *)dest_data;
      char * src  = (char *)src_data;
      strcpy( dest, src );
      break;
    }
  }
}

/**
 * Is the property equal to the default value ?
 */
bool ReflectionProperty::IsDefault( void * struct_base, void * default_base )
{
  void * struct_data = GetDataPointer( struct_base );
  void * default_data = GetDataPointer( default_base );
  switch( iType )
  {
    case T_BOOL:
    {
      bool data = *((bool *)struct_data);
      bool def = *((bool *)default_data);
      return def==data;
    }
    case T_DOUBLE:
    case T_DISTANCE:
    case T_ANGLE:
    {
      double data = *((double *)struct_data);
      double def = *((double *)default_data);
      return def==data;
    }
    case T_INT:
    {
      int data = *((int *)struct_data);
      int def = *((int *)default_data);
      return def==data;
    }
    case T_STRING:
    {
      char * data = (char *)struct_data;
      char * def  = (char *)default_data;
      return !strcmp( def, data );
    }
  }
  return false;
}

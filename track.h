/**
 * TRACK.H - Reads the track definition file 
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
 *  ver. 0.6b May,95  b for beta
 *  ver. 0.61 May,96
 *  ver. 0.67 Oct,97 added random track generator function, Henning Klaskala
 *  ver. 0.70 Nov,97 pit structures, trackin renamed to rgtwall
 *  ver. 0.71 Feb,98 seg_dist added to track description structure
 *  ver. 0.72 Mar,98 track record, multiple tracks
 *  ver  0.76 Oct 00 - ccdoc
 *
 * @author    Mitchell E. Timin, State College, PA 
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @see       CAR.H & TRACK.H for class and structure declarations 
 * @version   0.76
 */

#ifndef __RARSCORE_TRACK_H
#define __RARSCORE_TRACK_H

//--------------------------------------------------------------------------
//                             D E F I N E
//--------------------------------------------------------------------------

#define MAX_SEGMENT 100
#define MAX_SECTION 10
#define MAX_OBJECT3D 100

//--------------------------------------------------------------------------
//                           I N C L U D E 
//--------------------------------------------------------------------------

#include <stdio.h>
#include <string.h>

// Where is your chdir() function?
#ifdef __BORLANDC__
 // Borland compilers
#include <dir.h>
#elif defined _AFXDLL
 // MS VisualC
#include <direct.h>
#elif defined(__MWERKS__)
 // for Metrowerks compilers on mac
#include <unistd.h>
#else
 // UNIX systems ?
#include <unistd.h> 
#endif

//--------------------------------------------------------------------------
//                              T Y P E 
//--------------------------------------------------------------------------

/**
 * Profile of the height 
 */
class ProfileHeight
{
  public: 
    const char * sName;
    double aHeight[ 20 ];

    int GetNbSubSeg();
};

extern ProfileHeight g_aProfileHeight[];

/** 
 * The track consists of a sequence of these segments:  (see CARZ.CPP)   
 * describes an arc or a straight line   
 */
class segment 
{ 
  public: 
    // calculated data
    double radius;                  // 0.0 means straight line, < 0.0 means right turn   
    double length;                  // radians if an arc, feet if straight line   
    double beg_x, beg_y;            // coordinates of begining of segment   
    double end_x, end_y;            // coordinates of end   
    double cen_x, cen_y;            // coordinates of arc center (if arc)   
    double beg_ang, end_ang;        // path angles, radians, 0.0 is in x direction  
    
    segment()
    {
      radius = 0;
      length = 0;
    }
};

/**
 * Data of one side (right, left) for 3D 
 */
class SegmentSide3D
{
  public:
    // Data read from file
    bool   m_bShowTree;
    bool   m_bShowBorderTree;
    double m_fBorderSize;
    double m_fBarrierHeight;
    double m_fBorderTreeSize;
    double m_fBorderTreeHeight;
    char   m_sTextureBorder[32];
    char   m_sTextureBarrier[32];
    char   m_sTextureBorderTree[32];
    char   m_sTextureTree[32];

    SegmentSide3D()
    {
      // default values
      m_bShowTree = true;
      m_bShowBorderTree = true;
      m_fBorderSize = 90;
      m_fBarrierHeight = 4.2;
      m_fBorderTreeSize = 90;
      m_fBorderTreeHeight = 75;
      strcpy( m_sTextureBorder, "grass_in" );
      strcpy( m_sTextureBarrier, "barrier" );
      strcpy( m_sTextureBorderTree, "grass_out" );
      strcpy( m_sTextureTree, "tree" );
    }

    int equals( SegmentSide3D * side )
    {
      if( m_bShowTree==side->m_bShowTree
       && m_bShowBorderTree==side->m_bShowBorderTree
       && m_fBorderSize==side->m_fBorderSize 
       && m_fBarrierHeight==side->m_fBarrierHeight 
       && m_fBorderTreeSize==side->m_fBorderTreeSize 
       && m_fBorderTreeHeight==side->m_fBorderTreeHeight
       && strcmp(m_sTextureBorder, side->m_sTextureBorder)==0 
       && strcmp(m_sTextureBarrier, side->m_sTextureBarrier)==0 
       && strcmp(m_sTextureBorderTree, side->m_sTextureBorderTree)==0 
       && strcmp(m_sTextureTree, side->m_sTextureTree)==0 )
      {
        return 1;
      }
      else
      {
        return 0;
      }
    }
};

/**
 * Data of one side (right, left) for 3D 
 */
class Segment3D
{
  public:
    SegmentSide3D lft;
    SegmentSide3D rgt;

    // Data read from file
    double m_fLength;                  // (Straight) Length of a segment for a straight
    double m_fRadius;                  // (Curve) Radius of a segment 
    double m_fArc;                     // (Curve) Angle of the curve 
    double m_fEndZ;                    // Height at the end of the segment
    double m_fEndBanking;              // Banking at the end of the segment
    double m_fHillHeight;              // Height of a hill (profile_height=hill)
    char m_sName[32];                  // Name of the segment
    char m_sProfileHeight[32];         // Type of height profile
    char m_sType[32];                  // Type of the segment
    char m_sSection[32];               // Section name for y a yinv type of segment
    char m_sModel[32];                 // 3DS Model for special segments where we do not want to use the 3D model generated automatically by Rars

    // Calculated data
    int m_iPrvSeg;                     // This id allows to build the track easily. It is used mostly by the sections.
    int m_iNxtSeg;                     // This id allows to build the track easily. It is used mostly by the sections.
    int m_iOtherYSeg;                  // For a y segment, this is the id of the other associated segment.
    ProfileHeight * m_pProfileHeight; 

    Segment3D();
    void Rebuild();
    SegmentSide3D& side(bool bLft)
    {
      if( bLft ) return lft;
             else return rgt;
    }
};

/**
 * Section: a section is a group of segment that are not in the main track
 * like a pitstop, and so on
 *
 * The segments of the section are stored in rgtwall and seg3D (like normal segments)
 * but are define after NSEG (so the drivers do not see them)
 */
class Section
{
  public:
    char   m_sName[32];               // Name of the section (ex: "pitstop")
    int    m_iFirstSeg;               // First segment
    int    m_iLastSeg;                // Last segment

    Section()
    {
      // default values
      m_sName[0] = 0;
      m_iFirstSeg = -1;
      m_iLastSeg = -1;
    }
};

/**
 * Object3D (3D object on the track)
 */
class Object3D
{
  public:
    char m_sModel[32];                 // Name of the model (file.3ds)
    double x, y, z;                    // Position of the object
    double rot_x, rot_y, rot_z;        // Rotation of the object

    Object3D()
    {
      m_sModel[0] = 0;
      x = y = z = 0;
      rot_x = rot_y = rot_z = 0;
    }
};

enum PropertyType
{
  T_ANGLE,
  T_BOOL,
  T_DISTANCE,
  T_DOUBLE,
  T_INT,
  T_STRING
};

/**
 * This class allow to make a kind of 'Reflection' (Java word) of a C structure
 *
 * It is used to remove the need to maintain in a lot of place the list
 * of all the properties of the segments
 */
class ReflectionProperty 
{
  public:
    const char * sXmlTag;
    void * data;               // Pointer to the data itseft (double *) (char *) ....
    void * base;               // Pointer to the reference structure containing the data 
    int    iType;              // Type of the data
    const char * sDescription; // Description of the data

    void * GetDataPointer( void * struct_base );
    char * GetString( void * struct_base );
    void SetString( void * struct_base, const char * val );
    void CopyValue( void * dest_base, void * src_base );
    bool IsDefault( void * struct_base, void * default_base );

    // Static
    static void ReadXml( ReflectionProperty a[], void * struct_base, const char * param_name, const char * param_value );
    static void WriteXml( FILE * out, ReflectionProperty a[], void * struct_base, void * default_base, char * space );
};

struct fastest_lap 
{
  char track[33];
  double speed;
  char rob_name[33];
};

struct track_desc 
{
  char sName[32];     // name of the track
  int NSEG;           // number of track segments (see drawpath() in DRAW.CPP)
  double width;       // width of track, feet   
  double length;      // length of track, feet (5280 ft/mile, 3281 ft/km)   

  segment *rgtwall;   // replaces trackout   
  segment *lftwall;   // replaces trackin   
  segment *trackout;  // old track defining arrays   
  segment *trackin;   

  double *seg_dist;   // distance from SF lane to end of each segment
  int pit_side;       // car needs to know pit location
  double pit_entry;   // to avoid entering and exiting cars   
  double pit_exit;    // and to calculate estimated pit times   
  double pit_speed;   //    
  fastest_lap record; // fastest ever lap on this race
};

/**
 * class Track encapsulates all information about a track
 */
class Track
{
private:
  track_desc m_oTrackDesc;               // Precalculated track description

  void Init();
  void LoadXml( char * filename );       // Load the track in xml format
  void AllocSegment();                   // Allocate the segments
  void WriteXmlSegment( FILE * out, const char * tag, const char * space, Segment3D * seg3D, Segment3D * default_seg );
  void WriteXmlSegmentSide( FILE * out, const char * tag, const char * space, SegmentSide3D * side, SegmentSide3D * default_side );
  void CalcMinMax();                     // Find the min and max of the track

public:
  char m_sFileName[32];                  // The filename this object was read from
  char m_sShortName[32];                 // The filename this object was read from (without .trk)
  double width;                          // width of track, feet   
  double length;                         // length of track, feet (5280 ft/mile, 3281 ft/km)   
  int NSEG;                              // number of segments of the track (<>m_iNumSegment)
  int m_iNumSegment;                     // number of segments of the track + the segments of the additional sections like pitstop (<>NSEG)
  int m_iNumObject3D;                    // number of Object3D (decorations) of the track
  int m_iNumSection;                     // number of sections
  
  // segments
  segment   * rgtwall;                   // replaces trackout   
  segment   * lftwall;                   // replaces trackin   
  Segment3D * m_aSeg;                    // Segment of the track(with rgtwall), mostly for 3D info of the track
  Segment3D   m_oDefault;                // default segment

  // I use static size here (not very clean) but it allows to remove a lot of code about allocation/desallocation of memory
  Object3D    m_aObject3D[MAX_OBJECT3D]; // list of the Object3D (decorations) of the track
  Section     m_aSection[MAX_SECTION];   // list of the sections track
  Object3D    m_oSky;                    // Name of the sky Object3D 

  double    * seg_dist;                  // distance from SF lane to end of each segment
  int         pit_side;                  // car needs to know pit location
  double      pit_entry;                 // to avoid entering and exiting cars   
  double      pit_exit;                  // and to calculate estimated pit times   
  double      pit_speed;                 //    
  fastest_lap m_oRecord;                 // fastest ever lap on this race

  ////
  //// Data read from the track file
  ////
  // Description of the track
  char m_sName[32];            // Name of the track (<>filename)
  char m_sAuthor[32];          // Author of the track
  char m_sDescription[128];    // Description of the track
  int  m_iVersion;             // Version of the track

  // Unit data
  char m_sUnitDistanceName[32];// Name of the distance unit
  double m_fUnitDistanceScale; // Number of (Rars) feets in a distance unit

  // Header data
  double m_fFinish;            // Fraction of segment 0 prior to finish line (0->1)   
  double m_fRgtStartX;         // Coordinates of where to start drawing track (feet)  
  double m_fRgtStartY;         //    
  double m_fStartAng;          // Angle of first segment (radians)   
  int    m_iStartRows;         // How many rows of starting cars

  // XWindow data
  double m_fXMax;              // Max coordinate in XWindow (feet) + see CalcMinMax
  double m_fYMax;              // 
  double m_fScoreBoardX;       // These are in feet, track coordinates   
  double m_fScoreBoardY;       //   (where the scoreboard is located)   
  double m_fLeaderBoardX;      // Upper left corner of leader board, feet   
  double m_fLeaderBoardY;      //
  double m_fInstPanelX;        // Instrument Panel   
  double m_fInstPanelY;        //
  double m_fMessageX;          // Where "Length of track... " starts
  double m_fMessageY;          // 

  // Pitstop Data
  int m_iPitSide;              // 1 is right side; left side is -1
  double m_fPitEntry;          // where car starts steering to pits   
  double m_fPitExit;           // back on the track   
  double m_fPitLaneStart;      // pit start-end denote stopping area    
  double m_fPitLaneEnd;        // those four are compared to s.distance    
  double m_fPitLaneSpeed;      // feet per second, divide by 1.8 to get mph    

  ////
  //// Others
  ////
  double SCALE;                // feet per pixel, mapping track to screen   
  double from_start_to_seg1;   // distance from start/finish line to seg end   
  double finish_rx, finish_ry; // These four variables are used to
  double finish_lx, finish_ly; // locate the finish line on the screen.
  double m_fLftStartX;         // Coordinates of where to start drawing track (feet)  
  double m_fLftStartY;         //    
  double m_fXMin;              // Min coordinate of the track - see CalcMinMax
  double m_fYMin;              //
  bool   m_bCopy;              // Allow to copy the track for the edition (TrackEditor) 

  Track();                                      // dummy constructor (used by reflection only)
  Track(char* fileName);                        // real constructor
  ~Track();                                     // destructor
 
  int SaveXml();                                // Save the track in Xml format
  void Rebuild();                               // Rebuild the lftwall from rgtwall
  void track_setup(double xstart, double ystart, double alfstart, segment *seg);
  void AddSegment( int pos, char * section );   // Add a segment to the rgtwall
  void RemoveSegment( int pos );                // Remove a segment to the rgtwall
  int GetSectionId( int iSeg );                 // Get the id of the section containing a segment 

  track_desc get_track_description() const;     // Return a track_desc structure    
  void readTrackFastestLap();                   // Reads track record from file
  void writeTrackFastestLap(fastest_lap);       // Write track record to file

  static void generateRandomTrack();            // Generate a random track and write it to file "random.trk"
  static void readTrackNamesFromFile();         // Read track files from season file


};


//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

////
//// Variables
////

// Global variable (defined in track.cpp)
extern Track* currentTrack;

// For the track editor
extern ReflectionProperty g_aTrackInfoProp[];
extern ReflectionProperty g_aTrackUnitProp[];
extern ReflectionProperty g_aTrackHeaderProp[];
extern ReflectionProperty g_aTrackXWindowProp[];
extern ReflectionProperty g_aTrackPitstopProp[];
extern ReflectionProperty g_aSegProp[];
extern ReflectionProperty g_aSegSideProp[];
extern ReflectionProperty g_aObject3DProp[];

////
//// Functions
////
track_desc get_track_description();  // returns a track_desc structure (retained for compatability with drivers)

#endif


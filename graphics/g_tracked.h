
/**
 * G_TRACKED.H - Track Editor types
 *
 * @author    Marc Gueury
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef __GRAPHICS_TRACKED_H
#define __GRAPHICS_TRACKED_H

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "../3ds/model_3ds.h"
#include "g_track3d.h"
#include "../track.h"

#define TRACKED_VIEW_MODE_2D   0
#define TRACKED_VIEW_MODE_3DS  1
#define TRACKED_VIEW_MODE_ONE_SEG  2

#define AREA_MAX 20
#define AREA_VERTEX_MAX 1000

#define AREA_TYPE_LEFT 1
#define AREA_TYPE_RIGHT 2


//--------------------------------------------------------------------------
//                           F O R W A R D
//--------------------------------------------------------------------------

class GTrackListArea;

//--------------------------------------------------------------------------
//                             T Y P E S
//--------------------------------------------------------------------------

/**
 * Ancestor of all actions
 */
class Action
{
  public:
    Action * m_pNext;
    Action * m_pPrev;
    
    Action() 
    {
      m_pNext = NULL;
      m_pPrev = NULL;
    };
    virtual ~Action() {};

    virtual void Do() {};
    virtual void Undo() {};
};

/**
 * Change track segment
 */
class ActionSegmentChange : public Action
{
  int m_iSeg;
  Segment3D m_NewSeg3D;
  Segment3D m_OldSeg3D;

  public:
    ActionSegmentChange( int iSeg, Segment3D seg3D );
    virtual ~ActionSegmentChange() {};
    virtual void Do();
    virtual void Undo();
};

/**
 * New track segment
 */
class ActionSegmentNew : public Action
{
  int m_iSeg;
  char m_sSection[32];

  public:
    ActionSegmentNew( int iSeg, const char * section );
    virtual ~ActionSegmentNew() {};
    virtual void Do();
    virtual void Undo();
};

/**
 * Delete track segment
 */
class ActionSegmentDelete : public Action
{
  int m_iSeg;
  Segment3D m_OldSeg;
  int m_iSection;
  char m_sSection[32];

  public:
    ActionSegmentDelete( int iSeg );
    virtual ~ActionSegmentDelete() {};
    virtual void Do();
    virtual void Undo();
};

/**
 * Change View Mode
 */
class ActionChangeViewMode : public Action
{
  int m_iViewMode;
  int m_iOldViewMode;

  public:
    ActionChangeViewMode( int iSeg );
    virtual ~ActionChangeViewMode() {};
    virtual void Do();
    virtual void Undo();
};

/**
 * Automatic closure of the track
 */
class ActionAutoClosure : public Action
{
  int m_iLastCurve[MAX_SECTION];
  int m_iLastStraight[MAX_SECTION];
  Segment3D m_LastCurve[MAX_SECTION];
  Segment3D m_LastStraight[MAX_SECTION];

  public:
    ActionAutoClosure();
    virtual ~ActionAutoClosure() {};
    virtual void Do();
    virtual void Undo();
};

/**
 * Stack of actions
 */
class ActionManager
{
  Action * m_ListAction;
  Action * m_pLastAction;

  public:
    ActionManager();
    ~ActionManager();

    void Init();
    void Reset();
    bool IsEmpty();
    void AddAction( Action * a );
    void RemoveNextActions( Action * a );
    void Do( Action * a );
    void Undo();
    void Redo();
};

//---------------------------------------------------------------------------

class GTrackArea
{
  public:
    Vertex m_aVertex[AREA_VERTEX_MAX];
    int m_iNumVertex; 
    int m_iType;

    GTrackArea( int type=0 );
    void Reset();
    void Add( Vertex &v );
    void FlipOrder();
    void RotateOrder( int iFirst );
    bool FindFirstIntersection( GTrackArea &other, Vertex &inter, int &i, int &i2, int &j, int &j2 );
    bool FindNextIntersection( GTrackArea &other, Vertex &inter, int &i, int &i2, int &j, int &j2 );
    void RemoveDuplicate();
    void Copy( GTrackArea &src, int beg, int end, bool bReset=true );
    double SumAngle();
    int NumIntersection( Vertex &a, Vertex &b );
    Vertex GetVertexOutside(int i);
    bool IsInArea( Vertex &a );
    bool IsOutsideVertexInsideRoad( GTrackListArea &lft, GTrackListArea &rgt );
    int FindMaxX();
};

class GTrackListArea
{
  public:
    int m_iNumArea;
    GTrackArea * m_aArea[AREA_MAX];

    GTrackListArea();
    GTrackListArea( GTrackListArea &lft, GTrackListArea &rgt );
    ~GTrackListArea();
    void Reset();
    void Add( GTrackArea * a );
    void Remove( GTrackArea * a );
    void SplitArea( GTrackListArea &lft, GTrackListArea &rgt );
    void CopyAll( Model_3DS &model );
};

//---------------------------------------------------------------------------

class SegmentSideCalcData
{
  public:
    double m_fBorderSize;
    Vertex m_vEndBorderTree;
};

class SegmentCalcData
{
  public:
    SegmentSideCalcData lft;
    SegmentSideCalcData rgt;

    SegmentSideCalcData& side(bool bLft)
    {
      if( bLft ) return lft;
            else return rgt;
    }
};

/** 
 * Data used inside the track editor for each side (left/right)
 */
class TrackEditorSideData
{
  public:
    // Points to generate the 3DS file
    int p_road_beg, p_road_end;
    int p_redwhite_beg, p_redwhite_end;
    int p_border_in_beg, p_border_barrier_beg, p_border_in_end, p_border_barrier_end;
    int p_border_tree_beg, p_border_tree_end;
    int p_barrier_top_beg, p_barrier_bottom_beg, p_barrier_top_end, p_barrier_bottom_end;
    int p_barrier2_top_beg, p_barrier2_bottom_beg, p_barrier2_top_end, p_barrier2_bottom_end;
    int p_tree_top_beg, p_tree_bottom_beg, p_tree_top_end, p_tree_bottom_end;

    // Data for to build a sub-segment
    double m_fEndX;
    double m_fEndY;
    double m_fEndZ;

    double m_fRedWhiteSize;
    double m_fBorderSize;
    double m_fBarrierSize;
    double m_fBarrierHeight;
    double m_fBorderTreeSize;
    double m_fBorderTreeHeight;

    bool m_bShow;
    bool m_bShowTree;
    bool m_bShowBorderTree;
    bool m_bShowBarrier;

    bool m_bShowRelief;

    // Vertex
    Vertex m_vBorderTree;

    // Materials
    int mf_border, mf_barrier, mf_border_tree, mf_tree;

    // Area
    GTrackListArea area;

    void Reset()
    {
      m_fRedWhiteSize = 0;
      m_fBarrierSize = 0;
      m_bShow = true;
      m_bShowBarrier = true;
      m_bShowBorderTree = true;
      m_bShowTree = true;
      m_bShowRelief = true;
    }
};

/**
 * Global variables
 */
class TrackEditor
{
  private:
    TrackEditorSideData lft, rgt;

    // Materials
    int mf_road, mf_red_white;
    SegmentCalcData * m_aCalcData;

    void CalcSubSegmentCommon( int i, int i0, int j, int iNbSubSeg, double end_z, double radius, TrackEditorSideData * side, segment * sidewall, SegmentSide3D  *segside, SegmentSide3D  *segside0, SegmentSideCalcData *calc, SegmentSideCalcData *calc0, double sign );
    void CurveAutoCorrection( int i, int j, int o, TrackEditorSideData * side, segment * sidewall, double a, double sign );
    void StraightAutoCorrection( TrackEditorSideData * side );
    double CorrectSubSegmentPos( int i, int j, int o, TrackEditorSideData * side, segment * sidewall, double posj, double step_size );

    void GenerateSubSegment3DS( Model_3DS::Object * pObject, int j, double end_nb_tex );
    void GenerateSubSegmentSide3DS( Model_3DS::Object * pObject, int j, double end_nb_tex, TrackEditorSideData * side, bool bTextureSide, double dx, double dy );
    int NextUnused( int i, bool aUsed[], int iNumVertex );

    // Os specific functions
    void Refresh( bool bRebuildList=true );
    void UpdateData( bool bGetData );

  public:
    int m_iViewMode;
    int m_iCurSegment;
    int m_iCurSection;
    ActionManager m_oActionManager;

    // Segment in the lists
    Segment3D     m_Segment;
    SegmentSide3D m_SegmentSide;

  // Current Property
    char m_sPropValue[256];
    ReflectionProperty * m_pStructProp;
    void * m_pStructBase;
    void * m_pDefaultBase;

    // Copy/Paste
    char m_sClipBoardValue[256];            // Value of the property 
    ReflectionProperty * m_pClipBoardProp;  // Property
    void * m_pClipBoardBase;                // Base of the property
    int m_iClipBoardSegmentId;              // Segment id
    char m_sClipBoardText[256];             // Description of the clipboard

    TrackEditor();
    void PreCalc();
    void PostCalc();
    void GenerateSegmentModel( Model_3DS &model, int i );
    void GenerateRelief();
    void Rebuild( bool bSave=false, bool bCreateList=false );

    void Load( const char * name );
    void CheckCurrent();
    void EditProperty();
    void SetResetCommon();

    // Actions
    void OnFileNew();
    void OnFileSave();
    void OnFileClose();
    void OnEditUndo();
    void OnEditRedo();
    void OnPropSet();
    void OnPropReset();
    void OnSegmentNew();
    void OnSegmentDelete();
    void OnSegmentInfo();
    void OnSegmentNxt();
    void OnSegmentPrv();
    void OnEditCopy();
    void OnEditPaste();
    void OnEditPasteAll();
    void OnViewTrack2d();
    void OnViewTrack3ds();
    void OnViewTrackOneSeg(); 
    void OnTrackGen3d();
    void OnTrackAutoclosure(); 
};

//--------------------------------------------------------------------------
//                           G L O B A L S
//--------------------------------------------------------------------------

extern TrackEditor g_TrackEditor;
extern TTrack3D track3D;

#endif


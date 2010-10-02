/**
 * CDlgTrackEditor.cpp - Windows dialog for the track editor
 *
 * History
 *  ver. 0.9 creation
 *
 * @author    Marc Gueury (mgueury@skynet.be)
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "stdafx.h"
#include "vc_rars.h"
#include "DlgTrackEditor.h"
#include "DlgTrackProperty.h"
#include "DlgTrackLoad.h"
#include "DlgTrackSection.h"
#include "../globals.h"
#include "../misc.h"
#include "../track.h"
#include "../graphics/g_global.h"
#include "../graphics/g_tracked.h"
#include <direct.h>
#include "../race_manager.h"

#ifdef _DEBUG
#define new new(__FILE__, __LINE__)
#endif

//--------------------------------------------------------------------------
//                           E X T E R N S
//--------------------------------------------------------------------------

extern RaceManager g_RaceManager;

//--------------------------------------------------------------------------
//                        Class CDlgTrackEditor
//--------------------------------------------------------------------------

/**
 * Constructor
 */
CDlgTrackEditor::CDlgTrackEditor(CWnd* pParent /*=NULL*/)
	: CDlgRarsCommon(CDlgTrackEditor::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrackEditor)
	m_sSegList = _T("");
	m_sSectionList = _T("");
	//}}AFX_DATA_INIT

  // Value from GetClientRect( m_OrigClientRect );
  m_OrigClientRect.top = 0;
  m_OrigClientRect.bottom = 424;
  m_OrigClientRect.left= 0;
  m_OrigClientRect.right = 618;

  LPCTSTR lpTableName = MAKEINTRESOURCE( IDR_TRACK_ACCELERATOR );
  m_hAccel = LoadAccelerators( theApp.m_hInstance, lpTableName );
}

/**
 * Destructor
 */
CDlgTrackEditor::~CDlgTrackEditor()
{
  delete g_ViewManager;
}

void CDlgTrackEditor::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrackEditor)
	DDX_Control(pDX, IDC_CLIPBOARD, m_ClipBoardStatic);
	DDX_Control(pDX, IDC_SECTION_LIST, m_ListSection);
	DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
	DDX_Control(pDX, IDC_PROP_VALUE, m_PropValue);
	DDX_Control(pDX, IDC_PROP_NAME, m_PropName);
	DDX_Control(pDX, IDC_CENTER_LIST, m_CenterList);
	DDX_Control(pDX, IDC_RIGHT_LIST, m_RightList);
	DDX_Control(pDX, IDC_LEFT_LIST, m_LeftList);
	DDX_Control(pDX, IDC_SEGMENT_LIST, m_ListSeg);
	DDX_LBString(pDX, IDC_SEGMENT_LIST, m_sSegList);
	DDX_LBString(pDX, IDC_SECTION_LIST, m_sSectionList);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrackEditor, CDialog)
	//{{AFX_MSG_MAP(CDlgTrackEditor)
	ON_COMMAND(ID_TRACK_GEN3D, OnTrackGen3d)
	ON_LBN_SELCHANGE(IDC_SEGMENT_LIST, OnSelchangeSegmentList)
	ON_WM_SIZE()
	ON_WM_PAINT()
	ON_WM_TIMER()
	ON_COMMAND(ID_EDIT_UNDO, OnEditUndo)
	ON_COMMAND(ID_EDIT_REDO, OnEditRedo)
	ON_COMMAND(ID_SEGMENT_NEW, OnSegmentNew)
	ON_COMMAND(ID_SEGMENT_DELETE, OnSegmentDelete)
	ON_COMMAND(ID_FILE_NEW, OnFileNew)
	ON_COMMAND(ID_TRACK_PROPERTY, OnTrackProperty)
	ON_COMMAND(ID_VIEW_TRACK_ONE_SEG, OnViewTrackOneSeg)
	ON_COMMAND(ID_VIEW_TRACK2D, OnViewTrack2d)
	ON_COMMAND(ID_VIEW_TRACK3DS, OnViewTrack3ds)
	ON_LBN_SELCHANGE(IDC_CENTER_LIST, OnSelchangeCenterList)
	ON_LBN_SELCHANGE(IDC_LEFT_LIST, OnSelchangeLeftList)
	ON_LBN_SELCHANGE(IDC_RIGHT_LIST, OnSelchangeRightList)
	ON_BN_CLICKED(IDC_PROP_SET, OnPropSet)
	ON_BN_CLICKED(IDC_PROP_RESET, OnPropReset)
	ON_COMMAND(ID_EDIT_COPY, OnEditCopy)
	ON_COMMAND(ID_EDIT_PASTE, OnEditPaste)
	ON_COMMAND(ID_SEGMENT_NXT, OnSegmentNxt)
	ON_COMMAND(ID_SEGMENT_PRV, OnSegmentPrv)
	ON_LBN_SELCHANGE(IDC_SECTION_LIST, OnSelchangeSectionList)
	ON_COMMAND(ID_SECTION_NEW, OnSectionNew)
	ON_COMMAND(ID_EDIT_PASTE_ALL, OnEditPasteAll)
	ON_COMMAND(ID_TRACK_AUTOCLOSURE, OnTrackAutoclosure)
	ON_COMMAND(ID_SEGMENT_INFO, OnSegmentInfo)
	ON_COMMAND(ID_FILE_CLOSE, OnFileClose)
	ON_COMMAND(ID_FILE_LOAD, OnFileLoad)
	ON_COMMAND(ID_FILE_SAVE, OnFileSave)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackEditor message handlers

BOOL CDlgTrackEditor::OnInitDialog() 
{
  CDlgRarsCommon::OnInitDialog();
  
  ShowWindow( SW_SHOWMAXIMIZED );

  // Define the global variables needed by the track
  args.m_iStartRows = 2;
  args.m_iRaceLength = 0;
  args.m_iSurface = 1;
  args.m_iNumCar = 0;
  g_oRarsIni.m_iViewMode = MODE_TRACK_EDITOR;
  g_oRarsIni.m_iFollowMode = FOLLOW_NOBODY;
  g_oRarsIni.m_i3DS = 0;

  // Initialise the graphics
  if( chdir("tracks")==0 )
  {
    chdir("..");
  }
  else
  {
    chdir("..");
  }
  vc_main_part0( this );
  g_ViewManager = new TViewManager( NULL );
  g_ViewManager->InitAllViews();

  InitDialogPart2();

	return TRUE;
}

void CDlgTrackEditor::OnPaint() 
{
  CDlgRarsCommon::OnPaint();
}

void CDlgTrackEditor::OnTimer(UINT nIDEvent) 
{
	CDialog::OnTimer(nIDEvent);
  g_ViewManager->Refresh();
  g_RaceManager.Keyboard();
}

void CDlgTrackEditor::OnSize(UINT nType, int cx, int cy) 
{
  CDlgRarsCommon::OnSize(nType, cx, cy);
}

BOOL CDlgTrackEditor::PreTranslateMessage(MSG* pMsg) 
{
  return CDlgRarsCommon::PreTranslateMessage(pMsg); 
}

//-----------------------------------------------------------------------------

void CDlgTrackEditor::OnFileLoad() 
{
  CDlgTrackLoad dlg;
  int res = dlg.DoModal();
  if( res==IDOK )
  {
    g_TrackEditor.Load( dlg.m_sTrackName );
  }
}

/**
 * Refresh the list 
 */
void CDlgTrackEditor::Refresh( bool bRebuildList )
{
  if( bRebuildList )
  {
    m_ListSection.ResetContent();
    m_ListSeg.ResetContent();
  }

  if( currentTrack!=NULL )
  {
    // Fill the list of track segments
    char s[64];
    int i;

    if( bRebuildList )
    {
      for( i=0; i<currentTrack->m_iNumSection; i++ )
      {
        m_ListSection.AddString( currentTrack->m_aSection[i].m_sName );
      }
      for( i=0; i<currentTrack->m_iNumSegment; i++ )
      {
        sprintf( s, "%d", i );
        m_ListSeg.AddString( s );
      }
    }

    g_TrackEditor.CheckCurrent();

    m_ListSection.SetCurSel( g_TrackEditor.m_iCurSection );
    m_ListSeg.SetCurSel( g_TrackEditor.m_iCurSegment );

    segment * seg = &(currentTrack->rgtwall[g_TrackEditor.m_iCurSegment]);
    Segment3D * seg3D = &(currentTrack->m_aSeg[g_TrackEditor.m_iCurSegment]);

    // Refresh the 3 lists of the dialog box 
    g_TrackEditor.m_Segment = *seg3D;
    PropertyRefresh( &m_CenterList, g_aSegProp, &g_TrackEditor.m_Segment, &(currentTrack->m_oDefault) );
    PropertyRefresh( &m_LeftList, g_aSegSideProp, &g_TrackEditor.m_Segment.lft, &(currentTrack->m_oDefault.lft) );
    PropertyRefresh( &m_RightList, g_aSegSideProp, &g_TrackEditor.m_Segment.rgt, &(currentTrack->m_oDefault.rgt) );
    g_TrackEditor.EditProperty();

    m_ClipBoardStatic.SetWindowText( g_TrackEditor.m_sClipBoardText );
  }
}

/**
 * Changing the section
 */
void CDlgTrackEditor::OnSelchangeSectionList() 
{
  g_TrackEditor.m_iCurSection = max( m_ListSection.GetCurSel(), 0 );
  g_TrackEditor.m_iCurSegment = currentTrack->m_aSection[g_TrackEditor.m_iCurSection].m_iFirstSeg;
  Refresh( false );
}

/**
 * Changing the segment
 */
void CDlgTrackEditor::OnSelchangeSegmentList() 
{
  g_TrackEditor.m_iCurSegment = max( m_ListSeg.GetCurSel(), 0 );
  Refresh( false );
}

void CDlgTrackEditor::OnSectionNew() 
{
  if( currentTrack )
  {
    CDlgTrackSection dlg;
    int iSeg = g_TrackEditor.m_iCurSegment;
    dlg.m_iSeg = iSeg;

    if( dlg.DoModal() == IDOK )
    {
      g_TrackEditor.m_oActionManager.Do( new ActionSegmentNew(currentTrack->m_iNumSegment, dlg.m_sSection) );
      Refresh();
    }
  }
}

void CDlgTrackEditor::OnTrackProperty() 
{
  if( currentTrack )
  {
    CDlgTrackProperty dlg;
    dlg.DoModal();
    g_TrackEditor.Rebuild();
  }
}

/**
 * Exiting
 */
void CDlgTrackEditor::OnCancel() 
{
	if( !g_TrackEditor.m_oActionManager.IsEmpty() )
  {
    switch( MessageBox("Do you want to save ?", "Save on exit", MB_YESNOCANCEL) )
    {
      case IDCANCEL: 
        return;
      case IDYES:
        currentTrack->SaveXml();
        break;
    }
  }
	CDialog::OnCancel();
}

//-----------------------------------------------------------------------------------------
// Edition of the properties (with the help of the lists)
//-----------------------------------------------------------------------------------------

/**
 * Refresh one list of properties
 */
void CDlgTrackEditor::PropertyRefresh( CListBox * list, ReflectionProperty * a, void * struct_base, void * default_base )
{
  char buffer2[256];

  if( g_TrackEditor.m_pStructProp==NULL )
  {
    m_PropName.SetWindowText( "" );
    m_Description.SetWindowText( "" );
    m_PropValue.SetWindowText( "" );
  }

  list->ResetContent();

  int i=0;
  while( a[i].sXmlTag!=NULL )
  {
    char * buffer = a[i].GetString(struct_base);
    bool bDefault = a[i].IsDefault(struct_base, default_base);

    if( bDefault )
    {
      sprintf( buffer2, "%s = %s (default)", a[i].sXmlTag, buffer );
    }
    else
    {
      sprintf( buffer2, "%s = %s", a[i].sXmlTag, buffer );
    }
    int pos = list->AddString( buffer2 );
    list->SetItemData( pos, (DWORD)&(a[i]) ); 
    i++;
  }
}

/**
 * Change the current edited properties when clicking on a list
 */
void CDlgTrackEditor::EditProperty( CListBox * list, void * struct_base, void * default_base )
{
  int pos = list->GetCurSel();
  ReflectionProperty * prop = (ReflectionProperty *) list->GetItemData(pos);

  g_TrackEditor.m_pStructProp  = prop;
  g_TrackEditor.m_pStructBase  = struct_base;
  g_TrackEditor.m_pDefaultBase = default_base;
  g_TrackEditor.EditProperty();
}

void CDlgTrackEditor::OnSelchangeCenterList() 
{
  EditProperty( &m_CenterList, &g_TrackEditor.m_Segment, &(currentTrack->m_oDefault) );
}

void CDlgTrackEditor::OnSelchangeLeftList() 
{
  EditProperty( &m_LeftList, &g_TrackEditor.m_Segment.lft, &(currentTrack->m_oDefault.lft) );
}

void CDlgTrackEditor::OnSelchangeRightList() 
{
  EditProperty( &m_RightList, &g_TrackEditor.m_Segment.rgt, &(currentTrack->m_oDefault.rgt) );
}

//---------------------------------------------------------------------------
// Call to the Portable code
//---------------------------------------------------------------------------

void CDlgTrackEditor::OnFileNew() 
{
  g_TrackEditor.OnFileNew();
}

void CDlgTrackEditor::OnFileSave() 
{
  g_TrackEditor.OnFileSave();
}

void CDlgTrackEditor::OnFileClose() 
{
  g_TrackEditor.OnFileClose();
}

void CDlgTrackEditor::OnEditUndo() 
{
  g_TrackEditor.OnEditUndo();
}

void CDlgTrackEditor::OnEditRedo() 
{
  g_TrackEditor.OnEditRedo();
}

void CDlgTrackEditor::OnPropSet() 
{
  g_TrackEditor.OnPropSet();
}

void CDlgTrackEditor::OnPropReset() 
{
  g_TrackEditor.OnPropReset();
}

void CDlgTrackEditor::OnSegmentNew() 
{
  g_TrackEditor.OnSegmentNew();
}

void CDlgTrackEditor::OnSegmentDelete() 
{
  g_TrackEditor.OnSegmentDelete();
}

void CDlgTrackEditor::OnSegmentInfo() 
{
  g_TrackEditor.OnSegmentInfo();
}

void CDlgTrackEditor::OnSegmentNxt() 
{
  g_TrackEditor.OnSegmentNxt();
}

void CDlgTrackEditor::OnSegmentPrv() 
{
  g_TrackEditor.OnSegmentPrv();
}

void CDlgTrackEditor::OnEditCopy() 
{
  g_TrackEditor.OnEditCopy();
}

void CDlgTrackEditor::OnEditPaste() 
{
  g_TrackEditor.OnEditPaste();
}

void CDlgTrackEditor::OnEditPasteAll() 
{
  g_TrackEditor.OnEditPasteAll();
}

void CDlgTrackEditor::OnViewTrack2d() 
{
  g_TrackEditor.OnViewTrack2d();
}

void CDlgTrackEditor::OnViewTrack3ds() 
{
  g_TrackEditor.OnViewTrack3ds();
}

void CDlgTrackEditor::OnViewTrackOneSeg() 
{
  g_TrackEditor.OnViewTrackOneSeg();
}

void CDlgTrackEditor::OnTrackGen3d() 
{
  g_TrackEditor.OnTrackGen3d();
}

void CDlgTrackEditor::OnTrackAutoclosure() 
{
  g_TrackEditor.OnTrackAutoclosure();
}


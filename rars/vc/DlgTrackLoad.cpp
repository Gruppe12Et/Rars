// DlgTrackLoad.cpp : implementation file
//

#include "stdafx.h"
#include "vc_rars.h"
#include "DlgTrackLoad.h"
#include <direct.h>

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackLoad dialog


CDlgTrackLoad::CDlgTrackLoad(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrackLoad::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrackLoad)
	m_sTrackName = _T("");
	//}}AFX_DATA_INIT
}


void CDlgTrackLoad::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrackLoad)
	DDX_Control(pDX, IDC_TRACK_NAME, m_TrackList);
	DDX_LBString(pDX, IDC_TRACK_NAME, m_sTrackName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrackLoad, CDialog)
	//{{AFX_MSG_MAP(CDlgTrackLoad)
	ON_LBN_DBLCLK(IDC_TRACK_NAME, OnDblclkTrackName)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackLoad message handlers

BOOL CDlgTrackLoad::OnInitDialog() 
{
	CDialog::OnInitDialog();
	
  m_TrackList.Dir( 0, "tracks/*.tr*" );
    
  if( m_TrackList.GetCount()==0 )
  {
    // We are in a wrong directory
    // Just to be sure, go in upper directory
    chdir( ".." );
    m_TrackList.Dir( 0, "tracks/*.tr*" );
  }
	
	return TRUE;  
}

void CDlgTrackLoad::OnDblclkTrackName() 
{
	OnOK();
}

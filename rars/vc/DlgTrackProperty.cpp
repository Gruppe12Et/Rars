// DlgTrackProperty.cpp : implementation file
//

#include "stdafx.h"
#include "vc_rars.h"
#include "track.h"
#include "DlgTrackProperty.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackProperty dialog


CDlgTrackProperty::CDlgTrackProperty(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrackProperty::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrackProperty)
	m_sName         = currentTrack->m_sFileName;
	//}}AFX_DATA_INIT
}


void CDlgTrackProperty::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrackProperty)
	DDX_Control(pDX, IDC_PROP_LIST, m_PropList);
	DDX_Control(pDX, IDC_DESCRIPTION, m_Description);
	DDX_Control(pDX, IDC_PROP_NAME, m_PropName);
	DDX_Control(pDX, IDC_PROP_VALUE, m_PropValue);
	DDX_Text(pDX, IDC_TRACK_NAME, m_sName);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrackProperty, CDialog)
	//{{AFX_MSG_MAP(CDlgTrackProperty)
	ON_BN_CLICKED(IDC_PROP_SET, OnPropSet)
	ON_BN_CLICKED(IDC_PROP_RESET, OnPropReset)
	ON_LBN_SELCHANGE(IDC_PROP_LIST, OnSelchangePropList)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackProperty message handlers

BOOL CDlgTrackProperty::OnInitDialog() 
{
	CDialog::OnInitDialog();
  m_pStructProp = NULL;
  m_Track = *currentTrack;
  m_Track.m_bCopy = TRUE;
  Refresh();

	return TRUE;
}

void CDlgTrackProperty::Refresh() 
{
  m_PropList.ResetContent();
  PropertyRefresh( "info", g_aTrackInfoProp );
  PropertyRefresh( "unit", g_aTrackUnitProp );
  PropertyRefresh( "header", g_aTrackHeaderProp );
  PropertyRefresh( "xwindow", g_aTrackXWindowProp );
  PropertyRefresh( "pitstop", g_aTrackPitstopProp );
  EditProperty();
}

/**
 * Fill the list with the properties
 */
void CDlgTrackProperty::PropertyRefresh( const char * tag, ReflectionProperty * a )
{
  char buffer2[256];

  int i=0;
  while( a[i].sXmlTag!=NULL )
  {
    char * buffer = a[i].GetString(&m_Track);
    bool bDefault = a[i].IsDefault(&m_Track, &m_DefaultTrack);

    if( bDefault )
    {
      sprintf( buffer2, "%s: %s = %s (default)", tag, a[i].sXmlTag, buffer );
    }
    else
    {
      sprintf( buffer2, "%s: %s = %s", tag, a[i].sXmlTag, buffer );
    }
    int pos = m_PropList.AddString( buffer2 );
    m_PropList.SetItemData( pos, (DWORD)&(a[i]) ); 
    i++;
  }
}

/**
 * Called when clicking on the button set
 */
void CDlgTrackProperty::OnPropSet() 
{
  if( m_pStructProp!=NULL )
  {
    char buffer[256]; 
    m_PropValue.GetWindowText( buffer, 256 );
    m_pStructProp->SetString( m_pStructBase, buffer );
    Refresh();
  }
}

/**
 * Called when clicking on the button set
 */
void CDlgTrackProperty::OnPropReset() 
{
  if( m_pStructProp!=NULL )
  {
    m_pStructProp->CopyValue( m_pStructBase, m_pDefaultBase );
    Refresh();
  }
}


void CDlgTrackProperty::OnSelchangePropList() 
{
  int pos = m_PropList.GetCurSel();
  ReflectionProperty * prop = (ReflectionProperty *) m_PropList.GetItemData(pos);

  m_pStructProp  = prop;
  m_pStructBase  = &m_Track;
  m_pDefaultBase = &m_DefaultTrack;
  EditProperty();
}

void CDlgTrackProperty::EditProperty() 
{
  if( m_pStructProp!=NULL )
  {
    m_PropName.SetWindowText( m_pStructProp->sXmlTag );
    m_Description.SetWindowText( m_pStructProp->sDescription );
    m_PropValue.SetWindowText( m_pStructProp->GetString(m_pStructBase) );
    m_PropValue.SetFocus();
  }
}

void CDlgTrackProperty::OnOK() 
{
	UpdateData( TRUE );
  strcpy( m_Track.m_sFileName, m_sName );

  *currentTrack = m_Track;
	CDialog::OnOK();
}

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
#include "../misc.h"
#include "../track.h"
#include "DlgTrackSection.h"

#ifdef _DEBUG
#define new new(__FILE__, __LINE__)
#endif

//--------------------------------------------------------------------------
//                        Class CDlgTrackSection
//--------------------------------------------------------------------------

CDlgTrackSection::CDlgTrackSection(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgTrackSection::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgTrackSection)
	m_sSection = _T("");
	//}}AFX_DATA_INIT
  m_iSeg = 0;
}


void CDlgTrackSection::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgTrackSection)
	DDX_Text(pDX, IDC_SECTION, m_sSection);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgTrackSection, CDialog)
	//{{AFX_MSG_MAP(CDlgTrackSection)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackSection message handlers


void CDlgTrackSection::OnOK() 
{
  UpdateData( true );

  // The name of section is the the one of the current segment
  for( int i=0; i<currentTrack->m_iNumSection; i++ )
  {
    if( strcmp(currentTrack->m_aSection[i].m_sName, m_sSection)==0 )
    {
      warning("The name of the section exists already ." );
      return;
    }
  }

  CDialog::OnOK();
}

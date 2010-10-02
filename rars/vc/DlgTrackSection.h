#if !defined(AFX_DLGTRACKSECTION_H__0B087D60_F16E_11D5_80FB_0010A7087511__INCLUDED_)
#define AFX_DLGTRACKSECTION_H__0B087D60_F16E_11D5_80FB_0010A7087511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTrackSection.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackSection dialog

class CDlgTrackSection : public CDialog
{
// Construction
public:
	CDlgTrackSection(CWnd* pParent = NULL);   // standard constructor

  int m_iSeg;

// Dialog Data
	//{{AFX_DATA(CDlgTrackSection)
	enum { IDD = IDD_TRACK_SECTION };
	CString	m_sSection;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrackSection)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrackSection)
	virtual void OnOK();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACKSECTION_H__0B087D60_F16E_11D5_80FB_0010A7087511__INCLUDED_)

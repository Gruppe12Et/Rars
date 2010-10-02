#if !defined(AFX_DLGTRACKLOAD_H__C0E053E2_B5B6_11D5_80FB_0010A7087511__INCLUDED_)
#define AFX_DLGTRACKLOAD_H__C0E053E2_B5B6_11D5_80FB_0010A7087511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTrackLoad.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackLoad dialog

class CDlgTrackLoad : public CDialog
{
// Construction
public:
	CDlgTrackLoad(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgTrackLoad)
	enum { IDD = IDD_TRACK_LOAD };
	CListBox	m_TrackList;
	CString	m_sTrackName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrackLoad)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrackLoad)
	virtual BOOL OnInitDialog();
	afx_msg void OnDblclkTrackName();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACKLOAD_H__C0E053E2_B5B6_11D5_80FB_0010A7087511__INCLUDED_)

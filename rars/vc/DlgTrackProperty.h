#if !defined(AFX_DLGTRACKPROPERTY_H__C0E053E0_B5B6_11D5_80FB_0010A7087511__INCLUDED_)
#define AFX_DLGTRACKPROPERTY_H__C0E053E0_B5B6_11D5_80FB_0010A7087511__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000
// DlgTrackProperty.h : header file
//

/////////////////////////////////////////////////////////////////////////////
// CDlgTrackProperty dialog

class CDlgTrackProperty : public CDialog
{
// Construction
public:
	CDlgTrackProperty(CWnd* pParent = NULL);   // standard constructor
  void Refresh();
  void PropertyRefresh( const char * tag, ReflectionProperty * a );
  void EditProperty();

  // Current property in the lists
  Track m_Track, m_DefaultTrack;

  ReflectionProperty * m_pStructProp;
  void * m_pStructBase;
  void * m_pDefaultBase;

// Dialog Data
	//{{AFX_DATA(CDlgTrackProperty)
	enum { IDD = IDD_TRACK_PROPERTY };
	CListBox	m_PropList;
	CStatic	m_Description;
	CStatic	m_PropName;
	CEdit	m_PropValue;
	CString	m_sName;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrackProperty)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrackProperty)
	virtual void OnOK();
	afx_msg void OnPropSet();
	afx_msg void OnPropReset();
	virtual BOOL OnInitDialog();
	afx_msg void OnSelchangePropList();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_DLGTRACKPROPERTY_H__C0E053E0_B5B6_11D5_80FB_0010A7087511__INCLUDED_)

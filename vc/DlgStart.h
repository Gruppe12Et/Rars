// DlgStartRace.h : header file
//

#ifndef __DLGSTARTRACE_H
#define __DLGSTARTRACE_H
/////////////////////////////////////////////////////////////////////////////
// CDlgStartRace dialog

class CDlgStartRace : public CDialog
{
// Construction
public:
	CDlgStartRace(CWnd* pParent = NULL);   // standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgStartRace)
	enum { IDD = IDD_StartRace };
	CEdit	m_EditLaps;
	CSpinButtonCtrl	m_QualifLaps;
	CSpinButtonCtrl	m_NumberQualif;
	CSpinButtonCtrl	m_RaceLaps;
	CComboBox	m_Surface;
	CListBox	m_ListAvailable;
	CListBox	m_ListSelected;
	CComboBox	m_ComboFollowCar;
	CComboBox	m_ComboMovie;
	CComboBox	m_ComboOpenGL;
	CEdit	m_EditLastResult;
	CComboBox	m_ComboTrack;
	CString	m_sTrackName;
	int		m_iLaps;
	int		m_iRandom;
	CString	m_LastResult;
	int		m_iQualifLaps;
	int		m_iQualifNumber;
	int		m_iRadioQualif;
	CString	m_sOpenGL;
	CString	m_sMovie;
	CString	m_sFollowCar;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgStartRace)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL
	CFont m_Font;

// Implementation
protected:
	HICON   m_hIcon;

  void moveOneDriver(CListBox& source, CListBox& target);
  void moveAllDrivers(CListBox& source, CListBox& target);

  // Generated message map functions
	//{{AFX_MSG(CDlgStartRace)
	virtual void OnOK();
	virtual BOOL OnInitDialog();
	afx_msg void OnRandomRace();
	afx_msg void OnQualif();
	afx_msg void OnOrderRace();
	afx_msg void OnQualifOnly();
	afx_msg void OnUsePrevQualif();
	afx_msg void OnSelchangeTrackName();
	afx_msg void OnButtonRandom();
	afx_msg void OnDriverSelectAll();
	afx_msg void OnDriverRemoveOne();
	afx_msg void OnDriverRemoveAll();
	afx_msg void OnDriverSelectOne();
	afx_msg void OnDblclkDriverAvailable();
	afx_msg void OnDblclkDriverSelected();
	afx_msg void OnSelchangeMovie();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
private:

  // Disable some controls when Movie playback is selected
  void SetPlaybackMode(bool enable);

  int m_iNumLaps;
  bool isMovieSelected;
};

#endif // __DLGSTARTRACE_H

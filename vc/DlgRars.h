/**
 * DlgRars.h - Main Windows dialog of Rars
 *
 * History
 *  ver. 0.64  creation
 *  ver. 0.90  common parent class with CDlgTrackEditor
 *
 * @author    Marc Gueury (mgueury@skynet.be)
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef __DLGRARS_H__
#define __DLGRARS_H__

//--------------------------------------------------------------------------
//                              T Y P E S
//--------------------------------------------------------------------------

/**
 * Common Rars dialog variable, used by 
 * - the real Rars Dialog box
 * - the track editor
 */
class CDlgRarsCommon : public CDialog
{
  public:
    CDlgRarsCommon( UINT nIDTemplate, CWnd* pParentWnd = NULL );

    int     m_fOffX;
    int     m_fOffY;

    HBITMAP m_hBitmap2D;
    HBITMAP m_hBitmapClassic;
    HBITMAP m_hBitmapBoard;

    CStatic m_StaticBitmap2D;
    CStatic m_StaticBitmapClassic;
    CStatic m_StaticBitmapBoard;

    BOOL  m_bRectInit;
    CRect m_OrigClientRect,
          m_OrigStaticBitmap2D,
          m_OrigStaticBitmapClassic,
          m_OrigStaticBitmapBoard;

    HICON   m_hIcon;
    BOOL    m_bTimerInit;
    HACCEL  m_hAccel;

    BOOL OnInitDialog();
    void OnPaint();
    void InitDialog( int iViewMode );
    void InitDialogPart2();
    void SetViewMode( int mode );
    void OnSize(UINT nType, int cx, int cy); 

  	virtual BOOL PreTranslateMessage(MSG* pMsg);
};

/**
 * The rars main dialog box
 */
class CDlgRars : public CDlgRarsCommon
{
// Construction
public:
	CDlgRars(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CDlgRars)
	enum { IDD = IDD_RARS_DIALOG };
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgRars)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

  // Implementation
  public:
    void Refresh();

protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgRars)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnTimer(UINT nIDEvent);
	virtual void OnCancel();
	virtual void OnOK();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

#endif // __DLGRARS_H__

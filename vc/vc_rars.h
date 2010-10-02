//--------------------------------------------------------------------------
//
//    FILE: Vc_rars.h (VC++ specific)
//
//--------------------------------------------------------------------------

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#ifndef __VC_RARS_H
#define __VC_RARS_H

#include "stdafx.h"
#include "resource.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#define APP ((CRarsApp *)AfxGetApp())

//--------------------------------------------------------------------------
//                            E X T E R N
//--------------------------------------------------------------------------

class CDlgRarsCommon;

// VC_LOWGR.CPP
void vc_main_part0( CDlgRarsCommon * _pDlg );
void vc_main_part1();
BOOL vc_main_part2();
void vc_main_part3();
void ResizeAllViews( int x, int y );

//--------------------------------------------------------------------------
//                            T Y P E S
//--------------------------------------------------------------------------

class CRarsApp : public CWinApp
{
public:
	CRarsApp();

// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CRarsApp)
	public:
	virtual BOOL InitInstance();
	//}}AFX_VIRTUAL

// Implementation
  CDlgRarsCommon * m_pDlgRars;

	//{{AFX_MSG(CRarsApp)
	afx_msg void OnSpeedFast();
	afx_msg void OnSpeedMedium();
	afx_msg void OnSpeedSlow();
	afx_msg void OnCarNextCar();
	afx_msg void OnCarPrvCar();
	afx_msg void OnCarAnyOvertaking();
	afx_msg void OnCarForPosition();
	afx_msg void OnZoomIn();
	afx_msg void OnZoomOut();
	afx_msg void OnViewClassic();
	afx_msg void OnViewOpenglFullscreen();
	afx_msg void OnViewOpenglWindow();
	afx_msg void OnViewTelemetry();
	afx_msg void OnViewZoom();
	afx_msg void OnSettingsSave();
	afx_msg void OnCarManual();
	afx_msg void OnOpenglView();
	afx_msg void OnSpeedRewind();
	afx_msg void OnSpeedStop();
	afx_msg void OnViewTrajectory();
	afx_msg void OnViewDriverNames();
	afx_msg void OnCarNobody();
	afx_msg void OnMoveDown();
	afx_msg void OnMoveLeft();
	afx_msg void OnMoveRight();
	afx_msg void OnMoveUp();
	afx_msg void OnRotateDown();
	afx_msg void OnRotateLeft();
	afx_msg void OnRotateRight();
	afx_msg void OnRotateUp();
	afx_msg void OnRotateReset();
	afx_msg void OnSpeedPause();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//--------------------------------------------------------------------------
//                          G L O B A L S
//--------------------------------------------------------------------------

extern CRarsApp theApp;


/////////////////////////////////////////////////////////////////////////////

#endif // __VC_RARS_H
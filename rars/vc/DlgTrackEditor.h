/**
 * DlgTrackEditor.h - Windows dialog for the track editor
 *
 * History
 *  ver. 0.9 creation
 *
 * @author    Marc Gueury (mgueury@skynet.be)
 * @see:      C++ Coding Standard and CCDOC in help.htm
 * @version   0.90
 */

#ifndef __DLGTRACKEDITOR_H__
#define __DLGTRACKEDITOR_H__

//--------------------------------------------------------------------------
//                           I N C L U D E
//--------------------------------------------------------------------------

#include "../track.h"
#include "DlgRars.h"

//--------------------------------------------------------------------------
//                            D E F I N E
//--------------------------------------------------------------------------

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

//--------------------------------------------------------------------------
//                       Class CDlgTrackEditor
//--------------------------------------------------------------------------

/**
 * Rars Track editor
 */
class CDlgTrackEditor : public CDlgRarsCommon
{
// Construction
public:
	CDlgTrackEditor(CWnd* pParent = NULL);   
	~CDlgTrackEditor();   
  void Load( const char * name );
  void Refresh( bool bRebuildList=true );

  void EditProperty( CListBox * list, void * struct_base, void * default_base );
  void PropertyRefresh( CListBox * list, ReflectionProperty * a, void * struct_base, void * default_base );

// Dialog Data
	//{{AFX_DATA(CDlgTrackEditor)
	enum { IDD = IDD_TRACK_EDITOR };
	CStatic	m_ClipBoardStatic;
	CListBox	m_ListSection;
	CStatic	m_Description;
	CEdit	m_PropValue;
	CStatic	m_PropName;
	CListBox	m_CenterList;
	CListBox	m_RightList;
	CListBox	m_LeftList;
	CListBox	m_ListSeg;
	CString	m_sSegList;
	CString	m_sSectionList;
	//}}AFX_DATA


// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CDlgTrackEditor)
	public:
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:

	// Generated message map functions
	//{{AFX_MSG(CDlgTrackEditor)
	afx_msg void OnTrackGen3d();
	afx_msg void OnSelchangeSegmentList();
	virtual BOOL OnInitDialog();
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnPaint();
	afx_msg void OnTimer(UINT nIDEvent);
	afx_msg void OnEditUndo();
	afx_msg void OnEditRedo();
	afx_msg void OnSegmentNew();
	afx_msg void OnSegmentDelete();
	afx_msg void OnFileNew();
	afx_msg void OnTrackProperty();
	afx_msg void OnViewTrackOneSeg();
	afx_msg void OnViewTrack2d();
	afx_msg void OnViewTrack3ds();
	virtual void OnCancel();
	afx_msg void OnSelchangeCenterList();
	afx_msg void OnSelchangeLeftList();
	afx_msg void OnSelchangeRightList();
	afx_msg void OnPropSet();
	afx_msg void OnPropReset();
	afx_msg void OnEditCopy();
	afx_msg void OnEditPaste();
	afx_msg void OnSegmentNxt();
	afx_msg void OnSegmentPrv();
	afx_msg void OnSelchangeSectionList();
	afx_msg void OnSectionNew();
	afx_msg void OnEditPasteAll();
	afx_msg void OnTrackAutoclosure();
	afx_msg void OnSegmentInfo();
	afx_msg void OnFileClose();
	afx_msg void OnFileLoad();
	afx_msg void OnFileSave();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(__DLGTRACKEDITOR_H__)

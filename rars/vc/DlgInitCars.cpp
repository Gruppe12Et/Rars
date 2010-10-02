/**
 * DlgInitCars.cpp : dialog box shown during the initialisation of the 
 *                   cars.
 */

#include "stdafx.h"
#include "vc_rars.h"
#include "DlgInitCars.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

/////////////////////////////////////////////////////////////////////////////
// CDlgInitCars dialog


CDlgInitCars::CDlgInitCars(CWnd* pParent /*=NULL*/)
	: CDialog(CDlgInitCars::IDD, pParent)
{
	//{{AFX_DATA_INIT(CDlgInitCars)
		// NOTE: the ClassWizard will add member initialization here
	//}}AFX_DATA_INIT
}


void CDlgInitCars::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	//{{AFX_DATA_MAP(CDlgInitCars)
	DDX_Control(pDX, IDC_LIST_MESSAGE, m_ListMessage);
	//}}AFX_DATA_MAP
}


BEGIN_MESSAGE_MAP(CDlgInitCars, CDialog)
	//{{AFX_MSG_MAP(CDlgInitCars)
		// NOTE: the ClassWizard will add message map macros here
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CDlgInitCars message handlers

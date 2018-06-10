// PrjCleanerDlg.h : header file
//

#if !defined(AFX_PRJCLEANERDLG_H__560BB49A_9973_497B_ABE4_E5F96B3A11AE__INCLUDED_)
#define AFX_PRJCLEANERDLG_H__560BB49A_9973_497B_ABE4_E5F96B3A11AE__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

/////////////////////////////////////////////////////////////////////////////
// CPrjCleanerDlg dialog

#include "BrowseCtrl.h"
#include "PrjCleanerThread.h"

class CPrjCleanerDlg : public CDialog
{
// Construction
public:
	CPrjCleanerDlg(CWnd* pParent = NULL);	// standard constructor

// Dialog Data
	//{{AFX_DATA(CPrjCleanerDlg)
	enum { IDD = IDD_PRJCLEANER_DIALOG };
	CStatic	m_wndStatus;
	CButton	m_wndRun;
	CBrowseCtrl	m_wndPath;
	//}}AFX_DATA

	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CPrjCleanerDlg)
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV support
	//}}AFX_VIRTUAL

// Implementation
protected:
	HICON m_hIcon;

	// Generated message map functions
	//{{AFX_MSG(CPrjCleanerDlg)
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	afx_msg void OnRun();
	virtual void OnCancel();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()

protected:
	static CString ThousandsFormat(DWORD dwNum);
	LRESULT OnMsgFinish(WPARAM wParam, LPARAM lParam);
	LRESULT OnMsgProc(WPARAM wParam, LPARAM lParam);
	
	CPrjCleanerThread m_thread;
};

//{{AFX_INSERT_LOCATION}}
// Microsoft Visual C++ will insert additional declarations immediately before the previous line.

#endif // !defined(AFX_PRJCLEANERDLG_H__560BB49A_9973_497B_ABE4_E5F96B3A11AE__INCLUDED_)

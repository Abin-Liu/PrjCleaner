// PrjCleanerThread.h: interface for the CPrjCleanerThread class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_PRJCLEANERTHREAD_H__E6919E09_FDC1_4E36_9817_A881CBF91210__INCLUDED_)
#define AFX_PRJCLEANERTHREAD_H__E6919E09_FDC1_4E36_9817_A881CBF91210__INCLUDED_

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include "Thread.h"

class CPrjCleanerThread : public CThread  
{
public:
	CPrjCleanerThread();
	DWORD Start(LPCTSTR lpszPath, HWND hWnd, UINT nMsgProc, UINT nMsgFinish);

private:
	VOID OnStop();
	void Cleanup(LPCTSTR lpszPath);
	DWORD ThreadProc();
	HWND m_hWnd;
	UINT m_nMsgProc;
	UINT m_nMsgFinish;
	CString m_sPath;
	DWORD m_dwCount;
	DWORD m_dwSize;
};

#endif // !defined(AFX_PRJCLEANERTHREAD_H__E6919E09_FDC1_4E36_9817_A881CBF91210__INCLUDED_)

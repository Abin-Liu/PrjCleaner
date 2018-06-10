// PrjCleanerThread.cpp: implementation of the CPrjCleanerThread class.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "PrjCleaner.h"
#include "PrjCleanerThread.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CPrjCleanerThread::CPrjCleanerThread()
{
	m_hWnd = NULL;
	m_nMsgProc = 0;
	m_nMsgFinish = 0;
	m_dwCount = 0;
	m_dwSize = 0;
}

void CPrjCleanerThread::Cleanup(LPCTSTR lpszPath)
{
	::SendMessage(m_hWnd, m_nMsgProc, 0, (LPARAM)lpszPath);
	CFileFind ff;
	BOOL bOK = ff.FindFile(CString(lpszPath) + "\\*.*");
	while (bOK && !IsStopping())
	{
		bOK = ff.FindNextFile();
		if (ff.IsDots())
			continue;

		if (ff.IsDirectory())
			Cleanup(ff.GetFilePath());

		CString sExt = ff.GetFileName().Right(4);
		sExt.MakeLower();

		if (sExt == ".obj" || sExt == ".ilk" || sExt == ".pch" || sExt == ".pdb" || sExt == ".res" || sExt == ".idb" || sExt == ".sbr" || sExt == ".bsc")
		{
			m_dwCount++;
			m_dwSize += ff.GetLength();
			::DeleteFile(ff.GetFilePath());
		}
	}
}

DWORD CPrjCleanerThread::ThreadProc()
{
	m_dwCount = 0;
	m_dwSize = 0;
	Cleanup(m_sPath);
	return 0;
}

DWORD CPrjCleanerThread::Start(LPCTSTR lpszPath, HWND hWnd, UINT nMsgProc, UINT nMsgFinish)
{
	m_hWnd = hWnd;
	m_nMsgProc = nMsgProc;
	m_nMsgFinish = nMsgFinish;
	m_dwCount = 0;
	m_dwSize = 0;
	m_sPath = lpszPath;
	m_sPath.TrimRight("\\");
	return CThread::Start();
}

VOID CPrjCleanerThread::OnStop()
{
	::PostMessage(m_hWnd, m_nMsgFinish, (WPARAM)m_dwCount, (LPARAM)m_dwSize);
}

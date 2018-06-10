//////////////////////////////////////////////////////////////////////
// Thread.cpp
//
// CThread, a Win32 API wrapping class for basic thread operations.
// This is a pure virtual class, you need to derive your own class
// from CThread and implement the "ThreadProc" member function.
//
// After enough consideration I decided not to support suspending &
// resuming methods. In my opinion those are the most dangerous things
// your can ever do to your threads, suspending a thread may cause
// unpredicted results, for example, a thread can be suspended before
// releasing ownership of synchronization objects, in which case
// deadlocks occur.
//
// If, however, you must maintain suspending & resuming, you are free
// to do so in your own CThread derived classes, and of course do it
// at your own risk...
// 
// Abin (abinn32@yahoo.com)
// 4th May, 2006
//////////////////////////////////////////////////////////////////////
#include "stdafx.h" // Include if needed
#include "Thread.h"
#include <assert.h>

/////////////////////////////////////////////////////////////////////////////////
// CThread Implementation
/////////////////////////////////////////////////////////////////////////////////
CThread::CThread():
m_hThread(NULL), m_bAbort(FALSE), m_bNormalStopped(FALSE), m_ncsType(CS_NONE), m_pcs(NULL)
{
}

CThread::~CThread()
{
	Stop(); // Stop the thread if it's running
	::CloseHandle(m_hThread);

	// If we already have an internal CRITICAL_SECTION, delete it.
	if (m_ncsType == CS_INTERNAL && m_pcs)
	{
		::DeleteCriticalSection(m_pcs);
		delete m_pcs;
	}
}

DWORD CThread::Start(int nPriority)
{
	if (IsRunning())
	{
		::SetLastError(ERROR_TOO_MANY_TCBS);
		return 0;
	}
	
	m_bAbort = FALSE;
	::CloseHandle(m_hThread);
	DWORD dwThreadID = 0;	
	
	// Create the thread
	m_hThread = ::CreateThread(NULL, 0, _CTHREAD_InternalProc, (LPVOID)this, 0, &dwThreadID);
	if (m_hThread == NULL)
		dwThreadID = 0;
	else
		::SetThreadPriority(m_hThread, nPriority);

	return dwThreadID;	
}

VOID CThread::Stop(DWORD dwTimeoutMilliseconds)
{
	// terminate the thread if it's running
	m_bAbort = TRUE; // Set the stopping flag
	BOOL bTimeout = (::WaitForSingleObject(m_hThread, dwTimeoutMilliseconds) == WAIT_TIMEOUT);
	if (bTimeout)
	{
		::TerminateThread(m_hThread, -1); // Terminate if timeout, this is rude, though.

		if (!m_bNormalStopped)
		{
			m_bNormalStopped = TRUE;
			OnStop();
		}
	}

	m_bAbort = FALSE;
	if (bTimeout)
		::SetLastError(ERROR_TIMEOUT);
}

BOOL CThread::IsStopping() const
{
	return m_bAbort;
}

DWORD CThread::_CTHREAD_InternalProc(LPVOID lpParameter)
{
	CThread* pThread = (CThread*)lpParameter;
	assert (pThread);

	pThread->OnStart();
	pThread->m_bNormalStopped = FALSE;

	// Call the thread proc
	DWORD dwExitCode = pThread->ThreadProc();

	if (!pThread->m_bNormalStopped)
	{
		pThread->m_bNormalStopped = TRUE;
		pThread->OnStop();
	}

	return dwExitCode;
}

DWORD CThread::IsRunning() const
{
	return GetExitCode() == STILL_ACTIVE;
}

BOOL CThread::SetPriority(int nPriority) const
{
	return ::SetThreadPriority(m_hThread, nPriority);
}

int CThread::GetPriority() const
{
	return ::GetThreadPriority(m_hThread);
}

VOID CThread::ConditionalSleep(DWORD dwMillseconds, DWORD dwMinimumMilliseconds) const
{
	// Only sleep as long as m_iAbort is zero. It may slow your application down
	// if dwMinimumMilliseconds is very small, so beware. It is recommended that
	// value of dwMinimumMilliseconds to be at least 50.
	if (dwMillseconds == 0 || m_bAbort)
		return;

	if (dwMinimumMilliseconds == 0)
		dwMinimumMilliseconds = 1;

	if (dwMinimumMilliseconds > dwMillseconds)
		dwMinimumMilliseconds = dwMillseconds;

	DWORD dwRemain = dwMillseconds % dwMinimumMilliseconds;
	DWORD dwSteps = dwMillseconds / dwMinimumMilliseconds;

	for (DWORD i = 0; !m_bAbort && i < dwSteps; i++)
		::Sleep(dwMinimumMilliseconds);

	if (!m_bAbort && dwRemain)
		::Sleep(dwRemain);	
}

DWORD CThread::GetExitCode() const
{
	DWORD dwCode = 0;
	::GetExitCodeThread(m_hThread, &dwCode);
	return dwCode;
}

BOOL CThread::SetCriticalSection(int nType, LPCRITICAL_SECTION pcs)
{
	// nType check
	if (nType != CS_NONE && nType != CS_EXTERNAL && nType != CS_INTERNAL)
	{
		assert(FALSE);
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	// External CRITICAL_SECTION? pcs must be a valid address
	if (nType == CS_EXTERNAL && pcs == NULL)
	{
		assert(FALSE);
		::SetLastError(ERROR_INVALID_PARAMETER);
		return FALSE;
	}

	if (m_ncsType == nType)
		return TRUE; // No changes required.

	// If we already have an internal CRITICAL_SECTION, delete it.
	if (m_ncsType == CS_INTERNAL && m_pcs)
	{
		::DeleteCriticalSection(m_pcs);
		delete m_pcs;
	}

	m_ncsType = nType;
	m_pcs = NULL;

	if (m_ncsType == CS_EXTERNAL)
	{
		// Assign an external CRITICAL_SECTION address
		m_pcs = pcs;
	}
	else if (m_ncsType == CS_INTERNAL)
	{
		assert(pcs == NULL);

		// Create an internal CRITICAL_SECTION
		m_pcs = new CRITICAL_SECTION;
		assert(m_pcs);
		::InitializeCriticalSection(m_pcs);
	}
	else
	{
		assert(pcs == NULL);
	}

	return TRUE;
}

VOID CThread::EnterCriticalSection()
{
	assert(m_pcs);
	::EnterCriticalSection(m_pcs);
}

VOID CThread::LeaveCriticalSection()
{
	assert(m_pcs);
	::LeaveCriticalSection(m_pcs);
}

VOID CThread::OnStart()
{
}

VOID CThread::OnStop()
{
}

/////////////////////////////////////////////////////////////////////////////////
// End of CThread Implementation
/////////////////////////////////////////////////////////////////////////////////
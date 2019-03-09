// NXLog.h: interface for the CNXLog class.
//
//////////////////////////////////////////////////////////////////////

#if !defined(AFX_NXLOG_H__DF9682BE_3FD5_4712_897D_22193AD50832__INCLUDED_)
#define AFX_NXLOG_H__DF9682BE_3FD5_4712_897D_22193AD50832__INCLUDED_

#include <windows.h>
#include <stdio.h>
#include <DbgHelp.h>
#include <tchar.h>

#pragma comment(lib, "DbgHelp.lib")
#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

class CMiniDump
{
public:
	CMiniDump();
	~CMiniDump();

	static LONG WINAPI NXUnhandledExceptionFilter(
		                                            PEXCEPTION_POINTERS pExceptionInfo );


	static TCHAR m_szLogFileName[MAX_PATH];
	static LPTOP_LEVEL_EXCEPTION_FILTER m_previousFilter;
};

extern CMiniDump g_Dump;

#endif // !defined(AFX_NXLOG_H__DF9682BE_3FD5_4712_897D_22193AD50832__INCLUDED_)

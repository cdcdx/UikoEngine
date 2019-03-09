// NXLog.cpp: implementation of the CNXLog class.
//
//////////////////////////////////////////////////////////////////////
#include "MiniDump.h"
#include <string>
using namespace std;
//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

TCHAR CMiniDump::m_szLogFileName[MAX_PATH];
LPTOP_LEVEL_EXCEPTION_FILTER CMiniDump::m_previousFilter;

CMiniDump g_Dump;

CMiniDump::CMiniDump()
{
	SYSTEMTIME	systime = {0};
	TCHAR pDataBuffer[MAX_PATH] = {0};

	m_previousFilter = SetUnhandledExceptionFilter(NXUnhandledExceptionFilter);

    //OutputDebugString(_T("Minidump SetUnhandledExceptionFilter"));

	GetModuleFileName(0, m_szLogFileName, MAX_PATH);
    //GetModuleFileName(g_DllhMod, m_szLogFileName, MAX_PATH);

	GetLocalTime(&systime);

	_stprintf_s(pDataBuffer, MAX_PATH,
              TEXT("%04d-%02d-%02d_%02d-%02d-%02d.dmp"), 
		      systime.wYear, systime.wMonth, 
              systime.wDay, systime.wHour,
		      systime.wMinute, systime.wSecond);
	
	//wstring wsFile(m_szLogFileName);
	//int nPos = wsFile.rfind(_T('\\'));
	//if (nPos != wstring::npos)
	//{
	//	wsFile = wsFile.substr(0, nPos+1);
	//}
	//wsFile += wstring(pDataBuffer);
	//memset(m_szLogFileName, 0,sizeof(m_szLogFileName));
	//memcpy_s(m_szLogFileName, sizeof(m_szLogFileName), wsFile.c_str(), wsFile.length());
// 
 	PTSTR pszDot = _tcsrchr(m_szLogFileName, _T('\\'));
 
  	if (pszDot)
  	{
  		pszDot++;
  		if (_tcslen(pszDot) >= 3)
        {
#ifdef _UNICODE
			  //_tcscpy(pszDot, pDataBuffer);     //Garfield 20150814 unicode Debug±àÒë¾¯¸æ
			  wcscpy_s(pszDot, sizeof(pDataBuffer), pDataBuffer);
#else
			  memcpy_s(pszDot, MAX_PATH, pDataBuffer, sizeof(pDataBuffer));
#endif
        }
  			
  	}

}

CMiniDump::~CMiniDump()
{
	SetUnhandledExceptionFilter(m_previousFilter);
}

LONG WINAPI CMiniDump::NXUnhandledExceptionFilter(
	PEXCEPTION_POINTERS lpExceptionInfo)
{

	MINIDUMP_EXCEPTION_INFORMATION mdei = {0};
	MINIDUMP_TYPE	mdt;
	BOOL			retv = FALSE;
	HANDLE			hFile = 0;

	hFile = CreateFile(m_szLogFileName, 
                       GENERIC_READ | GENERIC_WRITE,
		               0, 
                       NULL, 
                       CREATE_ALWAYS, 
                       FILE_ATTRIBUTE_NORMAL, 
                       NULL);

	if (hFile == INVALID_HANDLE_VALUE )
		return FALSE;

	//OutputDebugString(_T("Minidump CreateFile"));

	mdei.ThreadId			= GetCurrentThreadId();
	mdei.ExceptionPointers	= lpExceptionInfo;
	mdei.ClientPointers		= FALSE;

	mdt	= MiniDumpNormal;
	retv = MiniDumpWriteDump(GetCurrentProcess(), 
                             GetCurrentProcessId(),
		                     hFile, 
                             mdt, 
                             ( lpExceptionInfo != 0 ) ? &mdei : 0, 0, 0);

	CloseHandle(hFile);

	return (LONG)retv;
}
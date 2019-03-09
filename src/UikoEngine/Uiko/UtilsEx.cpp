#include "stdafx.h"
#include "UtilsEx.h"
#include "timetk.h"
#include <atlstr.h>
UtilsEx::UtilsEx(void)
{
}

UtilsEx::~UtilsEx(void)
{
}

std::string UtilsEx::UnicodeToAnsi( const WCHAR* strSrc )
{
	std::string result;
	char* szRes;
	int i = 0;
	i = WideCharToMultiByte(CP_ACP, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new char[i + 1];
	WideCharToMultiByte(CP_ACP, 0, strSrc, -1, szRes, i, NULL, NULL);

	result = szRes;
	delete[]szRes;
	return result;
}

std::wstring UtilsEx::AnsiToUnicode( const char* strSrc )
{
	std::wstring result;
	WCHAR* szRes;
	int i = 0;
	i = MultiByteToWideChar(CP_ACP, 0, strSrc, -1, NULL, 0)*sizeof(WCHAR);
	szRes = new WCHAR[i + 1];
	MultiByteToWideChar(CP_ACP, 0, strSrc, -1, szRes, i);
	result = szRes;;
	delete[]szRes;
	return result;
}

std::string UtilsEx::UnicodeToUTF8( const WCHAR* strSrc )
{
	std::string result;
	char* szRes;
	int i = 0;
	i = WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, NULL, 0, NULL, NULL);
	szRes = new char[i + 1];
	WideCharToMultiByte(CP_UTF8, 0, strSrc, -1, szRes, i, NULL, NULL);
	result = szRes;
	delete[]szRes;
	return result;
}

std::wstring UtilsEx::UTF8ToUnicode( const char* strSrc )
{
	std::wstring result;
	WCHAR* szRes;
	int i = 0;
	i = MultiByteToWideChar(CP_UTF8, 0, strSrc, -1, NULL, 0)*sizeof(WCHAR);
	szRes = new WCHAR[i + 1];
	MultiByteToWideChar(CP_UTF8, 0, strSrc, -1, szRes, i);
	result = szRes;;
	delete[]szRes;
	return result;
}
//std::wstring UtilsEx::MultCHarToWideChar(string str)  
//{  
//	//获取缓冲区的大小，并申请空间，缓冲区大小是按字符计算的  
//	int len=MultiByteToWideChar(CP_ACP,0,str.c_str(),str.size(),NULL,0);  
//	TCHAR *buffer=new TCHAR[len+1];  
//	//多字节编码转换成宽字节编码  
//	MultiByteToWideChar(CP_ACP,0,str.c_str(),str.size(),buffer,len);  
//	buffer[len]='\0';//添加字符串结尾  
//	//删除缓冲区并返回值  
//	wstring return_value;  
//	return_value.append(buffer);  
//	delete []buffer;  
//	return return_value;  
//}  
//std::string UtilsEx::WideCharToMultiChar(wstring str)  
//{  
//	string return_value;  
//	//获取缓冲区的大小，并申请空间，缓冲区大小是按字节计算的  
//	int len=WideCharToMultiByte(CP_ACP,0,str.c_str(),str.size(),NULL,0,NULL,NULL);  
//	char *buffer=new char[len+1];  
//	WideCharToMultiByte(CP_ACP,0,str.c_str(),str.size(),buffer,len,NULL,NULL);  
//	buffer[len]='\0';  
//	//删除缓冲区并返回值  
//	return_value.append(buffer);  
//	delete []buffer;  
//	return return_value;  
//} 
std::vector< std::string > UtilsEx::string_split( const std::string& str, char ch, unsigned int maxSplits /*= 0 */ )
{
	std::vector<std::string> ret;
	unsigned int numSplits = 0;

	int start = 0;
	int pos = 0;
	do 
	{
		pos = str.find(ch, start);
		if (pos == start)
		{
			ret.push_back("");
			start = pos + 1;
		}
		else if (pos == -1 || ( maxSplits && numSplits == maxSplits ) )
		{
			ret.push_back( str.substr(start) );
			break;
		}
		else
		{
			ret.push_back( str.substr(start, pos - start) );
			start = pos + 1;
		}
		++ numSplits;

	} while (pos != -1);

	return ret;
}

BOOL UtilsEx::CreateFileDir(TCHAR *csFilePath)
{
    if(INVALID_FILE_ATTRIBUTES != GetFileAttributes(csFilePath))
    {
        return TRUE;
    }

    CString strText(csFilePath);
    if(strText.Right(1) != _T("\\"))
    {
        strText += _T("\\");
    }

    // 由外到内逐层遍历，直到每一级目录都创建成功或某级创建失败
    int iStart = 0;
    int iret = 0;
    CString strPath(_T(""));
    while(TRUE)
    {
        iret = strText.Find(_T("\\"),iStart);
        if(iret >= 0)
        {
            iStart = iret + 1;
            strPath = strText.Left(iStart);
            if(INVALID_FILE_ATTRIBUTES == GetFileAttributes(strPath.GetBuffer(0)))
            {
                if(!CreateDirectory(strPath,NULL))
                {
                    return FALSE;
                }
            }
        }
        else
        {
            return TRUE;
        }
    }
    return TRUE;//lint !e527
}

HANDLE UtilsEx::Create_File(TCHAR *csFileName, FILETIME Filetime)
{
	HANDLE hFileHandle = CreateFile(csFileName,
		GENERIC_WRITE,
		FILE_SHARE_READ|FILE_SHARE_WRITE,
		NULL,
		CREATE_ALWAYS,
		FILE_ATTRIBUTE_NORMAL,
		NULL);
	if (hFileHandle != INVALID_HANDLE_VALUE)
	{
		SetFileTime( hFileHandle, &Filetime, &Filetime, &Filetime );
		CloseHandle( hFileHandle );
	} 
	return hFileHandle;
}
//HANDLE UtilsEx::Create_File(TCHAR *csFileName)
//{
//	HANDLE hFileHandle = CreateFile(csFileName,
//		GENERIC_WRITE,
//		FILE_SHARE_READ|FILE_SHARE_WRITE,
//		NULL,
//		CREATE_ALWAYS,
//		FILE_ATTRIBUTE_NORMAL,
//		NULL);
//	return hFileHandle;
//}

std::string UtilsEx::UTF8ToAnsi( const char* strSrc )
{
	return UnicodeToAnsi(UTF8ToUnicode(strSrc).c_str()); 
}

std::string UtilsEx::AnsiToUTF8( const char* strSrc )
{
	return UnicodeToUTF8(AnsiToUnicode(strSrc).c_str()); 
}

string UtilsEx::time2string(time_t tm)
{
	timetk tk(tm);
	char buf[512] = "\0";

	sprintf_s(buf,"%04d-%02d-%02d %02d:%02d:%02d", tk.get_year(), tk.get_month(), \
		tk.get_day(), tk.get_hour(), tk.get_minute(), tk.get_second() );

	return string(buf);
}
string UtilsEx::time2stringEx(time_t tm)
{
	timetk tk(tm);
	char buf[512] = "\0";

	sprintf_s(buf,"%04d年%02d月%02d日", tk.get_year(), tk.get_month(), \
		tk.get_day());
	return string(buf);
}

time_t UtilsEx::string2time(string& str)
{
	//2013-09-23 22:49:33 
	int year = 0;
	int month = 0;
	int day = 0;
	int hour = 0;
	int minute = 0;
	int seconde = 0;

	//
	if (str.size() < 19)
	{
		return -1;
	}
	
	string sYear = str.substr(0, 4);
	string sMonth = str.substr(5, 2);
	string sDay = str.substr(8, 2);

	string sHour = str.substr(11, 2);
	string sMinute = str.substr(14, 2);
	string sSecond = str.substr(17, 2);

	year = atoi(sYear.c_str());
	month = atoi(sMonth.c_str());
	day = atoi(sDay.c_str());
	hour = atoi(sHour.c_str());
	minute = atoi(sMinute.c_str());
	seconde = atoi(sSecond.c_str());
	
	timetk tm(year, month, day, hour, minute, seconde);
	return tm.get_time();
}
void UtilsEx::time2SysTime(time_t tm, SYSTEMTIME &sys)
{
	memset(&sys, 0, sizeof(sys));
	timetk tk(tm);

	sys.wYear = tk.get_year();
	sys.wMonth = tk.get_month();
	sys.wDay = tk.get_day();
	sys.wHour = tk.get_hour();
	sys.wMinute = tk.get_minute();
	sys.wSecond = tk.get_second();
}
time_t UtilsEx::Systime2time_t(SYSTEMTIME& sys)
{
	timetk tk(sys.wYear, sys.wMonth, sys.wDay, sys.wHour, sys.wMinute, sys.wSecond);
	return tk.get_time();
}
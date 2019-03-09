#pragma once
//#include "stdafx.h"
#include <string>
#include <vector>
#include <time.h>
#include <io.h>
using namespace std;
class UtilsEx
{
public:
	UtilsEx(void);
	virtual ~UtilsEx(void);

	/**	@fn	UnicodeToAnsi
 *	@brief	Unicode转Ansi
 *	@param strSrc Unicode编码格式的源字符串
 *	@return	 转码后的Ansi字符串
 */
static std::string UnicodeToAnsi(const WCHAR* strSrc);

/**	@fn	AnsiToUnicode
 *	@brief	Ansi转Unicode
 *	@param strSrc Ansi编码格式的源字符串
 *	@return	 转码后的Unicode字符串
 */
static std::wstring AnsiToUnicode(const char* strSrc);

/**	@fn	UnicodeToUTF8
 *	@brief	Unicode转UTF8
 *	@param strSrc Unicode编码格式的源字符串
 *	@return	 转码后的UTF8字符串
 */
static std::string UnicodeToUTF8(const WCHAR* strSrc);

/**	@fn	UTF8ToUnicode
 *	@brief	UTF8转Unicode
 *	@param strSrc UTF8编码格式的源字符串
 *	@return	 转码后的Unicode字符串
 */
static std::wstring UTF8ToUnicode(const char* strSrc);

/**@fn	UTF8ToAnsi
 *	@brief 	UTF8转Ansi
 *	@param strSrc UTF8编码格式的源字符串
 *	@return	 转码后的UTF8字符串
 */
static std::string UTF8ToAnsi(const char* strSrc);

/**@fn	AnsiToUTF8
 *	@brief 	Ansi转UTF8
 *	@param strSrc Ansi编码格式的源字符串
 *	@return	 转码后的UTF8字符串
 */
static std::string AnsiToUTF8(const char* strSrc);

///**@pangtou  MultCHarToWideChar
// *
// */
//static std::wstring MultCHarToWideChar(string str);
//
///**@pangtou  WideCharToMultiChar
// *
// */
//static std::string WideCharToMultiChar(wstring str);

static std::vector< std::string > string_split( const std::string& str, char ch, unsigned int maxSplits = 0 );

static BOOL CreateFileDir(TCHAR *csFilePath);

static HANDLE Create_File(TCHAR *csFileName, FILETIME Filetime);

static bool  is_exist_file(const char* file)
{
	const std::string newfile(file);

#ifdef _WIN32
	if (_access(newfile.c_str(), 0) != -1)
#else
	if (access(newfile.c_str(), 0) != -1)
#endif
	{
		return true;
	}

	return false;
}

//
static string time2string(time_t tm);
static time_t string2time(string& str);
//生日
static string time2stringEx(time_t tm);

static void time2SysTime(time_t tm, SYSTEMTIME &sys);

static time_t Systime2time_t(SYSTEMTIME& sys);

};

#ifndef __ILibInterface_H
#define __ILibInterface_H

/** ¶ÔlibÎÄ¼şµÄ¸üĞÂºÍÌí¼Ó¹¤×÷

*   FileName  : ILibInterface.h

*   Author    : Double Sword

*   date      : 2011-8-16

*   Copyright : Copyright belong to Author and ZhiZunNet.CO.

*   RefDoc    : --

*/

/** ´ò¿ªzipÎÄ¼ş
*   @Author : Double sword
*   @Params : sourcrZipPath-zipÎÄ¼şÈ«Â·¾¶,password-ÃÜÂë	
*   @Return : ·µ»Ø¸ÃzipÎÄ¼şµÄid,¹Ø±ÕÊ±ĞèÒª´«Õâ¸ö²ÎÊı,0Ê§°Ü
*   @Date   : 2011-8-16
*/
extern int OpenZipLib(LPCSTR sourcrZipPath,LPCSTR password);

/** ¹Ø±ÕzipÎÄ¼ş
*   @Author : Double sword
*   @Params : fileId-´Óopenzip»ñµÃ,bHasUpdateOptÊÇ·ñµ÷ÓÃUpdateZipFile
*   @Return : ÎŞ
*   @Date   : 2011-8-16
*/
extern void CloseZipLib(int nZip,bool bHasUpdateOpt = true);

/** ¶Ô±ÈÎÄ¼şcrc
*   @Author : Double sword
*   @Params : nZip-OpenZip²úÉú,filename-ĞèÒª¶Ô±ÈµÄÎÄ¼şÃû(zipÄÚµÄÎÄ¼şÃû¿ÉÒÔ´øÄ¿Â¼1/1.txt)£¬ulCrc-ÎÄ¼şµÄcrc		
*   @Return : true±íÊ¾ĞèÒª¸üĞÂ£¬false²»ĞèÒª
*   @Date   : 2011-8-16
*/
extern bool CompareCrc(int nZip,LPCSTR filename,unsigned long ulCrc);

/** Ìí¼ÓĞèÒª¸üĞÂµÄÎÄ¼ş»º³å
*   @Author : Double sword
*   @Params : nZip-OpenZip²úÉú,filename-ÎÄ¼şÃû(zipÄÚµÄÎÄ¼şÃû¿ÉÒÔ´øÄ¿Â¼1/1.txt),pBuf-ÄÚ´æÁ÷,nLen-³¤¶È		
*   @Return : 
*   @Date   : 2011-8-16
*/
extern bool AddUpdateFile(int nZip,LPCSTR filename,char* pBuf,int nLen);

/** Ìí¼ÓÍêËùÓĞµÄÎÄ¼şµ÷ÓÃ´Îº¯Êı½øĞĞ¸üĞÂzipÎÄ¼ş
*   @Author : Double sword
*   @Params : nZip-OpenZip²úÉú,		
*   @Return : true³É¹¦,falseÊ§°Ü
*   @Date   : 2011-8-16
*/
extern bool UpdateZipFile(int nZip);

/** »ñÈ¡ÎÄ¼şÁĞ±í
*   @FuncName : GetZipFileList
*   @Author   : Double sword
*   @Params   : pszList \0·Ö¸îÏî \0\0±íÊ¾½áÊø	
*   @Return   :	void			
*   @Date     :	2012-10-31
*/
extern void GetZipFileList(int nZip,char* pszList);


/** ¶ÁÈ¡zipÎÄ¼ş½âÑ¹ºóµÄ´óĞ¡
*   @FuncName : 
*   @Author   : Double sword
*   @Params   : 	
*   @Return   :					
*   @Date     :	2012-10-31
*/
extern int GetZipFileSize(int nZip,TCHAR* pszFileName);


/** ¶ÁÈ¡zipÎÄ¼ş×Ö½ÚÁ÷
*   @FuncName : ReadZipFile
*   @Author   : Double sword
*   @Params   : nZip,pszFileName,pPwd²»ĞèÒªÃÜÂëÊ±´«NULL,pBuf ,lenth of pBuf

*   @Return   :	ÎÄ¼şµÄ´óĞ¡

*   @Date     :	2012-10-31
*/
extern int ReadZipFile(int nZip,TCHAR* pszFileName,TCHAR* pPwd,void* pBuf,int nBufLen);








/************************************************************************/
/* ¿â¼ÓÔØ                                                               */
/************************************************************************/
#if defined(_MT) && defined(_DLL)	//[MD]
#	ifdef _DEBUG			//MDD
#		ifdef _UNICODE 
#			pragma comment(lib,"ZipLibud.lib")
#		else
#			pragma comment(lib,"ZipLibd.lib")
#		endif
#	else					//MD
#		ifdef _UNICODE 
#			pragma comment(lib,"ZipLibu.lib")
#		else
#			pragma comment(lib,"ZipLib.lib")
#		endif
#	endif
#elif defined(_MT) && !defined(_DLL) //[MT]
#	ifdef _DEBUG			//MTD
#		ifdef _UNICODE 
#			pragma comment(lib,"ZipLibumtd.lib")
#		else
#			pragma comment(lib,"ZipLibmtd.lib")
#		endif
#	else					//MT
#		ifdef _UNICODE 
#			pragma comment(lib,"ZipLibumt.lib")
#		else
#			pragma comment(lib,"ZipLibmt.lib")
#		endif
#	endif
#endif




//sample
//
//int main()
//{
//
//#define __WINDOWS__
//#if defined(__WINDOWS__)
//	_CrtSetDbgFlag(_CrtSetDbgFlag(0) | _CRTDBG_LEAK_CHECK_DF);
//#endif
//
//	int nZip = OpenZip( TEXT("texture.paq"),TEXT("kh?ƒ}?t‹ôj‹EÔ‹‹MÔ‹Bÿ@") );
//	if (nZip)
//	{
//		char* pBuf = NULL;
//		int nSize = 0;
//		FILE* f = fopen("bk31.ani","rb");
//		if (f)
//		{
//			fseek(f,0,SEEK_END);
//			nSize = ftell(f);
//			fseek(f,0,SEEK_SET);
//			pBuf = new char[nSize];
//			fread(pBuf,1,nSize,f);
//
//		}
//		if( CompareCrc(nZip,"texture/bk31.ani",0) )
//		{
//			AddUpdateFile( nZip,"texture/bk31.ani",pBuf,nSize );
//		}
//
//		UpdateZipFile(nZip);
//
//		CloseZip(nZip);
//
//		if(pBuf)
//			delete[] pBuf;
//	}
//
//	return 0;
//}

#endif //__ILibInterface_H
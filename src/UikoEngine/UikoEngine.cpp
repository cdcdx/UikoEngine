#include "StdAfx.h"
#include "UikoEngine.h"
#include "Uiko/SkinEngine.h"
#include "Uiko/ITaskbar.h"
#include "Uiko/UtilsEx.h"
//#include "Utils/UIShadow.h"
#include "Uiko/WndShadow.h"

//Uiko Core
#include "core.h"
////#pragma comment( lib, "core.lib")

#include "Uiko/deldir.h"

#include <map>
#include <shlobj.h>
#include <process.h>
#include <atlconv.h>
#include <Windows.h>
#include <atlstr.h>
#include <stdlib.h>

////联网检测
//#include <ipexport.h>
//#include <IcmpApi.h>
//#pragma comment(lib, "Iphlpapi.lib")

//下载
#include <WinInet.h>
#pragma comment( lib, "wininet.lib")
#pragma comment( lib, "ws2_32.lib")

// Core状态
#define Core FALSE    //TRUE-open   FALSE-close

using namespace DuiLib;

//static LPBYTE resource_zip_buffer_ = NULL;

DuiLib::CSkinEngine* g_pDuiEngine = NULL;
DuiLib::CSkinEngineMsgBox* g_pDuiEngineMsgBox = NULL;

CWndShadow wndShadow;
CWndShadow msgboxwndShadow;

BOOL g_bMSGLoopFlag = FALSE;
//BOOL g_bMSGBoxLoopFlag = TRUE;

NOTIFYICONDATA m_nid;            //托盘属性
bool g_NotifyIcon = false;       //托盘是否存在

ITaskbarList3 *m_pTaskBarList;   //任务栏

//安装卸载详情
std::map<HWND, WNDPROC> g_windowInfoMap;
CDuiString g_tempParam = _T("");
CDuiString g_tempParamValue = _T("");
CDuiString g_tempParamDetail = _T("");
CDuiString g_installPageTabName = _T("");
std::map<CDuiString, CDuiString> g_controlLinkInfoMap;

//安装皮肤路径
CDuiString g_skinPath = _T("");

//DuiEngineMsgBox
TCHAR g_msgbox_xmlName[MAX_PATH] = {0};
TCHAR g_msgbox_lblTitle[MAX_PATH] = {0};
TCHAR g_msgbox_edtText[MAX_PATH] = {0};
TCHAR g_msgbox_btnClose[MAX_PATH] = {0}; 
TCHAR g_msgbox_btnYes[MAX_PATH] = {0}; 
TCHAR g_msgbox_btnNo[MAX_PATH] = {0}; 

//NSIS回调
extern HINSTANCE  g_hInstance;
extra_parameters* g_pluginParms;

typedef struct _tagROUTINEPARAM 
{
	HWND hWnd;
}ROUTINEPARAM, *LPROUTINEPARAM;

typedef struct _tagDLWNDPARAM
{
	HWND wnd;
    TCHAR dl_url[MAX_PATH];
    TCHAR dl_path[MAX_PATH];
    TCHAR dl_name[MAX_PATH];
    TCHAR dl_progressname[MAX_PATH];
}DLWNDPARAM, *LPDLWNDPARAM;

UINT DuiMessageBox( HWND hwndParent, LPCTSTR lpTitle, LPCTSTR lpText, LPCTSTR lpYes, LPCTSTR lpNo,  bool bYesBtnVisible );

unsigned int WINAPI RoutionNSISMessageProc( void* pvoid )
{ //NSIS消息线程
    OutputDebugString( _T("RoutionNSISMessageProc") );
	
	MSG msg = { 0 };
	while( ::GetMessage( &msg, NULL, 0, 0 ) )
	{
		if (g_bMSGLoopFlag)
	    {
		    OutputDebugString( _T("RoutionNSISMessageProc g_bMSGLoopFlag = true") );
		    
		    ::TranslateMessage( &msg );    //将键盘消息转换为字符消息
		    ::DispatchMessage( &msg );     //将消息传给窗体函数去处理.   该函数调度一个消息给窗口程序
	    }
	    else
	    {
		    OutputDebugString( _T("RoutionNSISMessageProc g_bMSGLoopFlag = false") );
	    }

		//::TranslateMessage( &msg );
		//::DispatchMessage( &msg );
	}
	return 0;
}
int NSISMessageProc()
{ //NSIS消息
	//CDuiString cccc;
	//cccc.Format( "NSISMessageProc  hwnd=%d  err=%d  \r\n", g_pDuiEngine->GetHWND(), GetLastError() );
	//OutputDebugString( cccc );

	MSG msg = { 0 };
	while( ::GetMessage( &msg, NULL, 0, 0 ) && g_bMSGLoopFlag )
	{
		//OutputDebugString( _T("NSISMessageProc g_bMSGLoopFlag = true") );

		::TranslateMessage( &msg );
		::DispatchMessage( &msg );
	}

	return 0;
}



//NSIS回调
static UINT_PTR PluginCallback( enum NSPIM msg )
{ //注册回调

#ifdef _DEBUG
    OutputDebugString(_T("Debug  PluginCallback  -------------------------------------------------------------->End"));
#endif

	return 0;
}

void InitDuiEngine( HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  InitDuiEngine  -------------------------------------------------------------->") );
#endif

	g_pluginParms = extra;
	EXDLL_INIT();
	extra->RegisterPluginCallback( g_hInstance, PluginCallback );
	{
		TCHAR skinPath[MAX_PATH];
		TCHAR skinPathZip[MAX_PATH];
		TCHAR skinPathZipPwd[MAX_PATH];
		TCHAR skinLayoutFileName[MAX_PATH];
		TCHAR installProductIcon[MAX_PATH];
		TCHAR installPageTabName[MAX_PATH];
		TCHAR installProductName[MAX_PATH];
		TCHAR SetWindowPosTopMost[MAX_PATH];
		TCHAR installProductShadow[MAX_PATH];

		ZeroMemory( skinPath, MAX_PATH );
		ZeroMemory( skinPathZip, MAX_PATH );
		ZeroMemory( skinPathZipPwd, MAX_PATH );
		ZeroMemory( skinLayoutFileName, MAX_PATH );
		ZeroMemory( installProductIcon, MAX_PATH );
		ZeroMemory( installPageTabName, MAX_PATH );
		ZeroMemory( installProductName, MAX_PATH );
		ZeroMemory( SetWindowPosTopMost, MAX_PATH );
		ZeroMemory( installProductShadow, MAX_PATH );

		popstring( skinPath );              // 皮肤路径|zip路径
		popstring( skinPathZip );           // zip名称
		popstring( skinPathZipPwd );        // zip密码
		popstring( skinLayoutFileName );  // 皮肤文件
		popstring( installProductIcon );  // 产品图标
		popstring( installPageTabName );  // 安装页面tab的名字
		popstring( installProductName );    // 产品名称
		popstring( SetWindowPosTopMost );   // 窗口置顶
		popstring( installProductShadow );  // 窗口阴影

		//if( _tcsstr(installProductName, _T("作品")) == NULL )
		//{
		//	MessageBox( 0, _T("没有授权!"), _T("dui-DllMain"), 0 );
		//	ExitProcess( 0 );
		//}

		DuiLib::CPaintManagerUI::SetInstance( g_hInstance );
		DuiLib::CPaintManagerUI::SetResourcePath( skinPath );

		//ZIP
		//if( _tcsstr(skinPathZip, _T("")) != NULL )
		if( _tcslen(skinPathZip) != 0 )
		{
			if( _tcslen(skinPathZipPwd) != 0 )
				DuiLib::CPaintManagerUI::SetResourceZip( skinPathZip, true, skinPathZipPwd);
			else
				DuiLib::CPaintManagerUI::SetResourceZip( skinPathZip, false, skinPathZipPwd);
		}

		g_installPageTabName = installPageTabName;
		g_skinPath = skinPath;

		g_pDuiEngine = new DuiLib::CSkinEngine();
		if ( g_pDuiEngine == NULL ) return;

		g_pDuiEngine->SetSkinXMLPath( skinLayoutFileName );
		g_pDuiEngine->Create( NULL, installProductName /*_T("DuiEngine")*/, UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 0, 0 );

		//任务栏图标
        HICON hIcon = ( HICON )LoadImage( NULL, installProductIcon, IMAGE_ICON, 
            32, 
            32, 
            LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE );
		::SendMessage( g_pDuiEngine->GetHWND(), WM_SETICON, ( WPARAM ) TRUE, ( LPARAM ) hIcon );

        //托盘图标
        m_nid.cbSize = sizeof(NOTIFYICONDATA);
        m_nid.hWnd = g_pDuiEngine->GetHWND();
        m_nid.uID = 0;
        m_nid.uFlags = NIF_ICON|NIF_MESSAGE|NIF_TIP;
        m_nid.hIcon = hIcon;//::LoadIcon(::GetModuleHandle(NULL), SS);//LPCTSTR SS = MAKEINTRESOURCE(installProductIcon);
        m_nid.uCallbackMessage = WM_DUI_NOTIFYICON;//定义处理消息类型
        lstrcpy(m_nid.szTip, TEXT(""));
        //显示图标
        //Shell_NotifyIcon(NIM_ADD,&m_nid);
        g_NotifyIcon = false;
		
		//任务栏进度条
		m_pTaskBarList = NULL;
		HRESULT hr = CoCreateInstance( CLSID_TaskbarList, NULL, CLSCTX_INPROC_SERVER, IID_PPV_ARGS( &m_pTaskBarList ) );
		if ( SUCCEEDED( hr ) )
		{
			hr = m_pTaskBarList->HrInit();
			if ( FAILED( hr ) )
			{
				m_pTaskBarList->Release();
				m_pTaskBarList = NULL;
			}
			m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NOPROGRESS );
			m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NORMAL );
		}

		//屏幕居中显示
		g_pDuiEngine->CenterWindow();

		//窗口阴影
		if ( _tcsicmp( installProductShadow, _T("true") ) == 0 )
		{
			bool bInit = CWndShadow::Initialize( CPaintManagerUI::GetInstance() );
			HWND hWNd = g_pDuiEngine->GetHWND();
			wndShadow.Create( hWNd );
			wndShadow.SetSize(5);
			wndShadow.SetPosition(0,0);
		}

		//置顶
		if ( _tcsicmp( SetWindowPosTopMost, _T("true") ) == 0 )
		{
			::SetWindowPos( g_pDuiEngine->GetHWND(), HWND_TOPMOST, 0, 0, 0, 0, SWP_NOMOVE|SWP_NOSIZE ); 
		}

		pushint( int( g_pDuiEngine->GetHWND() ) );
	}
}

void ReloadSkin( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  ReloadSkin  -------------------------------------------------------------->") );
#endif

	TCHAR skinPath[MAX_PATH];
	TCHAR skinLayoutFileName[MAX_PATH];
	ZeroMemory( skinPath, MAX_PATH );
	ZeroMemory( skinLayoutFileName, MAX_PATH );
	
	popstring( skinPath );            // 皮肤路径
	popstring( skinLayoutFileName );  // 皮肤文件

	g_skinPath = skinPath;

	//DuiLib::CPaintManagerUI::SetInstance( g_hInstance );
	//DuiLib::CPaintManagerUI::SetResourcePath( skinPath );

	//OutputDebugString( "ReloadSkin start");

	//if ( g_pDuiEngine == NULL ) 
	//{
	//	pushint( -1 );
	//	return;
	//}
	//g_pDuiEngine->SetSkinXMLPath( skinLayoutFileName );
	//CPaintManagerUI::SetResourceZip( skinLayoutFileName, true );
	//CPaintManagerUI::ReloadSkin();

	//OutputDebugString( "ReloadSkin end");

	pushint( 0 );

}

void FindControl( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );

	popstring( controlName );
	
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  FindControl  -------------------------------------------------------------->  a=%s \r\n"), controlName );
    OutputDebugString( cccc );
#endif
	
	CControlUI* pControl = static_cast<CControlUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		pushint( - 1 );
	else
    	pushint( 0 );
}


void BindControl( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	
	popstring( controlName ); 

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  BindControl  -------------------------------------------------------------->  a=%s \r\n"), controlName );
    OutputDebugString( cccc );
#endif

	int callbackID = popint();
	g_pDuiEngine->SaveToControlCallbackMap( controlName, callbackID );
}

//////////////////////////////////////////////////////////////////////////
void SetControlData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	TCHAR controlData[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( controlData, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );

	popstring( controlName );
	popstring( controlData );
	popstring( dataType );
	
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  SetControlData  --------------------------------------------------------------> a=%s b=%s c=%s \r\n"), controlName, controlData, dataType );
    OutputDebugString( cccc );
#endif

	CControlUI* pControl = static_cast<CControlUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;
	
	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetText( pControl->GetText() );
		else
			pControl->SetText( controlData );
	}
	else if ( _tcsicmp( dataType, _T("bkimage") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetBkImage( pControl->GetBkImage() );
		else
			pControl->SetBkImage( controlData );
	}
	else if ( _tcsicmp( dataType, _T("link") ) == 0 )
	{
		g_controlLinkInfoMap[controlName] = controlData;
	}
	else if ( _tcsicmp( dataType, _T("enable") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetEnabled( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetEnabled( false );
	}
	else if ( _tcsicmp( dataType, _T("visible") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetVisible( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetVisible( false );
	}
	else if ( _tcsicmp( dataType, _T("tooltip") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetToolTip( pControl->GetToolTip() );
		else
			pControl->SetToolTip( controlData );
	}
	else if ( _tcsicmp( dataType, _T("focus") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetFocus();
	}
	else if ( _tcsicmp( dataType, _T("select") ) == 0 )
	{
		CComboUI* pControl = static_cast<CComboUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
		if ( pControl == NULL )
			return;
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			;
		else
		{
			pControl->SelectItem( ( int )controlData, false );
		}
	}
	else if ( _tcsicmp( dataType, _T("width") ) == 0 )
	{
        if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
            ;
        else
        {
            SIZE szlt = {0,0};
            SIZE szrb = {0,0};
            szlt = pControl->GetFixedXY();
            szrb = pControl->EstimateSize(szrb);
            RECT rcPos;
            rcPos.left = szlt.cx;
            rcPos.top = szlt.cy;
            //rcPos.right = szlt.cx + atoi((char*)controlData);
# ifdef _UNICODE 
            rcPos.right = szlt.cx + _wtoi((wchar_t*)controlData);
# else
            rcPos.right = szlt.cx + atoi((char*)controlData);
# endif
            rcPos.bottom = szlt.cy + szrb.cy;
            pControl->SetPos(rcPos);
        }
	}
	else if ( _tcsicmp( dataType, _T("height") ) == 0 )
	{
        if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
            ;
        else
        {
            SIZE szlt = {0,0};
            SIZE szrb = {0,0};
            szlt = pControl->GetFixedXY();
            szrb = pControl->EstimateSize(szrb);
            RECT rcPos;
            rcPos.left = szlt.cx;
            rcPos.top = szlt.cy;
            rcPos.right = szlt.cx + szrb.cx;
            //rcPos.bottom = szlt.cy + atoi((char*)controlData);
# ifdef _UNICODE 
            rcPos.bottom = szlt.cy + _wtoi((wchar_t*)controlData);
# else
            rcPos.bottom = szlt.cy + atoi((char*)controlData);
# endif
            pControl->SetPos(rcPos);
        }
	}
}
void GetControlData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );
	
	popstring( controlName );
	popstring( dataType );
	
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  GetControlData  --------------------------------------------------------------> a=%s b=%s \r\n"), controlName, dataType );
    OutputDebugString( cccc );
#endif

	CControlUI* pControl = static_cast<CControlUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	TCHAR temp[MAX_PATH] = {0};
	
	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		_tcscpy( temp, pControl->GetText().GetData() );
		pushstring( temp );
	}
	else if ( _tcsicmp( dataType, _T("tooltip") ) == 0 )
	{
		_tcscpy( temp, pControl->GetToolTip().GetData() );
		pushstring( temp );
	}
	else if ( _tcsicmp( dataType, _T("visible") ) == 0 )
	{
		bool bVisible;
		bVisible = pControl->IsVisible();
		if ( pControl->IsVisible() )
		{
			pushstring( _T("true") );
		}
		else
		{
			pushstring( _T("false") );
		}
	}
}

void SetProgressData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR controlName[MAX_PATH];
	TCHAR controlData[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( controlData, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );

	popstring( controlName );
	popstring( controlData );
	popstring( dataType );
	
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  SetProgressData  --------------------------------------------------------------> a=%s b=%s c=%s \r\n"), controlName, controlData, dataType );
    OutputDebugString( cccc );
#endif

	CProgressUI* pControl = static_cast<CProgressUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetText( pControl->GetText() );
		else
			pControl->SetText( controlData );
	}
	else if ( _tcsicmp( dataType, _T("bkimage") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetBkImage( pControl->GetBkImage() );
		else
			pControl->SetBkImage( controlData );
	}
	else if ( _tcsicmp( dataType, _T("link") ) == 0 )
	{
		g_controlLinkInfoMap[controlName] = controlData;
	}
	else if ( _tcsicmp( dataType, _T("enable") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetEnabled( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetEnabled( false );
	}
	else if ( _tcsicmp( dataType, _T("visible") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetVisible( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetVisible( false );
	}
	else if ( _tcsicmp( dataType, _T("tooltip") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetToolTip( pControl->GetToolTip() );
		else
			pControl->SetToolTip( controlData );
	}
	else if ( _tcsicmp( dataType, _T("value") ) == 0 )
	{
		int data = 0 ;
#ifdef UNICODE
		data = _wtoi( controlData );
#else 
		data = atoi( controlData );
#endif

		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetValue( pControl->GetValue() );
		else
			pControl->SetValue( data );
	}

}



void GetProgressData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );
	
	popstring( controlName );
	popstring( dataType );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  GetProgressData  --------------------------------------------------------------> a=%s b=%s \r\n"), controlName, dataType );
    OutputDebugString( cccc );
#endif

	CProgressUI* pControl = static_cast<CProgressUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	TCHAR temp[MAX_PATH] = {0};
	
	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		_tcscpy( temp, pControl->GetText().GetData() );
		pushstring( temp );
	}
	else if ( _tcsicmp( dataType, _T("tooltip") ) == 0 )
	{
		_tcscpy( temp, pControl->GetToolTip().GetData() );
		pushstring( temp );
	}
	else if ( _tcsicmp( dataType, _T("value") ) == 0 )
	{
		pushint( pControl->GetValue() );
	}
}
void SetListData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	TCHAR controlData[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( controlData, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );

	popstring( controlName );
	popstring( controlData );
	popstring( dataType );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  SetListData  --------------------------------------------------------------> a=%s b=%s c=%s \r\n"), controlName, controlData, dataType );
    OutputDebugString( cccc );
#endif

	CListUI* pControl = static_cast<CListUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetText( pControl->GetText() );
		else
			pControl->SetText( controlData );
	}
	else if ( _tcsicmp( dataType, _T("select") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetItemIndex( pControl, -1 );
		else
			pControl->SetItemIndex( pControl, ( int )controlData );
	}
	else if ( _tcsicmp( dataType, _T("enable") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetEnabled( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetEnabled( false );
	}
	else if ( _tcsicmp( dataType, _T("visible") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("true") ) == 0 )
			pControl->SetVisible( true );
		else if ( _tcsicmp( controlData, _T("false") ) == 0 )
			pControl->SetVisible( false );
	}

}

void GetListData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR controlName[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );
	
	popstring( controlName );
	popstring( dataType );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  GetListData  --------------------------------------------------------------> a=%s b=%s \r\n"), controlName, dataType );
    OutputDebugString( cccc );
#endif

	CListUI* pControl = static_cast<CListUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	TCHAR temp[MAX_PATH] = {0};

	if ( _tcsicmp( dataType, _T("select") ) == 0 )
	{
		pushint( pControl->GetCurSel() );
		//pushint( pControl->GetItemIndex( pControl ) );
	}
	else if ( _tcsicmp( dataType, _T("count") ) == 0 )
	{
		pushint( pControl->GetCount() );
		//pushint( pControl->GetItemIndex( pControl ) );
	}
}
void SetComboData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

	TCHAR controlName[MAX_PATH];
	TCHAR controlData[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( controlData, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );

	popstring( controlName );
	popstring( controlData );
	popstring( dataType );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  SetListData  --------------------------------------------------------------> a=%s b=%s c=%s \r\n"), controlName, controlData, dataType );
    OutputDebugString( cccc );
#endif

	CComboUI* pControl = static_cast<CComboUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	if ( _tcsicmp( dataType, _T("text") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
			pControl->SetText( pControl->GetText() );
		else
			pControl->SetText( controlData );
	}
	else if ( _tcsicmp( dataType, _T("select") ) == 0 )
	{
		if ( _tcsicmp( controlData, _T("error") ) == 0 || _tcsicmp( controlData, _T("") ) == 0 )
		{
			//pControl->SetItemIndex( pControl, -1 );
			pControl->SelectItem( -1, false );
		}
		else
		{
			//pControl->SetItemIndex( pControl, ( int )controlData );
			pControl->SelectItem( ( int )controlData, false );
		}
	}

}

void GetComboData( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR controlName[MAX_PATH];
	TCHAR dataType[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( dataType, MAX_PATH );
	
	popstring( controlName );
	popstring( dataType );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  GetComboData  --------------------------------------------------------------> a=%s b=%s \r\n"), controlName, dataType );
    OutputDebugString( cccc );
#endif

	CListUI* pControl = static_cast<CListUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pControl == NULL )
		return;

	TCHAR temp[MAX_PATH] = {0};
	if ( _tcsicmp( dataType, _T("select") ) == 0 )
	{
		pushint( pControl->GetItemIndex( pControl ) );
	}
}
void DuiSendMessage( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR msgID[MAX_PATH];
	TCHAR wParam[MAX_PATH];
	TCHAR lParam[MAX_PATH];
 	ZeroMemory( msgID, MAX_PATH );
	ZeroMemory( wParam, MAX_PATH );
	ZeroMemory( lParam, MAX_PATH );

    HWND hwnd = ( HWND )popint();
	popstring( msgID );
	popstring( wParam );
	popstring( lParam );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  DuiSendMessage  --------------------------------------------------------------> a=%s b=%s c=%s \r\n"), msgID, wParam, lParam );
    OutputDebugString( cccc );
#endif

	if ( _tcsicmp( msgID, _T("WM_DUI_MIN") ) == 0 )
        ::SendMessage( hwnd, WM_DUI_MIN, ( WPARAM )wParam, ( LPARAM )lParam );
    else if ( _tcsicmp( msgID, _T("WM_DUI_MAX") ) == 0 )
        ::SendMessage( hwnd, WM_DUI_MAX, ( WPARAM )wParam, ( LPARAM )lParam );
    else if ( _tcsicmp( msgID, _T("WM_DUI_RESTORE") ) == 0 )
        ::SendMessage( hwnd, WM_DUI_RESTORE, ( WPARAM )wParam, ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_CLOSE") ) == 0 )
		::SendMessage( hwnd, WM_DUI_CLOSE, ( WPARAM )wParam, ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_BACK") ) == 0 )
		::SendMessage( hwnd, WM_DUI_BACK, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_BACK2") ) == 0 )
		::SendMessage( hwnd, WM_DUI_BACK2, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_BACK3") ) == 0 )
		::SendMessage( hwnd, WM_DUI_BACK3, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_NEXT") ) == 0 )
		::SendMessage( hwnd, WM_DUI_NEXT, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_NEXT2") ) == 0 )
		::SendMessage( hwnd, WM_DUI_NEXT2, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_NEXT3") ) == 0 )
		::SendMessage( hwnd, WM_DUI_NEXT3, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_CANCEL") ) == 0 )
	{
		//::SendMessage( hwnd, WM_DUI_CANCEL, ( WPARAM )wParam, ( LPARAM )lParam );

		TCHAR lpYesParam[MAX_PATH];
		TCHAR lpNoParam[MAX_PATH];
		ZeroMemory( lpYesParam, MAX_PATH );
		ZeroMemory( lpNoParam, MAX_PATH );
		popstring( lpYesParam );
		popstring( lpNoParam );

		LPCTSTR lpTitle = ( LPCTSTR )wParam;
		LPCTSTR lpText = ( LPCTSTR )lParam;
		LPCTSTR lpYes = ( LPCTSTR )lpYesParam;
		LPCTSTR lpNo = ( LPCTSTR )lpNoParam;

		if ( g_pDuiEngineMsgBox == NULL )
		{
			if( IDYES == MessageBox( hwnd, lpText, lpTitle, MB_YESNO))  //
			{
				pushint( 0 );
				//ExitProcess( 0 );
			}
			else
			{
				pushint( -1 );
			}
		}
		else
		{
			if ( IDYES == DuiMessageBox( hwnd, lpTitle, lpText, lpYes, lpNo, true ) )  //if ( IDYES == MessageBox( hwnd, lpText, lpTitle, MB_YESNO ) )  //
			{
				pushint( 0 );
			}
			else
			{
				pushint( -1 );
			}
		}

		return;
		//////////////////////////////////////////////////////////////////////////

		////Duilib弹窗消息窗口
		//LPCTSTR lpTitle = ( LPCTSTR )wParam;
		//LPCTSTR lpText = ( LPCTSTR )lParam;

		//CControlUI* pTitleControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ) );
		//CControlUI* pTipTextControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ) );
		//if ( pTitleControl != NULL )
		//	pTitleControl->SetText( lpTitle );
		//if ( pTipTextControl != NULL )
		//	pTipTextControl->SetText( lpText );

		//g_pDuiEngineMsgBox->CenterWindow();
		//int ret = g_pDuiEngineMsgBox->ShowModal();
		//return;

		//////////////////////////////////////////////////////////////////////////
	}
	else if ( _tcsicmp( msgID, _T("WM_DUI_MSGBOX") ) == 0 )
	{
		TCHAR lpYesParam[MAX_PATH];
		TCHAR lpNoParam[MAX_PATH];
		ZeroMemory( lpYesParam, MAX_PATH );
		ZeroMemory( lpNoParam, MAX_PATH );
		popstring( lpYesParam );
		popstring( lpNoParam );

		LPCTSTR lpTitle = ( LPCTSTR )wParam;
		LPCTSTR lpText = ( LPCTSTR )lParam;
		LPCTSTR lpYes = ( LPCTSTR )lpYesParam;
		LPCTSTR lpNo = ( LPCTSTR )lpNoParam;

		if ( g_pDuiEngineMsgBox == NULL )
		{
			if( IDYES == MessageBox( hwnd, lpText, lpTitle, MB_YESNO))  //
			{
				ExitProcess( 0 );
			}
			else
			{
				pushint( -1 );
			}
		}
		else
		{
			if ( IDYES == DuiMessageBox( hwnd, lpTitle, lpText, lpYes, lpNo, false ) )  //if ( IDYES == MessageBox( hwnd, lpText, lpTitle, MB_YESNO ) )  //
			{
				pushint( 0 );
			}
			else
			{
				pushint( -1 );
			}
		}

		return;
	}
	else if ( _tcsicmp( msgID, _T("WM_DUI_STARTINSTALL") ) == 0 )
		::SendMessage( hwnd, WM_DUI_STARTINSTALL, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_STARTUNINSTALL") ) == 0 )
		::SendMessage( hwnd, WM_DUI_STARTUNINSTALL, ( WPARAM )g_installPageTabName.GetData(), ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_FINISHEDINSTALL") ) == 0 )
		::SendMessage( hwnd, WM_DUI_FINISHEDINSTALL, ( WPARAM )wParam, ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_FINISHEDUNINSTALL") ) == 0 )
        ::SendMessage( hwnd, WM_DUI_FINISHEDUNINSTALL, ( WPARAM )wParam, ( LPARAM )lParam );
	else if ( _tcsicmp( msgID, _T("WM_DUI_OPTIONSTATE") ) == 0 ) // 返回Option的状态
	{
		COptionUI* pOption = static_cast<COptionUI*>( g_pDuiEngine->GetPaintManager().FindControl( wParam ) );
		if ( pOption == NULL )
			return;
		pushint( !pOption->IsSelected() );
	}
	else if ( _tcsicmp( msgID, _T("WM_DUI_OPTIONSELECT") ) == 0 ) //设置Option的状态
	{
		COptionUI* pOption = static_cast<COptionUI*>( g_pDuiEngine->GetPaintManager().FindControl( wParam ) );
		if ( pOption == NULL )
			return;
		if ( _tcsicmp( lParam, _T("true") ) == 0 )
		{
			pOption->Selected( true );
		}
		else
		{
			pOption->Selected( false );
		}
	}
	else if ( _tcsicmp( msgID, _T("WM_DUI_VISIBLE") ) == 0 )
	{
		CControlUI* pControl = static_cast<CControlUI*>( g_pDuiEngine->GetPaintManager().FindControl( wParam ) );
		if ( pControl == NULL )
			return;
		//pushint( !pControl->IsVisible() );
		if ( pControl->IsVisible() )
		{
			pControl->SetVisible( false );
		} else {
			pControl->SetVisible( true );
		}
	}
	else if ( _tcsicmp( msgID, _T("WM_DUI_ENABLED") ) == 0 )
	{
		CControlUI* pControl = static_cast<CControlUI*>( g_pDuiEngine->GetPaintManager().FindControl( wParam ) );
		if ( pControl == NULL )
			return;
		//pushint( !pControl->IsVisible() );
		if ( pControl->IsEnabled() )
		{
			pControl->SetEnabled( false );
		} else {
			pControl->SetEnabled( true );
		}
	}

	else if ( _tcsicmp( msgID, _T("WM_DUI_OPENURL") ) == 0 )
	{
		CDuiString url = ( CDuiString )wParam;
		if ( ( url.Find( _T("http://") ) == -1 ) && ( url.Find( _T("https://") ) == -1 ) )
		{
			pushstring( _T("urlerror") );
			return;
		}
		//Garfield 2014.8.27  修复打开网址异步
		ShellExecute( NULL, _T("open"), url, NULL, NULL, SW_SHOW );

		/*
		CDuiString lpCmdLine = _T("explorer \"");
		lpCmdLine += url;
		lpCmdLine += _T("\"");
		USES_CONVERSION;

		//Garfield 2012.12.11 FadeButton
		//std::string strCmdLine = T2A( lpCmdLine.GetData() );	
#ifdef _UNICODE
		std::string strCmdLine = T2A( lpCmdLine.GetData() );
#else
		std::string strCmdLine = lpCmdLine.GetData();
#endif
		WinExec( strCmdLine.c_str(), SW_SHOWNORMAL );   
		*/
	}
    /*else if ( _tcsicmp( msgID, _T("WM_DUI_NOTIFYICON") ) == 0 )
        ::SendMessage( hwnd, WM_DUI_NOTIFYICON, ( WPARAM )wParam, ( LPARAM )lParam );*/
}

///////////////////////////////加载协议、打开网页////////////////////////////////////////////
// 替换字符串
int replace( /*const string&*/ const TCHAR* inStr, const TCHAR* pSrc, const TCHAR* pReplace )
{
    //string strSrc = inStr;
    //LPSTR lpch = ( char* )strSrc.c_str();

#ifdef _UNICODE
    LPWSTR lpch = ( TCHAR* )inStr;
    int nOldLength = wcslen( lpch );
    int nSourceLen = wcslen( pSrc );
    int nReplaceLen = wcslen( pReplace );
#else
    LPSTR lpch = ( TCHAR* )inStr;
    int nOldLength = strlen( lpch );
    int nSourceLen = strlen( pSrc );
    int nReplaceLen = strlen( pReplace );
#endif
    //替换源文本为空 返回原文本
    if ( nSourceLen == 0 )
    {
        return -1;
    }

#ifdef _UNICODE
    LPWSTR lpszStart = lpch;
    LPWSTR lpszEnd = lpszStart + nOldLength;
    LPWSTR lpszTarget;
#else
    LPSTR lpszStart = lpch;
    LPSTR lpszEnd = lpszStart + nOldLength;
    LPSTR lpszTarget;
#endif

    // 下面是替换的代码。
    while ( lpszStart < lpszEnd )
    {

#ifdef _UNICODE
        while ((lpszTarget = ( TCHAR* )wcsstr(( TCHAR* )lpszStart, ( TCHAR* )pSrc)) != NULL)
#else
        while ((lpszTarget = ( CHAR* )_mbsstr(( unsigned char* )lpszStart, ( unsigned char* )pSrc)) != NULL)
#endif
        {
            int nBalance = nOldLength - (lpszTarget - lpch + nSourceLen);
            memmove( lpszTarget + nReplaceLen, lpszTarget + nSourceLen, nBalance * sizeof(TCHAR) );
            memcpy( lpszTarget, pReplace, nReplaceLen*sizeof(TCHAR) );
            lpszStart = lpszTarget + nReplaceLen;
            lpszStart[nBalance] = '\0';
            nOldLength += (nReplaceLen - nSourceLen);
        }
        
#ifdef _UNICODE
        lpszStart += wcslen( lpszStart ) + 1;
#else
        lpszStart += strlen( lpszStart ) + 1;
#endif

    }
    return 0;
}
// 替换字符串
int replaceall(TCHAR *str,TCHAR *fstr,TCHAR *rstr)
{
    int i,j,sl,fsl,rsl,f=0;
    TCHAR *p;

#ifdef _UNICODE
    sl = wcslen(str);
    fsl = wcslen(fstr);
    rsl = wcslen(rstr);
#else
    sl = strlen(str);
    fsl = strlen(fstr);
    rsl = strlen(rstr);
#endif

    for(p = str; *p; p++)
    {
        if(*p != *fstr)
            continue;
        for(i = 1; fstr[i]; i++)
            if(p[i] != fstr[i])
                break;
        if(i != fsl)
            continue;
        f = 1;
        if(fsl > rsl)
        {
            j = p - str;
            for(i = rsl; i<=sl-j; i++)
                p[i] = p[i+fsl-rsl];
            sl -= fsl - rsl;
        }
        else if(fsl < rsl)
        {
            j = p - str;
            for(i=sl+rsl-fsl-j; i>=rsl; i--)
                p[i] = p[i+fsl-rsl];
            sl += rsl - fsl;
        }
        for(i=0; rstr[i]; i++,p++)
            *p = rstr[i];
        p--;
    }
    return f;
}
// wchar* -> char*
char* wtoc(const wchar_t* str)
{
    DWORD dwMinSize;
    dwMinSize = WideCharToMultiByte(CP_ACP,NULL,str,-1,NULL,0,NULL,FALSE); //计算长度
    char *bbb= new char[dwMinSize];
    WideCharToMultiByte(CP_OEMCP,NULL,str,-1,bbb,dwMinSize,NULL,FALSE);
    return bbb;
}
// char* -> wchar*
wchar_t* ctow(const char *str)
{
    wchar_t* buffer;
    if(str)
    {
        size_t nu = strlen(str);
        size_t n =(size_t)MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),NULL,0);
        buffer=0;
        buffer = new wchar_t[n+1];
        MultiByteToWideChar(CP_ACP,0,(const char *)str,int(nu),buffer,int(n));   
    }
    return buffer;
    delete buffer;
}
void ShowLicense( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  ShowLicense  -------------------------------------------------------------->") );
#endif

	TCHAR controlName[MAX_PATH];
    TCHAR fileName[MAX_PATH];
    TCHAR ReplaceSrcTxt[MAX_PATH];
    TCHAR ReplaceDstTxt[MAX_PATH];
    ZeroMemory( controlName, MAX_PATH );
    ZeroMemory( fileName, MAX_PATH );
    ZeroMemory( ReplaceSrcTxt, MAX_PATH );
    ZeroMemory( ReplaceDstTxt, MAX_PATH );
    popstring( controlName );
    popstring( fileName );
    popstring( ReplaceSrcTxt );
    popstring( ReplaceDstTxt );
    
	CDuiString finalFileName = g_skinPath + _T("\\") + fileName;	
	CRichEditUI* pRichEditControl = static_cast<CRichEditUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pRichEditControl == NULL )
		return;

	// 读许可协议文件，append到richedit中
	USES_CONVERSION;
	FILE* infile;
	char *pLicense = NULL;

	//Garfield 2012.12.11 FadeButton
    //infile = fopen( T2A( finalFileName.GetData() ), "r");
# ifdef _UNICODE 
    infile = fopen( T2A( finalFileName.GetData() ), "r");
    //_wfopen_s(&infile, finalFileName.GetData(), _T("r"));
# else
    infile = fopen( finalFileName.GetData(), "r");
    //fopen_s(&infile, finalFileName.GetData(), "r");
# endif
    if (!infile)   return ;  
	
	fseek( infile, 0,  SEEK_END );
	int nSize = ftell( infile );
	fseek( infile, 0, SEEK_SET );
    pLicense = new char[nSize];
	if ( pLicense == NULL )
	{
		fclose( infile );
		return;
	}
    ZeroMemory( pLicense, sizeof(char) * nSize );
    fread( pLicense, sizeof(char), nSize-1, infile);
    
# ifdef _UNICODE
    TCHAR *pLicenseTxt = NULL;
    pLicenseTxt = new TCHAR[nSize];
    ZeroMemory( pLicenseTxt, sizeof( TCHAR ) * nSize );
    pLicenseTxt = A2T(pLicense);
    
    //replaceall( pLicenseTxt, ReplaceSrcTxt, ReplaceDstTxt );
    replace( pLicenseTxt, ReplaceSrcTxt, ReplaceDstTxt );

    pRichEditControl->SetText( pLicenseTxt );
    //pRichEditControl->AppendText( pLicenseTxt );
    
	if ( pLicenseTxt != NULL )
	{
		delete [] pLicenseTxt;
	}
# else
    std::string cccc;
    cccc = replace( pLicense, ReplaceSrcTxt, ReplaceDstTxt );

    pRichEditControl->SetText( pLicense );
    //pRichEditControl->AppendText( cccc.c_str() );
# endif
	
	if ( pLicense != NULL )
	{
		delete [] pLicense;
	}
	
    fclose( infile );
    return;
}


void ShowWebBrowser( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  ShowWebBrowser  -------------------------------------------------------------->") );
#endif

	TCHAR controlName[MAX_PATH];
	TCHAR websiteName[MAX_PATH];
	ZeroMemory( controlName, MAX_PATH );
	ZeroMemory( websiteName, MAX_PATH );
	popstring( controlName );
	popstring( websiteName );

	CWebBrowserUI* pWebBrowserControl = static_cast<CWebBrowserUI*>( g_pDuiEngine->GetPaintManager().FindControl( controlName ) );
	if ( pWebBrowserControl == NULL )
		return;
	CDuiString csWebsite;
	csWebsite.Format( _T("%s"), websiteName );
	
	int nPos = csWebsite.Find(_T(":"));
	if (nPos == -1)
	{
	    csWebsite = g_skinPath + _T("\\") + websiteName;
	}
	
	if ( pWebBrowserControl->GetHomePage() != csWebsite )
	{
		pWebBrowserControl->SetHomePage( websiteName );
	}
	pWebBrowserControl->Navigate2( websiteName );

	//CString cccc;
	//cccc.Format( L"\tShow ieWebBrowser name = %s site = %s err = %d\r\n", controlName, websiteName, GetLastError() );
	//OutputDebugString( cccc );
	
}

///////////////////////////////定时器////////////////////////////////////////////////////////
void CALLBACK TimerProc( HWND hwnd, UINT uMsg, UINT_PTR idEvent, DWORD dwTime )
{
	g_pluginParms->ExecuteCodeSegment( idEvent - 1, 0 );
}

void DuiCreatTimer( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  DuiCreatTimer  -------------------------------------------------------------->") );
#endif

	UINT callback;
	UINT interval;

	callback = popint();
	interval = popint();

	if ( !callback || !interval )
		return;

	SetTimer( g_pDuiEngine->GetHWND(), callback, interval, TimerProc );
}
void DuiKillTimer( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  DuiKillTimer  -------------------------------------------------------------->") );
#endif

	UINT id;
	id = popint();
	KillTimer( g_pDuiEngine->GetHWND(), id );
}



///////////////////////////////选择文件夹////////////////////////////////////////////////////
int CALLBACK BrowseForFolderCallbackProc( HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData )
{
	if ( uMsg == BFFM_INITIALIZED )     //选择文件夹对话框初始化
	{
	    //设置默认路径为pData
		SendMessage( hwnd, BFFM_SETSELECTION, TRUE, lpData ); 
        //在STATUSTEXT区域显示当前路径  
        ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, 0, lpData); 
        //设置选择文件夹对话框的标题
		::SetWindowText(hwnd, TEXT("选择文件夹"));
	}
	else if ( uMsg == BFFM_SELCHANGED ) //选择文件夹变更时 
	{
	    TCHAR pszPath[MAX_PATH];  
        //获取当前选择路径  
        SHGetPathFromIDList((LPCITEMIDLIST)lParam, pszPath);  
        //在STATUSTEXT区域显示当前路径  
        ::SendMessage(hwnd, BFFM_SETSTATUSTEXT, TRUE, (LPARAM)pszPath);
	}
	return 0;
}


void SelectFolderDialog( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  SelectFolderDialog  -------------------------------------------------------------->") );
#endif

	TCHAR title[MAX_PATH];
	TCHAR path[MAX_PATH];
	TCHAR result[MAX_PATH];
	ZeroMemory( title, MAX_PATH );
	ZeroMemory( path, MAX_PATH );
	ZeroMemory( result, MAX_PATH );
	popstring( title );
	popstring( path );

    //CreateDirectory( path, NULL);
	//创建目录
	addDir( path );

	LPITEMIDLIST resultPIDL;
	BROWSEINFO bi;
	bi.hwndOwner = g_pDuiEngine->GetHWND();
	bi.pidlRoot = NULL;
	bi.pszDisplayName = NULL;
	bi.lpszTitle = title;
#ifndef BIF_NEWDIALOGSTYLE
#define BIF_NEWDIALOGSTYLE 0x0040
#endif
	bi.ulFlags = BIF_STATUSTEXT | BIF_RETURNONLYFSDIRS | BIF_NEWDIALOGSTYLE | BIF_NONEWFOLDERBUTTON;
	bi.lpfn = BrowseForFolderCallbackProc;
	bi.lParam = (LPARAM)path;
	bi.iImage = 0;

	resultPIDL = SHBrowseForFolder( &bi );
	if ( !resultPIDL )
	{
		pushint( -1 );
		return;
	}

	if ( SHGetPathFromIDList( resultPIDL, result ) )
	{
		if ( result[_tcslen( result )-1] == _T( '\\' ) )
		{
			//Garfield 2012.12.11 FadeButton
			//result[_tcslen( result )-1] = _T( '' );
#ifdef _UNICODE
		    result[_tcslen( result )-1] = _T( '' );
#else
		    result[_tcslen( result )-1] = NULL;
#endif
		}
		pushstring( result );
	}
	else
		pushint( -1 );

	CoTaskMemFree( resultPIDL );
}


///////////////////////////////隐藏窗体//////////////////////////////////////////////////////
inline BOOL SetWindowTransparent( HWND hwnd, BOOL bOpaque, UINT nAlpha )//隐藏窗体
{
	// Check parameter
	if ( !::IsWindow( hwnd ) ) {
		return FALSE;
	}
	// nAlpha needs between 0 and 255
	if ( nAlpha > 255 ) {
		return FALSE;
	}

	if ( bOpaque ) {
		// Set WS_EX_LAYERED on this window 
		SetWindowLong( hwnd, 
			GWL_EXSTYLE, 
			GetWindowLong( hwnd, GWL_EXSTYLE ) | WS_EX_LAYERED );

		// Make this window 70% alpha
		::SetLayeredWindowAttributes( hwnd, 0, ( 255 * nAlpha ) / 100, LWA_ALPHA );
	} else {
		// Remove WS_EX_LAYERED from this window styles
		::SetWindowLong( hwnd, 
			GWL_EXSTYLE, 
			GetWindowLong( hwnd, GWL_EXSTYLE ) & ~WS_EX_LAYERED );
	}

	// Ask the window and its children to repaint
	::RedrawWindow( hwnd, 
		NULL, 
		NULL, 
		RDW_ERASE | RDW_INVALIDATE | RDW_FRAME | RDW_ALLCHILDREN );

	return TRUE;
}
///////////////////////////////安装核心//////////////////////////////////////////////////////
BOOL CALLBACK UIKOWindowProc( HWND hwnd, UINT message, WPARAM wParam, LPARAM lParam ) //NSIS安装卸载消息回调 DuiEngine
{
	BOOL res = 0;
	std::map<HWND, WNDPROC>::iterator iter = g_windowInfoMap.find( hwnd );
	if ( iter != g_windowInfoMap.end() )
	{
	
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("UIKOWindowProc  hwnd=%d  message10=%.8d  message16=0x%.8x  err=%d  wParam=%d  lParam=%d\r\n"), hwnd, message, message, GetLastError(), wParam, lParam );
    OutputDebugString( cccc );
#endif

		if ( message == WM_PAINT )  //0x000F
		{
			ShowWindow( hwnd, SW_HIDE );
		}
		else if ( message == 0x1013 )
		{ //释放大文件时，进度条一直增加，伪增加。
			CProgressUI* pProgress = static_cast<CProgressUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_tempParam ) );
			if ( pProgress == NULL ) return 0;
			int nvalue = pProgress->GetValue();
			if (nvalue >= 30000)
            {
                nvalue = 30000;
			}
			else
            {
                nvalue += 10;
			}
			pProgress->SetValue( nvalue );
			//////////////////////////////////////////////////////////////////////////进度比例值
			if (g_tempParamValue.GetData() != NULL)
			{
				CLabelUI* pValue = static_cast<CLabelUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_tempParamValue ) );
				if ( pValue == NULL ) return 0;

				CString iValue;
				iValue.Format(_T("%d.%d %s"), nvalue/300, (nvalue%300)/30, "%" );

				pValue->SetText( iValue );
			}
			//////////////////////////////////////////////////////////////////////////任务栏进度
			if ( m_pTaskBarList != NULL )
			{
				m_pTaskBarList->SetProgressValue( g_pDuiEngine->GetHWND(), nvalue, 30000 );
				if ( pProgress->GetValue() == 30000 )
				{
					m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NOPROGRESS );
					m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NORMAL );
				}
			}
			//////////////////////////////////////////////////////////////////////////end
		}
		else if ( message == LVM_SETITEMTEXT )    //0x102E
		{
			// TODO  安装细节显示  等找到消息再写
			CLabelUI* pDetail = static_cast<CLabelUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_tempParamDetail ) );
			if ( pDetail == NULL )
				return 0;
			pDetail->SetText( ( LPCTSTR )wParam );
		}
		else if ( message == PBM_SETPOS )    //0x0402
		{ //释放文件增加真实数据。
			CProgressUI* pProgress = static_cast<CProgressUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_tempParam ) );
			pProgress->SetMaxValue( 30000 );//Garfield 30000->1000 2014.7.31 
			if ( pProgress == NULL ) return 0;
			pProgress->SetValue( ( int )wParam );
			//////////////////////////////////////////////////////////////////////////进度比例值
			if (g_tempParamValue.GetData() != NULL)
			{
				CLabelUI* pValue = static_cast<CLabelUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_tempParamValue ) );
				if ( pValue == NULL ) return 0;

				CString iValue;
				iValue.Format(_T("%d.%d %s"), (int)wParam/300, (int)(wParam%300)/30, "%" );

				pValue->SetText( iValue );
			}
			//////////////////////////////////////////////////////////////////////////任务栏进度
			if ( m_pTaskBarList != NULL )
			{
				m_pTaskBarList->SetProgressValue( g_pDuiEngine->GetHWND(), ( int )wParam, 30000 );
				if ( pProgress->GetValue() == 30000 )
				{
					m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NOPROGRESS );
					m_pTaskBarList->SetProgressState( g_pDuiEngine->GetHWND(), TBPF_NORMAL );
				}
			}
			//////////////////////////////////////////////////////////////////////////安装完成 跳到下一页
			if ( pProgress->GetValue() == 30000 )    //Garfield 30000->1000 2014.7.31 
			{
				CTabLayoutUI* pTab = NULL;
				int currentIndex;
				pTab = static_cast<CTabLayoutUI*>( g_pDuiEngine->GetPaintManager().FindControl( g_installPageTabName ) );
				if ( pTab == NULL )
					return -1;
				currentIndex = pTab->GetCurSel();
				//Garfield 安装到下一页
				pTab->SelectItem( currentIndex + 1 );
				//Garfield 安装完不到下一页
				//pTab->SelectItem( currentIndex );
			}
			//////////////////////////////////////////////////////////////////////////end
		}
		else
		{
			res = CallWindowProc( iter->second, hwnd, message, wParam, lParam );
		}
/*
安装    //0x000c  //0x000f  //0x0014  //0x0018  //0x0028  //0x0046  //0x0047
        //0x004e  //0x007f  //0x0085  //0x0086  //0x0087  //0x00ae  //0x0136  //0x0127
        //0x0400  //0x0402  //0x1004  //0x1007  //0x1013  //0x10c1  //0x102E
        
退出    //0x001c  //0x0018  //0x0090  //0x0002  //0x0082
        
错误    //  2--系统找不到指定的文件  6--句柄无效  18--没有更多文件  80--文件存在  87--参数错误  183--当文件已存在时，无法创建该文件

--> 2--系统找不到指定的文件
        0x1004  0x1007  0x1013  0x10c1
--> 6--句柄无效
        0x000c  0x0018  0x0028  0x0046  0x0086  0x0087  0x00ae  0x0400  0x0402  0x1004  0x1007  0x1013  0x10c1
--> 18--没有更多文件
        0x1004  0x1007  0x1013  0x10c1
--> 80--文件存在
        0x004e  0x0402  0x1004  0x1007  0x10c1  0x1013
--> 87--参数错误
        0x0402  0x1004  0x1007
--> 183--当文件已存在时，无法创建该文件
        0x1004  0x1007  0x1013  0x10c1
*/
	}	
	return res;
}
void InstallCore( HWND hwndParent )
{
	TCHAR progressName[MAX_PATH];
	TCHAR progressValue[MAX_PATH];
	TCHAR progressDetail[MAX_PATH];
	ZeroMemory( progressName, MAX_PATH );
	ZeroMemory( progressValue, MAX_PATH );
	ZeroMemory( progressDetail, MAX_PATH );

	popstring( progressName );
	popstring( progressValue );
	popstring( progressDetail );
	
	g_tempParam = progressName;
	g_tempParamValue = progressValue;
	g_tempParamDetail = progressDetail;

	//隐藏安装闪屏 隐藏page instfiles
	SetWindowTransparent( hwndParent, TRUE, 0 );

	// 接管page instfiles的消息
	g_windowInfoMap[hwndParent] = ( WNDPROC ) SetWindowLong( hwndParent, GWL_WNDPROC, ( long ) UIKOWindowProc );

	HWND hProgressHWND = FindWindowEx( FindWindowEx( hwndParent, NULL, _T("#32770"), NULL ), NULL, _T("msctls_progress32"), NULL );
	g_windowInfoMap[hProgressHWND] = ( WNDPROC ) SetWindowLong( hProgressHWND, GWL_WNDPROC, ( long ) UIKOWindowProc );

	HWND hInstallDetailHWND = FindWindowEx( FindWindowEx( hwndParent, NULL, _T("#32770"), NULL ), NULL, _T("SysListView32"), NULL ); 
	g_windowInfoMap[hInstallDetailHWND] = ( WNDPROC ) SetWindowLong( hInstallDetailHWND, GWL_WNDPROC, ( long ) UIKOWindowProc );

	HWND hInstallWindowsHWND = FindWindowEx( FindWindowEx( hwndParent, NULL, _T("#32770"), NULL ), NULL, _T("#32770 ( 对话框 )"), NULL ); 
	g_windowInfoMap[hInstallWindowsHWND] = ( WNDPROC ) SetWindowLong( hInstallDetailHWND, GWL_WNDPROC, ( long ) UIKOWindowProc );
	
}
void StartInstall( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  StartInstall  -------------------------------------------------------------->") );
#endif

	//OutputDebugString( _T("inst") );
	InstallCore( hwndParent );

	//2014.10之前不执行
	//SYSTEMTIME tm;
	//GetSystemTime( &tm );
	//if ( tm.wYear < =  2014 && tm.wMonth < 10 ) { return ; }

}

void StartUninstall( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  StartUninstall  -------------------------------------------------------------->") );
#endif

	//OutputDebugString( _T("uninst") );
	InstallCore( hwndParent );

	//2014.10之前不执行
	//SYSTEMTIME tm;
	//GetSystemTime( &tm );
	//if ( tm.wYear < =  2014 && tm.wMonth < 10 ) { return ; }

}

///////////////////////////////任务栏图标////////////////////////////////////////////////////
void SetTrayIcon( bool bShowIcon, CString Icon, CString Tooltip )  /*ShowIcon true-显示 false-隐藏 */
{
    if ( bShowIcon )
    {
        if ( g_NotifyIcon == false )
        {
			g_NotifyIcon = true;
			//加载图标
			if (Icon.Find(_T(".ico")) > 0)
			{
                CString csIcon;
                csIcon.Format(_T("%s\\%s") , g_skinPath.GetData(), Icon.GetBuffer());
                HICON hIcon = ( HICON )LoadImage( NULL, csIcon, IMAGE_ICON, 
                    32, 
                    32, 
                    LR_DEFAULTCOLOR | LR_CREATEDIBSECTION | LR_LOADFROMFILE );
                    
				m_nid.hIcon = hIcon;//::LoadIcon(::GetModuleHandle(NULL), SS);//LPCTSTR SS = MAKEINTRESOURCE(installProductIcon);
			}
			
            //加载提示
            lstrcpy(m_nid.szTip, Tooltip);
            //显示图标
            Shell_NotifyIcon( NIM_ADD, &m_nid );
        }
        else
        {
            //加载提示
            lstrcpy(m_nid.szTip, Tooltip);
            //修改图标
            Shell_NotifyIcon( NIM_MODIFY, &m_nid );
        }
    }
    else
    {
        if ( g_NotifyIcon == true )
        {
            g_NotifyIcon = false;
            //隐藏图标
            Shell_NotifyIcon( NIM_DELETE, &m_nid );
        }
    }
}
void ShowTrayIcon( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  SetTrayIcon  -------------------------------------------------------------->") );
#endif

	TCHAR Icon[MAX_PATH];
	TCHAR ShowIcon[MAX_PATH];
    TCHAR Tooltip[MAX_PATH];

	ZeroMemory( Icon, MAX_PATH );
	ZeroMemory( ShowIcon, MAX_PATH );
    ZeroMemory( Tooltip, MAX_PATH );

    popstring( ShowIcon );
    popstring( Icon );
	popstring( Tooltip );
	
    if ( _tcsicmp( ShowIcon, _T("true") ) == 0 )
    {
        //显示图标
        SetTrayIcon(true, Icon, Tooltip);
    }
    else if ( _tcsicmp( ShowIcon, _T("false") ) == 0 )
    {
        //隐藏图标
        SetTrayIcon(false, Icon, Tooltip);
    }
}

///////////////////////////////下载核心//////////////////////////////////////////////////////
string analysisURL( string url ) //解析域名
{
	string serverName;
	string removeProtocol;

	int ret = -1;
	//ret = url.find( "http://");  
	if ( url.find( ("http://") ) != -1 )
	{
		removeProtocol = url.substr( strlen( "http://") );  
	}
	else if ( url.find( ("https://") ) != -1 )
	{
		removeProtocol = url.substr( strlen( "https://") );  
	}
	else
	{
		removeProtocol = url;  
	}

	ret = removeProtocol.find_first_of( '/' );  
	serverName = removeProtocol.substr( 0, ret );  
	return serverName;
}
int DownloadCore(TCHAR* dlUrl, TCHAR* dlLocalPath, TCHAR* dlName, TCHAR* pProgress)
{
    CProgressUI* m_DownProgress = static_cast<CProgressUI*>( g_pDuiEngine->GetPaintManager().FindControl( pProgress ) );

    //下载核心
	BOOL bRet = TRUE;
	BOOL bRes = FALSE;
	DWORD dwRead = 0;
    DWORD dwWrite = 0;
    TCHAR szBuff[MAX_PATH*10] = {0};
    HINTERNET hSession = NULL;
	HINTERNET hInterUrl = NULL;
    HANDLE hFile = INVALID_HANDLE_VALUE;

    long lTempFileSize = 0;
    long lPrecent = 0;
    CString csPrecent;

	hSession = InternetOpen( _T("Mozilla/5.0 ( compatible; MSIE 9.0; Windows NT 6.1; Win32; x86; Trident/5.0 )"), INTERNET_OPEN_TYPE_PRECONFIG, NULL, NULL, 0 );
	if (NULL == hSession)
	{
		bRet = FALSE;
		goto _Exit_Error;
	}

	hInterUrl = InternetOpenUrl(hSession, dlUrl, NULL, 0, INTERNET_FLAG_RELOAD, 0);
	if (NULL == hInterUrl)
	{
		bRet = FALSE;
		goto _Exit_Error;
	}

	//查询文件长度
	long lFileSize = 0;
	TCHAR wcBuf[1024] = _T("\0");
    DWORD dwLen = sizeof( wcBuf );
	DWORD bQuery;
	
	bQuery = HttpQueryInfo( hInterUrl, HTTP_QUERY_CONTENT_LENGTH, wcBuf, &dwLen, NULL );
	if ( !bQuery ) { return 0; }
	//lFileSize = atol( (char*)wcBuf )/100;
#if _UNICODE
	lFileSize = _wtol( (wchar_t*)wcBuf )/100;
#else
	lFileSize = atol( (char*)wcBuf )/100;
#endif
    if (lFileSize==0)
    {
        OutputDebugString(_T("lFileSize=0"));
    }
    
    //创建文件
	hFile = CreateFile(dlLocalPath, GENERIC_WRITE, FILE_SHARE_READ, 0, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, 0);
	if (INVALID_HANDLE_VALUE == hFile)
	{
		bRet = FALSE;
		goto _Exit_Error;
	}
    
    //保存数据
	while(1)
	{
		memset(szBuff, 0, sizeof(szBuff));
		bRes = InternetReadFile(hInterUrl, szBuff, sizeof(szBuff), &dwRead);
		if (!bRes)
		{
			bRet = FALSE;
			goto _Exit_Error;
		}
		if(0 == dwRead)
		{
			break;
		}

		bRes = WriteFile(hFile, szBuff, dwRead, &dwWrite, NULL);
		if (!bRes)
		{
			bRet = FALSE;
			goto _Exit_Error;
		}
		if (0 == dwWrite)
		{
			bRet = FALSE;
			goto _Exit_Error;
		}
		
        lTempFileSize = lTempFileSize + dwRead;
        lPrecent = lTempFileSize/lFileSize;
		
		if ( m_DownProgress != NULL )
		{
			m_DownProgress->SetValue( lPrecent );
		}
		else
		{
            if ( g_NotifyIcon )
		    {
                csPrecent.Format(_T("%s\r\nDownloading %d%s"), dlName, lPrecent, _T("%"));
                SetTrayIcon(true, _T(""), csPrecent);
		    }
		}
		if ( lFileSize == ( lTempFileSize/100 ) )
		{
		    if ( g_NotifyIcon )
		    {
                csPrecent.Format(_T("%s\r\nnDownload complete."), dlName);
                SetTrayIcon(false, _T(""), csPrecent);
		    }
		    
            goto _Exit_Error;
		}
	}


_Exit_Error:
	if (INVALID_HANDLE_VALUE != hFile)
	{
		CloseHandle(hFile);
	}
	if (NULL != hInterUrl)
	{
		InternetCloseHandle(hInterUrl);
	}
	if (NULL != hSession)
	{
		InternetCloseHandle(hSession);
	}
	return 0;
}
static unsigned int WINAPI RoutionDownloadCore( void* pVoid )
{
	//下载核心线程
	DLWNDPARAM* pParam = (DLWNDPARAM*)pVoid;
	if (pParam == NULL)
	{
		return 0;
	}
	
	DownloadCore(pParam->dl_url,pParam->dl_path,pParam->dl_name,pParam->dl_progressname);
	
	return 0;
}
void StartDownload( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  StartDownload  -------------------------------------------------------------->") );
#endif

	TCHAR dlWaitType[MAX_PATH];
	ZeroMemory( dlWaitType, MAX_PATH );
	DLWNDPARAM* pParam = new DLWNDPARAM;
	pParam->wnd = NULL;

	popstring( dlWaitType );
	popstring( pParam->dl_url );
	popstring( pParam->dl_path );
	popstring( pParam->dl_name );
	popstring( pParam->dl_progressname );

    if ( _tcsicmp( dlWaitType, _T("wait") ) == 0 )
    {
        //直接下载
		DownloadCore(pParam->dl_url,pParam->dl_path,pParam->dl_name,pParam->dl_progressname);
    }
    else
    {
	    //下载文件线程
	    ::_beginthreadex( NULL, 0, RoutionDownloadCore, pParam, 0, NULL );
    }
	return;

}
///////////////////////////////文件夹管理////////////////////////////////////////////////////
void CreateDir( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{ 
    
	TCHAR psDirName[MAX_PATH];
	ZeroMemory( psDirName, MAX_PATH );

	popstring( psDirName );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  CreateDir  -------------------------------------------------------------->  a=%s \r\n"), psDirName );
    OutputDebugString( cccc );
#endif

    addDir( psDirName );

}

void DeleteDir( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
    
	TCHAR psDirName[MAX_PATH];
	ZeroMemory( psDirName, MAX_PATH );

	popstring( psDirName );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  DeleteDir  -------------------------------------------------------------->  a=%s \r\n"), psDirName );
    OutputDebugString( cccc );
#endif

    //删除文件夹
    //removeDir( ( const char * )psDirName );
    
    //string cDirname(psDirName);
    //去掉只读删除文件夹
    DelDirFileOpt( psDirName );

}
///////////////////////////////窗体尺寸//////////////////////////////////////////////////////
void SetWndSize( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR FullScreenRefresh[MAX_PATH];
	ZeroMemory( FullScreenRefresh, MAX_PATH );
	
	int cx = popint();
    int cy = popint();
    popstring( FullScreenRefresh );
    
#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  SetWndSize  -------------------------------------------------------------->  a=%d  b=%d \r\n"), cx, cy );
    OutputDebugString( cccc );
#endif

    if( g_pDuiEngine != NULL )
    {
        RECT rcClient;
        ::GetWindowRect(g_pDuiEngine->GetHWND(), &rcClient);
        MoveWindow(g_pDuiEngine->GetHWND(), rcClient.left, rcClient.top, cx, cy, FALSE);
        
        if ( _tcsicmp( FullScreenRefresh, _T("false") ) == 0 )
        {
            //局部刷新
            ::GetUpdateRect(g_pDuiEngine->GetHWND(), &rcClient, FALSE);
        }
        else
        {
            //全屏刷新
            ::InvalidateRect(NULL,NULL,TRUE);
        }
        
        pushint(TRUE);
    }
    else
    {
        pushint(FALSE);
    }
}
/////////////////////////////////////////////////////////////////////////////////////////////
void ShowDebugString( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
 {
    TCHAR parameter[MAX_PATH];
    ZeroMemory( parameter, MAX_PATH );

    popstring( parameter );
	{
        OutputDebugString( parameter );
    }
}
//////////////////////////////////////////////////////////////////////////
void ShowPage( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  ShowPage  -------------------------------------------------------------->") );
#endif

	if( g_pDuiEngine != NULL )
	    ShowWindow( g_pDuiEngine->GetHWND(), TRUE );
	if( g_pDuiEngineMsgBox != NULL )
	    ShowWindow( g_pDuiEngineMsgBox->GetHWND(), FALSE );

	//NSIS消息线程
	//::_beginthreadex( NULL, 0, RoutionNSISMessageProc, NULL, 0, NULL );

    g_bMSGLoopFlag = TRUE;
    
	//NSIS消息
	MSG msg = { 0 };
	while( ::GetMessage( &msg, NULL, 0, 0 ) && g_bMSGLoopFlag ) 
	{
#ifdef _DEBUG
		//OutputDebugString( _T("ShowPage g_bMSGLoopFlag = true") );
#endif

		::TranslateMessage( &msg );    //将键盘消息转换为字符消息
		::DispatchMessage( &msg );     //将消息传给窗体函数去处理.   该函数调度一个消息给窗口程序
	}

}
void DestroyDuiEngine( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  DestroyDuiEngine  -------------------------------------------------------------->") );
#endif

    if(g_pDuiEngine->GetHWND() != NULL)
    {
	    //隐藏窗口
	    ShowWindow( g_pDuiEngine->GetHWND(), SW_HIDE );
	}
}
void ExitDuiEngine( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{

#ifdef _DEBUG
    OutputDebugString( _T("Debug  ExitDuiEngine  -------------------------------------------------------------->") );
#endif

    //隐藏图标
    if ( g_NotifyIcon )
    {
        SetTrayIcon(false, _T(""), _T(""));
    }
    
    //关进程
    ExitProcess( 0 );
}
//////////////////////////////////////////////////////////////////////////
void DuiEngineMessageBox( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	TCHAR lblTitle[MAX_PATH];
    TCHAR edtText[MAX_PATH];
    TCHAR btnYes[MAX_PATH];
    TCHAR btnNo[MAX_PATH];

	ZeroMemory( lblTitle, MAX_PATH );
    ZeroMemory( edtText, MAX_PATH );
    ZeroMemory( btnYes, MAX_PATH );
    ZeroMemory( btnNo, MAX_PATH );

    popstring( lblTitle );
    popstring( edtText );
    popstring( btnYes );
    popstring( btnNo );

#ifdef _DEBUG
    CDuiString cccc;
    cccc.Format( _T("Debug  DuiMessageBox  --------------------------------------------------------------> a=%s b=%s c=%s d=%s \r\n"), lblTitle, edtText, btnYes, btnNo );
    OutputDebugString( cccc );
#endif
    
}
//-------------------------------------------------------------------------------------------
void InitDuiEngineMsgBox( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	popstring( g_msgbox_xmlName );

	popstring( g_msgbox_lblTitle );
	popstring( g_msgbox_edtText );

	popstring( g_msgbox_btnClose );
	popstring( g_msgbox_btnYes );
	popstring( g_msgbox_btnNo );

	TCHAR installProductShadow[MAX_PATH];
	ZeroMemory( installProductShadow, MAX_PATH );
	popstring( installProductShadow ); // 窗口阴影

	if ( g_pDuiEngineMsgBox == NULL )
	{
		g_pDuiEngineMsgBox = new DuiLib::CSkinEngineMsgBox();
		if ( g_pDuiEngineMsgBox == NULL ) return;
		g_pDuiEngineMsgBox->SetSkinXMLPath( g_msgbox_xmlName );
		//g_pDuiEngineMsgBox->Create( hwnd, _T("DuiEngineMsgBox"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 0, 0 );
		g_pDuiEngineMsgBox->Create( g_pDuiEngine->GetHWND(), _T("DuiEngineMsgBox"), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_TOOLWINDOW, 0, 0, 0, 0 );

		////MessageBox窗口阴影
		if ( _tcsicmp( installProductShadow, _T("true") ) == 0 )
		{
			bool bInit = CWndShadow::Initialize( CPaintManagerUI::GetInstance() );
			HWND hWnd = g_pDuiEngineMsgBox->GetHWND();
			msgboxwndShadow.Create( hWnd );
			msgboxwndShadow.SetSize(5);
			msgboxwndShadow.SetPosition(0,0);
		}

		//屏幕居中显示
		g_pDuiEngineMsgBox->CenterWindow();

		g_pDuiEngineMsgBox->ShowWindow( SW_HIDE );

	}

}
void DuiEngineMsgBox( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra )
{
	HWND hwnd = ( HWND )popint();
	TCHAR wParam[MAX_PATH];
	TCHAR lParam[MAX_PATH];

	ZeroMemory( wParam, MAX_PATH );
	ZeroMemory( lParam, MAX_PATH );

	popstring( wParam );
	popstring( lParam );

	//Duilib弹窗消息窗口
	LPCTSTR lpTitle = ( LPCTSTR )wParam;
	LPCTSTR lpText = ( LPCTSTR )lParam;

	CControlUI* pTitleControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ) );
	CControlUI* pTipTextControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ) );
	if ( pTitleControl != NULL )
		pTitleControl->SetText( lpTitle );
	if ( pTipTextControl != NULL )
		pTipTextControl->SetText( lpText );

	//屏幕居中显示
	g_pDuiEngineMsgBox->CenterWindow();

	int ret = g_pDuiEngineMsgBox->ShowModal();

	//Garfield 无用了
	g_pDuiEngineMsgBox = NULL;
	if ( ret == IDOK )
	{
		pushint( 0 );
	}
	pushint( -1 );
}
UINT DuiMessageBox( HWND hwndParent, LPCTSTR lpTitle, LPCTSTR lpText, LPCTSTR lpYes, LPCTSTR lpNo,  bool bYesBtnVisible )
{ //消息弹窗

	if ( g_pDuiEngineMsgBox == NULL )
	{
		g_pDuiEngineMsgBox = new DuiLib::CSkinEngineMsgBox();
		if ( g_pDuiEngineMsgBox == NULL ) return IDNO;
		g_pDuiEngineMsgBox->SetSkinXMLPath( g_msgbox_xmlName );
		g_pDuiEngineMsgBox->Create( hwndParent, _T(""), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW, 0, 0, 0, 0 );

		//父窗口居中显示
		g_pDuiEngineMsgBox->ParentWindowCenter( hwndParent );
		//屏幕居中显示
		//g_pDuiEngineMsgBox->CenterWindow();

	}

	CControlUI* pTitleControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ) );
	CControlUI* pTipTextControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ) );
	CControlUI* pYesBtnControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_btnYes ) );
	CControlUI* pNoBtnControl = static_cast<CControlUI*>( g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_btnNo ) );
	if ( pTitleControl != NULL )
		pTitleControl->SetText( lpTitle );
	if ( pTipTextControl != NULL )
		pTipTextControl->SetText( lpText );
	if ( ( pYesBtnControl != NULL ) && (lpYes != _T("-1")) )
		pYesBtnControl->SetText( lpYes );
	if ( ( pNoBtnControl != NULL ) && (lpYes != _T("-1")) )
		pNoBtnControl->SetText( lpNo );
	if ( ( pYesBtnControl != NULL ) && ( lpYes == _T("") ) )
	{
		pYesBtnControl->SetVisible( bYesBtnVisible );
	}
	else
	{
		pYesBtnControl->SetVisible( bYesBtnVisible );
	}

	//父窗口居中显示
	g_pDuiEngineMsgBox->ParentWindowCenter( hwndParent );
	//屏幕居中显示
	//g_pDuiEngineMsgBox->CenterWindow();

	int ret = g_pDuiEngineMsgBox->ShowModal();
	g_pDuiEngineMsgBox = NULL;
	if ( ret == IDOK )
	{
		//OutputDebugString( _T("IDYES") );
		return IDYES;
	}
	//OutputDebugString( _T("IDNO") );
	return IDNO;
}




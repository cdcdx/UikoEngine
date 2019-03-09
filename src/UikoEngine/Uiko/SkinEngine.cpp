#include "StdAfx.h"
#include "api.h"
#include "SkinEngine.h"
//#include "WndShadow.h"
#include <process.h>

extern extra_parameters* g_pluginParms;

//extern BOOL g_bMSGBoxLoopFlag;
extern BOOL g_bMSGLoopFlag;

extern DuiLib::CSkinEngine* g_pDuiEngine; 
extern DuiLib::CSkinEngineMsgBox* g_pDuiEngineMsgBox;

extern TCHAR g_msgbox_xmlName[MAX_PATH];
extern TCHAR g_msgbox_lblTitle[MAX_PATH];
extern TCHAR g_msgbox_edtText[MAX_PATH];
extern TCHAR g_msgbox_btnClose[MAX_PATH]; 
extern TCHAR g_msgbox_btnYes[MAX_PATH]; 
extern TCHAR g_msgbox_btnNo[MAX_PATH]; 

//extern unsigned int WINAPI RoutionNSISMessageProc( void* pvoid );
extern int NSISMessageProc();

namespace DuiLib {

	//消息弹窗
	UINT DuiMessageBox(HWND hwndParent, LPCTSTR lpTitle, LPCTSTR lpText, LPCTSTR lpYes, LPCTSTR lpNo,  bool bYesBtnVisible)
	{
		//g_bMSGBoxLoopFlag = FALSE;

		if( g_pDuiEngineMsgBox == NULL )
		{
			g_pDuiEngineMsgBox = new DuiLib::CSkinEngineMsgBox();
			if( g_pDuiEngineMsgBox == NULL ) return IDNO;
			g_pDuiEngineMsgBox->SetSkinXMLPath( g_msgbox_xmlName );
			g_pDuiEngineMsgBox->Create( hwndParent, _T(""), UI_WNDSTYLE_FRAME, WS_EX_STATICEDGE | WS_EX_APPWINDOW , 0, 0, 0, 0 );

			//父窗口居中显示
			g_pDuiEngineMsgBox->ParentWindowCenter( hwndParent );
			//屏幕居中显示
			//g_pDuiEngineMsgBox->CenterWindow();

		}

		CControlUI* pTitleControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ));
		CControlUI* pTipTextControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ));
		CControlUI* pYesBtnControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_btnYes ));
		CControlUI* pNoBtnControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_btnNo ));
		if( pTitleControl != NULL )
			pTitleControl->SetText( lpTitle );
		if( pTipTextControl != NULL )
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
		if( ret == IDOK )
		{
			//OutputDebugString( _T("IDYES") );
			return IDYES;
		}
		//OutputDebugString( _T("IDNO") );
		return IDNO;
	}

    //  CSkinEngine
	void CSkinEngine::Notify(TNotifyUI& msg)
	{
		//CDuiString name = msg.sType;
		//OutputDebugString("\r\nCSkinEngine Notify ");
		//OutputDebugString(name);
		//name = msg.pSender->GetName();
		//OutputDebugString(name);

		std::map<CDuiString, int >::iterator iter = m_controlCallbackMap.find( msg.pSender->GetName() );
		if( _tcsicmp( msg.sType, _T("click") ) == 0 ){
			if( iter != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
		}
		else if( _tcsicmp( msg.sType, _T("itemclick") ) == 0 ){
			if( iter != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
		}
		else if( _tcsicmp( msg.sType, _T("itemselect") ) == 0 ){
			if( iter != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
		}
		else if( _tcsicmp( msg.sType, _T("textchanged") ) == 0 ){
			if( iter != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
		}
		else if( _tcsicmp( msg.sType, _T("tabselect") ) == 0 ){
			if( iter != m_controlCallbackMap.end() )
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
		}
		else
		{
			//OutputDebugString("CSkinEngine Notify ------------------------>");
			//OutputDebugString(msg.sType);
		}
	}

	LRESULT CSkinEngine::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create( GetSkinXMLPath(), (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier( this );
		ShowWindow( FALSE );

		//----------------------------------------------------------------------------------
		CWebBrowserUI* pWebBrowserControl = static_cast<CWebBrowserUI*>(m_pm.FindControl( _T("ieWebBrowser") ));
		if( pWebBrowserControl != NULL )
		{
			pWebBrowserControl->SetWebBrowserEventHandler(this);
			pWebBrowserControl->Navigate2(_T("about:blank"));
		}
		//---------------------------------------------------------------------------------
		return 0;
	}

	LRESULT CSkinEngine::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT CSkinEngine::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CSkinEngine::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

    //inline HBITMAP CreateMyBitmap(HDC hDC, int cx, int cy, COLORREF** pBits)
    //{
	   // LPBITMAPINFO lpbiSrc = NULL;
	   // lpbiSrc = (LPBITMAPINFO) new BYTE[sizeof(BITMAPINFOHEADER)];
	   // if (lpbiSrc == NULL) return NULL;

	   // lpbiSrc->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	   // lpbiSrc->bmiHeader.biWidth = cx;
	   // lpbiSrc->bmiHeader.biHeight = cy;
	   // lpbiSrc->bmiHeader.biPlanes = 1;
	   // lpbiSrc->bmiHeader.biBitCount = 32;
	   // lpbiSrc->bmiHeader.biCompression = BI_RGB;
	   // lpbiSrc->bmiHeader.biSizeImage = cx * cy;
	   // lpbiSrc->bmiHeader.biXPelsPerMeter = 0;
	   // lpbiSrc->bmiHeader.biYPelsPerMeter = 0;
	   // lpbiSrc->bmiHeader.biClrUsed = 0;
	   // lpbiSrc->bmiHeader.biClrImportant = 0;

	   // HBITMAP hBitmap = CreateDIBSection(hDC, lpbiSrc, DIB_RGB_COLORS, (void **)pBits, NULL, NULL);
	   // delete[] lpbiSrc;
	   // return hBitmap;
    //}
    //LRESULT CSkinEngine::DoPaint()
    //{
	   // RECT rcClient = { 0 };
	   // ::GetClientRect(m_hWnd, &rcClient);
	   // DWORD dwWidth = rcClient.right - rcClient.left;
	   // DWORD dwHeight = rcClient.bottom - rcClient.top;

	   // HDC hDcPaint = ::GetDC(m_hWnd);
	   // HDC hDcBackground = ::CreateCompatibleDC(hDcPaint);
	   // COLORREF* pBackgroundBits;
	   // HBITMAP hbmpBackground = CreateMyBitmap(hDcPaint, dwWidth, dwHeight, &pBackgroundBits);
	   // ::ZeroMemory(pBackgroundBits, dwWidth * dwHeight * 4);
	   // HBITMAP hOldBitmap = (HBITMAP)::SelectObject(hDcBackground, hbmpBackground);
	   // SetBkMode(hDcBackground, OPAQUE);
	   // m_pm.GetRoot()->SetPos(rcClient);
	   // m_pm.GetRoot()->DoPaint(hDcBackground, rcClient);
    //	
	   // RECT rcWnd = { 0 };
	   // ::GetWindowRect(m_hWnd, &rcWnd);

	   // BLENDFUNCTION bf = { AC_SRC_OVER, 0, 255, AC_SRC_ALPHA };
	   // POINT ptPos = { rcWnd.left, rcWnd.top };
	   // SIZE sizeWnd = { dwWidth, dwHeight };
	   // POINT ptSrc = { 0, 0 };
	   // UpdateLayeredWindow(m_hWnd, hDcPaint, &ptPos, &sizeWnd, hDcBackground, &ptSrc, 0, &bf, ULW_ALPHA);

	   // ::SelectObject(hDcBackground, hOldBitmap);
	   // if (hDcBackground != NULL) ::DeleteDC(hDcBackground);
	   // if (hbmpBackground != NULL) ::DeleteObject(hbmpBackground);
	   // ::ReleaseDC(m_hWnd, hDcPaint);
	   // return 0;
    //}
	LRESULT CSkinEngine::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
				if( pControl && 
				    _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
                    _tcscmp(pControl->GetClass(), _T("FadeButtonUI")) != 0 &&
                    _tcscmp(pControl->GetClass(), _T("TextUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("EditUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("RichEditUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ListUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("TreeViewUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ActiveXUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("WebBrowserUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 
				  )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CSkinEngine::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
// 		SIZE szRoundCorner = m_pm.GetRoundCorner();
// 		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
// 			CDuiRect rcWnd;
// 			::GetWindowRect(*this, &rcWnd);
// 			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
// 			rcWnd.right++; rcWnd.bottom++;
// 			RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
// 			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
// 			HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
// 			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
// 			::SetWindowRgn(*this, hRgn1, TRUE);
// 			::DeleteObject(hRgn1);
// 			::DeleteObject(hRgn2);
// 		}
// 
// 		bHandled = FALSE;
// 		return 0;

		SIZE szRoundCorner = m_pm.GetRoundCorner();
#if defined(WIN32) && !defined(UNDER_CE)
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			HRGN hRgn = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom, szRoundCorner.cx, szRoundCorner.cy);
			::SetWindowRgn(*this, hRgn, TRUE);
			::DeleteObject(hRgn);
		}
#endif
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngine::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bZoomed = ::IsZoomed(this->GetHWND());
		if ((wParam & 0xFFF0 ) == SC_MAXIMIZE ||(wParam &  0xFFF0) == SC_SIZE)
		{ 
			bHandled = TRUE;
			return 0;
		}
		else if ((wParam & 0xFFF0 ) == SC_CLOSE)  //Garfield 2014/8/25 屏蔽
		{
			//OutputDebugString(_T("Alt+F4"));
			if ( g_pDuiEngineMsgBox == NULL )
			{
				if( IDYES == MessageBox( m_hWnd, _T("是否关闭程序？"), _T("温馨提示"), MB_YESNO))  //
				{
					pushint( 0 );
					ExitProcess( 0 );
				}
				else
				{
					pushint( -1 );
				}
			}
			else
			{
				if( IDYES == DuiMessageBox( m_hWnd, _T("温馨提示"), _T("是否关闭程序？"), _T("是"), _T("否"), 1 ))  //if( IDYES == MessageBox( m_hWnd, _T("是否关闭程序？"), _T("温馨提示"), MB_YESNO))  //
				{
					ExitProcess( 0 );
				}
				else
				{
					pushint( -1 );
				}
			}
			return 0;

		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CSkinEngine::OnUIKOMinMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		SendMessage(WM_SYSCOMMAND, SC_MINIMIZE, 0);
		return lRes;
	}
	LRESULT CSkinEngine::OnUIKOMaxMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		SendMessage(WM_SYSCOMMAND, SC_MAXIMIZE, 0);
		return lRes;
	}
	LRESULT CSkinEngine::OnUIKORestoreMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		SendMessage(WM_SYSCOMMAND, SC_RESTORE, 0);
		return lRes;
	}

	LRESULT CSkinEngine::OnUIKOCloseMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		Close();
		return lRes;
	}

	LRESULT CSkinEngine::OnUIKOBackMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		if( currentIndex == 0 )
			return -1;
		else
			pTab->SelectItem( currentIndex - 1 );

		return 0;
	}

	LRESULT CSkinEngine::OnUIKONextMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;
		currentIndex = pTab->GetCurSel();
		pTab->SelectItem( currentIndex + 1 );

		return 0;
	}
	LRESULT CSkinEngine::OnUIKOCancelMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//Duilib弹窗消息窗口
		LPCTSTR lpTitle = (LPCTSTR)wParam;
		LPCTSTR lpText = (LPCTSTR)lParam;

		CControlUI* pTitleControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ));
		CControlUI* pTipTextControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ));
		if( pTitleControl != NULL )
			pTitleControl->SetText( lpTitle );
		if( pTipTextControl != NULL )
			pTipTextControl->SetText( lpText );

		//屏幕居中显示
		g_pDuiEngineMsgBox->CenterWindow();

		int ret = g_pDuiEngineMsgBox->ShowModal();
		return 0;

	}
	LRESULT CSkinEngine::OnUIKOMSGBOX(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		//Duilib弹窗消息窗口
		LPCTSTR lpTitle = (LPCTSTR)wParam;
		LPCTSTR lpText = (LPCTSTR)lParam;

		CControlUI* pTitleControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_lblTitle ));
		CControlUI* pTipTextControl = static_cast<CControlUI*>(g_pDuiEngineMsgBox->GetPaintManager().FindControl( g_msgbox_edtText ));
		if( pTitleControl != NULL )
			pTitleControl->SetText( lpTitle );
		if( pTipTextControl != NULL )
			pTipTextControl->SetText( lpText );

		//屏幕居中显示
		g_pDuiEngineMsgBox->CenterWindow();

		int ret = g_pDuiEngineMsgBox->ShowModal();
		return 0;

	}
	LRESULT CSkinEngine::OnUIKOStartInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;

		currentIndex = pTab->GetCurSel();
		//Garfield 安装到下一页
		pTab->SelectItem( currentIndex + 1 );
		//Garfield 安装完不到下一页
		//pTab->SelectItem( currentIndex );

		g_bMSGLoopFlag = FALSE;

		//OutputDebugString(_T("OnUIKOStartInstallMSG---------------------------------->"));
		return lRes;
	}

	LRESULT CSkinEngine::OnUIKOStartUninstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		CTabLayoutUI* pTab = NULL;
		LPCTSTR tabName = (LPCTSTR)wParam;
		int currentIndex;
		pTab = static_cast<CTabLayoutUI*>( m_pm.FindControl( tabName ) );
		if( pTab == NULL )
			return -1;

		currentIndex = pTab->GetCurSel();
		//Garfield 安装到下一页
		pTab->SelectItem( currentIndex + 1 );
		//Garfield 安装完不到下一页
		//pTab->SelectItem( currentIndex );

		g_bMSGLoopFlag = FALSE;

		return lRes;
	}

	LRESULT CSkinEngine::OnUIKOFinishedInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		Close();
		return lRes;
	}
	LRESULT CSkinEngine::OnUIKOFinishedUninstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LRESULT lRes = 0;
		Close();
		return lRes;
	}

	LRESULT CSkinEngine::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
			case WM_CREATE:          lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
			case WM_CLOSE:           lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
			case WM_DESTROY:         lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
			case WM_NCACTIVATE:      lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
			case WM_NCCALCSIZE:      lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
			case WM_NCPAINT:         lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
            case WM_NCHITTEST:       lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
            //case WM_PAINT:            lRes = OnPaint(uMsg, wParam, lParam, bHandled); break;
            case WM_SIZE:            lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
			case WM_SYSCOMMAND:      lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
            case WM_DUI_MIN:         lRes = OnUIKOMinMSG(uMsg, wParam, lParam, bHandled); break;
            case WM_DUI_MAX:         lRes = OnUIKOMaxMSG(uMsg, wParam, lParam, bHandled); break;
            case WM_DUI_RESTORE:     lRes = OnUIKORestoreMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_CLOSE:       lRes = OnUIKOCloseMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_BACK:        lRes = OnUIKOBackMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_NEXT:        lRes = OnUIKONextMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_CANCEL:      lRes = OnUIKOCancelMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_MSGBOX:      lRes = OnUIKOMSGBOX(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_STARTINSTALL:       lRes = OnUIKOStartInstallMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_STARTUNINSTALL:     lRes = OnUIKOStartUninstallMSG(uMsg, wParam, lParam, bHandled); break;
			case WM_DUI_FINISHEDINSTALL:    lRes = OnUIKOFinishedInstallMSG(uMsg, wParam, lParam, bHandled); break;
            case WM_DUI_FINISHEDUNINSTALL:  lRes = OnUIKOFinishedUninstallMSG(uMsg, wParam, lParam, bHandled); break;
            case WM_DUI_NOTIFYICON:
                switch (lParam)
                {
                    case WM_MOUSEMOVE:
                        OutputDebugString(_T("WM_MOUSEMOVE"));
                        
                        break;
                    /*case WM_RBUTTONUP:
                        break;
                    case WM_RBUTTONDOWN:
                        break;
                    case WM_RBUTTONDBLCLK:
                        //POINT point;
                        //GetCursorPos(&point);
                        //OnRButtonDown(0, CPoint(point));
                        break;
                    case WM_LBUTTONUP:
                        break;
                    case WM_LBUTTONDOWN:
                        break;
                    case WM_LBUTTONDBLCLK:
                        //ShowWindow(true);
                        //::SetFocus(m_hWnd);
                        //::SetForegroundWindow(m_hWnd);
                        break;
                    default:
                        break;*/
                }
                break;
			default:
				bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void  CSkinEngine::SetSkinXMLPath( LPCTSTR path )
	{
		_tcscpy( m_skinXMLPath, path );
		//OutputDebugString(m_skinXMLPath);
	}

	LPCTSTR CSkinEngine::GetSkinXMLPath()
	{
		return m_skinXMLPath;
	}

	CPaintManagerUI& CSkinEngine::GetPaintManager()
	{
		return m_pm;
	}



	
	//  CSkinEngineMsgBox
	void CSkinEngineMsgBox::Notify(TNotifyUI& msg)
	{
		//CDuiString name = msg.sType;
		//OutputDebugString("\r\nCSkinEngineMsgBox Notify ");
		//OutputDebugString(name);
		//name = msg.pSender->GetName();
		//OutputDebugString(name); 

		std::map<CDuiString, int >::iterator iter = m_controlCallbackMap.find( msg.pSender->GetName() );
		if( _tcsicmp( msg.sType, _T("click") ) == 0 ){
			if( iter != m_controlCallbackMap.end() ){
				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
			}
			else if( _tcsicmp( msg.pSender->GetName(), g_msgbox_btnClose ) == 0 ){
 				SetParent( this->GetHWND(), NULL );
 				ShowWindow( SW_HIDE, FALSE );
 				::EnableWindow(g_pDuiEngine->GetHWND(), TRUE);
				::SetFocus(g_pDuiEngine->GetHWND());

				{
					//NSISMessageProc();
					//NSIS消息线程
					//::_beginthreadex( NULL, 0, RoutionNSISMessageProc, NULL, 0, NULL );
				}

				//Close(IDCANCEL);
				pushstring( _T("IDCANCEL") );
			}
			else if( _tcsicmp( msg.pSender->GetName(), g_msgbox_btnYes ) == 0 ){
				SetParent( this->GetHWND(), NULL );
				ShowWindow( SW_HIDE, FALSE );
				::EnableWindow(g_pDuiEngine->GetHWND(), TRUE);
				::SetFocus(g_pDuiEngine->GetHWND());

				{
					//NSISMessageProc();
					//NSIS消息线程
					//::_beginthreadex( NULL, 0, RoutionNSISMessageProc, NULL, 0, NULL );
				}

				//Close(IDOK);
				pushstring( _T("IDYES") );

				PostQuitMessage(0);
				ExitProcess( 0 );
			}
			else if( _tcsicmp( msg.pSender->GetName(), g_msgbox_btnNo ) == 0 ){
 				SetParent( this->GetHWND(), NULL );
 				ShowWindow( SW_HIDE, FALSE );
 				::EnableWindow(g_pDuiEngine->GetHWND(), TRUE);
				::SetFocus(g_pDuiEngine->GetHWND());

				{
					//NSISMessageProc();
					//NSIS消息线程
					//::_beginthreadex( NULL, 0, RoutionNSISMessageProc, NULL, 0, NULL );
				}

				//Close(IDCANCEL);
				pushstring( _T("IDNO") );
			}
		}
 		else if( _tcsicmp( msg.sType, _T("textchanged") ) == 0 ){
 			if( iter != m_controlCallbackMap.end() )
 				g_pluginParms->ExecuteCodeSegment( iter->second - 1, 0 );
 		}
	}

	LRESULT CSkinEngineMsgBox::OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		LONG styleValue = ::GetWindowLong(*this, GWL_STYLE);
		styleValue &= ~WS_CAPTION;
		::SetWindowLong(*this, GWL_STYLE, styleValue | WS_CLIPSIBLINGS | WS_CLIPCHILDREN);
		m_pm.Init(m_hWnd);
		CDialogBuilder builder;
		CControlUI* pRoot = builder.Create( GetSkinXMLPath(), (UINT)0, NULL, &m_pm);
		ASSERT(pRoot && "Failed to parse XML");
		m_pm.AttachDialog(pRoot);
		m_pm.AddNotifier( this );
		ShowWindow( FALSE );
		return 0;
	}

	LRESULT CSkinEngineMsgBox::OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		bHandled = FALSE;
		
		return 0;
	}

	LRESULT CSkinEngineMsgBox::OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		::PostQuitMessage(0L);
		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngineMsgBox::OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		if( ::IsIconic(*this) ) bHandled = FALSE;
		return (wParam == 0) ? TRUE : FALSE;
	}

	LRESULT CSkinEngineMsgBox::OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}
	LRESULT CSkinEngineMsgBox::ParentWindowCenter(HWND hwndParent)
	{
		//父窗口居中显示
		RECT rcParent;
		::GetWindowRect( hwndParent, &rcParent);
		SIZE rcMsgBox = m_pm.GetClientSize();

		RECT rcNew;
		rcNew.left = ( (rcParent.right - rcParent.left) - rcMsgBox.cx )/2 + rcParent.left ;
		rcNew.top = ( (rcParent.bottom - rcParent.top) - rcMsgBox.cy )/2 + rcParent.top ;
		//rcNew.right = rcNew.left + rcMsgBox.cx ;
		//rcNew.bottom = rcNew.top + rcMsgBox.cy ;
		MoveWindow(this->GetHWND(), rcNew.left, rcNew.top, rcMsgBox.cx, rcMsgBox.cy, FALSE);

		return 0;
	}
	LRESULT CSkinEngineMsgBox::OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		return 0;
	}

	LRESULT CSkinEngineMsgBox::OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		POINT pt; pt.x = GET_X_LPARAM(lParam); pt.y = GET_Y_LPARAM(lParam);
		::ScreenToClient(*this, &pt);

		RECT rcClient;
		::GetClientRect(*this, &rcClient);

		if( !::IsZoomed(*this) ) {
			RECT rcSizeBox = m_pm.GetSizeBox();
			if( pt.y < rcClient.top + rcSizeBox.top ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTTOPLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTTOPRIGHT;
				return HTTOP;
			}
			else if( pt.y > rcClient.bottom - rcSizeBox.bottom ) {
				if( pt.x < rcClient.left + rcSizeBox.left ) return HTBOTTOMLEFT;
				if( pt.x > rcClient.right - rcSizeBox.right ) return HTBOTTOMRIGHT;
				return HTBOTTOM;
			}
			if( pt.x < rcClient.left + rcSizeBox.left ) return HTLEFT;
			if( pt.x > rcClient.right - rcSizeBox.right ) return HTRIGHT;
		}

		RECT rcCaption = m_pm.GetCaptionRect();
		if( pt.x >= rcClient.left + rcCaption.left && pt.x < rcClient.right - rcCaption.right \
			&& pt.y >= rcCaption.top && pt.y < rcCaption.bottom ) {
				CControlUI* pControl = static_cast<CControlUI*>(m_pm.FindControl(pt));
                if( pControl && 
                    _tcscmp(pControl->GetClass(), _T("ButtonUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("OptionUI")) != 0 &&
                    _tcscmp(pControl->GetClass(), _T("FadeButtonUI")) != 0 &&
                    _tcscmp(pControl->GetClass(), _T("TextUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("EditUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("RichEditUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ListUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("TreeViewUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ActiveXUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("WebBrowserUI")) != 0 && 
                    _tcscmp(pControl->GetClass(), _T("ScrollBarUI")) != 0 
                  )
					return HTCAPTION;
		}

		return HTCLIENT;
	}

	LRESULT CSkinEngineMsgBox::OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		SIZE szRoundCorner = m_pm.GetRoundCorner();
		if( !::IsIconic(*this) && (szRoundCorner.cx != 0 || szRoundCorner.cy != 0) ) {
			CDuiRect rcWnd;
			::GetWindowRect(*this, &rcWnd);
			rcWnd.Offset(-rcWnd.left, -rcWnd.top);
			rcWnd.right++; rcWnd.bottom++;
			RECT rc = { rcWnd.left, rcWnd.top + szRoundCorner.cx, rcWnd.right, rcWnd.bottom };
			HRGN hRgn1 = ::CreateRectRgnIndirect( &rc );
			HRGN hRgn2 = ::CreateRoundRectRgn(rcWnd.left, rcWnd.top, rcWnd.right, rcWnd.bottom - szRoundCorner.cx, szRoundCorner.cx, szRoundCorner.cy);
			::CombineRgn( hRgn1, hRgn1, hRgn2, RGN_OR );
			::SetWindowRgn(*this, hRgn1, TRUE);
			::DeleteObject(hRgn1);
			::DeleteObject(hRgn2);
		}

		bHandled = FALSE;
		return 0;
	}

	LRESULT CSkinEngineMsgBox::OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled)
	{
		BOOL bZoomed = ::IsZoomed(this->GetHWND());
		if ((wParam & 0xFFF0 ) == SC_MAXIMIZE ||(wParam &  0xFFF0) == SC_SIZE)
		{ 
			bHandled = TRUE;
			return 0;
		}
		LRESULT lRes = CWindowWnd::HandleMessage(uMsg, wParam, lParam);
		return lRes;
	}

	LRESULT CSkinEngineMsgBox::HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam)
	{
		LRESULT lRes = 0;
		BOOL bHandled = TRUE;
		switch( uMsg ) {
			case WM_CREATE:        lRes = OnCreate(uMsg, wParam, lParam, bHandled); break;
			case WM_CLOSE:         lRes = OnClose(uMsg, wParam, lParam, bHandled); break;
			case WM_DESTROY:       lRes = OnDestroy(uMsg, wParam, lParam, bHandled); break;
			case WM_NCACTIVATE:    lRes = OnNcActivate(uMsg, wParam, lParam, bHandled); break;
			case WM_NCCALCSIZE:    lRes = OnNcCalcSize(uMsg, wParam, lParam, bHandled); break;
			case WM_NCPAINT:       lRes = OnNcPaint(uMsg, wParam, lParam, bHandled); break;
			case WM_NCHITTEST:     lRes = OnNcHitTest(uMsg, wParam, lParam, bHandled); break;
			case WM_SIZE:          lRes = OnSize(uMsg, wParam, lParam, bHandled); break;
			case WM_SYSCOMMAND:    lRes = OnSysCommand(uMsg, wParam, lParam, bHandled); break;
			default:
				 bHandled = FALSE;
		}
		if( bHandled ) return lRes;
		if( m_pm.MessageHandler(uMsg, wParam, lParam, lRes) ) return lRes;
		return CWindowWnd::HandleMessage(uMsg, wParam, lParam);
	}

	void  CSkinEngineMsgBox::SetSkinXMLPath( LPCTSTR path )
	{
		_tcscpy( m_skinXMLPath, path );
	}

	LPCTSTR CSkinEngineMsgBox::GetSkinXMLPath()
	{
		return m_skinXMLPath;
	}

	CPaintManagerUI& CSkinEngineMsgBox::GetPaintManager()
	{
		return m_pm;
	}

} // namespace DuiLib 
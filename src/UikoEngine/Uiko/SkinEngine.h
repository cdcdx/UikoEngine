#ifndef __SKINENGINE_H__
#define  __SKINENGINE_H__
#pragma  once

#include "../UIlib.h"
#include "pluginapi.h"
#include <map>
#include "MsgDef.h"

#define			Block_Num			4

namespace DuiLib {

// CSkinEngine
class CSkinEngine : public CWindowWnd, public INotifyUI
	,CWebBrowserEventHandler

{
public:
	virtual HRESULT STDMETHODCALLTYPE GetHostInfo(
		/* [out][in] */ DOCHOSTUIINFO __RPC_FAR *pInfo)
	{
		//MessageBox(m_hWnd, _T("test"), _T("asasas"), MB_OK);
		if (pInfo != NULL)
		{
			pInfo->dwFlags |= DOCHOSTUIFLAG_NO3DBORDER | DOCHOSTUIFLAG_SCROLL_NO;
			pInfo->dwDoubleClick = DOCHOSTUIDBLCLK_DEFAULT;
		}
		return S_OK;
	}

public:
	CSkinEngine() {}
	~CSkinEngine() {}

	LPCTSTR GetWindowClassName() const { return _T("DuiEngine"); }
	UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/)
	{ 
		m_pm.RemoveNotifier( this );	
		delete this; 
	}

	void Notify(TNotifyUI& msg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
 	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
  	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT DoPaint();
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnUIKOMinMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnUIKOMaxMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
    LRESULT OnUIKORestoreMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOCloseMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOBackMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKONextMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOCancelMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOMSGBOX(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOStartInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOStartUninstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOFinishedInstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnUIKOFinishedUninstallMSG(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);
	LRESULT MessageHandler(UINT uMsg, WPARAM wParam, LPARAM lParam, bool& bHandled);

	void    SetSkinXMLPath( LPCTSTR path );
	LPCTSTR GetSkinXMLPath();

	CPaintManagerUI& GetPaintManager();
 	void SaveToControlCallbackMap( CDuiString ctlName, int callback ) 
 	{ 
 		m_controlCallbackMap[ctlName] = callback; 
 	}

private:
	CPaintManagerUI            m_pm;
	TCHAR                      m_skinXMLPath[MAX_PATH];
	std::map<CDuiString, int>  m_controlCallbackMap;

};



// CSkinEngineMsgBox
class CSkinEngineMsgBox : public CWindowWnd, public INotifyUI
{
public:
	CSkinEngineMsgBox() {}
	~CSkinEngineMsgBox() {}

	LPCTSTR GetWindowClassName() const { return _T("DuiEngine"); }
	UINT GetClassStyle() const { return CS_DBLCLKS; }
	void OnFinalMessage(HWND /*hWnd*/) 
	{
		m_pm.RemoveNotifier(this);
		delete this;
	}

	void Notify(TNotifyUI& msg);

	LRESULT OnCreate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnClose(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnDestroy(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcActivate(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcCalcSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcPaint(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnNcHitTest(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT ParentWindowCenter(HWND hwndParent);//UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSize(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT OnSysCommand(UINT uMsg, WPARAM wParam, LPARAM lParam, BOOL& bHandled);
	LRESULT HandleMessage(UINT uMsg, WPARAM wParam, LPARAM lParam);

	void    SetSkinXMLPath( LPCTSTR path );
	LPCTSTR GetSkinXMLPath();

	CPaintManagerUI& GetPaintManager();
	void SaveToControlCallbackMap( CDuiString ctlName, int callback ) 
	{ 
		m_controlCallbackMap[ctlName] = callback; 
	}

	CPaintManagerUI              m_pm;
//public:
//	RECT  m_rcParent;
private:
	TCHAR                        m_skinXMLPath[MAX_PATH];
	std::map<CDuiString, int>    m_controlCallbackMap;

};

} // namespace DuiLib
#endif
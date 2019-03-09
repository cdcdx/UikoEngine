/************************************************************************/
/*  VC6.0实现Windows7任务栏特性头文件									*/
/*  Made by Garfield													*/
/************************************************************************/

//#include "StdAfx.h"
#include "../UIlib.h"

#ifndef __ITaskbarList_INTERFACE_DEFINED__
#define __ITaskbarList_INTERFACE_DEFINED__

#define __RPC__in
#define __RPC__in_opt
#define __RPC__in_opt_string
#define __RPC__in_ecount(size) 
#define __RPC__in_ecount_full(size)

//extern "C++"
//{
//    template<typename T> void** IID_PPV_ARGS_Helper(T** pp) 
//    {
//        static_cast<IUnknown*>(*pp);    // make sure everyone derives from IUnknown
//        return reinterpret_cast<void**>(pp);
//    }
//}

#define IID_PPV_ARGS(ppType) __uuidof(**(ppType)), IID_PPV_ARGS_Helper(ppType)

typedef enum THUMBBUTTONFLAGS
{	THBF_ENABLED	= 0,
	THBF_DISABLED	= 0x1,
	THBF_DISMISSONCLICK	= 0x2,
	THBF_NOBACKGROUND	= 0x4,
	THBF_HIDDEN	= 0x8,
	THBF_NONINTERACTIVE	= 0x10
} 	THUMBBUTTONFLAGS;

typedef enum THUMBBUTTONMASK
{	THB_BITMAP	= 0x1,
	THB_ICON	= 0x2,
	THB_TOOLTIP	= 0x4,
	THB_FLAGS	= 0x8
} 	THUMBBUTTONMASK;

typedef struct THUMBBUTTON
{
    THUMBBUTTONMASK dwMask;
    UINT iId;
    UINT iBitmap;
    HICON hIcon;
    WCHAR szTip[ 260 ];
    THUMBBUTTONFLAGS dwFlags;
} 	THUMBBUTTON;

typedef struct THUMBBUTTON *LPTHUMBBUTTON;

/* interface ITaskbarList */
/* [object][uuid] */ 

EXTERN_C const IID IID_ITaskbarList;

MIDL_INTERFACE("56FDF342-FD6D-11d0-958A-006097C9A090")
ITaskbarList : public IUnknown
{
public:
virtual HRESULT STDMETHODCALLTYPE HrInit( void) = 0;

virtual HRESULT STDMETHODCALLTYPE AddTab( 
    /* [in] */ __RPC__in HWND hwnd) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE DeleteTab( 
    /* [in] */ __RPC__in HWND hwnd) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE ActivateTab( 
    /* [in] */ __RPC__in HWND hwnd) = 0;
	
	virtual HRESULT STDMETHODCALLTYPE SetActiveAlt( 
    /* [in] */ __RPC__in HWND hwnd) = 0;
			
};
#endif

#ifndef __ITaskbarList2_INTERFACE_DEFINED__
#define __ITaskbarList2_INTERFACE_DEFINED__

/* interface ITaskbarList2 */
/* [object][uuid] */ 


EXTERN_C const IID IID_ITaskbarList2;

MIDL_INTERFACE("602D4995-B13A-429b-A66E-1935E44F4317")
ITaskbarList2 : public ITaskbarList
{
public:
virtual HRESULT STDMETHODCALLTYPE MarkFullscreenWindow( 
    /* [in] */ __RPC__in HWND hwnd,
    /* [in] */ BOOL fFullscreen) = 0;
			
};
#endif

#ifndef __ITaskbarList3_INTERFACE_DEFINED__
#define __ITaskbarList3_INTERFACE_DEFINED__

/* interface ITaskbarList3 */
/* [object][uuid] */ 

typedef /* [v1_enum] */ 
enum TBPFLAG
{	TBPF_NOPROGRESS	= 0,
	TBPF_INDETERMINATE	= 0x1,
	TBPF_NORMAL	= 0x2,
	TBPF_ERROR	= 0x4,
	TBPF_PAUSED	= 0x8
} 	TBPFLAG;

//DEFINE_ENUM_FLAG_OPERATORS(TBPFLAG)

EXTERN_C const IID IID_ITaskbarList3;

MIDL_INTERFACE("ea1afb91-9e28-4b86-90e9-9e9f8a5eefaf")
ITaskbarList3 : public ITaskbarList2
{
public:
    virtual HRESULT STDMETHODCALLTYPE SetProgressValue( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ ULONGLONG ullCompleted,
        /* [in] */ ULONGLONG ullTotal) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetProgressState( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ TBPFLAG tbpFlags) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE RegisterTab( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndMDI) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE UnregisterTab( 
        /* [in] */ __RPC__in HWND hwndTab) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetTabOrder( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndInsertBefore) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetTabActive( 
        /* [in] */ __RPC__in HWND hwndTab,
        /* [in] */ __RPC__in HWND hwndMDI,
        /* [in] */ DWORD dwReserved) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ThumbBarAddButtons( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ UINT cButtons,
        /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ThumbBarUpdateButtons( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ UINT cButtons,
        /* [size_is][in] */ __RPC__in_ecount_full(cButtons) LPTHUMBBUTTON pButton) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE ThumbBarSetImageList( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in_opt HIMAGELIST himl) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetOverlayIcon( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in HICON hIcon,
        /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszDescription) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetThumbnailTooltip( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [string][unique][in] */ __RPC__in_opt_string LPCWSTR pszTip) = 0;
		
		virtual HRESULT STDMETHODCALLTYPE SetThumbnailClip( 
        /* [in] */ __RPC__in HWND hwnd,
        /* [in] */ __RPC__in RECT *prcClip) = 0;
			
};
#endif
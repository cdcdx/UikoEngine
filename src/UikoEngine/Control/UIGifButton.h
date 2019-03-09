#ifndef __UIGIFBUTTON_H__
#define __UIGIFBUTTON_H__

#pragma once

namespace DuiLib {

	class CButtonUI;
	
	class CGifHandler
	{
	public:
		CGifHandler();
		virtual ~CGifHandler();
		int GetFrameCount();
		void AddFrameInfo(TImageInfo* pFrameInfo);
		TImageInfo* GetNextFrameInfo();
		TImageInfo* GetCurrentFrameInfo();
		TImageInfo* GetFrameInfoAt(int index);
	private:
		CStdPtrArray  ImageInfos;
			int nCurrentFrame;
		int nFrameCount;
		bool isDeleting;
	};
	
	class UILIB_API CGifButtonUI : public CButtonUI
	{
	public:	
		CGifButtonUI();
		~CGifButtonUI();

		LPCTSTR GetClass() const
		{
			return _T("GifButtonUI");
		}


		LPVOID GetInterface( LPCTSTR pstrName )
		{
			if( _tcscmp(pstrName, _T("GifButton")) == 0 ) 
				return static_cast<CGifButtonUI*>(this);

			return CControlUI::GetInterface(pstrName);
		}

		void SetNormalGifImage(LPCTSTR pstrName);
		void SetAttribute(LPCTSTR pstrName, LPCTSTR pstrValue);
		virtual void DoEvent(TEventUI& event);
		virtual void SetVisible(bool bVisible = true);//设置控件可视性
		virtual void PaintStatusImage(HDC hDC);

	protected:
		enum
		{
			GIF_TIMER_ID = 15
		};

		bool m_isUpdateTime;
		CGifHandler* m_pGif;
		int m_nPreUpdateDelay;

	};

} // namespace DuiLib

#endif // __UIGIFBUTTON_H__
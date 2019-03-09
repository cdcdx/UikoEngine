#ifndef MSGDEF_H
#define MSGDEF_H
#pragma once

#include <WinUser.h>

#define  WM_DUI_MIN                   WM_USER + 780
#define  WM_DUI_MAX                   WM_USER + 781
#define  WM_DUI_RESTORE               WM_USER + 782
#define  WM_DUI_CLOSE                 WM_USER + 783

#define  WM_DUI_BACK                  WM_USER + 784
#define  WM_DUI_BACK2                 WM_USER + 785
#define  WM_DUI_BACK3                 WM_USER + 786
#define  WM_DUI_NEXT                  WM_USER + 787
#define  WM_DUI_NEXT2                 WM_USER + 788
#define  WM_DUI_NEXT3                 WM_USER + 789
#define  WM_DUI_CANCEL                WM_USER + 790
#define  WM_DUI_MSGBOX                WM_USER + 791

#define  WM_DUI_HIDE                  WM_USER + 792     //隐藏窗口
#define  WM_DUI_NOTIFYICON            WM_USER + 793     //托盘图标

#define  WM_DUI_STARTINSTALL          WM_USER + 880     //开始安装
#define  WM_DUI_FINISHEDINSTALL       WM_USER + 881     //完成安装
#define  WM_DUI_STARTUNINSTALL        WM_USER + 882     //开始卸载
#define  WM_DUI_FINISHEDUNINSTALL     WM_USER + 883     //完成卸载

#define  WM_DUI_OPENURL               WM_USER + 884     //打开网址
#define  WM_DUI_VISIBLE               WM_USER + 885     //设置显示隐藏
#define  WM_DUI_ENABLED               WM_USER + 886     //设置不用可用
#define  WM_DUI_SETTEXT               WM_USER + 887     //设置文本

#define  WM_DUI_OPTIONSTATE           WM_USER + 980     //返回 Option 的状态
#define  WM_DUI_PROGRESSSTATE         WM_USER + 981     //返回 Progress 的进度

#endif
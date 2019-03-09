#ifndef __NSUIKOSKINENGINE_H__
#define __NSUIKOSKINENGINE_H__
#pragma once

#include "UIlib.h"
#include "stdafx.h"
#include "Uiko/pluginapi.h"
#include "Uiko/MsgDef.h"
#include <windows.h>

/* 参数： 1. skin的路径（相对setup.exe生成的路径）
 *        2. skin布局文件名
 *        3. 安装页面tab的名字
 * 功能： 初始化界面
*/
DLLEXPORT void InitDuiEngine(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. skin的路径（相对setup.exe生成的路径）
 *        2. skin布局文件名
 * 功能： 换肤
*/
DLLEXPORT void ReloadSkin(HWND hwndParent, int string_size, TCHAR *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. control的名字
 * 功能： 寻找特定的control是否存在
*/
DLLEXPORT void FindControl(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 有click事件的control的名字
 *        2. 许可协议文件名字
 * 功能： 为控件绑定对应的事件，有click消息时执行对应代码
*/
DLLEXPORT void BindControl(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);


/* 参数： 1. control的名字
 *        2. 赋给control的数据
 *        3. 数据的类型 (现在提供三种数据类型： 1. text; 2. bkimage; 3. link; 4. enable; 5.visible; 6.tooltip;)
 * 功能： 为控件绑定对应的事件，有click消息时执行对应代码
*/
DLLEXPORT void SetControlData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. control的名字
 *        2. 数据的类型 (现在提供一种数据类型： 1. text; 2.tooltip; 3.value )
 * 功能： 为控件绑定对应的事件，有click消息时执行对应代码
*/
DLLEXPORT void GetControlData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. Progress的名字
 *        2. 赋给Progress的数据
 *        3. 数据的类型 (现在提供三种数据类型： value )
 * 功能： 为控件绑定对应的事件，有click消息时执行对应代码
*/
DLLEXPORT void SetProgressData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. Progress的名字
 *        2. 数据的类型 (现在提供一种数据类型： value )
 * 功能： 为控件绑定对应的事件，有click消息时执行对应代码
*/
DLLEXPORT void GetProgressData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);


DLLEXPORT void SetListData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);
DLLEXPORT void GetListData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);
DLLEXPORT void SetComboData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);
DLLEXPORT void GetComboData(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);


/* 参数： 1. 消息HWND
 *        2. 消息ID
 *		  3. WPARAM
 *	      4. LPARAM
 * 功能： 发消息
*/
DLLEXPORT void DuiSendMessage(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. richedit control的名字
 *        2. 许可协议文件名字
 * 功能： 显示许可证文件
*/
DLLEXPORT void ShowLicense(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 网页控件的名字
 *        2. 网页名字
 * 功能： 显示网页
*/
DLLEXPORT void ShowWebBrowser(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);


/* 参数： 1. TimerID(一般是回调函数的ID)
 *        2. interval
 * 功能： 创建定时器
*/
DLLEXPORT void DuiCreatTimer(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. TimerID(一般是回调函数的ID)
 * 功能： 杀死定时器
*/
DLLEXPORT void DuiKillTimer(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 标题（例如： 请选择文件夹）
 * 功能： 发消息
*/
DLLEXPORT void SelectFolderDialog(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 响应开始安装进度的进度条名字
 * 功能： 开始安装响应
*/
DLLEXPORT void StartInstall(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 响应开始卸载进度的进度条名字
 * 功能： 开始安装响应
*/
DLLEXPORT void StartUninstall(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/*
 * 参数：显示/隐藏 托盘图标
 * 功能：
*/
DLLEXPORT void ShowTrayIcon( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra );

/* 参数： 1. 下载
 * 功能： 开始下载响应
*/
DLLEXPORT void StartDownload(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. 创建文件夹
 * 功能： 创建文件夹
*/
DLLEXPORT void CreateDir( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra );

/* 参数： 1. 删除文件夹
 * 功能： 删除文件夹
*/
DLLEXPORT void DeleteDir( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra );
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 参数： 无
 * 功能： 设置窗口大小
*/
DLLEXPORT void SetWndSize( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra );

/* 参数： 无
 * 功能： 输出Debug信息
*/
DLLEXPORT void ShowDebugString( HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra );

/* 参数： 无
 * 功能： 显示界面（注意：一定是最后才Show出来）
*/
DLLEXPORT void ShowPage(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 无
 * 功能： 隐藏窗口
*/
DLLEXPORT void DestroyDuiEngine(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 无
 * 功能： 退出安装
*/
DLLEXPORT void ExitDuiEngine(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);


////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/* 参数： 1. 布局文件的名字
 *        2. 标题控件名字
 *		  3. 提示内容控件名字
 *		  4. 关闭按钮控件名字
 *		  5. 确定按钮控件名字
 *		  6. 取消按钮控件名字
 * 功能： 初始化MessageBox
*/
DLLEXPORT void InitDuiEngineMsgBox(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

/* 参数： 1. MessageBox(选择yes no)
 * 功能： 
*/
DLLEXPORT void DuiEngineMsgBox(HWND hwndParent, int string_size, char *variables, stack_t **stacktop, extra_parameters *extra);

#endif




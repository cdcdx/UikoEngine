/***********************************************************
*                     编译NSIS ANSI
*   file:   dui.nsi
*   author: Garfield
*   taobao: http://shop64088102.taobao.com 
*   version:4.0.1.1001
*   lastdate:2015-09-24
************************************************************/

# 安装程序初始定义常量
!define PRODUCT_PACKTYPE              "Setup"                          # 打包类型( Setup 安装包/ Update 补丁包)
!define PRODUCT_NAME                  "UikoEngine"                     # 产品名称
!define PRODUCT_NAME_EN               "UikoEngine"                     # 产品英文名称
!define PRODUCT_NAME_MUTEX            "UikoEngine"                     # 互斥量名
!define PRODUCT_VERSION               "1.0.0.1"                        # 程序版本
!define PRODUCT_DISK                  "C:\"                            # 默认安装盘符(C:\或D:\)
!define PRODUCT_DISKPATH              "Program Files"                  # 默认安装目录(game 或 Program Files)
!define PRODUCT_PATH                  "UikoEngine"                     # 程序安装路径(安装目录)
!define PRODUCT_FILEPATH              "bin"                            # 程序打包路径(当前目录)
!define PRODUCT_SKINPATH              "skin"                           # 皮肤打包路径(当前目录)
!define PRODUCT_SKINZIP               "skin.rdn"
!define PRODUCT_MAIN_EXE              "Browser.exe"                    # 主程序名
!define PRODUCT_MAIN_UNEXE            "uninst.exe"                     # 卸载exe文件
!define PRODUCT_MAIN_SHORTCUT_TOOLTIP "${PRODUCT_NAME}"                # 快捷方式 提示信息
!define PRODUCT_MAIN_SHORTCUT_ICO     "inst.ico"                       # 快捷方式 ico文件(安装目录下***.ico)
!define PRODUCT_MAIN_SHORTCUT_RUN     ""                               # 快捷方式 主程序运行参数 
!define PRODUCT_MAIN_ICO              "${PRODUCT_SKINPATH}\inst.ico"          # 安装包ico文件
!define PRODUCT_MAIN_UNICO            "${PRODUCT_SKINPATH}\uninst.ico"        # 卸载程序ico文件

!include "dui.nsh"

/****** 安装区段02 ******/
Section "Install" Sec02
  
SectionEnd

/****** 卸载区段02 ******/
Section "Uninstall" un.Sec02

	# 删目录
	dui::DeleteDirectory "$INSTDIR"
	
SectionEnd

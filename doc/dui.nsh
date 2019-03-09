/***********************************************************
*                     打包NSIS源码V4.0
*   file:   dui.nsh
*   author: Garfield
*   taobao: http://shop64088102.taobao.com 
*   version:4.0.0.1001
*   lastdate:2015-06-21
************************************************************/

;安装包 解压空白
!system '>blank set/p=MSCF<nul'
!packhdr temp.dat 'cmd /c Copy /b temp.dat /b +blank&&del blank'
/*# UPX
!define HAVE_UPX 1
!ifdef HAVE_UPX
!packhdr tmp.dat "E:UPX\upx --best tmp.dat"
!endif
*/

;!define PRODUCT_WEB_AD                "http://.*/ad.html"                        # 网页广告
!define PRODUCT_WEB_SITE              "http://www.cdcdx.com/"                    # 完成打开网页
!define PRODUCT_WEB_LICENCE           "http://.*/license.html"                   # 网络协议链接 格式:"http://..."

!define PRODUCT_LANGUAGE              "2052"                                     # 2052-SimpChinese  1033-English
!define PRODUCT_COMMENTS              "UikoEngine"                               # 备注注释说明
!define PRODUCT_PUBLISHER             "www.cdcdx.com"
!define PRODUCT_PUBLISHER_EN          "CDCDX.COM"
!define PRODUCT_PUBLISHER_SITE        "http://www.cdcdx.com"                     # 发行公司官网
!define PRODUCT_HELP_LINK             "http://www.cdcdx.com"                     # 帮助链接
# ---------------------------------------------------------------------
!define PRODUCT_BRANDINGTEXT          "--绿色无毒无木马--如遇到阻止请选择允许"   # BrandingText
!define PRODUCT_ROOT_KEY              "HKLM"
!define PRODUCT_SUB_KEY               "Software\Microsoft\Windows\CurrentVersion\Uninstall\${PRODUCT_NAME_EN}"    # 注册表路径

!define MUI_ICON                      "${PRODUCT_MAIN_ICO}"                      # 安装ico
!define MUI_UNICON                    "${PRODUCT_MAIN_UNICO}"                    # 卸载ico

# 选择压缩方式
SetCompressor /SOLID lzma             ;zlib\bzip2\lzma

# 引入的头文件
!include "MUI.nsh"
;!include "MUI2.nsh"
!include "LogicLib.nsh"
!include "WinMessages.nsh"
!include "FileFunc.nsh"
!include "WordFunc.nsh"
!include "Library.nsh"
!include "StrFunc.nsh"
!include "x64.nsh"
!include "UAC.nsh"

!AddPluginDir "."

Var Dialog               # 自定义界面的窗口句柄
Var MessageBoxHandle     # 自定义消息界面句柄
Var DesktopIconState     # 添加到桌面快捷方式
Var QuickLaunchBarState  # 添加到快捷启动栏
Var StartMenuState       # 添加到开始菜单
Var BootRunState         # 开机启动程序
Var RunNowState          # 立即启动程序
Var FreeSpaceSize
Var installPath
;Var timerID
;Var timerIDUninstall
;Var InstallValue
Var InstallState         # 重复安装状态
Var LocalPath
Var PCOnline             # 联网状态
Var SystemTime           # 系统时间
Var WindowsVersion       # 系统版本

# 引入的dll
ReserveFile "${NSISDIR}\Plugins\inetc.dll"
ReserveFile "${NSISDIR}\Plugins\system.dll"
ReserveFile "${NSISDIR}\Plugins\TCP.dll"
ReserveFile "${NSISDIR}\Plugins\dui.dll"                               # 调用NSIS皮肤插件

# 安装界面包含的语言设置
!insertmacro MUI_LANGUAGE "SimpChinese"
# 安装预释放文件
!insertmacro MUI_RESERVEFILE_INSTALLOPTIONS

VIProductVersion "${PRODUCT_VERSION}"                                  # 版本(上方显示)
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "ProductName"       "${PRODUCT_NAME_EN}"         # 产品名称
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "Contact"           "${PRODUCT_PUBLISHER}"       # 公司(图标显示)
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "Comments"          "${PRODUCT_COMMENTS}"        # 备注注释    VIAddVersionKey /LANG=${LANG_ID}
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "CompanyName"       "${PRODUCT_PUBLISHER}"       # 公司(图标显示)
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "LegalTrademarks"   "${PRODUCT_NAME}"            # 合法商标
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "LegalCopyright"    "Copyright (C) ${PRODUCT_PUBLISHER_EN}. All Rights Reserved."    # 版权
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "FileDescription"   "${PRODUCT_NAME} 安装程序"   # 文件说明描述(上方显示)(图标显示)
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "FileVersion"       "${PRODUCT_VERSION}"         # 文件版本
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "ProductVersion"    "${PRODUCT_VERSION}"         # 产品版本
VIAddVersionKey /LANG=${PRODUCT_LANGUAGE}  "OriginalFilename"  "${PRODUCT_MAIN_EXE}"        # 原文件名


Name       "${PRODUCT_NAME}"                                           # 提示对话框的标题
OutFile    "${PRODUCT_NAME}_${PRODUCT_PACKTYPE}.exe"                   # 输出文件名 _V${PRODUCT_VERSION}
#InstallDir "$PROGRAMFILES\${PRODUCT_PATH}"                             # 默认安装路径 C盘
InstallDir "$LOCALAPPDATA\${PRODUCT_PATH}"                             # 默认安装路径 LOCALAPPDATA
InstallDirRegKey "${PRODUCT_ROOT_KEY}" "${PRODUCT_SUB_KEY}" "InstallPath"                   # 上次安装路径 注册表
BrandingText "${PRODUCT_NAME}${PRODUCT_BRANDINGTEXT}"                  # 版权文字

ShowInstDetails   show                                                 # 显示安装程序的细节      hide|show|nevershow
ShowUninstDetails show                                                 # 显示卸载程序的细节      hide|show|nevershow
RequestExecutionLevel admin                                            # 管理员权限 Vista/7/8 UAC
CRCCheck off

# -----------------------------自定义页面------------------------------------
Page         custom     inst_UI
Page         instfiles  "" inst_Progress
UninstPage   custom     un.uninst_UI
UninstPage   instfiles  "" un.uninst_Progress

# -----------------------------安装部分------------------------------------

# 安装Section
Section "Install" Sec01
    ${If} $InstallState == "Cover"
        SetOutPath "$TEMP"
        Delete "$INSTDIR\*.*"
        RMDir /r "$INSTDIR"
    ${EndIf}
    
    # 复制安装文件
    SetOutPath "$INSTDIR"
    SetOverwrite ifnewer
    
    File /r "${PRODUCT_FILEPATH}\*"
    
    SetOverwrite on
    SetRebootFlag false
    
SectionEnd

# 快捷方式 / 注册表
Section -AdditionalIcons
    
    # 开始菜单
    SetShellVarContext current
    StrCmp $StartMenuState "1" "" +5
        CreateDirectory "$SMPROGRAMS\${PRODUCT_NAME}"
        CreateShortCut  "$SMPROGRAMS\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "${PRODUCT_MAIN_SHORTCUT_RUN}" "" 0 SW_SHOWNORMAL "" "${PRODUCT_MAIN_SHORTCUT_TOOLTIP}"
        CreateShortCut  "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"       "$INSTDIR\${PRODUCT_MAIN_EXE}" "${PRODUCT_MAIN_SHORTCUT_RUN}" "" 0 SW_SHOWNORMAL "" "${PRODUCT_MAIN_SHORTCUT_TOOLTIP}"
        CreateShortCut  "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk"   "$INSTDIR\${PRODUCT_MAIN_UNEXE}" "" "" 0 SW_SHOWNORMAL "" "点击开始卸载"
    
    # 桌面快捷方式
    StrCmp $DesktopIconState "1" +1 +2
        CreateShortCut  "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "${PRODUCT_MAIN_SHORTCUT_RUN}" "" 0 SW_SHOWNORMAL "" "${PRODUCT_MAIN_SHORTCUT_TOOLTIP}"
    
    # 任务栏快捷方式
    StrCmp $QuickLaunchBarState "1" +1 taskbar
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
        ${if} $R0 >= 6.0  # Vista以上  锁定任务栏
            CreateShortCut "$DESKTOP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}"
            ExecShell taskbarpin "$DESKTOP\${PRODUCT_NAME}.lnk"
            ExecShell startpin "$DESKTOP\${PRODUCT_NAME}.lnk"
        ${else}           # XP以下  快速任务栏
            IfFileExists "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" 0 +2
                Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk";
            CreateShortCut "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}"
        ${Endif}
    taskbar:
    
    SetShellVarContext all
    
    Call GetSystemTime
    # 注册表信息
    WriteUninstaller "$INSTDIR\${PRODUCT_MAIN_UNEXE}"
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "BuildTime"       "$SystemTime"                    # 安装时间
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "Contact"         "${PRODUCT_PUBLISHER}"           # 出版商
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "Copyright"       "Copyright (C) ${PRODUCT_PUBLISHER_EN}. All Rights Reserved."   # 版权说明
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "DisplayIcon"     "$INSTDIR\${PRODUCT_MAIN_EXE}"   # 程序所在路径
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "DisplayName"     "${PRODUCT_NAME}"                # 程序名
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "DisplayNameEn"   "${PRODUCT_NAME_EN}"             # 程序英文名
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "DisplayVersion"  "${PRODUCT_VERSION}"             # 程序版本
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "HelpLink"        "${PRODUCT_HELP_LINK}"           # 帮助链接
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "InstallDate"     "$SystemTime"                    # 安装时间
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "InstallLocation" "$INSTDIR"                       # 安装目录
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "ProgramPath"     "$INSTDIR"                       # 安装目录
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "Publisher"       "${PRODUCT_PUBLISHER}"           # 出版商
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "URLInfoAbout"    "${PRODUCT_PUBLISHER_SITE}"      # 网站信息
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "URLUpdateInfo"   "${PRODUCT_PUBLISHER_SITE}"      # 网站信息
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "UninstallString" "$INSTDIR\${PRODUCT_MAIN_UNEXE}" # 卸载程序所在路径
    
    # 技术链接
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "SupportLink"     "http://shop64088102.taobao.com"            # 技术链接
    WriteRegStr ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "Support"         "Powered by UikoEngine"                     # 技术支持

SectionEnd

# 安装自定义界面
Function inst_UI
    
    # 初始化窗口
    dui::InitDuiEngine /NOUNLOAD "$TEMP\${PRODUCT_NAME_EN}Setup" "${PRODUCT_SKINZIP}" "invalid_string_position" "inst.xml" "inst.ico" "inst_UITab" "${PRODUCT_NAME}" "false" "true"
    ;dui::InitDuiEngine /NOUNLOAD "$TEMP\${PRODUCT_NAME_EN}Setup" "inst.xml" "inst.ico" "inst_UITab" "${PRODUCT_NAME}" "false" "false"
    Pop $Dialog

    # 初始化MessageBox窗口
    dui::InitDuiEngineMsgBox "msgbox.xml" "msgbox_lblTitle" "msgbox_edtText" "msgbox_btnClose" "msgbox_btnYes" "msgbox_btnNo" "true"
    Pop $MessageBoxHandle

    # ----------------------------第一个页面-----------------------------------------------
    
    # 取消按钮绑定函数
    dui::FindControl "inst_Welcome_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnClose button"
    ${Else}
        GetFunctionAddress $0 onGlobalCancelFunc
        dui::BindControl "inst_Welcome_btnClose"  $0
    ${EndIf}
    
    # 软件名称显示设定
    dui::FindControl "inst_Welcome_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_lblSoftName button"
    ${Else}
        ;dui::SetControlData "inst_Welcome_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "inst_Welcome_lblSoftName" $0
    ${EndIf}
    
    # 查看协议详情按钮绑定函数
    dui::FindControl "inst_Welcome_btnLicence"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnLicence button"
    ${Else}
        GetFunctionAddress $0 onLicenseLinkBtnFunc
        dui::BindControl "inst_Welcome_btnLicence"  $0 
    ${EndIf}
    
    # 已经阅读协议按钮绑定函数
    dui::FindControl "inst_Welcome_btnAccept"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnAccept button"
    ${Else}
        GetFunctionAddress $0 onAcceptLicenceFunc    ;安装控件不用可用
        dui::BindControl "inst_Welcome_btnAccept"  $0 
    ${EndIf}
    
    # 快速安装按钮绑定函数
    dui::FindControl "inst_Welcome_btnQuickInst"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnQuickInst button"
    ${Else}
        GetFunctionAddress $0 onQuickInstBtnFunc
        dui::BindControl "inst_Welcome_btnQuickInst"  $0
    ${EndIf}
    
    # 下一步按钮绑定函数
    dui::FindControl "inst_Welcome_btnNext"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnNext button"
    ${Else}
        GetFunctionAddress $0 onNextBtnFunc
        dui::BindControl "inst_Welcome_btnNext"  $0
    ${EndIf}
    
    # 下一步按钮绑定函数
    dui::FindControl "inst_Welcome_btnCover"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Welcome_btnCover button"
    ${Else}
        GetFunctionAddress $0 onNextBtnFunc
        dui::BindControl "inst_Welcome_btnCover"  $0
    ${EndIf}
    
    # ----------------------------第二个页面-----------------------------------------------
    
    # 取消按钮绑定函数
    dui::FindControl "inst_Option_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnClose button"
    ${Else}
        GetFunctionAddress $0 onGlobalCancelFunc
        dui::BindControl "inst_Option_btnClose"  $0
    ${EndIf}
    
    # 软件名称显示设定
    dui::FindControl "inst_Option_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_lblSoftName button"
    ${Else}
        ;dui::SetControlData "inst_Option_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "inst_Option_lblSoftName" $0
    ${EndIf}
    
    # 可用磁盘空间设定数据
    dui::FindControl "inst_Option_lblLastSpace"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_lblLastSpace button"
    ${Else}
        dui::SetControlData "inst_Option_lblLastSpace"  $FreeSpaceSize  "text"
    ${EndIf}
    
    # 安装路径编辑框设定数据
    dui::FindControl "inst_Option_edtPath"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnOpenPath button"
    ${Else}
        dui::SetControlData "inst_Option_edtPath"  $installPath "text"
        GetFunctionAddress $0 onTextChangeFunc
        dui::BindControl "inst_Option_edtPath" $0
    ${EndIf}

    # 安装路径浏览按钮绑定函数
    dui::FindControl "inst_Option_btnOpenPath"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnOpenPath button"
    ${Else}
        GetFunctionAddress $0 onInstallPathBrownBtnFunc
        dui::BindControl "inst_Option_btnOpenPath"  $0
    ${EndIf}

    # 创建开始菜单绑定函数
    dui::FindControl "inst_Option_btnStartMenu"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnStartMenu button"
    ${Else}
        StrCpy $StartMenuState "1"
        GetFunctionAddress $0 onStartMenuStateFunc
        dui::BindControl "inst_Option_btnStartMenu"  $0
    ${EndIf}

    # 创建桌面快捷方式绑定函数
    dui::FindControl "inst_Option_btnShortCut"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnShortCut button"
    ${Else}
        StrCpy $DesktopIconState "1"
        GetFunctionAddress $0 onDesktopIconStateFunc
        dui::BindControl "inst_Option_btnShortCut"  $0
    ${EndIf}

    # 创建任务栏快捷方式绑定函数
    dui::FindControl "inst_Option_btnQuickLaunchBar"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnQuickLaunchBar button"
    ${Else}
        GetFunctionAddress $0 onQuickLaunchBarStateFunc
        dui::BindControl "inst_Option_btnQuickLaunchBar"  $0
    ${EndIf}
    
    # 开机启动程序绑定函数
    dui::FindControl "inst_Complete_btnBootRun"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_btnBootRun button"
    ${Else}
        GetFunctionAddress $0 onBootRunStateFunc
        dui::BindControl "inst_Complete_btnBootRun"  $0
    ${EndIf}
    
    # 开始安装按钮绑定函数
    dui::FindControl "inst_Option_btnStartInst"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnStartInst button"
    ${Else}
        GetFunctionAddress $0 onStartInstBtnFunc
        dui::BindControl "inst_Option_btnStartInst"  $0
    ${EndIf}
    
    # 上一步按钮绑定函数
    dui::FindControl "inst_Option_btnBack"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_btnBack button"
    ${Else}
        GetFunctionAddress $0 onBackBtnFunc
        dui::BindControl "inst_Option_btnBack"  $0
    ${EndIf}
    
    # ----------------------------第三个页面-----------------------------------------------
    
    # 软件名称显示设定
    dui::FindControl "inst_Inst_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Inst_lblSoftName button"
    ${Else}
        ;dui::SetControlData "inst_Inst_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "inst_Inst_lblSoftName" $0
    ${EndIf}
    
    # 取消按钮绑定函数
    dui::FindControl "inst_Inst_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Inst_btnClose button"
    ${Else}
        GetFunctionAddress $0 onGlobalCancelFunc
        dui::BindControl "inst_Inst_btnClose"  $0
    ${EndIf}
    
    /*# 进度条数字显示绑定函数
    dui::FindControl "inst_Inst_lblPercent"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Inst_lblPercent button"
    ${Else}
        StrCpy $InstallValue  "0"
        GetFunctionAddress $timerID instProgressValue
        dui::DuiCreatTimer $timerID 1000  # callback interval
    ${EndIf}
    */
    # ----------------------------第四个页面-----------------------------------------------
    
    # 软件名称显示设定
    dui::FindControl "inst_Complete_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_lblSoftName button"
    ${Else}
        ;dui::SetControlData "inst_Complete_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "inst_Complete_lblSoftName" $0
    ${EndIf}
    
    # 取消按钮绑定函数
    dui::FindControl "inst_Complete_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_btnClose button"
    ${Else}
        GetFunctionAddress $0 onFinishedBtnFunc
        dui::BindControl "inst_Complete_btnClose"  $0
    ${EndIf}
    
    # 查看官方网站按钮绑定函数
    dui::FindControl "inst_Complete_btnWebsite"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_btnWebsite button"
    ${Else}
        GetFunctionAddress $0 onWebSiteLinkBtnFunc
        dui::BindControl "inst_Complete_btnWebsite"  $0 
    ${EndIf}
    
    # 立即体验按钮绑定函数
    dui::FindControl "inst_Complete_btnRun"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_btnRun button"
    ${Else}
        GetFunctionAddress $0 onFinishedRunBtnFunc
        dui::BindControl "inst_Complete_btnRun"  $0
    ${EndIf}
    
    # 安装完成按钮绑定函数
    dui::FindControl "inst_Complete_btnOK"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Complete_btnOK button"
    ${Else}
        GetFunctionAddress $0 onFinishedBtnFunc
        dui::BindControl "inst_Complete_btnOK"  $0
    ${EndIf}
    
    # ---------------------------------显示------------------------------------------------
    
    ${If} $InstallState == "Cover"
        ReadRegStr $LocalPath ${PRODUCT_ROOT_KEY} ${PRODUCT_SUB_KEY} "InstallLocation"
        StrCmp $LocalPath "" +4 0
        dui::SetControlData "inst_Option_edtPath" $LocalPath "text"
        
        dui::SetControlData "inst_Option_edtPath" "false" "enable"
        dui::SetControlData "inst_Option_btnOpenPath" "false" "enable"
        ;dui::DuiSendMessage $Dialog WM_DUI_ENABLED "inst_Option_edtPath"
        ;dui::DuiSendMessage $Dialog WM_DUI_ENABLED "inst_Option_btnOpenPath"
    
        dui::SetControlData "inst_Option_btnStartInst" "开始覆盖" "text"
        dui::SetControlData "inst_Welcome_btnQuickInst" "快速覆盖" "text"
        dui::SetControlData "inst_Welcome_btnNext" "false" "visible"
        dui::SetControlData "inst_Welcome_btnCover" "true" "visible"
        ;dui::DuiSendMessage $Dialog WM_DUI_VISIBLE "inst_Welcome_btnNext"
    ${EndIf}
    ;-----------------------------------------------------------------------------------------
    
    dui::ShowPage
    
FunctionEnd

# 安装初始化
Function .onInit
    
    # 360认证要求不能使用静默参数
    IfSilent 0 +2
    SetSilent normal
    
    # 创建临时目录  $PLUGINSDIR
    InitPluginsDir
    ;MessageBox MB_OK "$PLUGINSDIR"
    
    # 检测安装程序是否运行 创建互斥
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME_MUTEX}Mutex") i .r1 ?e'
    Pop $R0
    StrCmp $R0 0 +3
        MessageBox MB_ICONINFORMATION "${PRODUCT_NAME} 安装程序已经运行！"
        Abort
    ;MessageBox::show MB_ICONINFORMATION "温馨提示" "" "${PRODUCT_NAME} 安装程序已经运行！" "关闭"
    
    # 检测联网
    Call InternetOnline
    
    StrCpy $StartMenuState 1
    StrCpy $DesktopIconState 1
    StrCpy $QuickLaunchBarState 0
    StrCpy $BootRunState 0
    
    # 默认安装盘符 C盘D盘
    ;Push $R0
    ;${DriveSpace} ${PRODUCT_DISK} "/D=F /S=M" $R0
    ;${If} $R0 = null
    ;    StrCpy $INSTDIR "C:\${PRODUCT_DISKPATH}\${PRODUCT_PATH}"
    ;${Else}
    ;    StrCpy $INSTDIR "${PRODUCT_DISK}${PRODUCT_DISKPATH}\${PRODUCT_PATH}"
    ;${EndIf}
    ;Pop $R0
  
    # 上次安装路径 注册表
    Push $R2
    ReadRegStr $R2 "${PRODUCT_ROOT_KEY}" "${PRODUCT_SUB_KEY}" "InstallPath"
    StrCmp $R2 "" +2 +1
        StrCpy $INSTDIR $R2
    Pop $R2
    
    # 上次安装路径 剩余空间更新
    StrCpy $installPath $INSTDIR
    Call UpdateFreeSpace
    
    # 判断操作系统
    Call GetWindowsVersion
    Pop $R0
    StrCmp $R0 "98"   done
    StrCmp $R0 "2000" done
    Goto End
    done:
        MessageBox MB_OK "对不起，${PRODUCT_NAME}目前不支持此系统。"
        Abort
    End:

    # 检查版本
    ReadRegStr $0 ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}" "DisplayVersion"
    Var /Global local_check_version
    ${VersionCompare} "${PRODUCT_VERSION}" "$0" $local_check_version
    
    # 覆盖安装
    ${If} $0 != ""
        # 相同版本
        ${If} $local_check_version == "0"
            StrCmp $local_check_version "0" 0 +2
                StrCpy $InstallState "Cover"
                Goto CHECK_RUN
            StrCmp $local_check_version "0" 0 +4
            MessageBox MB_YESNO "您已经安装当前版本的${PRODUCT_NAME},是否覆盖安装？" IDYES true IDNO false
            true:
                StrCpy $InstallState "Cover"
                Goto CHECK_RUN
            false:
                Quit
        # 安装包版本较低
        ${ElseIf} $local_check_version == "2"
            MessageBox MB_OK|MB_ICONINFORMATION "您已经安装较新版本的${PRODUCT_NAME}，此旧版本无法完成安装。$\r$\n继续安装需先卸载已有版本。"
            Quit
        # 安装包版本较高
        ${Else}
            Goto CHECK_RUN
        ${EndIf}
    ${EndIf}

CHECK_RUN:
    # 检测程序是否在运行
    TCP::FindProc "${PRODUCT_MAIN_EXE}"
    IntCmp $R0 1 check uninst
    exit:
        Quit
    check:
        MessageBox MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2 "${PRODUCT_NAME}正在运行，安装无法进行，请退出程序后重试。$\r$\n点击“确定”立即结束进程，点击“取消”退出。" IDCANCEL exit
        Push "${PRODUCT_MAIN_EXE}"
        Processwork::KillProcess
    uninst:
        ;MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2  
        ;Abort
    
    # 窗口置顶
    FindWindow $0 "#32770" ""
    System::Call "user32::SetWindowPos(i r0, i -1,i 0,i 0,i 0,i 0,i 3)"
    BringToFront
    
    # 释放皮肤资源文件
    SetOutPath $TEMP\${PRODUCT_NAME_EN}Setup
    File "${PRODUCT_SKINPATH}\${PRODUCT_SKINZIP}"
    ;File "${PRODUCT_SKINPATH}\*.png"
    ;File "${PRODUCT_SKINPATH}\*.ico"
    ;File "${PRODUCT_SKINPATH}\*.xml"
    ;File ".\${PRODUCT_SKINPATH}\*.txt"
    
FunctionEnd

# 安装成功
Function .onInstSuccess
    
    # 隐藏窗口
    HideWindow
    
    # 强制立即运行 no
    StrCpy $RunNowState "0"
    
    # 开机启动程序
    StrCmp $BootRunState "1" +1 bootrun
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
        ${if} $R0 >= 6.0  # Vista以上
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} hide"
        ${else}           # XP以下
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} hide"
        ${Endif}
    bootrun:
    
    # 运行程序
    Call RunAfterInstall
    
FunctionEnd

# 全局取消
Function onGlobalCancelFunc
    
    dui::DuiSendMessage $Dialog WM_DUI_CANCEL "${PRODUCT_NAME}" "确定要退出安装？" "确定" "取消"
    Pop $0
    ;MessageBox MB_OK "$0"
    ${If} $0 == "0"
        # 隐藏窗口
        dui::DestroyDuiEngine
        
        # 删目录
        dui::DeleteDirectory "$PLUGINSDIR"
        
        # 退出程序
        dui::ExitDuiEngine
        Abort
    ${EndIf}
    
FunctionEnd

# 上一步 / 返回
Function onBackBtnFunc
    dui::DuiSendMessage $Dialog WM_DUI_BACK
FunctionEnd

# 下一步
Function onNextBtnFunc
    dui::DuiSendMessage $Dialog WM_DUI_NEXT
FunctionEnd
        
# 同意软件协议
Function onAcceptLicenceFunc
    dui::DuiSendMessage $Dialog WM_DUI_ENABLED "inst_Welcome_btnQuickInst"
    dui::DuiSendMessage $Dialog WM_DUI_ENABLED "inst_Welcome_btnNext"
    dui::DuiSendMessage $Dialog WM_DUI_ENABLED "inst_Welcome_btnCover"
FunctionEnd

# 打开许可协议网址
Function onLicenseLinkBtnFunc
    dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_WEB_LICENCE}
    Pop $0
    ${If} $0 == "urlerror"
        MessageBox MB_OK "urlerror"
    ${EndIf}
FunctionEnd

# 打开官方网站
Function onWebSiteLinkBtnFunc
    dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_PUBLISHER_SITE}
    Pop $0
    ${If} $0 == "url error"
        MessageBox MB_OK "url error"
    ${EndIf}
FunctionEnd

# -------------快捷方式操作相关--------------------------------------

# 开始菜单
Function onStartMenuStateFunc
    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Custom_btnStartMenu" ""
    Pop $0
    ${If} $0 == "1"
        StrCpy $StartMenuState "1"
    ${Else}
        StrCpy $StartMenuState "0"
    ${EndIf}
FunctionEnd

# 桌面快捷方式
Function onDesktopIconStateFunc
    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Custom_btnShortCut" ""
    Pop $0
    ${If} $0 == "1"
        StrCpy $DesktopIconState "1"
    ${Else}
        StrCpy $DesktopIconState "0"
    ${EndIf}
FunctionEnd

# 任务栏快捷方式
Function onQuickLaunchBarStateFunc
    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Option_btnQuickLaunchBar" ""
    Pop $1
    ${If} $1 == "1"
        StrCpy $QuickLaunchBarState "1"
    ${Else}
        StrCpy $QuickLaunchBarState "0"
    ${EndIf}
FunctionEnd

# 开机启动程序
Function onBootRunStateFunc

    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Complete_btnBootRun" ""
    Pop $1
    ${If} $1 == "1"
        StrCpy $BootRunState "1"
    ${Else}
        StrCpy $BootRunState "0"
    ${EndIf}
    
FunctionEnd

# -------------安装路径选择相关---------------------------------------
# 安装路径变动
Function onTextChangeFunc

    # 改变可用磁盘空间大小
    dui::GetControlData inst_Option_edtPath "text"
    Pop $0
    ;MessageBox MB_OK $0
    StrCpy $INSTDIR $0

    # 重新获取磁盘空间
    Call UpdateFreeSpace

    # 更新磁盘空间文本显示
    dui::FindControl "inst_Option_lblLastSpace"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_lblLastSpace button"
    ${Else}
        ;dui::SetText2Control "inst_Option_lblLastSpace"  $FreeSpaceSize
        dui::SetControlData "inst_Option_lblLastSpace"  $FreeSpaceSize  "text"
    ${EndIf}
    
    # 路径是否合法（合法则不为0Bytes）
    ${If} $FreeSpaceSize == "0Bytes"
        dui::SetControlData "inst_Option_btnStartInst" "false" "enable"
    ${Else}
        dui::SetControlData "inst_Option_btnStartInst" "true" "enable"
    ${EndIf}
FunctionEnd

# 选择文件夹
Function onInstallPathBrownBtnFunc

    dui::GetControlData "inst_Option_edtPath" "text"
    Pop $1
    dui::SelectFolderDialog "选择文件夹" $1
    Pop $installPath
    ;MessageBox MB_OK $installPath
    ${If} $installPath == "-1"
        StrCpy $installPath $INSTDIR
    ${Else}
        StrCpy $installPath "$installPath\${PRODUCT_PATH}"
    ${EndIf}
    
    StrCpy $0 $installPath 
    
    ${If} $0 == "-1"
    ${Else}
        StrCpy $INSTDIR "$installPath"  ;\${PRODUCT_NAME_EN}"
        # 设置安装路径编辑框文本
        dui::FindControl "inst_Option_edtPath"
        Pop $0
        ${If} $0 == "-1"
            MessageBox MB_OK "Do not have inst_Option_btnOpenPath button"
        ${Else}
            StrCpy $installPath $INSTDIR
            dui::SetControlData "inst_Option_edtPath"  $installPath  "text"
        ${EndIf}
    ${EndIf}

    # 重新获取磁盘空间
    Call UpdateFreeSpace

    # 路径是否合法（合法则不为0Bytes）
    ${If} $FreeSpaceSize == "0Bytes"
        dui::SetControlData "inst_Option_btnStartInst" "false" "enable"
    ${Else}
        dui::SetControlData "inst_Option_btnStartInst" "true" "enable"
    ${EndIf}

    # 更新磁盘空间文本显示
    dui::FindControl "inst_Option_lblLastSpace"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Option_lblLastSpace button"
    ${Else}
        ;dui::SetText2Control "inst_Option_lblLastSpace"  $FreeSpaceSize
        dui::SetControlData "inst_Option_lblLastSpace"  $FreeSpaceSize  "text"
    ${EndIf}
FunctionEnd

# 重新获取磁盘空间
Function UpdateFreeSpace
    ${GetRoot} $INSTDIR $0
    StrCpy $1 "Bytes"

    System::Call kernel32::GetDiskFreeSpaceEx(tr0,*l,*l,*l.r0)
    ${If} $0 > 1024
    ${OrIf} $0 < 0
        System::Int64Op $0 / 1024
        Pop $0
        StrCpy $1 "KB"
        ${If} $0 > 1024
        ${OrIf} $0 < 0
            System::Int64Op $0 / 1024
            Pop $0
            StrCpy $1 "MB"
            ${If} $0 > 1024
            ${OrIf} $0 < 0
                System::Int64Op $0 / 1024
                Pop $0
                StrCpy $1 "GB"
            ${EndIf}
        ${EndIf}
    ${EndIf}
    StrCpy $FreeSpaceSize  "$0$1 )"
FunctionEnd

# --------------安装相关----------------------------------------------
# 快速安装
Function onQuickInstBtnFunc
    
    # 下一步
    Call onNextBtnFunc
    
    # 判断是否联网
    ;${if} $PCOnline == 200  # 在线
    ;    ;dui::SetControlData "inst_Inst_Center" "false" "visible"
    ;    dui::SetControlData "inst_Inst_Center2" "true" "visible"
    ;    dui::ShowWebBrowser "ieWebBrowser" "${PRODUCT_WEB_AD}"     # "网页控件名字" "网址"
    ;${Endif}
    
    # 开始安装代码
    dui::DuiSendMessage $Dialog WM_DUI_STARTINSTALL
    
    ;Call inst_Progress
    
FunctionEnd

# 开始安装
Function onStartInstBtnFunc
    
    # 判断是否联网
    ;${if} $PCOnline == 200  # 在线
    ;    ;dui::SetControlData "inst_Inst_Center" "false" "visible"
    ;    dui::SetControlData "inst_Inst_Center2" "true" "visible"
    ;    dui::ShowWebBrowser "ieWebBrowser" "${PRODUCT_WEB_AD}"     # "网页控件名字" "网址"
    ;${Endif}
    
    # 快速安装代码
    dui::DuiSendMessage $Dialog WM_DUI_STARTINSTALL
    
    ;Call inst_Progress
    
FunctionEnd

# 安装进度显示
Function inst_Progress
    
    # 进度条绑定函数
    dui::FindControl "inst_Inst_Progress"
    Pop $0
    ;MessageBox MB_OK "inst_Progress $0" 
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Inst_Progress button"
    ${Else}
        dui::StartInstall  "inst_Inst_Progress" "inst_Inst_lblPercent"
    ${EndIf}
    
    # 下一步
    ;Call onNextBtnFunc
    
FunctionEnd

/*# 安装进度数字显示
Function instProgressValue

    dui::GetProgressData inst_Inst_Progress "value"
    Pop $0
    StrCpy $R0 $0
    System::Int64Op $R0 / 300
    Pop $0
    StrCpy $R1 $0
    System::Int64Op $R0 % 300
    Pop $0
    StrCpy $R0 $0
    System::Int64Op $R0 / 30
    Pop $0
    StrCpy $InstallValue $R1.$0%
    
    # 进度条显示函数
    dui::FindControl "inst_Inst_lblPercent"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have inst_Inst_lblPercent button"
    ${Else}
        dui::SetControlData "inst_Inst_lblPercent"  $InstallValue  "text"
    ${EndIf}
    
FunctionEnd
*/

# ------------安装完成相关--------------------------------------------
# 立即体验
Function onFinishedRunBtnFunc
    
    # 强制立即运行 yes
    StrCpy $RunNowState "1"
    
    # 开机启动程序
    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Complete_btnBootRun" ""
    Pop $0
    ${If} $0 == "0"
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
        ${if} $R0 >= 6.0  # Vista以上
            ;MessageBox MB_OK "开机启动程序 - $R0 - ${PRODUCT_NAME_EN} - $INSTDIR\${PRODUCT_MAIN_EXE}"
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} -autorun"
        ${else}           # XP以下
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} -autorun"
        ${Endif}
    ${Endif}
    
    # 隐藏窗口
    dui::DestroyDuiEngine
    
    # 运行程序
    Call RunAfterInstall
    
    # 打开网页
    ;dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_WEB_SITE}
    
    # 结束定时器
    ;dui::DuiKillTimer $timerID
    
    # 结束安装
    dui::DuiSendMessage $Dialog WM_DUI_FINISHEDINSTALL
    
FunctionEnd

# 安装完成
Function onFinishedBtnFunc
    
    # 强制立即运行 no
    StrCpy $RunNowState "0"
    
    # 开机启动程序
    dui::DuiSendMessage $Dialog WM_DUI_OPTIONSTATE "inst_Complete_btnBootRun" ""
    Pop $0
    ${If} $0 == "0"
        ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
        ${if} $R0 >= 6.0  # Vista以上
            ;MessageBox MB_OK "开机启动程序 - $R0 - ${PRODUCT_NAME_EN} - $INSTDIR\${PRODUCT_MAIN_EXE}"
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} -autorun"
        ${else}           # XP以下
            ;CreateShortCut "$SMSTARTUP\${PRODUCT_NAME}.lnk" "$INSTDIR\${PRODUCT_MAIN_EXE}" "" "$INSTDIR\${PRODUCT_MAIN_EXE}" 0
            WriteRegStr HKLM  "Software\Microsoft\Windows\CurrentVersion\Run" "${PRODUCT_NAME_EN}"  "$INSTDIR\${PRODUCT_MAIN_EXE} -autorun"
        ${Endif}
    ${Endif}
    
    # 隐藏窗口
    dui::DestroyDuiEngine
    
    # 运行程序
    Call RunAfterInstall
    
    # 打开网页
    ;dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_WEB_SITE}
    
    # 结束定时器
    ;dui::DuiKillTimer $timerID
    
    # 结束安装
    dui::DuiSendMessage $Dialog WM_DUI_FINISHEDINSTALL
    
FunctionEnd

# ------------功能相关--------------------------------------------
# 运行程序
Function RunAfterInstall
    StrCmp $RunNowState "1" "" +2
    Exec '"$INSTDIR\${PRODUCT_MAIN_EXE}"'
FunctionEnd

# 联网检测
Function InternetOnline

    TCP::CheckURL "www.baidu.com"
    Pop $0
    ${if} $0 == 200         # 百度在线
        StrCpy $PCOnline 200
    ${else}                 # 不在线
        StrCpy $PCOnline 0
    ${Endif}
    ;MessageBox MB_OK "The URL returns: $0 $PCOnline"
    
FunctionEnd

# 系统时间
Function GetSystemTime
    # 获取系统时间
    System::Alloc 16
    System::Call "kernel32::GetLocalTime(isR0)"
    System::Call "*$R0(&i2.R1,&i2.R2,&i2.R3,&i2.R4,&i2.R5,&i2.R6,&i2.R7,&i2.R8)"
    System::Free $R0
    StrCpy $SystemTime "$R1-$R2-$R4,$R5:$R6:$R7";.$R8,星期$R3"
    ;MessageBox MB_OK "$R1年$R2月$R4日,星期$R3,$R5:$R6:$R7.$R8"
FunctionEnd

# 系统版本 支持Win10
Function GetWindowsVersion

    Push $R0
    Push $R1
    Push $R2
    
    ClearErrors
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion" ;ProductName
    IfErrors 0 lbl_winnt
    # we are not NT
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows\CurrentVersion" "VersionNumber"
    StrCpy $R1 $R0 1
    StrCmp $R1 '4' 0 lbl_error
    StrCpy $R1 $R0 3
    StrCmp $R1 '4.0' lbl_win32_95
    StrCmp $R1 '4.9' lbl_win32_ME lbl_win32_98
lbl_win32_95:
    StrCpy $R0 '95'
    Goto lbl_done
lbl_win32_98:
    StrCpy $R0 '98'
    Goto lbl_done
lbl_win32_ME:
    StrCpy $R0 'ME'
    Goto lbl_done
lbl_winnt:
    # check if Windows is Client or Server.
    ReadRegStr $R2 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "InstallationType"
  
    StrCpy $R1 $R0 1
    StrCmp $R1 '3' lbl_winnt_x
    StrCmp $R1 '4' lbl_winnt_x
    StrCpy $R1 $R0 3
    StrCmp $R1 '5.0' lbl_winnt_2000
    StrCmp $R1 '5.1' lbl_winnt_XP
    StrCmp $R1 '5.2' lbl_winnt_2003
    StrCmp $R1 '6.0' lbl_winnt_vista_2008
    StrCmp $R1 '6.1' lbl_winnt_7_2008R2
    StrCmp $R1 '6.2' lbl_winnt_8_2012
    StrCmp $R1 '6.3' lbl_winnt_81_2012R2
    StrCmp $R1 '6.4' lbl_winnt_10_2016 ; the early Windows 10 tech previews used version 6.4
    StrCpy $R1 $R0 4
    StrCmp $R1 '10.0' lbl_winnt_10_2016
    Goto lbl_error

  lbl_winnt_x:
    StrCpy $R0 "NT $R0" 6
    Goto lbl_done
  lbl_winnt_2000:
    Strcpy $R0 '2000'
    Goto lbl_done
  lbl_winnt_XP:
    Strcpy $R0 'XP'
    Goto lbl_done
  lbl_winnt_2003:
    Strcpy $R0 '2003'
    Goto lbl_done
  ;----------------- Family - Vista / 2008 -------------
  lbl_winnt_vista_2008:
    StrCmp $R2 'Client' go_vista
    StrCmp $R2 'Server' go_2008
    go_vista:
      Strcpy $R0 'Vista'
      Goto lbl_done
    go_2008:
      Strcpy $R0 'Server2008'
      Goto lbl_done
  ;----------------- Family - 7 / 2008R2 -------------
  lbl_winnt_7_2008R2:
    StrCmp $R2 'Client' go_7
    StrCmp $R2 'Server' go_2008R2
    go_7:
      Strcpy $R0 'Win7'
      Goto lbl_done
    go_2008R2:
      Strcpy $R0 'Server2008R2'
      Goto lbl_done
  ;----------------- Family - 8 / 2012 -------------
  lbl_winnt_8_2012:
    StrCmp $R2 'Client' go_8
    StrCmp $R2 'Server' go_2012
    go_8:
      Strcpy $R0 'Win8'
      Goto lbl_done
    go_2012:
      Strcpy $R0 'Server2012'
      Goto lbl_done
  ;----------------- Family - 8.1 / 2012R2 -------------
  lbl_winnt_81_2012R2:
    StrCmp $R2 'Client' go_81
    StrCmp $R2 'Server' go_2012R2
    go_81:
      Strcpy $R0 'Win8.1'
      Goto lbl_done
    go_2012R2:
      Strcpy $R0 'Server2012R2'
      Goto lbl_done
  ;----------------- Family - 10 / 2016 -------------
  lbl_winnt_10_2016:
    StrCmp $R2 'Client' go_10
    StrCmp $R2 'Server' go_2016
    go_10:
      Strcpy $R0 'Win10.0'
      Goto lbl_done
    go_2016:
      Strcpy $R0 'Server2016'
      Goto lbl_done
  ;-----------------------------------------------------
lbl_error:
    StrCpy $R0 ''
lbl_done:
    Pop $R2
    Pop $R1
    StrCpy $WindowsVersion $R0
    
    GetVersion::WindowsPlatformArchitecture
    Pop $R2
    StrCpy $WindowsVersion "$WindowsVersion-$R2"
    Exch $R0
    
FunctionEnd

# --------------------------------------------------------------------------------------------------------------------

# -----------------------------卸载部分------------------------------------

# 卸载Section
Section "Uninstall" un.Sec01
    
    ;ReadRegStr $LocalPath ${PRODUCT_ROOT_KEY} ${PRODUCT_SUB_KEY} "InstallLocation"
    ;MessageBox MB_OK "$LocalPath"
    
    # 删除注册表
    DeleteRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}"
    DeleteRegKey /ifempty ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}"
    SetRebootFlag false

    # 删除任务栏
    SetShellVarContext current
    ReadRegStr $R0 HKLM "SOFTWARE\Microsoft\Windows NT\CurrentVersion" "CurrentVersion"
    ${if} $R0 >= 6.0   # 删除锁定任务栏
        IfFileExists "$APPDATA\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\${PRODUCT_NAME}.lnk" 0 +3
            ExecShell taskbarunpin "$APPDATA\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
            ;ExecShell taskbarunpin "$DESKTOP\${PRODUCT_NAME}.lnk"
            ExecShell startunpin "$DESKTOP\${PRODUCT_NAME}.lnk"
            Delete "$APPDATA\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"
            Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
    ${else}            # 删除快速任务栏
        IfFileExists "$QUICKLAUNCH\${PRODUCT_NAME}.lnk" 0 +2
            Delete "$QUICKLAUNCH\${PRODUCT_NAME}.lnk";
    ${Endif}

    # 桌面快捷方式
    Delete "$DESKTOP\${PRODUCT_NAME}.lnk"
    Delete "$APPDATA\Microsoft\Internet Explorer\Quick Launch\User Pinned\TaskBar\${PRODUCT_NAME}.lnk"

    # 开始菜单
    Delete "$SMPROGRAMS\${PRODUCT_NAME}.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\${PRODUCT_NAME}.lnk"
    Delete "$SMPROGRAMS\${PRODUCT_NAME}\卸载${PRODUCT_NAME}.lnk"
    RMDir /r "$SMPROGRAMS\${PRODUCT_NAME}"
    SetShellVarContext all
    
    ;MessageBox MB_OK "$INSTDIR"
    # 删安装目录
    SetOutPath "$TEMP"
    Delete "$INSTDIR\*.*"
    Delete "$INSTDIR\*"
    RMDir /r "$INSTDIR\*"
    RMDir /r "$INSTDIR"

    SetRebootFlag false
    SetAutoClose false
    
SectionEnd

# 卸载自定义界面
Function un.uninst_UI
    
    # 如果存在uninst.xml
    IfFileExists "$TEMP\${PRODUCT_NAME_EN}Setup\${PRODUCT_SKINZIP}" +3 +1
        MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name)，及其组件？" IDYES unshowpage IDNO +1
        Quit
    
    # 初始化窗口
    dui::InitDuiEngine /NOUNLOAD "$TEMP\${PRODUCT_NAME_EN}Setup" "${PRODUCT_SKINZIP}" "invalid_string_position" "uninst.xml" "uninst.ico" "uninst_UITab" "${PRODUCT_NAME}" "false" "true"
    ;dui::InitDuiEngine /NOUNLOAD "$TEMP\${PRODUCT_NAME_EN}Setup" "uninst.xml" "uninst.ico" "uninst_UITab" "${PRODUCT_NAME}" "false" "false"
    Pop $Dialog

    # 初始化MessageBox窗口
    dui::InitDuiEngineMsgBox "msgbox.xml" "msgbox_lblTitle" "msgbox_edtText" "msgbox_btnClose" "msgbox_btnYes" "msgbox_btnNo" "false"
    Pop $MessageBoxHandle
    
    # -------------------------------------开始卸载页面------------------------------------
    
    # 取消按钮绑定函数
    dui::FindControl "uninst_Welcome_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Welcome_btnClose button"
    ${Else}
        GetFunctionAddress $0 un.onGlobalCancelFunc
        dui::BindControl "uninst_Welcome_btnClose"  $0
    ${EndIf}
    
    # 软件名称显示设定
    dui::FindControl "uninst_Welcome_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Welcome_lblSoftName button"
    ${Else}
        ;dui::SetControlData "uninst_Welcome_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "uninst_Welcome_lblSoftName" $0
    ${EndIf}
    
    # 开始卸载按钮绑定函数
    dui::FindControl "uninst_Welcome_btnStartUninst"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Welcome_btnStartUninst button"
    ${Else}
        GetFunctionAddress $0 un.onStartUninstallBtnFunc
        dui::BindControl "uninst_Welcome_btnStartUninst"  $0
    ${EndIf}

    # 取消按钮绑定函数
    dui::FindControl "uninst_Welcome_btnCancel"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Welcome_btnCancel button"
    ${Else}
        GetFunctionAddress $0 un.onGlobalCancelFunc
        dui::BindControl "uninst_Welcome_btnCancel"  $0
    ${EndIf}

    # --------------------------------第二个页面-------------------------------------------

    # 软件名称显示设定
    dui::FindControl "uninst_Uninst_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Uninst_lblSoftName button"
    ${Else}
        ;dui::SetControlData "uninst_Uninst_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "uninst_Uninst_lblSoftName" $0
    ${EndIf}
    
    # 取消按钮绑定函数
    dui::FindControl "uninst_Uninst_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Uninst_btnClose button"
    ${Else}
        GetFunctionAddress $0 un.onGlobalCancelFunc
        dui::BindControl "uninst_Uninst_btnClose"  $0
    ${EndIf}
    
    
    /*# 进度条数字显示绑定函数
    dui::FindControl "uninst_Uninst_lblPercent"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Uninst_lblPercent button"
    ${Else}
        StrCpy $InstallValue  "0"
        GetFunctionAddress $timerIDUninstall un.uninstProgressValue
        dui::DuiCreatTimer $timerIDUninstall 500  # callback interval
    ${EndIf}
    */
    
    # --------------------------------卸载完成页面----------------------------------------
    
    # 软件名称显示设定
    dui::FindControl "uninst_Complete_lblSoftName"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Complete_lblSoftName button"
    ${Else}
        ;dui::SetControlData "uninst_Complete_lblSoftName"  ${PRODUCT_NAME} "text"
        dui::BindControl "uninst_Complete_lblSoftName" $0
    ${EndIf}
    
    # 取消按钮绑定函数
    dui::FindControl "uninst_Complete_btnClose"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Complete_btnClose button"
    ${Else}
        GetFunctionAddress $0 un.onUninstallFinishedBtnFunc
        dui::BindControl "uninst_Complete_btnClose"  $0
    ${EndIf}
    
    # 完成安装 按钮绑定函数
    dui::FindControl "uninst_Complete_btnOK"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Complete_btnOK button"
    ${Else}
        GetFunctionAddress $0 un.onUninstallFinishedBtnFunc
        dui::BindControl "uninst_Complete_btnOK"  $0
    ${EndIf}
    
    # ---------------------------------显示------------------------------------------------
    
    dui::ShowPage
    
unshowpage:

FunctionEnd

# 卸载初始化
Function un.onInit
    
    # 360认证要求不能使用静默参数
    IfSilent 0 +2
    SetSilent normal
    
    # 创建临时目录  $PLUGINSDIR
    InitPluginsDir
    ;MessageBox MB_OK "$PLUGINSDIR"
    
    # 检测卸载程序是否运行 创建互斥
    System::Call 'kernel32::CreateMutexA(i 0, i 0, t "${PRODUCT_NAME_MUTEX}Mutex") i .r3 ?e'
    Pop $R0
    StrCmp $R0 0 +3
        MessageBox MB_ICONINFORMATION "${PRODUCT_NAME} 卸载程序已经运行！"
        Abort
    ;MessageBox::show MB_ICONINFORMATION "温馨提示" "" "${PRODUCT_NAME} 卸载程序已经运行！" "关闭"
    
    # 检测联网
    Call un.unInternetOnline
    
    # 卸载检测程序是否在运行
    TCP::FindProc "${PRODUCT_MAIN_EXE}"
    IntCmp $R0 1 check uninst
    exit:
        Quit
    check:
        MessageBox MB_ICONQUESTION|MB_OKCANCEL|MB_DEFBUTTON2 "卸载程序检测到 ${PRODUCT_NAME} 正在运行，请退出程序后重试。$\r$\n点击“确定”立即结束进程，点击“取消”退出。" IDCANCEL exit
        Push "${PRODUCT_MAIN_EXE}"
        Processwork::KillProcess
    uninst:
        ;MessageBox MB_ICONQUESTION|MB_YESNO|MB_DEFBUTTON2 "你确实要完全移除 $(^Name) ，及其所有的组件？" IDYES +2  
        ;Abort
    
    # 窗口置顶
    FindWindow $0 "#32770" ""
    System::Call "user32::SetWindowPos(i r0, i -1,i 0,i 0,i 0,i 0,i 3)"
    BringToFront
    
    # 释放皮肤资源文件
    SetOutPath $TEMP\${PRODUCT_NAME_EN}Setup
    File "${PRODUCT_SKINPATH}\${PRODUCT_SKINZIP}"
    ;File "${PRODUCT_SKINPATH}\*.png"
    ;File "${PRODUCT_SKINPATH}\*.ico"
    ;File "${PRODUCT_SKINPATH}\*.xml"
    ;File ".\${PRODUCT_SKINPATH}\*.txt"
    
FunctionEnd

# 卸载成功
Function un.onUninstSuccess
    
    # 隐藏窗口
    HideWindow

    # 删注册表
    DeleteRegKey ${PRODUCT_ROOT_KEY} "${PRODUCT_SUB_KEY}"
    
    # 打开网页
    dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_WEB_SITE}
    
FunctionEnd

# 卸载全局取消
Function un.onGlobalCancelFunc

    # 隐藏窗口
    dui::DestroyDuiEngine

    # 删目录
    dui::DeleteDirectory "$PLUGINSDIR"
    
    # 退出程序
    dui::ExitDuiEngine
    Abort
    
    /*# 取消卸载时不弹窗确认
    dui::DuiSendMessage $Dialog WM_DUI_CANCEL "${PRODUCT_NAME}" "确定要退出卸载？" "确定" "取消"
    Pop $0
    ${If} $0 == "0
        Abort
        # 退出程序
        dui::ExitDuiEngine
    ${EndIf}
    */
    
FunctionEnd

# 开始卸载
Function un.onStartUninstallBtnFunc
    
    # 判断是否联网
    ;${if} $PCOnline == 200  # 在线
    ;    ;dui::SetControlData "uninst_Uninst_Center" "false" "visible"
    ;    dui::SetControlData "uninst_Uninst_Center2" "true" "visible"
    ;    dui::ShowWebBrowser "ieWebBrowser" "${PRODUCT_WEB_AD}"     # "网页控件名字" "网址"
    ;${Endif}
    
    # 开始卸载
    dui::DuiSendMessage $Dialog WM_DUI_STARTUNINSTALL
    
    # 下一步
    ;Call un.onNextBtnFunc
    
FunctionEnd

# 卸载进度显示
Function un.uninst_Progress
    
    # 如果存在uninst.xml
    IfFileExists "$TEMP\${PRODUCT_NAME_EN}Setup\${PRODUCT_SKINZIP}" +1 unshowpage2
    
    # 进度条绑定函数
    dui::FindControl "uninst_Uninst_Progress"
    Pop $0
    ${If} $0 == "-1"
            MessageBox MB_OK "Do not have uninst_Uninst_Progress button"
    ${Else}
            dui::StartUninstall  "uninst_Uninst_Progress" "uninst_Uninst_lblPercent"
    ${EndIf}
    
unshowpage2:

FunctionEnd

/*# 安装进度数字显示
Function un.uninstProgressValue

    dui::GetProgressData uninst_Uninst_Progress "value"
    Pop $0
    StrCpy $R0 $0
    System::Int64Op $R0 / 300
    Pop $0
    StrCpy $R1 $0
    System::Int64Op $R0 % 300
    Pop $0
    StrCpy $R0 $0
    System::Int64Op $R0 / 30
    Pop $0
    StrCpy $InstallValue $R1.$0%
    
    
    # 进度条显示函数
    dui::FindControl "uninst_Uninst_lblPercent"
    Pop $0
    ${If} $0 == "-1"
        MessageBox MB_OK "Do not have uninst_Uninst_lblPercent button"
    ${Else}
        dui::SetControlData "uninst_Uninst_lblPercent"  $InstallValue  "text"
    ${EndIf}
    
FunctionEnd
*/

# 修复完成/卸载完成
Function un.onUninstallFinishedBtnFunc

    # 隐藏窗口
    dui::DestroyDuiEngine
    
    # 删皮肤目录
    IfFileExists "$TEMP\${PRODUCT_NAME_EN}Setup\${PRODUCT_SKINZIP}" +1 +6
        Delete "$TEMP\${PRODUCT_NAME_EN}Setup\*.*"
        Delete "$TEMP\${PRODUCT_NAME_EN}Setup\*"
        Delete "$TEMP\${PRODUCT_NAME_EN}Setup\"
        RMDir "$TEMP\${PRODUCT_NAME_EN}Setup\"
        RMDir /r "$TEMP\${PRODUCT_NAME_EN}Setup\"
    
    # 打开网页
    ;dui::DuiSendMessage $Dialog WM_DUI_OPENURL ${PRODUCT_WEB_SITE}
    
    # 结束定时器
    ;dui::DuiKillTimer $timerIDUninstall
    
    # 结束安装
    dui::DuiSendMessage $Dialog WM_DUI_FINISHEDUNINSTALL

    # 退出程序
    dui::ExitDuiEngine
    Abort
    
FunctionEnd

# 联网检测
Function un.unInternetOnline

    TCP::CheckURL "www.baidu.com"
    Pop $0
    ${if} $0 == 200         # 百度在线
        StrCpy $PCOnline 200
    ${else}                 # 不在线
        StrCpy $PCOnline 0
    ${Endif}
    ;MessageBox MB_OK "The URL returns: $0 $PCOnline"
    
FunctionEnd


// stdafx.h : 标准系统包含文件的包含文件，
// 或是经常使用但不常更改的
// 特定于项目的包含文件

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // 从 Windows 头中排除极少使用的资料
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // 某些 CString 构造函数将是显式的

// 关闭 MFC 对某些常见但经常可放心忽略的警告消息的隐藏
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC 核心组件和标准组件
#include <afxext.h>         // MFC 扩展


#include <afxdisp.h>        // MFC 自动化类



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC 对 Internet Explorer 4 公共控件的支持
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC 对 Windows 公共控件的支持
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // 功能区和控件条的 MFC 支持









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


//MFC 文件
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//MFC 文件
#include <Nb30.h>
#include <AfxInet.h>

//平台定义
#include "Platform.h"

//网络命令
#include "CMD_Commom.h"
#include "CMD_GameServer.h"
#include "CMD_LogonServer.h"

//进程命令
#include "IPC_GameFrame.h"

//组件文件
#include "ServiceCoreHead.h"
#include "ProcessControlHead.h"

//组件文件
#include "WHImageHead.h"
#include "DownLoadHead.h"
#include "SkinControlHead.h"
#include "UserServiceHead.h"
#include "PlatformDataHead.h"
#include "ShareControlHead.h"
#include "GamePropertyHead.h"
#include "AvatarControlHead.h"
#include "GameEngineHead.h"
#include "PlatformEvent.h"
//////////////////////////////////////////////////////////////////////////////////
//链接代码

//多媒体库
#pragma comment (lib,"Winmm.lib")

#pragma comment (lib,"WHImage.lib")
#pragma comment (lib,"DownLoad.lib")
#pragma comment (lib,"ServiceCore.lib")
#pragma comment (lib,"SkinControl.lib")
#pragma comment (lib,"GameProperty.lib")
#pragma comment (lib,"ShareControl.lib")
#pragma comment (lib,"PlatformData.lib")
#pragma comment (lib,"AvatarControl.lib")
#pragma comment (lib,"GameEngine.lib")


// stdafx.h : ��׼ϵͳ�����ļ��İ����ļ���
// ���Ǿ���ʹ�õ��������ĵ�
// �ض�����Ŀ�İ����ļ�

#pragma once

#ifndef VC_EXTRALEAN
#define VC_EXTRALEAN            // �� Windows ͷ���ų�����ʹ�õ�����
#endif

#include "targetver.h"

#define _ATL_CSTRING_EXPLICIT_CONSTRUCTORS      // ĳЩ CString ���캯��������ʽ��

// �ر� MFC ��ĳЩ�����������ɷ��ĺ��Եľ�����Ϣ������
#define _AFX_ALL_WARNINGS

#include <afxwin.h>         // MFC ��������ͱ�׼���
#include <afxext.h>         // MFC ��չ


#include <afxdisp.h>        // MFC �Զ�����



#ifndef _AFX_NO_OLE_SUPPORT
#include <afxdtctl.h>           // MFC �� Internet Explorer 4 �����ؼ���֧��
#endif
#ifndef _AFX_NO_AFXCMN_SUPPORT
#include <afxcmn.h>             // MFC �� Windows �����ؼ���֧��
#endif // _AFX_NO_AFXCMN_SUPPORT

#include <afxcontrolbars.h>     // �������Ϳؼ����� MFC ֧��









#ifdef _UNICODE
#if defined _M_IX86
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='x86' publicKeyToken='6595b64144ccf1df' language='*'\"")
#elif defined _M_X64
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='amd64' publicKeyToken='6595b64144ccf1df' language='*'\"")
#else
#pragma comment(linker,"/manifestdependency:\"type='win32' name='Microsoft.Windows.Common-Controls' version='6.0.0.0' processorArchitecture='*' publicKeyToken='6595b64144ccf1df' language='*'\"")
#endif
#endif


//MFC �ļ�
#include <AfxWin.h>
#include <AfxExt.h>
#include <AfxCmn.h>
#include <AfxDisp.h>

//MFC �ļ�
#include <Nb30.h>
#include <AfxInet.h>

//ƽ̨����
#include "Platform.h"

//��������
#include "CMD_Commom.h"
#include "CMD_GameServer.h"
#include "CMD_LogonServer.h"

//��������
#include "IPC_GameFrame.h"

//����ļ�
#include "ServiceCoreHead.h"
#include "ProcessControlHead.h"

//����ļ�
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
//���Ӵ���

//��ý���
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

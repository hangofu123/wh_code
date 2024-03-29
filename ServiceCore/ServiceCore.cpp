// ServiceCore.cpp : 定义 DLL 的初始化例程。
//

#include "stdafx.h"
#include "ServiceCoreHead.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//静态变量
static AFX_EXTENSION_MODULE ServiceCoreDLL = { NULL,NULL };

//////////////////////////////////////////////////////////////////////////////////

//导出函数
extern "C" int APIENTRY DllMain(HINSTANCE hInstance, DWORD dwReason, LPVOID lpReserved)
{
	if (dwReason == DLL_PROCESS_ATTACH)
	{
		if (!AfxInitExtensionModule(ServiceCoreDLL, hInstance)) return 0;
		new CDynLinkLibrary(ServiceCoreDLL);
	}
	else if (dwReason == DLL_PROCESS_DETACH)
	{
		AfxTermExtensionModule(ServiceCoreDLL);
	}

	return 1;
}

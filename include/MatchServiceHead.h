#ifndef MATCH_SERVICE_HEAD_HEAD_FILE
#define MATCH_SERVICE_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////

//平台定义
#include "Platform.h"

//消息定义
#include "CMD_Commom.h"
#include "CMD_Correspond.h"
#include "CMD_GameServer.h"

//平台文件
#include "ServiceCoreHead.h"
#include "GameServiceHead.h"
#include "KernelEngineHead.h"

//////////////////////////////////////////////////////////////////////////////////
//导出定义

//导出定义
#ifndef MATCH_SERVICE_CLASS
	#ifdef  MATCH_SERVICE_DLL
		#define MATCH_SERVICE_CLASS _declspec(dllexport)
	#else
		#define MATCH_SERVICE_CLASS _declspec(dllimport)
	#endif
#endif

//模块定义
#ifndef _DEBUG
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//组件名字
#else
	#define MATCH_SERVICE_DLL_NAME	TEXT("MatchService.dll")			//组件名字

#endif

//////////////////////////////////////////////////////////////////////////////////

//游戏服务
DECLARE_MODULE_HELPER(GameMatchServiceManager,MATCH_SERVICE_DLL_NAME,"CreateGameMatchServiceManager")

//////////////////////////////////////////////////////////////////////////////////

#endif
#ifndef PLATFORM_DATA_HEAD_HEAD_FILE
#define PLATFORM_DATA_HEAD_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//ƽ̨�ļ�

//ƽ̨�ļ�
#include "Platform.h"

//��ͷ�ļ�
#include "WHImageHead.h"
#include "WHSocketHead.h"
#include "SkinControlHead.h"
#include "AvatarControlHead.h"
#include "ServiceCoreHead.h"

//////////////////////////////////////////////////////////////////////////////////
//��������

//��������
#ifndef PLATFORM_DATA_CLASS
	#ifdef  PLATFORM_DATA_DLL
		#define PLATFORM_DATA_CLASS _declspec(dllexport)
	#else
		#define PLATFORM_DATA_CLASS _declspec(dllimport)
	#endif
#endif

//ģ�鶨��
#ifndef _DEBUG
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformData.dll")			//��� DLL ����
#else
	#define PLATFORM_DATA_DLL_NAME	TEXT("PlatformData.dll")			//��� DLL ����
#endif

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

#ifndef PLATFORM_DATA_DLL
	#include "GlobalServer.h"
	#include "GlobalType.h" 
	#include "GlobalUserInfo.h"
	#include "OptionParameter.h"
#endif

//////////////////////////////////////////////////////////////////////////////////

#endif
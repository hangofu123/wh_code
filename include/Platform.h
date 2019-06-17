#ifndef PLATFORM_HEAD_FILE
#define PLATFORM_HEAD_FILE

//////////////////////////////////////////////////////////////////////////////////
//�����ļ�

//�����ļ�
#include "Macro.h"
#include "Define.h"

//�ṹ�ļ�
#include "Struct.h"
#include "Packet.h"
#include "Property.h"

//ģ���ļ�
#include "Array.h"
#include "Module.h"
#include "PacketAide.h"
#include "ServerRule.h"
#include "RightDefine.h"

//////////////////////////////////////////////////////////////////////////////////

//����汾
#define VERSION_FRAME				PROCESS_VERSION(6,0,3)				//��ܰ汾
#define VERSION_PLAZA				PROCESS_VERSION(9,0,3)				//�����汾
#define VERSION_MOBILE_ANDROID		PROCESS_VERSION(6,0,3)				//�ֻ��汾
#define VERSION_MOBILE_IOS			PROCESS_VERSION(6,0,3)				//�ֻ��汾

//�汾����
#define VERSION_EFFICACY			0									//Ч��汾
#define VERSION_FRAME_SDK			INTERFACE_VERSION(6,3)				//��ܰ汾

//////////////////////////////////////////////////////////////////////////////////
//�����汾

#ifndef _DEBUG

//ƽ̨����
const TCHAR	szProduct[]=TEXT("��������");							//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("AQPGamePlaza");					//�㳡����
const TCHAR szProductKey[]=TEXT("AQPGamePlatform");					//��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://127.0.0.1");						//��¼��ַ
const TCHAR szLogonServer[]=TEXT("http://127.0.0.1");						//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://127.0.0.1");				//ƽ̨��վ

#else

//////////////////////////////////////////////////////////////////////////////////
//�ڲ�汾

//ƽ̨����
const TCHAR	szProduct[]=TEXT("��������");								//��Ʒ����
const TCHAR szPlazaClass[]=TEXT("WHQPGamePlaza");					    //�㳡����
const TCHAR szProductKey[]=TEXT("WHQPGamePlatform");				    //��Ʒ����

//��ַ����
const TCHAR szCookieUrl[]=TEXT("http://127.0.0.1");			//��¼��ַ
const TCHAR szLogonServer[]=TEXT("http://127.0.0.1");					//��¼��ַ
const TCHAR szPlatformLink[]=TEXT("http://127.0.0.1");		//ƽ̨��վ

#endif

//////////////////////////////////////////////////////////////////////////////////

//���ݿ���
const TCHAR szPlatformDB[]=TEXT("QPPlatformDB");						//ƽ̨���ݿ�
const TCHAR szAccountsDB[]=TEXT("QPAccountsDB");						//�û����ݿ�
const TCHAR	szTreasureDB[]=TEXT("QPTreasureDB");						//�Ƹ����ݿ�
const TCHAR	szExerciseDB[]=TEXT("QPExerciseDB");						//��ϰ���ݿ�

//////////////////////////////////////////////////////////////////////////////////

//��Ȩ��Ϣ
const TCHAR szCompilation[]=TEXT("9E0F66DC-C919-4413-8C83-8759EF60E563");

//////////////////////////////////////////////////////////////////////////////////

#endif
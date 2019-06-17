#ifndef GLOBAL_TYPE_HEAD_FILE
#define GLOBAL_TYPE_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////////////

//��ַ��Ϣ
struct tagTypeItem
{
	WORD							wTypePort;						//����˿�
	DWORD							dwTypeAddr;						//�����ַ
	TCHAR							szTypeHost[LEN_TYPE];			//������Ϣ
};

//////////////////////////////////////////////////////////////////////////////////
#define LEN_TYPE					32									//���䳤��					
//ȫ�ַ�����
class PLATFORM_DATA_CLASS CGlobalType
{
	//��������
public:
	//���캯��
	CGlobalType();
	//��������
	virtual ~CGlobalType();

	//ȫ����Ϣ
public:
	//������Ŀ
	WORD GetItemCount();

	//�ϴε�¼
public:
	//�ϴε�¼
	VOID SetLastTypeName(LPCTSTR pszLastType);
	//�ϴε�¼
	VOID GetLastTypeName(TCHAR szLastType[LEN_TYPE]);

	//������Ϣ
public:
	//��������
	VOID GetTypeItemName(WORD wIndex, TCHAR szResult[LEN_TYPE]);
	//������Ϣ
	bool GetTypeItemInfo(LPCTSTR pszTypeName, tagTypeItem & TypeItem);

	//д�뺯��
protected:
	//��ȡ��ֵ
	VOID WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue);
	//д������
	VOID WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString);

	//��ȡ����
protected:
	//��ȡ��ֵ
	UINT ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//��ȡ����
	VOID ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
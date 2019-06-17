#ifndef GLOBAL_TYPE_HEAD_FILE
#define GLOBAL_TYPE_HEAD_FILE

#pragma once

#include "PlatformDataHead.h"

//////////////////////////////////////////////////////////////////////////////////

//地址信息
struct tagTypeItem
{
	WORD							wTypePort;						//服务端口
	DWORD							dwTypeAddr;						//服务地址
	TCHAR							szTypeHost[LEN_TYPE];			//域名信息
};

//////////////////////////////////////////////////////////////////////////////////
#define LEN_TYPE					32									//房间长度					
//全局服务器
class PLATFORM_DATA_CLASS CGlobalType
{
	//函数定义
public:
	//构造函数
	CGlobalType();
	//析构函数
	virtual ~CGlobalType();

	//全局信息
public:
	//子项数目
	WORD GetItemCount();

	//上次登录
public:
	//上次登录
	VOID SetLastTypeName(LPCTSTR pszLastType);
	//上次登录
	VOID GetLastTypeName(TCHAR szLastType[LEN_TYPE]);

	//子项信息
public:
	//子项名字
	VOID GetTypeItemName(WORD wIndex, TCHAR szResult[LEN_TYPE]);
	//子项信息
	bool GetTypeItemInfo(LPCTSTR pszTypeName, tagTypeItem & TypeItem);

	//写入函数
protected:
	//读取数值
	VOID WriteFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nValue);
	//写入数据
	VOID WriteFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, LPCTSTR pszString);

	//读取函数
protected:
	//读取数值
	UINT ReadFileValue(LPCTSTR pszKeyName, LPCTSTR pszItemName, UINT nDefault);
	//读取数据
	VOID ReadFileString(LPCTSTR pszKeyName, LPCTSTR pszItemName, TCHAR szResult[], WORD wMaxCount);
};

//////////////////////////////////////////////////////////////////////////////////

#endif
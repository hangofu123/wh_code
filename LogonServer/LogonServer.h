
// LogonServer.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CLogonServerApp: 
// �йش����ʵ�֣������ LogonServer.cpp
//

class CLogonServerApp : public CWinApp
{
public:
	CLogonServerApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CLogonServerApp theApp;
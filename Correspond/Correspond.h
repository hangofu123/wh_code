
// Correspond.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CCorrespondApp: 
// �йش����ʵ�֣������ Correspond.cpp
//

class CCorrespondApp : public CWinApp
{
public:
	CCorrespondApp();

// ��д
public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CCorrespondApp theApp;
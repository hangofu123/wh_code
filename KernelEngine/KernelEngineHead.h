// KernelEngine.h : KernelEngine DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CKernelEngineApp
// �йش���ʵ�ֵ���Ϣ������� KernelEngine.cpp
//

class CKernelEngineApp : public CWinApp
{
public:
	CKernelEngineApp();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};

// testforH264dll.h : PROJECT_NAME Ӧ�ó������ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CtestforH264dllApp:
// �йش����ʵ�֣������ testforH264dll.cpp
//

class CtestforH264dllApp : public CWinApp
{
public:
	CtestforH264dllApp();

// ��д
	public:
	virtual BOOL InitInstance();

// ʵ��

	DECLARE_MESSAGE_MAP()
};

extern CtestforH264dllApp theApp;
// H264.h : H264 DLL ����ͷ�ļ�
//

#pragma once

#ifndef __AFXWIN_H__
	#error "�ڰ������ļ�֮ǰ������stdafx.h�������� PCH �ļ�"
#endif

#include "resource.h"		// ������


// CH264App
// �йش���ʵ�ֵ���Ϣ������� H264.cpp
//

class CH264App : public CWinApp
{
public:
	CH264App();

// ��д
public:
	virtual BOOL InitInstance();

	DECLARE_MESSAGE_MAP()
};


// DICOM.h : PROJECT_NAME ���� ���α׷��� ���� �� ��� �����Դϴ�.
//

#pragma once

#ifndef __AFXWIN_H__
	#error "PCH�� ���� �� ������ �����ϱ� ���� 'stdafx.h'�� �����մϴ�."
#endif

#include "resource.h"		// �� ��ȣ�Դϴ�.


// CDICOMApp:
// �� Ŭ������ ������ ���ؼ��� DICOM.cpp�� �����Ͻʽÿ�.
//

class CDICOMApp : public CWinApp
{
public:
	CDICOMApp();

// �������Դϴ�.
public:
	virtual BOOL InitInstance();

// �����Դϴ�.

	DECLARE_MESSAGE_MAP()
};

extern CDICOMApp theApp;
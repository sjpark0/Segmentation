﻿
// MaskRCNN_MFC.h: MaskRCNN_MFC 응용 프로그램의 기본 헤더 파일
//
#pragma once

#ifndef __AFXWIN_H__
	#error "include 'pch.h' before including this file for PCH"
#endif

#include "resource.h"       // 주 기호입니다.


// CMaskRCNNMFCApp:
// 이 클래스의 구현에 대해서는 MaskRCNN_MFC.cpp을(를) 참조하세요.
//

class CMaskRCNNMFCApp : public CWinApp
{
public:
	CMaskRCNNMFCApp() noexcept;


// 재정의입니다.
public:
	virtual BOOL InitInstance();
	virtual int ExitInstance();

// 구현입니다.

public:
	afx_msg void OnAppAbout();
	DECLARE_MESSAGE_MAP()
};

extern CMaskRCNNMFCApp theApp;

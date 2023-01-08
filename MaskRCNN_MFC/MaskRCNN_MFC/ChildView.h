
// ChildView.h: CChildView 클래스의 인터페이스
//


#pragma once


// CChildView 창
#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc.hpp"
#include "opencv2/dnn.hpp"

using namespace cv;
using namespace cv::dnn;
using namespace std;

class CChildView : public CWnd
{
private:
	vector<Mat> m_vecOuts;
	
	bool m_bIsSegment;
	bool m_bIscomposite;
	unsigned char *m_pSegmentImage;
	unsigned char *m_pCompositeImage;
	int  m_iSegmentWidth;
	int  m_iSegmentHeight;
	int  m_iCompositeWidth;
	int  m_iCompositeHeight;
	
	int  m_iScreenSegWidth;
	int  m_iScreenSegHeight;
	int  m_iScreenCompWidth;
	int  m_iScreenCompHeight;
	BITMAPINFO    *m_pBitmapInfoSeg;
	BITMAPINFO    *m_pBitmapInfoComp;
// 생성입니다.
public:
	CChildView();

// 특성입니다.
public:

// 작업입니다.
public:

// 재정의입니다.
	protected:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);

// 구현입니다.
public:
	virtual ~CChildView();

	// 생성된 메시지 맵 함수
protected:
	afx_msg void OnPaint();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg BOOL OnEraseBkgnd(CDC* pDC);
	int SetSegmentFile(CString filename);
	int SetCompositeFile(CString filename);
	int DeallocationAll();
	afx_msg void OnLButtonDown(UINT nFlags, CPoint point);
};


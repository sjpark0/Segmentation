
// ChildView.cpp: CChildView 클래스의 구현
//

#include "pch.h"
#include "framework.h"
#include "MaskRCNN_MFC.h"
#include "ChildView.h"



#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CChildView

CChildView::CChildView()
{
	m_pSegmentImage = NULL;
	m_pCompositeImage = NULL;
	m_pBitmapInfoSeg = NULL;
	m_pBitmapInfoComp = NULL;
	m_bIsSegment = false;
	m_bIscomposite = false;
}

CChildView::~CChildView()
{
	DeallocationAll();
}


BEGIN_MESSAGE_MAP(CChildView, CWnd)
	ON_WM_PAINT()
	ON_WM_ERASEBKGND()
	ON_WM_LBUTTONDOWN()
END_MESSAGE_MAP()



// CChildView 메시지 처리기

BOOL CChildView::PreCreateWindow(CREATESTRUCT& cs) 
{
	if (!CWnd::PreCreateWindow(cs))
		return FALSE;

	cs.dwExStyle |= WS_EX_CLIENTEDGE;
	cs.style &= ~WS_BORDER;
	cs.lpszClass = AfxRegisterWndClass(CS_HREDRAW|CS_VREDRAW|CS_DBLCLKS, 
		::LoadCursor(nullptr, IDC_ARROW), reinterpret_cast<HBRUSH>(COLOR_WINDOW+1), nullptr);

	return TRUE;
}

void CChildView::OnPaint() 
{
	CPaintDC dc(this); // 그리기를 위한 디바이스 컨텍스트입니다.
	
	
	if (!m_bIscomposite && !m_bIsSegment) return;
	
	CDC *thisDC = this->GetDC();
	CDC memDC;
	memDC.CreateCompatibleDC(thisDC);
	CRect clientRect;
	GetClientRect(clientRect);
	CBitmap bitmap;
	bitmap.CreateCompatibleBitmap(thisDC, clientRect.Width(), clientRect.Height());
	memDC.SelectObject(bitmap);
	SetStretchBltMode(memDC.m_hDC, COLORONCOLOR);

	if (m_bIsSegment) {
		m_iScreenSegWidth = clientRect.Width() / 2;
		m_iScreenSegHeight = m_iScreenSegWidth * m_iSegmentHeight / m_iSegmentWidth;
		if (m_iScreenSegHeight > clientRect.Height()) {
			m_iScreenSegHeight = clientRect.Height();
			m_iScreenSegWidth = m_iScreenSegHeight * m_iSegmentWidth / m_iSegmentHeight;
		
		}
		StretchDIBits(memDC.m_hDC, 0, 0, m_iScreenSegWidth, m_iScreenSegHeight, 0, 0, m_iSegmentWidth, m_iSegmentHeight, m_pSegmentImage, m_pBitmapInfoSeg, DIB_RGB_COLORS, SRCCOPY);

	}
	if (m_bIscomposite) {
		m_iScreenCompWidth = clientRect.Width() / 2;
		m_iScreenCompHeight = m_iScreenCompWidth * m_iCompositeHeight / m_iCompositeWidth;
		if (m_iScreenCompHeight > clientRect.Height()) {
			m_iScreenCompHeight = clientRect.Height();
			m_iScreenCompWidth = m_iScreenCompHeight * m_iCompositeWidth / m_iCompositeHeight;

		}
		StretchDIBits(memDC.m_hDC, clientRect.Width() / 2, 0, m_iScreenCompWidth, m_iScreenCompHeight, 0, 0, m_iCompositeWidth, m_iCompositeHeight, m_pCompositeImage, m_pBitmapInfoComp, DIB_RGB_COLORS, SRCCOPY);

	}
	thisDC->BitBlt(0, 0, clientRect.Width(), clientRect.Height(), &memDC, 0, 0, SRCCOPY);

	memDC.DeleteDC();
	bitmap.DeleteObject();
	// TODO: 여기에 메시지 처리기 코드를 추가합니다.
	
	// 그리기 메시지에 대해서는 CWnd::OnPaint()를 호출하지 마십시오.
}



BOOL CChildView::OnEraseBkgnd(CDC* pDC)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.

	return NULL;
}


int CChildView::SetSegmentFile(CString filename)
{
	CT2CA temp(filename);
	Mat img = imread(temp.m_psz);
	m_iSegmentWidth = img.cols;
	m_iSegmentHeight = img.rows;
	
	m_bIsSegment = true;
	// TODO: 여기에 구현 코드 추가.
	if (m_pBitmapInfoSeg) {
		delete[]m_pBitmapInfoSeg;
	}
	if (m_pSegmentImage) {
		delete[]m_pSegmentImage;
	}
	m_pBitmapInfoSeg = (BITMAPINFO*)new char[sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256];
	m_pBitmapInfoSeg->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfoSeg->bmiHeader.biWidth = img.cols;
	m_pBitmapInfoSeg->bmiHeader.biHeight = -(int)img.rows;  // if negative, image is mirrored along y-axis
	m_pBitmapInfoSeg->bmiHeader.biPlanes = 1;
	m_pBitmapInfoSeg->bmiHeader.biBitCount = img.channels() * 8;
	m_pBitmapInfoSeg->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfoSeg->bmiHeader.biSizeImage = 0;
	m_pBitmapInfoSeg->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfoSeg->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfoSeg->bmiHeader.biClrUsed = 256;
	m_pBitmapInfoSeg->bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 256; i++) {
		m_pBitmapInfoSeg->bmiColors[i].rgbBlue = i;
		m_pBitmapInfoSeg->bmiColors[i].rgbGreen = i;
		m_pBitmapInfoSeg->bmiColors[i].rgbRed = i;
		m_pBitmapInfoSeg->bmiColors[i].rgbReserved = 0;
	}
	
	m_pSegmentImage = new unsigned char[img.rows * img.cols * img.channels()];
	memcpy(m_pSegmentImage, img.data, img.rows * img.cols * img.channels());

	String textGraph = "..\\model\\mask_rcnn_inception_v2_coco_2018_01_28.pbtxt";
	String modelWeights = "..\\model\\frozen_inference_graph.pb";

	Net net = readNetFromTensorflow(modelWeights, textGraph);
	net.setPreferableBackend(DNN_BACKEND_OPENCV);
	net.setPreferableTarget(DNN_TARGET_CPU);

	Mat blobImg;
	blobImg = blobFromImage(img, 1.0, Size(img.cols, img.rows), Scalar(0, 0, 0), false, false, CV_32F);

	net.setInput(blobImg);
	std::vector<String> outNames(2);
	outNames[0] = "detection_out_final";
	outNames[1] = "detection_masks";
	
	net.forward(m_vecOuts, outNames);
	
	Invalidate();
	return 0;
}


int CChildView::SetCompositeFile(CString filename)
{
	CT2CA temp(filename);
	Mat img = imread(temp.m_psz);
	m_bIscomposite = true;
	m_iCompositeWidth = img.cols;
	m_iCompositeHeight = img.rows;
	// TODO: 여기에 구현 코드 추가.
	if (m_pBitmapInfoComp) {
		delete[]m_pBitmapInfoComp;
	}
	if (m_pCompositeImage) {
		delete[]m_pCompositeImage;
	}
	// TODO: 여기에 구현 코드 추가.
	m_pBitmapInfoComp = (BITMAPINFO*)new char[sizeof(BITMAPINFO) + sizeof(RGBQUAD) * 256];
	m_pBitmapInfoComp->bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	m_pBitmapInfoComp->bmiHeader.biWidth = img.cols;
	m_pBitmapInfoComp->bmiHeader.biHeight = -(int)img.rows;  // if negative, image is mirrored along y-axis
	m_pBitmapInfoComp->bmiHeader.biPlanes = 1;
	m_pBitmapInfoComp->bmiHeader.biBitCount = img.channels() * 8;
	m_pBitmapInfoComp->bmiHeader.biCompression = BI_RGB;
	m_pBitmapInfoComp->bmiHeader.biSizeImage = 0;
	m_pBitmapInfoComp->bmiHeader.biXPelsPerMeter = 0;
	m_pBitmapInfoComp->bmiHeader.biYPelsPerMeter = 0;
	m_pBitmapInfoComp->bmiHeader.biClrUsed = 256;
	m_pBitmapInfoComp->bmiHeader.biClrImportant = 0;

	for (int i = 0; i < 256; i++) {
		m_pBitmapInfoComp->bmiColors[i].rgbBlue = i;
		m_pBitmapInfoComp->bmiColors[i].rgbGreen = i;
		m_pBitmapInfoComp->bmiColors[i].rgbRed = i;
		m_pBitmapInfoComp->bmiColors[i].rgbReserved = 0;
	}
	m_pCompositeImage = new unsigned char[img.rows * img.cols * img.channels()];
	memcpy(m_pCompositeImage, img.data, img.rows * img.cols * img.channels());
	Invalidate();

	return 0;
}


int CChildView::DeallocationAll()
{
	// TODO: 여기에 구현 코드 추가.
	if (m_pSegmentImage) {
		delete[]m_pSegmentImage;
		m_pSegmentImage = NULL;
	}
	if (m_pCompositeImage) {
		delete[]m_pCompositeImage;
		m_pCompositeImage = NULL;
	}
	if (m_pBitmapInfoComp) {
		delete[]m_pBitmapInfoComp;
		m_pBitmapInfoComp = NULL;
	}
	if (m_pBitmapInfoSeg) {
		delete[]m_pBitmapInfoSeg;
		m_pBitmapInfoSeg = NULL;
	}
	m_bIsSegment = false;
	m_bIscomposite = false;
	return 0;
}


void CChildView::OnLButtonDown(UINT nFlags, CPoint point)
{
	// TODO: 여기에 메시지 처리기 코드를 추가 및/또는 기본값을 호출합니다.
	if (m_bIsSegment && point.x < m_iScreenSegWidth) {
		Mat outDetections = m_vecOuts[0];
		Mat outMasks = m_vecOuts[1];

		const int numDetections = outDetections.size[2];
		const int numClasses = outMasks.size[1];

		outDetections = outDetections.reshape(1, outDetections.total() / 7);
		for (int i = 0; i < numDetections; ++i)
		{
			float score = outDetections.at<float>(i, 2);
			if (score > 0.5)
			{
				// Extract the bounding box
				int classId = static_cast<int>(outDetections.at<float>(i, 1));
				int left = static_cast<int>(m_iSegmentWidth * outDetections.at<float>(i, 3));
				int top = static_cast<int>(m_iSegmentHeight * outDetections.at<float>(i, 4));
				int right = static_cast<int>(m_iSegmentWidth * outDetections.at<float>(i, 5));
				int bottom = static_cast<int>(m_iSegmentHeight * outDetections.at<float>(i, 6));

				if (classId != 0) continue;

				left = max(0, min(left, m_iSegmentWidth - 1));
				top = max(0, min(top, m_iSegmentHeight - 1));
				right = max(0, min(right, m_iSegmentWidth - 1));
				bottom = max(0, min(bottom, m_iSegmentHeight - 1));
				//printf("%d, %d, %d, %d, %d\n", left, top, right, bottom, classId);
				if (point.x * m_iSegmentWidth / m_iScreenSegWidth > left && point.x * m_iSegmentWidth / m_iScreenSegWidth < right && point.y * m_iSegmentHeight / m_iScreenSegHeight > top && point.y * m_iSegmentHeight / m_iScreenSegHeight < bottom) {
					Rect box = Rect(left, top, right - left + 1, bottom - top + 1);
					// Extract the mask for the object
					Mat objectMask(outMasks.size[2], outMasks.size[3], CV_32F, outMasks.ptr<float>(i, classId));
					resize(objectMask, objectMask, Size(box.width, box.height));
					Mat mask = (objectMask > 0.7);
					mask.convertTo(mask, CV_8U);
					Mat img(m_iSegmentHeight, m_iSegmentWidth, CV_8UC3, m_pSegmentImage);
					Scalar color = Scalar(255, 0, 0);
					Mat coloredRoi = (0.99 * color + 0.01 * img(box));
					coloredRoi.convertTo(coloredRoi, CV_8UC3);
					coloredRoi.copyTo(img(box), mask);

					Invalidate();
				}
				
				//resize(objectMask, objectMask, Size(box.width, box.height));


				// Draw bounding box, colorize and show the mask on the image
				
			}
		}
	//	printf("%d, %d\n", point.x * m_iSegmentWidth / m_iScreenSegWidth, point.y * m_iSegmentHeight / m_iScreenSegHeight);

	}
	else if(m_bIscomposite && point.x >= m_iScreenSegWidth){
	//	printf("%d, %d\n", (point.x - m_iScreenSegWidth) * m_iCompositeWidth / m_iScreenCompWidth, point.y * m_iCompositeHeight / m_iScreenCompHeight);

	}
	CWnd::OnLButtonDown(nFlags, point);
}

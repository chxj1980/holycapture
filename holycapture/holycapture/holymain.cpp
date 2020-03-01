#include <iostream>
#include <Windows.h>
#include <MultiMux.h>
#include <filenameio.h>
#include <opencv2/opencv.hpp>
#include <DXGIManager.hpp>
#include "iutimer.h"

#ifdef _WIN32
#pragma comment(lib, "dxgiwrapper.lib")
#pragma comment(lib, "ffencoder.lib")
#ifndef _DEBUG
#pragma comment(lib, "opencv_world400.lib")
#else
#pragma comment(lib, "opencv_world400d.lib")
#endif
#endif

typedef void*(*lpFun)(void);

using namespace std;
using namespace cv;

int main()
{
	CMultiMux* pMux = createMultiMux();
	pMux->LoadConfig("decodeconfig.ini");
	auto pManager = std::make_shared<DXGIManager>();
	pManager->setup();
	RECT rect = pManager->get_output_rect();
	BYTE * pRgb = (BYTE*)malloc((rect.right - rect.left) * 4 * (rect.bottom - rect.top));
	size_t pSize = 0;

	Mat mat;
	Mat matRGB;
	mat = cv::Mat::zeros(rect.bottom - rect.top, rect.right - rect.left, CV_8UC4);
	int inx = 30000;
	while (1)
	{
		double time = 0;
		double counts = 0;
		LARGE_INTEGER nFreq;
		LARGE_INTEGER nBeginTime;
		LARGE_INTEGER nEndTime;
		QueryPerformanceFrequency(&nFreq);
		QueryPerformanceCounter(&nBeginTime);//��ʼ��ʱ

		pManager->get_output_data(&pRgb, &pSize);
		mat.data = pRgb;
		//cvtColor(mat, matRGB, COLOR_RGBA2BGR);
		cvtColor(mat, matRGB, COLOR_RGBA2RGB);
		pMux->PushStreamData(0, matRGB.data);
		inx--;
		QueryPerformanceCounter(&nEndTime);//ֹͣ��ʱ

		time = (double)(nEndTime.QuadPart - nBeginTime.QuadPart) / (double)nFreq.QuadPart;//�������ִ��ʱ�䵥λΪs
		cout << "����ִ��ʱ�䣺" << time * 1000 << "ms" << endl;

		if (time * 1000 < 40)
		{
			std::chrono::milliseconds dura((int)(40 - (time * 1000)));
			std::this_thread::sleep_for(dura);
		}

		inx--;
		if (inx <= 0)
			break;
	}
	delete pMux;
	pMux = NULL;
	return 0;
}
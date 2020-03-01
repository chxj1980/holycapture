#ifndef __MULTI_MUX_H__
#define __MULTI_MUX_H__

#include <map>

#ifndef _WIN32
#include <pthread.h>
#else
#include <thread>
#include <mutex>
#endif

//#define USE_OPENCV

#ifdef USE_OPENCV
#include <opencv2/opencv.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#endif

#include <iostream>

//#define STATICX

#ifndef STATICX
#ifdef _WIN32
#define EXPORT_DLL __declspec(dllexport)
#define IMPORT_DLL __declspec(dllimport)
#define EXPORT_DLLA extern"C" __declspec(dllexport)
#define IMPORT_DLLA extern"C" __declspec(dllimport)
#else
#define EXPORT_DLL
#define IMPORT_DLL
#define EXPORT_DLLA
#define IMPORT_DLLA
#endif
#else
#ifdef _WIN32
#define EXPORT_DLLA extern"C" __declspec(dllexport)
#define IMPORT_DLLA extern"C" __declspec(dllimport)
#else
#define EXPORT_DLLA
#define IMPORT_DLLA
#endif
#endif

class CImageMuxing;

typedef struct _rtmpconfig
{
	std::string strUrl;
	std::string strFmt;
	int         nFrameRate;

	_rtmpconfig(){
		strUrl = "rtmp://192.168.1.103/live";
		strFmt = "flv";
		nFrameRate = 20;
	}
}rtmpconfig;

#ifndef _WIN32
class MuxLocker {
public:
	MuxLocker() { pthread_mutex_init(&m_mutex, NULL); }

	virtual ~MuxLocker() { pthread_mutex_destroy(&m_mutex); }

	bool Lock() { pthread_mutex_lock(&m_mutex); return true; }
	bool UnLock() { pthread_mutex_unlock(&m_mutex); return true; }

private:
	pthread_mutex_t m_mutex;
};
#else
class MuxLocker {
public:
	MuxLocker() { std::cout << "constructor" << std::endl; }

	virtual ~MuxLocker() { std::cout << "destructor" << std::endl; }

	bool Lock() { m_mutex.lock(); return true; }
	bool UnLock() { m_mutex.unlock(); return true; }
	bool TryLock() { m_mutex.try_lock(); return true; }
private:
	std::mutex m_mutex;
};
#endif

/*extern "C"*/EXPORT_DLLA void* CreateMuxHandle();

EXPORT_DLL void* HelloWorld();



class EXPORT_DLL CMultiMux {
public:
	CMultiMux();

	virtual ~CMultiMux();

	int PushStreamData(int nChannel, uint8_t* pRgb);

	int LoadConfig(const char* strPath);

	int PushStreamData(int nChannel, int nWidth, int nHeight, uint8_t* pRgb);

	int PushStreamData(int nChannel, int nWidth, int nHeight, const char* strFmt, const char* strUtl, uint8_t* pRgb, int nFrameRate = 20);

	int RemoveChannel(int nChannel);

#ifdef USE_OPENCV

#ifdef CV_THREE
	int PushStreamData(int nChannel, IplImage* pImg);

	int PushStreamData(int nChannel, const char* strFmt, const char* strUtl, IplImage* pImg, int nFrameRate=20);
#endif
	int PushStreamData(int nChannel, cv::Mat mat);

	int PushStreamData(int nChannel, const char* strFmt, const char* strUtl, cv::Mat mat, int nFrameRate = 20);
#endif

	//int AddStreamMux(CImageMuxing* m_pMux);
private:
	std::map<int, CImageMuxing*> m_mapMux;
	int m_nChannels;
	MuxLocker *	m_pLock;
	std::map<int, rtmpconfig*>   m_mapConfig;
};

EXPORT_DLL CMultiMux* createMultiMux();

#endif //__MULTI_MUXING_H__
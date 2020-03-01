#ifndef __IMAGE_MUXING_H__
#define __IMAGE_MUXING_H__
/*
��Ƶ������
*/

#define __STDC_FORMAT_MACROS

#include "MultiMux.h"
extern "C"
{
	#include <libavutil/avassert.h>
	#include <libavutil/channel_layout.h>
	#include <libavutil/opt.h>
	#include <libavutil/mathematics.h>
	#include <libavutil/timestamp.h>
	#include <libavformat/avformat.h>
	#include <libswscale/swscale.h>
	#include <libswresample/swresample.h>
}

#define USE_AUDIO

// a wrapper around a single output AVStream
typedef struct OutputStream
{
	AVStream *st;
	AVCodecContext *enc;
	/* pts of the next frame that will be generated */
	int64_t next_pts;
	int samples_count;
	AVFrame *frame;
	AVFrame *tmp_frame;
	float t, tincr, tincr2;
	struct SwsContext *sws_ctx;
	struct SwrContext *swr_ctx;
} OutputStream;

//#define USE_AUDIO

class EXPORT_DLL CImageMuxing {
public:
	CImageMuxing();

	CImageMuxing(int nWidth, int nHeight, const char* strFmt, int nFrameRate, const char* chOutputUrl);

	virtual ~CImageMuxing();

	//���������ʼ����ز���
	int InitVideo(int nWidth, int nHeight, const char* strFmt, int nFrameRate, const char* chOutputUrl);

	//����ͼƬĿ¼��ʼ����ز���
	int InitVideo(const char* strListDir, const char* strFmt, int nFrameRate, const char* chOutputUrl);

	//int InitVideo(const char* strFmt, int nFrameRate, const char* chOutputUrl);

	int PushStream(uint8_t* pRgb);

	int PushAudioStream(uint8_t* rgb);



#ifdef USE_OPENCV
	int PushStream(cv::Mat  mat);
#ifdef CV_THREE
	int PushStream(IplImage* pImg);
#endif
#endif

	int UnInit();

	int InitParam();

private:
	OutputStream   m_pOsmVideo;
	OutputStream   m_pOsmAudio;
	AVOutputFormat *m_pFmt;			/*����ļ�����*/
	AVFormatContext *m_pOc;			/*��������������*/
	AVCodec *m_pAudioCodec;			/*�������*/
	AVCodec *m_pVideoCodec;

private:
	int      m_nWidth;
	int      m_nHeight;
	std::string   m_strFmt;			//��������
	std::string   m_strUrl;			//���������ַ
	int      m_nFrameRate;			//֡��
	int      m_nBitRate;			//
	bool     m_bInit;
	std::string   m_strListDir;		//����Ŀ¼�����е�ͼƬ������

	int      m_nHasVideo;
	int      m_nHasAudio;
	int      m_nEncodeVideo;
	int      m_nEncodeAudio;

	static int  ChannelCount;

	int      m_nCanPush;
};

EXPORT_DLL CImageMuxing* createMuxer(int nWidth, int nHeight, const char* strFmt, const char* pOutUrl, int nFrameRate);

#endif //__IMAGE_MUXING_H__

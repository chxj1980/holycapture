#pragma once

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
#include <libavutil/imgutils.h>
}


int resample_picture(const int &width,const int &height,const int &OutWidth,const int &OutHeight,AVFrame *src_frame,AVFrame **dst_frame)
{
	AVFrame *tmp = av_frame_alloc();
	if (tmp == NULL)
	{
		return -2;
	}

	tmp->width = width;
	tmp->height = height;
	tmp->format = AV_PIX_FMT_YUV420P;
	int pic_size = avpicture_get_size((AVPixelFormat)tmp->format, width, height);
	uint8_t *m_yuvBuffer = new uint8_t[pic_size];
	avpicture_fill((AVPicture*)(tmp),m_yuvBuffer,(AVPixelFormat)tmp->format,tmp->width,tmp->height);

	SwsContext *scaleCtx = sws_getContext(width, height, (AVPixelFormat)tmp->format, OutWidth, OutHeight, (AVPixelFormat)tmp->format,SWS_BICUBIC, NULL, NULL, NULL);
	if (scaleCtx == NULL)
	{
		return -3;
	}

	if (sws_scale(scaleCtx,src_frame->data, src_frame->linesize,0,height,tmp->data, tmp->linesize) <= 0)
	{
		return -4;
	}

	sws_freeContext(scaleCtx);
	*dst_frame = tmp;
	return 0;
}

void ResizeImage()
{
	av_register_all();
	FILE* src_file = NULL;
	fopen_s(&src_file, "input.yuv", "rb");
	FILE* dst_file = NULL;
	fopen_s(&dst_file, "output.yuv", "wb");
	int src_width = 1280;
	int src_height = 720;
	int dst_width = 640;
	int dst_height = 480;

	int n_yuvsize = src_width * src_height * 3 / 2;

	char* p_src_yuv = (char*)av_malloc(n_yuvsize);

	//ffmpeg图像缩放算法，线性插值，二次线性插值
	struct SwsContext* pws = sws_getContext(src_width, src_height, AV_PIX_FMT_YUV420P,
		dst_width, dst_height, AV_PIX_FMT_YUV420P, SWS_BICUBIC, NULL, NULL, NULL);

	AVFrame src_frame, dst_frame;
	av_image_alloc(dst_frame.data, dst_frame.linesize, dst_width, dst_height, AV_PIX_FMT_YUV420P, 1);
	//dst_frame.data = av_calloc(0, dst_frame.linesize*dst_height*dst_height);
	
	av_image_fill_arrays(src_frame.data, src_frame.linesize, (uint8_t*)p_src_yuv, AV_PIX_FMT_YUV420P, src_width, src_height, 1);

	while (fread(p_src_yuv, 1, n_yuvsize, src_file) == n_yuvsize) 
	{
		if (dst_height != sws_scale(pws, src_frame.data, src_frame.linesize, 0, src_height, src_frame.data, src_frame.linesize))
			continue;

		fwrite(dst_frame.data[0], dst_frame.linesize[0] * dst_height, 1, dst_file);
		fwrite(dst_frame.data[1], dst_frame.linesize[1] * dst_height/2, 1, dst_file);
		fwrite(dst_frame.data[2], dst_frame.linesize[2] * dst_height/2, 1, dst_file);
	}

	av_free(p_src_yuv);
	av_freep(&dst_frame.data[0]);
	sws_freeContext(pws);
	fclose(src_file);
	fclose(dst_file);
}

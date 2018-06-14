/*
* Copyright (c) 2014, ����ǧ��
* All rights reserved.
* 
* �ļ����ƣ�vxRTSPClient.cpp
* �������ڣ�2014��7��24��
* �ļ���ʶ��
* �ļ�ժҪ��RTSP �ͻ��˹�������ز����ӿ����������͡�
* 
* ��ǰ�汾��1.0.0.0
* ��    �ߣ�
* ������ڣ�2014��7��24��
* �汾ժҪ��
* 
* ȡ���汾��
* ԭ����  ��
* ������ڣ�
* �汾ժҪ��
* 
*/
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <time.h>
#include <fcntl.h>
#include <stdarg.h>
#include <sys/stat.h>
#include "vxRTSPClient.h"
#include "stdafx.h"

#ifdef __cplusplus
extern "C"
{
#endif // __cplusplus

#define __STDC_LIMIT_MACROS
#define __STDC_CONSTANT_MACROS

#include <libavcodec/avcodec.h>
#include <libavformat/avformat.h>
#include <libswscale/swscale.h>

#ifdef __cplusplus
};
#endif // __cplusplus

#include <process.h>
#include <cassert>

#include "vxRTSPClient.inl"
#include "vxRTSPMemory.inl"

///////////////////////////////////////////////////////////////////////////

/**************************************************************************
* FunctionName:
*     destroy_thread
* Description:
*     ���ٹ����̡߳�
* Parameter:
*     @[in ] hthread: �߳̾����
*     @[in ] timeout: �ȴ��ĳ�ʱʱ�䡣
* ReturnValue:
*     void
*/
static void destroy_thread(x_handle_t hthread, x_uint32_t xt_timeout)
{
	DWORD dwExitCode = 0;
	if ((X_NULL != hthread) && GetExitCodeThread(hthread, &dwExitCode) && (STILL_ACTIVE == dwExitCode))
	{
		DWORD dwWait = WaitForSingleObject(hthread, xt_timeout);
		if (WAIT_TIMEOUT == dwWait)
		{
			TerminateThread(hthread, -1);
		}
	}

	CloseHandle(hthread);
}
static char *get_time_string(void)   
{   
	time_t tt;
	struct tm *t;
	static char timebuf[64];

	time(&tt);
	t = gmtime(&tt); 
	sprintf(&timebuf[0],"%04d-%02d-%02d %02d:%02d:%02d ",
		t->tm_year + 1900, t->tm_mon+1, t->tm_mday,
		t->tm_hour, t->tm_min, t->tm_sec);

	return  &timebuf[0];  
}

//int log_write(fmt, args...)
int log_write(const char *pFormat, ...)
{	

	int 		ret;
	struct stat 	sgbuf;
	char 		*timestr;

	char		loginfo[1024] = {0};
	va_list		args;

	va_start(args, pFormat);
	vsprintf(loginfo,pFormat,args);
	va_end(args);
	FILE* fp = fopen("./H264.log", "ab+");
	if(fp == NULL)
	{
		return -1;
	}

	timestr = get_time_string();
	fwrite(timestr,strlen(timestr), 1, fp);
	fwrite(loginfo,strlen(loginfo), 1, fp);
	fwrite("\r\n",strlen("\r\n"), 1, fp);
	fclose(fp);

	return 0;	
}

///////////////////////////////////////////////////////////////////////////
// vxFFmpegDecode definition

class vxFFmpegDecode
{
	// constructor/destructor
public:
	vxFFmpegDecode(void);
	~vxFFmpegDecode(void);

	// public interfaces
public:
	/**************************************************************************
	* FunctionName:
	*     initial
	* Description:
	*     �����ʼ��������
	* Parameter:
	*     @[in ] xt_decode_id: ������ ID��
	* ReturnValue:
	*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
	*/
	x_int32_t initial(x_int32_t xt_decode_id = AV_CODEC_ID_H264);

	/**************************************************************************
	* FunctionName:
	*     release
	* Description:
	*     ���󷴳�ʼ��������
	*/
	void release(void);

	/**************************************************************************
	* FunctionName:
	*     input_nalu_data
	* Description:
	*     ���뵥�� NALU ���ݰ�ִ�н��롣
	* Parameter:
	*     @[in ] xt_buf: ���ݰ����档
	*     @[in ] xt_size: ���ݰ����泤�ȡ�
	*     @[in ] xt_flip: ��ʶ�Ƿ�Խ���ͼ����д�ֱ��ת��
	* ReturnValue:
	*     ���� x_true����ʾ���һ֡������ͼ����빤�������� X_FALSE, ��ʾδ��ɡ�
	*/
	x_bool_t input_nalu_data(x_uint8_t * xt_buf, x_uint32_t xt_size, x_bool_t xt_flip,WT_ImageInfo_t	*imageInfo,enum WT_PixelFormat_t m_PixelFormat);

	/**************************************************************************
	* FunctionName:
	*     decode_buf
	* Description:
	*     ���� �������ͼ������ �Ļ����ַ��
	*/
	inline x_uint8_t * decode_data(void) { return (x_uint8_t *)m_xmem_decode.data(); }

	/**************************************************************************
	* FunctionName:
	*     decode_width
	* Description:
	*     ���� �������ͼ�� �Ŀ�ȡ�
	*/
	inline x_int32_t decode_width(void) const { return m_xt_width; }

	/**************************************************************************
	* FunctionName:
	*     decode_height
	* Description:
	*     ���� �������ͼ�� �ĸ߶ȡ�
	*/
	inline x_int32_t decode_height(void) const { return m_xt_height; }

	// class data
protected:
	AVCodecContext    * m_avcodec_context_ptr;
	AVCodec           * m_avcodec_ptr;
	AVFrame           * m_avframe_decode;
	AVFrame           * m_avframe_rgb_buf;
	struct SwsContext * m_sws_context_ptr;
	AVPacket            m_avpacket_decode;

	x_int32_t           m_xt_width;
	x_int32_t           m_xt_height;
	xmemblock           m_xmem_decode;

};

///////////////////////////////////////////////////////////////////////////
// vxFFmpegDecode implementation

//=========================================================================

// 
// vxFFmpegDecode constructor/destructor
// 

vxFFmpegDecode::vxFFmpegDecode(void)
			: m_avcodec_context_ptr(X_NULL)
			, m_avcodec_ptr(X_NULL)
			, m_avframe_decode(X_NULL)
			, m_avframe_rgb_buf(X_NULL)
			, m_sws_context_ptr(X_NULL)
			, m_xt_width(0)
			, m_xt_height(0)
{

}

vxFFmpegDecode::~vxFFmpegDecode(void)
{
	release();
}

//=========================================================================

// 
// vxFFmpegDecode public interfaces
// 

/**************************************************************************
* FunctionName:
*     initial
* Description:
*     �����ʼ��������
* Parameter:
*     @[in ] xt_decode_id: ������ ID��
* ReturnValue:
*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
*/
x_int32_t vxFFmpegDecode::initial(x_int32_t xt_decode_id /*= AV_CODEC_ID_H264*/)
{
	x_int32_t xt_err = X_ERR_UNKNOW;

	do 
	{
		// ������Ч�ж�
		if ((X_NULL != m_avcodec_context_ptr) ||
			(X_NULL != m_avcodec_ptr) ||
			(X_NULL != m_avframe_decode))
		{
			break;
		}

		// ��ʼ�� ffmpeg �������
		av_init_packet(&m_avpacket_decode);
/*
		m_avcodec_context_ptr = avcodec_alloc_context3(avcodec_find_decoder((AVCodecID)xt_decode_id));
		if (X_NULL == m_avcodec_context_ptr)
		{
			break;
		}
*/
		// ������
		m_avcodec_ptr = avcodec_find_decoder((AVCodecID)xt_decode_id);
		if (X_NULL == m_avcodec_ptr)
		{
			break;
		}

		m_avcodec_context_ptr = avcodec_alloc_context3(m_avcodec_ptr);
		if (X_NULL == m_avcodec_context_ptr)
		{
			break;
		}

		//if (avcodec_open(m_avcodec_context_ptr, m_avcodec_ptr) < 0)
		if (avcodec_open2(m_avcodec_context_ptr, m_avcodec_ptr,0) < 0)
		{
			break;
		}

		// �������֡
		m_avframe_decode = avcodec_alloc_frame();
		if (X_NULL == m_avframe_decode)
		{
			break;
		}

		xt_err = X_ERR_OK;
	} while (0);

	return xt_err;
}

/**************************************************************************
* FunctionName:
*     release
* Description:
*     ���󷴳�ʼ��������
*/
void vxFFmpegDecode::release(void)
{
	av_free_packet(&m_avpacket_decode);

	if (X_NULL != m_avcodec_context_ptr)
	{
		avcodec_close(m_avcodec_context_ptr);
		av_free(m_avcodec_context_ptr);
		m_avcodec_context_ptr = X_NULL;
	}

	m_avcodec_ptr = X_NULL;



	if (X_NULL != m_avframe_decode)
	{
		av_free(m_avframe_decode);
		m_avframe_decode = X_NULL;
	}

	if (X_NULL != m_avframe_rgb_buf)
	{
		av_free(m_avframe_rgb_buf);
		m_avframe_rgb_buf = X_NULL;
	}
	if (X_NULL != m_sws_context_ptr)
	{
		sws_freeContext(m_sws_context_ptr);
		m_sws_context_ptr = X_NULL;
	}
}

/**************************************************************************
* FunctionName:
*     input_nalu_data
* Description:
*     ���뵥�� NALU ���ݰ�ִ�н��롣
* Parameter:
*     @[in ] xt_buf: ���ݰ����档
*     @[in ] xt_size: ���ݰ����泤�ȡ�
*     @[in ] xt_flip: ��ʶ�Ƿ�Խ���ͼ����д�ֱ��ת��
* ReturnValue:
*     ���� x_true����ʾ���һ֡������ͼ����빤�������� X_FALSE, ��ʾδ��ɡ�
*/
x_bool_t vxFFmpegDecode::input_nalu_data(x_uint8_t * xt_buf, x_uint32_t xt_size, x_bool_t xt_flip,WT_ImageInfo_t	*imageInfo,enum WT_PixelFormat_t m_PixelFormat)
{
	
	m_avpacket_decode.data = xt_buf;
	m_avpacket_decode.size = (int)xt_size;

	int decoded_frame = 0;
	avcodec_decode_video2(m_avcodec_context_ptr, m_avframe_decode, &decoded_frame, &m_avpacket_decode);
	if (0 != decoded_frame)
	{
		// ���������ͼ���Ⱥ͸߶�
		m_xt_width  = m_avcodec_context_ptr->width;
		m_xt_height = m_avcodec_context_ptr->height;
		imageInfo->uWidth=m_avcodec_context_ptr->width;
		imageInfo->uHeight= m_avcodec_context_ptr->height;
		if (m_PixelFormat == WT_PIX_FMT_YUV420P)
		{
			imageInfo->pBuffer[0] = m_avframe_decode->data[0];
			imageInfo->pBuffer[1] = m_avframe_decode->data[1];
			imageInfo->pBuffer[2] = m_avframe_decode->data[2];
			imageInfo->uPitch[0]= m_avframe_decode->linesize[0];
			imageInfo->uPitch[1]= m_avframe_decode->linesize[1];
			imageInfo->uPitch[2]= m_avframe_decode->linesize[2];
			imageInfo->uPixFmt = WT_PIX_FMT_YUV420P;
		}


		if (xt_flip)
		{
			// ����Ϊ��ֱ��ת��ʽת���� RGB ��ʽ����
			m_avframe_decode->data[0] = m_avframe_decode->data[0] + m_avframe_decode->linesize[0] * (m_avcodec_context_ptr->height - 1);
			m_avframe_decode->data[1] = m_avframe_decode->data[1] + m_avframe_decode->linesize[1] * (m_avcodec_context_ptr->height / 2 - 1);
			m_avframe_decode->data[2] = m_avframe_decode->data[2] + m_avframe_decode->linesize[2] * (m_avcodec_context_ptr->height / 2 - 1);
			m_avframe_decode->linesize[0] *= -1;
			m_avframe_decode->linesize[1] *= -1;
			m_avframe_decode->linesize[2] *= -1;
		}

		if (X_NULL == m_avframe_rgb_buf)
		{
			m_xmem_decode.auto_resize(4 * m_xt_width * m_xt_height);

			m_avframe_rgb_buf = avcodec_alloc_frame();
			if (X_NULL == m_avframe_rgb_buf)
			{
				return X_FALSE;
			}

			avpicture_fill((AVPicture *)m_avframe_rgb_buf, (uint8_t *)m_xmem_decode.data(), PIX_FMT_BGR24, m_avcodec_context_ptr->width, m_avcodec_context_ptr->height);
		}

		if (X_NULL == m_sws_context_ptr)
		{
			m_sws_context_ptr = sws_getContext(m_avcodec_context_ptr->width, m_avcodec_context_ptr->height, PIX_FMT_YUV420P,
										m_avcodec_context_ptr->width, m_avcodec_context_ptr->height, PIX_FMT_BGR24, SWS_BICUBIC, X_NULL, X_NULL, X_NULL);
			if (X_NULL == m_sws_context_ptr)
			{
				return X_FALSE;
			}
		}

		// �� YUV ���ݸ�ʽת���� RGB ����
		try
		{
			sws_scale(m_sws_context_ptr, m_avframe_decode->data, m_avframe_decode->linesize, 0, m_avcodec_context_ptr->height, m_avframe_rgb_buf->data, m_avframe_rgb_buf->linesize);
		}
		catch (CMemoryException* e)
		{
			log_write("input_nalu_data()::sws_scale error CMemoryException ");
			return X_FALSE;
		}
		catch (CFileException* e)
		{
				log_write("input_nalu_data()::sws_scale error CFileException ");
			return X_FALSE;
		}
		catch (CException* e)
		{
			log_write("input_nalu_data()::sws_scale error CException ");
			return X_FALSE;
		}
		if (m_PixelFormat == WT_PIX_FMT_BGR24)
		{
			imageInfo->pBuffer[0] =  (uint8_t *)m_xmem_decode.data();
			imageInfo->uPitch[0]= (int) m_avframe_rgb_buf->linesize;
			imageInfo->uPixFmt = WT_PIX_FMT_BGR24;
		}
	
	}

	return (0 != decoded_frame);

}

///////////////////////////////////////////////////////////////////////////
// vxRTSPClient

//=========================================================================

// 
// vxRTSPClient static invoking methods
// 

/**************************************************************************
* FunctionName:
*     initial_lib
* Description:
*     ���ʼ��������
* Parameter:
*     @[in ] xt_pv_param: Ԥ��������
* ReturnValue:
*     �ɹ������� x_err_ok��ʧ�ܣ����� �����롣
*/
x_int32_t vxRTSPClient::initial_lib(x_handle_t xt_pv_param)
{
	x_int32_t xt_err = X_ERR_UNKNOW;

	do 
	{
		av_register_all();

		xt_err = X_ERR_OK;
	} while (0);

	return xt_err;
}

/**************************************************************************
* FunctionName:
*     uninitial_lib
* Description:
*     �ⷴ��ʼ��������
*/
void vxRTSPClient::uninitial_lib(void)
{

}

/**************************************************************************
* FunctionName:
*     thread_work_recv
* Description:
*     ���ݲɼ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_recv(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->recv_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     thread_work_decode
* Description:
*     ���ݽ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_work_decode(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->decode_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     thread_work_decode
* Description:
*     ���ݽ����߳���ں�����
*/
x_uint32_t __stdcall vxRTSPClient::thread_ReConnect(x_handle_t pv_param)
{
	((vxRTSPClient *)pv_param)->reconnect_loop();
	return 0;
}

/**************************************************************************
* FunctionName:
*     realframe_cbk_entry
* Description:
*     ʵʱ����֡�ص��ӿڡ�
*/
void vxRTSPClient::realframe_cbk_entry(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type, x_handle_t xt_user)
{
	vxRTSPClient * pv_this = (vxRTSPClient *)xt_user;
	pv_this->realframe_proc(xt_handle, xt_buf, xt_size, xt_type);
}

//=========================================================================

// 
// vxRTSPClient constructor/destructor
// 

vxRTSPClient::vxRTSPClient(void)
			: m_xt_hthread_recv(X_NULL)
			, m_xt_hthread_decode(X_NULL)
			, m_xt_bexitflag(X_TRUE)
			, m_xt_rtsp_client(X_NULL)
			, m_xt_realframe_queue(X_NULL)
			, m_xt_real_context_valid(X_FALSE)
			, m_xt_real_context_info(X_NULL)
			, m_xt_rtsp_url(X_NULL)
			, m_xt_width(0)
			, m_xt_height(0)
			, m_xt_flip(X_FALSE)
			, m_xfunc_realcbk(X_NULL)
{
	// ���������ڴ����
	m_xt_realframe_queue = (x_handle_t)(new xmemblock_cirqueue());
	if (X_NULL == m_xt_realframe_queue)
	{
		assert(false);
	}

	m_xt_real_context_info = (x_handle_t)(new xmemblock());
	if (X_NULL == m_xt_real_context_info)
	{
		assert(false);
	}

	x_uint32_t xt_tid;
	m_disconnect_times=0;
	m_getdata=0;
	memset(m_rtsp_url,0,200);
	m_WorkStatue = 0; //ֹͣ������

	 m_decodeThreadStatue = 0; // 0ֹͣ�� 
	 m_ReciveThreadStatue = 0; // 0ֹͣ�� 


	// ���������߳�
	m_xt_hthread_reconnect = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_ReConnect, this, 0, &xt_tid);


	 m_hwnd = NULL;

	 m_Enable = 0;
	 m_H264handle = 0;
}

vxRTSPClient::~vxRTSPClient(void)
{
	xmemblock * xt_mblk = (xmemblock *)m_xt_real_context_info;
	if (X_NULL != xt_mblk)
	{
		delete xt_mblk;
		xt_mblk = X_NULL;

		m_xt_real_context_info = X_NULL;
	}

	// �����ζ��л���
	xmemblock_cirqueue * xt_cirqueue = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL != xt_cirqueue)
	{
		xt_cirqueue->clear_cir_queue();
		delete xt_cirqueue;
		xt_cirqueue = X_NULL;

		m_xt_realframe_queue = X_NULL;
	}

	close();

	// ���������߳�
	if (X_NULL != m_xt_hthread_reconnect)
	{
		destroy_thread(m_xt_hthread_reconnect, INFINITE);
		m_xt_hthread_reconnect = X_NULL;
	}


}

//=========================================================================

// 
// vxRTSPClient public interfaces
// 

/**************************************************************************
* FunctionName:
*     open
* Description:
*     RTSP��ַ�Ĵ򿪲�����
* Parameter:
*     @[in ] xt_rtsp_url: RTSP URL ��ַ��
*     @[in ] xfunc_realcbk: �������ݻص������ӿڡ�

* ReturnValue:
*     �ɹ������� 0��ʧ�ܣ����� �����롣
*/
x_int8_t vxRTSPClient::open(const x_string_t xt_rtsp_url,HWND hwnd,enum WT_PixelFormat_t pixelFormat, int nEnable,WT_H264HANDLE handle)
{
	x_int8_t xt_err = X_ERR_UNKNOW;
	m_WorkStatue = 1; //�û�Ҫ��ʼ����
	do 
	{
		// ������Ч�ж�
		if ((X_NULL != m_xt_hthread_recv)    ||
			(X_NULL != m_xt_hthread_decode)  ||
			(X_NULL != m_xt_rtsp_client))
		{
			break;
		}

		// �������
		m_xt_rtsp_url   = _strdup(xt_rtsp_url);
		memset(m_rtsp_url,0,200);
		strcpy(m_rtsp_url,m_xt_rtsp_url);	
		m_hwnd = hwnd;
		m_PixelFormat = pixelFormat;
		m_xt_bexitflag  = X_FALSE;
		m_Enable = nEnable;
		m_H264handle = handle;
		// ���� RTSP ���ݽ��յĿͻ��˹�������
		m_xt_rtsp_client = (x_handle_t)vxRtspCliHandle::create(xt_rtsp_url);
		if (X_NULL == m_xt_rtsp_client)
		{
			break;
		}

		// �������ݻص��ӿ�
		((vxRtspCliHandle *)m_xt_rtsp_client)->set_recved_realframe_cbk((CLI_REALFRAME_CBK)&vxRTSPClient::realframe_cbk_entry, this);

		x_uint32_t xt_tid;

		// �������ݽ��յĹ����߳�
		m_xt_hthread_recv = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_work_recv, this, 0, &xt_tid);
		if (X_NULL == m_xt_hthread_recv)
		{
			break;
		}
		CloseHandle(m_xt_hthread_recv);
		m_xt_hthread_recv = X_NULL;
		// �������ݽ���Ĺ����߳�
		m_xt_hthread_decode = (x_handle_t)_beginthreadex(X_NULL, 0, &vxRTSPClient::thread_work_decode, this, 0, &xt_tid);
		if (X_NULL == m_xt_hthread_decode)
		{
			break;
		}
		CloseHandle(m_xt_hthread_decode);
		m_xt_hthread_decode = X_NULL;
		// ���� ��Ƶͷ��������Ϣ
		m_xt_real_context_valid = X_FALSE;
		if (X_NULL != m_xt_real_context_info)
		{
			((xmemblock *)m_xt_real_context_info)->reset();
		}

		xt_err = X_ERR_OK;
	} while (0);

	if (X_ERR_OK != xt_err)
	{
		DestroySource();
	}

	return xt_err;
}


void vxRTSPClient::DestroySource(void)
{
	// �����˳���ʶ
	m_xt_bexitflag = X_TRUE;
	//m_getdata=0;
	int times = 0;
	//�ж��������߳��Ƿ��Ѿ��˳�
	while(m_decodeThreadStatue || m_ReciveThreadStatue)
	{
		Sleep(10);
		times++;
		if(times > 300) // ���3������ôǿ���˳�
			break;
	}
	try{

		// ɾ�� RTSP ���ݽ��յĿͻ��˶���
		if (X_NULL != m_xt_rtsp_client)
		{
			vxRtspCliHandle::destroy((vxRtspCliHandle *)m_xt_rtsp_client);
			m_xt_rtsp_client = X_NULL;
		}
		// ���� ��Ƶͷ��������Ϣ
		m_xt_real_context_valid = X_FALSE;
		if (X_NULL != m_xt_real_context_info)
		{
			((xmemblock *)m_xt_real_context_info)->reset();
		}

		if (X_NULL != m_xt_rtsp_url)
		{
			free(m_xt_rtsp_url);
			m_xt_rtsp_url = X_NULL;
		}

	}
	catch (CMemoryException* e)
	{
		log_write("DestroySource()::DestroySource error CMemoryException ");
		
	}
	catch (CFileException* e)
	{
		log_write("DestroySource()::DestroySource error CFileException ");
		
	}
	catch (CException* e)
	{
		log_write("DestroySource()::DestroySource error CException ");
		
	}
	m_xt_width  = 0;
	m_xt_height = 0;
}
/**************************************************************************
* FunctionName:
*     close
* Description:
*     ����رղ�����
*/
void vxRTSPClient::close(void)
{
 
	m_WorkStatue = 0; //�û�Ҫ��ֹͣ����
	DestroySource();//����
}

/**************************************************************************
* FunctionName:
*     set_max_cached_block_nums
* Description:
*     ������󻺴��ڴ���������
* Parameter:
*     @[in ] xt_max_nums: ��󻺴��ڴ���������
* ReturnValue:
*     void
*/
void vxRTSPClient::set_max_cached_block_nums(x_uint32_t xt_max_nums)
{
	if (X_NULL != m_xt_realframe_queue)
	{
		((xmemblock_cirqueue *)m_xt_realframe_queue)->resize_max_blocks(xt_max_nums);
	}
}

/**************************************************************************
* FunctionName:
*     get_max_cached_block_nums
* Description:
*     ��ȡ��󻺴��ڴ���������
*/
x_uint32_t vxRTSPClient::get_max_cached_block_nums(void) const
{
	if (X_NULL != m_xt_realframe_queue)
	{
		return (x_uint32_t)((xmemblock_cirqueue *)m_xt_realframe_queue)->max_blocks_size();
	}

	return 0;
}

/**************************************************************************
* FunctionName:
*     get_realframe_context
* Description:
*     ��ȡ ��Ƶ����������Ϣ��
* Parameter:
*     @[out] xt_buf: ��Ϣ������档
*     @[in out] xt_size: ��Σ���Ϣ�������Ĵ�С���زΣ���Ƶ����������Ϣ�Ĵ�С��
* ReturnValue:
*     �ɹ������� X_TRUE��ʧ�ܣ����� X_FALSE��
*/
x_bool_t vxRTSPClient::get_realframe_context(x_uint8_t * xt_buf, x_uint32_t & xt_size)
{
	xmemblock * xmblk = (xmemblock *)m_xt_real_context_info;
	if ((X_NULL == xmblk) || (X_TRUE != m_xt_real_context_valid))
	{
		return X_FALSE;
	}

	if ((X_NULL != xt_buf) && (xt_size >= xmblk->size()))
	{
		memcpy(xt_buf, xmblk->data(), xmblk->size());
	}

	xt_size = xmblk->size();

	return X_TRUE;
}

//=========================================================================

// 
// vxRTSPClient inner invoking methods
// 

/**************************************************************************
* FunctionName:
*     recv_loop
* Description:
*     ���ݽ��յ��¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::recv_loop(void)
{
	
	vxRtspCliHandle * rtsp_client_handle = (vxRtspCliHandle *)m_xt_rtsp_client;
	if (X_NULL == rtsp_client_handle)
	{
		return;
	}
	m_ReciveThreadStatue = 1;
	rtsp_client_handle->do_event_loop(&m_xt_bexitflag);
	rtsp_client_handle->shutdown_stream();

/*
	// ɾ�� RTSP ���ݽ��յĿͻ��˶���
	if (X_NULL != m_xt_rtsp_client)
	{
		vxRtspCliHandle::destroy((vxRtspCliHandle *)m_xt_rtsp_client);
		m_xt_rtsp_client = X_NULL;
	}
	
	//m_xt_hthread_recv = X_NULL;
	*/
	m_xt_bexitflag = X_TRUE;
	m_ReciveThreadStatue = 0;
}

/**************************************************************************
* FunctionName:
*     decode_loop
* Description:
*     ���ݽ�����¼��������̣����� thread_work_recv() �ӿڻص��ò�������
*/
void vxRTSPClient::decode_loop(void)
{

	WT_H264Decode_t h264_decode;
	memset(&h264_decode,0,sizeof(h264_decode));
	h264_decode.handle = m_H264handle;


	xmemblock * x_block_ptr = X_NULL;
	// ����֡���յĻ��λ������
	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}

	// ffmpeg �����������
	vxFFmpegDecode ffdecode;

	// ��ʼ�� ffmpeg �������
	if (X_ERR_OK != ffdecode.initial(AV_CODEC_ID_H264))
	{
		return;
	}
	m_decodeThreadStatue = 1;
	// ѭ��������յ�������֡��ִ�н��롢�ص��ȹ���
	while (!is_exit_work())
	{
		// ȡ֡����
		x_block_ptr = x_cirqueue_ptr->pop_front_from_saved_queue();
		if (X_NULL == x_block_ptr)
		{
			Sleep(5);
			//m_getdata = 0;
			continue;
		}
		m_getdata = 1;
		//m_disconnect_times = 0;
		// ��������֡
		if (ffdecode.input_nalu_data((x_uint8_t *)x_block_ptr->data(), x_block_ptr->size(), m_xt_flip,&(h264_decode.imageInfo),m_PixelFormat))
		{
			m_xt_width  = ffdecode.decode_width();
			m_xt_height = ffdecode.decode_height();
			// ���ݻص�
			if (X_NULL != m_xfunc_realcbk)
			{

				m_xfunc_realcbk(&h264_decode);
			}
			if(m_Enable && m_PixelFormat == WT_PIX_FMT_BGR24)
			{
				//��ʾ
				ShowImage(ffdecode.decode_data(), 3 * m_xt_width * m_xt_height,m_xt_width,m_xt_height);

			}
		}

		// ����֡�ڴ��
		x_cirqueue_ptr->recyc(x_block_ptr);
		x_block_ptr = X_NULL;


	}
	if (X_NULL != x_block_ptr)
	{
		x_cirqueue_ptr->recyc(x_block_ptr);
	}

	x_cirqueue_ptr->clear_cir_queue();
	//CloseHandle(m_xt_hthread_decode);
	//m_xt_hthread_decode = X_NULL;
	m_xt_width  = 0;
	m_xt_height = 0;
	m_xt_bexitflag = X_TRUE;
	m_decodeThreadStatue = 0;

}

void vxRTSPClient::reconnect_loop(void)
{
	// ѭ��������յ�������֡��ִ�н��롢�ص��ȹ���
	while (1)
	{
		
		if ((m_WorkStatue == 1))//�ж��û��Ƿ������������������ô�ͽ�������   ������û������ر�����ô���ǲ�������������
		{
			if(m_getdata == 0)//�ж��Ƿ��ȡ��������
			{
				m_disconnect_times++;//û��������ô�ۼ�
			}else{
			
				m_disconnect_times = 0;//��������ô����ۼ�
			}
				
			m_getdata = 0;//��ձ�ʶ
			if(m_disconnect_times > _RECONNECT_TIME_)//�ۼƳ�����һ��ʱ��
			{
				//��������
				DestroySource();//������Դ
				Sleep(5000);
				real_decode_enable_flip(0);
				open(m_rtsp_url,m_hwnd,m_PixelFormat,m_Enable,m_H264handle);//���´���
				m_disconnect_times = 0;//���¼���
			} 
			
		}
		Sleep(1000);//�߳�ʱ��ÿ��һ��
	}
}
/**************************************************************************
* FunctionName:
*     realframe_proc
* Description:
*     ʵʱ����֡�ص����մ������̣����� realframe_cbk_entry() �ӿڻص��ò�������
*/
void vxRTSPClient::realframe_proc(x_handle_t xt_handle, x_handle_t xt_buf, x_uint32_t xt_size, x_uint32_t xt_type)
{
	if (m_xt_rtsp_client != xt_handle)
	{
		return;
	}

	xmemblock_cirqueue * x_cirqueue_ptr = (xmemblock_cirqueue *)m_xt_realframe_queue;
	if (X_NULL == x_cirqueue_ptr)
	{
		return;
	}

	xmemblock * x_block_ptr = x_cirqueue_ptr->alloc();
	if (X_NULL == x_block_ptr)
	{
		return;
	}

	x_uint8_t xt_start_code[4] = { 0x00, 0x00, 0x00, 0x01 };
	x_block_ptr->write_block(xt_start_code, 4 * sizeof(x_uint8_t));
	x_block_ptr->append_data(xt_buf, xt_size);

	if (RTSP_FRAMETYPE_RCDPARAM == xt_type)
	{

		
		if ((X_NULL != m_xt_real_context_info) && (X_NULL != xt_buf))
		{
			((xmemblock *)m_xt_real_context_info)->append_data(x_block_ptr->data(), x_block_ptr->size());
		}

		m_xt_real_context_valid = (X_NULL == xt_buf);


	}

	// ʵʱ H264 �����ص�
	if (X_NULL != m_xfunc_realcbk)
	{
		//m_xfunc_realcbk(X_REAL_TYPE_H264, (x_uint8_t *)x_block_ptr->data(), x_block_ptr->size(), 0, 0, m_xt_user);
	}

	x_cirqueue_ptr->push_back_to_saved_queue(x_block_ptr);
}


void vxRTSPClient::ShowImage(unsigned char *pFrameRGB,UINT nSize, int nWidth, int nHeight)
{


	CRect rc;
	HDC hDC = GetDC(m_hwnd);
	CDC *pDC = NULL;
	BITMAPINFO m_bmphdr={0};
	DWORD dwBmpHdr = sizeof(BITMAPINFO);
	if(hDC !=NULL)
	{
		pDC = CDC::FromHandle(hDC);
	

		m_bmphdr.bmiHeader.biBitCount = 24;
		m_bmphdr.bmiHeader.biClrImportant = 0;
		m_bmphdr.bmiHeader.biSize = dwBmpHdr;
		m_bmphdr.bmiHeader.biSizeImage = 0;
		m_bmphdr.bmiHeader.biWidth = nWidth;
		m_bmphdr.bmiHeader.biHeight = -nHeight;
		m_bmphdr.bmiHeader.biXPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biYPelsPerMeter = 0;
		m_bmphdr.bmiHeader.biClrUsed = 0;
		m_bmphdr.bmiHeader.biPlanes = 1;
		m_bmphdr.bmiHeader.biCompression = BI_RGB;
 
		if(hDC !=NULL)
		{
			GetWindowRect(m_hwnd,rc);
			pDC->SetStretchBltMode(HALFTONE);

			StretchDIBits(hDC,
				0,0,
				rc.Width(),rc.Height(),
				0, 0,
				nWidth, nHeight,
				pFrameRGB,
				&m_bmphdr,
				DIB_RGB_COLORS,
				SRCCOPY);
		}
		//pDC->DeleteDC();	
		ReleaseDC(m_hwnd,hDC);		
	}

}
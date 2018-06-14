#ifndef _WT_H264_H
#define _WT_H264_H
#include "stdafx.h"
#include "targetver.h"
typedef unsigned int WT_H264HANDLE;

// ���ظ�ʽ
enum WT_PixelFormat_t{
	WT_PIX_FMT_BGR24,
	WT_PIX_FMT_YUV420P,
};

/**ͼ����Ϣ*/
typedef struct
{
	unsigned		uWidth;			/**<���*/
	unsigned		uHeight;		/**<�߶�*/
	unsigned		uPitch[6];		/**<ͼ���ȵ�һ��������ռ�ڴ��ֽ���
										EP_PIX_FMT_BGR24:
										uPitch[0]: BGRһ��������ռ�ڴ��ֽ���
										
										EP_PIX_FMT_YUV420P��
										uPitch[0]: Yһ��������ռ�ڴ��ֽ���
										uPitch[1]: Uһ��������ռ�ڴ��ֽ���
										uPitch[2]: Vһ��������ռ�ڴ��ֽ���
									*/
	unsigned		uPixFmt;		/**<ͼ�����ظ�ʽ���ο�ö�ٶ���ͼ���ʽ��PixelFormat_t��*/
	unsigned char	*pBuffer[6];	/**<ͼ���ڴ���׵�ַ
										EP_PIX_FMT_BGR24:
										pBuffer[0]: BGR�׵�ַ
										
										EP_PIX_FMT_YUV420P��
										pBuffer[0]: Y�׵�ַ
										pBuffer[1]: U�׵�ַ
										pBuffer[2]: V�׵�ַ
									*/
}
WT_ImageInfo_t;

typedef struct WT_H264Decode_t{
	WT_H264HANDLE	handle;
	WT_ImageInfo_t	imageInfo;

	char			reserved[100];
}WT_H264Decode_t;

/* ����H264����

	return��
		�ɹ���0
		ʧ�ܣ�<0

	notes��
		�˽ӿڽ������һ��
*/
int __stdcall WT_H264Init();


/* ����H264����
	param:
		chIp:			Ҫ���ӵ��豸IP��ַ
		hwnd:			Ҫ��ʾ�Ŀؼ����
		pixelFormat:	����H264���������ظ�ʽ
		nEnable:		���ý�����Ƿ�������ʾ���ؼ���:nEnable:1��ʾ;0����ʾ
						ע������pixelFormat==WT_PIX_FMT_BGR24ʱ���Ż���������ʾ
	
	return��
		�ɹ�������H264���
		ʧ�ܣ�0
*/
WT_H264HANDLE __stdcall WT_H264Start(char *chIp, HWND hwnd, enum WT_PixelFormat_t pixelFormat, int nEnable) ;


/*  ���������豸��H264������
	param:
		handle:			H264���

	return��
		�ɹ���0
		ʧ�ܣ�<0

	notes:
		�Ͽ�H.264���պ���������ؼ���ʾ�ģ���Ҫ����
		����ShowWindow(TRUE)������ʹ�ؼ�������
*/
int __stdcall WT_H264End(WT_H264HANDLE handle);


/*  
	H264���٣��ͷ���Դ

	notes:
		������Դ����������ؼ���ʾ�ģ���Ҫ����
		����ShowWindow(TRUE)������ʹ�ؼ�������
*/
void __stdcall WT_H264Destory();


/*  ����H264��ʾ�Ĵ���
	param:
		handle:			H264���
		hwnd:			����ؼ����

	return��
		�ɹ���0
		ʧ�ܣ�<0
*/
int __stdcall WT_ChangeH264ShowWindows(WT_H264HANDLE handle, HWND hwnd);


/*  
	ͨ���ص��ķ�ʽ����ȡ���ա�����������
	notes��
		����Ĳ�������WT_H264Decode_t�ṹ��
*/
typedef void (*WT_H264DecodeCallback)(WT_H264Decode_t *h264_decode);
void __stdcall WT_RegH264DecodeEvent(WT_H264DecodeCallback h264Decode);
/*Ϊ�Ѿ��򿪵ĵ�·����ⵥ������ص�����
param��
handle:			H264���
h264Decode �ص�����
*/
int __stdcall WT_RegH264DecodeEvent_EX(WT_H264DecodeCallback h264Decode,WT_H264HANDLE handle);

#endif
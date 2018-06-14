#include "WT_H264.h"
#include "vxRTSPClient.h"


/* ����H264����

	return��
		�ɹ���0
		ʧ�ܣ�<0

	notes��
		�˽ӿڽ������һ��
*/
vxRTSPClient       m_rtspClient[16];
int Find_Decode()
{
	int i = 0;
	for (i = 0; i< 16;i++)
	{
		if(!m_rtspClient[i].is_working())
			return i;
	} 
	return -1;
}
int __stdcall WT_H264Init()
{
	vxRTSPClient::initial_lib(0);
	return 0;
}
/*  
	H264���٣��ͷ���Դ

	notes:
		������Դ����������ؼ���ʾ�ģ���Ҫ����
		����ShowWindow(TRUE)������ʹ�ؼ�������
*/
void __stdcall WT_H264Destory()
{
	int i = 0;
	for(i = 0; i< 16;i++)
	{
		if(m_rtspClient[i].is_working())
			m_rtspClient[i].close();
	}

	vxRTSPClient::uninitial_lib();
}
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
WT_H264HANDLE __stdcall WT_H264Start(char *chIp, HWND hwnd, enum WT_PixelFormat_t pixelFormat, int nEnable) 
{
	char str[200] = {0};
	WT_H264HANDLE handle = 0;
	sprintf(str,"rtsp://%s/stream1",chIp);
	WT_H264HANDLE ID = Find_Decode();
	if (ID < 0 || ID > 15)
	{
	return 0;
	}
	if(m_rtspClient[ID].is_working())
		return -1;
	handle = ID+1;
	m_rtspClient[ID].real_decode_enable_flip(0);
	if( 0 != m_rtspClient[ID].open(str,hwnd,pixelFormat, nEnable,handle))
		return 0;
	return handle;

}

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
int __stdcall WT_H264End(WT_H264HANDLE handle)
{
	int ID = handle -1;
	if(ID < 0 || ID >15)
		return -1;
	if(!m_rtspClient[ID].is_working())
		return -1;
	m_rtspClient[ID].close();
	return 0;

}

/*  ����H264��ʾ�Ĵ���
	param:
		handle:			H264���
		hwnd:			����ؼ����

	return��
		�ɹ���0
		ʧ�ܣ�<0
*/
int __stdcall WT_ChangeH264ShowWindows(WT_H264HANDLE handle, HWND hwnd)
{
	int ID = handle -1;
	m_rtspClient[ID].m_hwnd = hwnd;
	return 0;
}
	
/*  
	ͨ���ص��ķ�ʽ����ȡ���ա�����������
	notes��
		����Ĳ�������WT_H264Decode_t�ṹ��
*/
void __stdcall WT_RegH264DecodeEvent(WT_H264DecodeCallback h264Decode)
{
	int i = 0;
	for (i =0 ;i<16;i++)
	{
		m_rtspClient[i].m_xfunc_realcbk = h264Decode;
	}
	
}

/*Ϊ�Ѿ��򿪵ĵ�·����ⵥ������ص�����
param��
handle:			H264���
h264Decode �ص�����
*/
int __stdcall WT_RegH264DecodeEvent_EX(WT_H264DecodeCallback h264Decode,WT_H264HANDLE handle)
{
	int ID = handle -1;
	if(ID < 0 || ID >15)
	return -1;
	m_rtspClient[ID].m_xfunc_realcbk = h264Decode;
	return 1;

}
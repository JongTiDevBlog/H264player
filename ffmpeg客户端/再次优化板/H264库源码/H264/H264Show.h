/*
�������ܣ�����Ƶ��
hWnd:��ʾ�Ĵ���
szURL������ַ ���磺rtsp://192.168.0.98/stream1
����ֵ�����ؾ��
*/
HANDLE __stdcall H264_Play(HWND hWnd, LPCSTR szURL);
/*
�ر���
hPlay  ��H264_Play���صľ��
*/
VOID __stdcall H264_Destroy(HANDLE hPlay);
/*
���ſ���
hPlay:H264_Play���صľ��
cmd : 0 ��ͣ���� 1 ��ʼ����
*/
VOID __stdcall H264_Control(HANDLE hPlay,int cmd);
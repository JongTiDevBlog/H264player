// H264videoDlg.h : ͷ�ļ�
//

#pragma once

#include <tchar.h>
#include <time.h>
#include <vlc/vlc.h>
#include "afxwin.h"

// CH264videoDlg �Ի���
class CH264videoDlg : public CDialog
{
// ����
public:
	CH264videoDlg(CWnd* pParent = NULL);	// ��׼���캯��
	//int showH264();
// �Ի�������
	enum { IDD = IDD_H264VIDEO_DIALOG };

	libvlc_instance_t *     m_vlc_ins;
	libvlc_media_player_t * m_vlc_player;
	libvlc_media_t *        m_vlc_media;
	void * m_hwnd;
	CString m_url;
	int m_x;
	int m_y;
	int m_wigth;
	int m_higth;

	void FreeH264();
	int showH264();
	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()

public:
	afx_msg void OnBnClickedOk();
	afx_msg void OnBnClickedButton1();
	// ��ʼ����
	CButton m_startplay;
	// ֹͣ����
	CButton m_stopplay;
	// ��ַ�����
	CEdit m_urlEdi;
	afx_msg void OnClose();

	// ��ʾ��Ƶ
	CStatic m_PicVideo;
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	afx_msg void OnBnClickedButton2();
	// ȫ��
	CButton m_Fullscreen;
	CStatic m_staticUrl;
};

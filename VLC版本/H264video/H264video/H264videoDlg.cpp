// H264videoDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "H264video.h"
#include "H264videoDlg.h"


#pragma comment(lib, "libvlc.lib")
#pragma comment(lib, "libvlccore.lib")


#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CH264videoDlg �Ի���




CH264videoDlg::CH264videoDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CH264videoDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	m_vlc_ins    = NULL;
	m_vlc_player = NULL;
	m_vlc_media  = NULL;
	m_hwnd = NULL;
	m_x = 500;
	m_y = 250;
	m_wigth = 600;
	m_higth = 600;


}

void CH264videoDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDOK, m_startplay);
	DDX_Control(pDX, IDC_BUTTON1, m_stopplay);
	DDX_Control(pDX, IDC_EDIT1, m_urlEdi);
	DDX_Control(pDX, IDC_H264_SHOW, m_PicVideo);
	DDX_Control(pDX, IDC_BUTTON2, m_Fullscreen);
	DDX_Control(pDX, IDC_RUL_, m_staticUrl);
}

BEGIN_MESSAGE_MAP(CH264videoDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CH264videoDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDC_BUTTON1, &CH264videoDlg::OnBnClickedButton1)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON2, &CH264videoDlg::OnBnClickedButton2)
END_MESSAGE_MAP()


// CH264videoDlg ��Ϣ�������

BOOL CH264videoDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// ���ý���
	MoveWindow(m_x,m_y,m_wigth,m_higth);
	m_PicVideo.MoveWindow(4,60,m_wigth - 15,m_higth - 130, TRUE);
	m_urlEdi.MoveWindow(60,30,m_wigth - 120,28);
	m_staticUrl.MoveWindow(10,30,50,28);
	m_staticUrl.SetWindowText("URL:");
	m_startplay.MoveWindow(80,m_higth - 60,50,30);
	m_Fullscreen.MoveWindow(m_wigth/2 - 25,m_higth - 60,50,30);
	m_stopplay.MoveWindow(m_wigth - 80 -50,m_higth - 60,50,30);
	//  ��ʾTitleBar  
	//ModifyStyle(0,  WS_CAPTION,  SWP_FRAMECHANGED); 
	m_startplay.EnableWindow(TRUE);
	m_stopplay.EnableWindow(FALSE);

	//��ʼ��������
	m_hwnd = GetDlgItem(IDC_H264_SHOW)->GetSafeHwnd();
	m_vlc_ins = libvlc_new(0, NULL);
	if(m_vlc_ins != NULL)
	{
		// ����һ��VLC������
		m_vlc_player = libvlc_media_player_new(m_vlc_ins);
	}



	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CH264videoDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialog::OnPaint();
	}



}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CH264videoDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}


int CH264videoDlg::showH264()
{
		if(m_vlc_player != NULL && m_vlc_ins !=NULL)
		{
			//libvlc_media_player_set_hwnd (m_vlc_player, hwnd);
			// ͨ���ļ�·������һ��ý��ʵ��,�������ҵĲ����ļ�
			//m_vlc_media = libvlc_media_new_path(m_vlc_ins, "d:\\my.h264");
			m_vlc_media = libvlc_media_new_location(m_vlc_ins,m_url.GetBuffer(0));
			if(m_vlc_media != NULL)
			{
				// ����ý��ʵ��
				libvlc_media_parse(m_vlc_media);
				// ��ȡý���ļ��Ĳ��ų���,  ���� ms
				//libvlc_time_t duration = libvlc_media_get_duration(m_vlc_media);

				// �˴��ǻ�ȡý������������Ƶ����Ƶ���Լ��������͵Ĺ����Ϣ
				//libvlc_media_track_info_t *media_tracks = NULL;
				//int trackCount = libvlc_media_get_tracks_info(m_vlc_media, &media_tracks);
				// �������ͷ��ڴ棬���Ҳ��Ե�ʱ��������⣬��û��ϸ�о���Ϊ��
				// free(media_tracks);  // crash?

				// �Ѵ򿪵�ý���ļ����ø�������
				libvlc_media_player_set_media(m_vlc_player,m_vlc_media);

				// ��Ϊ��windowsϵͳ��������Ҫ����һ��HWND����������Ϊ����,�����ֱ��ʹ�����洰��,������ǲ���
				libvlc_media_player_set_hwnd(m_vlc_player,m_hwnd);
				// ��ʼ������Ƶ
				libvlc_media_player_play(m_vlc_player);
				
			}

		}
	return 0;
}
void CH264videoDlg::FreeH264()
{
	if(m_vlc_media)
	{
		libvlc_media_player_stop(m_vlc_player);
		libvlc_media_release(m_vlc_media);
		m_vlc_media = NULL;

	}
	if (m_vlc_player)
	{
		libvlc_media_player_release(m_vlc_player);
		m_vlc_player = NULL;
	}
	if (m_vlc_ins)
	{
		libvlc_release(m_vlc_ins);
		m_vlc_ins = NULL;

	}
	
}

void CH264videoDlg::OnBnClickedOk()
{
	// ��ʼ����
	if(m_vlc_player)
	{
		m_urlEdi.GetWindowText(m_url);
		showH264();
		m_startplay.EnableWindow(FALSE);
		m_stopplay.EnableWindow(TRUE);
		Sleep(1000);
	}


}

void CH264videoDlg::OnBnClickedButton1()
{
	//ֹͣ����
	if (m_vlc_player)
	{
		libvlc_media_player_stop(m_vlc_player);
		//FreeH264();
		m_startplay.EnableWindow(TRUE);
		m_stopplay.EnableWindow(FALSE);
		Sleep(1000);

	}

}


void CH264videoDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ

	FreeH264();
	CDialog::OnClose();
}



BOOL CH264videoDlg::PreTranslateMessage(MSG* pMsg)
{
	// TODO: �ڴ����ר�ô����/����û���


if (WM_KEYDOWN == pMsg->message)
{
	if (VK_ESCAPE == pMsg->wParam)  
	{  
		MoveWindow(m_x,m_y,m_wigth,m_higth);
		m_PicVideo.MoveWindow(4,60,m_wigth - 15,m_higth - 130, TRUE);
		m_urlEdi.MoveWindow(60,30,m_wigth - 120,28);
		m_staticUrl.MoveWindow(10,30,50,28);
		m_staticUrl.SetWindowText("URL:");
		m_startplay.MoveWindow(80,m_higth - 60,50,30);
		m_Fullscreen.MoveWindow(m_wigth/2 - 25,m_higth - 60,50,30);
		m_stopplay.MoveWindow(m_wigth - 80 -50,m_higth - 60,50,30);
		//  ��ʾTitleBar  
		ModifyStyle(0,  WS_CAPTION,  SWP_FRAMECHANGED); 

		m_stopplay.ShowWindow(SW_SHOW);
		m_startplay.ShowWindow(SW_SHOW);
		m_Fullscreen.ShowWindow(SW_SHOW);
		m_urlEdi.ShowWindow(SW_SHOW);
		m_staticUrl.ShowWindow(SW_SHOW);
		return TRUE;
	}
}


	return CDialog::PreTranslateMessage(pMsg);
}

void CH264videoDlg::OnBnClickedButton2()
{
	// ʵ��ȫ��
	//��ȡ��Ļ�ֱ���
	ModifyStyle(WS_CAPTION,  0,  SWP_FRAMECHANGED);
	int nFullWidth  = GetSystemMetrics(SM_CXSCREEN);  
	int nFullHeight = GetSystemMetrics(SM_CYSCREEN);  
	MoveWindow(0,0,nFullWidth,nFullHeight);
	m_PicVideo.MoveWindow(0, 0, nFullWidth, nFullHeight, TRUE);


	ShowWindow(SW_HIDE);
	ShowWindow(SW_SHOW);
	m_stopplay.ShowWindow(SW_HIDE);
	m_startplay.ShowWindow(SW_HIDE);
	m_Fullscreen.ShowWindow(SW_HIDE);
	m_urlEdi.ShowWindow(SW_HIDE);
	m_staticUrl.ShowWindow(SW_HIDE);
	Invalidate();
	
}

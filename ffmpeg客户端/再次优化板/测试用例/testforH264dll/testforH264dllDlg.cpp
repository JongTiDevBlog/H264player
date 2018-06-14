// testforH264dllDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "testforH264dll.h"
#include "testforH264dllDlg.h"
#include "H264Show.h"
#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CtestforH264dllDlg �Ի���


HANDLE  H264handl = NULL;

CtestforH264dllDlg::CtestforH264dllDlg(CWnd* pParent /*=NULL*/)
	: CDialog(CtestforH264dllDlg::IDD, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);

	m_x = 500;//����
	m_y = 250;
	m_wigth = 600;//���
	m_higth = 600;


	//Init_H264();
}

void CtestforH264dllDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialog::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_EDIT1, m_urlTX);
	DDX_Control(pDX, IDC_STATIC1, m_show);
	DDX_Control(pDX, IDOK, m_start);
	DDX_Control(pDX, IDC_BUTTON1, m_fullscreen);
	DDX_Control(pDX, IDCANCEL, m_closestream);
	DDX_Control(pDX, IDC_STATIC2, m_txt);
}

BEGIN_MESSAGE_MAP(CtestforH264dllDlg, CDialog)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	//}}AFX_MSG_MAP
	ON_BN_CLICKED(IDOK, &CtestforH264dllDlg::OnBnClickedOk)
	ON_BN_CLICKED(IDCANCEL, &CtestforH264dllDlg::OnBnClickedCancel)
	ON_WM_CLOSE()
	ON_BN_CLICKED(IDC_BUTTON1, &CtestforH264dllDlg::OnBnClickedButton1)
	//ON_BN_CLICKED(IDC_BUTTON2, &CtestforH264dllDlg::OnBnClickedButton2)
	ON_STN_DBLCLK(IDC_STATIC1, &CtestforH264dllDlg::OnStnDblclickStatic1)
	ON_WM_LBUTTONDBLCLK()
	ON_BN_CLICKED(IDC_BUTTON5, &CtestforH264dllDlg::OnBnClickedButton5)
	ON_BN_CLICKED(IDC_BUTTON6, &CtestforH264dllDlg::OnBnClickedButton6)
	ON_BN_CLICKED(IDC_BUTTON7, &CtestforH264dllDlg::OnBnClickedButton7)
END_MESSAGE_MAP()


// CtestforH264dllDlg ��Ϣ�������

BOOL CtestforH264dllDlg::OnInitDialog()
{
	CDialog::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

	// TODO: �ڴ���Ӷ���ĳ�ʼ������

	m_urlTX.SetWindowText("rtsp://192.168.0.239/stream1");
	MoveWindow(m_x,m_y,m_wigth,m_higth);
	m_show.MoveWindow(4,80,m_wigth - 15,m_higth - 150, TRUE);
	m_urlTX.MoveWindow(60,30,m_wigth - 200,28);
	m_txt.MoveWindow(10,30,50,28);
	m_txt.SetWindowText("URL:");

	m_fullscreen.MoveWindow(m_wigth/2 - 25,m_higth - 60,50,30);
	m_start.MoveWindow(m_wigth-80,10,50,30);
	m_closestream.MoveWindow(m_wigth-80,50,50,30);

	//  ��ʾTitleBar  
	ModifyStyle(0,  WS_CAPTION,  SWP_FRAMECHANGED); 

	m_fullscreen.ShowWindow(SW_SHOW);
	m_urlTX.ShowWindow(SW_SHOW);
	m_txt.ShowWindow(SW_SHOW);
	m_start.ShowWindow(SW_SHOW);
	m_closestream.ShowWindow(SW_SHOW);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CtestforH264dllDlg::OnPaint()
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
HCURSOR CtestforH264dllDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}

void CtestforH264dllDlg::OnBnClickedOk()
{
	CString s1;
	int ret;
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CWnd* pWnd = this->GetDlgItem(IDC_STATIC1); //��ȡͼƬ�ؼ��Ĵ���ָ�� 

	m_urlTX.GetWindowText(s1);
	H264handl = H264_Play(* pWnd,s1);

	//OnOK();
}

void CtestforH264dllDlg::OnBnClickedCancel()
{
if(H264handl)	
 H264_Destroy(H264handl);
}

void CtestforH264dllDlg::OnClose()
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	//exit_H264();
	OnCancel();
	CDialog::OnClose();
}

void CtestforH264dllDlg::OnBnClickedButton1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	CString s1;
	//exit_H264();
	ModifyStyle(WS_CAPTION,  0,  SWP_FRAMECHANGED);
	int nFullWidth  = GetSystemMetrics(SM_CXSCREEN);  
	int nFullHeight = GetSystemMetrics(SM_CYSCREEN);  
	MoveWindow(0,0,nFullWidth,nFullHeight);
	m_show.MoveWindow(0, 0, nFullWidth, nFullHeight, TRUE);
	CWnd* pWnd = this->GetDlgItem(IDC_STATIC1); //��ȡͼƬ�ؼ��Ĵ���ָ�� 
	m_urlTX.GetWindowText(s1);

	//Open_stream(pWnd,s1.GetBuffer(0));
	//change_Window(pWnd);

}



void CtestforH264dllDlg::OnStnDblclickStatic1()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������


	//�Ŵ󴰿�
	MoveWindow(m_x,m_y,m_wigth,m_higth);
	m_show.MoveWindow(4,80,m_wigth - 15,m_higth - 150, TRUE);
	m_urlTX.MoveWindow(60,30,m_wigth - 120,28);
	m_txt.MoveWindow(10,30,50,28);
	m_txt.SetWindowText("URL:");
	m_startplay.MoveWindow(80,m_higth - 60,50,30);
	m_fullscreen.MoveWindow(m_wigth/2 - 25,m_higth - 60,50,30);

	m_closestream.MoveWindow(m_wigth-20,50,50,30);

	//  ��ʾTitleBar  
	ModifyStyle(0,  WS_CAPTION,  SWP_FRAMECHANGED); 

	m_fullscreen.ShowWindow(SW_SHOW);
	m_urlTX.ShowWindow(SW_SHOW);
	m_txt.ShowWindow(SW_SHOW);
	m_start.ShowWindow(SW_SHOW);
	m_closestream.ShowWindow(SW_SHOW);

	CWnd* pWnd = this->GetDlgItem(IDC_STATIC1); //��ȡͼƬ�ؼ��Ĵ���ָ�� 
	//change_Window(pWnd);

}

void CtestforH264dllDlg::OnLButtonDblClk(UINT nFlags, CPoint point)
{
	// TODO: �ڴ������Ϣ�����������/�����Ĭ��ֵ
	MoveWindow(m_x,m_y,m_wigth,m_higth);
	m_show.MoveWindow(4,60,m_wigth - 15,m_higth - 130, TRUE);
	m_urlTX.MoveWindow(60,30,m_wigth - 200,28);
	m_txt.MoveWindow(10,30,50,28);
	m_txt.SetWindowText("URL:");

	m_fullscreen.MoveWindow(m_wigth/2 - 25,m_higth - 60,50,30);

	m_start.MoveWindow(m_wigth-80,10,50,30);
	m_closestream.MoveWindow(m_wigth-80,40,50,30);

	//  ��ʾTitleBar  
	ModifyStyle(0,  WS_CAPTION,  SWP_FRAMECHANGED); 



	m_fullscreen.ShowWindow(SW_SHOW);
	m_urlTX.ShowWindow(SW_SHOW);
	m_txt.ShowWindow(SW_SHOW);
	m_start.ShowWindow(SW_SHOW);
	m_closestream.ShowWindow(SW_SHOW);

	CWnd* pWnd = this->GetDlgItem(IDC_STATIC1); //��ȡͼƬ�ؼ��Ĵ���ָ�� 
	//change_Window(pWnd);

	CDialog::OnLButtonDblClk(nFlags, point);

}

void CtestforH264dllDlg::OnBnClickedButton5()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
	if(H264handl)	
		H264_Control(H264handl,1);
}

void CtestforH264dllDlg::OnBnClickedButton6()
{
	if(H264handl)	
		H264_Control(H264handl,0);
}

void CtestforH264dllDlg::OnBnClickedButton7()
{

}

// testforH264dllDlg.h : ͷ�ļ�
//

#pragma once
#include "afxwin.h"


// CtestforH264dllDlg �Ի���
class CtestforH264dllDlg : public CDialog
{
// ����
public:
	CtestforH264dllDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
	enum { IDD = IDD_TESTFORH264DLL_DIALOG };

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
	afx_msg void OnBnClickedCancel();
	afx_msg void OnClose();
	CEdit m_urlTX;
	afx_msg void OnBnClickedButton1();
	CStatic m_show;
	CButton m_start;
	afx_msg void OnBnClickedButton2();
	afx_msg void OnBnClickedButton3();
	afx_msg void OnBnClickedButton4();
	afx_msg void OnStnDblclickStatic1();
	CButton m_startplay;
	CButton m_stoplay;
	CButton m_fullscreen;
	CButton m_closestream;
	CStatic m_txt;

	int m_x;
	int m_y;
	int m_wigth;
	int m_higth;
	afx_msg void OnLButtonDblClk(UINT nFlags, CPoint point);
	afx_msg void OnBnClickedButton5();
	afx_msg void OnBnClickedButton6();
	afx_msg void OnBnClickedButton7();
};


// LogonServerDlg.h : ͷ�ļ�
//

#pragma once

#include "ServiceUnits.h"
// CLogonServerDlg �Ի���
class CLogonServerDlg : public CDialogEx, public IServiceUnitsSink
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���
// ����
public:
	CLogonServerDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGONSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV ֧��


// ʵ��
protected:
	HICON m_hIcon;

	// ���ɵ���Ϣӳ�亯��
	virtual BOOL OnInitDialog();
	afx_msg void OnSysCommand(UINT nID, LPARAM lParam);
	afx_msg void OnPaint();
	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartServer();
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);
public:
	afx_msg void OnBnClickedStopService();
	afx_msg void OnBnClickedCancel();
	afx_msg void OnOK();
};

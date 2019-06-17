
// CorrespondDlg.h : ͷ�ļ�
//

#pragma once
#include "ServiceUnits.h"

// CCorrespondDlg �Ի���
class CCorrespondDlg : public CDialogEx, public IServiceUnitsSink
{
// ����
public:
	CCorrespondDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CORRESPOND_DIALOG };
#endif

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
	afx_msg void OnBnClickedStartServer();

protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);
	virtual VOID OnCancel();
	virtual VOID OnOK();
	afx_msg void OnBnClickedStopService();
	afx_msg void OnBnClickedButton1();
};

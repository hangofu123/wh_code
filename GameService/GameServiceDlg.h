
// GameServiceDlg.h : ͷ�ļ�
//

#pragma once

#include "ServiceUnits.h"
// CGameServiceDlg �Ի���
class CGameServiceDlg : public CDialogEx,public IServiceUnitsSink
{
	//�������
protected:
	CServiceUnits					m_ServiceUnits;						//����Ԫ
	CTraceServiceControl			m_TraceServiceControl;				//׷�ٴ���

																		//�������
protected:
	CModuleDBParameter				m_ModuleDBParameter;				//ģ�����

																		//���ò���
protected:
	bool							m_bAutoControl;						//�Զ�����
	bool							m_bOptionSuccess;					//���ñ�־
	tagModuleInitParameter			m_ModuleInitParameter;				//���ò���
// ����
public:
	CGameServiceDlg(CWnd* pParent = NULL);	// ��׼���캯��

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMESERVICE_DIALOG };
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
	afx_msg void OnBnClickedStartService();
public:
	//����״̬
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//��������
protected:
	//����ͼ��
	VOID UpdateServerLogo(LPCTSTR pszServerDLL);
	//���±���
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);
	//����״̬
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//�������
protected:
	//��������
	bool StartServerService(WORD wServerID);
	//��ȡ����
	bool InitDataBaseParameter(tagDataBaseParameter & DataBaseParameter);
public:
	afx_msg void OnBnClickedStopService();
	afx_msg void OnBnClickedOpenServer();
	afx_msg void OnBnClickedOpenMatch();
	afx_msg void OnBnClickedCreateServer();
	afx_msg void OnBnClickedOptionServer();
	afx_msg void OnBnClickedOptionMatch();
	LRESULT OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam);
protected:
	afx_msg LRESULT OnProcessCmdLine(WPARAM wParam, LPARAM lParam);
};

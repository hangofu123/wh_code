
// LogonServerDlg.h : 头文件
//

#pragma once

#include "ServiceUnits.h"
// CLogonServerDlg 对话框
class CLogonServerDlg : public CDialogEx, public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口
// 构造
public:
	CLogonServerDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_LOGONSERVER_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
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

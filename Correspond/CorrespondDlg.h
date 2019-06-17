
// CorrespondDlg.h : 头文件
//

#pragma once
#include "ServiceUnits.h"

// CCorrespondDlg 对话框
class CCorrespondDlg : public CDialogEx, public IServiceUnitsSink
{
// 构造
public:
	CCorrespondDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_CORRESPOND_DIALOG };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);	// DDX/DDV 支持


// 实现
protected:
	HICON m_hIcon;

	// 生成的消息映射函数
	virtual BOOL OnInitDialog();
	afx_msg void OnPaint();

	afx_msg HCURSOR OnQueryDragIcon();
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedStartServer();

protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);
	virtual VOID OnCancel();
	virtual VOID OnOK();
	afx_msg void OnBnClickedStopService();
	afx_msg void OnBnClickedButton1();
};

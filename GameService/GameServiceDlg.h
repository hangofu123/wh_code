
// GameServiceDlg.h : 头文件
//

#pragma once

#include "ServiceUnits.h"
// CGameServiceDlg 对话框
class CGameServiceDlg : public CDialogEx,public IServiceUnitsSink
{
	//组件变量
protected:
	CServiceUnits					m_ServiceUnits;						//服务单元
	CTraceServiceControl			m_TraceServiceControl;				//追踪窗口

																		//组件变量
protected:
	CModuleDBParameter				m_ModuleDBParameter;				//模块参数

																		//配置参数
protected:
	bool							m_bAutoControl;						//自动控制
	bool							m_bOptionSuccess;					//配置标志
	tagModuleInitParameter			m_ModuleInitParameter;				//配置参数
// 构造
public:
	CGameServiceDlg(CWnd* pParent = NULL);	// 标准构造函数

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_GAMESERVICE_DIALOG };
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
	afx_msg void OnBnClickedStartService();
public:
	//服务状态
	virtual VOID OnServiceUnitsStatus(enServiceStatus ServiceStatus);

	//辅助函数
protected:
	//更新图标
	VOID UpdateServerLogo(LPCTSTR pszServerDLL);
	//更新标题
	VOID UpdateServerTitle(enServiceStatus ServiceStatus);
	//更新状态
	VOID UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter);

	//服务控制
protected:
	//启动房间
	bool StartServerService(WORD wServerID);
	//获取连接
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


// LogonServerDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// 用于应用程序“关于”菜单项的 CAboutDlg 对话框

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// 对话框数据
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV 支持

// 实现
protected:
	DECLARE_MESSAGE_MAP()
};

CAboutDlg::CAboutDlg() : CDialogEx(IDD_ABOUTBOX)
{
}

void CAboutDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);

}

BEGIN_MESSAGE_MAP(CAboutDlg, CDialogEx)
END_MESSAGE_MAP()


// CLogonServerDlg 对话框



CLogonServerDlg::CLogonServerDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_LOGONSERVER_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
	
}

void CLogonServerDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);
}

BEGIN_MESSAGE_MAP(CLogonServerDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CLogonServerDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, &CLogonServerDlg::OnBnClickedStopService)
	ON_BN_CLICKED(IDCANCEL, &CLogonServerDlg::OnBnClickedCancel)
END_MESSAGE_MAP()


// CLogonServerDlg 消息处理程序

BOOL CLogonServerDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 将“关于...”菜单项添加到系统菜单中。

	// IDM_ABOUTBOX 必须在系统命令范围内。
	ASSERT((IDM_ABOUTBOX & 0xFFF0) == IDM_ABOUTBOX);
	ASSERT(IDM_ABOUTBOX < 0xF000);

	CMenu* pSysMenu = GetSystemMenu(FALSE);
	if (pSysMenu != NULL)
	{
		BOOL bNameValid;
		CString strAboutMenu;
		bNameValid = strAboutMenu.LoadString(IDS_ABOUTBOX);
		ASSERT(bNameValid);
		if (!strAboutMenu.IsEmpty())
		{
			pSysMenu->AppendMenu(MF_SEPARATOR);
			pSysMenu->AppendMenu(MF_STRING, IDM_ABOUTBOX, strAboutMenu);
		}
	}

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CLogonServerDlg::OnSysCommand(UINT nID, LPARAM lParam)
{
	if ((nID & 0xFFF0) == IDM_ABOUTBOX)
	{
		CAboutDlg dlgAbout;
		dlgAbout.DoModal();
	}
	else
	{
		CDialogEx::OnSysCommand(nID, lParam);
	}
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CLogonServerDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // 用于绘制的设备上下文

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// 使图标在工作区矩形中居中
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// 绘制图标
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//当用户拖动最小化窗口时系统调用此函数取得光标
//显示。
HCURSOR CLogonServerDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CLogonServerDlg::OnBnClickedStartServer()
{
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}

VOID CLogonServerDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
	{
		//设置标题
		SetWindowText(TEXT("登录服务器 -- [ 停止 ]"));

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(TRUE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("服务停止成功");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Config:	//配置状态
	{
		//设置标题
		SetWindowText(TEXT("登录服务器 -- [ 初始化 ]"));

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(FALSE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("正在初始化组件...");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Service:	//服务状态
	{
		//设置标题
		SetWindowText(TEXT("登录服务器 -- [ 运行 ]"));

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(FALSE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("服务启动成功");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	}

	return;
}


void CLogonServerDlg::OnBnClickedStopService()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CLogonServerDlg::OnBnClickedCancel()
{
	//关闭询问
	if (m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion = TEXT("协调服务器正在运行中，您确实要关闭服务器吗？");
		if (AfxMessageBox(pszQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) return;
	}

	//停止服务
	m_ServiceUnits.ConcludeService();
	__super::OnCancel();
}

void CLogonServerDlg::OnOK()
{

}


// CorrespondDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "Correspond.h"
#include "CorrespondDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCorrespondDlg 对话框



CCorrespondDlg::CCorrespondDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_CORRESPOND_DIALOG, pParent)
{
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CCorrespondDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
	DDX_Control(pDX, IDC_TRACE_MESSAGE, m_TraceServiceControl);

}

BEGIN_MESSAGE_MAP(CCorrespondDlg, CDialogEx)
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVER, &CCorrespondDlg::OnBnClickedStartServer)
	ON_BN_CLICKED(IDC_STOP_SERVICE, &CCorrespondDlg::OnBnClickedStopService)
	ON_BN_CLICKED(IDC_BUTTON1, &CCorrespondDlg::OnBnClickedButton1)
END_MESSAGE_MAP()


// CCorrespondDlg 消息处理程序

BOOL CCorrespondDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// 设置此对话框的图标。  当应用程序主窗口不是对话框时，框架将自动
	//  执行此操作
	SetIcon(m_hIcon, TRUE);			// 设置大图标
	SetIcon(m_hIcon, FALSE);		// 设置小图标

									//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

// 如果向对话框添加最小化按钮，则需要下面的代码
//  来绘制该图标。  对于使用文档/视图模型的 MFC 应用程序，
//  这将由框架自动完成。

void CCorrespondDlg::OnPaint()
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
HCURSOR CCorrespondDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCorrespondDlg::OnBnClickedStartServer()
{
	//启动服务
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

VOID CCorrespondDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//状态设置
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
	{
		//设置标题
		SetWindowText(TEXT("协调服务器 -- [ 停止 ]"));

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
		SetWindowText(TEXT("协调服务器 -- [ 初始化 ]"));

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
		SetWindowText(TEXT("协调服务器 -- [ 运行 ]"));

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


void CCorrespondDlg::OnBnClickedStopService()
{
	//停止服务
	try
	{
		m_ServiceUnits.ConcludeService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return;
}




void CCorrespondDlg::OnCancel()
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

VOID CCorrespondDlg::OnOK()
{

}




void CCorrespondDlg::OnBnClickedButton1()
{
	/*unsigned char szOutData[10] = { 0x55 ,0xAA,0x01,0x00,0x01,0xF0 };
	unsigned char szReslutout = 0x00;
	for (int i = 2; i < 4; i++)
	{
		szReslutout ^= szOutData[i];
	}
	char hang[100] = "";
	sprintf(hang, "0x%02x", szReslutout);
	MessageBox(hang);*/
}

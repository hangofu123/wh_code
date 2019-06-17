
// GameServiceDlg.cpp : 实现文件
//

#include "stdafx.h"
#include "GameService.h"
#include "GameServiceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//消息定义
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//处理命令

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


// CGameServiceDlg 对话框



CGameServiceDlg::CGameServiceDlg(CWnd* pParent /*=NULL*/)
	: CDialogEx(IDD_GAMESERVICE_DIALOG, pParent)
{
	m_bAutoControl = false;
	m_bOptionSuccess = false;
	ZeroMemory(&m_ModuleInitParameter, sizeof(m_ModuleInitParameter));
	m_hIcon = AfxGetApp()->LoadIcon(IDR_MAINFRAME);
}

void CGameServiceDlg::DoDataExchange(CDataExchange* pDX)
{
	CDialogEx::DoDataExchange(pDX);
}

BEGIN_MESSAGE_MAP(CGameServiceDlg, CDialogEx)
	ON_WM_SYSCOMMAND()
	ON_WM_PAINT()
	ON_WM_QUERYDRAGICON()
	ON_BN_CLICKED(IDC_START_SERVICE, &CGameServiceDlg::OnBnClickedStartService)
	ON_BN_CLICKED(IDC_STOP_SERVICE, &CGameServiceDlg::OnBnClickedStopService)
	ON_BN_CLICKED(IDC_OPEN_SERVER, &CGameServiceDlg::OnBnClickedOpenServer)
	ON_BN_CLICKED(IDC_OPEN_MATCH, &CGameServiceDlg::OnBnClickedOpenMatch)
	ON_BN_CLICKED(IDC_CREATE_SERVER, &CGameServiceDlg::OnBnClickedCreateServer)
	ON_BN_CLICKED(IDC_OPTION_SERVER, &CGameServiceDlg::OnBnClickedOptionServer)
	ON_BN_CLICKED(IDC_OPTION_MATCH, &CGameServiceDlg::OnBnClickedOptionMatch)
	ON_MESSAGE(WM_PROCESS_CMD_LINE, &CGameServiceDlg::OnProcessCmdLine)
END_MESSAGE_MAP()


// CGameServiceDlg 消息处理程序

BOOL CGameServiceDlg::OnInitDialog()
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

									//设置标题
	SetWindowText(TEXT("游戏服务器 -- [ 停止 ]"));

	//设置图标
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//设置组件
	m_ServiceUnits.SetServiceUnitsSink(this);

	//命令处理
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);


	return TRUE;  // 除非将焦点设置到控件，否则返回 TRUE
}

void CGameServiceDlg::OnSysCommand(UINT nID, LPARAM lParam)
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

void CGameServiceDlg::OnPaint()
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
HCURSOR CGameServiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGameServiceDlg::OnBnClickedStartService()
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

VOID CGameServiceDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
	{
		//更新标题
		UpdateServerTitle(ServiceStatus);

		//服务按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		//配置按钮
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);
		if ((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) != 0)
			GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);

		//运行按钮
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("服务停止成功");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Config:	//配置状态
	{
		//更新标题
		UpdateServerTitle(ServiceStatus);

		//设置按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		//配置按钮
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);

		//运行按钮
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("正在初始化组件...");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Service:	//服务状态
	{
		//更新标题
		UpdateServerTitle(ServiceStatus);

		//服务按钮
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		//配置按钮
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);

		//运行按钮
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(TRUE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(TRUE);

		//提示信息
		LPCTSTR pszDescribe = TEXT("服务启动成功");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	}

	return;
}

VOID CGameServiceDlg::UpdateServerLogo(LPCTSTR pszServerDLL)
{
	//加载资源
	HINSTANCE hInstance = AfxLoadLibrary(pszServerDLL);

	//加载图形
	if (hInstance != NULL)
	{
		//设置资源
		AfxSetResourceHandle(hInstance);

		//设置资源
		CStatic * pServerLogo = (CStatic *)GetDlgItem(IDC_SERVER_LOGO);
		pServerLogo->SetIcon(::LoadIcon(hInstance, TEXT("SERVER_ICON")));

		//释放资源
		AfxFreeLibrary(hInstance);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	return;
}

VOID CGameServiceDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//变量定义
	LPCTSTR pszStatusName = NULL;
	LPCTSTR pszServerName = NULL;

	//状态字符
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//停止状态
	{
		pszStatusName = TEXT("停止");
		break;
	}
	case ServiceStatus_Config:	//配置状态
	{
		pszStatusName = TEXT("初始化");
		break;
	}
	case ServiceStatus_Service:	//运行状态
	{
		pszStatusName = TEXT("运行");
		break;
	}
	}

	//房间名字
	if (m_bOptionSuccess == false) pszServerName = TEXT("游戏服务器");
	else pszServerName = m_ModuleInitParameter.GameServiceOption.szServerName;

	//构造标题
	TCHAR szTitle[128] = TEXT("");
	_sntprintf(szTitle, CountArray(szTitle), TEXT("[ %s ] -- [ %s ]"), pszServerName, pszStatusName);

	//设置标题
	SetWindowText(szTitle);

	return;
}

VOID CGameServiceDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//设置变量
	m_bOptionSuccess = true;
	m_ModuleInitParameter = ModuleInitParameter;

	//更新标题
	UpdateServerTitle(ServiceStatus_Stop);
	UpdateServerLogo(ModuleInitParameter.GameServiceAttrib.szServerDLLName);

	//设置按钮
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	if ((ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) != 0)
	{
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
	}

	//设置控件
	SetDlgItemText(IDC_GAME_NAME, m_ModuleInitParameter.GameServiceAttrib.szGameName);
	SetDlgItemText(IDC_SERVER_NAME, m_ModuleInitParameter.GameServiceOption.szServerName);

	//监听端口
	if (m_ModuleInitParameter.GameServiceOption.wServerPort == 0)
	{
		SetDlgItemText(IDC_SERVER_PORT, TEXT("自动适配"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT, m_ModuleInitParameter.GameServiceOption.wServerPort);
	}

	//设置模块
	LPCTSTR pszServerDLLName = m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;
	m_ServiceUnits.CollocateService(pszServerDLLName, m_ModuleInitParameter.GameServiceOption);

	//构造提示
	TCHAR szString[256] = TEXT("");
	LPCTSTR pszServerName = m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf(szString, CountArray(szString), TEXT("[ %s ] 房间参数加载成功"), pszServerName);

	//输出信息
	CTraceService::TraceString(szString, TraceLevel_Normal);

	return;
}


bool CGameServiceDlg::StartServerService(WORD wServerID)
{
	//变量定义
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));

	//设置参数
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//读取配置
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer(wServerID) == false)
	{
		CTraceService::TraceString(TEXT("房间配置参数不存在或者加载失败"), TraceLevel_Exception);
		return false;
	}

	//更新状态
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	//启动服务
	try
	{
		m_ServiceUnits.StartService();
	}
	catch (...)
	{
		ASSERT(FALSE);
	}

	return true;
}

bool CGameServiceDlg::InitDataBaseParameter(tagDataBaseParameter & DataBaseParameter)
{
	//获取路径
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//构造路径
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	_sntprintf(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//读取配置
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//连接信息
	DataBaseParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("PlatformDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBAddr"), NULL, DataBaseParameter.szDataBaseAddr, CountArray(DataBaseParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBUser"), NULL, DataBaseParameter.szDataBaseUser, CountArray(DataBaseParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBPass"), NULL, DataBaseParameter.szDataBasePass, CountArray(DataBaseParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBName"), szPlatformDB, DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName));

	return true;
}

void CGameServiceDlg::OnBnClickedStopService()
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


void CGameServiceDlg::OnBnClickedOpenServer()
{
	//变量定义
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));

	//设置参数
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//配置房间
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer() == false) return;

	//更新状态
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOpenMatch()
{
	// TODO: 在此添加控件通知处理程序代码
}


void CGameServiceDlg::OnBnClickedCreateServer()
{
	//变量定义
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//设置参数
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//创建房间
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.CreateGameServer()==false) return;

	//更新状态
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOptionServer()
{
	//游戏模块
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(m_ModuleInitParameter.GameServiceAttrib.szServerDLLName, GAME_SERVICE_CREATE_NAME);

	//加载模块
	if (GameServiceManager.CreateInstance() == false)
	{
		AfxMessageBox(TEXT("服务组件不存在或者加载失败，配置房间失败"), MB_ICONERROR);
		return;
	}

	//配置房间
	CDlgServerWizard DlgServerWizard;
	DlgServerWizard.SetWizardParameter(GameServiceManager.GetInterface(), &m_ModuleInitParameter.GameServiceOption);

	//创建房间
	if (DlgServerWizard.CreateGameServer() == false) return;

	//更新状态
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOptionMatch()
{
	if ((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) == 0) return;
}

LRESULT CGameServiceDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine = (LPCTSTR)(lParam);

	//房间标识
	TCHAR szSrverID[32] = TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine, TEXT("/ServerID:"), szSrverID, CountArray(szSrverID)) == true)
	{
		//获取房间
		WORD wServerID = (WORD)(_tstol(szSrverID));

		//启动房间
		if (wServerID != 0)
		{
			//设置变量
			m_bAutoControl = true;

			//启动房间
			StartServerService(wServerID);
		}
	}

	return 0L;
}



afx_msg LRESULT CGameServiceDlg::OnProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//变量定义
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine = (LPCTSTR)(lParam);

	//房间标识
	TCHAR szSrverID[32] = TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine, TEXT("/ServerID:"), szSrverID, CountArray(szSrverID)) == true)
	{
		//获取房间
		WORD wServerID = (WORD)(_tstol(szSrverID));

		//启动房间
		if (wServerID != 0)
		{
			//设置变量
			m_bAutoControl = true;

			//启动房间
			StartServerService(wServerID);
		}
	}

	return 0L;
}

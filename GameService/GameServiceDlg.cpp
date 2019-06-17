
// GameServiceDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "GameService.h"
#include "GameServiceDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif
//��Ϣ����
#define WM_PROCESS_CMD_LINE			(WM_USER+100)						//��������

// ����Ӧ�ó��򡰹��ڡ��˵���� CAboutDlg �Ի���

class CAboutDlg : public CDialogEx
{
public:
	CAboutDlg();

// �Ի�������
#ifdef AFX_DESIGN_TIME
	enum { IDD = IDD_ABOUTBOX };
#endif

	protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV ֧��

// ʵ��
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


// CGameServiceDlg �Ի���



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


// CGameServiceDlg ��Ϣ�������

BOOL CGameServiceDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ��������...���˵�����ӵ�ϵͳ�˵��С�

	// IDM_ABOUTBOX ������ϵͳ���Χ�ڡ�
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

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

									//���ñ���
	SetWindowText(TEXT("��Ϸ������ -- [ ֹͣ ]"));

	//����ͼ��
	HICON hIcon = LoadIcon(AfxGetInstanceHandle(), MAKEINTRESOURCE(IDR_MAINFRAME));
	SetIcon(hIcon, TRUE);
	SetIcon(hIcon, FALSE);

	//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	//�����
	LPCTSTR pszCmdLine = AfxGetApp()->m_lpCmdLine;
	if (pszCmdLine[0] != 0) PostMessage(WM_PROCESS_CMD_LINE, 0, (LPARAM)pszCmdLine);


	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CGameServiceDlg::OnPaint()
{
	if (IsIconic())
	{
		CPaintDC dc(this); // ���ڻ��Ƶ��豸������

		SendMessage(WM_ICONERASEBKGND, reinterpret_cast<WPARAM>(dc.GetSafeHdc()), 0);

		// ʹͼ���ڹ����������о���
		int cxIcon = GetSystemMetrics(SM_CXICON);
		int cyIcon = GetSystemMetrics(SM_CYICON);
		CRect rect;
		GetClientRect(&rect);
		int x = (rect.Width() - cxIcon + 1) / 2;
		int y = (rect.Height() - cyIcon + 1) / 2;

		// ����ͼ��
		dc.DrawIcon(x, y, m_hIcon);
	}
	else
	{
		CDialogEx::OnPaint();
	}
}

//���û��϶���С������ʱϵͳ���ô˺���ȡ�ù��
//��ʾ��
HCURSOR CGameServiceDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CGameServiceDlg::OnBnClickedStartService()
{
	//��������
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
	case ServiceStatus_Stop:	//ֹͣ״̬
	{
		//���±���
		UpdateServerTitle(ServiceStatus);

		//����ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);

		//���ð�ť
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(TRUE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);
		if ((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) != 0)
			GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);

		//���а�ť
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("����ֹͣ�ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Config:	//����״̬
	{
		//���±���
		UpdateServerTitle(ServiceStatus);

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		//���ð�ť
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(FALSE);

		//���а�ť
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(FALSE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���ڳ�ʼ�����...");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Service:	//����״̬
	{
		//���±���
		UpdateServerTitle(ServiceStatus);

		//����ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVICE)->EnableWindow(FALSE);

		//���ð�ť
		GetDlgItem(IDC_OPEN_MATCH)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPEN_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_CREATE_SERVER)->EnableWindow(FALSE);
		GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(FALSE);

		//���а�ť
		GetDlgItem(IDC_RUN_PARAMETER)->EnableWindow(TRUE);
		GetDlgItem(IDC_SERVICE_CONTROL)->EnableWindow(TRUE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���������ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	}

	return;
}

VOID CGameServiceDlg::UpdateServerLogo(LPCTSTR pszServerDLL)
{
	//������Դ
	HINSTANCE hInstance = AfxLoadLibrary(pszServerDLL);

	//����ͼ��
	if (hInstance != NULL)
	{
		//������Դ
		AfxSetResourceHandle(hInstance);

		//������Դ
		CStatic * pServerLogo = (CStatic *)GetDlgItem(IDC_SERVER_LOGO);
		pServerLogo->SetIcon(::LoadIcon(hInstance, TEXT("SERVER_ICON")));

		//�ͷ���Դ
		AfxFreeLibrary(hInstance);
		AfxSetResourceHandle(GetModuleHandle(NULL));
	}

	return;
}

VOID CGameServiceDlg::UpdateServerTitle(enServiceStatus ServiceStatus)
{
	//��������
	LPCTSTR pszStatusName = NULL;
	LPCTSTR pszServerName = NULL;

	//״̬�ַ�
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
	{
		pszStatusName = TEXT("ֹͣ");
		break;
	}
	case ServiceStatus_Config:	//����״̬
	{
		pszStatusName = TEXT("��ʼ��");
		break;
	}
	case ServiceStatus_Service:	//����״̬
	{
		pszStatusName = TEXT("����");
		break;
	}
	}

	//��������
	if (m_bOptionSuccess == false) pszServerName = TEXT("��Ϸ������");
	else pszServerName = m_ModuleInitParameter.GameServiceOption.szServerName;

	//�������
	TCHAR szTitle[128] = TEXT("");
	_sntprintf(szTitle, CountArray(szTitle), TEXT("[ %s ] -- [ %s ]"), pszServerName, pszStatusName);

	//���ñ���
	SetWindowText(szTitle);

	return;
}

VOID CGameServiceDlg::UpdateParameterStatus(tagModuleInitParameter & ModuleInitParameter)
{
	//���ñ���
	m_bOptionSuccess = true;
	m_ModuleInitParameter = ModuleInitParameter;

	//���±���
	UpdateServerTitle(ServiceStatus_Stop);
	UpdateServerLogo(ModuleInitParameter.GameServiceAttrib.szServerDLLName);

	//���ð�ť
	GetDlgItem(IDC_START_SERVICE)->EnableWindow(TRUE);
	GetDlgItem(IDC_OPTION_SERVER)->EnableWindow(TRUE);

	if ((ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) != 0)
	{
		GetDlgItem(IDC_OPTION_MATCH)->EnableWindow(TRUE);
	}

	//���ÿؼ�
	SetDlgItemText(IDC_GAME_NAME, m_ModuleInitParameter.GameServiceAttrib.szGameName);
	SetDlgItemText(IDC_SERVER_NAME, m_ModuleInitParameter.GameServiceOption.szServerName);

	//�����˿�
	if (m_ModuleInitParameter.GameServiceOption.wServerPort == 0)
	{
		SetDlgItemText(IDC_SERVER_PORT, TEXT("�Զ�����"));
	}
	else
	{
		SetDlgItemInt(IDC_SERVER_PORT, m_ModuleInitParameter.GameServiceOption.wServerPort);
	}

	//����ģ��
	LPCTSTR pszServerDLLName = m_ModuleInitParameter.GameServiceAttrib.szServerDLLName;
	m_ServiceUnits.CollocateService(pszServerDLLName, m_ModuleInitParameter.GameServiceOption);

	//������ʾ
	TCHAR szString[256] = TEXT("");
	LPCTSTR pszServerName = m_ModuleInitParameter.GameServiceOption.szServerName;
	_sntprintf(szString, CountArray(szString), TEXT("[ %s ] ����������سɹ�"), pszServerName);

	//�����Ϣ
	CTraceService::TraceString(szString, TraceLevel_Normal);

	return;
}


bool CGameServiceDlg::StartServerService(WORD wServerID)
{
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));

	//���ò���
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//��ȡ����
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer(wServerID) == false)
	{
		CTraceService::TraceString(TEXT("�������ò��������ڻ��߼���ʧ��"), TraceLevel_Exception);
		return false;
	}

	//����״̬
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	//��������
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
	//��ȡ·��
	TCHAR szWorkDir[MAX_PATH] = TEXT("");
	CWHService::GetWorkDirectory(szWorkDir, CountArray(szWorkDir));

	//����·��
	TCHAR szIniFile[MAX_PATH] = TEXT("");
	_sntprintf(szIniFile, CountArray(szIniFile), TEXT("%s\\ServerParameter.ini"), szWorkDir);

	//��ȡ����
	CWHIniData IniData;
	IniData.SetIniFilePath(szIniFile);

	//������Ϣ
	DataBaseParameter.wDataBasePort = (WORD)IniData.ReadInt(TEXT("PlatformDB"), TEXT("DBPort"), 1433);
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBAddr"), NULL, DataBaseParameter.szDataBaseAddr, CountArray(DataBaseParameter.szDataBaseAddr));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBUser"), NULL, DataBaseParameter.szDataBaseUser, CountArray(DataBaseParameter.szDataBaseUser));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBPass"), NULL, DataBaseParameter.szDataBasePass, CountArray(DataBaseParameter.szDataBasePass));
	IniData.ReadEncryptString(TEXT("PlatformDB"), TEXT("DBName"), szPlatformDB, DataBaseParameter.szDataBaseName, CountArray(DataBaseParameter.szDataBaseName));

	return true;
}

void CGameServiceDlg::OnBnClickedStopService()
{
	//ֹͣ����
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
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter, sizeof(DataBaseParameter));

	//���ò���
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//���÷���
	CDlgServerItem DlgServerItem;
	if (DlgServerItem.OpenGameServer() == false) return;

	//����״̬
	UpdateParameterStatus(DlgServerItem.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOpenMatch()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CGameServiceDlg::OnBnClickedCreateServer()
{
	//��������
	tagDataBaseParameter DataBaseParameter;
	ZeroMemory(&DataBaseParameter,sizeof(DataBaseParameter));

	//���ò���
	InitDataBaseParameter(DataBaseParameter);
	m_ModuleDBParameter.SetPlatformDBParameter(DataBaseParameter);

	//��������
	CDlgServerWizard DlgServerWizard;
	if (DlgServerWizard.CreateGameServer()==false) return;

	//����״̬
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOptionServer()
{
	//��Ϸģ��
	CGameServiceManagerHelper GameServiceManager;
	GameServiceManager.SetModuleCreateInfo(m_ModuleInitParameter.GameServiceAttrib.szServerDLLName, GAME_SERVICE_CREATE_NAME);

	//����ģ��
	if (GameServiceManager.CreateInstance() == false)
	{
		AfxMessageBox(TEXT("������������ڻ��߼���ʧ�ܣ����÷���ʧ��"), MB_ICONERROR);
		return;
	}

	//���÷���
	CDlgServerWizard DlgServerWizard;
	DlgServerWizard.SetWizardParameter(GameServiceManager.GetInterface(), &m_ModuleInitParameter.GameServiceOption);

	//��������
	if (DlgServerWizard.CreateGameServer() == false) return;

	//����״̬
	UpdateParameterStatus(DlgServerWizard.m_ModuleInitParameter);

	return;
}


void CGameServiceDlg::OnBnClickedOptionMatch()
{
	if ((m_ModuleInitParameter.GameServiceOption.wServerType&GAME_GENRE_MATCH) == 0) return;
}

LRESULT CGameServiceDlg::OnMessageProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//��������
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine = (LPCTSTR)(lParam);

	//�����ʶ
	TCHAR szSrverID[32] = TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine, TEXT("/ServerID:"), szSrverID, CountArray(szSrverID)) == true)
	{
		//��ȡ����
		WORD wServerID = (WORD)(_tstol(szSrverID));

		//��������
		if (wServerID != 0)
		{
			//���ñ���
			m_bAutoControl = true;

			//��������
			StartServerService(wServerID);
		}
	}

	return 0L;
}



afx_msg LRESULT CGameServiceDlg::OnProcessCmdLine(WPARAM wParam, LPARAM lParam)
{
	//��������
	CWHCommandLine CommandLine;
	LPCTSTR pszCommandLine = (LPCTSTR)(lParam);

	//�����ʶ
	TCHAR szSrverID[32] = TEXT("");
	if (CommandLine.SearchCommandItem(pszCommandLine, TEXT("/ServerID:"), szSrverID, CountArray(szSrverID)) == true)
	{
		//��ȡ����
		WORD wServerID = (WORD)(_tstol(szSrverID));

		//��������
		if (wServerID != 0)
		{
			//���ñ���
			m_bAutoControl = true;

			//��������
			StartServerService(wServerID);
		}
	}

	return 0L;
}


// LogonServerDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "LogonServer.h"
#include "LogonServerDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


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


// CLogonServerDlg �Ի���



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


// CLogonServerDlg ��Ϣ�������

BOOL CLogonServerDlg::OnInitDialog()
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

	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
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

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CLogonServerDlg::OnPaint()
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
	case ServiceStatus_Stop:	//ֹͣ״̬
	{
		//���ñ���
		SetWindowText(TEXT("��¼������ -- [ ֹͣ ]"));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(FALSE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(TRUE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("����ֹͣ�ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Config:	//����״̬
	{
		//���ñ���
		SetWindowText(TEXT("��¼������ -- [ ��ʼ�� ]"));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���ڳ�ʼ�����...");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	case ServiceStatus_Service:	//����״̬
	{
		//���ñ���
		SetWindowText(TEXT("��¼������ -- [ ���� ]"));

		//���ð�ť
		GetDlgItem(IDC_STOP_SERVICE)->EnableWindow(TRUE);
		GetDlgItem(IDC_START_SERVER)->EnableWindow(FALSE);

		//��ʾ��Ϣ
		LPCTSTR pszDescribe = TEXT("���������ɹ�");
		CTraceService::TraceString(pszDescribe, TraceLevel_Normal);

		break;
	}
	}

	return;
}


void CLogonServerDlg::OnBnClickedStopService()
{
	// TODO: �ڴ���ӿؼ�֪ͨ����������
}


void CLogonServerDlg::OnBnClickedCancel()
{
	//�ر�ѯ��
	if (m_ServiceUnits.GetServiceStatus() != ServiceStatus_Stop)
	{
		LPCTSTR pszQuestion = TEXT("Э�����������������У���ȷʵҪ�رշ�������");
		if (AfxMessageBox(pszQuestion, MB_YESNO | MB_DEFBUTTON2 | MB_ICONQUESTION) != IDYES) return;
	}

	//ֹͣ����
	m_ServiceUnits.ConcludeService();
	__super::OnCancel();
}

void CLogonServerDlg::OnOK()
{

}

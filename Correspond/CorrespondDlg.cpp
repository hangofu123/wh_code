
// CorrespondDlg.cpp : ʵ���ļ�
//

#include "stdafx.h"
#include "Correspond.h"
#include "CorrespondDlg.h"
#include "afxdialogex.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CCorrespondDlg �Ի���



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


// CCorrespondDlg ��Ϣ�������

BOOL CCorrespondDlg::OnInitDialog()
{
	CDialogEx::OnInitDialog();

	// ���ô˶Ի����ͼ�ꡣ  ��Ӧ�ó��������ڲ��ǶԻ���ʱ����ܽ��Զ�
	//  ִ�д˲���
	SetIcon(m_hIcon, TRUE);			// ���ô�ͼ��
	SetIcon(m_hIcon, FALSE);		// ����Сͼ��

									//�������
	m_ServiceUnits.SetServiceUnitsSink(this);

	return TRUE;  // ���ǽ��������õ��ؼ������򷵻� TRUE
}

// �����Ի��������С����ť������Ҫ����Ĵ���
//  �����Ƹ�ͼ�ꡣ  ����ʹ���ĵ�/��ͼģ�͵� MFC Ӧ�ó���
//  �⽫�ɿ���Զ���ɡ�

void CCorrespondDlg::OnPaint()
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
HCURSOR CCorrespondDlg::OnQueryDragIcon()
{
	return static_cast<HCURSOR>(m_hIcon);
}



void CCorrespondDlg::OnBnClickedStartServer()
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

VOID CCorrespondDlg::OnServiceUnitsStatus(enServiceStatus ServiceStatus)
{
	//״̬����
	switch (ServiceStatus)
	{
	case ServiceStatus_Stop:	//ֹͣ״̬
	{
		//���ñ���
		SetWindowText(TEXT("Э�������� -- [ ֹͣ ]"));

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
		SetWindowText(TEXT("Э�������� -- [ ��ʼ�� ]"));

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
		SetWindowText(TEXT("Э�������� -- [ ���� ]"));

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


void CCorrespondDlg::OnBnClickedStopService()
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




void CCorrespondDlg::OnCancel()
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

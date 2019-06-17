#ifndef TCP_SOCKET_HEAD_FILE
#define TCP_SOCKET_HEAD_FILE

#pragma once

#include "WinSock2.h"
#include "WHSocketHead.h"

//////////////////////////////////////////////////////////////////////////

//TCP ��������
class CTCPSocket : public CWnd, public ITCPSocket
{
	//��������
protected:
	WORD							m_wSocketID;						//�����ʶ
	BYTE							m_cbSocketStatus;					//����״̬

	//�ں˱���
protected:
	SOCKET							m_hSocket;							//���Ӿ��
	ITCPSocketSink *				m_pITCPSocketSink;					//�ص��ӿ�

	//������Ϣ
protected:
	BYTE							m_ProxyServerType;					//��������
	tagProxyServer					m_ProxyServerInfo;					//������Ϣ

	//���ձ���
protected:
	WORD							m_wRecvSize;						//���ճ���
	BYTE							m_cbRecvBuf[SOCKET_TCP_BUFFER*10];	//���ջ���

	//�������
protected:
	bool							m_bNeedBuffer;						//����״̬
	DWORD							m_dwBufferData;						//��������
	DWORD							m_dwBufferSize;						//�����С
	LPBYTE							m_pcbDataBuffer;					//��������

	//��������
protected:
	BYTE							m_cbSendRound;						//�ֽ�ӳ��
	BYTE							m_cbRecvRound;						//�ֽ�ӳ��
	DWORD							m_dwSendXorKey;						//������Կ
	DWORD							m_dwRecvXorKey;						//������Կ

	//��������
protected:
	DWORD							m_dwSendTickCount;					//����ʱ��
	DWORD							m_dwRecvTickCount;					//����ʱ��
	DWORD							m_dwSendPacketCount;				//���ͼ���
	DWORD							m_dwRecvPacketCount;				//���ܼ���

	//��������
public:
	//���캯��
	CTCPSocket();
	//��������
	virtual ~CTCPSocket();

	//�����ӿ�
public:
	//�ͷŶ���
	virtual VOID Release() { delete this; }
	//�ӿڲ�ѯ
	virtual VOID * QueryInterface(REFGUID Guid, DWORD dwQueryVer);

	//��ʶ�ӿ�
public:
	//��ȡ��ʶ
	virtual WORD GetSocketID() { return m_wSocketID;}
	//���ñ�ʶ
	virtual VOID SetSocketID(WORD wSocketID) { m_wSocketID=wSocketID; }

	//���ýӿ�
public:
	//���ýӿ�
	virtual bool SetTCPSocketSink(IUnknownEx * pIUnknownEx);
	//��ȡ�ӿ�
	virtual VOID * GetTCPSocketSink(const IID & Guid, DWORD dwQueryVer);

	//��Ϣ�ӿ�
public:
	//��ȡ״̬
	virtual BYTE GetSocketStatus() { return m_cbSocketStatus; }
	//���ͼ��
	virtual DWORD GetLastSendTick() { return m_dwSendTickCount; }
	//���ռ��
	virtual DWORD GetLastRecvTick() { return m_dwRecvTickCount; }
	//������Ŀ
	virtual DWORD GetSendPacketCount() { return m_dwSendPacketCount; }
	//������Ŀ
	virtual DWORD GetRecvPacketCount() { return m_dwRecvPacketCount; }

	//����ӿ�
public:
	//�������
	virtual BYTE ProxyServerTesting();
	//���ô���
	virtual bool SetProxyServerInfo(BYTE ProxyServerType, const tagProxyServer & ProxyServer);

	//�����ӿ�
public:
	//�ر�����
	virtual VOID CloseSocket();
	//���Ӳ���
	virtual BYTE Connect(DWORD dwServerIP, WORD wPort);
	//���Ӳ���
	virtual BYTE Connect(LPCTSTR pszServerIP, WORD wPort);
	//���ͺ���
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID);
	//���ͺ���
	virtual WORD SendData(WORD wMainCmdID, WORD wSubCmdID, VOID * pData, WORD wDataSize);

	//��������
protected:
	//���Ӵ���
	DWORD ConnectProxyServer();
	//��ַ����
	DWORD TranslateAddr(LPCTSTR pszServerAddr);

	//��������
protected:
	//�ر�����
	VOID CloseSocket(BYTE cbShutReason);
	//��������
	VOID AmortizeBuffer(VOID * pData, WORD wDataSize);
	//��������
	DWORD SendDataBuffer(VOID * pBuffer, WORD wSendSize);

	//���ܺ���
protected:
	//��������
	WORD CrevasseBuffer(BYTE cbDataBuffer[], WORD wDataSize);
	//��������
	WORD EncryptBuffer(BYTE cbDataBuffer[], WORD wDataSize, WORD wBufferSize);

	//��������
private:
	//�ֽ�ӳ��
	inline WORD SeedRandMap(WORD wSeed);
	//����ӳ��
	inline BYTE MapSendByte(BYTE cbData);
	//����ӳ��
	inline BYTE MapRecvByte(BYTE cbData);

	//��Ϣӳ��
protected:
	//ʱ����Ϣ
	VOID OnTimer(UINT_PTR nIDEvent);
	//������Ϣ
	LRESULT	OnSocketNotify(WPARAM wParam, LPARAM lParam);

	//��Ϣ����
protected:
	//�����ȡ
	LRESULT OnSocketNotifyRead(WPARAM wParam, LPARAM lParam);
	//���緢��
	LRESULT OnSocketNotifyWrite(WPARAM wParam, LPARAM lParam);
	//����ر�
	LRESULT OnSocketNotifyClose(WPARAM wParam, LPARAM lParam);
	//��������
	LRESULT OnSocketNotifyConnect(WPARAM wParam, LPARAM lParam);

	DECLARE_MESSAGE_MAP()
};

//////////////////////////////////////////////////////////////////////////

#endif
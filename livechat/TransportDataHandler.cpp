/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: TransportDataHandler.cpp
 *   desc: 传输数据处理实现类
 */

#include "TransportDataHandler.h"
#include "TransportPacketHandler.h"
#include "ITask.h"
#include "ISocketHandler.h"
#include "IAutoLock.h"
#include "CommonDef.h"
#include <common/KLog.h>

CTransportDataHandler::CTransportDataHandler(void)
{
	m_listener = NULL;
	m_packetHandler = NULL;

	m_sendThread = NULL;
	m_recvThread = NULL;

	m_startLock = NULL;
	m_bStart = false;
	m_svrPort = 0;
	m_socketHandler = NULL;
	m_socketLock = NULL;

	m_sendTaskListLock = NULL;
}

CTransportDataHandler::~CTransportDataHandler(void)
{
	FileLog("LiveChatClient", "CTransportDataHandler::~CTransportDataHandler() begin, this:%p", this);
	Stop();
	FileLog("LiveChatClient", "CTransportDataHandler::~CTransportDataHandler() Stop() ok, this:%p", this);
	Uninit();
	FileLog("LiveChatClient", "CTransportDataHandler::~CTransportDataHandler() end, this:%p", this);
}

// 初始化
bool CTransportDataHandler::Init(ITransportDataHandlerListener* listener)
{
	FileLog("LiveChatClient", "CTransportDataHandler::Init() listener:%p", listener);

	bool result = false;
	m_listener = listener;

	// 创建待发送队列锁
	if (NULL == m_sendTaskListLock) {
		m_sendTaskListLock = IAutoLock::CreateAutoLock();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_sendTaskListLock:%p", m_sendTaskListLock);

	if (NULL == m_packetHandler) {
		m_packetHandler = ITransportPacketHandler::Create();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_packetHandler:%p", m_packetHandler);

	if (NULL == m_sendThread) {
		m_sendThread = IThreadHandler::CreateThreadHandler();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_sendThread:%p", m_sendThread);

	if (NULL == m_recvThread) {
		m_recvThread = IThreadHandler::CreateThreadHandler();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_recvThread:%p", m_recvThread);

	if (NULL == m_startLock) {
		m_startLock = IAutoLock::CreateAutoLock();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_startLock:%p", m_startLock);

	if (NULL == m_socketLock) {
		m_socketLock = IAutoLock::CreateAutoLock();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::Init() create m_socketLock:%p", m_socketLock);

	result = (NULL != m_sendTaskListLock
				&& NULL != m_packetHandler
				&& NULL != m_sendThread
				&& NULL != m_recvThread
				&& NULL != m_startLock
				&& NULL != m_socketLock);

	FileLog("LiveChatClient", "CTransportDataHandler::Init() result:%d", result);

	if (!result) {
		Uninit();
	}

	return result;
}

// 反初始化
void CTransportDataHandler::Uninit()
{
	FileLog("LiveChatClient", "CTransportDataHandler::Uninit()");

	delete m_sendThread;
	m_sendThread = NULL;

	delete m_recvThread;
	m_recvThread = NULL;

	ITransportPacketHandler::Release(m_packetHandler);
	m_packetHandler = NULL;

	IAutoLock::ReleaseAutoLock(m_startLock);
	m_startLock = NULL;

	IAutoLock::ReleaseAutoLock(m_socketLock);
	m_socketLock = NULL;

	m_listener = NULL;
}

// 开始连接
bool CTransportDataHandler::Start(const list<string>& ips, unsigned int port)
{
	FileLog("LiveChatClient", "CTransportDataHandler::Start() ips.size:%d, port:%d, this:%p", ips.size(), port, this);

	m_startLock->Lock();

	// 若已经启动则停止
	if (IsStart()) {
		StopProc();
	}

	// 赋值
	m_svrIPs = ips;
	m_svrPort = port;
	// 启动发送及连接线程
	if (NULL != m_recvThread) {
		m_bStart = m_recvThread->Start(RecvThread, this);
	}

	// 启动失败
	if (!m_bStart) {
		StopProc();
	}

	m_startLock->Unlock();

	FileLog("LiveChatClient", "CTransportDataHandler::Start() m_bStart:%d", m_bStart);

	return m_bStart;
}

// 停止连接
bool CTransportDataHandler::Stop()
{
	bool result = false;

	FileLog("LiveChatClient", "CTransportDataHandler::Stop() begin");
	m_startLock->Lock();
	result = StopProc();
	m_startLock->Unlock();
	FileLog("LiveChatClient", "CTransportDataHandler::Stop() end");

	return result;
}

// 停止连接处理函数
bool CTransportDataHandler::StopProc()
{
	FileLog("LiveChatClient", "CTransportDataHandler::StopProc() begin");
	long startTime = getCurrentTime();

	// 断开连接
	DisconnectProc();

	FileLog("LiveChatClient", "CTransportDataHandler::StopProc() m_recvThread->WaitAndStop()");
	// 停接收线程
	if (NULL != m_recvThread) {
		m_recvThread->WaitAndStop();
	}

	m_svrIPs.clear();
	m_svrPort = 0;

	FileLog("LiveChatClient", "CTransportDataHandler::StopProc() ReleaseSocketHandler(m_socketHandler)");
	// 释放socket
	m_socketLock->Lock();
	if (NULL != m_socketHandler) {
		ISocketHandler::ReleaseSocketHandler(m_socketHandler);
		m_socketHandler = NULL;
	}
	m_socketLock->Unlock();

	long endTime = getCurrentTime();
	long diffTime = DiffTime(startTime, endTime);

	FileLog("LiveChatClient", "CTransportDataHandler::StopProc() end, m_bStart:%d, diffTime:%ld", m_bStart, diffTime);

	return true;
}

// 是否开始
bool CTransportDataHandler::IsStart()
{
	FileLog("LiveChatClient", "CTransportDataHandler::IsStart() m_bStart:%d", m_bStart);
	return m_bStart;
}

// 发送task数据（把task放到发送列队）
bool CTransportDataHandler::SendTaskData(ITask* task)
{
	FileLog("LiveChatClient", "CTransportDataHandler::SendTaskData() task:%p", task);
	bool result = false;
	if (m_bStart) {
		m_sendTaskList.push_back(task);
		result = true;
	}
	FileLog("LiveChatClient", "CTransportDataHandler::SendTaskData() end, task:%p", task);
	return result;
}

// 发送线程
TH_RETURN_PARAM CTransportDataHandler::SendThread(void* arg)
{
	FileLog("LiveChatClient", "CTransportDataHandler::SendThread() arg:%p", arg);
	CTransportDataHandler* pThis = (CTransportDataHandler*) arg;
	pThis->SendThreadProc();
	FileLog("LiveChatClient", "CTransportDataHandler::SendThread() end, arg:%p", arg);
	return NULL;
}

void CTransportDataHandler::SendThreadProc(void)
{
	FileLog("LiveChatClient", "CTransportDataHandler::SendThreadProc() begin, m_bStart:%d", m_bStart);
	// 连接
	if (m_bStart) {
		FileLog("LiveChatClient", "CTransportDataHandler::SendThreadProc() SendProc()");
		// 处理task发送队列
		SendProc();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::SendThreadProc() end");
}

// 接收线程
TH_RETURN_PARAM CTransportDataHandler::RecvThread(void* arg)
{
	FileLog("LiveChatClient", "CTransportDataHandler::RecvThread() arg:%p", arg);
	CTransportDataHandler* pThis = (CTransportDataHandler*) arg;
	pThis->RecvThreadProc();
	FileLog("LiveChatClient", "CTransportDataHandler::RecvThread() end");
	return NULL;
}

void CTransportDataHandler::RecvThreadProc(void)
{
	// 连接服务器
	bool bConnect = ConnectProc();
	FileLog("LiveChatClient", "CTransportDataHandler::RecvThreadProc() ConnectProc(), m_listener:%p, bConnect:%d", m_listener, bConnect);
	m_listener->OnConnect(bConnect);
	FileLog("LiveChatClient", "CTransportDataHandler::RecvThreadProc() m_listener->OnConnect(bConnect) ok");

	if (bConnect && m_bStart) {
		// 启动发送线程
		if (NULL != m_sendThread) {
			m_sendThread->Start(SendThread, this);
		}

		// 接收处理
		RecvProc();

		// 断开连接
		DisconnectProc();

		// 等待发送线程停止
		if (NULL != m_sendThread) {
			m_sendThread->WaitAndStop();
		}

		// 断开回调
		DisconnectCallback();
	}
	FileLog("LiveChatClient", "CTransportDataHandler::RecvThreadProc() end");
}

void CTransportDataHandler::RecvProc(void)
{
	// 接收处理
	const unsigned int bufferSize = 1024;
	char buffer[bufferSize] = {0};
	unsigned int bufferLength = 0;
	unsigned int bufferOffset = 0;

	FileLog("LiveChatClient", "CTransportDataHandler::RecvProc()");

	while (m_socketHandler->Recv(buffer + bufferOffset, bufferSize - bufferOffset, bufferLength))
	{
		// 本次收到的数据长度 + 之前未解包的数据长度
		bufferLength += bufferOffset;

		FileLog("LiveChatClient", "CTransportDataHandler::RecvProc() bufferOffset:%d, bufferLength:%d, recvLength:%d", bufferOffset, bufferLength, bufferLength-bufferOffset);

		// 开始解包
		UNPACKET_RESULT_TYPE unpackResult;
		do {
			TransportProtocol* tp = NULL;
			unsigned int useLen = 0;

			unpackResult = m_packetHandler->Unpacket(buffer, bufferLength, &tp, useLen);
			if (unpackResult == UNPACKET_SUCCESS) {
				m_listener->OnRecv(tp);
			}

			if (unpackResult != UNPACKET_MOREDATA
				&& useLen > 0)
			{
				// 移除已解包的数据
				RemoveData(buffer, bufferLength, useLen);
				bufferLength -= useLen;
			}
			FileLog("LiveChatClient", "CTransportDataHandler::RecvProc() unpack:%d, bufferLength:%d", unpackResult, bufferLength);
		} while (unpackResult != UNPACKET_MOREDATA);
		// 记录未解包的数据长度
		bufferOffset = bufferLength;

		FileLog("LiveChatClient", "CTransportDataHandler::RecvProc() Process OK, bufferOffset:%d", bufferOffset);
	}

	FileLog("LiveChatClient", "CTransportDataHandler::RecvProc() end");
}

bool CTransportDataHandler::ConnectProc()
{
	bool result = false;
	FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() start, this:%p", this);
	FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() m_svrIPs.size:%d", m_svrIPs.size());
	FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() m_bStart:%d", m_bStart);

	list<string>::const_iterator iter;
	for (iter = m_svrIPs.begin();
		iter != m_svrIPs.end() && m_bStart;
		iter++)
	{
		FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() ip:%s start", (*iter).c_str());
		m_socketLock->Lock();
		m_socketHandler = ISocketHandler::CreateSocketHandler(ISocketHandler::TCP_SOCKET);
		m_socketLock->Unlock();
		if (NULL != m_socketHandler){
			result = m_socketHandler->Create();
			result = result && m_socketHandler->Connect((*iter), m_svrPort, 0) == SOCKET_RESULT_SUCCESS;
		}
		FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() ip:%s end", (*iter).c_str());

		if (result) {
			break;
		}
		else {
			FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() ReleaseSocketHandler start");
			m_socketLock->Lock();
			ISocketHandler::ReleaseSocketHandler(m_socketHandler);
			m_socketHandler = NULL;
			m_socketLock->Unlock();
			FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() ReleaseSocketHandler end");
		}
	}

	FileLog("LiveChatClient", "CTransportDataHandler::ConnectProc() end");
	return result;
}

void CTransportDataHandler::SendProc()
{
	FileLog("LiveChatClient", "CTransportDataHandler::SendProc()");

	unsigned int bufferSize = 1024 * 10;
	unsigned char* buffer = new unsigned char[bufferSize];
	while (m_bStart) {
		if (!m_sendTaskList.empty()) {
			ITask* task = m_sendTaskList.front();
			m_sendTaskList.pop_front();
			if (NULL != task && NULL != m_packetHandler) {
				// 获取task发送的data
				unsigned int dataLen = 0;
				if (m_packetHandler->Packet(task, buffer, bufferSize, dataLen)) {
					bool result = m_socketHandler->Send(buffer, dataLen);
					m_listener->OnSend(result, task);

					// 发送不成功，断开连接
					if (!result) {
						DisconnectProc();
						FileLog("LiveChatClient", "CTransportDataHandler::SendProc() send fail, disconnect now!");
					}
				}
			}
		}
		else {
			Sleep(100);
		}
	}

	FileLog("LiveChatClient", "CTransportDataHandler::SendProc() end");

	delete buffer;
}

void CTransportDataHandler::DisconnectProc()
{
	FileLog("LiveChatClient", "CTransportDataHandler::DisconnectProc()");

	m_bStart = false;

	m_socketLock->Lock();
	if (NULL != m_socketHandler) {
		m_socketHandler->Shutdown();
	}
	m_socketLock->Unlock();

	FileLog("LiveChatClient", "CTransportDataHandler::DisconnectProc() end");
}

void CTransportDataHandler::DisconnectCallback()
{
	FileLog("LiveChatClient", "CTransportDataHandler::DisconnectCallback()");
	m_listener->OnDisconnect(m_sendTaskList);
	m_sendTaskList.clear();
	FileLog("LiveChatClient", "CTransportDataHandler::DisconnectCallback() end");
}

// 删除pBuffer头nRemoveLength个字节
void CTransportDataHandler::RemoveData(char* pBuffer, int nBufferLength, int nRemoveLength)
{
	FileLog("LiveChatClient", "CTransportDataHandler::RemoveData() pBuffer:%p nBufferLength:%d, nRemoveLength:%d", pBuffer, nBufferLength, nRemoveLength);
	char* pSrc = pBuffer + nRemoveLength;
	char* pDes = pBuffer;
	int i = 0;
	for (i = 0; i < nBufferLength - nRemoveLength; i++) {
		pDes[i] = pSrc[i];
	}
}

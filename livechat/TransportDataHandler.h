/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: TransportDataHandler.h
 *   desc: 传输数据处理实现类
 */

#pragma once

#include "ITransportDataHandler.h"
#include "IThreadHandler.h"
#include "ITaskManager.h"	// 加载任务列表(TaskList)

using namespace std;

class IAutoLock;
class ISocketHandler;
class ITransportPacketHandler;
class CTransportDataHandler : public ITransportDataHandler
{
public:
	CTransportDataHandler(void);
	virtual ~CTransportDataHandler(void);

public:
	// 初始化
	virtual bool Init(ITransportDataHandlerListener* listener);
	// 开始连接
	virtual bool Start(const list<string>& ips, unsigned int port);
	// 停止连接（若连接断开会自动停止）
	virtual bool Stop();
	// 是否开始
	virtual bool IsStart();
	// 发送task数据（把task放到发送列队）
	virtual bool SendTaskData(ITask* task);

private:
	// 反初始化
	void Uninit();
	// 停止连接处理函数
	bool StopProc();

private:
	// 发送线程
	static TH_RETURN_PARAM SendThread(void* arg);
	void SendThreadProc(void);
	// 接收线程
	static TH_RETURN_PARAM RecvThread(void* arg);
	void RecvThreadProc(void);
	void RecvProc(void);

private:
	bool ConnectProc();
	void SendProc();
	void DisconnectProc();
	void DisconnectCallback();
	// 删除pBuffer头nRemoveLength个字节
	void RemoveData(char* pBuffer, int nBufferLength, int nRemoveLength);

private:
	ITransportDataHandlerListener*	m_listener;			// 监听器
	ITransportPacketHandler*		m_packetHandler;	// 传输包处理器

	list<string>		m_svrIPs;			// 服务器IP
	unsigned int		m_svrPort;			// 服务器端口
	ISocketHandler*		m_socketHandler;	// socket
	IAutoLock*			m_socketLock;		// socket锁

	IAutoLock*			m_startLock;		// 开始锁
	bool				m_bStart;			// 开始标志
	IThreadHandler*		m_sendThread;		// 发送线程
	IThreadHandler*		m_recvThread;		// 接收线程

	IAutoLock*			m_sendTaskListLock;	// 待发送任务队列锁
	TaskList			m_sendTaskList;		// 待发送任务队列
};

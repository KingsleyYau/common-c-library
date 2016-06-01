/*
 * author: Samson.Fan
 *   date: 2015-05-30
 *   file: HearbeatTask.cpp
 *   desc: 心跳Task实现类
 */

#include "HearbeatTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include <common/KLog.h>
#include <common/CheckMemoryLeak.h>

HearbeatTask::HearbeatTask(void)
{
	m_listener = NULL;
	m_seq = 0;
}

HearbeatTask::~HearbeatTask(void)
{
}

// 初始化
bool HearbeatTask::Init(ILiveChatClientListener* listener)
{
	bool result = false;
	if (NULL != listener)
	{
		m_listener = listener;
		result = true;
	}
	return result;
}
	
// 处理已接收数据
bool HearbeatTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	// 没有接收数据
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool HearbeatTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	const unsigned int body = 0;
	memcpy(data, &body, sizeof(body));
	dataLen = sizeof(body);

	// 发送一个int的0字节
	FileLog("LiveChatClient", "HearbeatTask::GetSendData() dataLen:%u", dataLen);
	return true;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE HearbeatTask::GetSendDataProtocolType()
{
	return NOHEAD_PROTOCOL;
}
	
// 获取命令号
int HearbeatTask::GetCmdCode()
{
	// 没有 cmd
	return TCMD_UNKNOW;
}

// 设置seq
void HearbeatTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int HearbeatTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool HearbeatTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void HearbeatTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	// 不用处理
}

// 未完成任务的断线通知
void HearbeatTask::OnDisconnect()
{
	// 不用回调
}

/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: RecvKickOfflineTask.cpp
 *   desc: 接收服务器踢下线Task实现类
 */

#include "RecvKickOfflineTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/KLog.h>

RecvKickOfflineTask::RecvKickOfflineTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_kickType = KOT_MAINTAIN;
}

RecvKickOfflineTask::~RecvKickOfflineTask(void)
{
}

// 初始化
bool RecvKickOfflineTask::Init(ILiveChatClientListener* listener)
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
bool RecvKickOfflineTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_INTEGER)
	{
		FileLog("LiveChatClient", "RecvKickOfflineTask::Handle() kickType:%d", root->intValue);
		m_kickType = (KICK_OFFLINE_TYPE)root->intValue;
		result = true;
	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnRecvKickOffline(m_kickType);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvKickOfflineTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvKickOfflineTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int RecvKickOfflineTask::GetCmdCode()
{
	return TCMD_RECVKICKOFFLINE;
}

// 设置seq
void RecvKickOfflineTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvKickOfflineTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvKickOfflineTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvKickOfflineTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvKickOfflineTask::OnDisconnect()
{
	// 不用回调
}

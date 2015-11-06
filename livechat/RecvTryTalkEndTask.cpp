/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: RecvTryTalkEndTask.cpp
 *   desc: 接收试聊结束Task实现类
 */

#include "RecvTryTalkEndTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json.h>

RecvTryTalkEndTask::RecvTryTalkEndTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

RecvTryTalkEndTask::~RecvTryTalkEndTask(void)
{
}

// 初始化
bool RecvTryTalkEndTask::Init(ILiveChatClientListener* listener)
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
bool RecvTryTalkEndTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_STRING)
	{
		m_userId = root->strValue;
		result = true;
	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnRecvTryTalkEnd(m_userId);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvTryTalkEndTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvTryTalkEndTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int RecvTryTalkEndTask::GetCmdCode()
{
	return TCMD_RECVTRYEND;
}

// 设置seq
void RecvTryTalkEndTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvTryTalkEndTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvTryTalkEndTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvTryTalkEndTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvTryTalkEndTask::OnDisconnect()
{
	// 不用回调
}

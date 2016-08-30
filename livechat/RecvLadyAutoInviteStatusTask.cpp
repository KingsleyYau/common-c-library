/*
 * author: Alex shum
 *   date: 2016-08-24
 *   file: RecvLadyAutoInviteStatusTask.cpp
 *   desc: 女士发送自动邀请消息状态通知Task实现类
 */

#include "RecvLadyAutoInviteStatusTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/CommonFunc.h>


RecvLadyAutoInviteStatusTask::RecvLadyAutoInviteStatusTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_isOpenStatus = false;

}

RecvLadyAutoInviteStatusTask::~RecvLadyAutoInviteStatusTask(void)
{
}

// 初始化
bool RecvLadyAutoInviteStatusTask::Init(ILiveChatClientListener* listener)
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
bool RecvLadyAutoInviteStatusTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull())
	{
		if(root->type == DT_FALSE || root->type == DT_TRUE)
		{

			m_errType = LCC_ERR_SUCCESS;
			m_errMsg  = "";
			result = true;
			m_isOpenStatus = root->boolValue;
		}
	}
	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnAutoInviteStatusUpdate(m_isOpenStatus);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvLadyAutoInviteStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvLadyAutoInviteStatusTask::GetSendDataProtocolType()
{
	return AMF_PROTOCOL;
}
	
// 获取命令号
int RecvLadyAutoInviteStatusTask::GetCmdCode()
{
	return TCMD_RECVAUTOINVITEMSG;
}

// 设置seq
void RecvLadyAutoInviteStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvLadyAutoInviteStatusTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvLadyAutoInviteStatusTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvLadyAutoInviteStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvLadyAutoInviteStatusTask::OnDisconnect()
{
	// 不用回调
}

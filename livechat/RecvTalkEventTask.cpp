/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: RecvTalkEventTask.cpp
 *   desc: 接收聊天事件Task实现类
 */

#include "RecvTalkEventTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>

// 请求参数定义
#define TARGETID_PARAM	"targetId"
#define TYPEID_PARAM	"typeId"

RecvTalkEventTask::RecvTalkEventTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
	m_eventType = TET_UNKNOW;
}

RecvTalkEventTask::~RecvTalkEventTask(void)
{
}

// 初始化
bool RecvTalkEventTask::Init(ILiveChatClientListener* listener)
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
bool RecvTalkEventTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_OBJECT)
	{
		amf_object_handle userIdObject = root->get_child(TARGETID_PARAM);
		result = !userIdObject.isnull() && userIdObject->type == DT_STRING;
		if (result) {
			m_userId = userIdObject->strValue;
		}

		amf_object_handle typeObject = root->get_child(TYPEID_PARAM);
		result = !typeObject.isnull() && typeObject->type == DT_INTEGER;
		if (result) {
			m_eventType = (TET_BEGIN <= typeObject->intValue && typeObject->intValue < TET_END) ? 
								(TALK_EVENT_TYPE)typeObject->intValue : TET_UNKNOW;
		}
	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnRecvTalkEvent(m_userId, m_eventType);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvTalkEventTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvTalkEventTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int RecvTalkEventTask::GetCmdCode()
{
	return TCMD_RECVTALKEVENT;	
}

// 设置seq
void RecvTalkEventTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvTalkEventTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvTalkEventTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvTalkEventTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvTalkEventTask::OnDisconnect()
{
	// 不用回调
}

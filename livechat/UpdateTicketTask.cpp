/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: UpdateTicketTask.cpp
 *   desc: 更新票根Task实现类
 */

#include "UpdateTicketTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json.h>

// 请求参数定义
#define FROMID_PARAM		"fromId"		// 发送用户Id
#define TICKET_PARAM		"ticket"		// 票根

UpdateTicketTask::UpdateTicketTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_fromId = "";
	m_ticket = 0;
}

UpdateTicketTask::~UpdateTicketTask(void)
{
}

// 初始化
bool UpdateTicketTask::Init(ILiveChatClientListener* listener)
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
bool UpdateTicketTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_OBJECT)
	{
		// fromId
		amf_object_handle fromIdObject = root->get_child(FROMID_PARAM);
		result = !fromIdObject.isnull() && fromIdObject->type == DT_STRING;
		if (result) {
			m_fromId = fromIdObject->strValue;
		}

		// ticket
		amf_object_handle ticketObject = root->get_child(TICKET_PARAM);
		result = !ticketObject.isnull() && ticketObject->type == DT_INTEGER;
		if (result) {
			m_ticket = ticketObject->intValue;
		}
	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnUpdateTicket(m_fromId, m_ticket);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UpdateTicketTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE UpdateTicketTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UpdateTicketTask::GetCmdCode()
{
	return TCMD_UPDATETICKET;
}

// 设置seq
void UpdateTicketTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UpdateTicketTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UpdateTicketTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void UpdateTicketTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void UpdateTicketTask::OnDisconnect()
{
	// 不用回调
}

/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: UpdateStatusTask.cpp
 *   desc: 更新用户在线状态Task实现类
 */

#include "UpdateStatusTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>

// 请求参数定义
#define USERID_PARAM		"userId"		// 用户Id
#define SERVER_PARAM		"server"		// 服务器名称
#define CLIENTTYPE_PARAM	"clientType"	// 客户端类型
#define STATUS_PARAM		"statusId"		// 在线状态

UpdateStatusTask::UpdateStatusTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
	m_server = "";
	m_clientType = CLIENT_ANDROID;
	m_status = USTATUS_OFFLINE_OR_HIDDEN;
}

UpdateStatusTask::~UpdateStatusTask(void)
{
}

// 初始化
bool UpdateStatusTask::Init(ILiveChatClientListener* listener)
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
bool UpdateStatusTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_OBJECT)
	{
		// userId
		amf_object_handle userIdObject = root->get_child(USERID_PARAM);
		result = !userIdObject.isnull() && userIdObject->type == DT_STRING;
		if (result) {
			m_userId = userIdObject->strValue;
		}

		// server
		amf_object_handle serverObject = root->get_child(SERVER_PARAM);
		result = !serverObject.isnull() && serverObject->type == DT_STRING;
		if (result) {
			m_server = serverObject->strValue;
		}

		// clientType
		amf_object_handle clientTypeObject = root->get_child(CLIENTTYPE_PARAM);
		result = !clientTypeObject.isnull() && clientTypeObject->type == DT_INTEGER;
		if (result) {
			m_clientType = (CLIENT_TYPE)clientTypeObject->intValue;
		}

		// status
		amf_object_handle statusObject = root->get_child(STATUS_PARAM);
		result = !statusObject.isnull() && statusObject->type == DT_INTEGER;
		if (result) {
			m_status = GetUserStatusType(statusObject->intValue);
		}
	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnUpdateStatus(m_userId, m_server, m_clientType, m_status);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UpdateStatusTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE UpdateStatusTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UpdateStatusTask::GetCmdCode()
{
	return TCMD_UPDATESTATUS;
}

// 设置seq
void UpdateStatusTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UpdateStatusTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UpdateStatusTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void UpdateStatusTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void UpdateStatusTask::OnDisconnect()
{
	// 不用回调
}

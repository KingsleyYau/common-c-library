/*
 * author: Samson.Fan
 *   date: 2016-03-23
 *   file: RecvAutoInviteTask.cpp
 *   desc: 接收自动邀请消息Task实现类
 */

#include "RecvAutoInviteTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>
#include <common/CommonFunc.h>

// 请求参数定义
#define WOMANID_PARAM		"womanId"		// 女士Id
#define MANID_PARAM			"manId"			// 男士Id
#define KEY_PARAM			"key"			// 验证码

RecvAutoInviteTask::RecvAutoInviteTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_womanId = "";
	m_manId = "";
	m_key = "";
}

RecvAutoInviteTask::~RecvAutoInviteTask(void)
{
}

// 初始化
bool RecvAutoInviteTask::Init(ILiveChatClientListener* listener)
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
bool RecvAutoInviteTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()
		&& root->type == DT_OBJECT)
	{
		// womanId
		amf_object_handle womanIdObject = root->get_child(WOMANID_PARAM);
		result = !womanIdObject.isnull() && womanIdObject->type == DT_STRING;
		if (result) {
			m_womanId = womanIdObject->strValue;
		}

		// manId
		amf_object_handle manIdObject = root->get_child(MANID_PARAM);
		result = !manIdObject.isnull() && manIdObject->type == DT_STRING;
		if (result) {
			m_manId = manIdObject->strValue;
		}

		// key
		amf_object_handle keyObject = root->get_child(KEY_PARAM);
		result = !keyObject.isnull() && keyObject->type == DT_DOUBLE;
		if (result) {
			char buffer[256] = {0};
			snprintf(buffer, sizeof(buffer), "%f", keyObject->doubleValue);

			m_key = buffer;
		}

	}

	// 通知listener
	if (NULL != m_listener 
		&& result) 
	{
		m_listener->OnRecvAutoInviteMsg(m_womanId, m_manId, m_key);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool RecvAutoInviteTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	// 本协议没有返回
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE RecvAutoInviteTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int RecvAutoInviteTask::GetCmdCode()
{
	return TCMD_RECVAUTOINVITEMSG;
}

// 设置seq
void RecvAutoInviteTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int RecvAutoInviteTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool RecvAutoInviteTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void RecvAutoInviteTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 未完成任务的断线通知
void RecvAutoInviteTask::OnDisconnect()
{
	// 不用回调
}

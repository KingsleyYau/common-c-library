/*
 * author: Samson.Fan
 *   date: 2015-04-01
 *   file: SendEmotionTask.cpp
 *   desc: 发送高级表情Task实现类
 */

#include "SendEmotionTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

// 请求参数定义
#define USERID_PARAM		"toId"		// 对方用户Id
#define EMOTIONID_PARAM		"eid"		// 高级表情Id
#define TICKET_PARAM		"ticket"	// 票根

SendEmotionTask::SendEmotionTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
	m_emotionId = "";
	m_ticket = 0;
}

SendEmotionTask::~SendEmotionTask(void)
{
}

// 初始化
bool SendEmotionTask::Init(ILiveChatClientListener* listener)
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
bool SendEmotionTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_TRUE) {
			m_errType = LCC_ERR_SUCCESS;
			m_errMsg = "";
			result = true;
		}

		// 解析失败协议
		if (!result) {
			int errType = 0;
			string errMsg = "";
			if (GetAMFProtocolError(root, errType, errMsg)) {
				m_errType = (LCC_ERR_TYPE)errType;
				m_errMsg = errMsg;
				result = true;
			}
		}
	}

	// 协议解析失败
	if (!result) {
		m_errType = LCC_ERR_PROTOCOLFAIL;
		m_errMsg = "";
	}

	// 通知listener
	if (NULL != m_listener) {
		m_listener->OnSendEmotion(m_userId, m_emotionId, m_ticket, m_errType, m_errMsg);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool SendEmotionTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	root[USERID_PARAM] = m_userId;
	root[EMOTIONID_PARAM] = m_emotionId;
	root[TICKET_PARAM] = m_ticket;
	Json::FastWriter writer;
	string json = writer.write(root);

	// 填入buffer
	if (json.length() < dataSize) {
		memcpy(data, json.c_str(), json.length());
		dataLen = json.length();

		result  = true;
	}
	return result;
}

// 获取待发送数据的类型
TASK_PROTOCOL_TYPE SendEmotionTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int SendEmotionTask::GetCmdCode()
{
	return TCMD_SENDEMOTION;	
}

// 设置seq
void SendEmotionTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int SendEmotionTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool SendEmotionTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void SendEmotionTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool SendEmotionTask::InitParam(const string& userId, const string& emotionId, int ticket)
{
	bool result = false;
	if (!userId.empty() 
		&& !emotionId.empty()) 
	{
		m_userId = userId;
		m_emotionId = emotionId;
		m_ticket = ticket;

		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void SendEmotionTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnSendEmotion(m_userId, m_emotionId, m_ticket, LCC_ERR_CONNECTFAIL, "");
	}
}

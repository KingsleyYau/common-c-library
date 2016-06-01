/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: GetVoiceCodeTask.cpp
 *   desc: 获取语音发送验证码Task实现类
 */

#include "GetVoiceCodeTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

GetVoiceCodeTask::GetVoiceCodeTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

GetVoiceCodeTask::~GetVoiceCodeTask(void)
{
}

// 初始化
bool GetVoiceCodeTask::Init(ILiveChatClientListener* listener)
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
bool GetVoiceCodeTask::Handle(const TransportProtocol* tp)
{
	bool result = false;

	string voiceCode("");
		
	AmfParser parser;
	amf_object_handle root = parser.Decode((char*)tp->data, tp->GetDataLength());
	if (!root.isnull()) {
		// 解析成功协议
		if (root->type == DT_STRING) {
			voiceCode = root->strValue;

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
		m_listener->OnGetVoiceCode(m_userId, m_ticket, m_errType, m_errMsg, voiceCode);
	}
	
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetVoiceCodeTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root(m_userId);
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
TASK_PROTOCOL_TYPE GetVoiceCodeTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int GetVoiceCodeTask::GetCmdCode()
{
	return TCMD_GETVOICECODE;
}

// 设置seq
void GetVoiceCodeTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetVoiceCodeTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetVoiceCodeTask::IsWaitToRespond()
{
	return true;
}

// 获取处理结果
void GetVoiceCodeTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool GetVoiceCodeTask::InitParam(const string& userId, int ticket)
{
	bool result = false;
	if (!userId.empty()) 
	{
		m_userId = userId;
		m_ticket = ticket;

		result = true;
	}
	
	return result;
}

// 未完成任务的断线通知
void GetVoiceCodeTask::OnDisconnect()
{
	if (NULL != m_listener) {
		m_listener->OnGetVoiceCode(m_userId, m_ticket, LCC_ERR_CONNECTFAIL, "", "");
	}
}

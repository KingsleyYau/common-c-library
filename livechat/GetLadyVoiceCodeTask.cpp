/*
 * author: Samson.Fan
 *   date: 2015-03-25
 *   file: GetLadyVoiceCodeTask.cpp
 *   desc: 获取女士语音发送验证码Task实现类
 */

#include "GetLadyVoiceCodeTask.h"
#include "ITaskManager.h"
#include "ILiveChatClient.h"
#include "AmfPublicParse.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

GetLadyVoiceCodeTask::GetLadyVoiceCodeTask(void)
{
	m_listener = NULL;

	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
}

GetLadyVoiceCodeTask::~GetLadyVoiceCodeTask(void)
{
}

// 初始化
bool GetLadyVoiceCodeTask::Init(ILiveChatClientListener* listener)
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
bool GetLadyVoiceCodeTask::Handle(const TransportProtocol* tp)
{
	// 无返回
	return false;
}

// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool GetLadyVoiceCodeTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
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
TASK_PROTOCOL_TYPE GetLadyVoiceCodeTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}

// 获取命令号
int GetLadyVoiceCodeTask::GetCmdCode()
{
	return TCMD_GETLADYVOICECODE;
}

// 设置seq
void GetLadyVoiceCodeTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int GetLadyVoiceCodeTask::GetSeq()
{
	return m_seq;
}

// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool GetLadyVoiceCodeTask::IsWaitToRespond()
{
	return false;
}

// 获取处理结果
void GetLadyVoiceCodeTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool GetLadyVoiceCodeTask::InitParam(const string& userId)
{
	bool result = false;
	if (!userId.empty())
	{
		m_userId = userId;
		result = true;
	}

	return result;
}

// 未完成任务的断线通知
void GetLadyVoiceCodeTask::OnDisconnect()
{
//	if (NULL != m_listener) {
//		m_listener->OnGetLadyVoiceCode(m_userId, LCC_ERR_CONNECTFAIL, "", "");
//	}
    // 没有回调
}

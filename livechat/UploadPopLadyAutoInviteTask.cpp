/*
 * author: Samson.Fan
 *   date: 2015-03-31
 *   file: UploadPopLadyAutoInviteTask.cpp
 *   desc: 上传弹出女士自动邀请消息Task实现类
 */

#include "UploadPopLadyAutoInviteTask.h"
#include <json/json/json.h>
#include <common/CheckMemoryLeak.h>

// 请求参数定义
#define TARGETID_PARAM	"targetId"	// 聊天对象ID
#define MSGID_PARAM		"msgId"		// 弹出的消息内容
#define KEY_PARAM		"key"		// 验证码

UploadPopLadyAutoInviteTask::UploadPopLadyAutoInviteTask(void)
{
	m_listener = NULL;
	
	m_seq = 0;
	m_errType = LCC_ERR_FAIL;
	m_errMsg = "";

	m_userId = "";
	m_msg = "";
	m_key = "";
}

UploadPopLadyAutoInviteTask::~UploadPopLadyAutoInviteTask(void)
{
}

// 初始化
bool UploadPopLadyAutoInviteTask::Init(ILiveChatClientListener* listener)
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
bool UploadPopLadyAutoInviteTask::Handle(const TransportProtocol* tp)
{
	bool result = false;
	// 本命令无返回
	return result;
}
	
// 获取待发送的数据，可先获取data长度，如：GetSendData(NULL, 0, dataLen);
bool UploadPopLadyAutoInviteTask::GetSendData(void* data, unsigned int dataSize, unsigned int& dataLen)
{
	bool result = false;
	
	// 构造json协议
	Json::Value root;
	root[TARGETID_PARAM] = m_userId;
	root[MSGID_PARAM] = m_msg;
	root[KEY_PARAM] = atof(m_key.c_str());
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
TASK_PROTOCOL_TYPE UploadPopLadyAutoInviteTask::GetSendDataProtocolType()
{
	return JSON_PROTOCOL;
}
	
// 获取命令号
int UploadPopLadyAutoInviteTask::GetCmdCode()
{
	return TCMD_UPLOADLADYAUTOINVITE;
}
	
// 设置seq
void UploadPopLadyAutoInviteTask::SetSeq(unsigned int seq)
{
	m_seq = seq;
}

// 获取seq
unsigned int UploadPopLadyAutoInviteTask::GetSeq()
{
	return m_seq;
}
	
// 是否需要等待回复。若false则发送后释放(delete掉)，否则发送后会被添加至待回复列表，收到回复后释放
bool UploadPopLadyAutoInviteTask::IsWaitToRespond()
{
	return false;
}
	
// 获取处理结果
void UploadPopLadyAutoInviteTask::GetHandleResult(LCC_ERR_TYPE& errType, string& errMsg)
{
	errType = m_errType;
	errMsg = m_errMsg;
}

// 初始化参数
bool UploadPopLadyAutoInviteTask::InitParam(const string& userId, const string& msg, const string& key)
{
	bool result = false;
	if (!userId.empty())
	{
		m_userId = userId;
		m_msg = msg;
		m_key = key;

		result = true;
	}
	return result;
}

// 未完成任务的断线通知
void UploadPopLadyAutoInviteTask::OnDisconnect()
{
	// 不用回调
}
